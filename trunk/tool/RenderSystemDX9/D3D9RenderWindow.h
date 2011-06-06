#pragma once
#include "RenderWindow.h"

class CD3D9RenderWindow: public CRenderWindow
{
public:
	CD3D9RenderWindow();
	~CD3D9RenderWindow();
public:
	bool Create(WNDPROC pWndProc, const std::wstring& wstrWindowTitle, unsigned long uWidth, unsigned long uHeight, bool bFullScreen = false); 
	void SetFullscreen(bool bFullScreen, unsigned long width, unsigned long height);
	double GetTime();
	float GetElapsedTime();
	bool FrameBegin();
	void FrameEnd();
	HWND GetHWND();
	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool Init(bool bParseCommandLine, bool bShowMsgBoxOnError, bool bHandleAltEnter);
	bool m_bActive;
	bool m_bMoving;
	bool m_bMinimized;
};