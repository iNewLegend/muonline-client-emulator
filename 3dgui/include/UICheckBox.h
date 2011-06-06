// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIButton.h"

class CUICheckBox : public CUIButton
{
public:
	CUICheckBox();
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual void OnPressUp();
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	bool IsChecked() { return m_bChecked!=0; }
	void SetChecked(bool bChecked) { m_bChecked= bChecked; }
	byte GetCheckValue() { return m_bChecked; }
	void SetCheckValue(byte btValue) { m_bChecked = btValue; }
protected:
	CUIStyle m_StyleChecked;
	CUIStyle m_StyleText;

	byte m_bChecked;
	RECT m_rcButton;
	RECT m_rcText;
};