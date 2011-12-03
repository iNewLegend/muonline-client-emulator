#include "stdafx.h"
#include "ZModule_HPAP.h"
#include "ZGame.h"
#include "ZApplication.h"
#include "CheckReturnCallStack.h"

ZModule_HPAP::ZModule_HPAP() : m_LastAttacker(MUID(0,0)), m_bRealDamage_DebugRegister(false), m_fAccumulationDamage(0.f), m_bAccumulationDamage(false)
{
	float f = float(rand() % 1000); 
	f += float(rand() % 100) * 0.01f;
	m_fMask.Set_MakeCrc(f);

	m_bRealDamage.Set_MakeCrc(false);
	m_fHP.Set_MakeCrc(1000.f);
	m_fAP.Set_MakeCrc(1000.f);
	m_fMaxHP.Set_MakeCrc(100);
	m_fMaxAP.Set_MakeCrc(100);
}



ZModule_HPAP::~ZModule_HPAP()
{
}

float ZModule_HPAP::GetHP() 
{
	return  m_fHP.Ref() - GetMask();
}

float	ZModule_HPAP::GetAP() 
{
	return m_fAP.Ref() - GetMask();
}

void ZModule_HPAP::OnDamage(MUID uidAttacker,float fDamage, float fRatio)
{
	_ASSERT(fDamage >= 0);
	if (fDamage < 0) fDamage *= -1.f;	// ��ŷ���� ����������� �־ ȸ���� �� �����Ƿ�..

	m_LastAttacker = uidAttacker;

	// ����Ʈ �׽�Ʈ�� ġƮ üũ
#ifndef _PUBLISH
	if (CheckQuestCheet() == true) return;
#endif
	//jintriple3 ����� �������� �� ���� �ڵ�
	//�� ĳ���Ͱ� �ƴϰų� npc�� �ƴϸ�....
	//m_bRealDamage�� ��� �� Ŭ���̾�Ʈ���� ������ �������� �����ϳ� ���ϳ��� üũ�ϴ� ����.
	//MyCharacter �̰ų� NPC�� �� Ŭ���̾�Ʈ���� ������ ����.
	if(!m_bRealDamage.Ref())	
		PROTECT_DEBUG_REGISTER(!m_bRealDamage_DebugRegister)	//�̰� ���� ���ο� ���� �ϳ� �� ���.
			return;

	// NPC�� ���̵� ������������� ����
	ZObject* pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
	if ((pAttacker) && (!IsPlayerObject(pAttacker)))
	{
		ZActor* pActor = (ZActor*)pAttacker;
		//fDamage = (int)(fDamage * (pActor->GetTC()));
		fDamage = fDamage * (pActor->GetQL() * 0.2f + 1);
	}

	// ���� �ױ������� ���� �������� �����Ѵ�(�ϴ� �����ʸ�Ʈ ������)
	if(IsAccumulationDamage())
	{
		AccumulateDamage(fDamage);
#ifndef _PUBLISH	// ���κ��忡�� ���� ����� ���� ���
		char szAccumulationDamagePrint[256];
		sprintf(szAccumulationDamagePrint, "���� ���� ���������[%2.1f]", GetAccumulationDamage());
		ZChatOutput(MCOLOR(255, 200, 100), szAccumulationDamagePrint);
#endif
	}


	float fHPDamage = (float)((float)fDamage * fRatio);
	float fAPDamage = fDamage - fHPDamage;

	if ((GetAP() - fAPDamage) < 0)
	{
		fHPDamage += (fAPDamage - GetAP());
		fAPDamage -= (fAPDamage - GetAP());
	}

	SetHP(GetHP() - fHPDamage);
	SetAP(GetAP() - fAPDamage);
	
//jintriple3
/*	if(m_bRealDamage)
	{
		// NPC�� ���̵� ������������� ����
		ZObject* pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
		if ((pAttacker) && (!IsPlayerObject(pAttacker)))
		{
			ZActor* pActor = (ZActor*)pAttacker;
			//damage = (int)(damage * (pActor->GetTC()));
			damage = (int)(damage * (pActor->GetQL() * 0.2f + 1));
		}*/

/*
		float fHPDamage = 0, fAPDamage = 0;

		fHPDamage = damage * fRatio;
		fAPDamage = damage * (1.0f - fRatio);

*/
	//jintripl3
/*
		int nHPDamage = (int)((float)damage * fRatio);
		int nAPDamage = damage - nHPDamage;

		if ((GetAP() - nAPDamage) < 0)
		{
			nHPDamage += (nAPDamage - GetAP());
			nAPDamage -= (nAPDamage - GetAP());
		}

		SetHP(GetHP() - nHPDamage);
		SetAP(GetAP() - nAPDamage);
	}*/
}


void ZModule_HPAP::InitStatus()
{
	m_LastAttacker = MUID(0,0);
}

bool ZModule_HPAP::CheckQuestCheet()
{
#ifdef _PUBLISH
	return false;
#endif

	// ����Ʈ �׽�Ʈ�� ġƮ üũ
	if (IsMyCharacter((ZObject*)m_pContainer))
	{
		if ((ZIsLaunchDevelop()) && (ZGetGameClient()->GetServerMode() == MSM_TEST))
		{
			if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
			{
				if (ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD) == true) return true;
			}
		}
	}

	return false;
}

void ZModule_HPAP::ShiftFugitiveValues()
{
	m_fMask.ShiftHeapPos_CheckCrc();

	m_fMaxHP.ShiftHeapPos_CheckCrc();
	m_fMaxAP.ShiftHeapPos_CheckCrc();

	m_fHP.ShiftHeapPos_CheckCrc();
	m_fAP.ShiftHeapPos_CheckCrc();

	m_bRealDamage.ShiftHeapPos_CheckCrc();
}