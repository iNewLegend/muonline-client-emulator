#pragma once
#include <vector>
#include <string>

#define FILE_ATTR_READONLY             0x00000001  
#define FILE_ATTR_HIDDEN               0x00000002  
#define FILE_ATTR_SYSTEM               0x00000004  
#define FILE_ATTR_DIRECTORY            0x00000010  
#define FILE_ATTR_ARCHIVE              0x00000020  
#define FILE_ATTR_DEVICE               0x00000040  
#define FILE_ATTR_NORMAL               0x00000080  
#define FILE_ATTR_TEMPORARY            0x00000100  
#define FILE_ATTR_SPARSE_FILE          0x00000200  
#define FILE_ATTR_REPARSE_POINT        0x00000400  
#define FILE_ATTR_COMPRESSED           0x00000800  
#define FILE_ATTR_OFFLINE              0x00001000  
#define FILE_ATTR_NOT_CONTENT_INDEXED  0x00002000  
#define FILE_ATTR_ENCRYPTED            0x00004000  

struct FileInfo
{
	FileInfo():uFilesize(0),uClus(0),uAttr(0){}
	std::wstring	wstrFilename;
	unsigned long	uFilesize;
	unsigned long	uClus;
	unsigned char	uAttr;
	bool IsDirectory(){return FILE_ATTR_DIRECTORY==(uAttr&FILE_ATTR_DIRECTORY);}
};

class CDir
{
public:
	CDir();
	virtual ~CDir();
	bool ReadDir(const std::wstring& wstrPath);
	//FileInfo& GetFileInfo(){}
private:
public:
	std::vector<FileInfo>	m_FileInfo;
};

std::string		GetFilename(const std::string& str);
std::wstring	GetFilename(const std::wstring& wstr);
std::string		GetParentPath(const std::string& str);
std::wstring	GetParentPath(const std::wstring& wstr);
std::wstring	GetChildPath(const std::wstring& wstrParent,const std::wstring& wstrChild);
std::string		GetExtension(const std::string& str);
std::wstring	GetExtension(const std::wstring& wstr);
std::string		ChangeExtension(const std::string& strFilename, const std::string& strExt);
std::wstring	ChangeExtension(const std::wstring& wstrFilename, const std::wstring& wstrExt);
std::wstring	getCurrentDirectory();

bool fileCopy(const char* szSrcFilename, const char* szDestFilename);
bool pathCopy(const char* szSrcPath, const char* szDestPath);

std::string getRealFilename(const char* szPath, const char* szFilename);
std::string getSimpleFilename(const std::string& strPath, const std::string& strFilename);