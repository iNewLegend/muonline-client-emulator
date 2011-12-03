//
//	Misc Functions about File
//
//                                              written by lee jang ho
//
//////////////////////////////////////////////////////////////////////
#ifndef _FILEINFO_H
#define _FILEINFO_H

#include <windows.h>

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

// path �� extension�� ������ ������ �����̸� ���.
void GetPureFilename(char *pPureFilename,const char *pFilename);

// ���� ����ΰ�? ( ��Ʈ��ũ ��δ� �������� ���� )
BOOL IsFullPath(const char *pPath);

// ��� �б� ( ����� ���� int�� ��� ��ü ũ�⸦ ������ �ִ�. )
BOOL ReadHeader(HANDLE hFile, void *pHeader, int nHeaderSize);

// ������ �����ϴ°�?
bool IsExist(const char *filename);

#endif	// _FILEINFO_H
