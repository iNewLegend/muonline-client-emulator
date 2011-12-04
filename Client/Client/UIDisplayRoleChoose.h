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
public:
	Vec3D				m_vEye;
	Vec3D				m_vLookAt;
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
	virtual void	OnLButtonDown	(POINT point);
	// ----
	void			SetEye			(const Vec3D& vEye)		{ m_vEye = vEye; }
	const Vec3D		GetEye			()						{ return m_vEye; }
	void			SetLookAt		(const Vec3D& vLookAt)	{ m_vLookAt = vLookAt; }
	const Vec3D		GetLookAt		()const					{ return m_vLookAt; }
	// ----
	int				getSelectIndex	()const					{ return m_nSelectIndex; }
	void			setSelectIndex	(int nSelectIndex)		{ m_nSelectIndex = nSelectIndex; }
private:
	iRenderNode*		m_pModelObject;
	Matrix				m_mProj;
	Matrix				m_mView;
	DirectionalLight	m_lightDir;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------