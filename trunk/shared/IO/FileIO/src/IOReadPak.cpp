#include "IOReadPak.h"
#include "FileSystem.h"
#include "assert.h"

IOReadPak::IOReadPak():m_fp(NULL),m_uOffset(0),m_size(0)
{
}

IOReadPak::~IOReadPak()
{
	Close();
}

bool IOReadPak::Open(const std::string& strFilename, const std::string& strArchive)
{
	Close();
	std::string strPak;
	std::string strFile = GetFilename(strFilename);
	std::string strPakFilename = GetParentPath(strFilename);
	while (strPakFilename.length()>0)
	{
		std::string strPakName = GetFilename(strPakFilename);
		strPakFilename = GetParentPath(strPakFilename);
		strPak = strPakFilename+strPakName+".pak";
		if (openPak(strPak))
		{
			if (openFile(strFile))
			{
				return true;
			}
			Close();
		}
		strFile.insert(0,strPakName+"\\");
	}
	return false;
}

void IOReadPak::Close()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
	mapIndex.clear();
}

unsigned long IOReadPak::Read(void* buf, unsigned long n)
{
	if(m_fp)
	{
		return fread(buf, n, 1, m_fp);
	}
	return 0;
}

unsigned long IOReadPak::Seek(unsigned long offset, int origin)
{
	if(m_fp)
	{
		return fseek(m_fp, m_uOffset+offset, origin);
	}
	return 0;
}

unsigned long IOReadPak::Move(unsigned long offset)
{
	return Seek(offset, SEEK_CUR);
}

unsigned long IOReadPak::Tell()
{
	if(m_fp)
	{
		return ftell(m_fp)-m_uOffset;
	}
	return 0;
}

unsigned long IOReadPak::GetSize()
{
	return m_size;
}

bool IOReadPak::openPak(const std::string& strFilename)
{
	assert(m_fp==NULL);
	m_fp = fopen(strFilename.c_str(), "rb");
	if(!m_fp)
	{
		return false;
	}

	char szFileHeader[32]="";
	fread(szFileHeader, sizeof(char), 32, m_fp);
	if(strcmp(szFileHeader, "DawnPack.TqDigital") != 0)
	{
		fclose(m_fp);
		return false;
	}

	unsigned long uVersion = 0;
	fread(&uVersion, sizeof(unsigned long), 1, m_fp);
	if(uVersion != 1000)
	{
		fclose(m_fp);
		return false;
	}

	unsigned long uFileAmount = 0;
	fread(&uFileAmount, sizeof(unsigned long), 1, m_fp);

	for(unsigned long uCounter = 0; uCounter < uFileAmount; uCounter++)
	{
		unsigned long idSubFile;
		unsigned long uFileOffset;
		unsigned long uFileSize;
		fread(&idSubFile, sizeof(unsigned long), 1, m_fp);
		fread(&uFileSize, sizeof(unsigned long), 1, m_fp);
		fread(&uFileOffset, sizeof(unsigned long), 1, m_fp);

		FileIndexInfo& infoIndex = mapIndex[idSubFile];
		infoIndex.uOffset = uFileOffset;
		infoIndex.uSize = uFileSize;
	}
	return true;
}

inline unsigned long GeneratePakFileID(const char* str)
{
	if(!str)
		return 0;
	int i;
	unsigned int v;
	static unsigned m[70];
	strncpy((char *)m,str,256);
	for (i=0;i<256/4 && m[i];i++) ;
	m[i++]=0x9BE74448,m[i++]=0x66F42C48;
	v=0xF4FA8928;

	__asm {
		mov esi,0x37A8470E		;x0=0x37A8470E
			mov edi,0x7758B42B		;y0=0x7758B42B
			xor ecx,ecx
_loop:
		mov ebx,0x267B0B11		;w=0x267B0B11
			rol v,1
			lea eax,m
			xor ebx,v

			mov eax,[eax+ecx*4]
		mov edx,ebx
			xor esi,eax
			xor edi,eax

			add edx,edi
			or edx,0x2040801		;a=0x2040801
			and edx,0xBFEF7FDF		;c=0xBFEF7FDF

			mov eax,esi
			mul edx
			adc eax,edx
			mov edx,ebx
			adc eax,0

			add edx,esi
			or edx,0x804021			;b=0x804021
			and edx,0x7DFEFBFF		;d=0x7DFEFBFF

			mov esi,eax
			mov eax,edi
			mul edx

			add edx,edx
			adc eax,edx
			jnc _skip
			add eax,2
_skip:
		inc ecx;
		mov edi,eax
			cmp ecx,i
			jnz _loop
			xor esi,edi
			mov v,esi
	}
	//#ifdef _DEBUG
	//	char szTemp[512];
	//	sprintf(szTemp, "%s FileID: %u\n", str,v);
	//	::OutputDebugStringA(szTemp);	
	//#endif
	return v;
}

bool IOReadPak::openFile(const std::string& strFilename)
{
	unsigned long idFile = GeneratePakFileID(strFilename.c_str());
	std::map<unsigned long, FileIndexInfo>::iterator iterFile = mapIndex.find(idFile);
	if(iterFile != mapIndex.end())
	{
		fseek(m_fp, iterFile->second.uOffset, SEEK_SET);
		m_uOffset = iterFile->second.uOffset;
		m_size = iterFile->second.uSize;
		return true;
	}
	return false;
}

bool IOReadPak::Exists(const std::string& strFilename)
{
	IOReadPak pak;
	return pak.Open(strFilename);
}
