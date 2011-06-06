#pragma once
#include "windows.h"

class CTimer
{
public:
	CTimer();

	void Reset();				// 重置计时器
	void Start();				// 开启计时器
	void Stop();				// 停止(或暂停)计时器
	void Advance();				// 前进计时器 0.1 秒
	double GetAbsoluteTime();	// 获取绝对的系统时间
	double GetTime();			// 获取当前的时间
	double GetElapsedTime(bool bRecord = true);	// 获取在Get*ElapsedTime()调用间间隔时间
	void GetTimeValues(double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime, float& fSleepTime, int nLockFPS); // 一次性获取全部时间值
	bool IsStopped();			// 计时器是否停止

	// Limit the current thread to one processor (the current one). This ensures that timing code runs
	// on only one processor, and will not suffer any ill effects from power management.
	void LimitThreadAffinityToCurrentProc();

protected:
	LARGE_INTEGER GetAdjustedCurrentTime(); // 获取调整过的当前时间

	bool m_bTimerStopped;
	LONGLONG m_llQPFTicksPerSec;

	LONGLONG m_llStopTime;
	LONGLONG m_llLastElapsedTime;
	LONGLONG m_llBaseTime;
};

CTimer& GetGlobalTimer();