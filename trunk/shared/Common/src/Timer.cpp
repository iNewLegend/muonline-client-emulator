#include "Timer.h"
//#include <mmsystem.h>
//#include <commctrl.h> // for InitCommonControls() 
//#include <shellapi.h> // for ExtractIcon()
//#include <new.h>      // for placement new
//#include <math.h>      
//#include <limits.h>      
//#include <stdio.h>
//#include <XInput.h> // Header for XInput APIs
#include "assert.h"

CTimer& GetGlobalTimer()
{
	static CTimer timer;
	return timer;
}

CTimer::CTimer(void)
{
	m_bTimerStopped     = true;
	m_llQPFTicksPerSec  = 0;

	m_llStopTime        = 0;
	m_llLastElapsedTime = 0;
	m_llBaseTime        = 0;

	// Use QueryPerformanceFrequency to get the frequency of the counter
	LARGE_INTEGER qwTicksPerSec;
	QueryPerformanceFrequency(&qwTicksPerSec);
	m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
}

// CTimer::~CTimer(void)
// {
// }

void CTimer::Reset()
{
	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	m_llBaseTime        = qwTime.QuadPart;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_llStopTime        = 0;
	m_bTimerStopped     = FALSE;
}

void CTimer::Start()
{
	// Get the current time
	LARGE_INTEGER qwTime;
	QueryPerformanceCounter(&qwTime);

	if(m_bTimerStopped)
		m_llBaseTime += qwTime.QuadPart - m_llStopTime;
	m_llStopTime = 0;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_bTimerStopped = FALSE;
}

void CTimer::Stop()
{
	if(!m_bTimerStopped)
	{
		LARGE_INTEGER qwTime;
		QueryPerformanceCounter(&qwTime);
		m_llStopTime = qwTime.QuadPart;
		m_llLastElapsedTime = qwTime.QuadPart;
		m_bTimerStopped = TRUE;
	}
}

void CTimer::Advance()
{
	m_llStopTime += m_llQPFTicksPerSec/10;
}

double CTimer::GetAbsoluteTime()
{
	LARGE_INTEGER qwTime;
	QueryPerformanceCounter(&qwTime);

	double fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;

	return fTime;
}

double CTimer::GetTime()
{
	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	double fAppTime = (double) (qwTime.QuadPart - m_llBaseTime) / (double) m_llQPFTicksPerSec;

	return fAppTime;
}

void CTimer::GetTimeValues(double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime, float& fSleepTime, int nLockFPS)
{
	assert(pfTime && pfAbsoluteTime && pfElapsedTime);    

	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	float fElapsedTime = (float) ((double) (qwTime.QuadPart - m_llLastElapsedTime) / (double) m_llQPFTicksPerSec);
	// ห๘ึก
	if (0&&nLockFPS > 0)
	{
		fSleepTime = 1.0f / (float)nLockFPS - fElapsedTime;
		if (fSleepTime > 0.0f)
		{
			Sleep(DWORD(fSleepTime*1000));
			qwTime = GetAdjustedCurrentTime();
			fElapsedTime = (float) ((double) (qwTime.QuadPart - m_llLastElapsedTime) / (double) m_llQPFTicksPerSec);
		}
		else
		{
			fSleepTime = 0.0f;
		}
	}

	m_llLastElapsedTime = qwTime.QuadPart;

	// Clamp the timer to non-negative values to ensure the timer is accurate.
	// fElapsedTime can be outside this range if processor goes into a 
	// power save mode or we somehow get shuffled to another processor.  
	// However, the main thread should call SetThreadAffinityMask to ensure that 
	// we don't get shuffled to another processor.  Other worker threads should NOT call 
	// SetThreadAffinityMask, but use a shared copy of the timer data gathered from 
	// the main thread.
	if(fElapsedTime < 0.0f)
		fElapsedTime = 0.0f;

	*pfAbsoluteTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
	*pfTime = (qwTime.QuadPart - m_llBaseTime) / (double) m_llQPFTicksPerSec;   
	*pfElapsedTime = fElapsedTime;
}

double CTimer::GetElapsedTime(bool bRecord)
{
	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	double fElapsedTime = (double) (qwTime.QuadPart - m_llLastElapsedTime) / (double) m_llQPFTicksPerSec;
	if (bRecord)
	{
		m_llLastElapsedTime = qwTime.QuadPart;
	}

	// See the explanation about clamping in CTimer::GetTimeValues()
	if(fElapsedTime < 0.0f)
		fElapsedTime = 0.0f;

	return fElapsedTime;
}

// If stopped, returns time when stopped otherwise returns current time
LARGE_INTEGER CTimer::GetAdjustedCurrentTime()
{
	LARGE_INTEGER qwTime;
	if(m_llStopTime != 0)
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter(&qwTime);
	return qwTime;
}

bool CTimer::IsStopped()
{
	return m_bTimerStopped;
}

// Limit the current thread to one processor (the current one). This ensures that timing code 
// runs on only one processor, and will not suffer any ill effects from power management.
// See "Game Timing and Multicore Processors" for more details

void CTimer::LimitThreadAffinityToCurrentProc()
{
	HANDLE hCurrentProcess = GetCurrentProcess();

	// Get the processor affinity mask for this process
	DWORD_PTR dwProcessAffinityMask = 0;
	DWORD_PTR dwSystemAffinityMask = 0;

	if(GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask) != 0 && dwProcessAffinityMask)
	{
		// Find the lowest processor that our process is allows to run against
		DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

		// Set this as the processor that our thread must always run against
		// This must be a subset of the process affinity mask
		HANDLE hCurrentThread = GetCurrentThread();
		if(INVALID_HANDLE_VALUE != hCurrentThread)
		{
			SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
			CloseHandle(hCurrentThread);
		}
	}

	CloseHandle(hCurrentProcess);
}