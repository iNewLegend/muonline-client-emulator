#pragma once
#include "Root.h"
#include "Dialog/DlgMainEditor.h"
#include "TSingleton.h"

// Root
class CMainRoot : public CRoot, public TSingleton<CMainRoot>
{
public:
	CMainRoot();
	~CMainRoot();
public:
	CDlgMainEditor& getMainDialog();
	void setUILanguage(const std::string& strLanguage);
	void OnFrameMove( double fTime, float fElapsedTime );
	void OnFrameRender( double fTime, float fElapsedTime );

	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	CDlgMainEditor	m_dlgMain;
};

#define WE_SCENE	CMainRoot::getInstance().getMainDialog().getDisplay().getScene()
#define WE_TERRAIN	CMainRoot::getInstance().getMainDialog().getDisplay().getTerrain()
#define WE_BRUSH	CMainRoot::getInstance().getMainDialog().getDisplay().getTerrain().GetBrushDecal()