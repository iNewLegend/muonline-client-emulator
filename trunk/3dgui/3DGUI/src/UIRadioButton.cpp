#include "UIRadioButton.h"
#include "UICombo.h"
#include "tinyxml.h"

CUIRadioButton::CUIRadioButton()
{
	m_Type = UI_CONTROL_RADIOBUTTON;
	m_uButtonGroup = 0;
}

void CUIRadioButton::XMLParse(const TiXmlElement* pControlElement)
{
	CUICheckBox::XMLParse(pControlElement);
	// group
	if (pControlElement->Attribute("group"))
	{
		m_uButtonGroup=atoi(pControlElement->Attribute("group"));
	}
}

void CUIRadioButton::OnPressUp()
{
	GetParentDialog()->ClearRadioButtonGroup(m_uButtonGroup);
	m_bChecked = true;
	SendEvent(EVENT_RADIOBUTTON_CHANGED);
}