#ifndef _ZMODULE_LIGHTNINGDAMAGE_H
#define _ZMODULE_LIGHTNINGDAMAGE_H

#include "ZModule.h"
#include "ZModuleID.h"

class ZModule_LightningDamage : public ZModule {
	float	m_fBeginTime;	// ������ ������ �ð�
	float	m_fNextDamageTime;	// ������ �������� ���� �ð�
	float	m_fNextEffectTime;

	float	m_fDamage;		// �����ð��� ������
	float	m_fDuration;	// ���ӽð�
	ZObject* m_pOwner;		// �������� �� ���

	bool	m_bOnDamage;	// ������ �ް� �ִ� ���ΰ�

public:
	DECLARE_ID(ZMID_LIGHTNINGDAMAGE)
	ZModule_LightningDamage();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();
	bool IsOnDamage() { return m_bOnDamage; }
	float GetDamageBeginTime() { return m_fBeginTime; }

	void BeginDamage(ZObject* pOwner, int nDamage, float fDuration);	// �������� �ֱ� �����Ѵ�
};

#endif