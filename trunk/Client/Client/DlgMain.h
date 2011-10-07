//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* DlgMain.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIMainDialog.h"
#include "UIDisplayWorld.h"
#include "UIIcon.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDlgMain : public CUIMainDialog
{
public:
	CDlgMain();
	~CDlgMain();
	// ----
	virtual void OnControlRegister	();
	virtual bool OnInitDialog		();
	virtual void OnFrameMove		(double fTime, float fElapsedTime);
	virtual void OnFrameRender		(const Matrix& mTransform, double fTime, float fElapsedTime);
	virtual bool MsgProc			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ----
	CUIDisplayWorld & getDisplay(){ return m_DisplayWorld; };
	CUIIcon & getIconCursor(){ return m_IconCursor; };
private:
	CUIDisplayWorld	m_DisplayWorld;
	CUIIcon			m_IconCursor;

};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------