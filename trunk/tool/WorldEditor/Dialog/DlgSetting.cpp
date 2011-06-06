#include "DlgSetting.h"
#include "DlgToolbar.h"
#include "..\MainRoot.h"

CDlgSetting::CDlgSetting()
{
}

CDlgSetting::~CDlgSetting()
{	
}

void CDlgSetting::OnControlRegister()
{
	CUIDialog::OnControlRegister();

	RegisterControl("IDC_EDITBOX_TERRAIN_WIDTH", m_EditBoxWidth);
	RegisterControl("IDC_EDITBOX_TERRAIN_HEIGHT", m_EditBoxHeight);
	RegisterControl("IDC_CHECKBOX_ANIM_OBJECT", m_CheckBoxAnimObject);

	RegisterControl("IDC_NUM_CAMERA_ANGLE",	m_NumCameraAngle);
	RegisterControl("IDC_NUM_CAMERA_FAR",	m_NumCameraFar);
	RegisterControl("IDC_SLIDER_FOG_START",	m_SliderFogStart);
	RegisterControl("IDC_SLIDER_FOG_END",	m_SliderFogEnd);
	RegisterControl("IDC_NUM_FOG_DENSITY",	m_NumFogDensity);

	RegisterControl("IDC_COLOR_FOG",		m_ColorFog);
	RegisterControl("IDC_COLOR_AMBIENT",	m_ColorAmbient);
	RegisterControl("IDC_COLOR_DIFFUSE",	m_ColorDiffuse);


	RegisterControlEvent("IDC_CHECKBOX_TERRAIN_LAYER1",	(PEVENT)&CDlgSetting::OnBtnTerrainLayer0Visible);
	RegisterControlEvent("IDC_CHECKBOX_TERRAIN_LAYER2",	(PEVENT)&CDlgSetting::OnBtnTerrainLayer1Visible);
	RegisterControlEvent("IDC_CHECKBOX_TERRAIN_GRID",	(PEVENT)&CDlgSetting::OnBtnTerrainGridVisible);
	RegisterControlEvent("IDC_CHECKBOX_TERRAIN_BBOX",	(PEVENT)&CDlgSetting::OnBtnTerrainBBoxVisible);
	RegisterControlEvent("IDC_CHECKBOX_STATIC_OBJECT",	(PEVENT)&CDlgSetting::OnBtnObjectVisible);
	RegisterControlEvent("IDC_CHECKBOX_OBJECT_BBOX",	(PEVENT)&CDlgSetting::OnBtnObjectBBoxVisible);

	RegisterControlEvent("IDC_BTN_TERRAIN_RESIZE",		(PEVENT)&CDlgSetting::OnBtnTerrainResize);

	RegisterControlEvent("IDC_BTN_LIGHTMAP",			(PEVENT)&CDlgSetting::OnBtnCalcLightMap);

	RegisterControlEvent("IDC_NUM_CAMERA_ANGLE",	(PEVENT)&CDlgSetting::OnCameraChanged);
	RegisterControlEvent("IDC_NUM_CAMERA_FAR",		(PEVENT)&CDlgSetting::OnCameraChanged);
	RegisterControlEvent("IDC_SLIDER_FOG_START",	(PEVENT)&CDlgSetting::OnFogChanged);
	RegisterControlEvent("IDC_SLIDER_FOG_END",		(PEVENT)&CDlgSetting::OnFogChanged);
	RegisterControlEvent("IDC_NUM_FOG_DENSITY",		(PEVENT)&CDlgSetting::OnFogChanged);
	RegisterControlEvent("IDC_COLOR_FOG",			(PEVENT)&CDlgSetting::OnFogChanged);
	RegisterControlEvent("IDC_COLOR_AMBIENT",		(PEVENT)&CDlgSetting::OnMaterialChanged);
	RegisterControlEvent("IDC_COLOR_DIFFUSE",		(PEVENT)&CDlgSetting::OnMaterialChanged);
}

bool CDlgSetting::OnInitDialog()
{
	//WE_TERRAIN.showLayer0(!WE_TERRAIN.isShowLayer0());
	//WE_TERRAIN.showLayer1(!WE_TERRAIN.isShowLayer1());
	//WE_TERRAIN.showGrid(!WE_TERRAIN.isShowGrid());
	//WE_TERRAIN.ShowBox(!WE_TERRAIN.IsShowBox());
	m_SliderFogStart.SetRange(0,100);
	m_SliderFogStart.SetPageSize(1);
	m_SliderFogEnd.SetRange(0,255);
	m_SliderFogEnd.SetPageSize(1);
	return true;
}

void CDlgSetting::SetVisible(bool bVisible)
{
	CUIDialog::SetVisible(bVisible);
	if (bVisible)
	{
		m_EditBoxWidth.SetInt(WE_TERRAIN.GetWidth());
		m_EditBoxHeight.SetInt(WE_TERRAIN.GetHeight());
	}
}

void CDlgSetting::OnBtnTerrainLayer0Visible()
{
	WE_TERRAIN.showLayer0(!WE_TERRAIN.isShowLayer0());
}

void CDlgSetting::OnBtnTerrainLayer1Visible()
{
	WE_TERRAIN.showLayer1(!WE_TERRAIN.isShowLayer1());
}

void CDlgSetting::OnBtnTerrainGridVisible()
{
	WE_TERRAIN.showGrid(!WE_TERRAIN.isShowGrid());
}

void CDlgSetting::OnBtnTerrainBBoxVisible()
{
	WE_TERRAIN.ShowBox(!WE_TERRAIN.IsShowBox());
}

void CDlgSetting::OnBtnObjectVisible()
{
	WE_SCENE.setShowObject(!WE_SCENE.getShowObject());
}

void CDlgSetting::OnBtnObjectBBoxVisible()
{
	WE_SCENE.setShowObjectBBox(!WE_SCENE.getShowObjectBBox());
}

void CDlgSetting::OnBtnTerrainResize()
{
	WE_TERRAIN.resize(m_EditBoxWidth.GetInt(),m_EditBoxHeight.GetInt(),0);
}

void CDlgSetting::OnBtnCalcLightMap()
{
	WE_SCENE.CalcLightMap();
}

void CDlgSetting::OnCameraChanged()
{
}

void CDlgSetting::OnFogChanged()
{
	Fog fog;
	fog.fStart		= (float)m_SliderFogEnd.GetValue()*0.01f*m_SliderFogStart.GetValue();
	fog.fEnd		= (float)m_SliderFogEnd.GetValue();
	fog.fDensity	= m_NumFogDensity.getFloat();
	fog.color		= m_ColorFog.getColor();
	WE_SCENE.setFog(fog);
}

void CDlgSetting::OnMaterialChanged()
{
	DirectionalLight light;
	light.vAmbient = Vec4D(m_ColorAmbient.getColor());
	light.vDiffuse = Vec4D(m_ColorDiffuse.getColor());
	light.vSpecular = Vec4D(1.0f,1.0f,1.0f,1.0f);
	light.vDirection = Vec3D(-1.0f,-1.0f,1.0f);
	WE_SCENE.setLight(light);
}

void CDlgSetting::init()
{
	const Fog& fog = WE_SCENE.getFog();
	m_SliderFogStart.SetValue((int)(fog.fStart*100/fog.fEnd));
	m_SliderFogEnd.SetValue((int)fog.fEnd);
	m_NumFogDensity.setFloat(fog.fDensity,0,2);
	m_ColorFog.setColor(fog.color);

	const DirectionalLight& light = WE_SCENE.getLight();
	m_ColorAmbient.setColor(light.vAmbient);
	m_ColorDiffuse.setColor(light.vDiffuse);
}