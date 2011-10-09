#include "UISlider.h"
#include "UIDialog.h"

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 17

CUISlider::CUISlider()
{
	m_Type = UI_CONTROL_SLIDER;
	m_nMin = 0;
	m_nMax = 100;
	m_nValue = 50;
	m_bH = true;
	m_nPageSize = 1;
}
#include "tinyxml.h"
void CUISlider::SetStyle(const std::string& strStyleName)
{
	m_Style.setStyle(strStyleName);
	m_StyleButton.setStyle(strStyleName+".button");// 没有tinyxml会编译不通过
}

void CUISlider::UpdateRects()
{
	CUIControl::UpdateRects();
	m_bH = m_rcBoundingBox.getWidth()>m_rcBoundingBox.getHeight();
	m_rcButton.set(0,0,m_rcBoundingBox.getWidth(),m_rcBoundingBox.getHeight());
	if(m_bH)
	{
		int nRangeLength = m_nMax-m_nMin;
		int nButtonSize = __max(m_rcBoundingBox.getWidth()*m_nPageSize/(nRangeLength+m_nPageSize),SCROLLBAR_MINTHUMBSIZE);
		m_rcButton.right = m_rcButton.left + nButtonSize;
		if(nRangeLength>0)
		{
			int nButtonX = (int) ((m_nValue - m_nMin) * (float)(m_rcBoundingBox.getWidth()-nButtonSize)/nRangeLength);
			m_rcButton.offset(nButtonX, 0);
		}
	}
	else
	{
		int nRangeLength = m_nMax-m_nMin;
		int nButtonSize = __max(m_rcBoundingBox.getHeight()*m_nPageSize/(nRangeLength+m_nPageSize),SCROLLBAR_MINTHUMBSIZE);
		m_rcButton.bottom = m_rcButton.top + nButtonSize;
		if(nRangeLength>0)
		{
			int nButtonY = (int) ((m_nValue - m_nMin) * (float)(m_rcBoundingBox.getHeight()-nButtonSize)/nRangeLength);
			m_rcButton.offset(0, nButtonY);
		}
	}
}

int CUISlider::ValueFromPos(POINT pt)
{
	if(m_bH)
	{
		int nButtonSize = m_rcButton.getWidth();
		float fValuePerPixel = (float)(m_nMax - m_nMin)/(float)(m_rcBoundingBox.getWidth()-nButtonSize);
		return (int) (0.5f + m_nMin + fValuePerPixel * (pt.x /*- m_rcBoundingBox.left*/-nButtonSize));
	}
	else
	{
		int nButtonSize = m_rcButton.getHeight();
		float fValuePerPixel = (float)(m_nMax - m_nMin)/(float)(m_rcBoundingBox.getHeight()-nButtonSize);
		return (int) (0.5f + m_nMin + fValuePerPixel * (pt.y /*- m_rcBoundingBox.top*/-nButtonSize));
	}
	return 0;
}

bool CUISlider::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_HOME:
				SetValueInternal(m_nMin);
				return true;

			case VK_END:
				SetValueInternal(m_nMax);
				return true;

			case VK_LEFT:
			case VK_UP:
				SetValueInternal(m_nValue - 1);
				return true;

			case VK_RIGHT:
			case VK_DOWN:
				SetValueInternal(m_nValue + 1);
				return true;

			case VK_NEXT:
				SetValueInternal(m_nValue - (10 > (m_nMax - m_nMin) / 10 ? 10 : (m_nMax - m_nMin) / 10));
				return true;

			case VK_PRIOR:
				SetValueInternal(m_nValue + (10 > (m_nMax - m_nMin) / 10 ? 10 : (m_nMax - m_nMin) / 10));
				return true;
			}
			break;
		}
	}
	return false;
}

void CUISlider::OnMouseMove(POINT pt)
{
	if(IsPressed())
	{
		screenToClient(pt);
		pt.x+=m_ptDragOffset.x;
		pt.y+=m_ptDragOffset.y;
		SetValueInternal(ValueFromPos(pt));
	}
}

void CUISlider::OnMouseWheel(POINT point,short wheelDelta)
{
	int nScrollAmount = int(wheelDelta) / WHEEL_DELTA;
	SetValueInternal(m_nValue - nScrollAmount);
}

void CUISlider::OnLButtonDown(POINT point)
{
	screenToClient(point);
	if(m_rcButton.ptInRect(point))
	{
		// Pressed while inside the control
		SetPressed(true);

		//m_nDragX = pt.x;
		m_ptDragOffset.x = m_rcButton.right - point.x;
		m_ptDragOffset.y = m_rcButton.bottom - point.y;
		SetFocus();
		return;
	}
	if(m_rcRelativeBox.ptInRect(point))
	{
		m_ptDragOffset.x=0;
		m_ptDragOffset.y=0;
		//SetPressed(true);
		SetFocus();
		if(m_bH)
		{
			if(point.x > m_rcButton.left)
			{
				SetValueInternal(m_nValue + __max(m_nPageSize-1,1));
				return;
			}
			if(point.x < m_rcButton.right)
			{
				SetValueInternal(m_nValue - __max(m_nPageSize-1,1));
				return;
			}
		}
		else
		{
			if(point.y > m_rcButton.top)
			{
				SetValueInternal(m_nValue + m_nPageSize - 1);
				return;
			}
			if(point.y < m_rcButton.bottom)
			{
				SetValueInternal(m_nValue - m_nPageSize + 1);
				return;
			}
		}
	}
}

void CUISlider::OnLButtonUp(POINT point)
{
	if(IsPressed())
	{
		SetPressed(false);
		SendEvent(EVENT_SLIDER_VALUE_CHANGED);
	}
}

bool CUISlider::SetValue(int nValue)
{
	nValue = __max(m_nMin, nValue);
	nValue = __min(m_nMax, nValue);
	if(nValue == m_nValue)
		return false;
	m_nValue = nValue;
	UpdateRects();
	return true;
}

void CUISlider::SetRange(int nMin, int nMax) 
{
	m_nMin = nMin;
	m_nMax = __max(nMax,nMin);
	SetValue(m_nValue);
	UpdateRects();
}

int CUISlider::GetPageSize()
{
	return m_nPageSize;
}

void CUISlider::SetPageSize(int nPageSize)
{
	m_nPageSize = __max(nPageSize, 1);
}

void CUISlider::SetValueInternal(int nValue)
{
	if (SetValue(nValue))
	{
		SendEvent(EVENT_SLIDER_VALUE_CHANGED);
	}
}

void CUISlider::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CONTROL_STATE iState = GetState();
	m_Style.draw(mTransform,m_rcRelativeBox,L"",iState, fElapsedTime);
	m_StyleButton.draw(m_Style.mWorld,m_rcButton,L"",iState, fElapsedTime);
}