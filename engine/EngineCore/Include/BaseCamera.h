#pragma once
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "Common.h"

#define GET_VAR(y,z)				auto get##y()		{ return z; };

class CBaseCamera
{
public:
	CBaseCamera();
	virtual void	FrameMove(float fElapsedTime) = 0;

	// Functions to change camera matrices
	virtual void Reset(); 
	virtual void SetViewParams(Vec3D& vEyePt, Vec3D& vLookatPt);
	virtual void SetProjParams(float fFOV, int nWidth, int nHeight, float fNearPlane, float fFarPlane);
	virtual void SetFarClip(float fFarPlane);
	// Functions to change behavior
	virtual void SetDrag(bool bMovementDrag, float fTotalDragTimeToZero = 0.25f) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
	virtual void SetScalers(float fRotationScaler = 0.01f, float fMoveScaler = 5.0f)  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }

	// Functions to get state
	virtual GSET_CONST_VAR(Matrix&, m_m,ViewMatrix);
	virtual GSET_CONST_VAR(Matrix&, m_m,ProjMatrix);

	virtual GSET_CONST_VAR(Vec3D&, m_v,EyePt);
	virtual GSET_CONST_VAR(Vec3D&, m_v,LookAt);

	virtual GSET_CONST_VAR(float, m_f,NearPlane);
	virtual GSET_CONST_VAR(float, m_f,FarPlane);
	virtual GSET_CONST_VAR(float, m_f,YawAngle);
	virtual GSET_CONST_VAR(float, m_f,PitchAngle);

	virtual void	addMouseDelta(Vec3D vMouseDelta){m_vMouseDelta+=vMouseDelta;}
protected:
	// 更新周转率
	virtual void UpdateVelocity(float fElapsedTime);
	Matrix			m_mViewMatrix;			// View matrix 
	Matrix			m_mProjMatrix;			// Projection matrix

	Vec3D			m_vMouseDelta;			// Mouse relative delta smoothed over a few frames

	Vec3D			m_vDefaultEye;			// Default camera eye position
	Vec3D			m_vDefaultLookAt;		// Default LookAt position
	Vec3D			m_vEyePt;				// Camera eye position
	Vec3D			m_vLookAt;				// LookAt position
	
	float			m_fYawAngle;			// 摄像机（左右）偏航角度
	float			m_fPitchAngle;			// 摄像机（上下）倾斜角度
	
	Vec3D			m_vVelocity;			// 摄像机的周转率

	bool			m_bMovementDrag;		// 是否运动托曳 If true, then camera movement will slow to a stop otherwise movement is instant
	Vec3D			m_vVelocityDrag;		// 托曳速率 Velocity drag force
	float			m_fDragTimer;			// 倒计时去运用托曳 Countdown timer to apply drag
	float			m_fTotalDragTimeToZero; // 托曳总时间 Time it takes for velocity to go from full to 0

	Vec3D			m_vRotVelocity;			// Velocity of camera

	float			m_fFOV;					// Field of view
	int				m_nSceneWidth;
	int				m_nSceneHeight;
	float			m_fAspect;				// Aspect ratio
	float			m_fNearPlane;			// Near plane
	float			m_fFarPlane;			// Far plane

	float			m_fRotationScaler;		// Scaler for rotation
	float			m_fMoveScaler;			// Scaler for movement

	bool			m_bEnableYAxisMovement;	// If true, then camera can move in the y-axis
};