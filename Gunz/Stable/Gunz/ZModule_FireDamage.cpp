#include "stdafx.h"
#include "ZModule_FireDamage.h"
#include "ZGame.h"
#include "ZApplication.h"
#include "ZModule_HPAP.h"

#define DAMAGE_DELAY	0.1f	//1.f	// ������ �ִ� ����
#define EFFECT_DELAY	0.15f			// ����Ʈ ����

int GetEffectLevel();

ZModule_FireDamage::ZModule_FireDamage()
{
	m_bOnDamage = false;
	m_pOwner = NULL;
}

void ZModule_FireDamage::InitStatus()
{
	m_bOnDamage = false;
	Active(false);
}

bool ZModule_FireDamage::Update(float fElapsed)
{
	ZObject* pObj = MDynamicCast(ZObject, m_pContainer);
	if (!pObj)
		_ASSERT(0);
	else
	{
		// ������ ���� DAMAGE_DELAY
		if(ZGetGame()->GetTime()>m_fNextDamageTime) {
			m_fNextDamageTime+=DAMAGE_DELAY;

			// ������ �ް� �ִ� ����Ʈ �׾����� ������ ���������� ���� ���¿����� ���δ�..
			if(pObj->IsDie()) {

				if( pObj->m_pVMesh->GetVisibility() < 0.5f ) {//����Ʈ�� Life Ÿ�ӵ� �����ϱ�...
					m_bOnDamage = false;
					return false;
				}

			}
			else //�����������..
			{
				// ���̾� ������Ʈ�� ���⼭ �����Ϸ��� ������ ����� ������ �ȵ� �ϴ� �ּ�ó���Ѵ�
				// 6 * (1.f-fFR) �̺κж����� zitem.xml���� damage="1"�κκ��� damage="7"�� �ϴ� ����
				//float fFR = 0;
				//float fDamage = 6 * (1.f-fFR) + (float)m_nDamage;

				// HPAP ����� ���� �������� ���� �����̳��� OnDamaged�� ����ϵ��� �Ѵ�
				pObj->OnDamaged(m_pOwner, pObj->GetPosition(), ZD_FIRE, MWT_NONE, m_fDamage, 0);
				/*ZModule_HPAP *pModule = (ZModule_HPAP*)m_pContainer->GetModule(ZMID_HPAP);
				if(pModule) {
					pModule->OnDamage(m_pOwner,m_fDamage,0);
					//pObj->OnScream();
				}*/
			}
		}

		if(ZGetGame()->GetTime()>m_fNextEffectTime) {

			if(!pObj->IsDie())
			{
				int nEffectLevel = GetEffectLevel()+1;

				m_fNextEffectTime+=EFFECT_DELAY * nEffectLevel;

				ZGetEffectManager()->AddEnchantFire2( pObj );
				ZGetEffectManager()->AddEnchantFire2( pObj );
			}
		}
	}

	if(m_fNextDamageTime-m_fBeginTime>m_fDuration) {
		m_bOnDamage = false;
		return false;
	}
	return true;
}

// �������� �ֱ� �����Ѵ�
void ZModule_FireDamage::BeginDamage(ZObject* pOwner, int nDamage, float fDuration)
{
	m_fBeginTime = ZGetGame()->GetTime();

	m_pOwner = pOwner;
	m_fDamage = (float)nDamage*DAMAGE_DELAY;	// DAMAGE_DELAY �� �������� ������
	m_fDuration = fDuration+0.01f;		// 0.1�� ������ ����� ������ ������ �۾��� ������ ������ ������ �ȵŴ� ��찡 �־� ���Ӱ� ������� 0.01f�� �����ش�.

	if (!m_bOnDamage)
	{
		m_fNextDamageTime = m_fBeginTime+DAMAGE_DELAY;
		m_fNextEffectTime = m_fBeginTime;
	}

	m_bOnDamage = true;

	Active();
}