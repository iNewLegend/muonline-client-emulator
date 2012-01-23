#include "CSVFile.h"
#include <algorithm>
#include "IORead.h"

CCsvFile::CCsvFile()
	:m_nLine(0)
	,m_buffer(NULL)
{
}

CCsvFile::CCsvFile(const char* szFilename)
	:m_nLine(0)
	,m_buffer(NULL)
{
	open(szFilename);
}

CCsvFile::~CCsvFile()
{
	if (m_buffer)
	{
		delete[] m_buffer;
	}
	m_date.clear();
}

bool CCsvFile::open(const char* szFilename)
{
	m_nLine = 0;
	IOReadBase* f = IOReadBase::autoOpen(szFilename);
	if (f)
	{
		if (m_buffer)
		{
			delete[] m_buffer;
		}
		m_date.clear();
		int nLength = f->GetSize();
		m_buffer = new char[nLength+1];
		f->Read(m_buffer,nLength);
		m_buffer[nLength]=0;
		IOReadBase::autoClose(f);

		return open(m_buffer,nLength);
	}

	return false;
}

bool CCsvFile::open(char* buffer, int nLength)
{
	// ----
	m_nColumnCount = 0;
	for (int i=0; i<nLength-1; ++i)
	{
		if (buffer[i]==0x0D)
		{
			if (buffer[i+1] == 0x0A)
			{
				m_nColumnCount++;
				break;
			}
		}
		else if (buffer[i]==',')
		{
			m_nColumnCount++;
		}
	}
	if (m_nColumnCount==0)
	{
		return false;
	}
	// ----
	int line = 0;
	char* pEnd = buffer+nLength;

	bool bString = false;
	m_date.push_back(buffer);
	while (buffer!=pEnd)
	{
		switch (*buffer)
		{
		case 0x0D:
			if (buffer[1] == 0x0A)
			{
				*buffer = 0;
				buffer++;
				*buffer = 0;
				line++;
				m_date.resize(line*m_nColumnCount);
				m_date.push_back(buffer+1);
			}
			break;

		case '"':
			{
				bString=!bString;
			}
			break;

		case ',':
			if (!bString)
			{
				*buffer = 0;
				m_date.push_back(buffer+1);
			}
			break;
		}
		buffer++;
	}
	return true;
}

int CCsvFile::getKeyIndex(const char* szKey)const
{
	for (int i = 0; i < m_nColumnCount; i++)
	{
		if (strcmp(m_date[i],szKey)==0)
		{
			return i;
		}
	}
	return -1;
}

const char* CCsvFile::getStr(unsigned long uKeyIndex, const char* szDefault)const
{
	int id = m_nLine*m_nColumnCount+uKeyIndex;
	if (id<m_date.size())
	{
		return m_date[id];
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
	m_nLine++;
	return m_nLine*m_nColumnCount+m_nColumnCount<=m_date.size();
}


CCsvFile& CCsvFile::seek(unsigned long uKeyIndex, const char* szVal)
{
	if (uKeyIndex==0)
	{
		m_nLine=0;
	}
	do 
	{
		const char* szRet = getStr(uKeyIndex, NULL);
		if (szRet)
		{
			if (strcmp(szRet,szVal)==0)
			{
				return *this;
			}
		}
	} while (seekNextLine());
	return *this;
}

CCsvFile& CCsvFile::seek(unsigned long uKeyIndex, int nVal)
{
	if (uKeyIndex==0)
	{
		m_nLine=0;
	}
	do
	{
		if (nVal == getInt(uKeyIndex,-1))
		{
			return *this;
		}
	} while (seekNextLine());
	return *this;
}

CCsvFile& CCsvFile::Seek(const char* szKey, const char* szVal)
{
	int nKeyIndex = getKeyIndex(szKey);
	return seek(nKeyIndex,szVal);
}

CCsvFile& CCsvFile::Seek(const char* szKey, int nVal)
{
	int nKeyIndex = getKeyIndex(szKey);
	return seek(nKeyIndex,nVal);
}

int CCsvFile::operator[](unsigned long uKeyIndex)
{
	const char* szVal = getStr(uKeyIndex, NULL);
	if (szVal)
	{
		return atoi(szVal);
	}
	return 0;
}

int CCsvFile::operator[](const char* szKey)
{
	unsigned long uKeyIndex = getKeyIndex(szKey);
	const char* szVal = getStr(uKeyIndex, NULL);
	if (szVal)
	{
		return atoi(szVal);
	}
	return 0;
}