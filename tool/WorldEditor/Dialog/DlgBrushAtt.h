#pragma once
#include "DlgBrush.h"

class CDlgBrushAtt : public CDlgBrush
{
public:
	CDlgBrushAtt();
	~CDlgBrushAtt();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void SetVisible(bool bVisible);

	void OnRadioBtnAttChanged();
private:
	CUICheckBox m_RadioBtnSafe;
	CUICheckBox m_RadioBtnBalk;
	CUICheckBox m_RadioBtnClear;
};