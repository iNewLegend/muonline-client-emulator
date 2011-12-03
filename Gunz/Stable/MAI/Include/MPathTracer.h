#ifndef MPathTracer_H
#define MPathTracer_H

#include "rutils.h"

struct MPathTracerPROPERTY{
	float	fMaxVelocity;			// Max Velocity
	float	fAcceleration;			// Acceleration
	float	fMaxRotationVelocity;	// Max Rotation Velocity ( Radian )
	float	fRotationAcceleration;	// Rotation Acceleration ( Radian )
};

// Abstract Collision Map
class MPathTracerCollisionMap{
public:
	virtual bool IsCollision(rvector* pRecommendedPos, float* t, rvector& To, rvector& From) = 0;
	virtual float GetMapHeight(float x, float y){ return 0; }
};

class MFreeMapModel;
class MPTFMCollisionMap : public MPathTracerCollisionMap{
	MFreeMapModel*	m_pFM;
public:
	MPTFMCollisionMap(MFreeMapModel* pFM){
		m_pFM = pFM;
	}
	virtual bool IsCollision(rvector* pRecommendedPos, float* t, rvector& To, rvector& From);
};

class MPathTracer{
public:
	// Property
	MPathTracerPROPERTY	m_Property;

	// Values
	rvector		m_Pos;
	rvector		m_Dir;
	rvector		m_Velocity;
	float		m_fAngularVelocity;

	// Attribute
	int			m_nDecelerationConstant;		// ������ �Ÿ� ���
	float		m_fDirectionSettingConstant;	// ��ǥ ������ ���߱� �����ϴ� �Ÿ�
	int			m_nAgularDecelerationConstant;	// Angular Speed���� ������ ���� ��
	bool		m_bDirectionSpeed;				// ���⿡ ���� ������ ���� ( Default = false )
	float		m_fArrivalConstant;				// ���� ��� ( 0 ~ 1.0f ), MaxVelocity*m_fArrivalConstant�� ������ ����
	float		m_nDecelerationCurveConstant;	// ���� Ŀ�� ��� t^m_nDecelerationCurveConstant
	float		m_nDirectionalSpeedContant;		// ��ǥ�������� ���� ���̿� ���� �ӵ� ������ �Ǵ� ���� (0:����~1.0f:����)

	// �������
	float		m_fResultRotatePercent;			// ������ ȸ���� �� ( -1.0 ~ 1.0 )

	// Debug
	rvector		m_DebugRecommendedPos;
	rvector		m_DebugTowardPos;

	enum MPathTracerType{
		MPTT_GROUND = 0,
		MPTT_FLOATING = 1,
		MPTT_ONAIR = 2,
		MPTT_LADINGAIR = 3,
	};

	MPathTracerType	m_nType;

protected:
	bool ProcMove(rvector* pNewPos, rvector* pNewDir, rvector* pNewVelocity, rvector* pDir, rvector& Distance, rvector& RealDistance, bool bSmoothArrival);

public:
	MPathTracer(bool bDirectionSpeed=false);
	virtual ~MPathTracer(void);

	bool Move(rvector* pDir, rvector& TargetPos, MPathTracerCollisionMap* pCM, bool bSmoothArrival);

	void SetMaxVelocity(float v);
	void SetAcceleration(float v);
	void SetMaxRotationVelocity(float v);
	void SetRotationAcceleration(float v);

	// Helper Functions.......
	float AdjustMaxVeloctiy(float fDistance, float fDecConstant=2.0f);		// �Ÿ��� ���� Max Velocity ����
};


#define DEFAULT_DECELERATION_CONSTANT		50
#define DEFAULT_ANGULARDECELERATION_CONTANT	5
#define DEFAULT_ARRIVAL_CONTANT				0.2f
#define DEFAULT_DECELERATION_CURVE_CONSTANT	1.1f
#define DEFAULT_DIRECTIONAL_SPEED_CONSTANT	1.0f
#define DEFAULT_DIRECTION_SETTING_CONSTANT	50
#define DEFAULT_PATHTRACERTYPE				MPTT_GROUND

#endif
