#include "stdafx.h"
#include "MMatchServer.h"
#include "MMatchRuleQuest.h"
#include "MQuestLevel.h"
#include "MQuestLevelGenerator.h"
#include "MBlobArray.h"
#include "MQuestFormula.h"
#include "MCommandCommunicator.h"
#include "MSharedCommandTable.h"
#include "MMatchTransDataType.h"
#include "MMatchConfig.h"
#include "MMatchFormula.h"
#include "MQuestItem.h"
#include "MMATH.H"
#include "MAsyncDBJob.h"
#include "MQuestNPCSpawnTrigger.h"
#include "MQuestItem.h"

MMatchRuleQuest::MMatchRuleQuest(MMatchStage* pStage) : MMatchRuleBaseQuest(pStage), m_pQuestLevel(NULL),
														m_nCombatState(MQUEST_COMBAT_NONE), m_nPrepareStartTime(0),
														m_nCombatStartTime(0), m_nQuestCompleteTime(0), m_nPlayerCount( 0 )
{
	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
		m_SacrificeSlot[ i ].Release();

	m_StageGameInfo.nQL = 0;
	m_StageGameInfo.nPlayerQL = 0;
	m_StageGameInfo.nMapsetID = 1;
	m_StageGameInfo.nScenarioID = MMatchServer::GetInstance()->GetQuest()->GetScenarioCatalogue()->GetDefaultStandardScenarioID();
}

MMatchRuleQuest::~MMatchRuleQuest()
{
	ClearQuestLevel();
}

// Route ������ ���� /////////////////////////////////////////////////////////////////
void MMatchRuleQuest::RouteMapSectorStart()
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_START, MUID(0,0));
	char nSectorIndex = char(m_pQuestLevel->GetCurrSectorIndex());
	pCmd->AddParameter(new MCommandParameterChar(nSectorIndex));
	pCmd->AddParameter(new MCommandParameterUChar(0));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteCombatState()
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_COMBAT_STATE, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterChar(char(m_nCombatState)));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteMovetoPortal(const MUID& uidPlayer)
{
	if (m_pQuestLevel == NULL) return;

	int nCurrSectorIndex = m_pQuestLevel->GetCurrSectorIndex();

	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_MOVETO_PORTAL, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterChar(char(nCurrSectorIndex)));
	pCmd->AddParameter(new MCommandParameterUChar(0));
	pCmd->AddParameter(new MCommandParameterUID(uidPlayer));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteReadyToNewSector(const MUID& uidPlayer)
{
	if (m_pQuestLevel == NULL) return;

	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_READYTO_NEWSECTOR, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(uidPlayer));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteObtainQuestItem(unsigned long int nQuestItemID)
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_OBTAIN_QUESTITEM, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUInt(nQuestItemID));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteObtainZItem(unsigned long int nItemID)
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_OBTAIN_ZITEM, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUInt(nItemID));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteGameInfo()
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_GAME_INFO, MUID(0,0));

	void* pBlobGameInfoArray = MMakeBlobArray(sizeof(MTD_QuestGameInfo), 1);
	MTD_QuestGameInfo* pGameInfoNode = (MTD_QuestGameInfo*)MGetBlobArrayElement(pBlobGameInfoArray, 0);

	if (m_pQuestLevel)
	{
		m_pQuestLevel->Make_MTDQuestGameInfo(pGameInfoNode, MMATCH_GAMETYPE_QUEST);
	}

	pCmd->AddParameter(new MCommandParameterBlob(pBlobGameInfoArray, MGetBlobArraySize(pBlobGameInfoArray)));
	MEraseBlobArray(pBlobGameInfoArray);

	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteCompleted()
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_COMPLETED, MUID(0,0));

	int nSize = (int)m_PlayerManager.size();
	void* pBlobRewardArray = MMakeBlobArray(sizeof(MTD_QuestReward), nSize);

	int idx = 0;
	for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		MQuestPlayerInfo* pPlayerInfo = (*itor).second;
		MTD_QuestReward* pRewardNode = (MTD_QuestReward*)MGetBlobArrayElement(pBlobRewardArray, idx);
		idx++;

		pRewardNode->uidPlayer = (*itor).first;
		pRewardNode->nXP = pPlayerInfo->nXP;
		pRewardNode->nBP = pPlayerInfo->nBP;
	}

	pCmd->AddParameter(new MCommandParameterBlob(pBlobRewardArray, MGetBlobArraySize(pBlobRewardArray)));
	MEraseBlobArray(pBlobRewardArray);

	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteFailed()
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_FAILED, MUID(0,0));
	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteStageGameInfo()
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_STAGE_GAME_INFO, MUID(0,0));
	pCmd->AddParameter(new MCmdParamChar(char(m_StageGameInfo.nQL)));
	pCmd->AddParameter(new MCmdParamChar(char(m_StageGameInfo.nMapsetID)));
	pCmd->AddParameter(new MCmdParamUInt(m_StageGameInfo.nScenarioID));

	MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void MMatchRuleQuest::RouteSectorBonus(const MUID& uidPlayer, unsigned long int nEXPValue, unsigned long int nBP)
{
	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject(uidPlayer);	
	if (!IsEnabledObject(pPlayer)) return;

	MCommand* pNewCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_BONUS, MUID(0,0));
	pNewCmd->AddParameter(new MCmdParamUID(uidPlayer));
	pNewCmd->AddParameter(new MCmdParamUInt(nEXPValue));
	pNewCmd->AddParameter(new MCmdParamUInt(nBP));	// BP�������� ����Ʈ������ ���� ����
	MMatchServer::GetInstance()->RouteToListener( pPlayer, pNewCmd );
}

// Route ������ �� ///////////////////////////////////////////////////////////////////

void MMatchRuleQuest::OnBegin()
{
	m_nQuestCompleteTime = 0;

	MakeQuestLevel();

	MMatchRuleBaseQuest::OnBegin();		// ���⼭ ���������� ���� - ������ ����

	// ������ �Ϸ� �Ͽ����� �����Ҷ��� �ο����� ���� ������ ���ؼ� ���� ���� ���� �����Ѵ�.
	m_nPlayerCount = static_cast< int >( m_PlayerManager.size() );

	// ���ӽ����ϸ� ������ ��� ������ ��.
	// ��������� �α� ������ DestroyAllSlot()���� m_QuestGameLogInfoMgr�� ����.
	DestroyAllSlot();

	// ���� �������� Log�� �ʿ��� ������ ������.
	CollectStartingQuestGameLogInfo();

	SetCombatState(MQUEST_COMBAT_PREPARE);
}

void MMatchRuleQuest::OnEnd()
{
	ClearQuestLevel();

	MMatchRuleBaseQuest::OnEnd();
}

bool MMatchRuleQuest::OnRun()
{
	bool ret = MMatchRuleBaseQuest::OnRun();
	if (ret == false) return false;

	if (GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		CombatProcess();
	}

	return true;
}


// ������ �� ��������.
void MMatchRuleQuest::CombatProcess()
{
	switch (m_nCombatState)
	{
	case MQUEST_COMBAT_PREPARE:			// ��ε� ���ͷ� �����⸦ ��ٸ��� �ñ�
		{
			if (CheckReadytoNewSector())		// ��� �� ���Ϳ� ���ö����� PREPARE
			{
				SetCombatState(MQUEST_COMBAT_PLAY);				
			};
		}
		break;
	case MQUEST_COMBAT_PLAY:			// ���� ���� �÷��� �ñ�
		{
			COMBAT_PLAY_RESULT nResult = CheckCombatPlay();
			switch(nResult)
			{
			case CPR_PLAYING:
				{
					ProcessCombatPlay();
				}
				break;
			case CPR_COMPLETE:
				{
					if (CheckQuestCompleteDelayTime())
					{
						SetCombatState(MQUEST_COMBAT_COMPLETED);
					}
				}
				break;
			case CPR_FAILED:
				{
					// ������� �������� �� ���� Ŭ�������� ������ �������θ� �˻��ؼ� ������ ��������... - by �߱���.
					// OnFail�� OnCheckRoundFinish���� ó���ϴ� �������� ��������.
					// SetCombatState(MQUEST_COMBAT_NONE);
					// m_bQuestCompleted = false;
					// OnFailed();
				}
				break;
			};
		}
		break;
	case MQUEST_COMBAT_COMPLETED:			// ������ ������ ���� ��ũ�� �ǳʰ��� �ñ�
		{
			// ����Ʈ Ŭ��� �ƴϰ� ���� ���Ͱ� ���� ������ �ٷ� PREPARE���°� �ȴ�.
			if (!m_bQuestCompleted)
			{
                SetCombatState(MQUEST_COMBAT_PREPARE);
			}
		}
		break;
	};
}


void MMatchRuleQuest::OnBeginCombatState(MQuestCombatState nState)
{
#ifdef _DEBUG
	mlog( "Quest state : %d.\n", nState );
#endif

	switch (nState)
	{
	case MQUEST_COMBAT_PREPARE:
		{
			m_nPrepareStartTime = MMatchServer::GetInstance()->GetTickTime();
		}
		break;
	case MQUEST_COMBAT_PLAY:
		{
			m_nCombatStartTime = MMatchServer::GetInstance()->GetTickTime();
			// ��������� �ʱ�ȭ
			m_pStage->m_WorldItemManager.OnRoundBegin();
			m_pStage->m_ActiveTrapManager.Clear();
			m_pStage->ResetPlayersCustomItem();

			RouteMapSectorStart();

			// ��� ��Ȱ
			if (m_pQuestLevel->GetCurrSectorIndex() != 0)
				RefreshPlayerStatus();

			// ���� ���� �̵����� �÷��� ��
			for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
			{
				MQuestPlayerInfo* pPlayerInfo = (*itor).second;
				pPlayerInfo->bMovedtoNewSector = false;
			}
		}
		break;
	case MQUEST_COMBAT_COMPLETED:
		{
			if (CheckQuestCompleted())
			{
				OnCompleted();
			}
			else if( !CheckPlayersAlive() )
			{
				// ������ �߰��� ����.
				OnFailed();
			}
			else
			{
				OnSectorCompleted();
			}
		}
		break;
	};
}

void MMatchRuleQuest::OnEndCombatState(MQuestCombatState nState)
{
	switch (nState)
	{
	case MQUEST_COMBAT_PREPARE:
		break;
	case MQUEST_COMBAT_PLAY:
		break;
	case MQUEST_COMBAT_COMPLETED:
		break;
	};
}

MMatchRuleQuest::COMBAT_PLAY_RESULT MMatchRuleQuest::CheckCombatPlay()
{
	// �����濡�� ������ ������ Complete
	if ( m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
	{
		// ������ �������� �˻��Ѵ�.
//		if ( (m_pQuestLevel) && (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1)))
		{
			if ( m_NPCManager.IsBossDie())
				return CPR_COMPLETE;
		}
	}

	// ��� ���� �� �׿����� Complete
	if ((m_pQuestLevel->GetNPCQueue()->IsEmpty()) && (m_NPCManager.GetNPCObjectCount() <= 0))
	{
		return CPR_COMPLETE;
	}

	// ��� ������ �׾����� ���� ���з� ������.
	if( !CheckPlayersAlive() )
	{
		return CPR_FAILED;
	}

	return CPR_PLAYING;
}

void MMatchRuleQuest::OnCommand(MCommand* pCommand)
{
	MMatchRuleBaseQuest::OnCommand(pCommand);
}


///
// First : 
// Last  : 2005.04.27 �߱���.
//
// ����������� ���Կ� �÷�������, QL���� ��������� ���̺��� �����ۿ� �ش��ϴ� ���̺��� �ִ��� �˻��ϱ� ���� ȣ���.
//  �������� ���Կ� �÷��������� QL���� ����� ��. ��������� ���̺� �˻� ����� ������ ����.
//  ������ �����ҽÿ��� ��������� ���̺� �˻� ����� �����϶��� ������ ������.
///
bool MMatchRuleQuest::MakeQuestLevel()
{
	// ������ ����Ʈ ���� ������ ������.
	if( 0 != m_pQuestLevel )
	{
		delete m_pQuestLevel;
		m_pQuestLevel = 0;
	}

	MQuestLevelGenerator	LG( GetGameType() );

	LG.BuildPlayerQL(m_StageGameInfo.nPlayerQL);
	LG.BuildMapset(m_StageGameInfo.nMapsetID);
	
	for (int i = 0; i < MAX_SCENARIO_SACRI_ITEM; i++)
	{
		LG.BuildSacriQItem(m_SacrificeSlot[i].GetItemID());
	}

	m_pQuestLevel = LG.MakeLevel();


	// ù���ͺ��� �������� �� �����Ƿ�..
	InitJacoSpawnTrigger();

	return true;
}

void MMatchRuleQuest::ClearQuestLevel()
{
	if (m_pQuestLevel)
	{
		delete m_pQuestLevel;
		m_pQuestLevel = NULL;
	}
}




void MMatchRuleQuest::MoveToNextSector()
{
	// m_pQuestLevel�� ���������� �̵����ش�.
	m_pQuestLevel->MoveToNextSector(GetGameType());

	for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		MQuestPlayerInfo* pPlayerInfo = (*itor).second;
		pPlayerInfo->bMovedtoNewSector = false;
	}

	InitJacoSpawnTrigger();	
}

void MMatchRuleQuest::InitJacoSpawnTrigger()
{
	// ���� ���� ���Ͱ� ���������̸� JacoTrigger �ߵ�
	if (m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
	{
		int nDice = m_pQuestLevel->GetStaticInfo()->nDice;
		MQuestScenarioInfoMaps* pMap = &m_pQuestLevel->GetStaticInfo()->pScenario->Maps[nDice];

		SpawnTriggerInfo info;
		
		info.nSpawnNPCCount = pMap->nJacoCount;
		info.nSpawnTickTime = pMap->nJacoSpawnTickTime;
		info.nCurrMinNPCCount = pMap->nJacoMinNPCCount;
		info.nCurrMaxNPCCount = pMap->nJacoMaxNPCCount;

		m_JacoSpawnTrigger.Clear();
		m_JacoSpawnTrigger.BuildCondition(info);

		for (vector<MQuestScenarioInfoMapJaco>::iterator itor = pMap->vecJacoArray.begin(); itor != pMap->vecJacoArray.end(); ++itor)
		{
			SpawnTriggerNPCInfoNode node;
			node.nNPCID = (*itor).nNPCID;
			node.fRate = (*itor).fRate;

			m_JacoSpawnTrigger.BuildNPCInfo(node);
		}
	}
}

void MMatchRuleQuest::SetCombatState(MQuestCombatState nState)
{
	if (m_nCombatState == nState) return;

	OnEndCombatState(m_nCombatState);
	m_nCombatState = nState;
	OnBeginCombatState(m_nCombatState);

	RouteCombatState();
}


bool MMatchRuleQuest::CheckReadytoNewSector()
{
	// ���� �ð��� ������ �ٷ� ���� ���ͷ� �̵��Ѵ�.
	unsigned long nNowTime = MMatchServer::GetInstance()->GetTickTime();
	if ((nNowTime - m_nPrepareStartTime) > PORTAL_MOVING_TIME)
	{
		return true;
	}

	for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		MQuestPlayerInfo* pPlayerInfo = (*itor).second;
		if ((pPlayerInfo->pObject->CheckAlive()) && (pPlayerInfo->bMovedtoNewSector == false)) return false;
	}
	
	return true;
}

// ���� Ŭ����
void MMatchRuleQuest::OnSectorCompleted()
{
	// ���� ���ʽ�
	MQuestScenarioInfo* pScenario = m_pQuestLevel->GetStaticInfo()->pScenario;
	if (pScenario)
	{
		int nSectorXP = pScenario->nSectorXP;
		int nSectorBP = pScenario->nSectorBP;

		// ���� ���ʽ��� ���
		if (nSectorXP < 0)
		{
			int nSectorCount = (int)m_pQuestLevel->GetStaticInfo()->SectorList.size();
			nSectorXP = MQuestFormula::CalcSectorXP(pScenario->nXPReward, nSectorCount);
		}
		// ���� ���ʽ��� ���
		if (nSectorBP < 0)
		{
			int nSectorCount = (int)m_pQuestLevel->GetStaticInfo()->SectorList.size();
			nSectorBP = MQuestFormula::CalcSectorXP(pScenario->nBPReward, nSectorCount);
		}

		if ((nSectorXP > 0) || (nSectorBP > 0))
		{
			for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
			{
				int nAddedSectorXP = nSectorXP;
				int nAddedSectorBP = nSectorBP;

				MMatchObject* pPlayer = (*itor).second->pObject;
				if ((!IsEnabledObject(pPlayer)) || (!pPlayer->CheckAlive())) continue;

				// ����ġ, �ٿ�Ƽ ���ʽ� ���
				const float fXPBonusRatio = MMatchFormula::CalcXPBonusRatio(pPlayer, MIBT_QUEST);
				const float fBPBonusRatio = MMatchFormula::CalcBPBounsRatio(pPlayer, MIBT_QUEST);
				nAddedSectorXP += (int)(nAddedSectorXP * fXPBonusRatio);
				nAddedSectorBP += (int)(nAddedSectorBP * fBPBonusRatio);

				// ���� ����
				MGetMatchServer()->ProcessPlayerXPBP(m_pStage, pPlayer, nAddedSectorXP, nAddedSectorBP);

				// �����
				int nExpPercent = MMatchFormula::GetLevelPercent(pPlayer->GetCharInfo()->m_nXP, 
																pPlayer->GetCharInfo()->m_nLevel);
				unsigned long int nExpValue = MakeExpTransData(nAddedSectorXP, nExpPercent);
				RouteSectorBonus(pPlayer->GetUID(), nExpValue, nAddedSectorBP);
			}
		}
	}

	// ���� ��� ��Ȱ��Ų��.
//	RefreshPlayerStatus();

	MoveToNextSector();
}

// ����Ʈ ������
void MMatchRuleQuest::OnCompleted()
{
	MMatchRuleBaseQuest::OnCompleted();

#ifdef _QUEST_ITEM
	// ���⼭ DB�� QuestGameLog����.
	PostInsertQuestGameLogAsyncJob();	
	SetCombatState(MQUEST_COMBAT_NONE);
#endif
	
}

// ����Ʈ ���н�
void MMatchRuleQuest::OnFailed()
{
	SetCombatState(MQUEST_COMBAT_NONE);
	m_bQuestCompleted = false;

	MMatchRuleBaseQuest::OnFailed();

	PostInsertQuestGameLogAsyncJob();
}

// ����Ʈ�� ��� �������� üũ
bool MMatchRuleQuest::CheckQuestCompleted()
{
	if (m_pQuestLevel)
	{
		// �ʹ� ���� �������� üũ
		unsigned long int nStartTime = GetStage()->GetStartTime();
		unsigned long int nNowTime = MMatchServer::GetInstance()->GetTickTime();

		// �ּ��� �� ���ͺ� ���� ���� ������ * ���ͼ���ŭ�� �ð��� �귯�� ������ ���� �� �ִٰ� ������.
		unsigned long int nCheckTime = QUEST_COMBAT_PLAY_START_DELAY * m_pQuestLevel->GetMapSectorCount();

		if (MGetTimeDistance(nStartTime, nNowTime) < nCheckTime) return false;


		if (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1))
		{
			return true;
		}
	}

	return false;
}

// ������ ���ʹ� �������� ���� �� �ֵ��� ������ �ð��� �д�.
bool MMatchRuleQuest::CheckQuestCompleteDelayTime()
{
	if ((m_pQuestLevel) && (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1)))
	{
		unsigned long int nNowTime = MMatchServer::GetInstance()->GetTickTime();
		if (m_nQuestCompleteTime == 0)
			m_nQuestCompleteTime = nNowTime;
		if (MGetTimeDistance(m_nQuestCompleteTime, nNowTime) > QUEST_COMPLETE_DELAY)
			return true;

		return false;
	}

	return true;
}

void MMatchRuleQuest::ProcessCombatPlay()
{
	ProcessNPCSpawn();

}

void MMatchRuleQuest::MakeNPCnSpawn(MQUEST_NPC nNPCID, bool bAddQuestDropItem)
{
	MQuestNPCSpawnType nSpawnType = MNST_MELEE;
	MQuestNPCInfo* pNPCInfo = MMatchServer::GetInstance()->GetQuest()->GetNPCInfo(nNPCID);
	if (pNPCInfo)
	{
		nSpawnType = pNPCInfo->GetSpawnType();
		int nPosIndex = m_pQuestLevel->GetRecommendedSpawnPosition(nSpawnType, MMatchServer::GetInstance()->GetTickTime());

		MMatchNPCObject* pNPCObject = SpawnNPC(nNPCID, nPosIndex);

		if (pNPCObject)
		{
			// drop item ����
			MQuestDropItem item;
			int nDropTableID = pNPCInfo->nDropTableID;
			int nQL = m_pQuestLevel->GetStaticInfo()->nQL;
			MMatchServer::GetInstance()->GetQuest()->GetDropTable()->Roll(item, nDropTableID, nQL);

			// AddQuestDropItem=false�̸� ��������۸� ����Ѵ�.
			if ((bAddQuestDropItem==true) || (item.nDropItemType == QDIT_WORLDITEM))
			{
				pNPCObject->SetDropItem(&item);

				// ������� �������� level�� �־���´�.
				if ((item.nDropItemType == QDIT_QUESTITEM) || (item.nDropItemType == QDIT_ZITEM))
				{
					m_pQuestLevel->OnItemCreated((unsigned long int)(item.nID), item.nRentPeriodHour);
				}
			}
		}
	}
}

void MMatchRuleQuest::ProcessNPCSpawn()
{
	if (CheckNPCSpawnEnable())
	{
		MQUEST_NPC npc;
		if (m_pQuestLevel->GetNPCQueue()->Pop(npc))
		{
			MakeNPCnSpawn(npc, true);
		}
	}
	else
	{
		// �������� ��� Queue�� �ִ� NPC���� ��� ������������ Jaco���� ������Ų��.
		if (m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
		{
			// ������ ����ְ� �⺻������ ���� NPC�� �� ���´����� ������ ����
			if ((m_NPCManager.GetBossCount() > 0) /* && (m_pQuestLevel->GetNPCQueue()->IsEmpty()) */ )
			{
				int nAliveNPCCount = m_NPCManager.GetNPCObjectCount();
				

				if (m_JacoSpawnTrigger.CheckSpawnEnable(nAliveNPCCount))
				{
					int nCount = (int)m_JacoSpawnTrigger.GetQueue().size();
					for (int i = 0; i < nCount; i++)
					{
						MQUEST_NPC npc = m_JacoSpawnTrigger.GetQueue()[i];
						MakeNPCnSpawn(npc, false);
					}
				}
			}
		}
	}
}


bool MMatchRuleQuest::CheckNPCSpawnEnable()
{
	if (m_pQuestLevel->GetNPCQueue()->IsEmpty()) return false;

	if (m_NPCManager.GetNPCObjectCount() >= m_pQuestLevel->GetStaticInfo()->nLMT) return false;
	unsigned long int nNowTime = MMatchServer::GetInstance()->GetTickTime();

	if ((nNowTime - m_nCombatStartTime) < QUEST_COMBAT_PLAY_START_DELAY)
	{
		return false;
	}


	return true;

}

void MMatchRuleQuest::OnRequestTestSectorClear()
{
	ClearAllNPC();

	SetCombatState(MQUEST_COMBAT_COMPLETED);
}

void MMatchRuleQuest::OnRequestTestFinish()
{
	ClearAllNPC();

	m_pQuestLevel->GetDynamicInfo()->nCurrSectorIndex = m_pQuestLevel->GetMapSectorCount()-1;

	SetCombatState(MQUEST_COMBAT_COMPLETED);
}


void MMatchRuleQuest::OnRequestMovetoPortal(const MUID& uidPlayer)
{
//	MQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidPlayer);

	RouteMovetoPortal(uidPlayer);
}




void MMatchRuleQuest::OnReadyToNewSector(const MUID& uidPlayer)
{
	MQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidPlayer);
	if (pPlayerInfo)
	{
        pPlayerInfo->bMovedtoNewSector = true;
	}

	RouteReadyToNewSector(uidPlayer);
}

bool MMatchRuleQuest::OnCheckRoundFinish()
{
	return MMatchRuleBaseQuest::OnCheckRoundFinish();
}

void MMatchRuleQuest::DistributeReward()
{
	if (!m_pQuestLevel) return;

	// ���� ������ ����Ʈ ������ ��쿡�� �����ϰ� ��.
	if( MSM_TEST != MGetServerConfig()->GetServerMode() )  return;

	MQuestScenarioInfo* pScenario = m_pQuestLevel->GetStaticInfo()->pScenario;
	if (!pScenario) return;

	MMatchObject*					pPlayer;



	const int nRewardXP		= pScenario->nXPReward;
	const int nRewardBP		= pScenario->nBPReward;
	const int nScenarioQL	= pScenario->nQL;

	MakeRewardList();		// ������ ���

	for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		MQuestPlayerInfo* pPlayerInfo = (*itor).second;

		// ����ġ, �ٿ�Ƽ ���
		DistributeXPnBP( pPlayerInfo, nRewardXP, nRewardBP, nScenarioQL );

		pPlayer = MMatchServer::GetInstance()->GetObject((*itor).first);
		if( !IsEnabledObject(pPlayer) ) continue;

		// ����Ʈ ������ ���
		void* pSimpleQuestItemBlob = NULL;
		if (!DistributeQItem( pPlayerInfo, &pSimpleQuestItemBlob )) continue;

		// �Ϲ� ������ ���
		void* pSimpleZItemBlob = NULL;
		if (!DistributeZItem( pPlayerInfo, &pSimpleZItemBlob )) continue;

		// DB����ȭ ���� �˻�.
		pPlayer->GetCharInfo()->GetDBQuestCachingData().IncreasePlayCount();

		// Ŀ�ǵ� ����.
		RouteRewardCommandToStage( pPlayer, (*itor).second->nXP, (*itor).second->nBP, pSimpleQuestItemBlob, pSimpleZItemBlob );

		MEraseBlobArray( pSimpleQuestItemBlob );

		MGetMatchServer()->ResponseCharacterItemList( pPlayer->GetUID() );
	}

}


void MMatchRuleQuest::InsertNoParamQItemToPlayer( MMatchObject* pPlayer, MQuestItem* pQItem )
{
	if( !IsEnabledObject(pPlayer) || (0 == pQItem) ) return;

	MQuestItemMap::iterator itMyQItem = pPlayer->GetCharInfo()->m_QuestItemList.find( pQItem->GetItemID() );

	if( pPlayer->GetCharInfo()->m_QuestItemList.end() != itMyQItem )
	{
		// ������ ������ �ִ� ����Ʈ ������. ������ ���� �����ָ� ��.
		const int nOver = itMyQItem->second->Increase( pQItem->GetCount() );
		if( 0 < nOver )
			pQItem->Decrease( nOver );
	}
	else
	{
		// ó�� ȹ���� ����Ʈ ������. ���� �߰����� ��� ��.
		if( !pPlayer->GetCharInfo()->m_QuestItemList.CreateQuestItem(pQItem->GetItemID(), pQItem->GetCount(), pQItem->IsKnown()) )
			mlog( "MMatchRuleQuest::DistributeReward - %d��ȣ �������� Create( ... )�Լ� ȣ�� ����.\n", pQItem->GetItemID() );
	}
}


void MMatchRuleQuest::MakeRewardList()
{
	int								nPos;
	int								nPlayerCount;
	int								nLimitRandNum;
	MQuestItem*						pRewardQItem;
	MQuestLevelItemMap::iterator	itObtainQItem, endObtainQItem;
	MQuestLevelItem*				pObtainQItem;

	nPlayerCount	= static_cast< int >( m_PlayerManager.size() );
	endObtainQItem	= m_pQuestLevel->GetDynamicInfo()->ItemMap.end();
	nLimitRandNum	= m_nPlayerCount - 1;

	vector<MQuestPlayerInfo*>	a_vecPlayerInfos;
	for (MQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		MQuestPlayerInfo* pPlayerInfo = (*itor).second;

		// Ȥ�� ���� ������ ������ �������� ���������� �𸣴� �ʱ�ȭ.
		pPlayerInfo->RewardQuestItemMap.Clear();
		pPlayerInfo->RewardZItemList.clear();

		a_vecPlayerInfos.push_back(pPlayerInfo);
	}

	for( itObtainQItem = m_pQuestLevel->GetDynamicInfo()->ItemMap.begin(); itObtainQItem != endObtainQItem; ++itObtainQItem )
	{
		pObtainQItem = itObtainQItem->second;

		// ȹ������ �������� ����.
		if (!pObtainQItem->bObtained) continue;	

		if (pObtainQItem->IsQuestItem())
		{
			// ����Ʈ ������ -----------------------------------------------------

			// �����Ҷ��� �ο��������� roll�� ��.
			nPos = RandomNumber( 0, nLimitRandNum );

			// ���� �����ִ� �ο����� Ŭ��� �׳� ����.
			if (( nPos < nPlayerCount ) && (nPos < (int)a_vecPlayerInfos.size()))
			{
				// ����Ʈ �������� ��� ó��
				MQuestItemMap* pRewardQuestItemMap = &a_vecPlayerInfos[ nPos ]->RewardQuestItemMap;

				pRewardQItem = pRewardQuestItemMap->Find( pObtainQItem->nItemID );
				if( 0!= pRewardQItem )
					pRewardQItem->Increase(); // ������ ȹ���� ������.
				else
				{
					// ó�� ȹ��.
					if( !pRewardQuestItemMap->CreateQuestItem(pObtainQItem->nItemID, 1) )
					{
						mlog( "MMatchRuleQuest::MakeRewardList - ItemID:%d ó�� ȹ���� ������ ���� ����.\n", pObtainQItem->nItemID );
						continue;
					}
				}
			}
		}
		else
		{
			// �Ϲ� �������� ��� ó�� -------------------------------------------

			
			
			RewardZItemInfo iteminfo;
			iteminfo.nItemID = pObtainQItem->nItemID;
			iteminfo.nRentPeriodHour = pObtainQItem->nRentPeriodHour;

			int nLoopCounter = 0;
			const int MAX_LOOP_COUNT = 5;

			// �ִ� 5������ �������� �������� ������ ���� ����� ã�´�.
			while (nLoopCounter < MAX_LOOP_COUNT)
			{
				nLoopCounter++;

				// �����Ҷ��� �ο��������� roll�� ��.
				nPos = RandomNumber( 0, nLimitRandNum );

				// ���� �����ִ� �ο����� Ŭ��� �׳� ����.
				if (( nPos < nPlayerCount ) && (nPos < (int)a_vecPlayerInfos.size()))
				{
					MQuestPlayerInfo* pPlayerInfo = a_vecPlayerInfos[ nPos ];
					MQuestRewardZItemList* pRewardZItemList = &pPlayerInfo->RewardZItemList;

					// ������ ���ƾ߸� ���� �� �ִ�.
					if (IsEnabledObject(pPlayerInfo->pObject))
					{
						if (IsEquipableItem(iteminfo.nItemID, MAX_LEVEL, pPlayerInfo->pObject->GetCharInfo()->m_nSex))
						{
							pRewardZItemList->push_back(iteminfo);
							break;
						}
					}
				}
			}
		}

	}
}


///< ����ġ�� �ٿ�Ƽ ��� �ű�. -by �߱���.
void MMatchRuleQuest::DistributeXPnBP( MQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL )
{
	float fXPRate, fBPRate;

	MQuestFormula::CalcRewardRate(fXPRate, 
								  fBPRate,
								  nScenarioQL, 
								  pPlayerInfo->nQL,
								  pPlayerInfo->nDeathCount, 
								  pPlayerInfo->nUsedPageSacriItemCount, 
								  pPlayerInfo->nUsedExtraSacriItemCount);

	pPlayerInfo->nXP = int(nRewardXP * fXPRate);
	pPlayerInfo->nBP = int(nRewardBP * fBPRate);


	// ������ ����ġ, �ٿ�Ƽ ����
	if (IsEnabledObject(pPlayerInfo->pObject))
	{
		// ����ġ ���ʽ� ���
		const float fXPBonusRatio = MMatchFormula::CalcXPBonusRatio(pPlayerInfo->pObject, MIBT_QUEST);
		const float fBPBonusRatio = MMatchFormula::CalcBPBounsRatio(pPlayerInfo->pObject, MIBT_QUEST);

		int nExpBonus = (int)(pPlayerInfo->nXP * fXPBonusRatio);
		pPlayerInfo->nXP += nExpBonus;

		int nBPBonus = (int)(pPlayerInfo->nBP * fBPBonusRatio);
		pPlayerInfo->nBP += nBPBonus;

		MMatchServer::GetInstance()->ProcessPlayerXPBP(m_pStage, pPlayerInfo->pObject, pPlayerInfo->nXP, pPlayerInfo->nBP);
	}
}

// ����Ʈ ������ ���
bool MMatchRuleQuest::DistributeQItem( MQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob)
{
	MMatchObject* pPlayer = pPlayerInfo->pObject;
	if (!IsEnabledObject(pPlayer)) return false;

	MQuestItemMap* pObtainQuestItemMap = &pPlayerInfo->RewardQuestItemMap;

	// Client�� �����Ҽ� �ִ� ���·� Quest item������ ������ Blob����.
	void* pSimpleQuestItemBlob = MMakeBlobArray( sizeof(MTD_QuestItemNode), static_cast<int>(pObtainQuestItemMap->size()) );
	if( 0 == pSimpleQuestItemBlob )
	{
		mlog( "MMatchRuleQuest::DistributeReward - Quest item ������ ���� Blob������ ����.\n" );
		return false;
	}

	// �α׸� ���ؼ� �ش� ������ ���� �������� ������ ������ ����.
	if( !m_QuestGameLogInfoMgr.AddRewardQuestItemInfo(pPlayer->GetUID(), pObtainQuestItemMap) )
	{
		mlog( "m_QuestGameLogInfoMgr -�ش� ������ �αװ�ü�� ã�µ� ����." );
	}

	int nBlobIndex = 0;
	for(MQuestItemMap::iterator itQItem = pObtainQuestItemMap->begin(); itQItem != pObtainQuestItemMap->end(); ++itQItem )
	{
		MQuestItem* pQItem = itQItem->second;
		MQuestItemDesc* pQItemDesc = pQItem->GetDesc();
		if( 0 == pQItemDesc )
		{
			mlog( "MMatchRuleQuest::DistributeReward - %d �������� ��ũ���� ������ �Ǿ����� ����.\n", pQItem->GetItemID() );
			continue;
		}

		// ����ũ ���������� �˻縦 ��.
		pPlayer->GetCharInfo()->m_DBQuestCachingData.CheckUniqueItem( pQItem );
		// ������� Ƚ���� �˻縦 ��.
		pPlayer->GetCharInfo()->m_DBQuestCachingData.IncreaseRewardCount();

		if( MMQIT_MONBIBLE == pQItemDesc->m_nType )
		{
			// ���� ���� ó��.
			if( !pPlayer->GetCharInfo()->m_QMonsterBible.IsKnownMonster(pQItemDesc->m_nParam) )
				pPlayer->GetCharInfo()->m_QMonsterBible.WriteMonsterInfo( pQItemDesc->m_nParam );
		}
		else if( 0 != pQItemDesc->m_nParam )
		{
			// Param���� �����Ǿ� �ִ� �������� ���� ó���� ����� ��.				
		}
		else
		{
			// DB�� ������ �Ǵ� ����Ʈ �����۸� �������� ������.
			InsertNoParamQItemToPlayer( pPlayer, pQItem );
		}

		MTD_QuestItemNode* pQuestItemNode;
		pQuestItemNode = reinterpret_cast< MTD_QuestItemNode* >( MGetBlobArrayElement(pSimpleQuestItemBlob, nBlobIndex++) );
		Make_MTDQuestItemNode( pQuestItemNode, pQItem->GetItemID(), pQItem->GetCount() );
	}

	*ppoutSimpleQuestItemBlob = pSimpleQuestItemBlob;
	return true;
}

bool MMatchRuleQuest::DistributeZItem( MQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob)
{
	MMatchObject* pPlayer = pPlayerInfo->pObject;
	if (!IsEnabledObject(pPlayer)) return false;

	// ������ ������ MAX_QUEST_REWARD_ITEM_COUNT(500��)���� ������ �Ϲݾ������� �߰������� �ʴ´�.
	if(!MGetMatchServer()->CheckUserCanDistributeRewardItem(pPlayer))
	{
		*ppoutQuestRewardZItemBlob = MMakeBlobArray( sizeof(MTD_QuestZItemNode), 0 );
		return true;	// ��, �ٸ��͵�(XP, BP, ����Ʈ��� ������ ��)�� ������Ʈ ���ش�.
	}

	MQuestRewardZItemList* pObtainZItemList = &pPlayerInfo->RewardZItemList;

	// Client�� �����Ҽ� �ִ� ���·� Quest item������ ������ Blob����.
	void* pSimpleZItemBlob = MMakeBlobArray( sizeof(MTD_QuestZItemNode), (int)(pObtainZItemList->size()) );
	if( 0 == pSimpleZItemBlob )
	{
		mlog( "MMatchRuleQuest::DistributeZItem - Ztem ������ ���� Blob������ ����.\n" );
		return false;
	}

	// ĳ�� ������ ȹ�� �α׸� ����� ����.
	if( !m_QuestGameLogInfoMgr.AddRewardZItemInfo(pPlayer->GetUID(), pObtainZItemList) )
	{
		mlog( "m_QuestGameLogInfoMgr -�ش� ������ �αװ�ü�� ã�µ� ����." );
	}

	int nBlobIndex = 0;
	for(MQuestRewardZItemList::iterator itor = pObtainZItemList->begin(); itor != pObtainZItemList->end(); ++itor )
	{
		RewardZItemInfo iteminfo = (*itor);
		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(iteminfo.nItemID);

		if (pItemDesc == NULL) continue;
		if (!IsEquipableItem(iteminfo.nItemID, MAX_LEVEL, pPlayer->GetCharInfo()->m_nSex)) 	continue;

		// ������ ������ ���
		// TodoH(��) - ����Ʈ ������ ���� ���� �κ�. �ϴ� ������ 1���� ����!
		MMatchServer::GetInstance()->DistributeZItem(pPlayer->GetUID(), iteminfo.nItemID, true, iteminfo.nRentPeriodHour, 1);

		// ��ӻ���
		MTD_QuestZItemNode* pZItemNode  = (MTD_QuestZItemNode*)(MGetBlobArrayElement(pSimpleZItemBlob, nBlobIndex++));
		pZItemNode->m_nItemID			= iteminfo.nItemID;
		pZItemNode->m_nRentPeriodHour	= iteminfo.nRentPeriodHour;
	}

	*ppoutQuestRewardZItemBlob = pSimpleZItemBlob;

	return true;
}

void MMatchRuleQuest::RouteRewardCommandToStage( MMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob)
{
	if( !IsEnabledObject(pPlayer) || (0 == pSimpleQuestItemBlob) )
		return;

	MCommand* pNewCmd = MMatchServer::GetInstance()->CreateCommand( MC_MATCH_USER_REWARD_QUEST, MUID(0, 0) );
	if( 0 == pNewCmd )
		return;

	pNewCmd->AddParameter( new MCmdParamInt(nRewardXP) );
	pNewCmd->AddParameter( new MCmdParamInt(nRewardBP) );
	pNewCmd->AddParameter( new MCommandParameterBlob(pSimpleQuestItemBlob, MGetBlobArraySize(pSimpleQuestItemBlob)) );
	pNewCmd->AddParameter( new MCommandParameterBlob(pSimpleZItemBlob, MGetBlobArraySize(pSimpleZItemBlob)) );

	MMatchServer::GetInstance()->RouteToListener( pPlayer, pNewCmd );
}



void MMatchRuleQuest::OnRequestPlayerDead(const MUID& uidVictim)
{
	MQuestPlayerManager::iterator itor = m_PlayerManager.find(uidVictim);
	if (itor != m_PlayerManager.end())
	{
		MQuestPlayerInfo* pPlayerInfo = (*itor).second;
		pPlayerInfo->nDeathCount++;
	}
}


void MMatchRuleQuest::OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues)
{
	if( 0 == pObj )
		return;
	
	if (m_nCombatState != MQUEST_COMBAT_PLAY) 
	{
#ifdef _DEBUG
		mlog( "obtain quest item fail. not combat play.\n" );
#endif
		return;
	}

	int nQuestItemID = pnExtraValues[0];
	int nRentPeriodHour = pnExtraValues[1];

	if (m_pQuestLevel->OnItemObtained(pObj, (unsigned long int)nQuestItemID))
	{
		// true���̸� ������ ��������.

		if (IsQuestItemID(nQuestItemID))
            RouteObtainQuestItem(unsigned long int(nQuestItemID));
		else 
			RouteObtainZItem(unsigned long int(nQuestItemID));
	}
}


void MMatchRuleQuest::OnRequestDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		OnResponseDropSacrificeItemOnSlot( uidSender, nSlotIndex, nItemID );
	}
}


void MMatchRuleQuest::OnResponseDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( (MAX_SACRIFICE_SLOT_COUNT > nSlotIndex) && (0 <= nSlotIndex) ) 
	{
		// �ߺ� �˻�.
		// if( IsSacrificeItemDuplicated(uidSender, nSlotIndex, nItemID) )
		//	return;
		
		MQuestItemDesc* pQItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID );
		if( 0 == pQItemDesc )
		{
			// ItemID�� �� �������̰ų� ItemID�� �ش��ϴ� Description�� ����.
			// ����ư error...

			mlog( "MMatchRuleBaseQuest::SetSacrificeItemOnSlot - ItemID�� �� �������̰ų� %d�� �ش��ϴ� Description�� ����.\n", nItemID );
			ASSERT( 0 );
			return;
		}

		// �������� Ÿ���� ����������� ��츸 ����.
		if( pQItemDesc->m_bSecrifice )
		{
			MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject( uidSender );
			if( !IsEnabledObject(pPlayer) )
			{
				mlog( "MMatchRuleBaseQuest::SetSacrificeItemOnSlot - ������ ����.\n" );
				return;
			}

			MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
			if( 0 == pStage )
				return;

			// �ƹ��� ���Կ� �����Ҽ� ����.

			MQuestItem* pQuestItem = pPlayer->GetCharInfo()->m_QuestItemList.Find( nItemID );
			if( 0 == pQuestItem )
				return;
			
			// ������ ������� �˻�.
			int nMySacriQItemCount = CalcuOwnerQItemCount( uidSender, nItemID );
			if( -1 == nMySacriQItemCount )
				return;
			if( nMySacriQItemCount >= pQuestItem->GetCount() )
			{
				// ������ �����ؼ� �ø��� ���ߴٰ� �뺸��.
				MCommand* pCmdMore = MMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM, MUID(0, 0) );
				if( 0 == pCmdMore )
					return;

				pCmdMore->AddParameter( new MCmdParamInt(NEED_MORE_QUEST_ITEM) );
				pCmdMore->AddParameter( new MCmdParamUID(uidSender) );
				pCmdMore->AddParameter( new MCmdParamInt(nSlotIndex) );
				pCmdMore->AddParameter( new MCmdParamInt(nItemID) );

				MMatchServer::GetInstance()->RouteToListener( pPlayer, pCmdMore );
				return;
			}

			MCommand* pCmdOk = MMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM, MUID(0, 0) );
			if( 0 == pCmdOk )
			{
				return;
			}

			pCmdOk->AddParameter( new MCmdParamInt(MOK) );
			pCmdOk->AddParameter( new MCmdParamUID(uidSender) );
			pCmdOk->AddParameter( new MCmdParamInt(nSlotIndex) );
			pCmdOk->AddParameter( new MCmdParamInt(nItemID) );
			
			MMatchServer::GetInstance()->RouteToStage( pStage->GetUID(), pCmdOk );
			
			// �Ϲ����� ó��.
			m_SacrificeSlot[ nSlotIndex ].SetAll( uidSender, nItemID );

			// ������ ������ ������Ʈ�Ǹ� ������Ʈ�� ������ �ٽ� ������.
			RefreshStageGameInfo();
		}
		else
		{
			// ����������� �ƴ�.
			ASSERT( 0 );
			return;
		}// if( pQItemDesc->m_bSecrifice )
	}
	else
	{
		// ������ �ε����� �� ��������.
		mlog( "MMatchRuleBaseQuest::OnResponseDropSacrificeItemOnSlot - %d�� ���� �ε����� ��ȿ���� �ʴ� �ε�����.\n", nSlotIndex );
		ASSERT( 0 );
		return;
	}
}


void MMatchRuleQuest::OnRequestCallbackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		OnResponseCallBackSacrificeItem( uidSender, nSlotIndex, nItemID );
	}
}


void MMatchRuleQuest::OnResponseCallBackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	// �ƹ��� �����Ҽ� ����.
	if( (MAX_SACRIFICE_SLOT_COUNT <= nSlotIndex) && (0 > nSlotIndex) ) 
		return;


	if( (0 == nItemID) || (0 == m_SacrificeSlot[nSlotIndex].GetItemID()) )
		return;

	if( nItemID != m_SacrificeSlot[nSlotIndex].GetItemID() )
		return;

	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
	{
		mlog( "MMatchRuleBaseQuest::OnResponseCallBackSacrificeItem - ���������� ����.\n" );
		return;
	}

	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
	if( 0 == pStage )
		return;

	MCommand* pCmdOk = MMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_CALLBACK_SACRIFICE_ITEM, MUID(0, 0) );
	if( 0 == pCmdOk )
	{
		return;
	}

	pCmdOk->AddParameter( new MCmdParamInt(MOK) );
	pCmdOk->AddParameter( new MCmdParamUID(uidSender) );									// ������ ȸ���� ��û�� ���̵�.
	pCmdOk->AddParameter( new MCmdParamInt(nSlotIndex) );
	pCmdOk->AddParameter( new MCmdParamInt(nItemID) );

	MMatchServer::GetInstance()->RouteToStage( pPlayer->GetStageUID(), pCmdOk );
	
	m_SacrificeSlot[ nSlotIndex ].Release();	

	// ������ ������ ������Ʈ�Ǹ� QL�� �ٽ� ������.
	RefreshStageGameInfo();
}


bool MMatchRuleQuest::IsSacrificeItemDuplicated( const MUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( (uidSender == m_SacrificeSlot[nSlotIndex].GetOwnerUID()) && (nItemID == m_SacrificeSlot[nSlotIndex].GetItemID()) )
	{
		// ���� �������� �÷��������� �߱⿡ �׳� ������ ����.

		return true;
	}

	return false;
}


/*
 * ���������� ���������� ó���ؾ� �� ���� ������� ���⿡ ������.
 */
void MMatchRuleQuest::PreProcessLeaveStage( const MUID& uidLeaverUID )
{
	MMatchRuleBaseQuest::PreProcessLeaveStage( uidLeaverUID );

	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject( uidLeaverUID );
	if( !IsEnabledObject(pPlayer) )
		return;
	
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		// ���������� �������� ������ ������ ��� �������� ���Կ� �÷� ���Ҵ��� �˻縦 ��.
		// ���� �÷����� �������� �ִٸ� �ڵ����� ȸ���� ��. - �������϶��� ����
		if (GetStage()->GetState() == STAGE_STATE_STANDBY) 
		{
			// ������ ��������� ����.
			if( (!m_SacrificeSlot[0].IsEmpty()) || (!m_SacrificeSlot[1].IsEmpty()) )
			{	
				for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
				{
					if( uidLeaverUID == m_SacrificeSlot[i].GetOwnerUID() )
						m_SacrificeSlot[ i ].Release();
				}

				MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
				if( 0 == pStage )
					return;

				// ����� ���� ������ ������.
				OnResponseSacrificeSlotInfoToStage( pStage->GetUID() );
			}
		}
	}
}


void MMatchRuleQuest::DestroyAllSlot()
{
	// ���⼭ ���Կ� �÷����ִ� �������� �Ҹ��Ŵ.

	MMatchObject*	pOwner;
	MQuestItem*		pQItem;
	MUID			uidOwner;
	unsigned long	nItemID;

	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
	{
		if( MUID(0, 0) == m_SacrificeSlot[i].GetOwnerUID() )
			continue;

		uidOwner = m_SacrificeSlot[ i ].GetOwnerUID();
		
		// �������� ������ ���������� �˻�.
		pOwner = MMatchServer::GetInstance()->GetObject( uidOwner );
		if( !IsEnabledObject(pOwner) )
		{
			continue;
		}

		nItemID = m_SacrificeSlot[ i ].GetItemID();

		// �������� �������� ���������� �˻�.
		pQItem = pOwner->GetCharInfo()->m_QuestItemList.Find( nItemID );
		if( 0 == pQItem )
		{
			continue;
		}

		m_SacrificeSlot[ i ].Release();

		pQItem->Decrease();

		pOwner->GetCharInfo()->GetDBQuestCachingData().IncreasePlayCount();
		MMatchServer::GetInstance()->OnRequestCharQuestItemList( uidOwner );
	}
}


///
// First	: �߱���.
// Last		: �߱���.
//
// QL������ ��û�� ó����. �⺻������ ��û���� ���������� �뺸��.
///

void MMatchRuleQuest::OnRequestQL( const MUID& uidSender )
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject( uidSender );
		if( 0 == pPlayer )
		{
			mlog( "MMatchRuleQuest::OnRequestQL - ������ ����.\n" );
			return;
		}

		OnResponseQL_ToStage( pPlayer->GetStageUID() );
	}
}


///
// First : �߱���.
// Last  : �߱���.
//
// ��û���� ���������� QL������ �뺸.
///
void MMatchRuleQuest::OnResponseQL_ToStage( const MUID& uidStage )
{
	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( uidStage );
	if( 0 == pStage )
	{
		mlog( "MMatchRuleQuest::OnRequestQL - �������� �˻� ����.\n" );
		return;
	}

	RefreshStageGameInfo();
}

///
// First : �߱���.
// Last  : �߱���.
//
// ���� ������ ������ ��û. �⺻������ ���������� �˸�.
///
void MMatchRuleQuest::OnRequestSacrificeSlotInfo( const MUID& uidSender )
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject( uidSender );
		if( 0 == pPlayer )
			return;

		MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
		if( 0 == pStage )
			return;

		OnResponseSacrificeSlotInfoToStage( pStage->GetUID() );
	}
}


///
// First : �߱���.
// Last  : �߱���.
//
// ���� ������ ������ ��û�ڿ� �˸�.
///
void MMatchRuleQuest::OnResponseSacrificeSlotInfoToListener( const MUID& uidSender )
{
	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
	{
		return;
	}

	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
	if( 0 == pStage )
		return;

	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_SLOT_INFO, MUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new MCmdParamUID(m_SacrificeSlot[0].GetOwnerUID()) );
	pCmd->AddParameter( new MCmdParamInt(m_SacrificeSlot[0].GetItemID()) );
	pCmd->AddParameter( new MCmdParamUID(m_SacrificeSlot[1].GetOwnerUID()) );
	pCmd->AddParameter( new MCmdParamInt(m_SacrificeSlot[1].GetItemID()) );

	MMatchServer::GetInstance()->RouteToListener( pPlayer, pCmd );
}


///
// First : �߱���.
// Last  : �߱���.
//
// ���� ������ ������ ���������� �˸�.
///
void MMatchRuleQuest::OnResponseSacrificeSlotInfoToStage( const MUID& uidStage )
{
	MMatchStage* pStage = MMatchServer::GetInstance()->FindStage( uidStage );
	if( 0 == pStage )
		return;

	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_SLOT_INFO, MUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new MCmdParamUID(m_SacrificeSlot[0].GetOwnerUID()) );
	pCmd->AddParameter( new MCmdParamInt(m_SacrificeSlot[0].GetItemID()) );
	pCmd->AddParameter( new MCmdParamUID(m_SacrificeSlot[1].GetOwnerUID()) );
	pCmd->AddParameter( new MCmdParamInt(m_SacrificeSlot[1].GetItemID()) );

	MMatchServer::GetInstance()->RouteToStage( uidStage, pCmd );
}


void MMatchRuleQuest::PostInsertQuestGameLogAsyncJob()
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		CollectEndQuestGameLogInfo();
		m_QuestGameLogInfoMgr.PostInsertQuestGameLog();
	}
}




int MMatchRuleQuest::CalcuOwnerQItemCount( const MUID& uidPlayer, const unsigned long nItemID )
{
	if(  0 == MMatchServer::GetInstance()->GetObject(uidPlayer) )
		return -1;

	int nCount = 0;
	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
	{
		if( (uidPlayer == m_SacrificeSlot[i].GetOwnerUID()) &&
			(nItemID == m_SacrificeSlot[i].GetItemID()) )
		{
			++nCount;
		}
	}

	return nCount;
}


const bool MMatchRuleQuest::PostNPCInfo()
{
	MMatchQuest*		pQuest			= MMatchServer::GetInstance()->GetQuest();
	MQuestScenarioInfo* pScenarioInfo	= pQuest->GetScenarioInfo( m_StageGameInfo.nScenarioID );
	if( NULL == pScenarioInfo )
	{
		return false;
	}
	
	vector< MQUEST_NPC > NPCList;
	
	for( size_t i = 0; i < SCENARIO_STANDARD_DICE_SIDES; ++i )
	{
		MakeJacoNPCList( NPCList, pScenarioInfo->Maps[i] );
		MakeNomalNPCList( NPCList, pScenarioInfo->Maps[i], pQuest );
	}

	void* pBlobNPC = MMakeBlobArray(sizeof(MTD_NPCINFO), int(NPCList.size()) );
	if( NULL == pBlobNPC )
	{
		return false;
	}

	vector< MQUEST_NPC >::iterator	itNL;
	vector< MQUEST_NPC >::iterator	endNL;
	MQuestNPCInfo*					pQuestNPCInfo		= NULL;
	int								nNPCIndex			= 0;
	MTD_NPCINFO*					pMTD_QuestNPCInfo	= NULL;

	endNL = NPCList.end();
	for( itNL = NPCList.begin(); endNL != itNL; ++ itNL )
	{
		pQuestNPCInfo = pQuest->GetNPCInfo( (*itNL) );	
		if( NULL == pQuestNPCInfo )
		{
			MEraseBlobArray( pBlobNPC );
			return false;
		}
			
		pMTD_QuestNPCInfo = reinterpret_cast< MTD_NPCINFO* >( MGetBlobArrayElement(pBlobNPC, nNPCIndex++) );
		if( NULL == pMTD_QuestNPCInfo )
		{
			_ASSERT( 0 );
			MEraseBlobArray( pBlobNPC );
			return false;
		}

		CopyMTD_NPCINFO( pMTD_QuestNPCInfo, pQuestNPCInfo );
	}

	MCommand* pCmdNPCList = MGetMatchServer()->CreateCommand( MC_QUEST_NPCLIST, MUID(0, 0) );
	if( NULL == pCmdNPCList )
	{
		MEraseBlobArray( pBlobNPC );
		return false;
	}

	pCmdNPCList->AddParameter( new MCommandParameterBlob(pBlobNPC, MGetBlobArraySize(pBlobNPC)) );
	pCmdNPCList->AddParameter( new MCommandParameterInt(GetGameType()) );
	
	MGetMatchServer()->RouteToStage( m_pStage->GetUID(), pCmdNPCList );

	MEraseBlobArray( pBlobNPC );

	return true;
}


///
// First : 2005.04.18 �߱���.
// Last  : 2005.04.18 �߱���.
//
// ������ �����ϱ����� �ع��� �ϴ� �۾��� ������.
// �غ� �۾��� ���а� �����ô� ������ �������� ���ϰ� �ؾ� ��.
///
bool MMatchRuleQuest::PrepareStart()
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		MakeStageGameInfo();

		if ((m_StageGameInfo.nQL >= 0) || (m_StageGameInfo.nQL <= MAX_QL)) 
		{
#ifdef _DEBUG
#else
			if(m_StageGameInfo.nScenarioID == 100)
				m_StageGameInfo.nScenarioID = 0;
#endif
			if ((m_StageGameInfo.nScenarioID > 0) || (m_StageGameInfo.nMapsetID > 0))
			{
				if( PostNPCInfo() )
				{
					return true;
				}
			}
		}
	}

	if( NULL != MMatchServer::GetInstance()->GetObject(m_pStage->GetMasterUID()) )
	{
		MCommand* pCmdNotReady = MGetMatchServer()->CreateCommand( MC_GAME_START_FAIL, m_pStage->GetMasterUID() );
		pCmdNotReady->AddParameter( new MCmdParamInt(QUEST_START_FAILED_BY_SACRIFICE_SLOT) );
		pCmdNotReady->AddParameter( new MCmdParamUID(MUID(0, 0)) );
		MGetMatchServer()->Post( pCmdNotReady );
	}

	return false;
}

void MMatchRuleQuest::MakeStageGameInfo()
{	
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		if( (GetStage()->GetState() != STAGE_STATE_STANDBY) && (STAGE_STATE_COUNTDOWN != GetStage()->GetState()) )
		{
			return;
		}

		// ���Կ� Level�� �´� �������� �������� �÷��� �ִ��� �˻簡 �ʿ���.
		// ������ �������� �÷��� ������� ������ ȸ�� ��û�� ����� ��.
		int nOutResultQL = -1;

		int nMinPlayerLevel = 1;
		MMatchStage* pStage = GetStage();
		if (pStage != NULL)
		{
			nMinPlayerLevel = pStage->GetMinPlayerLevel();

			// ������ ����̸� �ּҷ����� ��� ������ ���������Ѵ�.
			MMatchObject* pMaster = MMatchServer::GetInstance()->GetObject(pStage->GetMasterUID());
			if (IsAdminGrade(pMaster))
			{
				nMinPlayerLevel = pMaster->GetCharInfo()->m_nLevel;
			}
		}

		int nPlayerQL = MQuestFormula::CalcQL( nMinPlayerLevel );
//		m_StageGameInfo.nPlayerQL = nPlayerQL;

		unsigned int SQItems[MAX_SCENARIO_SACRI_ITEM];
		for (int i = 0; i < MAX_SCENARIO_SACRI_ITEM; i++)
		{
			SQItems[i] = (unsigned int)m_SacrificeSlot[i].GetItemID();
		}

		// �ϵ��ڵ�.. �Ƕ�... -_-;
		m_StageGameInfo.nMapsetID = 1;
		if ( !stricmp( pStage->GetMapName(), "mansion"))
			m_StageGameInfo.nMapsetID = 1;
		else if ( !stricmp( pStage->GetMapName(), "prison"))
			m_StageGameInfo.nMapsetID = 2;
		else if ( !stricmp( pStage->GetMapName(), "dungeon"))
			m_StageGameInfo.nMapsetID = 3;


		MMatchQuest* pQuest = MMatchServer::GetInstance()->GetQuest();
		unsigned int nScenarioID = pQuest->GetScenarioCatalogue()->MakeScenarioID(m_StageGameInfo.nMapsetID,
																				  nPlayerQL, SQItems);

		m_StageGameInfo.nScenarioID = nScenarioID;
		MQuestScenarioInfo* pScenario = pQuest->GetScenarioCatalogue()->GetInfo(nScenarioID);
		if (pScenario)
		{
			m_StageGameInfo.nQL = pScenario->nQL;
			m_StageGameInfo.nPlayerQL = nPlayerQL;
		}
		else
		{
			if ( nPlayerQL > 1)
			{
				m_StageGameInfo.nQL = 1;
				m_StageGameInfo.nPlayerQL = 1;
			}
			else
			{
				m_StageGameInfo.nQL = 0;
				m_StageGameInfo.nPlayerQL = 0;
			}
		}
	}
}

void MMatchRuleQuest::RefreshStageGameInfo()
{
	MakeStageGameInfo();
	RouteStageGameInfo();
}

void MMatchRuleQuest::OnChangeCondition()
{
	RefreshStageGameInfo();
}

void MMatchRuleQuest::CollectStartingQuestGameLogInfo()
{
	// �����ϱ����� ������ ������ �ݵ�� ������ ��.
	m_QuestGameLogInfoMgr.Clear();

	if( QuestTestServer() ) 
	{
		// Master CID
		MMatchObject* pMaster = MMatchServer::GetInstance()->GetObject( GetStage()->GetMasterUID() );
		if( IsEnabledObject(pMaster) )
			m_QuestGameLogInfoMgr.SetMasterCID( pMaster->GetCharInfo()->m_nCID );

		m_QuestGameLogInfoMgr.SetScenarioID( m_pQuestLevel->GetStaticInfo()->pScenario->nID );

		// Stage name ����.
		m_QuestGameLogInfoMgr.SetStageName( GetStage()->GetName() );

		// �����Ҷ��� ���� ������ ������.
		for(MQuestPlayerManager::iterator it = m_PlayerManager.begin() ; 
				it != m_PlayerManager.end(); ++it )
		{
			m_QuestGameLogInfoMgr.AddQuestPlayer( it->second->pObject->GetUID(), it->second->pObject );
		}

		m_QuestGameLogInfoMgr.SetStartTime( timeGetTime() );
	}
}


void MMatchRuleQuest::CollectEndQuestGameLogInfo()
{
	m_QuestGameLogInfoMgr.SetEndTime( timeGetTime() );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////	