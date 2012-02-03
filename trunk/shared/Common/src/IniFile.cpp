#include "IniFile.h"
#include <fstream>
using namespace std;

//一行最大字符数为260

#define MAX_COLS 260

#define E_OK                0x00L
#define E_OPEN_FILE_FAILED    0x01L

/*
功能
    读取INI文件
参数
    szFilename    in    读入的INI文件名称
返回值
    E_OK                调用成功
    E_OPEN_FILE_FAILED    打开文件错我
*/
int CIniFile::ReadIniFile(char* szFilename)
{
    ifstream fIniFile(szFilename);
    if(fIniFile == NULL)
    {
        return E_OPEN_FILE_FAILED;
    }
    char szLine[MAX_COLS] = {0};
    while (fIniFile.getline(szLine,MAX_COLS))
    {
        char* p = szLine;
        //是否为[]

        if( *p == '[')
        {
            RemoveComment( p );
            char* pEnd = strchr( p ,']');
            if( pEnd == NULL)
                continue;
            *pEnd = 0;
            IniSection is;
            is.m_isName = string( p + 1 );
            m_isSection.push_back(is);
            continue;
        }
        //是否为;

        Trim( p );
        if( *p == ';')
        {
            m_isSection[m_isSection.size() - 1].m_icComment.push_back(p + 1);
            continue;
        }
        //否则视为entry

        //p = szLine;

        char* pTemp = strchr( p,'=');
        if(pTemp == NULL)
        {
            continue;
        }
        *pTemp = 0;
        //创建一个Entry

        IniEntry ie;
        ie.m_strName = p ;
        ie.m_strValue = pTemp + 1;
        //将Entry加入到响应的Section

        m_isSection[m_isSection.size() - 1 ].m_ieEntry.push_back(ie);

        memset(szLine,0,MAX_COLS);
    }
    fIniFile.close();

    return E_OK;
}
/*
功能
    将CIniFile中的内容写入文件
参数
    szFilename    in    生成的INI文件名称
返回值
    E_OK        调用成功
    E_OPEN_FILE_FAILED    打开文件错误
*/
int CIniFile::WriteIniFile(char* szFilename)
{
    ofstream fIniFile(szFilename);
    if(fIniFile == NULL)
        return E_OPEN_FILE_FAILED;
    for (size_t i = 0; i < m_isSection.size();++i)
    {
        fIniFile.write("[",1);
        fIniFile.write(m_isSection[i].m_isName.c_str(),m_isSection[i].m_isName.length());
        fIniFile.write("]",1);
        fIniFile << endl;
        for (size_t j = 0; j < m_isSection[i].m_ieEntry.size();++ j)
        {
            fIniFile.write(m_isSection[i].m_ieEntry[j].m_strName.c_str(),m_isSection[i].m_ieEntry[j].m_strName.length());
            fIniFile.write("=",1);
            fIniFile.write(m_isSection[i].m_ieEntry[j].m_strValue.c_str(),m_isSection[i].m_ieEntry[j].m_strValue.length());
            fIniFile << endl;
        }
    }
    fIniFile.close();
    return E_OK;
}
/*
功能
    删除前后的空格（' ','/t','/r','/n'）
参数
    szString        in    传入的字符串
                    out 去除空格后的字符串
返回值
    E_OK    调用成功
*/
int CIniFile::Trim(char* &szString)
{
    char* p = szString;
    while (*p == ' ' || *p == '/t')
    {
        p ++;
    }
    szString = p;

    p = szString + strlen(szString) - 1;
    while ( *p == ' ' || *p == '/t' || *p == '/r' || *p == '/n')
    {
        -- p;
    }
    *( p + 1 ) = 0;
    

    return E_OK;
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
