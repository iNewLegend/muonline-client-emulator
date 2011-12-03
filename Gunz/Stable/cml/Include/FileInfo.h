//
//	Misc Functions about File
//
//                                              written by lee jang ho
//
//////////////////////////////////////////////////////////////////////
#ifndef _FILEINFO_H
#define _FILEINFO_H

#include <windows.h>
#include <time.h>

// ������ �ֱ� ������Ʈ�� �ð� ���
BOOL GetLastUpdate(const char *pFileName, FILETIME *ft);

// ������ ������Ʈ �Ǿ��°�?
//		pFileName
//		ot			:	���� �ð� ( ���� �������� �� �ð� )
BOOL IsModifiedOutside(const char *pFileName, FILETIME ot);

// Extension ����
BOOL RemoveExtension(char *pRemoveExt, const char *pFileName);

// Extension ��ü
void ReplaceExtension(char *pTargetName, const char *pSourceName, char *pExt);

// ��� ��� ���
void GetRelativePath(char *pRelativePath, const char *pBasePath, const char *pPath);

// ���� ��� ���
void GetFullPath(char *pFullPath, const char *pBasePath, const char *pRelativePath);
//�� ���� ��� ���... current directory�� ��������...
void GetFullPath(char *pFullPath, const char *pRelativePath);

// path �� ����..
void GetPurePath(char *pPurePath,const char *pFilename);
// path �� extension�� ������ ������ �����̸� ���.
void GetPureFilename(char *pPureFilename,const char *pFilename);
// extension �� ����
void GetPureExtension(char *pPureExtension,const char *pFilename);

// ���� ����ΰ�? ( ��Ʈ��ũ ��δ� �������� ���� )
BOOL IsFullPath(const char *pPath);

// ��� �б� ( ����� ���� int�� ��� ��ü ũ�⸦ ������ �ִ�. )
BOOL ReadHeader(HANDLE hFile, void *pHeader, int nHeaderSize);

// File�� CheckSum�� ���Ѵ�. (��¥, ������, ������ ����)
DWORD GetFileCheckSum(char* pszFileName);

// ������ �����ϴ°�?
bool IsExist(const char *filename);

// �θ� ���丮 �̸��� ��´�.
void GetParentDirectory(char* pszFileName);

// �־��� ��α����� ���丮�� �����Ѵ�.
bool MakePath(const char* pszFileName);

// time_t �� FILETIME ���� ��ȯ Code from MSDN
void time_tToFILETIME(time_t t, LPFILETIME pft);

// ������ WriteTime �� �����Ѵ�
BOOL MSetFileTime(LPCTSTR lpszPath, FILETIME ft);

bool GetMyDocumentsPath(char* path);

bool CreatePath(char* path);


#ifdef WIN32
#pragma comment(lib, "Shlwapi.lib")
#endif

#endif	// _FILEINFO_H
