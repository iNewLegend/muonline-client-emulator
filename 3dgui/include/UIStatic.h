// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIControl.h"

class CUIStatic : public CUIControl
{
public:
	CUIStatic();
	virtual void	OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	virtual bool	ContainsPoint(POINT pt) { return false; }
	virtual const wchar_t* GetText()const { return m_wstrText.c_str(); }
	size_t			GetTextLength() { return m_wstrText.size(); }
	virtual void	SetText(const wchar_t* wcsText);
protected:
	std::wstring m_wstrText;      // Window text  
};
