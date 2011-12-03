#pragma once

/////////////////////////////////////////////////////////////////////////
// Purpose		: Blocking �۾��� ���� ������� ���� Async ������ ������.
// Last Update	: 2004-02-04 

#pragma warning(disable:4786)
#include <list>
#include <algorithm>
using namespace std;


enum MASYNC_RESULT {
	MASYNC_RESULT_SUCCEED,
	MASYNC_RESULT_FAILED,
	MASYNC_RESULT_TIMEOUT
};


class MAsyncJob {
protected:
	int				m_nJobID;	// Job Type ID


	unsigned long	m_nPostTime;
	unsigned long	m_nFinishTime;

	MASYNC_RESULT	m_nResult;

	MUID			m_uidOwner;

public:
	MAsyncJob(int nJobID, const MUID& uidOwner) {
		m_nJobID = nJobID;
		m_uidOwner = uidOwner;
		m_nPostTime = 0;
		m_nFinishTime = 0;
	}
	virtual ~MAsyncJob()	{}

	int GetJobID()							{ return m_nJobID; }
	const MUID& GetOwnerUID()				{ return m_uidOwner; }
	unsigned long GetPostTime()				{ return m_nPostTime; }
	void SetPostTime(unsigned long nTime)	{ m_nPostTime = nTime; }
	unsigned long GetFinishTime()			{ return m_nFinishTime; }
	void SetFinishTime(unsigned long nTime)	{ m_nFinishTime = nTime; }

	MASYNC_RESULT GetResult()				{ return m_nResult; }
	void SetResult(MASYNC_RESULT nResult)	{ m_nResult = nResult; }

	virtual void Run(void* pContext) = 0;
};

class MAsyncJobList : protected list<MAsyncJob*> {
protected:
	CRITICAL_SECTION	m_csLock;
public:
	MAsyncJobList()				{ InitializeCriticalSection(&m_csLock); }
	virtual ~MAsyncJobList()	{ DeleteCriticalSection(&m_csLock); }

	void Lock()		{ EnterCriticalSection(&m_csLock); }
	void Unlock()	{ LeaveCriticalSection(&m_csLock); }

	// Unsafe Methods /////////////////////////////////////////////
	// ~Unsafe() �뵵 �̿ܿ� ������
	MAsyncJobList::iterator GetBeginItorUnsafe()	{ return begin(); }
	MAsyncJobList::iterator GetEndItorUnsafe()	{ return end(); }

	void AddUnsafe(MAsyncJob* pJob) {
		push_back(pJob);
	}
	void RemoveUnsafe(MAsyncJob* pJob, MAsyncJobList::iterator* itorOut) {
		iterator i = find(begin(), end(), pJob);
		if (i != end()) {
			iterator itorTmp = erase(i);
			if (itorOut)
				*itorOut = itorTmp;
		}
	}
	MAsyncJob* GetJobUnsafe() {
		if (begin() == end()) return NULL;
		MAsyncJob* pReturn = *begin();
		pop_front();
		return pReturn;
	}
	int GetCount() { return (int)size(); }
};

#define MAX_THREADPOOL_COUNT 10

class MAsyncProxy {
protected:
	HANDLE				m_hEventShutdown;
	HANDLE				m_hEventFetchJob;

	int					m_nThreadCount;
	HANDLE				m_ThreadPool[MAX_THREADPOOL_COUNT];

	MAsyncJobList		m_WaitQueue;
	MAsyncJobList		m_ResultQueue;

	CRITICAL_SECTION	m_csCrashDump;
	
protected:
	HANDLE GetEventShutdown()	{ return m_hEventShutdown; }
	HANDLE GetEventFetchJob()	{ return m_hEventFetchJob; }

	static DWORD WINAPI WorkerThread(LPVOID pJobContext);
	void OnRun();

public:
	MAsyncProxy();
	virtual ~MAsyncProxy();
	bool Create(int nThreadCount);
	void Destroy();
	
	int GetWaitQueueCount()		{ return m_WaitQueue.GetCount(); }
	int GetResultQueueCount()	{ return m_ResultQueue.GetCount(); }

	void PostJob(MAsyncJob* pJob);
	MAsyncJob* GetJobResult()	{
		MAsyncJob* pJob = NULL;
		m_ResultQueue.Lock();
			pJob = m_ResultQueue.GetJobUnsafe();
		m_ResultQueue.Unlock();
		return pJob;
	}

	DWORD CrashDump( PEXCEPTION_POINTERS ExceptionInfo );
};
