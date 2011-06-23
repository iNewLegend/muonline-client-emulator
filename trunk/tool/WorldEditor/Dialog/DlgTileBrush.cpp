#include "DlgTileBrush.h"
// For Material
#include "DlgMainEditor.h"
#include "..\MainRoot.h"

CDlgTileBrush::CDlgTileBrush()
{
}

CDlgTileBrush::~CDlgTileBrush()
{	
}

void CDlgTileBrush::OnControlRegister()
{
	CDlgBrush::OnControlRegister();

	RegisterControl( "IDC_OBJLIST_TILES", m_ObjListTiles);
	RegisterControl("IDC_RADIO_BTN_FIRST_LAYER",	m_RadioBtnFirstLayer);

	RegisterControlEvent("IDC_RADIO_BTN_FIRST_LAYER",	(PEVENT)&CDlgTileBrush::OnRadioBtnLayerChanged);
	RegisterControlEvent("IDC_RADIO_BTN_SECOND_LAYER",	(PEVENT)&CDlgTileBrush::OnRadioBtnLayerChanged);

	RegisterEvent("EDIT_TILE_MATERIAL", (PEVENT)&CDlgTileBrush::OnBtnTileEdit);
}

bool CDlgTileBrush::OnInitDialog()
{
	//m_ObjListTiles.m_ListBoxObject.SetSelectedByIndex(0);
	return CDlgBrush::OnInitDialog();
}

void CDlgTileBrush::OnRadioBtnLayerChanged()
{
	WE_BRUSH.SetBrushType(m_RadioBtnFirstLayer.IsChecked()?CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1:CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2);
}

void CDlgTileBrush::initTiles()
{
	//m_ObjListTiles.initObject(WE_SCENE);
}

void CDlgTileBrush::OnBtnTileEdit()
{
	CMainRoot::getInstance().getMainDialog().getMaterialDialog().setMaterial(m_ObjListTiles.getSelectedTileMaterial(),"");
	CMainRoot::getInstance().getMainDialog().getMaterialDialog().SetVisible(true);
}