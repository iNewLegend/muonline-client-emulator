#include "FileSystem.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
#include <signal.h>

#include <cctype>
#include <algorithm>

CDir::CDir()
{
}

CDir::~CDir()
{
	m_FileInfo.clear();
}

bool CDir::ReadDir(const std::wstring& wstrPath)
{
	m_FileInfo.clear();
	if (wstrPath.length()>0)
	{
		const std::wstring& wstrFindePath = wstrPath+L"\\*.*";
		WIN32_FIND_DATAW fd;
		HANDLE hFind = FindFirstFileW(wstrFindePath.c_str(), &fd);
		if(INVALID_HANDLE_VALUE==hFind)
		{
			FindClose(hFind);
			return false;
		}
		else
		{
			{
				FileInfo fileInfo;
				fileInfo.wstrFilename = L"..";
				fileInfo.uAttr |= FILE_ATTR_DIRECTORY;
				m_FileInfo.push_back(fileInfo);
			}
			do
			{
				std::wstring wstrName = fd.cFileName;
				if (L"."==wstrName || L".."==wstrName)
				{
					//FileInfo fileInfo;
					//fileInfo.wstrFilename = wstrName;
					//fileInfo.uAttr |= FILE_ATTR_DIRECTORY;
					//m_FileInfo.push_back(fileInfo);
				}
				else if (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
					FileInfo fileInfo;
					fileInfo.wstrFilename = wstrName;
					fileInfo.uAttr |= FILE_ATTR_DIRECTORY;
					m_FileInfo.push_back(fileInfo);
				}
				else
				{
					FileInfo fileInfo;
					fileInfo.wstrFilename = wstrName;
					m_FileInfo.push_back(fileInfo);
				}
			} while(FindNextFileW(hFind,&fd));
			FindClose(hFind);
		}
		
	}
	else
	{
		for(char i='A';i<='Z';i++)
		{
			wchar_t x[20]={i,':'};
			UINT Type=GetDriveTypeW(x);
			if(Type==DRIVE_FIXED||Type==DRIVE_REMOVABLE||Type==DRIVE_CDROM)
			{
				FileInfo fileInfo;
				fileInfo.wstrFilename = x;
				fileInfo.uAttr |= FILE_ATTR_DIRECTORY;
				m_FileInfo.push_back(fileInfo);
			}
		}
	}
	return true;
}

std::string GetFilename(const std::string& str)
{
	size_t uEnd = str.length()-1;
	for (int i=uEnd; i>= 0; i--)
	{
		if (str[i]==L'\\' || str[i]==L'/')
		{
			if (i==uEnd)
			{
				uEnd--;
			}
			else
			{
				std::string strFilename = str.substr(i+1,uEnd-i);
				std::transform(strFilename.begin(),strFilename.end(),strFilename.begin(),std::tolower);// toLower
				return strFilename;
			}
		}
		else if (0==i)
		{
			std::string strFilename = str.substr(0,uEnd+1);
			std::transform(strFilename.begin(),strFilename.end(),strFilename.begin(),std::tolower);// toLower
			return strFilename;
		}
	}
	return "";
}

std::wstring GetFilename(const std::wstring& wstr)
{
	size_t uEnd = wstr.length()-1;
	for (int i=wstr.length()-2; i>= 0; i--)
	{
		if (wstr[i]==L'\\' || wstr[i]==L'/')
		{
			if (i==uEnd)
			{
				uEnd--;
			}
			else
			{
				std::wstring wstrFilename = wstr.substr(i+1,uEnd-i);
				std::transform(wstrFilename.begin(),wstrFilename.end(),wstrFilename.begin(),std::tolower);// toLower
				return wstrFilename;
			}
		}
		else if (0==i)
		{
			std::wstring wstrFilename = wstr.substr(0,uEnd+1);
			std::transform(wstrFilename.begin(),wstrFilename.end(),wstrFilename.begin(),std::tolower);// toLower
			return wstrFilename;
		}
	}
	return L"";
}

std::string GetExtension(const std::string& str)
{
	std::string::size_type pos = str.find_last_of('.');
	if (std::string::npos!=pos)
	{
		std::string strExt = str.substr(pos);
		std::transform(strExt.begin(),strExt.end(),strExt.begin(),std::tolower);// toLower
		return strExt;
	}
	return "";
}

std::wstring GetExtension(const std::wstring& wstr)
{
	std::wstring::size_type pos = wstr.find_last_of(L'.');
	if (std::wstring::npos!=pos)
	{
		std::wstring wstrExt = wstr.substr(pos);
		std::transform(wstrExt.begin(),wstrExt.end(),wstrExt.begin(),std::tolower);// toLower
		return wstrExt;
	}
	return L"";
}

std::string ChangeExtension(const std::string& strFilename, const std::string& strExt)
{
	std::string::size_type pos = strFilename.find_last_of('.');
	if (strExt.length()>0)
	{
		if (std::string::npos!=pos)
		{
			std::string strNewFilename = strFilename.substr(0,pos)+strExt;
			return strNewFilename;
		}
		else
		{
			return strFilename+strExt;
		}
	}
	return strFilename.substr(0,pos);
}

std::wstring ChangeExtension(const std::wstring& wstrFilename, const std::wstring& wstrExt)
{
	std::wstring::size_type pos = wstrFilename.find_last_of(L'.');
	if (wstrExt.length()>0)
	{
		if (std::wstring::npos!=pos)
		{
			std::wstring wstrNewFilename = wstrFilename.substr(0,pos)+wstrExt;
			return wstrNewFilename;
		}
		else
		{
			return wstrFilename+wstrExt;
		}
	}
	return wstrFilename.substr(0,pos);
}

std::string GetParentPath(const std::string& str)
{
	for (int i=str.length()-2; i>= 0; i--)
	{
		if (str[i]=='\\' || str[i]=='/')
		{
			return str.substr(0,i+1);
		}
	}
	return "";
}

std::wstring GetChildPath(const std::wstring& wstrParent,const std::wstring& wstrChild)
{
	if (wstrParent.length()>0)
	{
		WCHAR wchar = wstrParent[wstrParent.size()-1];
		if (L'\\'==wchar || L'/'==wchar)
		{
			return wstrParent+wstrChild;//+L"\\";
		}
		else
		{
			return wstrParent+L"\\"+wstrChild;//+L"\\";
		}
	}
	return wstrChild;//+L"\\";
}

std::wstring GetParentPath(const std::wstring& wstr)
{
	for (int i=wstr.length()-2; i>= 0; i--)
	{
		if (wstr[i]==L'\\' || wstr[i]==L'/')
		{
			return wstr.substr(0,i+1);
		}
	}
	return L"";
}

std::wstring getCurrentDirectory()
{
	WCHAR wszBuffer[128];
	GetCurrentDirectoryW(128, wszBuffer);
	return std::wstring(wszBuffer)+L"\\";
}

#include "IORead.h"
bool fileCopy(const char* szSrcFilename, const char* szDestFilename)
{
	char* pBuf = NULL;
	size_t filesize = 0;
	{
		IOReadBase* pRead = IOReadBase::autoOpen(szSrcFilename);
		if (pRead)
		{
			filesize = pRead->GetSize();
			if (filesize>0)
			{
				pBuf = new char[filesize];
				if (pBuf)
				{
					pRead->Read(pBuf, filesize);
				}
			}
			IOReadBase::autoClose(pRead);
		}
	}
	if (pBuf)
	{
		// write
		FILE* f=fopen(szDestFilename,"wb");
		if(f)
		{
			fwrite(pBuf,filesize,1,f);
			fclose(f);
			delete[] pBuf;
			return true;
		}
		delete[] pBuf;
	}
	return false;
}

//#include   <direct.h>  
bool pathCopy(const const char* szSrcPath, const const char* szDestPath)
{
	/*int status = _wmkdir(szSrcPath.c_str());
	(!status) ? (printf("Directory created\n")) :(printf("Unable to create directory\n"));
	CDir dir;
	dir.ReadDir(wstrSrcPath);
	//if(IOReadBase::Exists())
	for (size_t i=0; i<dir.m_FileInfo.size(); i++)
	{
		const std::wstring& wstrFilename = dir.m_FileInfo[i].wstrFilename;
		if (dir.m_FileInfo[i].IsDirectory())
		{
			if (wstrFilename!=L"."&&wstrFilename!=L"..")
			{
				pathCopy(wstrSrcPath+wstrFilename,wstrDestPath+wstrFilename);
			}
		}
		else
		{
			fileCopy(wstrSrcPath+wstrFilename,wstrDestPath+wstrFilename);
		}
	}*/
	return true;
}

std::string getRealFilename(const char* szPath, const char* szFilename)
{
	if (szPath)
	{
		return IOReadBase::Exists(szFilename)?szFilename:std::string(szPath)+szFilename;
	}
	return szFilename;
}

std::string getSimpleFilename(const std::string& strPath, const std::string& strFilename)
{
	size_t pos = strFilename.find(strPath);
	if (0==pos)
	{
		return strFilename.substr(strPath.length());
	}
	return strFilename;
}
