//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* UIDisaplyModel.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIDisplay.h"
#include "Camera.h"
#include "RenderNode.h"
#include "SceneEffect.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CUIDisplayRenderNode : public CUIDisplay
{
public:
	// ----
	CUIDisplayRenderNode();
	~CUIDisplayRenderNode();
	// ----
	void			LoadModel		(const char* szFilename);
	void			OnFrameMove		(double fTime, float fElapsedTime);
	void			OnFrameRender	(const Matrix& mTransform, double fTime, float fElapsedTime);
	// ----
	virtual void	OnSize			(const CRect<int> & rc);
	// ----
	CRenderNode*	getRenderNode	(){return (CRenderNode*)m_pRenderNode;}
	void			setRenderNode	(CRenderNode* pRenderNode){m_pRenderNode = pRenderNode;}
	// ----
	void			SetEye			(const Vec3D& vEye)		{ m_vEye = vEye; }
	const Vec3D		GetEye			()						{ return m_vEye; }
	void			SetLookAt		(const Vec3D& vLookAt)	{ m_vLookAt = vLookAt; }
	const Vec3D		GetLookAt		()						{ return m_vLookAt; }
public:
	Matrix			m_mView;
	Matrix			m_mProj;
	Vec3D			m_vEye;
	Vec3D			m_vLookAt;
	CSceneEffect	m_SceneEffect;
protected:
	iRenderNode*	m_pRenderNode;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------