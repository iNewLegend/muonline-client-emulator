//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* UIDisaplyRoleChoose.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIDisplay.h"
#include "Camera.h"
#include "RenderNode.h"
#include "SceneEffect.h"
#include "Role.h"
#include "TSingleton.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_VISUAL_ROLE 5
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CUIDisplayRoleChoose : public CUIDisplay, public TSingleton<CUIDisplayRoleChoose>
{
private:
	iRenderNode*		m_pModelObject;
	Matrix				m_mProj;
	Matrix				m_mView;
	DirectionalLight	m_lightDir;
public:
	Vec3D				m_vEye;
	Vec3D				m_vLookAt;
	CSceneEffect		m_SceneEffect;
	CRole*				m_pRole[MAX_VISUAL_ROLE];
	int					m_nSelectIndex;
	// ----
	CUIDisplayRoleChoose();
	~CUIDisplayRoleChoose();
	// ----
	void			LoadModel		(const char * szFilename);
	void			OnFrameMove		(double fTime, float fElapsedTime);
	void			OnFrameRender	(const Matrix & mTransform, double fTime, float fElapsedTime);
	// ----
	virtual void	OnMouseMove		(POINT point);
	virtual void	OnMouseWheel	(POINT point, short wheelDelta);
	virtual void	OnLButtonDown	(POINT point);
	virtual void	OnLButtonUp		(POINT point);
	virtual void	OnRButtonDown	(POINT point);
	virtual void	OnRButtonUp		(POINT point);
	// ----
	virtual void	OnSize			(const CRect<int> & rc);
	// ----
	iRenderNode*	getModelObject	();
	// ----
	void			SetEye			(const Vec3D& vEye)		{ m_vEye = vEye; }
	const Vec3D		GetEye			()						{ return m_vEye; }
	void			SetLookAt		(const Vec3D& vLookAt)	{ m_vLookAt = vLookAt; }
	const Vec3D		GetLookAt		()const					{ return m_vLookAt; }
	// ----
	int				getSelectIndex	()const					{ return m_nSelectIndex; }
	void			setSelectIndex	(int nSelectIndex)		{ m_nSelectIndex = nSelectIndex; }
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------