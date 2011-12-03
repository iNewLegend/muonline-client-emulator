#include "stdafx.h"
#include "MMatchRuleBerserker.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MMatchServer.h"

//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleBerserker ///////////////////////////////////////////////////////////
MMatchRuleBerserker::MMatchRuleBerserker(MMatchStage* pStage) : MMatchRuleSoloDeath(pStage), m_uidBerserker(0,0)
{

}

bool MMatchRuleBerserker::OnCheckRoundFinish()
{
	return MMatchRuleSoloDeath::OnCheckRoundFinish();
}

void MMatchRuleBerserker::OnRoundBegin()
{
	m_uidBerserker = MUID(0,0);
}

void* MMatchRuleBerserker::CreateRuleInfoBlob()
{
	void* pRuleInfoArray = MMakeBlobArray(sizeof(MTD_RuleInfo_Berserker), 1);
	MTD_RuleInfo_Berserker* pRuleItem = (MTD_RuleInfo_Berserker*)MGetBlobArrayElement(pRuleInfoArray, 0);
	memset(pRuleItem, 0, sizeof(MTD_RuleInfo_Berserker));
	
	pRuleItem->nRuleType = MMATCH_GAMETYPE_BERSERKER;
	pRuleItem->uidBerserker = m_uidBerserker;

	return pRuleInfoArray;
}

void MMatchRuleBerserker::RouteAssignBerserker()
{	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_ASSIGN_BERSERKER, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(m_uidBerserker));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}


MUID MMatchRuleBerserker::RecommendBerserker()
{
	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return MUID(0,0);

	int nCount = 0;
	for(MUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ
		if (pObj->CheckAlive())
		{
			return pObj->GetUID();
		}
	}
	return MUID(0,0);

}


void MMatchRuleBerserker::OnEnterBattle(MUID& uidChar)
{
}

void MMatchRuleBerserker::OnLeaveBattle(MUID& uidChar)
{
	if (uidChar == m_uidBerserker)
	{
		m_uidBerserker = MUID(0,0);
		RouteAssignBerserker();
	}
}

void MMatchRuleBerserker::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	// ����ڰ� ����Ŀ�̰ų� ���� ����Ŀ�� �Ѹ� ������
	if ((m_uidBerserker == uidVictim) || (m_uidBerserker == MUID(0,0)))
	{
		bool bAttackerCanBeBerserker = false;

		 // �����ڰ� �ڽ��� �ƴ� ���
		if (uidAttacker != uidVictim)
		{
			MMatchObject* pAttacker = MMatchServer::GetInstance()->GetObject(uidAttacker);

			// �����ڰ� �׾������� ����Ŀ�� �� �� ����(���꼦)
			if ((pAttacker) && (pAttacker->CheckAlive()))
			{
				bAttackerCanBeBerserker = true;
			}
		}
		// �����ڰ� �ڽ��� ��� ����Ŀ�� �ƹ��� ���� �ʴ´�.
		else if ((uidAttacker == MUID(0,0)) || (uidAttacker == uidVictim))
		{
			bAttackerCanBeBerserker = false;
		}

		if (bAttackerCanBeBerserker) m_uidBerserker = uidAttacker;
		else m_uidBerserker = MUID(0,0);

		RouteAssignBerserker();
	}
}

