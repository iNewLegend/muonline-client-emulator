#include "UIComboColor.h"

CUIComboColor::CUIComboColor()
{
}

CUIComboColor::~CUIComboColor()
{	
}

void CUIComboColor::OnControlRegister()
{
	CUICombo::OnControlRegister();

	RegisterControl("IDC_SLIDER_R",m_SliderR);
	RegisterControl("IDC_SLIDER_G",m_SliderG);
	RegisterControl("IDC_SLIDER_B",m_SliderB);

	RegisterControlEvent("IDC_SLIDER_R",(PEVENT)&CUIComboColor::OnSliderChanged);
	RegisterControlEvent("IDC_SLIDER_G",(PEVENT)&CUIComboColor::OnSliderChanged);
	RegisterControlEvent("IDC_SLIDER_B",(PEVENT)&CUIComboColor::OnSliderChanged);

	m_SliderR.SetRange(0,255);
	m_SliderG.SetRange(0,255);
	m_SliderB.SetRange(0,255);
}

Color32 CUIComboColor::getColor()
{
	Color32 color;
	color.a=255;
	color.r=m_SliderR.GetValue();
	color.g=m_SliderG.GetValue();
	color.b=m_SliderB.GetValue();
	return color;
}

void CUIComboColor::setColor(Color32 color)
{
	m_SliderR.SetValue(color.r);
	m_SliderG.SetValue(color.g);
	m_SliderB.SetValue(color.b);
}

//Vec4D CUIComboColor::getColor()
//{
//	Vec4D vColor;
//	vColor.w=1.0f;
//	vColor.x=m_SliderR.GetValue()/255.0f;
//	vColor.y=m_SliderG.GetValue()/255.0f;
//	vColor.z=m_SliderB.GetValue()/255.0f;
//	return vColor;
//}

void CUIComboColor::setColor(const Vec4D& vColor)
{
	m_SliderR.SetValue((int)(vColor.x*255.0f));
	m_SliderG.SetValue((int)(vColor.y*255.0f));
	m_SliderB.SetValue((int)(vColor.z*255.0f));
}

void CUIComboColor::OnSliderChanged()
{
	SendEvent(EVENT_DEFAULT);
}
