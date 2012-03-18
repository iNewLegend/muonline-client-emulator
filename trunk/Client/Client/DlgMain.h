//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* DlgMain.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIMainDialog.h"
#include "UIIcon.h"
#include "TSingleton.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDlgMain : public CUIMainDialog, public TSingleton<CDlgMain>
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
	void		setIconCursor(CUIIcon* pIcon){m_pIconCursor = pIcon;}
	CUIIcon*	getIconCursor(){ return m_pIconCursor; }
private:
	CUIIcon*		m_pIconCursor;
	bool			m_bShowFPS;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------