#include "stdafx.h"
#include "MMatchDBMgr.h"
#include <windows.h>
#include <Mmsystem.h>
#include "MAsyncProxy.h"
#include "MMatchConfig.h"
#include "MCrashDump.h"
#ifndef _PUBLISH
	#include "MProcessController.h"
#endif

MAsyncProxy::MAsyncProxy()
{
	m_nThreadCount = 0;
	BYTE nInitVal = (BYTE)(INVALID_HANDLE_VALUE);
	FillMemory(m_ThreadPool, sizeof(HANDLE)*MAX_THREADPOOL_COUNT, nInitVal);
}

MAsyncProxy::~MAsyncProxy()
{
}

bool MAsyncProxy::Create(int nThreadCount)
{
	m_hEventShutdown = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventFetchJob = CreateEvent(NULL, FALSE, FALSE, NULL);

	InitializeCriticalSection(&m_csCrashDump);

	nThreadCount = __min(nThreadCount, MAX_THREADPOOL_COUNT);
	for (int i=0; i<nThreadCount; i++) {
		DWORD dwThreadId=0;
		HANDLE hThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThreadId);
		if (hThread == NULL)
			return false;
		m_ThreadPool[i] = hThread;
		m_nThreadCount++;
	}
	return true;
}

void MAsyncProxy::Destroy()
{
	for (int i=0; i<m_nThreadCount; i++) {
		SetEvent(GetEventShutdown());
		Sleep(100);
	}

	WaitForMultipleObjects(m_nThreadCount,  m_ThreadPool, TRUE, 2000);

	for (int i=0; i<MAX_THREADPOOL_COUNT; i++) {
		if (INVALID_HANDLE_VALUE != m_ThreadPool[i]) {
			TerminateThread(m_ThreadPool[i], 0);
			CloseHandle(m_ThreadPool[i]);
			m_ThreadPool[i] = INVALID_HANDLE_VALUE;
			m_nThreadCount--;			
		}
	}

	DeleteCriticalSection(&m_csCrashDump);

	CloseHandle(m_hEventFetchJob); m_hEventFetchJob = NULL;
	CloseHandle(m_hEventShutdown); m_hEventShutdown = NULL;
}

void MAsyncProxy::PostJob(MAsyncJob* pJob)
{
	m_WaitQueue.Lock();
		pJob->SetPostTime(timeGetTime());
		m_WaitQueue.AddUnsafe(pJob);	
	m_WaitQueue.Unlock();

	SetEvent(GetEventFetchJob());
}

DWORD WINAPI MAsyncProxy::WorkerThread(LPVOID pJobContext)
{
	MAsyncProxy* pProxy = (MAsyncProxy*)pJobContext;

	__try{
		pProxy->OnRun();
	} __except(pProxy->CrashDump(GetExceptionInformation())) 
	{
		// ������ �����ϵ��� �ϱ������̴�.
		#ifndef _PUBLISH
			char szFileName[_MAX_DIR];
			GetModuleFileName(NULL, szFileName, _MAX_DIR);
			HANDLE hProcess = MProcessController::OpenProcessHandleByFilePath(szFileName);
			TerminateProcess(hProcess, 0);
		#endif
	}

	ExitThread(0);
	return (0);
}

void MAsyncProxy::OnRun()
{
	MMatchDBMgr	DatabaseMgr;

	CString str = DatabaseMgr.BuildDSNString(MGetServerConfig()->GetDB_DNS(), 
		                                      MGetServerConfig()->GetDB_UserName(), 
											  MGetServerConfig()->GetDB_Password());
	if (!DatabaseMgr.Connect())
	{
		char szLog[32];
		sprintf(szLog, "DBCONNECT FAILED ThreadID=%d \n", GetCurrentThreadId());
#ifdef _DEBUG
		OutputDebugString(szLog);
#endif
		MessageBox(NULL, szLog, "MatchServer DB Error", MB_OK);
	}

	#define MASYNC_EVENTARRAY_SIZE	2
	HANDLE EventArray[MASYNC_EVENTARRAY_SIZE];

	ZeroMemory(EventArray, sizeof(HANDLE)*MASYNC_EVENTARRAY_SIZE);
	WORD wEventCount = 0;

	EventArray[wEventCount++] = GetEventShutdown();
	EventArray[wEventCount++] = GetEventFetchJob();

	bool bShutdown = false;
	while(!bShutdown) {
		#define TICK_ASYNCPROXY_LIVECHECK	1000
		DWORD dwResult = WaitForMultipleObjects(wEventCount, EventArray, 
												FALSE, TICK_ASYNCPROXY_LIVECHECK);
		if (WAIT_TIMEOUT == dwResult) {
			if (m_WaitQueue.GetCount() > 0) {
				SetEvent(GetEventFetchJob());
			}
			continue;
		}

		switch(dwResult) {
		case WAIT_OBJECT_0:		// Shutdown
			{
				bShutdown = true;
			}
			break;
		case WAIT_OBJECT_0 + 1:	// Fetch Job
			{
				m_WaitQueue.Lock();
					MAsyncJob* pJob = m_WaitQueue.GetJobUnsafe();
				m_WaitQueue.Unlock();

				if (pJob) {
					pJob->Run(&DatabaseMgr);
					pJob->SetFinishTime(timeGetTime());

					m_ResultQueue.Lock();
						m_ResultQueue.AddUnsafe(pJob);
					m_ResultQueue.Unlock();
				}

				if (m_WaitQueue.GetCount() > 0) {
					SetEvent(GetEventFetchJob());
				}
			}
			break;
		};	// switch
	};	// while
}

DWORD MAsyncProxy::CrashDump(PEXCEPTION_POINTERS ExceptionInfo)
{
	mlog("CrashDump Entered 1\n");
	EnterCriticalSection(&m_csCrashDump);
	mlog("CrashDump Entered 2\n");

	if (PathIsDirectory("Log") == FALSE)
		CreateDirectory("Log", NULL);

	time_t		tClock;
	struct tm*	ptmTime;

	time(&tClock);
	ptmTime = localtime(&tClock);

	char szFileName[_MAX_DIR];

	int nFooter = 1;
	while(TRUE) {
		sprintf(szFileName, "Log/MAsyncProxy_%02d-%02d-%02d-%d.dmp", 
			ptmTime->tm_year+1900, ptmTime->tm_mon+1, ptmTime->tm_mday, nFooter);

		if (PathFileExists(szFileName) == FALSE)
			break;

		nFooter++;
		if (nFooter > 100) 
		{
			LeaveCriticalSection(&m_csCrashDump);
			return false;
		}
	}

	DWORD ret = CrashExceptionDump(ExceptionInfo, szFileName, true);

	mlog("CrashDump Leaving\n");
	LeaveCriticalSection(&m_csCrashDump);
	mlog("CrashDump Leaved\n");

	return ret;
}
