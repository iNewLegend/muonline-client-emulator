#pragma once

//#include "common.h"
#include <windows.h>
#include <stdio.h>
#define LOG_TARGET_CONSOLE			0x00000001
#define LOG_TARGET_FILE				0x00000002
#define LOG_TARGET_WINDOW			0x00000004
#define LOG_TARGET_ALL				(LOG_TARGET_CONSOLE|LOG_TARGET_FILE|LOG_TARGET_WINDOW)
#define LOG_WINDOW_TITLE			"LOGÊä³ö"
#define LOG_WINDOW_CX				300
#define LOG_WINDOW_CY				600

#define LOG(_text) GetLog()->Log(_text);

class CLog
{
public:
	CLog();
	CLog(unsigned long uTarget, const char* szFilename = "log.txt");
	~CLog();
public:
	void	SetFilename(const char* szFilename);
	void	CreateLogWindow();
	int		Log(const char* fmt, ...);

private:
	unsigned long	m_uTarget;
	char			m_szFilename[260];
	HWND			m_hwnd;
	HWND			m_hwndList;
private:
	static	LRESULT CALLBACK
	CLog::WndProcLog(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

CLog* GetLog();