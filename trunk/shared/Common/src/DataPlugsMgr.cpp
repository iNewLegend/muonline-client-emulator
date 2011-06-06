#include "DataPlugsMgr.h"

typedef BOOL (WINAPI * PFN_DATA_Plug_CreateObject)(void ** pobj);

CDataPlugBase* CDataPlugsMgr::getPlugByExtension(const char* szExt)
{
	for (size_t i=0;i<m_setPlugObj.size();++i)
	{
		if (strcmp(m_setPlugObj[i].pObj->getFormat(),szExt)==0)
		{
			return m_setPlugObj[i].pObj;
		}
	}
	return NULL;
}

bool CDataPlugsMgr::loadPlugs(const char* szFilename)
{
	WIN32_FIND_DATAA wfd;
	std::string strFilename;
	std::string strDir;

	strDir = szFilename;
	for (int i=strDir.length()-2; i>= 0; i--)
	{
		if (strDir[i]=='\\' || strDir[i]=='/')
		{
			strDir = strDir.substr(0,i+1);
			break;
		}
	}

	HANDLE hf = FindFirstFileA(szFilename, &wfd);

	// If invalid, than return.
	if (INVALID_HANDLE_VALUE == hf)
	{
		return false;
	}

	// Add the plugs
	do 
	{
		strFilename = strDir + wfd.cFileName;
		loadPlug(strFilename.c_str());
	}while (FindNextFileA(hf, &wfd));

	// Find Close
	FindClose(hf);
	return true;
}

bool CDataPlugsMgr::loadPlug(const char* szFilename)
{
	bool brt = false;
	if (m_setPlugObj.size() > 255)
	{
		MessageBoxA(NULL,"插件过多", "message", MB_OK|MB_ICONINFORMATION);
		return false;
	}
	DATA_PLUG_ST stPs;
	ZeroMemory(&stPs, sizeof(stPs));
	stPs.hIns = LoadLibraryA(szFilename);
	if (stPs.hIns)
	{
		PFN_DATA_Plug_CreateObject pFunc = (PFN_DATA_Plug_CreateObject)GetProcAddress(stPs.hIns, "Data_Plug_CreateObject");
		if (pFunc)
		{
			if (pFunc((void **)&stPs.pObj))
			{
#ifdef _DEBUG
				if (stPs.pObj->isDebug())
#else
				if (!stPs.pObj->isDebug())
#endif
				{
					brt =true;
					m_setPlugObj.push_back(stPs);
				}
			}
		}
	}
	if (!brt)
	{
		if (stPs.pObj)
		{
			stPs.pObj->release();
		}
		if (stPs.hIns)
		{
			FreeLibrary(stPs.hIns);
		}
	}
	return brt;
}

std::string CDataPlugsMgr::getAllExtensions()
{
	std::string strExts;
	for (size_t i=0;i<m_setPlugObj.size();++i)
	{
		if (i>0)
		{
			strExts+="|";
		}
		strExts+=m_setPlugObj[i].pObj->getFormat();
	}
	return strExts;
}

/*
#include "winnt.h"

DWORD LoadPbDllFromMemory(LPVOID lpRawDll, LPVOID lpImageDll) 
{
SYSTEM_INFO sSysInfo; 
PIMAGE_DOS_HEADER dosHeader; 
PIMAGE_NT_HEADERS pNTHeader; 
PIMAGE_SECTION_HEADER section; 
PIMAGE_IMPORT_DESCRIPTOR pImportDesc; 
PIMAGE_IMPORT_BY_NAME pOrdinalName; 
PIMAGE_BASE_RELOCATION baseReloc; 
PDWORD lpLink; 
unsigned char Protection[4096]; 
HINSTANCE hDll; 
WORD i; 
DWORD ImagePages,fOldProtect,j,MaxLen,HdrLen,Addr1,Addr2,Pg,Pg1,Pg2; 
char * sDllName; 

if(NULL == lpRawDll) return 1 ; 

dosHeader = (PIMAGE_DOS_HEADER)lpRawDll; 

// Is this the MZ header? 
if ((TRUE == IsBadReadPtr(dosHeader,sizeof (IMAGE_DOS_HEADER))) ||
(IMAGE_DOS_SIGNATURE != dosHeader->e_magic)) 
return 2; 

// Get the PE header. 
pNTHeader = MakePtr(PIMAGE_NT_HEADERS,dosHeader,dosHeader->e_lfanew); 

// Is this a real PE image? 
if((TRUE == IsBadReadPtr(pNTHeader,sizeof ( IMAGE_NT_HEADERS))) || 
( IMAGE_NT_SIGNATURE != pNTHeader->Signature)) 
return 3 ; 

if(( pNTHeader->FileHeader.SizeOfOptionalHeader != 
sizeof(pNTHeader->OptionalHeader)) || 
(pNTHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)) 
return 4; 

if (pNTHeader->FileHeader.NumberOfSections < 1) return 5; 

section = IMAGE_FIRST_SECTION( pNTHeader ); 
int HeaderSize = sizeof(IMAGE_SECTION_HEADER); 

// 节头长度 
HdrLen = (DWORD)section - (DWORD)dosHeader + 
HeaderSize * pNTHeader->FileHeader.NumberOfSections; 

// 找出最大的节的长度,此节一般是代码所在的节(.text 节) 
MaxLen = HdrLen; 
int ii=0; 

for (i = 0;i<(DWORD)pNTHeader->FileHeader.NumberOfSections;i++)// find MaxLen 
{
if(MaxLen < section[i].VirtualAddress + section[i].SizeOfRawData) 
{
MaxLen = section[i].VirtualAddress + section[i].SizeOfRawData; 
}
if(strcmp((const char *)section[i].Name,".rsrc") == 0) ii=i; 
}

GetSystemInfo(&sSysInfo);
ImagePages = MaxLen / sSysInfo.dwPageSize; 
if (MaxLen % sSysInfo.dwPageSize) ImagePages++; 

// 分配所需的内存 
DWORD NeededMemory = ImagePages * sSysInfo.dwPageSize; 
lpImageDll = VirtualAlloc(NULL, NeededMemory, MEM_COMMIT, PAGE_EXECUTE_READWRITE); 

if (lpImageDll == NULL) return 6; // 分配内存失败 

MoveMemory( lpImageDll, lpRawDll, HdrLen ); // 复制节头 

DWORD OrgAddr = 0; 
DWORD NewAddr = 0; 
DWORD Size = 0; 

// 复制 .text 节数据 
for (i = 0;i<pNTHeader->FileHeader.NumberOfSections;i++) 
{
OrgAddr = (DWORD)lpImageDll + (DWORD)section[i].VirtualAddress; 
NewAddr = (DWORD)lpRawDll + (DWORD)section[i].PointerToRawData; 
Size = (DWORD)section[i].SizeOfRawData; 
MoveMemory((void *)OrgAddr, (void *)NewAddr, Size ); 
}

// 把指针指向新的 DLL 映像 
dosHeader = (PIMAGE_DOS_HEADER) lpImageDll; // Switch to new image 
pNTHeader = (PIMAGE_NT_HEADERS) ((DWORD)dosHeader + dosHeader->e_lfanew); 
section = (PIMAGE_SECTION_HEADER) ((DWORD)pNTHeader + sizeof(IMAGE_NT_HEADERS)); 
pImageBase = (PBYTE)dosHeader; 

if((ii!=0) && (IsNT()==TRUE)) 
{
section[ii].VirtualAddress = section[ii].VirtualAddress + (DWORD)lpRawDll; 
section[ii].PointerToRawData = section[ii].PointerToRawData + (DWORD)lpRawDll; 
}

DWORD importsStartRVA; 

// Look up where the imports section is (normally in the .idata section) 
// but not necessarily so. Therefore, grab the RVA from the data dir. 
importsStartRVA = GetImgDirEntryRVA(pNTHeader,IMAGE_DIRECTORY_ENTRY_IMPORT); 
if ( !importsStartRVA ) 
{
VirtualFree(dosHeader,0, MEM_RELEASE); 
return 7; 
}

pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader->
OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress; 

if(pImportDesc!= 0) 
pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) ((DWORD)pImportDesc + (DWORD)dosHeader); 
else 
{
VirtualFree(dosHeader,0, MEM_RELEASE); 
return 8; 
}

while (1) // 处理各入口表中的 DLL 
{
// 检查是否遇到了空的 IMAGE_IMPORT_DESCRIPTOR 
if ((pImportDesc->TimeDateStamp==0 ) && (pImportDesc->Name==0)) break; 

// 从磁盘载入必须的 Dll, 
// 注意,载入的 DLL 是合并的 DLL 的入口表中的 DLL, 
// 不是合并到 EXE 中的 DLL 
sDllName = (char *) (pImportDesc->Name + (DWORD)pImageBase); 
hDll = GetModuleHandle(sDllName); 

if (hDll == 0 ) hDll = LoadLibrary(sDllName); 

if (hDll == 0 ) 
{
MessageBoxA(NULL, "Can't find required Dll",
"Error in LoadPbDllFromMemory()",0); 
VirtualFree(dosHeader,0, MEM_RELEASE); 
return 9; 
}

DWORD *lpFuncNameRef = (DWORD *) (pImportDesc->OriginalFirstThunk +
(DWORD)dosHeader); 
DWORD *lpFuncAddr = (DWORD *) (pImportDesc->FirstThunk +
(DWORD)dosHeader); 

while( *lpFuncNameRef != 0) 
{
pOrdinalName = (PIMAGE_IMPORT_BY_NAME) (*lpFuncNameRef +
(DWORD)dosHeader); 
DWORD pIMAGE_ORDINAL_FLAG = 0x80000000; 

if (*lpFuncNameRef & pIMAGE_ORDINAL_FLAG) 
*lpFuncAddr = (DWORD) GetProcAddress(hDll,
(const char *)(*lpFuncNameRef & 0xFFFF)); 
else
*lpFuncAddr = (DWORD) GetProcAddress(hDll,
(const char *)pOrdinalName->Name); 

if (lpFuncAddr == 0) 
{
VirtualFree(dosHeader,0, MEM_RELEASE); 
return 10;// Can''t GetProcAddress 
}

lpFuncAddr++;
lpFuncNameRef++;
}
pImportDesc++;
}

DWORD TpOffset; 
baseReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress); 

if (baseReloc !=0) 
{
baseReloc = (PIMAGE_BASE_RELOCATION) ((DWORD)baseReloc + (DWORD)dosHeader); 
while(baseReloc->VirtualAddress != 0) 
{
PWORD lpTypeOffset = (PWORD) ((DWORD)baseReloc +
sizeof(IMAGE_BASE_RELOCATION)); 
while (lpTypeOffset < (PWORD)((DWORD)baseReloc +
(DWORD)baseReloc->SizeOfBlock)) 
{
TpOffset = *lpTypeOffset & 0xF000; 
if(TpOffset == 0x3000) 
{
lpLink = (PDWORD) ((DWORD)dosHeader + baseReloc->VirtualAddress + (*lpTypeOffset & 0xFFF)); 
*lpLink = (DWORD)dosHeader + (*lpLink) - pNTHeader->OptionalHeader.ImageBase; 
}
else
{
if (TpOffset != 0) 
{
VirtualFree(dosHeader,0, MEM_RELEASE); 
return 10; 
}
}
lpTypeOffset++;
}
baseReloc = (PIMAGE_BASE_RELOCATION)((DWORD)baseReloc + 
(DWORD)baseReloc->SizeOfBlock); 
}
}

// 取得原始的内存状态 
memset(Protection,0,4096);
for (i = 0;i<=pNTHeader->FileHeader.NumberOfSections;i++) 
{
if (i == pNTHeader->FileHeader.NumberOfSections) 
{
Addr1 = 0; 
Addr2 = HdrLen; 
j = 0x60000000; 
}
else
{
Addr1 = section[i].VirtualAddress; 
Addr2 = section[i].SizeOfRawData; 
j = section[i].Characteristics; 
}
Addr2 += Addr1 - 1; 

Pg1 = Addr1 / sSysInfo.dwPageSize; 
Pg2 = Addr2 / sSysInfo.dwPageSize; 
for(Pg = Pg1 ;Pg<=Pg2;Pg++) 
{
if (j & 0x20000000) Protection[Pg] |= 1; // Execute 
if (j & 0x40000000) Protection[Pg] |= 2; // Read 
if (j & 0x80000000) Protection[Pg] |= 4; // Write 
}
}

// 恢复原始的内存状态 
Addr1 = (DWORD)dosHeader; 
for (Pg = 0 ;Pg<= ImagePages;Pg++) 
{
switch(Protection[Pg])
{
case 2: 
fOldProtect = PAGE_READONLY; 
break;
case 3: 
fOldProtect = PAGE_EXECUTE_READ; 
break;
case 6: 
fOldProtect = PAGE_READWRITE; 
break;
default: 
// Ignore strange combinations
fOldProtect = PAGE_EXECUTE_READWRITE;  
break;
}

if (fOldProtect !=PAGE_EXECUTE_READWRITE) 
{
if (VirtualProtect((void *)Addr1, 
sSysInfo.dwPageSize, 
fOldProtect,
&fOldProtect) == 0) 
{
VirtualFree(dosHeader,0, MEM_RELEASE); 
return 11; 
}
}
Addr1 += sSysInfo.dwPageSize; 
}

EntryPoint = (LPENTRYPOINT) ((DWORD)pNTHeader->OptionalHeader.AddressOfEntryPoint +
(DWORD)dosHeader); 
LPVOID lpReserved = 0; 
EntryPoint((HINSTANCE)dosHeader, DLL_PROCESS_ATTACH, lpReserved); 
lpImageDll2=lpImageDll;
return 0; 
}*/