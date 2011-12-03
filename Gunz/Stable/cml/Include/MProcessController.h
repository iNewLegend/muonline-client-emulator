#pragma once


#include <windows.h>

#define _MPROCESS_CONTROLLER

#ifdef _MPROCESS_CONTROLLER
///////////////////////////

#include <Tlhelp32.h>

class MProcessController {
public:
	static bool FindProcessByName(const char* pszProcessName, PROCESSENTRY32* pOutPE32); // Win95 Compatible
	static HANDLE OpenProcessHandleByFilePath(const char* pszFilePath);	// CloseHandle �ʿ�, Win2K �̻�
	static HANDLE OpenProcessHandleByName(const char* pszFilePath); // CloseHandle �ʿ�, Win2K �̻�
	static bool StartProcess(const char* pszProcessPath, const BOOL bInheritHandles = TRUE);
	static bool StopProcess(HANDLE hProcess);
};

/// ���� ���μ����� �޸𸮸� ��ȯ�Ѵ�.
int MGetCurrProcessMemory();

///////////////////////////
#endif
