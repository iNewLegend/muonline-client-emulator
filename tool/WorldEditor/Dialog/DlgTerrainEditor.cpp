#include "DlgTerrainEditor.h"
#include "Terrain.h"
#include "..\MainRoot.h"

CDlgTerrainEditor::CDlgTerrainEditor()
{
}

CDlgTerrainEditor::~CDlgTerrainEditor()
{	
}

void CDlgTerrainEditor::OnControlRegister()
{
	m_dlgTerrainHeightBrush.Create("IDD_TERRAIN_BRUSH_HEIGHT",	this);
	m_dlgTerrainAttributeBrush.Create("IDD_TERRAIN_BRUSH_ATT",	this);
	m_dlgTerrainBrushTile.Create("IDD_TERRAIN_BRUSH_TILE",		this);
	m_dlgTerrainBrushColor.Create("IDD_TERRAIN_BRUSH_COLOR",	this);

	RegisterControlEvent("IDC_BTN_HEIGHT",			(PEVENT)&CDlgTerrainEditor::OnBtnTerrainHeight);
	RegisterControlEvent("IDC_BTN_ATTRIBUTE",		(PEVENT)&CDlgTerrainEditor::OnBtnTerrainAttribute);
	RegisterControlEvent("IDC_BTN_TEXTURE_PAINT",	(PEVENT)&CDlgTerrainEditor::OnBtnTerrainTile);
	RegisterControlEvent("IDC_BTN_COLOR_PAINT",		(PEVENT)&CDlgTerrainEditor::OnBtnTerrainColor);
}

bool CDlgTerrainEditor::OnInitDialog()
{
	OnBtnTerrainHeight();
	return true;
}
void CDlgTerrainEditor::SetVisible(bool bVisible)
{
	CUIDialog::SetVisible(bVisible);
	if (bVisible)
	{
		if (m_dlgTerrainHeightBrush.IsVisible())
		{
			WE_BRUSH.SetBrushType(CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT);
		}
		else if (m_dlgTerrainAttributeBrush.IsVisible())
		{
			WE_BRUSH.SetBrushType(CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT);
		}
		else if (m_dlgTerrainBrushTile.IsVisible())
		{
			m_dlgTerrainBrushTile.OnRadioBtnLayerChanged();
		}
		else if (m_dlgTerrainBrushColor.IsVisible())
		{
			WE_BRUSH.SetBrushType(CTerrainBrush::BRUSH_TYPE_TERRAIN_COLOR);
		}
	}
}

void CDlgTerrainEditor::closeAllChildDialog()
{
	m_dlgTerrainHeightBrush.SetVisible(false);
	m_dlgTerrainAttributeBrush.SetVisible(false);
	m_dlgTerrainBrushTile.SetVisible(false);
	m_dlgTerrainBrushColor.SetVisible(false);
}

void CDlgTerrainEditor::OnBtnTerrainHeight()
{
	WE_BRUSH.SetBrushType(CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT);
	closeAllChildDialog();
	m_dlgTerrainHeightBrush.SetVisible(true);
}

void CDlgTerrainEditor::OnBtnTerrainAttribute()
{
	WE_BRUSH.SetBrushType(CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT);
	closeAllChildDialog();
	m_dlgTerrainAttributeBrush.SetVisible(true);
}

void CDlgTerrainEditor::OnBtnTerrainTile()
{
	m_dlgTerrainBrushTile.OnRadioBtnLayerChanged();
	closeAllChildDialog();
	m_dlgTerrainBrushTile.SetVisible(true);
}

void CDlgTerrainEditor::OnBtnTerrainColor()
{
	WE_BRUSH.SetBrushType(CTerrainBrush::BRUSH_TYPE_TERRAIN_COLOR);
	closeAllChildDialog();
	m_dlgTerrainBrushColor.SetVisible(true);
}

void CDlgTerrainEditor::reset()
{
	m_dlgTerrainBrushTile.initTiles();
}