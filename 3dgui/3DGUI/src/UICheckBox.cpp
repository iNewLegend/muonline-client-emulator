#include "UICheckBox.h"
#include "tinyxml.h"

CUICheckBox::CUICheckBox()
{
	m_Type = UI_CONTROL_CHECKBOX;
	m_bChecked = false;
}

void CUICheckBox::XMLParse(const TiXmlElement* pControlElement)
{
	CUIButton::XMLParse(pControlElement);
	// Checked
	if (pControlElement->Attribute("checked"))
	{
		std::string strChecked = pControlElement->Attribute("checked");
		if ("true" == strChecked)
		{
			SetChecked(true);
		}
	}
}

void CUICheckBox::SetStyle(const std::string& strStyleName)
{
	m_Style.setStyle(strStyleName);
	m_StyleChecked.setStyle(strStyleName+".checked");
}

void CUICheckBox::OnPressUp()
{
	m_bChecked = !m_bChecked; 
	SendEvent(EVENT_CHECKBOX_CHANGED); 
}

void CUICheckBox::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIControl::updateUIMatrix(mTransform, fTime, fElapsedTime);
	RECT rc;
	rc.left =0;
	rc.right = m_rcRelativeBox.getWidth();
	rc.top =0;
	rc.bottom = m_rcRelativeBox.getHeight();
	CONTROL_STATE iState = GetState();
	m_Style.draw(rc, GetText(), m_bChecked?CONTROL_STATE_HIDDEN:iState, fElapsedTime);
	m_Style.draw(rc, GetText(), m_bChecked?iState:CONTROL_STATE_HIDDEN, fElapsedTime);
}