//
//	Common Open(Save) Dialog Module
//
//                                              written by lee jang ho
//
//////////////////////////////////////////////////////////////////////
#ifndef _OPENDLG_H
#define _OPENDLG_H

#include <windows.h>

// Modal OpenDlg
//		pFileName	:	�̸��� ������ ��Ʈ�� ����
//						Allow Multiple Selection�� ��� ( bMultiSel=TRUE ) ���丮��� \0 �׸��� ���ϸ� ����Ʈ�� \0���� ���еǾ� �´�.
//		nLen		:	pFileName ������ ũ��
//		pTitle		:	Caption Title
//		pFilter		:	Filter
//		pDefExt		:	Default Extension
//		bOpen		:	OpenDlg(TRUE) or SaveDlg(FALSE)
//		bMultiSel	:	Allo Multiple Selection(TRUE) at OpenDlg
BOOL OpenDlg(HWND hwnd, char *pFileName, int nLen, char *pTitle, char *pFilter, char *pDefExt, BOOL bOpen=TRUE, BOOL bMultiSel=FALSE);

// pFileName���� �� �н��� Multi Selection���� �����
// GetDirFirst�� ���丮�� ���(NULL�� ���� ����)
// GetNextFile�� NULL�� ���ö����� ���ϸ��� ����.
/*
char *GetDirFirst(char *pFileNames);
char *GetNextFile(void);
int GetFileCount(char *pFileName);
*/

int GetFileNameCount(const char *pFileNames);
const char *GetFileName(const char *pFileNames, int idx);


#endif
