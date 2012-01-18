#include "CSVFile.h"
#include <algorithm>

CCsvFile::CCsvFile()
{
	m_pRead = NULL;
	m_length = 0;
}

CCsvFile::CCsvFile(const char* szFilename)
{
	m_pRead = NULL;
	m_length = 0;
	open(szFilename);
}

CCsvFile::~CCsvFile()
{
	close();
}
#include "..\..\..\Common\Include\common.h"
bool CCsvFile::readLine(std::vector<std::string>& line)
{
	if (m_pRead)
	{
		line.clear();
		std::string strKey;
		
		char c;
		m_pRead->Read(&c,1);
		if (!m_pRead->IsEof())
		{
			while(!m_pRead->IsEof())
			{
				if ('\n' == c)
				{
					if(line.size()>0)
					{
						break;
					}
				}
				else if ('\r' != c)
				{
					strKey.push_back(c);
				}
				else if (',' == c)
				{
					line.push_back(strKey);
					strKey.clear();
				}
				m_pRead->Read(&c,1);
			}
			if (line.size()>0)
			{
				line.push_back(strKey);
				return true;
			}
		}
	}
	return false;
}

bool CCsvFile::open(const char* szFilename)
{
	close();
	m_pRead = IOReadBase::autoOpen(szFilename);
	if (m_pRead)
	{
		if (readLine(m_Key))
		{
			//seekNextLine();
			return true;
		}
		close();
	}
	return false;
}

void CCsvFile::close()
{
	m_LineBuffer.clear();
	if (m_pRead)
	{
		IOReadBase::autoClose(m_pRead);
	}
}

int CCsvFile::getKeyIndex(const char* szKey)const
{
	int nCount = m_Key.size();
	for (int i = 0; i < nCount; i++)
	{
		if (strcmp(m_Key[i].c_str(),szKey)==0)
		{
			return i;
		}
	}
	return -1;
}

const char* CCsvFile::getStr(unsigned long uKeyIndex, const char* szDefault)const
{
	if (m_LineBuffer.size()>uKeyIndex)
	{
		return m_LineBuffer[uKeyIndex].c_str();
	}
	return szDefault;
}

int	CCsvFile::getInt(unsigned long uKeyIndex, int nDefault)
{
	const char* szVal = getStr(uKeyIndex, NULL);
	if (szVal)
	{
		return atoi(szVal);
	}
	return nDefault;
}

const char* CCsvFile::getStr(const char* szKey, const char* szDefault)const
{
	const int nKeyIndex = getKeyIndex(szKey);
	return getStr(nKeyIndex, szDefault);
}

int	CCsvFile::getInt(const char* szKey, int nDefault)
{
	const int nKeyIndex = getKeyIndex(szKey);
	return getInt(nKeyIndex, nDefault);
}

bool CCsvFile::getBool(const char* szKey)
{
	const char* szVal = getStr(szKey, NULL);
	if (szVal)
	{
		return strcmp(szVal,"0")&&strcmp(szVal,"false")&&strcmp(szVal,"FALSE");
	}
	return false;
}

float CCsvFile::getFloat(const char* szKey, float fDefault)
{
	const char* szVal = getStr(szKey, NULL);
	if (szVal)
	{
		return (float)atof(szVal);
	}
	return fDefault;
}

unsigned long CCsvFile::getHex(const char* szKey, unsigned long uDefault)
{
	const char* szVal = getStr(szKey, NULL);
	if (szVal)
	{
		return strtol(szVal, NULL, 16);
	}
	return uDefault;
}

bool CCsvFile::seekNextLine()
{
	return readLine(m_LineBuffer);
}


bool CCsvFile::seek(unsigned long uKeyIndex, const char* szVal)
{
	//m_pRead->Seek(0,SEEK_SET);
	//seekNextLine();
	do 
	{
		const char* szRet = getStr(uKeyIndex, NULL);
		if (szRet)
		{
			if (strcmp(szRet,szVal)==0)
			{
				return true;
			}
		}
	} while (seekNextLine());
	return false;
}

bool CCsvFile::seek(unsigned long uKeyIndex, int nVal)
{
	do
	{
		if (nVal == getInt(uKeyIndex,-1))
		{
			return true;
		}
	} while (seekNextLine());
	return false;
}

bool CCsvFile::seek(const char* szVal)
{
	return seek(0,szVal);
}

bool CCsvFile::seek(int nVal)
{
	return seek(0,nVal);
}

bool CCsvFile::Seek(const char* szKey, const char* szVal)
{
	int nKeyIndex = getKeyIndex(szKey);
	return seek(nKeyIndex,szVal);
}

bool CCsvFile::Seek(const char* szKey, int nVal)
{
	int nKeyIndex = getKeyIndex(szKey);
	return seek(nKeyIndex,nVal);
}