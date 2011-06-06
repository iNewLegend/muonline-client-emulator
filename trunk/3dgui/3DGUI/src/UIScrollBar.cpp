#include "UIScrollBar.h"
#include "UIDialog.h"

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 17

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

CUIScrollBar::CUIScrollBar()
{
	m_Arrow = CLEAR;
	m_dArrowTS = 0.0;
}

CUIScrollBar::~CUIScrollBar()
{
}

void CUIScrollBar::OnControlRegister()
{
	CUICombo::OnControlRegister();

	RegisterControl("IDC_BTN_UP",m_BtnUp);
	RegisterControl("IDC_BTN_DOWN",m_BtnDown);
	RegisterControl("IDC_SLIDER",m_Slider);

	RegisterControlEvent("IDC_BTN_UP",(PEVENT)&CUIScrollBar::OnClickUp);
	RegisterControlEvent("IDC_BTN_DOWN",(PEVENT)&CUIScrollBar::OnClickDown);
}

// Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
// value scrolls up.
void CUIScrollBar::Scroll(int nDelta)
{
	m_Slider.SetValue(m_Slider.GetValue()+nDelta);
}

void CUIScrollBar::ShowItem(int nIndex)
{
	if(nIndex < 0)
		nIndex = 0;

	if(nIndex >= m_Slider.getMax()+m_Slider.GetPageSize())
		nIndex = m_Slider.getMax()+m_Slider.GetPageSize() - 1;

	if(m_Slider.GetValue() > nIndex)
		m_Slider.SetValue(nIndex);
	else if(m_Slider.GetValue() + m_Slider.GetPageSize() <= nIndex)
		m_Slider.SetValue(nIndex - m_Slider.GetPageSize() + 1);
}

bool CUIScrollBar::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CUIScrollBar::OnClickUp()
{
	// Check for click on up button
	//SetCapture(UIGetHWND());
	Scroll(-1);	
	m_Arrow = CLICKED_UP;
	m_dArrowTS = GetTickCount()*0.001f;
}

void CUIScrollBar::OnClickDown()
{
	// Check for click on down button
	//SetCapture(UIGetHWND());
	Scroll(1);
	m_Arrow = CLICKED_DOWN;
	m_dArrowTS = GetTickCount()*0.001f;
}

void CUIScrollBar::OnLButtonDown(POINT point)
{
	CUICombo::OnLButtonDown(point);
}

void CUIScrollBar::OnLButtonUp(POINT point)
{
	CUICombo::OnLButtonUp(point);
	SetPressed(false);
	m_Arrow = CLEAR;
}

void CUIScrollBar::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	// Check if the arrow button has been held for a while.
	// If so, update the thumb position to simulate repeated
	// scroll.
	if(m_BtnUp.IsPressed())
	{
		switch(m_Arrow)
		{
		case CLICKED_UP:
			if(SCROLLBAR_ARROWCLICK_DELAY < fTime - m_dArrowTS)
			{
				Scroll(-1);
				m_Arrow = HELD_UP;
				m_dArrowTS = fTime;
			}
			break;
		case HELD_UP:
			if(SCROLLBAR_ARROWCLICK_REPEAT < fTime - m_dArrowTS)
			{
				Scroll(-1);
				m_dArrowTS = fTime;
			}
			break;
		}
	}
	else if(m_BtnDown.IsPressed())
	{
		switch(m_Arrow)
		{
		case CLICKED_DOWN:
			if(SCROLLBAR_ARROWCLICK_DELAY < fTime - m_dArrowTS)
			{
				Scroll(1);
				m_Arrow = HELD_DOWN;
				m_dArrowTS = fTime;
			}
			break;
		case HELD_DOWN:
			if(SCROLLBAR_ARROWCLICK_REPEAT < fTime - m_dArrowTS)
			{
				Scroll(1);
				m_dArrowTS = fTime;
			}
			break;
		}
	}

	CUICombo::OnFrameRender(mTransform,fTime,fElapsedTime);
}

void CUIScrollBar::SetTrackRange(int nStart, int nEnd)
{
	m_Slider.SetRange(nStart,nEnd-m_Slider.GetPageSize());
}

int CUIScrollBar::GetTrackPos()
{
	return m_Slider.GetValue();
}

void CUIScrollBar::SetTrackPos(int nPosition)
{
	m_Slider.SetValue(nPosition);
}

void CUIScrollBar::SetPageSize(int nPageSize)
{
	m_Slider.SetPageSize(nPageSize);
}

int CUIScrollBar::GetPageSize()
{
	return m_Slider.GetPageSize();
}