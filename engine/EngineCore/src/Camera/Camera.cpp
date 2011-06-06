#include "Camera.h"

#define CAMERA_PI 3.14159f
CCamera::CCamera()
{
	m_vTargetPos = Vec3D(0, 0, 0);
	m_fRadius    = 5.0f;
	m_fDefaultRadius = 5.0f;
	m_fMinRadius = 1.0f;
	m_fMaxRadius = 80.0f;
	m_fMinPitchAngle = -CAMERA_PI*0.49f;
	m_fMaxPitchAngle = +CAMERA_PI*0.49f;
}

void CCamera::setTargetPos(const Vec3D& vPos)
{
	m_vTargetPos = vPos;
}

void CCamera::MoveTarget()
{
	// 基于摄像机的 yaw & pitch 创建旋转矩阵
	Matrix mCameraRot;
	mCameraRot.rotationYawPitchRoll(m_fYawAngle, m_fPitchAngle, 0);
	Vec3D vOffset = mCameraRot * Vec3D(-m_vRotVelocity.x, m_vRotVelocity.y, 0);
	vOffset *= m_fRadius* 0.1f;
	m_vTargetPos += vOffset;
	m_vEyePt += vOffset;
}

void CCamera::MoveTargetWithPlane()
{
	// 基于摄像机的 yaw 创建旋转矩阵
	Matrix mCameraRot;
	mCameraRot.rotationYawPitchRoll(m_fYawAngle, 0, 0);
	Vec3D vOffset = mCameraRot * Vec3D(-m_vRotVelocity.x, 0, m_vRotVelocity.y);
	vOffset *= m_fRadius* 0.1f;
	m_vTargetPos += vOffset;
	m_vEyePt += vOffset;
}

void CCamera::MoveAroundTarget()
{
	// Update the pitch & yaw angle based on mouse movement
	{
		m_fYawAngle   += m_vRotVelocity.x;
		m_fPitchAngle += m_vRotVelocity.y;
		// 倾斜角度检测：直上或直下
		m_fPitchAngle = __max(m_fMinPitchAngle,  m_fPitchAngle);
		m_fPitchAngle = __min(m_fMaxPitchAngle,  m_fPitchAngle);
	}

	// 基于摄像机的 yaw & pitch 创建旋转矩阵
	Matrix mCameraRot;
	mCameraRot.rotationYawPitchRoll(m_fYawAngle, m_fPitchAngle, 0);
	//mCameraRot.rotationYawPitchRoll(m_fYawAngle, 0, 0);

	//m_vLookAt  += vPosDeltaWorld;

	// 计算Eye坐标
	Vec3D vViewDir = mCameraRot * Vec3D(0,0,1);
	m_vEyePt = m_vTargetPos - vViewDir * m_fRadius;// + vPosDeltaWorld
}

void CCamera::FrameMove(float fElapsedTime)
{
	// Get amount of velocity based on the keyboard input and drag (if any)
	UpdateVelocity(fElapsedTime);

	// Simple euler method to calculate position delta
	//Vec3D vPosDelta = m_vVelocity * fElapsedTime;

	// Change the radius from the camera to the model based on wheel scrolling
	m_fRadius += m_vRotVelocity.z;//max(m_nMouseWheelDelta * m_fRadius * 0.1f,0.1f);
	m_fRadius = __min(m_fMaxRadius, m_fRadius);
	m_fRadius = __max(m_fMinRadius, m_fRadius);
	{
	//	MoveTarget();
	}
	//if (m_nCurrentButtonMask & MOUSE_RIGHT_BUTTON)
	{
	//	MoveTargetWithPlane();
	}
	//else
	{
		MoveAroundTarget();
	}

	m_vLookAt = m_vTargetPos;
	// 更新视矩阵
	Vec3D vUp(0,1,0);
	m_mViewMatrix.MatrixLookAtLH(m_vEyePt, m_vLookAt, vUp);
	m_Frustum.Build(m_mProjMatrix*m_mViewMatrix);
}

#include "Intersect.h"
void CCamera::GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir, int x, int y,const RECT& rc)
{
	::GetPickRay(vRayPos,vRayDir,x,y,m_mViewMatrix,m_mProjMatrix,rc);
}

void CCamera::World2Screen(const Vec3D& vWorldPos, Pos2D& posScreen)
{
	Matrix mProjXView = this->GetProjXView();
	Vec4D vOut = mProjXView*Vec4D(vWorldPos,1);
	float fW = vOut.w;
	posScreen.x = int(m_nSceneWidth*(0.5f+vOut.x*0.5f/fW));
	posScreen.y = int(m_nSceneHeight*(0.5f-vOut.y*0.5f/fW));
}