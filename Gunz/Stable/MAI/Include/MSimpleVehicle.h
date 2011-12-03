#ifndef MSIMPLEVEHICLE_H
#define MSIMPLEVEHICLE_H

#include "rutils.h"
#include "CMPtrList.h"

struct MSIMPLEVEHICLEPROPERTY{
	float	fMaxVelocity;			// Max Velocity
	float	fAcceleration;			// Acceleration
	float	fMaxRotationVelocity;	// Max Rotation Velocity ( Radian )
	float	fRotationAcceleration;	// Rotation Acceleration ( Radian )
};

// Abstract Collision Map
class MCollisionMap{
public:
	virtual bool IsIntersect(float* t, rvector& Pos, rvector& Dir, float fMagnitude) = 0;
};

class MObstacle;
class MRayCastingDriver;
// Collision Map Supported RayCastingDriver
class MRCDCollisionMap : public MCollisionMap{
public:
	CMPtrList<MObstacle>	m_Obstacles;
	MRayCastingDriver*		m_pRCD;
public:
	MRCDCollisionMap(void);
	void Initialize(void);

	void Add(MObstacle* pObstacle);
	void Share(MRayCastingDriver* pRCD);		// Ray Casting Driver�� ����Ÿ ����

protected:
	bool IsIntersect(CMPtrList<MObstacle>* pObstacles, float* t, rvector& Pos, rvector& Dir, float fMagnitude);
public:
	virtual bool IsIntersect(float* t, rvector& Pos, rvector& Dir, float fMagnitude);
};


class MSimpleVehicle{
public:
	MSIMPLEVEHICLEPROPERTY	m_Property;
	rvector		m_Pos;
	rvector		m_Dir;
	float		m_fVelocity;
	float		m_fAngularVelocity;
	bool		m_bDirectionSpeed;		// ���⿡ ���� ������ ���� ( Default = false )

	// �������
	float		m_fResultRotatePercent;	// ������ ȸ���� �� ( -1.0 ~ 1.0 )
public:
	MSimpleVehicle(bool bDirectionSpeed=false);
	virtual ~MSimpleVehicle(void);

	void Move(rvector& Dir, MCollisionMap* pCM);

	void SetMaxVelocity(float v);
	void SetAcceleration(float v);
	void SetMaxRotationVelocity(float v);
	void SetRotationAcceleration(float v);

	// Helper Functions.......
	float AdjustMaxVeloctiy(float fDistance, float fDecConstant=2.0f);		// �Ÿ��� ���� Max Velocity ����
};

#endif
