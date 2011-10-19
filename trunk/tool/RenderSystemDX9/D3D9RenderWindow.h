#pragma once
#include "RenderWindow.h"

class CD3D9RenderWindow: public CRenderWindow
{
public:
	CD3D9RenderWindow();
	~CD3D9RenderWindow();
public:
	// Choose either DXUTCreateWindow or DXUTSetWindow.  If using DXUTSetWindow, consider using DXUTStaticWndProc
	HRESULT D3DCreateWindow(WNDPROC pWndProc = NULL, const std::wstring& strWindowTitle = L"Direct3D Window", 
		HINSTANCE hInstance = NULL, HICON hIcon = NULL, HMENU hMenu = NULL,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	HRESULT DXUTSetWindow(WNDPROC pWndProc, HWND hWndFocus, HWND hWndDeviceWindowed, bool bHandleMessages = true);
	// Choose either DXUTCreateDevice or DXUTSetDevice or DXUTCreateDeviceFromSettings
	HRESULT D3DCreateDevice(UINT AdapterOrdinal = 0/*D3DADAPTER_DEFAULT*/, bool bWindowed = true, 
		int nSuggestedWidth = 0, int nSuggestedHeight = 0);
	bool Create(WNDPROC pWndProc, const std::wstring& wstrWindowTitle, unsigned long uWidth, unsigned long uHeight, bool bFullScreen = false); 
	void SetFullscreen(bool bFullScreen, unsigned long width, unsigned long height);
	double GetTime();
	float GetElapsedTime();
	bool FrameBegin();
	void FrameEnd();
	HWND GetHWND();
	void CheckForWindowSizeChange();
	void CheckForWindowChangingMonitors();
	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool Init();
	bool m_bActive;
	bool m_bMoving;
	bool m_bMinimized;
};