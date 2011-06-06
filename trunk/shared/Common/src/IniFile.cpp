// INI文件
// 仙剑修，2001.11.19

#include <windows.h>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "IniFile.h"
//#include "logfile.h"
#include "common.h"

CIniFile::CIniFile(const char * pFilename, const char * pSection)
{
	if(pFilename && strlen(pFilename) < INIWORDSIZE)
	{
		strcpy(m_bufFilename, pFilename);
	}
	else
	{
		m_bufFilename[0] = 0;
	//	LOGERROR("CIniFile::CIniFile() parameter error.");
	}

	if(pSection && strlen(pSection) < INIWORDSIZE)
	{
		strcpy(m_bufSection, pSection);
	}
	else
	{
		m_bufSection[0] = 0;
	//	LOGERROR("CIniFile::CIniFile() parameter error.");
	}
}

CIniFile::~CIniFile()
{
}

void	CIniFile::SetSection(const char * pSect)
{
	if(pSect && strlen(pSect) < INIWORDSIZE)
	{
		strcpy(m_bufSection, pSect);
	}
	else
	{
		m_bufSection[0] = 0;
	//	LOGERROR("CIniFile::SetSection()  parameter error.");
	}
}

bool	CIniFile::GetString(char * bufString, const char * pKey, int nBufSize)
{
	if(bufString)
		bufString[0] = 0;
	else
	{
//		LOGERROR("CIniFile::GetString() parameter error.");
		return false;
	}

	int	nSectLen = strlen(m_bufSection);
//	ASSERT(nSectLen < INIWORDSIZE);
	int	nKeyLen = strlen(pKey);
//	ASSERT(nKeyLen < INIWORDSIZE);
	FILE * pFile = fopen(m_bufFilename, "r");
	if(nSectLen && nKeyLen && pFile)
	{
		char buf[INIWORDSIZE+INIDATASIZE];
		do
		{
			fgets(buf, INIWORDSIZE+INIDATASIZE-1, pFile);
			buf[INIWORDSIZE+INIDATASIZE-1] = 0;
			if(buf[0] == '[' && buf[nSectLen+1] == ']' && strnicmp(buf+1, m_bufSection, nSectLen) == 0)
			{
				do
				{
					fgets(buf, INIWORDSIZE+INIDATASIZE-1, pFile);
					buf[INIWORDSIZE+INIDATASIZE-1] = 0;
					if(buf[0] && buf[0] != ';' 
							&& (buf[nKeyLen] == '=' || buf[nKeyLen] == ' ' || buf[nKeyLen] == '\t')
							&& strnicmp(buf, pKey, nKeyLen) == 0)
					{
						int		pos = nKeyLen;
						while(buf[pos] == ' ' || buf[pos] == '\t')		// 去掉前空
							pos++;
						if(buf[pos] == '=')		// 去掉'='
							pos++;
						while(buf[pos] == ' ' || buf[pos] == '\t')		// 去掉前空
							pos++;
						strncpy(bufString, buf + pos, nBufSize-1);
						bufString[nBufSize-1] = 0;
						if(bufString[0] && bufString[strlen(bufString)-1] == '\n')		// fgets 会多取一个 '\n' 字符
							bufString[strlen(bufString)-1] = 0;
						while(bufString[0] && (bufString[strlen(bufString)-1] == ' '
												|| bufString[strlen(bufString)-1] == '\t'))		// 去掉尾空
							bufString[strlen(bufString)-1] = 0;
						if(pFile)
							fclose(pFile);
						return true;
					}
				}while(!feof(pFile) && buf[0] != '[');
			}
		}while(!feof(pFile));
	}
	if(pFile)
		fclose(pFile);
	return false;
}

bool CIniFile::getString(std::string& str, const std::string& strKey)
{
	char buf[INIWORDSIZE];
	if (GetString(buf,strKey.c_str(),INIWORDSIZE))
	{
		str = buf;
		return true;
	}
	return false;	
}

int		CIniFile::GetInt(const char * pKey)
{
	char buf[INIWORDSIZE];		//??
	if(GetString(buf, pKey, INIWORDSIZE))
		return atoi(buf);
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////
std::string IniGetStr(const char *pszFileName, const char *pszTitle, const char *pszSubTitle)
{
	char pszStr[255]="";
	if (!pszFileName || !pszTitle || !pszSubTitle || !pszStr)
		return pszStr;

	FILE* fp = fopen(pszFileName, "r");
	if (!fp)
		return pszStr;

	char szSection[256]	= "";
	sprintf(szSection, "[%s]", pszTitle);

	bool bSucFound	=false;
	while(true)
	{
		char szLine[1024]	="";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;

		if (0 != stricmp(szSection, szLine))
			continue;

		// section found
		char szFormat[256] = "";
		sprintf(szFormat, "%s=%s\n", pszSubTitle, "%s");

		while (true)
		{
			char szLine[1024]	="";

			if (EOF == fscanf(fp, "%s\n", szLine))
				goto out;

			if (strrchr(szLine, '[') && strrchr(szLine, ']'))
				goto out;

			if (sscanf(szLine, szFormat, pszStr))
			{
				bSucFound = true;
				goto out;
			}
		}
	}

out:
	fclose(fp);

	return pszStr;
}

//////////////////////////////////////////////////////////////////////
int IniGetInt(const char *pszFileName, const char *pszTitle, const char *pszSubTitle,int nDefault)
{
	int iData=nDefault;
	if (!pszFileName || !pszTitle || !pszSubTitle)
		return iData;

	FILE* fp = fopen(pszFileName, "r");
	if (!fp)
		return iData;

	char szSection[256]	= "";
	sprintf(szSection, "[%s]", pszTitle);

	bool bSucFound	=false;
	while(true)
	{
		char szLine[1024]	="";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;

		if (0 != stricmp(szSection, szLine))
			continue;

		// section found
		char szFormat[256] = "";
		sprintf(szFormat, "%s=%s\n", pszSubTitle, "%d");

		while (true)
		{
			char szLine[1024]	="";
			if (EOF == fscanf(fp, "%s\n", szLine))
				goto out;

			if (strrchr(szLine, '[') && strrchr(szLine, ']'))
				goto out;

			if (sscanf(szLine, szFormat, &iData))
			{
				bSucFound = true;
				goto out;
			}
		}
	}

out:
	fclose(fp);

	return iData;
}
