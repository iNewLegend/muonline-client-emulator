// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UICombo.h"
#include "UIButton.h"
#include "UISlider.h"

//-----------------------------------------------------------------------------
// Scrollbar control
//-----------------------------------------------------------------------------
class CUIScrollBar : public CUICombo
{
public:
	CUIScrollBar();
	virtual ~CUIScrollBar();
	virtual void OnControlRegister();
	void OnClickUp();
	void OnClickDown();

	virtual bool    HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//
	virtual void	OnLButtonDown(POINT point);
	virtual void	OnLButtonUp(POINT point);

	virtual void    OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	void			SetTrackRange(int nStart, int nEnd);
	int				GetTrackPos();// { return m_nPosition; }
	void			SetTrackPos(int nPosition);// { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
	int				GetPageSize();// { return m_nPageSize; }
	void			SetPageSize(int nPageSize);// { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

	void			Scroll(int nDelta);    // Scroll by nDelta items (plus or minus)
	void			ShowItem(int nIndex);  // Ensure that item nIndex is displayed, scroll if necessary
protected:
	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR            No arrow is down.
	// CLICKED_UP       Up arrow is clicked.
	// CLICKED_DOWN     Down arrow is clicked.
	// HELD_UP          Up arrow is held down for sustained period.
	// HELD_DOWN        Down arrow is held down for sustained period.
	enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

	ARROWSTATE	m_Arrow; // State of the arrows
	double		m_dArrowTS;  // Timestamp of last arrow event.

	CUIButton	m_BtnUp;
	CUIButton	m_BtnDown;
	CUISlider	m_Slider;
};