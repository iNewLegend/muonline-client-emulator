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

class CUIDisplayRoleList : public CUIDisplay, public TSingleton<CUIDisplayRoleList>
{
public:
	Vec3D				m_vEye;
	Vec3D				m_vLookAt;
	// ----
	CUIDisplayRoleList();
	~CUIDisplayRoleList();
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
	GSET_VAR		(int, m_n, SelectIndex);
	void			clearAllRole	();
	CRole*			getRole			(int nIndex);
private:
	int					m_nSelectIndex;
	CRole*				m_pRole[MAX_VISUAL_ROLE];
	iRenderNode*		m_pModelObject;
	Matrix				m_mProj;
	Matrix				m_mView;
	DirectionalLight	m_lightDir;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------