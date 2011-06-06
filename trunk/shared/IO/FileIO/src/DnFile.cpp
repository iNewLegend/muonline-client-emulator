// File Name: DnFile.cpp
// Create by: Huang Yuhang
// Create on: 2003/11/16 20:13

//#pragma warning(disable:4786)
#include "DnFile.h"
#include "Windows.h"
#include  <io.h>

CRITICAL_SECTION	g_DnFileCriticalSection;

CDnFile::CDnFile()
{
	this->Create();
	InitializeCriticalSection (&g_DnFileCriticalSection);
}

CDnFile::~CDnFile()
{
	this->Destroy();
	LeaveCriticalSection (&g_DnFileCriticalSection);
	DeleteCriticalSection (&g_DnFileCriticalSection);
}

void CDnFile::BeforeUseDnFile()
{
	EnterCriticalSection (&g_DnFileCriticalSection);
}

void CDnFile::AfterUseDnFile()
{
	LeaveCriticalSection (&g_DnFileCriticalSection);
}

FILE* CDnFile::GetFPtr(const char* pszFile, unsigned long& usFileSize)
{
	if(!pszFile)
		return false;
	char szFileCopy[512];
	strcpy(szFileCopy, pszFile);
	strlwr(szFileCopy);
	unsigned long uFileNameLength = strlen(szFileCopy);
	for(size_t i=0; i<uFileNameLength; ++i)
	{
		if(szFileCopy[i]=='/')
		{
			szFileCopy[i]='\\';
		}
	}
	unsigned long idFile = this->GenerateID(szFileCopy);
	if(!this->CheckDisperseFile(idFile))
	{
		unsigned long uFileNameLength = strlen(szFileCopy);
		for(size_t i = 0; i < uFileNameLength; ++i)
		{
			if(szFileCopy[i] == '\\')
			{
				szFileCopy[i] = '\0';
			}
		}
		unsigned long idPack = this->GenerateID(szFileCopy);
		std::map<unsigned long, DnpInfo*>::iterator iter = m_mapDnp.find(idPack);
		if(iter != m_mapDnp.end())
		{
			DnpInfo* pInfoDnp = iter->second;
			if(pInfoDnp)
			{
				FILE* pFile = pInfoDnp->getFilePtr(pszFile,usFileSize);
				if (pFile)
				{
					return pFile;
				}
			}
		}
	}
	return 	NULL;	
}

bool CDnFile::CheckDisperseFile(const unsigned long uFileID)
{
	std::map<unsigned long, unsigned char>::iterator iter = m_mapDisperseFiles.find(uFileID);
	if(iter == m_mapDisperseFiles.end())
		return false;
	return true;
}

bool CDnFile::CheckDisperseFile(const char* pszFile)
{
	if(!pszFile)
		return false;
	char szFileCopy[512];
	strcpy(szFileCopy, pszFile);
	strupr(szFileCopy);
	unsigned long idFile = this->GenerateID(pszFile);
	return CheckDisperseFile(idFile);
}

void CDnFile::AddDisperseFile(const char* pszFile)
{
	if(!pszFile)
		return;
	char szFileCopy[512];
	strcpy(szFileCopy, pszFile);
	strlwr(szFileCopy);
	unsigned long uFileNameLength = strlen(szFileCopy);
	for(size_t i = 0; i < uFileNameLength; ++i)
	{
		if(szFileCopy[i] == '/')
		{
			szFileCopy[i] = '\\';
		}
	}
	unsigned long idFile = this->GenerateID(szFileCopy);
	m_mapDisperseFiles[idFile] = 0;
}

void CDnFile::ProcessDir(const char* pszDir)
{
	_finddata_t filestruct;
	int p = 0;
	int fn = 0;
	char szSearch[255];
	if (strlen(pszDir) == 0)
	{
		strcpy(szSearch , "*.*");
	}
	else
	{
		strcpy(szSearch , pszDir);
		strcat(szSearch , "/*.*");
	}

	int hnd = _findfirst(szSearch , &filestruct);
	if(hnd == -1) 
	{
		return;
	}
	do
	{
		char szFullName[MAX_PATH] = "";
		if (strlen(pszDir) > 0)
		{
			sprintf(szFullName , "%s/%s" , pszDir , filestruct.name);		
		}
		else
		{
			strcpy(szFullName , filestruct.name);
		}

		if (!(filestruct.attrib & _A_SUBDIR)) 
		{
			// Process the file name
			char szFileCopy[512];
			strcpy(szFileCopy, szFullName);
			strlwr(szFileCopy);
			unsigned long uFileNameLength = strlen(szFileCopy);
			for(size_t i = 0; i < uFileNameLength; ++i)
			{
				if(szFileCopy[i] == '/')
				{
					szFileCopy[i] = '\\';
				}
			}
			DWORD id = this->GenerateID(szFileCopy) ;
			m_mapDisperseFiles[id]=0;
		}
		else // Directory
		{
			if (strcmp(filestruct.name , "..") != 0 && strcmp(filestruct.name, ".") != 0)
			{
				ProcessDir(szFullName);
			}
		}
	}while(!_findnext(hnd , &filestruct)); 
}

bool CDnFile::OpenFile(const char* pszFile)
{
	if(!pszFile)
		return false;
	// 未打包目录中的文件列表预读, 优化未打包文件搜寻
	const char* p = strchr (pszFile, '.') ;
	if (p)
	{
		char szDirName[MAX_PATH] = "";
		strncpy (szDirName, pszFile, p - pszFile) ;

		ProcessDir(szDirName) ;
	}	
	char szFileCopy[512];
	strcpy(szFileCopy, pszFile);
	strlwr(szFileCopy);

	if(strlen(pszFile) < 4)
		return false;
	int nFileNameLength = strlen(szFileCopy);
	if(szFileCopy[nFileNameLength-1] != 'p' || szFileCopy[nFileNameLength-2] != 'n' ||
		szFileCopy[nFileNameLength-3] != 'd' || szFileCopy[nFileNameLength-4] != '.')
		return false;
	szFileCopy[nFileNameLength-4] = '\0';

	unsigned long idFile = this->GenerateID(szFileCopy);
	std::map<unsigned long, DnpInfo*>::iterator iter = m_mapDnp.find(idFile);
	if(iter != m_mapDnp.end())
		return false;

	DnpInfo* pDnpInfo = new DnpInfo;
	char szTemp[100];
	sprintf(szTemp, "new 0x%x\n", pDnpInfo);
	::OutputDebugStringA(szTemp);
	if(!pDnpInfo)
		return false;
	
	if (pDnpInfo->open(pszFile)==false)
	{
		delete(pDnpInfo);
	}

	m_mapDnp[idFile] = pDnpInfo;
	return true;
}

void CDnFile::CloseFile(const char* pszFile)
{
	if(!pszFile)
		return;
	char szFileCopy[512];
	strcpy(szFileCopy, pszFile);
	strupr(szFileCopy);
	unsigned long idFile = this->GenerateID(pszFile);
	std::map<unsigned long, DnpInfo*>::iterator iter = m_mapDnp.find(idFile);
	if(iter != m_mapDnp.end())
		return;
	DnpInfo* pDnp = iter->second;
	if(pDnp)
	{
		pDnp->close();
		delete(pDnp);
	}
	m_mapDnp.erase(iter);
}

void CDnFile::Destroy()
{
	std::map<unsigned long, DnpInfo*>::iterator iter = m_mapDnp.begin();
	while(iter != m_mapDnp.end())
	{
		DnpInfo* pDnp = iter->second;
		if(pDnp)
		{
			pDnp->close();
			delete(pDnp);
			char szTemp[100];
			sprintf(szTemp, "delete 0x%x\n", pDnp);
			::OutputDebugStringA(szTemp);
		}
		iter ++;
	}
	m_mapDnp.clear();
	m_mapDisperseFiles.clear();
}

void CDnFile::Create()
{
	this->Destroy();
}

unsigned long CDnFile::GenerateID(const char* str)
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

bool DnpInfo::open(const char* pszFile)
{
	fpDnp = fopen(pszFile, "rb");
	if(!fpDnp)
	{
		return false;
	}

	char szFileHeader[32]="";
	fread(szFileHeader, sizeof(char), 32, fpDnp);
	if(strcmp(szFileHeader, "DawnPack.TqDigital") != 0)
	{
		fclose(fpDnp);
		return false;
	}

	unsigned long uVersion = 0;
	fread(&uVersion, sizeof(unsigned long), 1, fpDnp);
	if(uVersion != 1000)
	{
		fclose(fpDnp);
		return false;
	}

	unsigned long uFileAmount = 0;
	fread(&uFileAmount, sizeof(unsigned long), 1, fpDnp);

	for(unsigned long uCounter = 0; uCounter < uFileAmount; uCounter++)
	{
		unsigned long idSubFile;
		unsigned long uFileOffset;
		unsigned long uFileSize;
		fread(&idSubFile, sizeof(unsigned long), 1, fpDnp);
		fread(&uFileSize, sizeof(unsigned long), 1, fpDnp);
		fread(&uFileOffset, sizeof(unsigned long), 1, fpDnp);

		FileIndexInfo& infoIndex = mapIndex[idSubFile];
		infoIndex.uOffset = uFileOffset;
		infoIndex.uSize = uFileSize;
	}
}

void DnpInfo::close()
{
	fclose(fpDnp);
	mapIndex.clear();
}

FILE* DnpInfo::getFilePtr(const char* pszFile, unsigned long& usFileSize)
{
	unsigned long idFile = CDnFile::GenerateID(pszFile);
	std::map<unsigned long, FileIndexInfo>::iterator iterFile = mapIndex.find(idFile);
	if(iterFile != mapIndex.end())
	{
		fseek(fpDnp, iterFile->second.uOffset, SEEK_SET);
		usFileSize = iterFile->second.uSize;
		return fpDnp;
	}
	return NULL;
}