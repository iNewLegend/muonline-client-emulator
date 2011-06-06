#pragma once
#include "IORead.h"

class IOReadFile: public IOReadBase
{
public:
	IOReadFile();
	virtual ~IOReadFile();
	virtual bool Open(const std::string& strFilename, const std::string& strArchive="");
	virtual void Close();
	virtual unsigned long Read(void* buf, unsigned long n);
	virtual unsigned long Seek(unsigned long offset, int origin);
	virtual unsigned long Move(unsigned long offset);
	virtual unsigned long Tell();
	virtual unsigned long GetSize();
	static bool Exists(const std::string& strFilename);
private:
	FILE* m_fp;
	unsigned long m_size;
};
/*
class IOReadMPQ: public IOReadBase
{
public:
	IOReadMPQ();
	virtual ~IOReadMPQ();
	virtual bool Open(const std::string& strFilename, const std::string& strArchive="");
	virtual void Close();
	virtual unsigned long Read(void* buf, unsigned long n);
	virtual unsigned long Seek(unsigned long offset, int origin);
	virtual unsigned long Move(unsigned long offset);
	virtual unsigned long Tell();
	virtual unsigned long GetSize();
	static bool Exists(const std::string& strFilename);
private:
	unsigned char *m_Buffer;
	size_t m_Pointer, m_Size;
};*/
#include <windows.h>
#include "zip/unzip.h"
class IOReadZip: public IOReadBase
{
public:
	IOReadZip();
	virtual ~IOReadZip();
	virtual bool Open(const std::string& strFilename, const std::string& strArchive="");
	bool OpenFormZip(const std::string& strZip,const std::string& strFile);
	virtual void Close();
	virtual unsigned long Read(void* buf, unsigned long n);
	virtual unsigned long Seek(unsigned long offset, int origin);
	virtual unsigned long Move(unsigned long offset);
	virtual unsigned long Tell();
	virtual unsigned long GetSize();
	static bool ExistsInZip(const std::string& strZip, const std::string& strFile);
	static bool Exists(const std::string& strFilename, std::string& strZip, std::string& strFile);
	static bool Exists(const std::string& strFilename);
private:
	HZIP	hz;
	int		m_nIndex;
	ZIPENTRY ze;
	unsigned long	totsize;
};