#pragma once
#include "DlgBrushAtt.h"
#include "DlgBrushHeight.h"
#include "DlgTileBrush.h"
#include "DlgBrushColor.h"

class CDlgTerrainEditor :public CUIDialog
{
public:
	CDlgTerrainEditor();
	~CDlgTerrainEditor();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void SetVisible(bool bVisible);

	void OnBtnTerrainHeight();
	void OnBtnTerrainAttribute();
	void OnBtnTerrainTile();
	void OnBtnTerrainColor();

	void OnHeightRangChanged();
	void OnBrushHeightTypeChanged();
	void reset();
private:
	void closeAllChildDialog();

	CDlgBrushHeight	m_dlgTerrainHeightBrush;
	CDlgBrushAtt	m_dlgTerrainAttributeBrush;
	CDlgTileBrush	m_dlgTerrainBrushTile;
	CDlgBrushColor	m_dlgTerrainBrushColor;
};