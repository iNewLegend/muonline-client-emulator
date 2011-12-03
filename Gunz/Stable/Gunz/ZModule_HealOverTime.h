#ifndef _ZMODULE_HEALOVERTIME_H
#define _ZMODULE_HEALOVERTIME_H

#include "ZModule.h"
#include "ZModuleID.h"

// �ð��� �帧�� ���� HP�� AP�� ȸ���Ǵ� ȿ���� ���� ���

class ZModule_HealOverTime : public ZModule {
	MProtectValue<float>	m_fBeginTime;		// �� ������ �ð�
	MProtectValue<float>	m_fNextHealTime;	// ������ �� ���� �ð�

	MProtectValue<float>	m_fHeal;			// 1ȸ ȸ����
	MProtectValue<int>		m_numHealDesire;	// �� ��ǥ ȸ��Ƚ��
	MProtectValue<int>		m_numHealDone;		// ���� ȸ���� Ƚ��

	MProtectValue<MMatchDamageType> m_type;
	MMatchItemEffectId		m_nEffectId;
	int						m_nItemId;			// ȿ���� ����Ų ������ID

	MProtectValue<bool>		m_bOnHeal;	// �� �ް� �ִ� ���ΰ�

public:
	DECLARE_ID(ZMID_HEALOVERTIME)
	ZModule_HealOverTime();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();

	void BeginHeal(MMatchDamageType type, int nHealAmount, int numHeal, MMatchItemEffectId effectId, int nItemId);
	bool IsOnHeal() { return m_bOnHeal.Ref(); }
	float GetHealBeginTime() { return m_fBeginTime.Ref(); }

	bool GetHealOverTimeBuffInfo(MTD_BuffInfo& out);

	void ShiftFugitiveValues();
};

#endif