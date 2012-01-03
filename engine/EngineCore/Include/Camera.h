#pragma once
#include "BaseCamera.h"
#include "Frustum.h"
#include "Pos2D.h"
#include <windows.h>

class CCamera  : public CBaseCamera
{
public:
	CCamera();
	virtual const Vec3D& getTargetPos(){return m_vTargetPos;}
	virtual void setTargetPos(const Vec3D& vPos);
	virtual void FrameMove(float fElapsedTime);

	virtual void MoveTarget();
	virtual void MoveTargetWithPlane();
	virtual void MoveAroundTarget();

	virtual Vec3D	getViewDir() const		{ return m_vLookAt - m_vEyePt; }
	virtual Vec3D   getCrossDir() const     { return (m_vLookAt - m_vEyePt).cross(Vec3D(0,1,0));}
	virtual Matrix	getProjXView() const	{ return m_mProjMatrix*m_mViewMatrix; }
	virtual CFrustum& getFrustum()	{ return m_Frustum; }
	virtual int		BoxCrossF(BBox & bbox)	{ return m_Frustum.CheckAABBVisible(bbox); }
public:
	virtual void GetPickRay(Vec3D& pRayPos, Vec3D& pRayDir,int x, int y,const RECT& rc);		// ʰȡ
	virtual void World2Screen(const Vec3D& vWorldPos, Pos2D& posScreen);
	GSET_VAR(float,m_f,Radius);
	GSET_VAR(float,m_f,DefaultRadius);
	GSET_VAR(float,m_f,MinRadius);
	GSET_VAR(float,m_f,MaxRadius);
	GSET_VAR(float,m_f,MinPitchAngle);
	GSET_VAR(float,m_f,MaxPitchAngle);

protected:
	CFrustum	m_Frustum;			// ��׶
	// ��������
	Vec3D		m_vTargetPos;
	// ���룺�����������ľ���
	float		m_fRadius;
	float		m_fDefaultRadius;
	// �������Ŀ���֮����С���뾶
	float		m_fMinRadius,m_fMaxRadius;
	// �������С������£���б�Ƕ�
	float		m_fMinPitchAngle,m_fMaxPitchAngle;
};