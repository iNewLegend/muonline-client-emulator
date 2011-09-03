//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* DlgMain.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIMainDialog.h"
#include "UIDisplayWorld.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDlgMain : public CUIMainDialog
{
private:
	CUIDisplayWorld	m_DisplayWorld;
public:
	CDlgMain();
	~CDlgMain();
	// ----
	virtual void OnControlRegister	();
	virtual bool OnInitDialog		();
	virtual void OnFrameMove		(double fTime, float fElapsedTime);
	virtual bool MsgProc			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ----
	CUIDisplayWorld & getDisplay(){ return m_DisplayWorld; };

};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------