#include "DlgBrushColor.h"
#include "Terrain.h"
#include "..\MainRoot.h"

CDlgBrushColor::CDlgBrushColor()
{
}

CDlgBrushColor::~CDlgBrushColor()
{	
}

void CDlgBrushColor::OnControlRegister()
{
	CDlgBrush::OnControlRegister();

	RegisterControl("IDC_COLOR_PAINT",	m_ColorPaint);

	RegisterControlEvent("IDC_COLOR_PAINT",	(PEVENT)&CDlgBrushColor::OnColorPaintChanged);
}

bool CDlgBrushColor::OnInitDialog()
{
	return CDlgBrush::OnInitDialog();
}

void CDlgBrushColor::SetVisible(bool bVisible)
{
	CDlgBrush::SetVisible(bVisible);
}

void CDlgBrushColor::OnColorPaintChanged()
{
	WE_BRUSH.setColor(m_ColorPaint.getColor());
}