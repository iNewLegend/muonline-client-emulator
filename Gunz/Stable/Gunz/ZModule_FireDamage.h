#ifndef _ZMODULE_FIREDAMAGE_H
#define _ZMODULE_FIREDAMAGE_H

#include "ZModule.h"
#include "ZModuleID.h"

class ZModule_FireDamage : public ZModule {
	float	m_fBeginTime;	// ������ ������ �ð�
	float	m_fNextDamageTime;	// ������ �������� ���� �ð�
	float	m_fNextEffectTime;	// ������ ����Ʈ �Ѹ� �ð�

	float	m_fDamage;		// �����ð��� ������
	float	m_fDuration;	// ���ӽð�
	ZObject* m_pOwner;		// �������� �� ���

	bool	m_bOnDamage;	// ������ �ް� �ִ� ���ΰ�
public:
	DECLARE_ID(ZMID_FIREDAMAGE)
	ZModule_FireDamage();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();

	void BeginDamage(ZObject* pOwner, int nDamage, float fDuration);	// �������� �ֱ� �����Ѵ�
	bool IsOnDamage() { return m_bOnDamage; }
	float GetDamageBeginTime() { return m_fBeginTime; }
};

#endif