#pragma once
#include "RenderWindow.h"
#include "RenderSystem.h"

class CRoot
{
public:
	CRoot();
	~CRoot();
public:
	bool LoadRenderSystemLibrary(const char* szFilename);
	bool CreateRenderWindow(const wchar_t* wcsTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false);
	virtual void	OnFrameMove(double fTime, float fElapsedTime);
	virtual void	OnFrameRender(double fTime, float fElapsedTime);
	virtual bool	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)= 0;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void Run();
	bool MainUpdate();

	CRenderWindow*	GetRenderWindow(){return m_pRenderWindow;}
	CRenderSystem*	GetRenderSystem(){return m_pRenderSystem;}
	HWND			GetHWND(){return m_hWnd;}
protected:
	CRenderWindow* m_pRenderWindow;
	CRenderSystem* m_pRenderSystem;
	HWND m_hWnd;
};

CRoot& GetRoot();

