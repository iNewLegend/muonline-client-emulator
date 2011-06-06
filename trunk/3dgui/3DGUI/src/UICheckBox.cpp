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
	m_Style.SetStyle(strStyleName);
	m_StyleChecked.SetStyle(strStyleName+".checked");
}

void CUICheckBox::OnPressUp()
{
	m_bChecked = !m_bChecked; 
	SendEvent(EVENT_CHECKBOX_CHANGED); 
}

void CUICheckBox::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CONTROL_STATE iState = GetState();
	m_Style.draw(mTransform,m_rcRelativeBox,m_wstrText.c_str(),m_bChecked?CONTROL_STATE_HIDDEN:iState,fElapsedTime);
	m_StyleChecked.draw(mTransform,m_rcRelativeBox,m_wstrText.c_str(),m_bChecked?iState:CONTROL_STATE_HIDDEN,fElapsedTime);
}