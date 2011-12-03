#ifndef _ZMODULE_MOVABLE_H
#define _ZMODULE_MOVABLE_H

#include "ZModule.h"
#include "ZModuleID.h"

class ZModule_Movable : public ZModule {
private:
	// �Ǹ����� �������� ����
//	float	m_fRadius;
//	float	m_fHeight;

	// �ʱ�ȭ�� �ϰ� ������� �ʴ� ������ �ּ�ó��. �޸����� ���� �˻��� �ܼ��� �Ǳ� ����
	//float	m_fMaxSpeed;	// �ְ�ӵ�
	//bool	m_bGravity;		// �߷��� ������ �޴°�?

	MProtectValue<float>	m_fDistToFloor;	/// �ٴڱ����� �Ÿ�
	MProtectValue<rplane>	m_FloorPlane;	/// �ٴ� ����� ������

	rvector m_lastMove;		// ���������� ������ �Ÿ�

	MProtectValue<bool>		m_bFalling;		// �������̴�
	MProtectValue<float>	m_fFallHeight;	// ���ϰ� ���۵� ����

	MProtectValue<bool>		m_bLanding;		// �̹��� �����߳�
	MProtectValue<bool>		m_bAdjusted;	// ������ �������� (��������) ������
	MProtectValue<float>	m_fLastAdjustedTime;	// ���������� ��� �ð�

	rvector m_Velocity;		// �� private ���� ����

	MProtectValue<bool>		m_bRestrict;		// �̼�����
	MProtectValue<float>	m_fRestrictTime;	// ������ �ɸ� �ð�
	MProtectValue<float>	m_fRestrictDuration;// ������ ���ӽð�
	MProtectValue<float>	m_fRestrictRatio;	// ���� ����

	MProtectValue<bool>		m_bHaste;			// ���Ӻ��ʽ�
	MProtectValue<float>	m_fHasteTime;		// ������ �ɸ� �ð�
	MProtectValue<float>	m_fHasteDuration;	// ������ ���ӽð�
	MProtectValue<float>	m_fHasteRatio;		// ���� ����

	int		m_nHasteItemId;						// ������ ����Ų ������ ID
	float	m_fNextHasteEffectTime;

	bool	m_bForceCollRadius35;	// ���浹�˻�� radius�� 35�� �����Ѵ�. 35�� �÷��̾� ĳ������ radius.
									// (�����̰� ������ ���� npc radius�� Ŭ���� �浹 �������� ���ϴ�, �޸� �浵�� ������
									// �̰����� ��ȸ. ���� �������� ���� �Ĺ����� ���ۿ� ����)

protected:
	void OnAdd();

public:

	DECLARE_ID(ZMID_MOVABLE)
	ZModule_Movable();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();

	const rvector &GetVelocity() { return m_Velocity; }
	void SetVelocity(rvector &vel) { m_Velocity=vel; }
	void SetVelocity(float x,float y,float z) { m_Velocity=rvector(x,y,z); }

	const rvector &GetLastMove() { return m_lastMove; }

	bool Move(rvector &diff);

	void UpdateGravity(float fDelta);

	float GetDistToFloor() { return m_fDistToFloor.Ref(); }

	float GetFallHeight() { return m_fFallHeight.Ref(); }
	bool isLanding() { return m_bLanding.Ref(); }
	bool isAdjusted() { return m_bAdjusted.Ref(); }
	float GetAdjustedTime() { return m_fLastAdjustedTime.Ref(); }

	float GetMoveSpeedRatio();
	void SetMoveSpeedRestrictRatio(float fRatio, float fDuration);
	void SetMoveSpeedHasteRatio(float fRatio, float fDuration, int nItemId);

	bool GetHasteBuffInfo(MTD_BuffInfo& out);

	void ForceCollRadius35(bool b) { m_bForceCollRadius35 = b; }
	
	void ShiftFugitiveValues();

//	void SetCollision(float fRadius, float fHeight) { m_fRadius = fRadius; m_fHeight = fHeight; }
	//void SetRadius(float fRadius) { m_fRadius = fRadius; }
//	float GetRadius()			{ return m_fRadius; }
//	float GetHeight()			{ return m_fHeight; }

protected:
	void UpdatePosition(float fDelta);
};

#endif