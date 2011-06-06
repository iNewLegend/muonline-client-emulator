#include "UIStatic.h"
#include "UIDialog.h"
#include "tinyxml.h"

CUIStatic::CUIStatic()
{
	m_Type = UI_CONTROL_STATIC;
}

void CUIStatic::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	if (m_rcOffset.getWidth()==0&&m_rcScale.getWidth()==0)
	{
		CRect<float> rect = m_rcBoundingBox.getRECT();
		UIGraph::getInstance().CalcTextRect(m_wstrText.c_str(),rect);
		m_rcBoundingBox=rect.getRECT();
	}
	m_Style.draw(mTransform,m_rcRelativeBox,m_wstrText.c_str(),GetState(), fElapsedTime);
}

void CUIStatic::SetText(const wchar_t* wcsText)
{
	m_wstrText = wcsText;
}