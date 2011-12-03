#ifndef MVehicle_H
#define MVehicle_H

#include "MAStar.h"
#include "rutils.h"

struct MVehiclePROPERTY{
	float	fMaxVelocity;			// Max Velocity
	float	fAcceleration;			// Acceleration
	float	fMaxRotationVelocity;	// Max Rotation Velocity ( Radian )
	float	fRotationAcceleration;	// Rotation Acceleration ( Radian )
};

// Abstract Collision Map
class MVehicleCollisionMap{
public:
	virtual bool IsCollision(rvector* pRecommendedPos, float* t, rvector& To, rvector& From) = 0;
	virtual float GetMapHeight(float x, float y){ return 0; }
};

class MVehicle : public MNodeModel{
public:
	// Property
	MVehiclePROPERTY	m_Property;

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

protected:
	bool ProcMove(rvector* pNewPos, rvector* pNewDir, rvector* pNewVelocity, rvector* pDir, rvector& Distance, rvector& RealDistance, bool bSmoothArrival);

public:
	MVehicle(bool bDirectionSpeed=false);
	virtual ~MVehicle(void);

	bool Move(rvector* pDir, rvector& TargetPos, MVehicleCollisionMap* pCM, bool bSmoothArrival);
	bool Move(MVehicleCollisionMap* pCM);

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


#define MAX_TEST_POS	8	// ���� ���� ��ġ ����

class MVehicleNode : public MNodeModel{
protected:
	rvector		m_Pos;		// Current Position
	rvector		m_Dir;		// Current Direction
	rvector		m_Velocity;

	// Static Properties
	float		m_fMaxVelocity;		// Max Velocity
	float		m_fAcceleration;	// Acceleration

	MVehicleNode*	m_Successors[MAX_TEST_POS];

protected:
	void GetNextTestPos(rvector* pNewPos, rvector* pNewVelocity, int i);
public:
	MVehicleNode(rvector& Pos, rvector& Dir, rvector& Velocity, float fMaxVelocity, float fAcceleration);
	//MVehicleNode(void);
	virtual ~MVehicleNode(void);

	rvector GetPos(void){ return m_Pos; }
	rvector GetDir(void){ return m_Dir; }
	rvector GetVelocity(void){ return m_Velocity; }

	// MNodeModel Virtual Functions
	virtual int GetSuccessorCount(void);
	virtual MNodeModel* GetSuccessor(int i);
	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	virtual float GetHeuristicCost(MNodeModel* pNode);

	//virtual float GetTotalCostPriority(void){ return m_fCostToGoal; }		// Goal������ Cost���� ����Ѵ�.
};

#endif
