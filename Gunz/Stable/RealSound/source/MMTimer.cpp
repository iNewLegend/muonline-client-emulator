/*
	MMTimer.cpp
	-----------
	
	"Multimedia Timer" handling class implementation file
	
	Programming by Chojoongpil
	MAIET entertainment software
*/
#include "stdafx.h"
#include "MMTimer.h"
#include "crtdbg.h"

////////////////////////////////////////
// Constructor & Destructor

MMTimer::MMTimer()
{	
    m_nIDTimer = NULL;
}

MMTimer::~MMTimer()
{
	Destroy();	// ������ Ÿ�̸Ӱ� �ִٸ� ���� �Ѵ�.
}

////////////////////////////////////////
// Member Function Implementation

void MMTimer::Destroy()
{
    if (m_nIDTimer){
        timeKillEvent (m_nIDTimer);
    }
}

// �ݹ��Լ��� ���� �����͸� �ξ���.
BOOL MMTimer::Create (UINT nPeriod, UINT nRes, DWORD dwUser, MMTIMERCALLBACK pfnCallback)
{
    BOOL bRtn = TRUE;
    
    _ASSERT(pfnCallback);
    _ASSERT(nPeriod > 10);
    _ASSERT(nPeriod >= nRes);

    m_nPeriod = nPeriod;
    m_nRes = nRes;
    m_dwUser = dwUser;
    m_pfnCallback = pfnCallback;

    if ((m_nIDTimer = timeSetEvent (m_nPeriod, m_nRes, TimeProc, (DWORD) this, TIME_PERIODIC)) == NULL){
        bRtn = FALSE;
    }

    return (bRtn);
}

// Multimedia Ÿ�̸� �޽����� ȣ��ɶ����� �� �ݹ� �Լ��� �Ҹ����.
void CALLBACK MMTimer::TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{	    
	// �ݹ��Լ��� ���� �����͸� �ξ���.
	// Callback�Լ��� static�̹Ƿ� ���� ������Ʈ�� pointer�� ��� ������� ó���ȴ�.
    MMTimer * ptimer = (MMTimer *) dwUser;
	
	// ��¥ Callback�Լ��� ȣ��ȴ�.
	// �Լ��� MMTIMERCALLBACK�� ���� ������.
    (ptimer->m_pfnCallback) (ptimer->m_dwUser);
}
