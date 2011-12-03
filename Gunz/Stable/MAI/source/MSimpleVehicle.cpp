#include "MSimpleVehicle.h"
#include "MRayCastingDriver.h"

MRCDCollisionMap::MRCDCollisionMap(void)
{
	m_pRCD = NULL;
}

void MRCDCollisionMap::Initialize(void)
{
	m_pRCD = NULL;
	m_Obstacles.DeleteAll();
}

void MRCDCollisionMap::Add(MObstacle* pObstacle)
{
	m_Obstacles.Add(pObstacle);
}

void MRCDCollisionMap::Share(MRayCastingDriver* pRCD)
{
	m_pRCD = pRCD;
}

bool MRCDCollisionMap::IsIntersect(CMPtrList<MObstacle>* pObstacles, float* t, rvector& Pos, rvector& Dir, float fMagnitude)
{
	MObstacle* pColObstacle = NULL;
	for(int i=0; i<pObstacles->GetCount(); i++){
		MObstacle* pObstacle = pObstacles->Get(i);
		rvector p;
		if(pObstacle->IsCollision(&p, Pos)==true) continue;
		if(pObstacle->IsIntersect(t, Pos, Dir, fMagnitude)==true && (*t>=0.0f && *t<=1.0f)){
			fMagnitude *= *t;
			pColObstacle = pObstacle;
		}
	}

	if(pColObstacle!=NULL) return true;
	return false;
}

bool MRCDCollisionMap::IsIntersect(float* t, rvector& Pos, rvector& Dir, float fMagnitude)
{
	bool bIntersect = false;

	if(IsIntersect(&m_Obstacles, t, Pos, Dir, fMagnitude)==true){
		bIntersect = true;
		fMagnitude *= *t;
	}

	if(m_pRCD!=NULL && IsIntersect(&(m_pRCD->m_Obstacles), t, Pos, Dir, fMagnitude)==true){
		bIntersect = true;
		fMagnitude *= *t;
	}

	return bIntersect;
}


MSimpleVehicle::MSimpleVehicle(bool bDirectionSpeed)
: m_Pos(0, 0, 0), m_Dir(1, 0, 0), m_fVelocity(0), m_fAngularVelocity(0), m_bDirectionSpeed(bDirectionSpeed)
{
}

MSimpleVehicle::~MSimpleVehicle(void)
{
}

/*
rvector MInterpolateVector(rvector &a,rvector &b,float x)
{
	if(IS_EQ3(a,b)) return a;
	float ab,theta,theta1,theta2,costheta1,costheta2,u,v;
	
	ab=(float)min(max(DotProduct(a,b),-1.),1.);
	if(ab==1.0f) return b;
	else if(ab==-1.0f) return a;
	
	theta=(float)acos(ab);
	if(theta==0.0f)
		return a;
	
	theta1=theta*x;
	theta2=theta*(1.0f-x);
	costheta1=(float)cos(theta1);
	costheta2=(float)cos(theta2);
	u=costheta1-ab*costheta2;
	v=costheta2-ab*costheta1;
	float D = (1.0f-ab*ab);
	if(D==0)
		return a;
	rvector vReturn=(1.0f/D*(u*a+v*b));
	//_ASSERT(!_isnan(vReturn.z));
	return vReturn;
}
*/

void MSimpleVehicle::Move(rvector& Dir, MCollisionMap* pCM)
{
	if(Dir.x==0.0f && Dir.y==0.0f && Dir.z==0.0f) return;	// ���� ��ġ�� ����

	// ȸ��
	/////////////////////////////////////////////////////////////
	float fDiffAngle = GetAngleOfVectors(Dir, m_Dir);
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

	rvector NewDir = Normalize(InterpolatedVector(m_Dir, Dir, fRotatePercent));
	//rvector NewDir = Normalize(MInterpolateVector(m_Dir, Dir, fRotatePercent));
	// InterpolatedVector()�� ���ݴ��� �ΰ��϶� ���װ� �ִ�.

	// ��� ��
	//m_fResultRotatePercent = fRotateAngle / m_Property.fMaxRotationVelocity;
	m_fResultRotatePercent = GetAngleOfVectors(NewDir, m_Dir) / m_Property.fMaxRotationVelocity;	// ���׷� ���� ���� ��갪�� �Է�

	// ���� & �̵�
	////////////////////////////////////////////////////////////
	float fNewVelocity = min(m_Property.fMaxVelocity, m_fVelocity + m_Property.fAcceleration); // ���� ���� �ӵ� ���

	if(fNewVelocity>=0.0f){
		if(m_bDirectionSpeed==true){	// ���⿡ ���� ������
			//float fDiffAngle = (float)fabs(GetAngleOfVectors(NewDir, m_Dir));
			fNewVelocity -= (fNewVelocity*(1.0f-fRotatePercent)*0.5f);
		}

		// �̵��� ���� �浹 üũ
		float t = 1.0f;
	#define EXPECT_CONSTANT	10.0f
		if(pCM!=NULL && pCM->IsIntersect(&t, m_Pos, NewDir, fNewVelocity*EXPECT_CONSTANT)==true && (t>0.0f && t<=1.0f)){
			//fNewVelocity = fNewVelocity * t * fRotatePercent / EXPECT_CONSTANT;		// �����ϰ� üũ�ϴ� ��� ( �𼭸� �κп��� ���� �� �ִ�. ��.�� )
			fNewVelocity = fNewVelocity * fRotatePercent * fRotatePercent;				// ��ħ�� ��������� �ε巴�� üũ ( �浹 ���ɽÿ� ���⿡ ����ؼ� �ӵ��� ���� )
			//fNewVelocity = fNewVelocity * max(t, 0.2f) * fRotatePercent / EXPECT_CONSTANT;		// �����ϰ� üũ�ϴ� ��� ( �𼭸� �κп��� ���� �� �ִ�. ��.�� )
		}
	}

	m_fVelocity = fNewVelocity;
	m_Dir = NewDir;
	m_Pos += (m_fVelocity*m_Dir);	// �̵�
}

void MSimpleVehicle::SetMaxVelocity(float v)
{
	m_Property.fMaxVelocity = v;
}

void MSimpleVehicle::SetAcceleration(float v)
{
	m_Property.fAcceleration = v;
}

void MSimpleVehicle::SetMaxRotationVelocity(float v)
{
	m_Property.fMaxRotationVelocity = v;
}

void MSimpleVehicle::SetRotationAcceleration(float v)
{
	m_Property.fRotationAcceleration = v;
}

float MSimpleVehicle::AdjustMaxVeloctiy(float fDistance, float fDecConstant)		// �Ÿ��� ���� Max Velocity ����
{
	m_Property.fMaxVelocity = min(m_Property.fMaxVelocity, m_Property.fMaxVelocity * fDistance / fDecConstant);
	return m_Property.fMaxVelocity;
}
