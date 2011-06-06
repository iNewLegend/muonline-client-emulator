#include "Log.h"
#include <time.h>

CLog* GetLog()
{
	static CLog g_Log(LOG_TARGET_FILE);
	return &g_Log;
}

CLog::CLog()
{
	m_uTarget = LOG_TARGET_FILE;
	m_szFilename[0] = NULL;
	m_hwnd = NULL;
}

CLog::CLog(unsigned long uTarget, const char* szFilename)
{
	m_uTarget = uTarget;

	if(uTarget & LOG_TARGET_FILE)
		SetFilename(szFilename);
	else
		m_szFilename[0] = NULL;

	if(uTarget & LOG_TARGET_WINDOW)
		CreateLogWindow();
	else
		m_hwnd = NULL;

	Log("Logging Started...");
}

CLog::~CLog()
{
	DestroyWindow(m_hwnd);
}

void CLog::SetFilename(const char* szFilename)
{
	strcpy(m_szFilename, szFilename);
}

void CLog::CreateLogWindow()
{
	int			x, y, cx, cy;
	WNDCLASS	wc;
	RECT		rc;

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)CLog::WndProcLog;
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= GetModuleHandle(NULL);
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH) ;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "LogWindow";

	RegisterClass(&wc);

	cx = LOG_WINDOW_CX;
	cy = LOG_WINDOW_CY;
	x = GetSystemMetrics(SM_CXSCREEN) - cx;
	y = 0;

	m_hwnd = CreateWindow("LogWindow", LOG_WINDOW_TITLE, WS_POPUP | WS_CAPTION, x, y, cx, cy, NULL, NULL, GetModuleHandle(NULL), NULL);
	GetClientRect(m_hwnd, &rc);
	m_hwndList = CreateWindow("LISTBOX", "", WS_CHILD | WS_VSCROLL, 0, 0, rc.right, rc.bottom, m_hwnd, NULL, GetModuleHandle(NULL), NULL);
	ShowWindow(m_hwnd, SW_SHOW);
	ShowWindow(m_hwndList, SW_SHOW);
}

int CLog::Log(const char* fmt, ...)
{
	char	buff[1024];
	WCHAR	wbuff[1024];
	char	date[128];
	char	time[128];

	_strdate(date);
	_strtime(time);
	//vswprintf
	vsprintf(buff, fmt, (char *)(&fmt+1));

	// Console输出的情况 
	if(m_uTarget & LOG_TARGET_CONSOLE)
	{
		printf("(date[%s] time[%s]) : %s\n", date, time, buff);
	}

	// Log File输出的情况
	if(m_uTarget & LOG_TARGET_FILE)
	{
		FILE*	fp = NULL;
		fp = fopen(m_szFilename, "a+");
		if(fp)
		{
			fprintf(fp, "(date[%s] time[%s]) : %s\n", date, time, buff);
			fclose(fp);
		}
	}

	int wlen = MultiByteToWideChar(CP_ACP, 0, buff, -1, 0, 0);
	MultiByteToWideChar(CP_ACP,0,buff,-1,wbuff,wlen);

	// Log Window输出的情况 
	if(m_uTarget & LOG_TARGET_WINDOW)
	{
		SendMessage(m_hwndList, LB_ADDSTRING, 0, (LPARAM) wbuff);
		UINT32 n = SendMessage(m_hwndList, LB_GETCOUNT, 0, 0L) - 1;
		SendMessage(m_hwndList, LB_SETCURSEL, (WPARAM)n, 0L);
	}
	return 1;
}

LRESULT CALLBACK 
CLog::WndProcLog(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

