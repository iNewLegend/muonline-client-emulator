#include "UIComboObjListTile.h"
#include "Scene.h"
#include "Terrain.h"
#include "..\TerrainEditor.h"
#include "Graphics.h"

CUIComboObjListTile::CUIComboObjListTile()
{
}

CUIComboObjListTile::~CUIComboObjListTile()
{	
}

void CUIComboObjListTile::OnListBoxObjectSelection()
{
	if (NULL==m_pScene)
	{
		return;
	}
	unsigned char uTileIndex = (unsigned char)m_ListBoxObject.GetSelectedData();
	((CTerrainEditor*)(m_pScene->getTerrainData()))->GetBrushDecal().SetTileID(uTileIndex);
	
	std::string strMaterial = getSelectedTileMaterial();
	if (strMaterial.length()>0)
	{
		CMaterial& material = GetRenderSystem().getMaterialMgr().getItem(strMaterial.c_str());
		m_uTileTexID = material.uTexture[0];
	}
}

void CUIComboObjListTile::initObject(CScene& scene)
{
	m_pScene=&scene;
	m_ListBoxObject.RemoveAllItems();
	static const int nNullTile = 255;
	m_ListBoxObject.AddItem(L"NULL",(LPVOID)nNullTile);
	CTerrain::MAP_TILES& tiles =((CTerrain*)(scene.getTerrainData()))->GetTiles();
	for (CTerrain::MAP_TILES::iterator it=tiles.begin(); it!=tiles.end(); it++)
	{
		m_ListBoxObject.AddItem(s2ws(it->second).c_str(),(LPVOID)it->first);
	}
}

void CUIComboObjListTile::SelectObjectByObjectID(size_t id)
{
	if (NULL==m_pScene)
	{
		return;
	}

	int nIndex = m_ListBoxObject.getItemIndexByData((void*)(id));
	m_ListBoxObject.SelectItem(nIndex);

	std::string strMaterial = getSelectedTileMaterial();
	if (strMaterial.length()>0)
	{
		CMaterial& material = GetRenderSystem().getMaterialMgr().getItem(strMaterial.c_str());
		m_uTileTexID = material.uTexture[0];
	}
}

size_t CUIComboObjListTile::getSelectedObjectID()
{
	return (size_t)m_ListBoxObject.GetSelectedData();
}

void CUIComboObjListTile::OnObjectListReload()
{
	//std::string strObjectListFilename = getDisplay().getScene().getObjectListFilename();
	//getDisplay().getScene().loadObjectList(strObjectListFilename,strObjectListFilename);
	//initObject();
	//std::string strTileListFilename = getDisplay().getTerrain()().getTileListFilename();
	////getDisplay().getTerrain()().LoadTiles(strTileListFilename);
	//initTiles();
}

void CUIComboObjListTile::OnObjectListEdit()
{
	if (GetParentDialog())
	{
		GetParentDialog()->postMsg("EDIT_TILE_MATERIAL");
	}
	//std::string strObjectListFilename = getDisplay().getScene().getObjectListFilename();
	//ShellExecuteW(NULL,L"open",s2ws(strObjectListFilename).c_str(),NULL,NULL,SW_SHOWNORMAL); 
	//std::string strTileListFilename = getDisplay().getTerrain()().getTileListFilename();
	//ShellExecuteW(NULL,L"open",s2ws(strTileListFilename).c_str(),NULL,NULL,SW_SHOWNORMAL); 
}

void CUIComboObjListTile::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIComboObjList::OnFrameMove(fTime, fElapsedTime);
}

void CUIComboObjListTile::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIComboObjList::OnFrameRender(mTransform,fTime, fElapsedTime);
	GetGraphics().DrawTex(m_ListBoxDisplay.GetBoundingBox().getRECT(),m_uTileTexID,0xFFFFFFFF);
}

std::string CUIComboObjListTile::getSelectedTileMaterial()
{
	CTerrain::MAP_TILES& tiles =((CTerrain*)(m_pScene->getTerrainData()))->GetTiles();
	if (tiles.find(getSelectedObjectID())!=tiles.end())
	{
		return tiles[getSelectedObjectID()];
	}
	return "";
}