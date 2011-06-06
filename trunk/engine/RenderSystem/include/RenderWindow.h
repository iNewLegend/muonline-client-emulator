#pragma once
#include <windows.h>
#include <xstring>

class CRenderWindow
{
public:
	CRenderWindow();
	~CRenderWindow();
public:
	virtual bool	Create(WNDPROC pWndProc, const std::wstring& strWindowTitle, unsigned long uWidth, unsigned long uHeight, bool bFullScreen = false) = 0;
	virtual void	SetFullscreen(bool bFullScreen, unsigned long width, unsigned long height) {}
	//virtual void Resize(unsigned long width, unsigned long height) = 0;
	virtual double	GetTime()= 0;
	virtual float	GetElapsedTime()= 0;
	virtual bool	FrameBegin()= 0;
	virtual void	FrameEnd()= 0;
	virtual HWND	GetHWND()= 0;
	virtual LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)= 0;
private:
};