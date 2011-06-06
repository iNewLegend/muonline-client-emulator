// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once

#include "UIControl.h"

//-----------------------------------------------------------------------------
// Slider control
//-----------------------------------------------------------------------------
class CUISlider : public CUIControl
{
public:
	CUISlider();
	virtual void SetStyle(const std::string& strStyleName);
	virtual bool	CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool	HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	//
	virtual void	UpdateRects(); 
	virtual void	OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	bool			SetValue(int nValue);
	int				GetValue() const { return m_nValue; };
	int				getMin() const { return m_nMin; };
	int				getMax() const { return m_nMax; };
	void			GetRange(int &nMin, int &nMax) const { nMin = m_nMin; nMax = m_nMax; }
	void			SetRange(int nMin, int nMax);
	int				GetPageSize();
	void			SetPageSize(int nPageSize);
protected:
	void			SetValueInternal(int nValue);
	int				ValueFromPos(POINT pt); 
protected:
	CUIStyle m_StyleButton;
	int		m_nValue;
	int		m_nMin;
	int		m_nMax;
	int		m_nDragX;      // Mouse position at start of drag
	POINT	m_ptDragOffset; // Drag offset from the center of the button
	CRect<int>	m_rcButton;
	bool	m_bH;
	int		m_nPageSize;  // How many items are displayable in one page

};