#include "DlgBrush.h"
#include "DlgToolbar.h"
#include "..\MainRoot.h"

CDlgBrush::CDlgBrush()
{
}

CDlgBrush::~CDlgBrush()
{	
}

void CDlgBrush::OnControlRegister()
{
	CUIDialog::OnControlRegister();

	RegisterControl( "IDC_SLIDER_BRUSH_RANDIUS", m_SliderBrushRandius);
	RegisterControl( "IDC_SLIDER_BRUSH_HARDNESS", m_SliderBrushHardness);
	RegisterControl( "IDC_SLIDER_BRUSH_STRENGTH", m_SliderBrushStrength);
	

	RegisterControlEvent("IDC_SLIDER_BRUSH_RANDIUS",	(PEVENT)&CDlgBrush::OnSliderBrushRandiusChanged);
	RegisterControlEvent("IDC_SLIDER_BRUSH_HARDNESS",	(PEVENT)&CDlgBrush::OnSliderBrushHardnessChanged);
	RegisterControlEvent("IDC_SLIDER_BRUSH_STRENGTH",	(PEVENT)&CDlgBrush::OnSliderBrushStrengthChanged);
}

bool CDlgBrush::OnInitDialog()
{
	m_SliderBrushRandius.SetRange(1,100);
	m_SliderBrushRandius.SetValue(50);
	m_SliderBrushHardness.SetRange(1,100);
	m_SliderBrushHardness.SetValue(50);
	m_SliderBrushStrength.SetRange(1,100);
	m_SliderBrushStrength.SetValue(10);
	return true;
}

void CDlgBrush::SetVisible(bool bVisible)
{
	CUIDialog::SetVisible(bVisible);
	if (bVisible)
	{
		OnSliderBrushRandiusChanged();
		OnSliderBrushHardnessChanged();
		OnSliderBrushStrengthChanged();
	}
}

void CDlgBrush::OnSliderBrushRandiusChanged()
{
	float fRandius = m_SliderBrushRandius.GetValue()/100.0f;
	WE_BRUSH.SetSize(fRandius);
}

void CDlgBrush::OnSliderBrushHardnessChanged()
{
	float fHardness = m_SliderBrushHardness.GetValue()/100.0f;
	WE_BRUSH.SetHardness(fHardness);
}

void CDlgBrush::OnSliderBrushStrengthChanged()
{
	float fStrength = m_SliderBrushStrength.GetValue()/100.0f;
	WE_BRUSH.SetStrength(fStrength);
}