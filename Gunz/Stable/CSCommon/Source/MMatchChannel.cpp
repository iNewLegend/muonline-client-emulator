#include "stdafx.h"
#include "ODBCRecordset.h"
#include "MMatchChannel.h"
#include "MCommand.h"
#include "MMatchObject.h"
#include "MMatchStage.h"
#include <math.h>
#include "MSmartRefreshImpl.h"
//#include "MMatchMapSet.h"


#define MTICK_CHANNEL_RUN					100
#define MTICK_CHANNEL_UPDATECHECKSUM		2000
#define MTICK_CHANNEL_EMPTY_PERIOD_LIMIT	60*1000		// 60 seconds


bool MMatchChannel::Create(const MUID& uid, const char* pszName, const char* pszRuleName, 
						MCHANNEL_TYPE nType, int nMaxPlayers, int nLevelMin, int nLevelMax,
						const bool bIsTicketChannel, const DWORD dwTicketItemID, const bool bIsUseTicket, const char* pszNameStrResId)
{
	m_nEmptyPeriod		= 0;
	m_nMaxStages		= DEFAULT_CHANNEL_MAXSTAGES;
	m_uidChannel		= uid;
	strcpy(m_szChannelName, pszName);
	strcpy(m_szRuleName, pszRuleName);
	m_nChannelType		= nType;
	m_nMaxPlayers		= nMaxPlayers;
	m_nLevelMin			= nLevelMin;
	m_nLevelMax			= nLevelMax;
	m_nRuleType			= MCHANNEL_RULE_NOVICE;
	m_bIsTicketChannel	= bIsTicketChannel;
	m_dwTicketItemID	= dwTicketItemID;
	m_bIsUseTicket		= bIsUseTicket;

	// ����ä���� ��� Ŭ���̾�Ʈ���� ������ �̸����� ǥ���� �� �ֵ��� ä���̸��� ��Ʈ�����ҽ� ID�� �����ߴٰ� ä�θ���Ʈ�� �Բ� ������
	if (pszNameStrResId) {
		_ASSERT(strlen(pszNameStrResId) < CHANNELNAME_STRINGRESID_LEN);
		strcpy(m_szChannelNameStrResId, pszNameStrResId);
	}
	else m_szChannelNameStrResId[0] = 0;

	//// Paged UserList and SmartRefreshing ////
	m_UserArray.Reserve(NUM_PLAYERLIST_NODE, nMaxPlayers);
	int nMaxPage = (int)ceil((float)nMaxPlayers/(float)NUM_PLAYERLIST_NODE);
	for (int i=0; i<nMaxPage; i++) {
		m_SmartRefresh.AddCategory(new MRefreshCategoryChannelImpl(this, i));
	}

	for (int i = 0; i < m_nMaxStages; i++)
	{
		m_UnusedStageIndexList.push_back(i);
	}

	memset(m_pStages, 0, sizeof(MMatchStage*) * MAX_CHANNEL_MAXSTAGES);


	// ä�η꿡 ���� Ư���� ����
	if (nType == MCHANNEL_TYPE_PRESET || nType == MCHANNEL_TYPE_DUELTOURNAMENT)
	{
		if (!stricmp(m_szRuleName, MCHANNEL_RULE_NOVICE_STR))				m_nRuleType = MCHANNEL_RULE_NOVICE;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_NEWBIE_STR))			m_nRuleType = MCHANNEL_RULE_NEWBIE;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_ROOKIE_STR))			m_nRuleType = MCHANNEL_RULE_ROOKIE;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_MASTERY_STR))			m_nRuleType = MCHANNEL_RULE_MASTERY;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_ELITE_STR))			m_nRuleType = MCHANNEL_RULE_ELITE;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_CHAMPION_STR))		m_nRuleType = MCHANNEL_RULE_CHAMPION;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_QUEST_STR))			m_nRuleType = MCHANNEL_RULE_QUEST;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_DUELTOURNAMENT_STR))	m_nRuleType = MCHANNEL_RULE_DUELTOURNAMENT;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_SET1_STR))			m_nRuleType = MCHANNEL_RULE_SET1;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_SET2_STR))			m_nRuleType = MCHANNEL_RULE_SET2;
		else if (!stricmp(m_szRuleName, MCHANNEL_RULE_SET3_STR))			m_nRuleType = MCHANNEL_RULE_SET3;
	}

	// ���̸��� "newbie"�̸� ����ä��
//	if ((nType == MCHANNEL_TYPE_PRESET) && (!stricmp(m_szRuleName, MCHANNEL_RULE_NEWBIE_STR)))
//	{
//		m_bNewbieChannel = true;
//	}

	return true;
}

void MMatchChannel::Destroy()
{
	m_ObjUIDCaches.clear();
	m_ObjUIDLobbyCaches.clear();
//	m_ObjStrCaches.clear();
}

bool MMatchChannel::CheckTick(unsigned long nClock)
{
	if (nClock - m_nLastTick < MTICK_CHANNEL_RUN) return false;
	return true;
}

void MMatchChannel::Tick(unsigned long nClock)
{
	if (IsChecksumUpdateTime(nClock))
		UpdateChecksum(nClock);

	m_SmartRefresh.UpdateCategory(nClock);

	if (GetObjCount() <= 0) m_nEmptyPeriod += MTICK_CHANNEL_RUN; else m_nEmptyPeriod = 0;
	m_nLastTick = nClock;
}

bool MMatchChannel::IsChecksumUpdateTime(unsigned long nTick)
{
	if (nTick - m_nLastChecksumTick > MTICK_CHANNEL_UPDATECHECKSUM)
		return true;
	else
		return false;
}

void MMatchChannel::UpdateChecksum(unsigned long nTick)
{
	m_nChecksum = (unsigned long)m_ObjUIDCaches.size() + m_uidChannel.Low;
	m_nLastChecksumTick = nTick;
}

bool MMatchChannel::CheckLifePeriod()
{
	MCHANNEL_TYPE nType = GetChannelType();
	if (nType == MCHANNEL_TYPE_PRESET || nType == MCHANNEL_TYPE_DUELTOURNAMENT) return true;

	if (GetObjCount() > 0) return true;
	if (GetEmptyPeriod() < MTICK_CHANNEL_EMPTY_PERIOD_LIMIT) return true;

	return false;
}

void MMatchChannel::AddObject(const MUID& uid, MMatchObject* pObj)
{
	m_ObjUIDCaches.Insert(uid, (void*)pObj); // Channel Cache
	//m_ObjStrCaches.insert(MObjectStrMap::value_type(string(pObj->GetCharInfo()->m_szName), pObj));
	
	m_UserArray.Add(pObj);

	JoinLobby(uid, pObj);
}

void MMatchChannel::RemoveObject(const MUID& uid)
{
	LeaveLobby(uid);

	MUIDRefCache::iterator i = m_ObjUIDCaches.find(uid);
	if (i != m_ObjUIDCaches.end())
	{
		MMatchObject* pObj = (MMatchObject*)((*i).second);
		m_UserArray.Remove(pObj);

		m_ObjUIDCaches.erase(i); // Channel Cache
	}
	
/*
	if (pObj)
	{
		MObjectStrMap::iterator Itor2 = m_ObjStrCaches.find(string(pObj->GetCharInfo()->m_szName));
		if (Itor2 != m_ObjStrCaches.end())
		{
			m_ObjStrCaches.erase(Itor2);
		}
	}
*/	
}

void MMatchChannel::JoinLobby(const MUID& uid, const MMatchObject* pObj)
{
	if (m_ObjUIDLobbyCaches.find(uid) == m_ObjUIDLobbyCaches.end())
	{
		m_ObjUIDLobbyCaches.Insert(uid, (void*)pObj); // Channel Cache
	}
	else
	{
		_ASSERT(0);
	}
}

void MMatchChannel::LeaveLobby(const MUID& uid)
{
	MUIDRefCache::iterator LobbyObjItor = m_ObjUIDLobbyCaches.find(uid);
	if (LobbyObjItor != m_ObjUIDLobbyCaches.end())
	{
		m_ObjUIDLobbyCaches.erase(LobbyObjItor);
	}
	else
	{
		_ASSERT(0);
	}
}

bool MMatchChannel::AddStage(MMatchStage* pStage)
{
	if (m_UnusedStageIndexList.empty()) return false;

	int nRecommendedStageIndex = *(m_UnusedStageIndexList.begin());
	m_UnusedStageIndexList.pop_front();
	if ((nRecommendedStageIndex < 0) || (nRecommendedStageIndex > m_nMaxStages)) return false;

	pStage->SetOwnerChannel(GetUID(), nRecommendedStageIndex);

	if (m_pStages[nRecommendedStageIndex] != NULL)
	{
		_ASSERT(0);
	}

	m_pStages[nRecommendedStageIndex] = pStage;

	return true;
}

bool LessCompStageIndexList(const int a, const int b)
{
	return (a<b);
}

void MMatchChannel::RemoveStage(MMatchStage* pStage)
{
	int nStageIndex = pStage->GetIndex();

	if ((nStageIndex >= 0) && (nStageIndex < m_nMaxStages))
	{
		list<int>::iterator it = lower_bound(m_UnusedStageIndexList.begin(), m_UnusedStageIndexList.end(), 
									nStageIndex, LessCompStageIndexList);

		m_UnusedStageIndexList.insert(it, nStageIndex);

		m_pStages[nStageIndex] = NULL;
	}
}

bool MMatchChannel::IsEmptyStage(int nIndex)
{
	if ((nIndex < 0) || (nIndex > m_nMaxStages)) return true;
	if (m_pStages[nIndex] == NULL) return true;
	return false;
}

MMatchStage* MMatchChannel::GetStage(int nIndex)
{
	if ((nIndex < 0) || (nIndex > m_nMaxStages)) return NULL;
	return m_pStages[nIndex];
}

int MMatchChannel::GetPrevStageCount(int nStageIndex)
{
	if ((nStageIndex < 0) || (nStageIndex > m_nMaxStages)) return 0;

	int nStageCount = 0;
	for (int i = 0; i < nStageIndex; i++)
	{
		if (m_pStages[i] == NULL)  continue;
		if (m_pStages[i]->GetState() == STAGE_STATE_CLOSE) continue;

		nStageCount++;
	}
	return nStageCount;
}

int MMatchChannel::GetNextStageCount(int nStageIndex)
{
	if ((nStageIndex < 0) || (nStageIndex > m_nMaxStages)) return 0;

	int nStageCount = 0;
	for (int i = nStageIndex+1; i < m_nMaxStages; i++)
	{
		if (m_pStages[i] == NULL)  continue;
		if (m_pStages[i]->GetState() == STAGE_STATE_CLOSE) continue;

		nStageCount++;
	}
	return nStageCount;
}

void MMatchChannel::SyncPlayerList(MMatchObject* pObj, int nPage)
{
	m_SmartRefresh.SyncClient(pObj->GetRefreshClientChannelImplement());
}


MMatchChannel* MMatchChannelMap::Find(const MUID& uidChannel)
{
	iterator i = find(uidChannel);
	if(i==end()) return NULL;

	MMatchChannel* pChannel = (*i).second;
	return pChannel;
}

MMatchChannel* MMatchChannelMap::Find(const MCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	if ((nChannelType < 0) || (nChannelType >= MCHANNEL_TYPE_MAX)) return NULL;

	for(map<MUID, MMatchChannel*>::iterator i = m_TypesChannelMap[nChannelType].begin(); 
		i != m_TypesChannelMap[nChannelType].end(); i++)
	{
		MMatchChannel* pChannel = (*i).second;
		if (strcmp(pChannel->GetName(), pszChannelName) == 0) return pChannel;
	}
	return NULL;
}

bool MMatchChannelMap::Add(const char* pszChannelName, const char* pszRuleName, MUID* pAllocUID, MCHANNEL_TYPE nType, int nMaxPlayers, int nLevelMin, int nLevelMax,
						   const bool bIsTicketChannel, const DWORD dwTicketItemID, const bool bIsUseTicket, const char* pszChannelNameStrResId)
{
	MUID uidChannel = UseUID();

	MMatchChannel* pChannel = new MMatchChannel;
	pChannel->Create(uidChannel, pszChannelName, pszRuleName, nType, nMaxPlayers, nLevelMin, nLevelMax, bIsTicketChannel, dwTicketItemID, bIsUseTicket, pszChannelNameStrResId);
	Insert(uidChannel, pChannel);
	*pAllocUID = uidChannel;


	if ((nType >= 0) && (nType < MCHANNEL_TYPE_MAX))
	{
		m_TypesChannelMap[nType].insert(map<MUID, MMatchChannel*>::value_type(uidChannel, pChannel));
	}
	else
	{
		MMatchServer::GetInstance()->LOG(MCommandCommunicator::LOG_FILE, "invalid channel type %d",nType);
		erase(uidChannel);

		delete pChannel;
		return false;
	}

	return true;
}

bool MMatchChannelMap::Remove(const MUID& uidChannel, MMatchChannelMap::iterator* pNextItor)
{
	MMatchChannelMap::iterator i = find(uidChannel);
	if(i == end()) return false;

	MMatchChannel* pChannel = (*i).second;
	//LOG(LOG_DEBUG, "Closing Channel '%s'(UID:%d%d)", pChannel->GetName(), pChannel->GetUID().High, pChannel->GetUID().Low);

	MCHANNEL_TYPE nType = pChannel->GetChannelType();
	if ((nType >= 0) && (nType < MCHANNEL_TYPE_MAX))
	{
		map<MUID, MMatchChannel*>::iterator itorChannelTypeMap = m_TypesChannelMap[nType].find(uidChannel);
		if (itorChannelTypeMap != m_TypesChannelMap[nType].end())
		{
			m_TypesChannelMap[nType].erase(itorChannelTypeMap);
		}
	}


	delete pChannel;

	MMatchChannelMap::iterator itorTemp = erase(i);
	if (pNextItor)
		*pNextItor = itorTemp;


	
	return true;
}


void MMatchChannelMap::Destroy()
{
	iterator itorChannel = begin();
	while(itorChannel != end()) {
		MUID uid = (*itorChannel).first;
		Remove(uid, &itorChannel);
	}

	for (int i = 0; i < MCHANNEL_TYPE_MAX; i++)
	{
		m_TypesChannelMap[i].clear();
	}
}

void MMatchChannelMap::Update(unsigned long nClock)
{
	unsigned long nChannelListChecksum = 0;
	for(iterator itor=begin(); itor != end();)
	{
		MMatchChannel* pChannel = (*itor).second;
		pChannel->Tick(nClock);
		if (pChannel->CheckLifePeriod() == false)
		{
			Remove(pChannel->GetUID(), &itor);
			continue;
		}
		else
		{
			itor++;
		}
		nChannelListChecksum += pChannel->GetChecksum();
	}

	m_nChecksum = nChannelListChecksum;
}

int MMatchChannelMap::GetChannelCount(MCHANNEL_TYPE nChannelType)
{
	if ((nChannelType < 0) || (nChannelType >= MCHANNEL_TYPE_MAX)) return 0;

	return (int)m_TypesChannelMap[nChannelType].size();

}

map<MUID, MMatchChannel*>::iterator MMatchChannelMap::GetTypesChannelMapBegin(MCHANNEL_TYPE nType)
{
	if ((nType < 0) || (nType >= MCHANNEL_TYPE_MAX)) return m_TypesChannelMap[MCHANNEL_TYPE_PRESET].begin();
	return m_TypesChannelMap[nType].begin();
}

map<MUID, MMatchChannel*>::iterator MMatchChannelMap::GetTypesChannelMapEnd(MCHANNEL_TYPE nType)
{
	if ((nType < 0) || (nType >= MCHANNEL_TYPE_MAX)) return m_TypesChannelMap[MCHANNEL_TYPE_PRESET].end();
	return m_TypesChannelMap[nType].end();
}

int MMatchChannel::GetPlayers()
{
	int nPlayers = 0;

	for( MUIDRefCache::iterator i = GetObjBegin();  i != GetObjEnd();  i++)
	{
		MMatchObject* pObj = (MMatchObject*)((*i).second);
		
//		if ( IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))		// �׽�Ʈ ����
//			continue;

		nPlayers++;
	}

	return nPlayers;
}
