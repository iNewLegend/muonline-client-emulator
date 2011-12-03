#include "stdafx.h"
#include "ZModule_ColdDamage.h"
#include "ZGame.h"
#include "ZApplication.h"
#include "MRTTI.h"

#define DAMAGE_DELAY	1.f			// ������ �ִ� ����
#define EFFECT_DELAY	0.15f			// ����Ʈ ����

int GetEffectLevel();

ZModule_ColdDamage::ZModule_ColdDamage()
{
	m_bOnDamage = false;

}

void ZModule_ColdDamage::InitStatus()
{
	//ZModule_Movable *pMovableModule = (ZModule_Movable*)m_pContainer->GetModule(ZMID_MOVABLE);
	//_ASSERT(pMovableModule!=NULL);
	//pMovableModule->SetMoveSpeedRestrictRatio(1.f);
	m_bOnDamage = false;
	Active(false);
}

bool ZModule_ColdDamage::Update(float fElapsed)
{
	//ZModule_Movable *pMovableModule = (ZModule_Movable*)m_pContainer->GetModule(ZMID_MOVABLE);
	//_ASSERT(pMovableModule!=NULL);
	//pMovableModule->SetMoveSpeedRestrictRatio(m_fMoveSpeed);

	ZObject *pObj = MStaticCast(ZObject,m_pContainer);

	if(ZGetGame()->GetTime()>m_fNextDamageTime) {
		m_fNextDamageTime+=DAMAGE_DELAY;

		ZModule_HPAP *pHPModule = (ZModule_HPAP*)m_pContainer->GetModule(ZMID_HPAP);
		_ASSERT(pHPModule!=NULL);

		{
			if(pObj->IsDie()) {

				if( pObj->m_pVMesh->GetVisibility() < 0.5f ) {//����Ʈ�� Life Ÿ�ӵ� �����ϱ�...
//					pMovableModule->SetMoveSpeedRestrictRatio(1.f);
					m_bOnDamage = false;
					return false;
				}
			}
			else //�����������..
			{
//				pObj->OnScream();
			}
		}
	}

	if(ZGetGame()->GetTime()>m_fNextEffectTime) {

		if(!pObj->IsDie())
		{
			int nEffectLevel = GetEffectLevel()+1;

			m_fNextEffectTime+=EFFECT_DELAY * nEffectLevel;

			ZGetEffectManager()->AddEnchantCold2( pObj );
		}
	}


	if(m_fNextDamageTime-m_fBeginTime>m_fDuration) {
//		pMovableModule->SetMoveSpeedRestrictRatio(1.f);
		m_bOnDamage = false;
		return false;
	}

	return true;
}

// �������� �ֱ� �����Ѵ�
void ZModule_ColdDamage::BeginDamage(float fMoveSpeed,float fDuration)
{
	m_fBeginTime = ZGetGame()->GetTime();
	
	if (!m_bOnDamage)
	{
		m_fNextDamageTime = m_fBeginTime+DAMAGE_DELAY;
		m_fNextEffectTime = m_fBeginTime;
	}

	m_bOnDamage = true;

	m_fDuration = fDuration;
//	m_fMoveSpeed = fMoveSpeed;


	ZModule_Movable *pMovableModule = (ZModule_Movable*)m_pContainer->GetModule(ZMID_MOVABLE);
	_ASSERT(pMovableModule!=NULL);
	pMovableModule->SetMoveSpeedRestrictRatio(fMoveSpeed,fDuration);

	Active();
}
