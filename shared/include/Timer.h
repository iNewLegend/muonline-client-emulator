#pragma once
#include "windows.h"

class CTimer
{
public:
	CTimer();

	void Reset();				// ���ü�ʱ��
	void Start();				// ������ʱ��
	void Stop();				// ֹͣ(����ͣ)��ʱ��
	void Advance();				// ǰ����ʱ�� 0.1 ��
	double GetAbsoluteTime();	// ��ȡ���Ե�ϵͳʱ��
	double GetTime();			// ��ȡ��ǰ��ʱ��
	double GetElapsedTime(bool bRecord = true);	// ��ȡ��Get*ElapsedTime()���ü���ʱ��
	void GetTimeValues(double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime, float& fSleepTime, int nLockFPS); // һ���Ի�ȡȫ��ʱ��ֵ
	bool IsStopped();			// ��ʱ���Ƿ�ֹͣ

	// Limit the current thread to one processor (the current one). This ensures that timing code runs
	// on only one processor, and will not suffer any ill effects from power management.
	void LimitThreadAffinityToCurrentProc();

protected:
	LARGE_INTEGER GetAdjustedCurrentTime(); // ��ȡ�������ĵ�ǰʱ��

	bool m_bTimerStopped;
	LONGLONG m_llQPFTicksPerSec;

	LONGLONG m_llStopTime;
	LONGLONG m_llLastElapsedTime;
	LONGLONG m_llBaseTime;
};

CTimer& GetGlobalTimer();