//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* UIDisaplyModel.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIDisplay.h"
#include "Camera.h"
#include "RenderNode.h"
#include "SceneEffect.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CUIDisplayModel : public CUIDisplay
{
private:
	iRenderNode*		m_pModelObject;
public:
	Vec3D				m_vEye;
	Vec3D				m_vLookAt;
	CSceneEffect		m_SceneEffect;
	// ----
	CUIDisplayModel();
	~CUIDisplayModel();
	// ----
	void			LoadModel		(const char* szFilename);
	void			OnFrameMove		(double fTime, float fElapsedTime);
	void			OnFrameRender	(const Matrix& mTransform, double fTime, float fElapsedTime);
	// ----
	virtual void	OnMouseMove		(POINT point);
	virtual void	OnMouseWheel	(POINT point, short wheelDelta);
	virtual void	OnLButtonDown	(POINT point);
	virtual void	OnLButtonUp		(POINT point);
	virtual void	OnRButtonDown	(POINT point);
	virtual void	OnRButtonUp		(POINT point);
	virtual void	OnMButtonDown	(POINT point);
	virtual void	OnMButtonUp		(POINT point);
	// ----
	virtual void	OnSize			(const CRect<int> & rc);
	// ----
	iRenderNode*	getModelObject	();
	// ----
	void			SetEye			(const Vec3D& vEye)		{ m_vEye = vEye; }
	const Vec3D		GetEye			()						{ return m_vEye; }
	void			SetLookAt		(const Vec3D& vLookAt)	{ m_vLookAt = vLookAt; }
	const Vec3D		GetLookAt		()						{ return m_vLookAt; }
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------