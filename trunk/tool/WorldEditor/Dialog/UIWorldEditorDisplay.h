#pragma once
#include "UIDisplay.h"
#include "Camera.h"
#include "SkyBox.h"
#include "..\TerrainEditor.h"
#include "SceneEffect.h"
#include "Scene.h"
#include "MeshCoordinate.h"

class CUIWorldEditorDisplay: public CUIDisplay
{
public:
	CUIWorldEditorDisplay();
	~CUIWorldEditorDisplay();
	//
	void SetFog(float fFogStart, float fFogEnd, float fFogDensity, DWORD dwFogColor);
	// Ö¡¸üÐÂ
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	// Ö¡äÖÈ¾
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	//
	void Render();

	CTerrainEditor& getTerrain(){return m_Terrain;}
	CScene*	getScene(){return m_pSceneNode;}
	void	setScene(CScene* pSceneNode){m_pSceneNode = pSceneNode;}
	//CQuake3BSP			g_SceneBsp;
virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(POINT point);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnRButtonDown(POINT point);
	virtual void OnRButtonUp(POINT point);
	virtual void OnMButtonDown(POINT point);
	virtual void OnMButtonUp(POINT point);

	virtual void OnFocusOut();

	const CCamera& GetCamera(){return m_Camera;}

	virtual void OnSize(const CRect<int>& rc);
	//virtual void OnPressed()

	void MoveCamera(int x,int y);

	// Snap
	float getFloorSnap(){return m_fFloorSnap;}
	float getGridSnap(){return m_fGridSnap;}
	void setFloorSnap(float fFloorSnap){m_fFloorSnap=fFloorSnap;}
	void setGridSnap(float fGridSnap){m_fGridSnap=fGridSnap;}
protected:
	POINT				m_ptLastMousePosition;
	CCamera				m_Camera;
	CSkyBox				m_SkyBox;
	CTerrainEditor		m_Terrain;
	CScene*				m_pSceneNode;
	CSceneEffect		m_SceneEffect;
	CMeshCoordinate		m_MeshCoordinate;
	Vec3D				m_vPosPressed;
	Vec3D				m_vPosMoveOn;
	Vec3D				m_vObjectLastPos;
	Vec3D				m_vLastMousePos;

	Vec3D				m_vBeforeCatchPos;
	Vec3D				m_vAfterCatchPos;

	enum COORD_PLAN_TYPE
	{
		CPT_YZ,
		CPT_ZX,
		CPT_XY,
	};
	COORD_PLAN_TYPE		m_CoordPlanType;

	float m_fCoordScale;

	float m_fFloorSnap;
	float m_fGridSnap;
};
