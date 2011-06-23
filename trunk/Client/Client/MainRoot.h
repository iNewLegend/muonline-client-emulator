//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* MainRoot.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Root.h"
#include "DlgMain.h"
#include "TSingleton.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CMainRoot : public CRoot, public TSingleton<CMainRoot>
{
public:
	CMainRoot();
	~CMainRoot();
	// ----
	CDlgMain & getMainDialog();
	// ----
	void setUILanguage		(const std::string & strLanguage);
	void OnFrameMove		(double fTime, float fElapsedTime);
	void OnFrameRender		(double fTime, float fElapsedTime);
	// ----
	bool MsgProc			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	CDlgMain	m_dlgMain;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------