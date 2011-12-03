#include "stdafx.h"
#include "ZModule_LightningDamage.h"
#include "ZGame.h"
#include "ZApplication.h"
#include "ZModule_HPAP.h"


#define DAMAGE_DELAY	1.f			// ������ �ִ� ����

ZModule_LightningDamage::ZModule_LightningDamage()
{
	m_bOnDamage = false;
}

void ZModule_LightningDamage::InitStatus()
{
	m_bOnDamage = false;
	Active(false);
}

bool ZModule_LightningDamage::Update(float fElapsed)
{
	ZObject* pObj = MDynamicCast(ZObject, m_pContainer);
	if (!pObj)
		_ASSERT(0);
	else
	{
		ZModule_Movable *pMovable = (ZModule_Movable*)m_pContainer->GetModule(ZMID_MOVABLE);
		if(pMovable)
			pMovable->SetVelocity(0,0,0);

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
				// ����Ʈ�� ������Ʈ�� ���⼭ �����Ϸ��� ������ ����� ������ �ȵ� �ϴ� �ּ�ó���Ѵ�
				// 6 * (1.f-fFR) �̺κ��� zitem.xml���� damage="1"�κκ��� damage="7"�� �ϴ� ����
				//float fFR = 0;
				//float fDamage = 6 * (1.f-fFR) + (float)m_nDamage;
				
				pObj->OnDamaged(m_pOwner, pObj->GetPosition(), ZD_COLD, MWT_NONE, m_fDamage, 0);
/*
				ZModule_HPAP *pModule = (ZModule_HPAP*)m_pContainer->GetModule(ZMID_HPAP);
				if(pModule) {
					pModule->OnDamage(m_pOwner->GetUID(),m_fDamage,0);
//					pObj->OnScream();

				}
				*/
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
void ZModule_LightningDamage::BeginDamage(ZObject* pOwner, int nDamage, float fDuration)
{
	m_fBeginTime = ZGetGame()->GetTime();

	if (!m_bOnDamage)
	{
		m_fNextDamageTime = m_fBeginTime+DAMAGE_DELAY;
	}

	m_bOnDamage = true;

	m_pOwner = pOwner;
	m_fDamage = nDamage;
	m_fDuration = fDuration;

	Active();
}
