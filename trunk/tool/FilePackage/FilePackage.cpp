// FilePackage.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <tchar.h>
#include "string.h"
#include "io.h"

#include <vector>

#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <process.h>
#include <signal.h>

#include <cctype>
#include <algorithm>
#include <iostream>
#include "conio.h"
#include "direct.h"

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

void GetFileListFromDir(const char* pszDir, std::vector<std::string>& setFilelist)
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
		if (!(filestruct.attrib & _A_HIDDEN)) // ignore hidden
		{
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
				if (szFileCopy[uFileNameLength-1]!='e'||
					szFileCopy[uFileNameLength-2]!='x'||
					szFileCopy[uFileNameLength-3]!='e'||
					szFileCopy[uFileNameLength-4]!='.')
				{
					setFilelist.push_back(szFileCopy);
				}
			}
			else // Directory
			{
				if (strcmp(filestruct.name , "..") != 0 && strcmp(filestruct.name, ".") != 0)
				{
					GetFileListFromDir(szFullName,setFilelist);
				}
			}
		}
	}while(!_findnext(hnd , &filestruct)); 
}

#define FILE_INFO_SIZE sizeof(unsigned long)*3
int _tmain(int argc, _TCHAR* argv[])
{
	//
	//char   szModuleFile[MAX_PATH]   ;    
	//GetModuleFileNameA(NULL, szModuleFile, MAX_PATH);  

	// get file list from dir
	std::vector<std::string> setFilelist;
	GetFileListFromDir("",setFilelist);

	std::cout<<"File count: "<<setFilelist.size()<<std::endl;
	// get the save filename
	char szPakFilename[128];
	GetCurrentDirectoryA(128, szPakFilename);
	if (strlen(szPakFilename) != 0)
	{
		strcat(szPakFilename , ".pak");
	}
	std::cout<<"The pak filename: "<<szPakFilename<<std::endl;

	// open or create pak
	FILE* f=fopen(szPakFilename, "wb");
	if(!f)
	{
		return false;
	}

	// Head
	char szFileHeader[32]="DawnPack.TqDigital";
	fwrite(szFileHeader, sizeof(char), 32, f);

	// Version
	unsigned long uVersion = 1000;
	fwrite(&uVersion, sizeof(unsigned long), 1, f);

	// count
	unsigned long uFileCount = setFilelist.size();
	fwrite(&uFileCount, sizeof(unsigned long), 1, f);

	// get offset
	unsigned long uFileListOffset = ftell(f);
	const unsigned long uFileDataStart = ftell(f)+setFilelist.size()*FILE_INFO_SIZE;
	unsigned long uFileDataOffset = uFileDataStart;

	// vector
	int nCount = 0;
	for (std::vector<std::string>::iterator it=setFilelist.begin();it!=setFilelist.end();it++)
	{
		nCount++;
		std::cout<<"packing "<<nCount<<"/"<<uFileCount<<": "<<it->c_str()<<std::endl;
		// check the filelist is ok
		if (uFileListOffset+FILE_INFO_SIZE>uFileDataStart)
		{
			std::cout<<"error: Out of file list range. Address: "<<uFileListOffset<<std::endl;
			break;
		}
		//
		unsigned long uFileSize = 0;
		// push back the file data to "*.pak"
		{
			FILE* fFile = fopen(it->c_str(),"rb");
			if (!fFile)
			{
				std::cout<<"warning: This file can be read."<<std::endl;
				continue;
			}
			{
				fseek(fFile, 0, SEEK_END);
				uFileSize = ftell(fFile);
				fseek(fFile, 0, SEEK_SET);
			}
			if (uFileSize>0)
			{
				char* buffer = new char[uFileSize];
				fread(buffer,uFileSize,1,fFile);
				// write data to pak
				fseek(f,uFileDataOffset,SEEK_SET);
				fwrite(buffer,uFileSize,1,f);
				delete buffer;
			}
			fclose(fFile);
		}
		// write file info to FileList
		fseek(f, uFileListOffset, SEEK_SET);
		unsigned long id = GeneratePakFileID(it->c_str());
		{
			fwrite(&id,sizeof(unsigned long),1,f);
			fwrite(&uFileSize,sizeof(unsigned long),1,f);
			fwrite(&uFileDataOffset, sizeof(unsigned long), 1, f);
		}
		// offset
		uFileListOffset+=FILE_INFO_SIZE;
		uFileDataOffset+=uFileSize;
	}
	fclose(f);
	std::cout<<"Completed."<<std::endl;

	system("pause");   

	//getch();
	return 0;
}

