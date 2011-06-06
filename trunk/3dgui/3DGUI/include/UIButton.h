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
	// 处理键盘
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// 处理鼠标
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnHotkey();
	virtual bool ContainsPoint(POINT pt);
	// 是否能获取焦点
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	// 渲染
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
};