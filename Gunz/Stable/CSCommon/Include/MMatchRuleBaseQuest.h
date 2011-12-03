#ifndef _MMATCHRULE_BASEQUEST_H
#define _MMATCHRULE_BASEQUEST_H

#include "MMatchRule.h"
#include "MMatchNPCObject.h"
#include "MQuestPlayer.h"
#include "MMatchQuestRound.h"


class MMatchQuestGameLogInfoManager;

/// ����Ʈ����� �θ� Ŭ����
class MMatchRuleBaseQuest : public MMatchRule {
protected:
	MMatchNPCManager			m_NPCManager;				///< NPC ������
	MQuestPlayerManager			m_PlayerManager;			///< �÷��̾� ������
	bool						m_bQuestCompleted;			///< ����Ʈ�� �������� ����

	unsigned long int			m_nLastNPCSpawnTime;		// for test
	int							m_nNPCSpawnCount;			///< ������ NPC��
	unsigned long int			m_nSpawnTime;				///< ������ NPC ���� �ð�
	int							m_nFirstPlayerCount;		///< ó�� ���ӽ��۽� �÷��̾� ��

	unsigned long int			m_nLastNPCAssignCheckTime;	///< ���������� NPC ���Ҵ� üũ�� �� �ð�
	unsigned long int			m_nLastPingTime;			///< ���������� Ŭ���̾�Ʈ �� �� �ð�

protected:
	virtual void OnBegin();								///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();								///< ��ü ���� ����� ȣ��
	virtual bool OnRun();								///< ����ƽ�� ȣ��
	virtual void OnRoundBegin();						///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();							///< ���� ���� �� ȣ��
	virtual bool OnCheckRoundFinish();					///< ���尡 �������� üũ
	virtual void OnRoundTimeOut();						///< ���尡 Ÿ�Ӿƿ����� ����� �� OnRoundEnd() ���̴�.
	virtual bool RoundCount();							///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.
	virtual bool OnCheckEnableBattleCondition();		///< ���� �������� üũ

	virtual void OnCommand(MCommand* pCommand);			///< ����Ʈ������ ����ϴ� Ŀ�ǵ� ó��
protected:
	/// npc�� �װ� ���� ��Ӿ������� �ִ��� üũ�ϰ� ������ �������� ��ӽ�Ų��.
	/// @param uidPlayer	������ �÷��̾� UID
	/// @param pDropItem	����� ������
	/// @param pos			��ġ
	void CheckRewards(MUID& uidPlayer, MQuestDropItem* pDropItem, MVector& pos);
	bool CheckPlayersAlive();								///< �÷��̾ ��� �׾����� üũ
	virtual void ProcessNPCSpawn() = 0;						///< NPC �����۾�
	virtual bool CheckNPCSpawnEnable() = 0;					///< NPC�� ���� �������� ����
	virtual void RouteGameInfo() = 0;						///< Ŭ���̾�Ʈ�� ���� ���� �����ش�.
	virtual void RouteStageGameInfo() = 0;					///< ����� ������������ �ٲ� ���� ������ �����ش�.

	/// NPC�� ������Ų��.
	/// @param nNPC			NPC ����
	/// @param nPosIndex	���� ��ġ
	MMatchNPCObject* SpawnNPC(MQUEST_NPC nNPC, int nPosIndex, bool bKeyNPC=false);

	virtual void OnCompleted();					///< ����Ʈ ������ ȣ��ȴ�.
	virtual void OnFailed();					///< ����Ʈ ���н� ȣ��ȴ�.
	virtual void RouteCompleted() = 0;			///< ����Ʈ ���� �޽����� ������. - ��������� �Բ� ������.
	virtual void RouteFailed() = 0;				///< ����Ʈ ���� �޽��� ������.
	virtual void DistributeReward() = 0;		///< ����Ʈ ������ ������ ���

	void ReAssignNPC();
	void SendClientLatencyPing();
public:
	// Ŀ�ǵ� ó�� ���� �Լ�

	/// NPC�� �׿����� ȣ��
	/// @param uidSender		�޼��� ���� �÷��̾�
	/// @param uidKiller		���� �÷��̾�
	/// @param uidNPC			���� NPC
	/// @param pos				NPC ��ġ
	virtual void OnRequestNPCDead(MUID& uidSender, MUID& uidKiller, MUID& uidNPC, MVector& pos);

	/// �÷��̾� �׾��� �� ȣ��
	/// @param uidVictim		���� �÷��̾� UID
	virtual void OnRequestPlayerDead(const MUID& uidVictim);
	virtual void OnRequestTestNPCSpawn(int nNPCType, int nNPCCount);
	virtual void OnRequestTestClearNPC();

	/// ��� �÷��̾� ��Ȱ �� HP, AP �ʱ�ȭ
	void RefreshPlayerStatus();
	/// ��� NPC�� �ʱ�ȭ
	void ClearAllNPC();

	/// �÷��̾ ��Ż�� �̵����� ��� ȣ��ȴ�.
	/// @param uidPlayer			�̵��� �÷��̾� UID
	virtual void OnRequestMovetoPortal(const MUID& uidPlayer) = 0;
	/// ��Ż�� �̵��ϰ� ���� �̵��� �Ϸ�Ǿ��� ��� ȣ��ȴ�.
	/// @param uidPlayer			�÷��̾� UID
	virtual void OnReadyToNewSector(const MUID& uidPlayer) = 0;

	virtual void OnRequestTestSectorClear() = 0;
	virtual void OnRequestTestFinish() = 0;


	// ����Ʈ������ �߰��� ���� �������̽�.
	// ���������� ���� ������ �غ�����. �ʹ� ���� �������̽��� MMatchRule���� �ö�� ������... - by �߱���.
	virtual void OnRequestDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnResponseDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnRequestCallbackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnResponseCallBackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )	{}
	virtual void OnRequestQL( const MUID& uidSender ) {}
	virtual void OnResponseQL_ToStage( const MUID& uidStage )	{}
	virtual void OnRequestSacrificeSlotInfo( const MUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToListener( const MUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToStage( const MUID& uidStage )	{}
	virtual void OnChangeCondition() {}

	virtual bool							PrepareStart() { return true; }
	virtual void							PreProcessLeaveStage( const MUID& uidLeaverUID );


	// ���� ���̺� �߰� �������̽�.
	virtual void CheckMonsterBible( const MUID& uidUser, const int nMonsterBibleIndex );
	virtual void PostNewMonsterInfo( const MUID& uidUser, const char nMonIndex );
	
public:
	MMatchRuleBaseQuest(MMatchStage* pStage);			///< ������
	virtual ~MMatchRuleBaseQuest();						///< �Ҹ���
	virtual void OnEnterBattle(MUID& uidChar);			///< ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnLeaveBattle(MUID& uidChar);			///< ������ �������� ȣ��ȴ�.

	virtual void RefreshStageGameInfo() = 0;
};


void InsertNPCIDonUnique( vector<MQUEST_NPC>& outNPCList, MQUEST_NPC nNPCID );
void MakeJacoNPCList( vector<MQUEST_NPC>& outNPCList, MQuestScenarioInfoMaps& ScenarioInfoMaps );
void MakeSurvivalKeyNPCList( vector<MQUEST_NPC>& outNPCList, MQuestScenarioInfoMaps& ScenarioInfoMaps );
void MakeNomalNPCList( vector<MQUEST_NPC>& outNPCList, MQuestScenarioInfoMaps& ScenarioInfoMaps, MMatchQuest* pQuest );
void CopyMTD_NPCINFO( MTD_NPCINFO* pDest, const MQuestNPCInfo* pSource );

#endif