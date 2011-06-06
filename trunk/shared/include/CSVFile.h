#pragma once
#include "IORead.h"
#include <vector>

class CCsvFile
{
public:
	CCsvFile();
	CCsvFile(const char* szFilename);
	~CCsvFile();

	bool		readLine(std::vector<std::string>& line);
	bool		open(const char* szFilename);
	void		close();
	int			getKeyIndex(const char* szKey)const;
	const char*	getStr(unsigned long uKeyIndex, const char* szDefault)const;
	int			getInt(unsigned long uKeyIndex, int nDefault);
	const char*	getStr(const char* szKey, const char* szDefault)const;
	int			getInt(const char* szKey, int nDefault=0);
	bool		getBool(const char* szKey);
	float		getFloat(const char* szKey, float fDefault=0.0f);
	unsigned long getHex(const char* szKey, unsigned long uDefault=0x00000000);
	bool		seekNextLine();
	bool		seek(unsigned long uKeyIndex, const char* szVal);
	bool		seek(unsigned long uKeyIndex, int nVal);
	bool		seek(const char* szVal);
	bool		seek(int nVal);
	bool		Seek(const char* szKey, const char* szVal);
	bool		Seek(const char* szKey, int nVal);
protected:
	IOReadBase* m_pRead;
	long	m_length;
	std::vector<std::string> m_LineBuffer;
	std::vector<std::string> m_Key;
};
