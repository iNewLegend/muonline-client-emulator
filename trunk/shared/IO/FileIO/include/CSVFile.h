#pragma once
#include <vector>

class CCsvFile
{
public:
	CCsvFile();
	CCsvFile(const char* szFilename);
	~CCsvFile();

	bool		open(const char* szFilename);
	bool		open(char* buffer, int nLength);
	int			getKeyIndex(const char* szKey)const;
	const char*	getStr(unsigned long uKeyIndex, const char* szDefault="NULL")const;
	int			getInt(unsigned long uKeyIndex, int nDefault=0);
	const char*	getStr(const char* szKey, const char* szDefault="NULL")const;
	int			getInt(const char* szKey, int nDefault=0);
	bool		getBool(const char* szKey);
	float		getFloat(const char* szKey, float fDefault=0.0f);
	unsigned long getHex(const char* szKey, unsigned long uDefault=0x00000000);
	bool		seekNextLine();
	CCsvFile&		seek(unsigned long uKeyIndex, const char* szVal);
	CCsvFile&		seek(unsigned long uKeyIndex, int nVal);
	CCsvFile&		seek(const char* szVal);
	CCsvFile&		seek(int nVal);
	CCsvFile&		Seek(const char* szKey, const char* szVal);
	CCsvFile&		Seek(const char* szKey, int nVal);
protected:
	int					m_nLine;
	std::vector<char*>	m_date;
	int					m_nColumnCount;
	char*				m_buffer;

};
