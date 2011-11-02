#include "UIButton.h"
#include "UICombo.h"

CUIButton::CUIButton()
{
	m_Type = UI_CONTROL_BUTTON;
	m_nHotkey = 0;
}

void CUIButton::OnPressDown()
{
}

void CUIButton::OnPressUp()
{
	SendEvent(EVENT_BUTTON_CLICKED);
}

bool CUIButton::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_SPACE:
				SetPressed(true);
				OnPressDown();
				return true;
			}
		}
	case WM_KEYUP:
		{
			switch(wParam)
			{
			case VK_SPACE:
				if(IsPressed())
				{
					SetPressed(false);
					OnPressUp();
				}
				return true;
			}
		}
	}
	return false;
}

void CUIButton::OnLButtonDown(POINT point)
{
	SetPressed(true);
}

void CUIButton::OnLButtonDblClk(POINT point)
{
	SendEvent(EVENT_BUTTON_DBLCLK);
}

void CUIButton::OnLButtonUp(POINT point)
{
	if(IsPressed())
	{
		SetPressed(false);
		OnPressUp();
	}
}

void CUIButton::OnHotkey()
{
	OnPressUp();
}

bool CUIButton::ContainsPoint(POINT pt)
{
	return m_rcBoundingBox.ptInRect(pt);
}

void CUIButton::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIControl::OnFrameRender(mTransform, fTime, fElapsedTime);
}