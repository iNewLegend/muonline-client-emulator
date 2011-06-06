#pragma once
#include "IORead.h"
#include <map>

class IOReadPak: public IOReadBase
{
public:
	IOReadPak();
	virtual ~IOReadPak();
	virtual bool Open(const std::string& strFilename, const std::string& strArchive="");
	virtual void Close();
	virtual unsigned long Read(void* buf, unsigned long n);
	virtual unsigned long Seek(unsigned long offset, int origin);
	virtual unsigned long Move(unsigned long offset);
	virtual unsigned long Tell();
	virtual unsigned long GetSize();
	static bool Exists(const std::string& strFilename);
private:
	typedef struct 
	{
		unsigned long uSize;
		unsigned long uOffset;
	}FileIndexInfo;
	bool openPak(const std::string& strFilename);
	bool openFile(const std::string& strFilename);
	std::map<unsigned long, FileIndexInfo> mapIndex;
	FILE* m_fp;
	unsigned long m_uOffset;
	unsigned long m_size;
};