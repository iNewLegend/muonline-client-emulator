#include "UIProgress.h"
#include "UIDialog.h"

CUIProgress::CUIProgress()
{
	m_Type = UI_CONTROL_PROGRESS;
	m_nMin = 0;
	m_nMax = 100;
	m_nValue = 50;
}

void CUIProgress::XMLParse(const TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
}

void CUIProgress::UpdateRects()
{
	CUIControl::UpdateRects();
	m_rcButton = m_rcBoundingBox;
	int nButtonX = (int) ((m_nValue - m_nMin) * (float)m_rcBoundingBox.getWidth() / (m_nMax - m_nMin));
	m_rcButton.right = m_rcButton.left + nButtonX;
}

bool CUIProgress::SetValue(int nValue)
{
	nValue = __max(m_nMin, nValue);
	nValue = __min(m_nMax, nValue);
	if(nValue == m_nValue)
		return false;
	m_nValue = nValue;
	UpdateRects();
	return true;
}

void CUIProgress::SetRange(int nMin, int nMax) 
{
	m_nMin = nMin;
	m_nMax = nMax;
	SetValue(m_nValue);
}

void CUIProgress::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIControl::OnFrameRender(mTransform, fTime, fElapsedTime);
}