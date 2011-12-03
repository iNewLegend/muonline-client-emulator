#ifndef _MMATCHRULE_BERSERKER_H
#define _MMATCHRULE_BERSERKER_H


#include "MMatchRule.h"
#include "MMatchRuleDeathMatch.h"


class MMatchRuleBerserker : public MMatchRuleSoloDeath {
protected:
	// ������� ---------------------
	MUID		m_uidBerserker;				// ���� ����Ŀ�� �÷��̾�

	// �Լ� -------------------------
	bool CheckKillCount(MMatchObject* pOutObject);
	virtual void OnRoundBegin();
	virtual bool OnCheckRoundFinish();
	void RouteAssignBerserker();
	MUID RecommendBerserker();
public:
	MMatchRuleBerserker(MMatchStage* pStage);
	virtual ~MMatchRuleBerserker() { }
	virtual void* CreateRuleInfoBlob();
	virtual void OnEnterBattle(MUID& uidChar);			///< ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnLeaveBattle(MUID& uidChar);			///< ������ �������� ȣ��ȴ�.
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_BERSERKER; }
	MUID& GetBerserker() { return m_uidBerserker; }
};

#endif