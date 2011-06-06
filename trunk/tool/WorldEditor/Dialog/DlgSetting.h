#pragma once
#include "UIDialog.h"

class CDlgSetting :public CUIDialog
{
public:
	CDlgSetting();
	~CDlgSetting();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void SetVisible(bool bVisible);
	void OnBtnTerrainLayer0Visible();
	void OnBtnTerrainLayer1Visible();
	void OnBtnTerrainGridVisible();
	void OnBtnTerrainBBoxVisible();
	void OnBtnObjectVisible();
	void OnBtnObjectBBoxVisible();
	void OnBtnTerrainResize();
	void OnBtnCalcLightMap();


	void OnCameraChanged();
	void OnFogChanged();
	void OnMaterialChanged();

	void init();
private:
	CUIEditBox m_EditBoxWidth;
	CUIEditBox m_EditBoxHeight;
	CUICheckBox m_CheckBoxAnimObject;

	CUIComboNumber	m_NumCameraAngle;
	CUIComboNumber	m_NumCameraFar;
	CUISlider		m_SliderFogStart;
	CUISlider		m_SliderFogEnd;
	CUIComboNumber	m_NumFogStart;
	CUIComboNumber	m_NumFogEnd;
	CUIComboNumber	m_NumFogDensity;

	CUIComboColor	m_ColorFog;
	CUIComboColor	m_ColorAmbient;
	CUIComboColor	m_ColorDiffuse;
};