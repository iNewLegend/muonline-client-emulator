//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* UIGeneralDialog.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIDialog.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CUIGeneralDialog : public CUIDialog
{
public:
	// ----
	virtual void OnControlRegister	();
	virtual bool OnInitDialog		();
	virtual void OnBtnClose			();
	virtual void OnFrameMove		(double fTime, float fElapsedTime);
	virtual void OnFrameRender		(const Matrix& mTransform, double fTime, float fElapsedTime);
	virtual bool MsgProc			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------