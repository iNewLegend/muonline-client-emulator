#pragma once
#include "UIDialog.h"

class CDlgBrush :public CUIDialog
{
public:
	CDlgBrush();
	~CDlgBrush();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void SetVisible(bool bVisible);

	void OnSliderBrushRandiusChanged();
	void OnSliderBrushHardnessChanged();
	void OnSliderBrushStrengthChanged();;
private:
	CUISlider	m_SliderBrushRandius;
	CUISlider	m_SliderBrushHardness;
	CUISlider	m_SliderBrushStrength;
};