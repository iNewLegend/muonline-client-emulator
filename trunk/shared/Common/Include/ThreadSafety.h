#pragma once
#include <windows.h>
#include "assert.h"

class CMyCriticalSection
{ 
public: 
	CMyCriticalSection(){InitializeCriticalSection(&m_cs);}; 
	CRITICAL_SECTION& getCriticalSection(){return m_cs;}
	~CMyCriticalSection(){DeleteCriticalSection(&m_cs);}; 
protected:
	CRITICAL_SECTION m_cs;
};

class CMySingleLock
{
public:
	CMySingleLock(CMyCriticalSection&  cs)
	{
		m_pCS = &cs.getCriticalSection();
		EnterCriticalSection(m_pCS);//进入临界区 
	}
	~CMySingleLock()
	{
		if (m_pCS)
		{
			unlock();
		}
	}
	void unlock()
	{
		assert(m_pCS);
		LeaveCriticalSection(m_pCS);//离开临界区
		m_pCS = NULL;
	}
protected:
	CRITICAL_SECTION* m_pCS;
};
#define		CRITICAL_SECTION_VARIABLE	m_xxxxxxCtrl;
#define		LOCK_DECLARATION	CMyCriticalSection m_xxxxxxCtrl;
#define		LOCK_THREAD			CMySingleLock xxxxxxLock(m_xxxxxxCtrl);
#define		UNLOCK_THREAD		xxxxxxLock.unlock();