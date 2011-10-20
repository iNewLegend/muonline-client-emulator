#pragma once
#include "Root.h"
#include "UIMainDialog.h"
#include "CustomUI/DlgUIList.h"
// œ‘ æ¿‡
class CMainRoot : public CRoot
{
public:
	CMainRoot();
	~CMainRoot();
public:
	void Run();
	void OnFrameMove( double fTime, float fElapsedTime );
	void OnFrameRender( double fTime, float fElapsedTime );

	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	CUIMainDialog		m_dlgMain;
	CDlgUIList			m_dlgUIList;
	CUIDialog			m_dlgView;
};