#pragma once
#include <string>

class IOReadBase
{
public:
	IOReadBase() {}
	virtual ~IOReadBase() {}
	static IOReadBase* autoOpen(const std::string& strFilename, const std::string& strArchive="");
	static void autoClose(IOReadBase*& pRead);
	virtual bool Open(const std::string& strFilename, const std::string& strArchive="") = 0;
	virtual void Close() = 0;
	virtual unsigned long Read(void* buf, unsigned long n) = 0;
	virtual unsigned long Seek(unsigned long offset, int origin) = 0;
	virtual unsigned long Move(unsigned long offset) = 0;
	virtual unsigned long Tell() = 0;
	virtual unsigned long GetSize() = 0;
	virtual bool IsEof();
	static bool Exists(const std::string& strFilename);
};