#pragma once
#include "DlgBrush.h"
#include "UIComboObjListTile.h"

class CDlgTileBrush : public CDlgBrush
{
public:
	CDlgTileBrush();
	~CDlgTileBrush();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();

	void OnRadioBtnLayerChanged();
	void initTiles();
	void OnBtnTileEdit();
private:
	CUIComboObjListTile m_ObjListTiles;
	CUIRadioButton	m_RadioBtnFirstLayer;
};