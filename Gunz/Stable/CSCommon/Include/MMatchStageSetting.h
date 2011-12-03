#ifndef _MMATCHSTAGESETTING_H
#define _MMATCHSTAGESETTING_H

#include <list>
using namespace std;
#include "MUID.h"
#include "MMatchGlobal.h"
#include "MMatchGameType.h"
#include "MMatchObject.h"
#include "MMatchMap.h"

class MMatchObject;
class MMatchStage;

#define MMATCH_TEAM_MAX_COUNT		2

#define MMATCH_SPECTATOR_STR		"SPECTATOR"
#define MMATCH_TEAM1_NAME_STR		"RED TEAM"
#define MMATCH_TEAM2_NAME_STR		"BLUE TEAM"


inline const char* GetTeamNameStr(MMatchTeam nTeam)
{
	switch (nTeam)
	{
	case MMT_SPECTATOR:
		return MMATCH_SPECTATOR_STR;
	case MMT_RED:
		return MMATCH_TEAM1_NAME_STR;
	case MMT_BLUE:
		return MMATCH_TEAM2_NAME_STR;
	default:
		return "";
	}
	return "";
}


enum STAGE_STATE {
	STAGE_STATE_STANDBY		= 0,
	STAGE_STATE_COUNTDOWN,
	STAGE_STATE_RUN,
	STAGE_STATE_CLOSE,

	STAGE_STATE_COUNT
};

#define MSTAGENODE_FLAG_FORCEDENTRY_ENABLED		1		// ����
#define MSTAGENODE_FLAG_PRIVATE					2		// ��й�
#define MSTAGENODE_FLAG_LIMITLEVEL				4		// ��������


/// �������� ���ð�. 
/// - ��Ʈ�� ���ۿ����ε� ����Ѵ�. ���� MTD�� �̵��ؾ���.
/// ���⿡ ������ �߰��Ϸ���, ���÷��̿͵� ������ �����Ƿ� 
/// ZReplayLoader::ConvertStageSettingNode() �Լ��� ��������� �Ѵ�.
struct MSTAGE_SETTING_NODE {
	MUID				uidStage;
	char				szMapName[MAPNAME_LENGTH];	// ���̸�
	char				nMapIndex;					// ���ε���
	MMATCH_GAMETYPE		nGameType;					// ����Ÿ��
	int					nRoundMax;					// ����
	int					nLimitTime;					// ���ѽð�(1 - 1��)
	int					nLimitLevel;				// ���ѷ���
	int					nMaxPlayers;				// �ִ��ο�
	bool				bTeamKillEnabled;			// ��ų����
	bool				bTeamWinThePoint;			// ������ ����
	bool				bForcedEntryEnabled;		// ������ ���� ����

	// �߰���
	bool				bAutoTeamBalancing;			// �������뷱�� - ���ð��ӿ����� ���
	// �����̸�
	bool				bIsRelayMap;				// �����̸� ����
	bool				bIsStartRelayMap;			// �����̸� ������ ����
	int					nRelayMapListCount;			// �����̸� ����Ʈ ����
	RelayMap			MapList[MAX_RELAYMAP_LIST_COUNT];	// �����̸� ����Ʈ
	RELAY_MAP_TYPE		nRelayMapType;				// �����̸� Ÿ��
	RELAY_MAP_REPEAT_COUNT	nRelayMapRepeatCount;	// �����̸� ȸ��
	
#ifdef _VOTESETTING
	bool				bVoteEnabled;				// ��ǥ���� ����
	bool				bObserverEnabled;			// ������� ����
#endif
};

// �� ó�� ��������� �������� ���� �ʱⰪ
#define MMATCH_DEFAULT_STAGESETTING_MAPNAME			"Mansion"

#define MMATCH_DEFAULT_STAGESETTING_GAMETYPE			MMATCH_GAMETYPE_DEATHMATCH_SOLO
#define MMATCH_DEFAULT_STAGESETTING_ROUNDMAX			50		// 50����
#define MMATCH_DEFAULT_STAGESETTING_LIMITTIME			30		// 30��
#define MMATCH_DEFAULT_STAGESETTING_LIMITLEVEL			0		// ������
#define MMATCH_DEFAULT_STAGESETTING_MAXPLAYERS			8		// 8��
#define MMATCH_DEFAULT_STAGESETTING_TEAMKILL			false	// ��ų�Ұ�
#define MMATCH_DEFAULT_STAGESETTING_TEAM_WINTHEPOINT	false	// ������ ����
#define MMATCH_DEFAULT_STAGESETTING_FORCEDENTRY			true	// ���԰���
#define MMATCH_DEFAULT_STAGESETTING_AUTOTEAMBALANCING	true	// �������뷱��


#define STAGESETTING_LIMITTIME_UNLIMITED				0		// ���ѽð��� �������� 0


struct MSTAGE_CHAR_SETTING_NODE {
	MUID	uidChar;
	int		nTeam;
	MMatchObjectStageState	nState;
	MSTAGE_CHAR_SETTING_NODE() : uidChar(MUID(0,0)), nTeam(0), nState(MOSS_NONREADY) {	}
};
class MStageCharSettingList : public list<MSTAGE_CHAR_SETTING_NODE*> {
public:
	void DeleteAll() {
		for (iterator i=begin(); i!=end(); i++) {
			delete (*i);
		}
		clear();
	}
};




class MMatchStageSetting {
protected:
	MProtectValue<MSTAGE_SETTING_NODE>	m_StageSetting;
	MUID					m_uidMaster;	// ����
	STAGE_STATE				m_nStageState;	// ���� State (������,�����,..)

	bool					m_bIsCheckTicket;
	DWORD					m_dwTicketItemID;
public:
	MStageCharSettingList	m_CharSettingList;
public:
	MMatchStageSetting();
	virtual ~MMatchStageSetting();
	void Clear();
	void SetDefault();
	unsigned long GetChecksum();
	MSTAGE_CHAR_SETTING_NODE* FindCharSetting(const MUID& uid);

	// Get
	const char*					GetMapName()				{ return m_StageSetting.Ref().szMapName; }
	int							GetMapIndex()				{ return m_StageSetting.Ref().nMapIndex; }
	int							GetRoundMax()				{ return m_StageSetting.Ref().nRoundMax; }
	int							GetLimitTime()				{ return m_StageSetting.Ref().nLimitTime; }
	int							GetLimitLevel()				{ return m_StageSetting.Ref().nLimitLevel; }
	MUID						GetMasterUID()				{ return m_uidMaster; }
	STAGE_STATE					GetStageState()				{ return m_nStageState; }
	MMATCH_GAMETYPE				GetGameType()				{ return m_StageSetting.Ref().nGameType; }
	int							GetMaxPlayers()				{ return m_StageSetting.Ref().nMaxPlayers; }
	bool						GetForcedEntry()			{ return m_StageSetting.Ref().bForcedEntryEnabled; }
	bool						GetAutoTeamBalancing()		{ return m_StageSetting.Ref().bAutoTeamBalancing; }
	const MSTAGE_SETTING_NODE*	GetStageSetting()			{ return &m_StageSetting.Ref(); }
	const MMatchGameTypeInfo*	GetCurrGameTypeInfo();
	DWORD						GetTicketItemID()			{ return m_dwTicketItemID; }
	int							GetRelayMapListCount()		{ return m_StageSetting.Ref().nRelayMapListCount; }
	const RelayMap*				GetRelayMapList()			{ return m_StageSetting.Ref().MapList; }
	RELAY_MAP_TYPE				GetRelayMapType()			{ return m_StageSetting.Ref().nRelayMapType; }
	RELAY_MAP_REPEAT_COUNT		GetRelayMapRepeatCount()	{ return m_StageSetting.Ref().nRelayMapRepeatCount; }

	bool						IsRelayMap()				{ return m_StageSetting.Ref().bIsRelayMap; }
	bool						IsStartRelayMap()			{ return m_StageSetting.Ref().bIsStartRelayMap; }

	// Set
	void SetMasterUID(const MUID& uid)				{ m_uidMaster = uid; }
	void SetMapName(char* pszName);
	void SetMapIndex(int nMapIndex);
	void SetRoundMax(int nRound)					{ MEMBER_SET_CHECKCRC(m_StageSetting, nRoundMax, nRound); }
	void SetLimitTime(int nTime)					{ MEMBER_SET_CHECKCRC(m_StageSetting, nLimitTime, nTime); }
	void SetGameType(MMATCH_GAMETYPE type)			{ MEMBER_SET_CHECKCRC(m_StageSetting, nGameType, type); }
	void SetStageState(STAGE_STATE nState)			{ m_nStageState = nState; }
	void SetTeamWinThePoint(bool bValue)			{ MEMBER_SET_CHECKCRC(m_StageSetting, bTeamWinThePoint, bValue); }
	void SetAutoTeamBalancing(bool bValue)			{ MEMBER_SET_CHECKCRC(m_StageSetting, bAutoTeamBalancing, bValue); }
	void SetIsCheckTicket( bool bIsCheck )			{ m_bIsCheckTicket = bIsCheck; }
	void SetTicketItemID( DWORD dwTicketItemID )	{ m_dwTicketItemID = dwTicketItemID; }
	
	void SetIsRelayMap(bool bValue)					{ MEMBER_SET_CHECKCRC(m_StageSetting, bIsRelayMap, bValue); }
	void SetIsStartRelayMap(bool bValue)			{ MEMBER_SET_CHECKCRC(m_StageSetting, bIsStartRelayMap, bValue); }

	void SetRelayMapListCount(int nValue)			{ MEMBER_SET_CHECKCRC(m_StageSetting, nRelayMapListCount, nValue); }
	void SetRelayMapList(RelayMap* pValue);
	void SetRelayMapType(RELAY_MAP_TYPE bValue)		{ MEMBER_SET_CHECKCRC(m_StageSetting, nRelayMapType, bValue); }
	void SetRelayMapRepeatCount(RELAY_MAP_REPEAT_COUNT bValue)	{ MEMBER_SET_CHECKCRC(m_StageSetting, nRelayMapRepeatCount, bValue); }
	
	void UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting);
	void UpdateCharSetting(const MUID& uid, unsigned int nTeam, MMatchObjectStageState nStageState);

	void ResetCharSetting()			{ m_CharSettingList.DeleteAll(); }
	bool IsTeamPlay();
	bool IsWaitforRoundEnd();
	bool IsQuestDrived();
	bool IsTeamWinThePoint()		{ return m_StageSetting.Ref().bTeamWinThePoint; }		///< ������ ����

	bool	IsCheckTicket()			{ return m_bIsCheckTicket; }
	
	// �ٹ��� Ŭ�󿡼��� ȣ��
	void AntiHack_ShiftHeapPos()	{ m_StageSetting.ShiftHeapPos(); }
	void AntiHack_CheckCrc()		{ m_StageSetting.CheckCrc(); }
};












#endif