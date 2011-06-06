// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIStatic.h"

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class CUIButton : public CUIStatic
{
public:
	CUIButton();
	virtual void OnPressDown();
	virtual void OnPressUp();
	// �������
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// �������
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnHotkey();
	virtual bool ContainsPoint(POINT pt);
	// �Ƿ��ܻ�ȡ����
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	// ��Ⱦ
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
};