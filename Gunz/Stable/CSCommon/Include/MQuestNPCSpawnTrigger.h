#ifndef _MQUESTNPCSPAWNTRIGGER_H
#define _MQUESTNPCSPAWNTRIGGER_H

#include "MBaseQuest.h"


struct SpawnTriggerNPCInfoNode
{
	MQUEST_NPC	nNPCID;
	float		fRate;
};

struct SpawnTriggerInfo
{
	int				nSpawnNPCCount;			// 1ȸ ������ ������ NPC��
	unsigned int	nSpawnTickTime;			// ���� ƽ Ÿ��

	// ����
	int				nCurrMinNPCCount;		// �̰������϶� �����Ѵ�.
	int				nCurrMaxNPCCount;		// �̰��̻��϶� �����Ѵ�.
};

class MQuestNPCSpawnTrigger
{
private:
	vector<SpawnTriggerNPCInfoNode>		m_NPCInfo;
	SpawnTriggerInfo					m_Info;
	unsigned int						m_nLastTime;
	float								m_fRateSum;
	vector<MQUEST_NPC>					m_NPCQueue;
	void MakeSpawnNPCs();
	MQUEST_NPC GetRandomNPC();
public:
	MQuestNPCSpawnTrigger();
	~MQuestNPCSpawnTrigger();
	void BuildNPCInfo(SpawnTriggerNPCInfoNode& NPCInfo);
	void BuildCondition(SpawnTriggerInfo& Info);

	bool CheckSpawnEnable(int nCurrNPCCount);
	void Clear();
	vector<MQUEST_NPC>& GetQueue() { return m_NPCQueue; }
};



#endif