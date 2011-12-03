#ifndef _MMATCHRULESURVIVAL_H
#define _MMATCHRULESURVIVAL_H

#include "MMatchRule.h"
#include "MMatchRuleBaseQuest.h"
#include "MMatchNPCObject.h"
#include "MMatchQuestRound.h"
#include "MSacrificeQItemTable.h"
#include "MQuestItem.h"
#include "MMatchSurvivalGameLog.h"
#include "MQuestNPCSpawnTrigger.h"
#include "MBaseGameType.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MQuestLevel;

/// �����̹� �� Ŭ����
class MMatchRuleSurvival : public MMatchRuleBaseQuest {

	typedef pair< MUID, unsigned long int > SacrificeSlot;	// <�������� �÷����� ������ UID, �÷����� �������� ItemID.>

	class MQuestSacrificeSlot
	{
	public :
		MQuestSacrificeSlot() 
		{
			Release();
		}

		~MQuestSacrificeSlot()
		{
		}

		MUID&				GetOwnerUID()	{ return m_SacrificeSlot.first; }
		unsigned long int	GetItemID()		{ return m_SacrificeSlot.second; }

		bool IsEmpty() 
		{
			if( (MUID(0, 0) == m_SacrificeSlot.first) && (0 == m_SacrificeSlot.second) )
				return true;
			return false;
		}


		void SetOwnerUID( const MUID& uidItemOwner )		{ m_SacrificeSlot.first = uidItemOwner; }
		void SetItemID( const unsigned long int nItemID )	{ m_SacrificeSlot.second = nItemID; }
		void SetAll( const MUID& uidItemOwner, const unsigned long int nItemID )
		{
			SetOwnerUID( uidItemOwner );
			SetItemID( nItemID );
		}


		bool IsOwner( const MUID& uidRequester, const unsigned long int nItemID )
		{
			if( (m_SacrificeSlot.first == uidRequester) && (m_SacrificeSlot.second == nItemID) )
				return true;
			return false;
		}

		void Release()
		{
			m_SacrificeSlot.first	= MUID( 0, 0 );
			m_SacrificeSlot.second	= 0;
		}

	private :
		SacrificeSlot	m_SacrificeSlot;
	};

private:
	// Ÿ������ ---------------------

	/// ���濡�� �ʿ��� ���� ���������� ������. �����߿����� m_pQuestLevel�� ��¥���� ����ȴ�.
	struct MQuestStageGameInfo
	{
		int				nQL;
		int				nPlayerQL;
		int				nMapsetID;
		unsigned int	nScenarioID;
	};
	/// ���ͳ� ���� ���
	enum COMBAT_PLAY_RESULT
	{
		CPR_PLAYING = 0,		///< ���� ������
		CPR_COMPLETE,			///< ���� Ŭ����
		CPR_FAILED				///< ����
	};

	/// �����̹� �ó����� �ݺ��� ������ ��ȭ�Ǵ� NPC�� �ɷ�ġ�� ����
	struct MQuestLevelReinforcedNPCStat
	{
		float			fMaxHP;
		float			fMaxAP;
		MQuestLevelReinforcedNPCStat() : fMaxAP(1), fMaxHP(1) {}
	};

	// ������� ---------------------

	unsigned long	m_nPrepareStartTime;	///< ��Ż �̵� �ð� ����ϱ� ���� ����
	unsigned long	m_nCombatStartTime;		///< �����̵��ϰ� ���ο� ���� ������ �ð�
	unsigned long	m_nQuestCompleteTime;	///< ����Ʈ Ŭ��� �ð�

	MQuestSacrificeSlot				m_SacrificeSlot[ MAX_SACRIFICE_SLOT_COUNT ];
	//int								m_iScenarioState;
	int								m_nPlayerCount;
	MMatchSurvivalGameLogInfoManager	m_SurvivalGameLogInfoMgr;

	MQuestStageGameInfo				m_StageGameInfo;	///< ���濡�� �ʿ��� ���� ���������� ������

	vector< MQUEST_NPC >			m_vecNPCInThisScenario;		///< �� �ó��������� ���� NPC ���
	map<MQUEST_NPC, MQuestLevelReinforcedNPCStat>	m_mapReinforcedNPCStat;	///< �ó����� �ݺ��� ��ġ�� ��ȭ�� NPC �ɷ�ġ�� ����

	typedef map<MQUEST_NPC, MQuestLevelReinforcedNPCStat>	MapReinforcedNPCStat;
	typedef MapReinforcedNPCStat::iterator		ItorReinforedNPCStat;

	// �Լ� -------------------------
	void ClearQuestLevel();
	void MakeStageGameInfo();
	void InitJacoSpawnTrigger();
	void MakeNPCnSpawn(MQUEST_NPC nNPCID, bool bAddQuestDropItem, bool bKeyNPC);
	int GetRankInfo(int nKilledNpcHpApAccum, int nDeathCount);
protected:
	MQuestLevel*			m_pQuestLevel;			///< ����Ʈ ���� ����
	MQuestNPCSpawnTrigger	m_JacoSpawnTrigger;		///< �������� ��� ���� �Ŵ���
	MQuestCombatState		m_nCombatState;			///< ���ͳ� ���� ����

	virtual void ProcessNPCSpawn();				///< NPC �����۾�
	virtual bool CheckNPCSpawnEnable();			///< NPC�� ���� �������� ����
	virtual void RouteGameInfo();				///< Ŭ���̾�Ʈ�� ���� ���� �����ش�.
	virtual void RouteStageGameInfo();			///< ����� ������������ �ٲ� ���� ������ �����ش�.
	virtual void RouteCompleted();				///< ����Ʈ ���� �޽����� ������. - ��������� �Բ� ������.
	virtual void RouteFailed();					///< ����Ʈ ���� �޽��� ������.
	virtual void OnCompleted();					///< ����Ʈ ������ ȣ��ȴ�.
	virtual void OnFailed();					///< ����Ʈ ���н� ȣ��ȴ�.
	virtual void DistributeReward() {}			///< ����Ʈ ������ ������ ��� - �����̹��� ������ ����

	void SendGameResult();						///< ���� ������ ����� �뺸

	/// ���� ���� ���۵Ǿ��ٰ� �޼��� ������.
	void RouteMapSectorStart();
	/// �ش� �÷��̾ ��Ż�� �̵��ߴٰ� �޼��� ������.
	/// @param uidPlayer �̵��� �÷��̾� UID
	void RouteMovetoPortal(const MUID& uidPlayer);
	/// �ش� �÷��̾ ��Ż�� �̵��� �Ϸ�Ǿ��ٰ� �޼��� ������.
	/// @param uidPlayer �̵��� �÷��̾� UID
	void RouteReadyToNewSector(const MUID& uidPlayer);
	/// �ش� ����Ʈ �������� �Ծ��ٰ� �޼��� ������.
	/// @param nQuestItemID  ����Ʈ ������ ID
	void RouteObtainQuestItem(unsigned long int nQuestItemID);
	/// �ش� �Ϲ� �������� �Ծ��ٰ� �޼��� ������.
	/// @param nItemID  �Ϲ� ������ ID
	void RouteObtainZItem(unsigned long int nItemID);
	/// ���� ����ġ ���Ʈ
	void RouteSectorBonus(const MUID& uidPlayer, unsigned long int nEXPValue, unsigned long int nBP);
	/// ���� ���� ���� ��ȭ�� �޼��� ������.
	void RouteCombatState();
	/// ����Ʈ ���� ����
	bool MakeQuestLevel();
	/// ���� ���� ó�� �۾�
	/// - ���߿� �Ϸ��� Combat ���� ������ Survival���鶧 MMatchRuleBaseQuest�� �Ű����� �Ѵ�.
	void CombatProcess();
	/// ���� ���ͷ� �̵�
	void MoveToNextSector();			
	/// ���� ���� ���� ��ȯ
	void SetCombatState(MQuestCombatState nState);
	/// ���� ���ͷ� �̵��Ϸ�Ǿ����� üũ
	bool CheckReadytoNewSector();
	/// ���� ���尡 �������� üũ�Ѵ�.
	COMBAT_PLAY_RESULT CheckCombatPlay();
	/// ����Ʈ�� ��� �������� üũ�Ѵ�.
	bool CheckQuestCompleted();
	/// ����Ʈ Complete�ϰ� ���� ������ ���� �� �ִ� �����ð� ���
	bool CheckQuestCompleteDelayTime();
	/// ���� Ŭ����� ȣ��ȴ�.
	void OnSectorCompleted();
	/// ���� Ŭ����� ���� Xp Bp �� ó��
	void RewardSectorXpBp();
	/// ���� ���� ó�� �۾�
	void ProcessCombatPlay();

	/// �ش� ���� ���� ó�� �����Ҷ�
	void OnBeginCombatState(MQuestCombatState nState);
	/// �ش� ���� ���� ��������
	void OnEndCombatState(MQuestCombatState nState);

	///< ���������� ���� �й�
	void MakeRewardList();
	///< ����ġ�� �ٿ�Ƽ�� ���.
	//void DistributeXPnBP( MQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL );	// �����̹����� ������
	///< ����Ʈ���� ���� ����Ʈ ������ �й�.		
	//bool DistributeQItem( MQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob); 	// �����̹����� ������
	///< ����Ʈ���� ���� �Ϲ� ������ �й�.		
	//bool DistributeZItem( MQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob);	// �����̹����� ������
protected:
	virtual void OnBegin();								///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();								///< ��ü ���� ����� ȣ��
	virtual bool OnRun();								///< ����ƽ�� ȣ��
	virtual void OnCommand(MCommand* pCommand);			///< ����Ʈ������ ����ϴ� Ŀ�ǵ� ó��
	virtual bool OnCheckRoundFinish();					///< ���尡 �������� üũ
public:
	MMatchRuleSurvival(MMatchStage* pStage);				///< ������
	virtual ~MMatchRuleSurvival();							///< �Ҹ���

	virtual void RefreshStageGameInfo();

	/// NPC�� �׿����� ȣ��
	/// @param uidSender		�޼��� ���� �÷��̾�
	/// @param uidKiller		���� �÷��̾�
	/// @param uidNPC			���� NPC
	/// @param pos				NPC ��ġ
	virtual void OnRequestNPCDead(MUID& uidSender, MUID& uidKiller, MUID& uidNPC, MVector& pos);

	/// �÷��̾� �׾��� �� ȣ��
	/// @param uidVictim		���� �÷��̾� UID
	virtual void OnRequestPlayerDead(const MUID& uidVictim);

	/// ���� �������� �Ծ��� ��� ȣ��ȴ�.
	/// @param pObj				�÷��̾� ������Ʈ
	/// @param nItemID			���� ������ ID
	/// @param nQuestItemID		����Ʈ ������ ID
	virtual void OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues);

	virtual void OnRequestTestSectorClear();
	virtual void OnRequestTestFinish();

	/// �÷��̾ ��Ż�� �̵����� ��� ȣ��ȴ�.
	/// @param uidPlayer			�̵��� �÷��̾� UID
	virtual void OnRequestMovetoPortal(const MUID& uidPlayer);
	/// ��Ż�� �̵��ϰ� ���� �̵��� �Ϸ�Ǿ��� ��� ȣ��ȴ�.
	/// @param uidPlayer			�÷��̾� UID
	virtual void OnReadyToNewSector(const MUID& uidPlayer);

	virtual void OnRequestDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnResponseDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnRequestCallbackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnResponseCallBackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnRequestQL( const MUID& uidSender );
	virtual void OnResponseQL_ToStage( const MUID& uidStage );
	virtual void OnRequestSacrificeSlotInfo( const MUID& uidSender );
	virtual void OnResponseSacrificeSlotInfoToListener( const MUID& uidSender );
	virtual void OnResponseSacrificeSlotInfoToStage( const MUID& uidStage );
	virtual void OnChangeCondition();

	virtual bool							PrepareStart();
	virtual bool							IsSacrificeItemDuplicated( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void							PreProcessLeaveStage( const MUID& uidLeaverUID );
	virtual void							DestroyAllSlot() {} // �����̹����� ������
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_SURVIVAL; }


	void InsertNoParamQItemToPlayer( MMatchObject* pPlayer, MQuestItem* pQItem );

	void PostInsertQuestGameLogAsyncJob();

	// ������ �����Ҷ� �ʿ��� ���� ����.
	void CollectStartingQuestGameLogInfo();
	// ������ ������ �ʿ��� ���� ����.
	void CollectEndQuestGameLogInfo();

	//void RouteRewardCommandToStage( MMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob );
	void RouteResultCommandToStage( MMatchObject* pPlayer, int nReachedRound, int nPoint);

private :
	int CalcuOwnerQItemCount( const MUID& uidPlayer, const unsigned long nItemID );
	const bool PostNPCInfo();
	bool PostRankingList();

	void ReinforceNPC();					// npc �ɷ� ��ȭ; �ó����� �ݺ����� ȣ��
	bool CollectNPCListInThisScenario();	// ���� ���۽� �� �ó��������� ����� NPC ����� �ۼ�
	int GetCurrentRoundIndex();					// ���� ����
	void PostPlayerPrivateRanking();			// ��� �� �濡 �ִ� �������� ���� ��ŷ ������ ��û�Ѵ�
};


#endif