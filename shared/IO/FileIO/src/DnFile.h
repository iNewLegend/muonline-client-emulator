#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <map>

class DnpInfo
{
public:
	bool open(const char* pszFile);
	void close();
	FILE* getFilePtr(const char* pszFile, unsigned long& usFileSize);
private:
	typedef struct 
	{
		unsigned long uSize;
		unsigned long uOffset;
	}FileIndexInfo;
private:
	FILE*	fpDnp;
	std::map<unsigned long, FileIndexInfo> mapIndex;
};

class CDnFile
{
public:
	CDnFile();
	virtual ~CDnFile();
public:
	void			BeforeUseDnFile();
	void			AfterUseDnFile();
	FILE*			GetFPtr(const char* pszFile, unsigned long& usFileSize);
	bool			CheckDisperseFile(const char* pszFile);
	bool			CheckDisperseFile(const unsigned long uFileID);
	void			AddDisperseFile(const char* pszFile);
	bool			OpenFile(const char* pszFile);
	void			CloseFile(const char* pszFile);
	static unsigned long GenerateID(const char* pszStr);	
private:
	void			Destroy();
	void			Create();
	void			ProcessDir(const char* pszDir);
private:
	std::map<unsigned long, DnpInfo*>		m_mapDnp;
	std::map<unsigned long, unsigned char>	m_mapDisperseFiles;
};