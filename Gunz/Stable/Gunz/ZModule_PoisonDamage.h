#ifndef _ZMODULE_POISONDAMAGE_H
#define _ZMODULE_POISONDAMAGE_H

#include "ZModule.h"
#include "ZModuleID.h"

class ZModule_PoisonDamage : public ZModule {
	float	m_fBeginTime;	// ������ ������ �ð�
	float	m_fNextDamageTime;	// ������ �������� ���� �ð�
	float	m_fNextEffectTime;	// ������ ����Ʈ �Ѹ� �ð�

	float	m_fDamage;		// �����ð��� ������
	float	m_fDuration;	// ���ӽð�
	ZObject* m_pOwner;		// �������� �� ���

	bool	m_bOnDamage;	// ������ �ް� �ִ� ���ΰ�

public:
	DECLARE_ID(ZMID_POISONDAMAGE)
	ZModule_PoisonDamage();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();
	bool IsOnDamage() { return m_bOnDamage; }
	float GetDamageBeginTime() { return m_fBeginTime; }

	void BeginDamage(ZObject* pOwner, int nDamage, float fDuration);	// �������� �ֱ� �����Ѵ�
};

#endif