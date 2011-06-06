// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIDialog.h"

class CUIMainDialog : public CUIDialog
{
public:
	CUIMainDialog();
	~CUIMainDialog();
	virtual	bool OnInitDialog();
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual bool create(const char* szFilename, const char* szID);

};
