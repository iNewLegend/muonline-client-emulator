#pragma once
#include "Manager.h"
#include "UIDialog.h"
#include "UIStatic.h"
#include "UICoordDisplay.h"

class CDlgFPS : public CUIDialog
{
public:
	CDlgFPS();
	~CDlgFPS();
public:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnControlRegister();
	void OnBtnClose();
	void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	void SetViewDir(Vec3D& vViewDir);
protected:
private:
	CUIStatic		m_StaticFps;
	CUIStatic		m_StaticInfo;
	CUICoordDisplay	m_CoordDisplay;
};