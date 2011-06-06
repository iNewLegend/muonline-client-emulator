// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UICombo.h"
#include "UIButton.h"
#include "UIEditBox.h"

class CUIComboNumber : public CUICombo
{
public:
	CUIComboNumber();
	~CUIComboNumber();
public:
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void OnControlRegister();
	virtual void SetText(const wchar_t* wcsText);
	void OnBtnUp();
	void OnBtnDown();
	void OnEditBoxChanged();
	float getFloat();
	float getExtentSize();
	void setFloat(float fValue,const int slen,const int alen);
	void setExtentSize(float fExtentSize);
private:
	CUIButton	m_BtnUp;
	CUIButton	m_BtnDown;
	CUIEditBox	m_EditBox;
	float		m_fExtentSize;
};