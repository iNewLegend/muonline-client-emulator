#include "stdafx.h"
#include "MCrashDump.h"

// CrashExceptionDump �Լ��� ����Ϸ��� dbghelp.dll�� �ʿ��ϴ�.
// �ش� dll�� http://www.microsoft.com/whdc/ddk/debugging/ ���� ���� �� �ִ�.
#pragma comment(lib, "dbghelp.lib") 


DWORD CrashExceptionDump(PEXCEPTION_POINTERS ExceptionInfo, const char* szDumpFileName, bool bMLog)
{
	HANDLE hFile = CreateFile(
		szDumpFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = ExceptionInfo;
		eInfo.ClientPointers = FALSE;

		MINIDUMP_CALLBACK_INFORMATION cbMiniDump;
		//cbMiniDump.CallbackRoutine = miniDumpCallback;
		cbMiniDump.CallbackRoutine = NULL;
		cbMiniDump.CallbackParam = 0;

		if( MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpNormal,
			ExceptionInfo ? &eInfo : NULL,
			NULL,
			NULL) == false )//&cbMiniDump); )
		{
			
			mlog("Can't Create Dump - MiniDumpWriteDump()\n");
		}		
	}else
	{
		mlog("Can't Create Dump - INVALID_HANDLE_VALUE\n");
	}
	
	CloseHandle(hFile);

	if (bMLog)
	{
		MFilterException(ExceptionInfo);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
