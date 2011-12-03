#include "MSimpleVehicle2.h"


void MSimpleVehicle2::ProcMove(rvector* pNewPos, rvector* pNewDir, float* pNewVelocity, rvector& Velocity)
{
	if(Velocity.x==0.0f && Velocity.y==0.0f && Velocity.z==0.0f) return;	// ���� ��ġ�� ����

	// ȸ��
	/////////////////////////////////////////////////////////////
	float fDiffAngle = GetAngleOfVectors(Velocity, m_Dir);
	float fRotateAngle = fDiffAngle;

	float fRotatePercent = 1.0f;

	m_fAngularVelocity = min(m_Property.fMaxRotationVelocity, m_fAngularVelocity + m_Property.fRotationAcceleration);	// Angular Acceleration
#define ANGULAR_DECCELRATION_CONTANT	2.0f
	if(fabs(fDiffAngle)<m_Property.fMaxRotationVelocity*ANGULAR_DECCELRATION_CONTANT){
		m_fAngularVelocity = min(m_fAngularVelocity, m_Property.fMaxRotationVelocity*ANGULAR_DECCELRATION_CONTANT/(ANGULAR_DECCELRATION_CONTANT+1));
	}

	if(fabs(fDiffAngle)>m_fAngularVelocity){	// ������ ������� �Ҵ�� RotationDegree��ŭ ȸ���� ���ϱ�
		if(fDiffAngle>=0) fRotateAngle = m_fAngularVelocity;
		else fRotateAngle = -m_fAngularVelocity;
	}

	if(fDiffAngle!=0.0f) fRotatePercent = fRotateAngle/fDiffAngle;

	rvector NewDir = Normalize(InterpolatedVector(m_Dir, Velocity, fRotatePercent));
	//rvector NewDir = Normalize(MInterpolateVector(m_Dir, Velocity, fRotatePercent));
	// InterpolatedVector()�� ���ݴ��� �ΰ��϶� ���װ� �ִ�.

	// ��� ��
	//m_fResultRotatePercent = fRotateAngle / m_Property.fMaxRotationVelocity;
	m_fResultRotatePercent = GetAngleOfVectors(NewDir, m_Dir) / m_Property.fMaxRotationVelocity;	// ���׷� ���� ���� ��갪�� �Է�

	// ���� & �̵�
	////////////////////////////////////////////////////////////
	float fDesireVelocity = Velocity.GetSafeMagnitude();
	float fNewVelocity = fDesireVelocity;
	if(fDesireVelocity>m_Property.fMaxVelocity)
		fNewVelocity = min(m_Property.fMaxVelocity, m_fVelocity + m_Property.fAcceleration); // ���� ���� �ӵ� ���

#define DIRECTIONAL_SPEED_CONSTANT	1.0f
	if(fNewVelocity>=0.0f){
		if(m_bDirectionSpeed==true){	// ���⿡ ���� ������
			fNewVelocity -= (fNewVelocity*(1.0f-fRotatePercent)*DIRECTIONAL_SPEED_CONSTANT);
			//fNewVelocity -= (fNewVelocity*(m_fAngularVelocity/m_Property.fMaxRotationVelocity)*DIRECTIONAL_SPEED_CONSTANT);
		}
	}

	// New Values
	*pNewVelocity = fNewVelocity;
	*pNewDir = NewDir;
	*pNewPos = m_Pos+(fNewVelocity*NewDir);
}

MSimpleVehicle2::MSimpleVehicle2(bool bDirectionSpeed)
: m_Pos(0, 0, 0), m_Dir(1, 0, 0), m_fVelocity(0), m_fAngularVelocity(0), m_bDirectionSpeed(bDirectionSpeed)
{
}

MSimpleVehicle2::~MSimpleVehicle2(void)
{
}

void MSimpleVehicle2::Move(rvector& Dir, MCollisionMap2* pCM)
{
	// �̵�
	rvector NewPos, NewDir;
	float fNewVelocity;
	ProcMove(&NewPos, &NewDir, &fNewVelocity, Dir);

	// �̵��� ���� �浹 üũ
	rvector RecommendedPos;
#define ESTIMATE_CONSTANT	4
	if(pCM!=NULL && pCM->IsCollision(&RecommendedPos, m_Pos+NewDir*fNewVelocity*ESTIMATE_CONSTANT)==true){
		ProcMove(&NewPos, &NewDir, &fNewVelocity, RecommendedPos-m_Pos);
		fNewVelocity = fNewVelocity * ESTIMATE_CONSTANT / (ESTIMATE_CONSTANT+1);
	}
		/*
		float t = 1.0f;
	#define EXPECT_CONSTANT	10.0f
		if(pCM!=NULL && pCM->IsIntersect(&t, m_Pos, NewDir, fNewVelocity*EXPECT_CONSTANT)==true && (t>0.0f && t<=1.0f)){
			//fNewVelocity = fNewVelocity * t * fRotatePercent / EXPECT_CONSTANT;		// �����ϰ� üũ�ϴ� ��� ( �𼭸� �κп��� ���� �� �ִ�. ��.�� )
			fNewVelocity = fNewVelocity * fRotatePercent * fRotatePercent;				// ��ħ�� ��������� �ε巴�� üũ ( �浹 ���ɽÿ� ���⿡ ����ؼ� �ӵ��� ���� )
			//fNewVelocity = fNewVelocity * max(t, 0.2f) * fRotatePercent / EXPECT_CONSTANT;		// �����ϰ� üũ�ϴ� ��� ( �𼭸� �κп��� ���� �� �ִ�. ��.�� )
		}
		*/

	m_fVelocity = fNewVelocity;
	m_Dir = NewDir;
	m_Pos = NewPos;
}

void MSimpleVehicle2::SetMaxVelocity(float v)
{
	m_Property.fMaxVelocity = v;
}

void MSimpleVehicle2::SetAcceleration(float v)
{
	m_Property.fAcceleration = v;
}

void MSimpleVehicle2::SetMaxRotationVelocity(float v)
{
	m_Property.fMaxRotationVelocity = v;
}

void MSimpleVehicle2::SetRotationAcceleration(float v)
{
	m_Property.fRotationAcceleration = v;
}

float MSimpleVehicle2::AdjustMaxVeloctiy(float fDistance, float fDecConstant)		// �Ÿ��� ���� Max Velocity ����
{
	m_Property.fMaxVelocity = min(m_Property.fMaxVelocity, m_Property.fMaxVelocity * fDistance / fDecConstant);
	return m_Property.fMaxVelocity;
}
