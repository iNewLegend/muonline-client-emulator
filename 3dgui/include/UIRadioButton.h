// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UICheckBox.h"

//-----------------------------------------------------------------------------
// RadioButton control
//-----------------------------------------------------------------------------
class CUIRadioButton : public CUICheckBox
{
public:
	CUIRadioButton();
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void OnPressUp();
	void SetChecked(bool bChecked) {m_bChecked = bChecked;}
	void SetButtonGroup(unsigned int uButtonGroup) { m_uButtonGroup = uButtonGroup; }
	unsigned int GetButtonGroup() { return m_uButtonGroup; }
protected:
	unsigned int m_uButtonGroup;
};