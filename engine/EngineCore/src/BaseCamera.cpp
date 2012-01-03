#include "BaseCamera.h"

CBaseCamera::CBaseCamera()
{
    // Set attributes for the view matrix
    Vec3D vEyePt    = Vec3D(0.0f,0.0f,0.0f);
    Vec3D vLookatPt = Vec3D(0.0f,0.0f,1.0f);

    // Setup the view matrix
    setViewParams(vEyePt, vLookatPt);

    // Setup the projection matrix
    setProjParams(3.14159f/4, 800, 600, 1.0f, 1000.0f);

    m_fYawAngle = 0.0f;
    m_fPitchAngle = 0.0f;

    m_vVelocity     = Vec3D(0.0f,0.0f,0.0f);
    m_bMovementDrag = true;
    m_vVelocityDrag = Vec3D(0.0f,0.0f,0.0f);
    m_fDragTimer    = 0.0f;
    m_fTotalDragTimeToZero = 1.25;
    m_vRotVelocity = Vec3D(0.0f,0.0f,0.0f);

    m_fRotationScaler = 0.01f;           
    m_fMoveScaler = 5.0f;           

    m_vMouseDelta   = Vec3D(0.0f,0.0f,0.0f);
}


void CBaseCamera::setViewParams(Vec3D& vEyePt, Vec3D& vLookatPt)
{
    m_vDefaultEye = m_vEyePt = vEyePt;
    m_vDefaultLookAt = m_vLookAt = vLookatPt;

    // Calc the view matrix
    Vec3D vUp(0,1,0);
    m_mViewMatrix.MatrixLookAtLH(vEyePt, vLookatPt, vUp);

    Matrix mInvView = m_mViewMatrix;
	mInvView.Invert();

    // The axis basis vectors and camera position are stored inside the 
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector
	// [badcode]
    Vec3D* pZBasis = (Vec3D*) &mInvView.m[2][0];

    m_fYawAngle   = atan2f(pZBasis->x, pZBasis->z);
    float fLen = sqrtf(pZBasis->z*pZBasis->z + pZBasis->x*pZBasis->x);
    m_fPitchAngle = -atan2f(pZBasis->y, fLen);
}

void CBaseCamera::setProjParams(float fFOV, int nWidth, int nHeight, float fNearPlane, float fFarPlane)
{
    // Set attributes for the projection matrix
    m_fFOV			= fFOV;
	m_nSceneWidth	= nWidth;
	m_nSceneHeight	= nHeight;
    m_fAspect		= nWidth / (float)nHeight;
    m_fNearPlane	= fNearPlane;
    m_fFarPlane		= fFarPlane;

	m_mProjMatrix.MatrixPerspectiveFovLH(fFOV, m_fAspect, fNearPlane, fFarPlane);
}

void CBaseCamera::setFarClip(float fFarPlane)
{
	m_fFarPlane   = fFarPlane;
	m_mProjMatrix.MatrixPerspectiveFovLH(m_fFOV, m_fAspect, m_fNearPlane, fFarPlane);
}

void CBaseCamera::updateVelocity(float fElapsedTime)
{
    Matrix mRotDelta;
    m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;
	m_vMouseDelta=Vec3D(0.0f,0.0f,0.0f);

    Vec3D vAccel;//

    // Normalize vector so if moving 2 dirs (left & forward), 
    // the camera doesn't move faster than if moving in 1 dir
	vAccel.normalize();

    // 加速度尺量的尺度
    vAccel *= m_fMoveScaler;

	// 是否拖曳
    if(m_bMovementDrag)
    {
        // Is there any acceleration this frame?
        if(vAccel.lengthSquared() > 0)
        {
            // If so, then this means the user has pressed a movement key\
            // so change the velocity immediately to acceleration 
            // upon keyboard input.  This isn't normal physics
            // but it will give a quick response to keyboard input
            m_vVelocity = vAccel;
            m_fDragTimer = m_fTotalDragTimeToZero;
            m_vVelocityDrag = vAccel / m_fDragTimer;
        }
        else 
        {
            // If no key being pressed, then slowly decrease velocity to 0
            if(m_fDragTimer > 0)
            {
                // Drag until timer is <= 0
                m_vVelocity -= m_vVelocityDrag * fElapsedTime;
                m_fDragTimer -= fElapsedTime;
            }
            else
            {
                // Zero velocity
                m_vVelocity = Vec3D(0.0f,0.0f,0.0f);
            }
        }
    }
    else
    {
        // 没有拖动 所以立即改变周转率
        m_vVelocity = vAccel;
    }
}

void CBaseCamera::reset()
{
    setViewParams(m_vDefaultEye, m_vDefaultLookAt);
}



