#include "DlgBrushAtt.h"
#include "..\MainRoot.h"

CDlgBrushAtt::CDlgBrushAtt()
{
}

CDlgBrushAtt::~CDlgBrushAtt()
{	
}

void CDlgBrushAtt::OnControlRegister()
{
	CDlgBrush::OnControlRegister();

	RegisterControl("IDC_RADIO_BTN_SAFE",	m_RadioBtnSafe);
	RegisterControl("IDC_RADIO_BTN_BALK",	m_RadioBtnBalk);
	RegisterControl("IDC_RADIO_BTN_CLEAR",	m_RadioBtnClear);

	RegisterControlEvent("IDC_RADIO_BTN_SAFE",	(PEVENT)&CDlgBrushAtt::OnRadioBtnAttChanged);
	RegisterControlEvent("IDC_RADIO_BTN_BALK",	(PEVENT)&CDlgBrushAtt::OnRadioBtnAttChanged);
	RegisterControlEvent("IDC_RADIO_BTN_CLEAR",	(PEVENT)&CDlgBrushAtt::OnRadioBtnAttChanged);
}

bool CDlgBrushAtt::OnInitDialog()
{
	//m_ListBoxTiles.SetSelectedByIndex(0);
	return CDlgBrush::OnInitDialog();
}

void CDlgBrushAtt::SetVisible(bool bVisible)
{
	WE_TERRAIN.showAttribute(bVisible);
	CDlgBrush::SetVisible(bVisible);
}

void CDlgBrushAtt::OnRadioBtnAttChanged()
{
	unsigned char uAtt= 0;

	if (m_RadioBtnSafe.IsChecked())
	{
		uAtt |= TERRAIN_ATT_TYPE_SAFE;
	}
	if (m_RadioBtnBalk.IsChecked())
	{
		uAtt |= TERRAIN_ATT_TYPE_BALK;
	}
	if (m_RadioBtnClear.IsChecked())
	{
		uAtt |= TERRAIN_ATT_TYPE_CLEAR;
	}
	WE_BRUSH.SetAtt(uAtt);
}