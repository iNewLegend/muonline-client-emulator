#pragma once
#include "DlgBrush.h"
#include "UIComboNumber.h"

class CDlgBrushHeight : public CDlgBrush
{
public:
	CDlgBrushHeight();
	~CDlgBrushHeight();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();

	void OnHeightRangChanged();
	void OnBrushHeightTypeChanged();
private:
	CUIComboNumber m_NumMin;
	CUIComboNumber m_NumMax;

	CUIRadioButton	m_RadioBtnNoraml;
	CUIRadioButton	m_RadioBtnSmooth;
};