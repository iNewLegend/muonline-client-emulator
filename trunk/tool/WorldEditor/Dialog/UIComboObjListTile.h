#pragma once
#include "UIComboObjList.h"

class CUIComboObjListTile : public CUIComboObjList
{
public:
	CUIComboObjListTile();
	~CUIComboObjListTile();
public:
	void initObject(CScene& scene);
	void SelectObjectByObjectID(size_t id);
	size_t getSelectedObjectID();


	void OnListBoxObjectSelection();
	void OnObjectListReload();
	void OnObjectListEdit();
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	std::string getSelectedTileMaterial();
public:
	size_t	m_uTileTexID;
};