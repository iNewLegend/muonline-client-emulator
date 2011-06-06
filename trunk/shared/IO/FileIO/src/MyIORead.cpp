#include "MyIORead.h"
#include "DnFile.h"
//#include "mpq/mpq_libmpq.h"
#include "FileSystem.h"
//#include <windows.h>
//#include <windowsx.h>
//#include <mmsystem.h>
//#include <stdio.h>
//#include <math.h>
//#include <process.h>
//#include <signal.h>

//extern "C" {
//	static voidpf ZCALLBACK zopen(voidpf opaque, const std::string& strFilename, int mode)
//	{
//		if (filename == NULL)
//			return NULL;
//		FATFile* file = new FATFile();
//		if(!file->Open(filename))
//		{
//			delete file;
//			return NULL;
//		}
//		return file;
//	}
//
//	static uLong  ZCALLBACK zread(voidpf opaque, voidpf stream, void* buf, uLong size)
//	{
//		if(stream == NULL)
//			return 0;
//		return ((FATFile *)stream)->Read(buf, size);
//	}
//
//	static uLong  ZCALLBACK zwrite(voidpf opaque, voidpf stream, const void* buf, uLong size)
//	{
//		return 0;
//	}
//
//	static long   ZCALLBACK ztell(voidpf opaque, voidpf stream)
//	{
//		if(stream == NULL)
//			return 0;
//		return ((FATFile *)stream)->Tell();
//	}
//
//	static long   ZCALLBACK zseek(voidpf opaque, voidpf stream, uLong offset, int origin)
//	{
//		if(stream == NULL)
//			return 0;
//		((FATFile *)stream)->Seek(offset, (FATFile::FileSeekMode)origin);
//		return 0;
//	}
//
//	static int    ZCALLBACK zclose(voidpf opaque, voidpf stream)
//	{
//		if(stream == NULL)
//			return 0;
//		((FATFile *)stream)->Close();
//		delete (FATFile *)stream;
//		return 0;
//	}
//
//	static int    ZCALLBACK zerror(voidpf opaque, voidpf stream)
//	{
//		return stream == NULL;
//	}
//}
//
//zlib_filefunc_def fatiofuncdef =
//{
//	zopen,
//	zread,
//	zwrite,
//	ztell,
//	zseek,
//	zclose,
//	zerror,
//	NULL
//};

//////////////////////////////////////////////////////////////////////////
//
IOReadFile::IOReadFile():m_fp(NULL),m_size(0)
{
}

IOReadFile::~IOReadFile()
{
	Close();
}

bool IOReadFile::Open(const std::string& strFilename, const std::string& strArchive)
{
	Close();
	m_fp = fopen(strFilename.c_str(), "rb");
	if (m_fp)
	{
		fseek(m_fp, 0, SEEK_END);
		m_size = ftell(m_fp);
		fseek(m_fp, 0, SEEK_SET);
	}
	return m_fp != NULL;
}

void IOReadFile::Close()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
}

unsigned long IOReadFile::Read(void* buf, unsigned long n)
{
	if(m_fp)
	{
		return fread(buf, n, 1, m_fp);
	}
	return 0;
}

unsigned long IOReadFile::Seek(unsigned long offset, int origin)
{
	if(m_fp)
	{
		return fseek(m_fp, offset, origin);
	}
	return 0;
}

unsigned long IOReadFile::Move(unsigned long offset)
{
	return Seek(offset, SEEK_CUR);
}

unsigned long IOReadFile::Tell()
{
	if(m_fp)
	{
		return ftell(m_fp);
	}
	return 0;
}

unsigned long IOReadFile::GetSize()
{
	return m_size;
}

bool IOReadFile::Exists(const std::string& strFilename)
{
	FILE* fp = fopen(strFilename.c_str(), "rb");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	return false;
}
/*
//////////////////////////////////////////////////////////////////////////
//IOReadMPQ
IOReadMPQ::IOReadMPQ():
m_Buffer(0),
m_Pointer(0),
m_Size(0)
{
}

IOReadMPQ::~IOReadMPQ()
{
	Close();
}

bool IOReadMPQ::Open(const std::string& strFilename, const std::string& strArchive)
{
	Close();
	for(ArchiveSet::iterator i=GetOpenArchives().begin(); i!=GetOpenArchives().end(); ++i)
	{
		mpq_archive &mpq_a = **i;
		int fileno = libmpq_file_number(&mpq_a, strFilename.c_str());
		if(fileno == LIBMPQ_EFILE_NOT_FOUND)
		{
			continue;
		}
		// Found!
		m_Size = libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (m_Size<=1)
		{
			return false;
		}
		m_Buffer = new unsigned char[m_Size];
		libmpq_file_getdata(&mpq_a, fileno, m_Buffer);
		return true;
	}
	return false;
}

void IOReadMPQ::Close()
{
	if (m_Buffer)
	{
		delete m_Buffer;
		m_Buffer = NULL;
	}
}

unsigned long IOReadMPQ::Read(void* buf, unsigned long n)
{
	if (IsEof()) 
		return 0;
	size_t rpos = m_Pointer + n;
	if (rpos > m_Size)
	{
		n = m_Size - m_Pointer;
	}
	memcpy(buf, &(m_Buffer[m_Pointer]), n);
	m_Pointer = rpos;
	return n;
}

unsigned long IOReadMPQ::Seek(unsigned long offset, int origin)
{
	if (SEEK_SET==origin)
	{
		m_Pointer = offset;
	}
	else if (SEEK_CUR==origin)
	{
		m_Pointer += offset;
	}
	else if (SEEK_END==origin)
	{
		m_Pointer = m_Size + offset;
	}
	return m_Pointer;
}

unsigned long IOReadMPQ::Move(unsigned long offset)
{
	return Seek(offset, SEEK_CUR);
}

unsigned long IOReadMPQ::Tell()
{
	return m_Pointer;
}

unsigned long IOReadMPQ::GetSize()
{
	return m_Size;
}

bool IOReadMPQ::Exists(const std::string& strFilename)
{
	for(ArchiveSet::iterator i=GetOpenArchives().begin(); i!=GetOpenArchives().end();++i)
	{
		mpq_archive &mpq_a = **i;
		int fileno = libmpq_file_number(&mpq_a, strFilename.c_str());
		if (fileno != LIBMPQ_EFILE_NOT_FOUND) 
			return true;
	}
	return false;
}
*/
IOReadZip::IOReadZip():
hz(NULL),
m_nIndex(0),
totsize(0)
{
}

IOReadZip::~IOReadZip()
{
	Close();
}

bool IOReadZip::Open(const std::string& strFilename, const std::string& strArchive)
{
	std::string strZip;
	std::string strFile;
	if (Exists(strFilename, strZip,strFile))
	{
		return OpenFormZip(strZip,strFile);
	}
	return false;
}

bool IOReadZip::OpenFormZip(const std::string& strZip,const std::string& strFile)
{
	hz = OpenZip(strZip.c_str(), 0);
	SetUnzipBaseDir(hz,"\\");
	FindZipItem(hz,strFile.c_str(),true,&m_nIndex,&ze);
	return true;
}

void IOReadZip::Close()
{
	if(hz != NULL)
	{
		CloseZip(hz);
		hz = NULL;
	}
}

unsigned long IOReadZip::Read(void* buf, unsigned long n)
{
	if (UnzipItem(hz,m_nIndex, buf,n)==ZR_OK)
	{
		totsize = ze.unc_size;
	}
	else
	{
		totsize+=n;
	}
	return totsize;
}

unsigned long IOReadZip::Seek(unsigned long offset, int origin)
{
	if(hz != NULL)
	{
		if(origin != SEEK_SET)
			return 0;
		int size = offset - totsize;
		totsize = offset;
		if(size <= 0)
			return 0;
		BYTE buf[1024];
		while(size > 1024)
		{
			UnzipItem(hz,m_nIndex, buf, 1024);
			size -= 1024;
		}
		UnzipItem(hz,m_nIndex, buf, size);
		return offset;
	}
	return 0;
}

unsigned long IOReadZip::Move(unsigned long offset)
{
	return Seek(offset, SEEK_CUR);
}

unsigned long IOReadZip::Tell()
{
	return totsize;
}

unsigned long IOReadZip::GetSize()
{
	return ze.unc_size;
}

bool IOReadZip::ExistsInZip(const std::string& strZip, const std::string& strFile)
{
	if (IOReadFile::Exists(strZip))
	{
		HZIP hz = OpenZip(strZip.c_str(), 0);
		SetUnzipBaseDir(hz,"\\");
		int i;
		ZIPENTRY ze;
		ZRESULT ret = FindZipItem(hz,strFile.c_str(),true,&i,&ze);
		GetZipItem(hz,-1,&ze);
		int numitems=ze.index;
		for (int i=0; i<numitems; i++)
		{
			GetZipItem(hz,i,&ze);
		}
		CloseZip(hz);
		return ret==ZR_OK;
	}
	return false;
}

bool IOReadZip::Exists(const std::string& strFilename, std::string& strZip, std::string& strFile)
{
	strFile = GetFilename(strFilename);
	std::string strZipFilename = GetParentPath(strFilename);
	while (strZipFilename.length()>0)
	{
		std::string strZipName = GetFilename(strZipFilename);
		strZipFilename = GetParentPath(strZipFilename);
		strZip = strZipFilename+strZipName+".zip";
		if (ExistsInZip(strZip,strFile))
		{
			return true;
		}
		strFile.insert(0,strZipName+"\\");
	}
	return false;
}

bool IOReadZip::Exists(const std::string& strFilename)
{
	std::string strZip;
	std::string strFile;
	return Exists(strFilename, strZip,strFile);
}