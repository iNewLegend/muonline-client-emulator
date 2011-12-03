#ifndef MMATCHOBJECT_H
#define MMATCHOBJECT_H

#include <vector>
#include <map>
#include <algorithm>
#include <string>
using namespace std;
#include "MMatchItem.h"
#include "MUID.h"
#include "MObject.h"
#include "MMatchGlobal.h"
#include "MMatchFriendInfo.h"
#include "MMatchClan.h"
#include "MMatchChannel.h"
#include "MSmartRefreshImpl.h"
#include "MQuestItem.h"
#include "MMatchAntiHack.h"
#include "MMatchHShield.h"
#include "MMatchGambleItem.h"
#include "MHackingTypes.h"
#include "MMatchObjectCharBuffInfo.h"

#include "MMatchAccountPenaltyInfo.h"

#include "MMatchCharBRInfo.h"

// ��� - �̰��� ����� UserGrade���̺�� ��ũ�� �¾ƾ� �Ѵ�.
enum MMatchUserGradeID
{
	MMUG_FREE			= 0,	// ���� ����
	MMUG_REGULAR		= 1,	// ���� ����
	MMUG_STAR			= 2,	// ��Ÿ����(����¯)

	MMUG_CRIMINAL		= 100,	// ������
	MMUG_WARNING_1		= 101,	// 1�� ���
	MMUG_WARNING_2		= 102,	// 2�� ���
	MMUG_WARNING_3		= 103,	// 3�� ���
	MMUG_CHAT_LIMITED	= 104,  // ä�� ����
	MMUG_PENALTY		= 105,	// �Ⱓ ����

	MMUG_EVENTMASTER	= 252,	// �̺�Ʈ ������
	MMUG_BLOCKED		= 253,	// ��� ����
	MMUG_DEVELOPER		= 254,	// ������
	MMUG_ADMIN			= 255	// ������
};



enum MMatchDisconnectStatus
{
	MMDS_CONNECTED = 1,
	MMDS_DISCONN_WAIT,
	MMDS_DISCONNECT,

	MMDS_END,
};



// �����̿��� ���� - ������ DB�� PGradeID�� ���� �ʰ�, ���� �ݸ���PC�� ���ʽ� �뵵�θ� ����Ѵ�.
enum MMatchPremiumGradeID
{
	MMPG_FREE			= 0,	// ����
	MMPG_PREMIUM_IP		= 1		// �ݸ��� PC�� ���ʽ�
};

// ���� - �̰��� ����� ���������� ��ũ�� �¾ƾ� �Ѵ�.
enum MMatchSex
{
	MMS_MALE = 0,		// ����
	MMS_FEMALE = 1		// ����
};


/// ���� ����
struct MMatchAccountInfo
{
	int						m_nAID;
	char					m_szUserID[ MAX_USERID_STRING_LEN ];	// ����ID
	MMatchUserGradeID		m_nUGrade;								// ���
	MMatchPremiumGradeID	m_nPGrade;								// ���� �̿��� ����

	MMatchHackingType		m_HackingType;
	SYSTEMTIME				m_EndBlockDate;							// ��ŷ �������� �����Ǵ� �ð�.
	DWORD					m_dwHackingBlockEndTimeMS;

	bool					m_bIsPowerLevelingHacker;
	int						m_nPowerLevelingRegTimeMin;

	int						m_nCCode;

	MMatchAccountInfo() : m_nAID(-1), m_nUGrade(MMUG_FREE), m_nPGrade(MMPG_FREE), m_nCCode(0)
	{
		m_HackingType				= MMHT_NO;
		m_dwHackingBlockEndTimeMS	= 0;
		m_bIsPowerLevelingHacker	= false;
		m_nPowerLevelingRegTimeMin	= 0;
		
		memset(m_szUserID, 0, MAX_USERID_STRING_LEN);
	}
};


struct MAccountItemNode
{
	int					nAIID;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;
	int					nCount;
};

// �÷��̾ ���� ��ġ�ϰ� �ִ� ��
enum MMatchPlace
{
	MMP_OUTSIDE	= 0,
	MMP_LOBBY = 1,
	MMP_STAGE	= 2,
	MMP_BATTLE	= 3,
	MMP_END
};

enum MMatchObjectStageState
{
	MOSS_NONREADY	= 0,
	MOSS_READY		= 1,
	MOSS_SHOP		= 2,
	MOSS_EQUIPMENT	= 3,
	MOSS_END
};

#define DEFAULT_CHAR_HP				100
#define DEFAULT_CHAR_AP				0

#define DBCACHING_REQUEST_POINT			40
#define DBCACHING_REQUEST_TIME_POINT	60 * 5	// 5�и��� ������Ʈ~!

struct DBCharCachingData
{
	int	nAddedXP;
	int	nAddedBP;
	int	nAddedKillCount;
	int	nAddedDeathCount;
	int nAddedPlayTime;

	void Reset()
	{
		nAddedXP = 0;
		nAddedBP = 0;
		nAddedKillCount = 0;
		nAddedDeathCount = 0;
		nAddedPlayTime = 0;
	}
	bool IsRequestUpdate()
	{
		if ((nAddedKillCount > DBCACHING_REQUEST_POINT) || (nAddedDeathCount > DBCACHING_REQUEST_POINT)
			|| nAddedPlayTime > DBCACHING_REQUEST_TIME_POINT)
			return true;

		return false;
	}
};

struct MGamePlayInfo
{
	int nPlayTime;
	int	nKillCount;
	int nDeathCount;
	int	nXP;
	int	nBP;

	void Reset()
	{
		nPlayTime = 0;
		nKillCount = 0;
		nDeathCount = 0;
		nXP = 0;
		nBP = 0;
	}
};

// ĳ������ Ŭ������
struct MMatchCharClanInfo
{
	int					m_nClanID;							// db���� Ŭ�� ID
	char				m_szClanName[ CLAN_NAME_LENGTH ];	// Ŭ�� �̸�
	MMatchClanGrade		m_nGrade;							// Ŭ�������� ����
	int					m_nContPoint;						// Ŭ�� �⿩��
	string				m_strDeleteDate;

	MMatchCharClanInfo() {  Clear(); }
	void Clear()
	{
		m_nClanID = 0; 
		// m_szClanName[0] = 0; 
		m_nGrade=MCG_NONE;
		m_nContPoint = 0;
		m_strDeleteDate.clear();

		memset( m_szClanName, 0, CLAN_NAME_LENGTH );
	}
	bool IsJoined() { return (m_nClanID == 0) ? false : true; }
public:
	int GetClanID() { return m_nClanID; }
};


#define DEFAULT_CHARINFO_MAXWEIGHT		100
#define DEFAULT_CHARINFO_SAFEFALLS		0
#define DEFAULT_CHARINFO_BONUSRATE		0.0f
#define DEFAULT_CHARINFO_PRIZE			0




/// ĳ���� ����
class MMatchCharInfo
{
public:
	unsigned long int	m_nCID;
	int					m_nCharNum;
	char				m_szName[MATCHOBJECT_NAME_LENGTH];
	int					m_nLevel;
	MMatchSex			m_nSex;			// ����
	int					m_nHair;		// �Ӹ�
	int					m_nFace;		// ��
	unsigned long int	m_nXP;
	int					m_nBP;
	float				m_fBonusRate;
	int					m_nPrize;
	int					m_nHP;
	int					m_nAP;
	int					m_nMaxWeight;
	int					m_nSafeFalls;
	int					m_nFR;
	int					m_nCR;
	int					m_nER;
	int					m_nWR;
	unsigned long int	m_nEquipedItemCIID[MMCIP_END];
	MMatchItemMap		m_ItemList;			// ������ ����
	MMatchEquipedItem	m_EquipedItem;		// ����ϰ� �ִ� ������ ����
	MMatchCharClanInfo	m_ClanInfo;			// Ŭ�� ����

	// ����Ʈ ������.
	MQuestItemMap		m_QuestItemList;
	DBQuestCachingData	m_DBQuestCachingData;
	DBQuestCachingData& GetDBQuestCachingData() { return m_DBQuestCachingData; }


	// ���� ���̺�
	MQuestMonsterBible	m_QMonsterBible;

	// gamble item.
	MMatchGambleItemManager m_GambleItemManager;

	unsigned long int	m_nTotalPlayTimeSec;		// �ɸ����� �� �÷��� �ð�
	unsigned long int	m_nPlayTimeSec;				// �����ؼ� �÷����� �ð�
	unsigned long int	m_nConnTime;				// ������ �ð�(1�� = 1000)
	unsigned long int	m_nBattleStartTime;			// ��Ʋ ���۽ð�

	unsigned long int	m_nTotalKillCount;			// ��ü ų��
	unsigned long int	m_nTotalDeathCount;			// ��ü ������
	unsigned long int	m_nConnKillCount;			// �������ķ� ������ ų��
	unsigned long int	m_nConnDeathCount;			// �������ķ� ������ ������
	unsigned long int   m_nConnXP;					// �������ķ� ������ ����ġ

	unsigned long int   m_nBattleStartXP;			// ��Ʋ ���� ����ġ

	bool				m_IsSendMyItemListByRequestClient;

	unsigned int		m_nRank;

protected:
	DBCharCachingData	m_DBCachingData;
public:
	MMatchCharInfo() : m_nCID(0), m_nCharNum(0), m_nLevel(0), m_nSex(MMS_MALE), m_nFace(0),
		               m_nHair(0), m_nXP(0), m_nBP(0), m_fBonusRate(DEFAULT_CHARINFO_BONUSRATE), m_nPrize(DEFAULT_CHARINFO_PRIZE), m_nHP(0),
					   m_nAP(0), m_nMaxWeight(DEFAULT_CHARINFO_MAXWEIGHT), m_nSafeFalls(DEFAULT_CHARINFO_SAFEFALLS),
					   m_nFR(0), m_nCR(0), m_nER(0), m_nWR(0),
					   m_nConnTime(0), m_nTotalKillCount(0), m_nTotalDeathCount(0), m_nConnKillCount(0), m_nConnDeathCount(0), 
					   m_nConnXP(0), m_nRank(0), m_nBattleStartTime(0), m_nPlayTimeSec(0), m_nTotalPlayTimeSec(0), m_nBattleStartXP(0)
	{
		memset(m_szName, 0, MATCHOBJECT_NAME_LENGTH);
		memset(m_nEquipedItemCIID, 0, sizeof(m_nEquipedItemCIID));
		memset(&m_DBCachingData, 0, sizeof(m_DBCachingData));
		memset(&m_QMonsterBible, 0, sizeof(MQuestMonsterBible) );

		m_QuestItemList.Clear();
		m_DBQuestCachingData.Reset();
		m_CharGamePlayInfo.Reset();


		m_IsSendMyItemListByRequestClient = false;

		m_EquipedItem.SetOwner( this );
	}
	bool EquipFromItemList();
	void ClearItems();
	void Clear();
	void GetTotalWeight(int* poutWeight, int* poutMaxWeight);


	// db caching���� �Բ� �����ش�.
	void IncKill()
	{ 
		m_nTotalKillCount += 1;
		m_nConnKillCount += 1;
		m_DBCachingData.nAddedKillCount += 1;
		m_CharGamePlayInfo.nKillCount +=1;
	}
	void IncDeath()
	{ 
		m_nTotalDeathCount += 1;
		m_nConnDeathCount += 1;
		m_DBCachingData.nAddedDeathCount += 1;
		m_CharGamePlayInfo.nDeathCount += 1;
	}
	void IncBP(int nAddedBP)		
	{ 
		m_nBP += nAddedBP;
		m_DBCachingData.nAddedBP += nAddedBP;
		m_CharGamePlayInfo.nBP += nAddedBP;
	}
	void DecBP(int nDecBP)
	{ 
		m_nBP -= nDecBP;
		m_DBCachingData.nAddedBP -= nDecBP;
		m_CharGamePlayInfo.nBP -= nDecBP;
	}
	void IncXP(int nAddedXP)
	{ 
		m_nConnXP += nAddedXP;
		m_nXP += nAddedXP;
		m_DBCachingData.nAddedXP += nAddedXP;
		m_CharGamePlayInfo.nXP += nAddedXP;
	}
	void DecXP(int nDecXP)
	{ 
		m_nConnXP -= nDecXP; 
		m_nXP -= nDecXP; 
		m_DBCachingData.nAddedXP -= nDecXP; 
		m_CharGamePlayInfo.nXP -= nDecXP;
	}

	void UpdatePlayTime(int nPlayTime)
	{
		int nAddedPlayTime = nPlayTime - m_nPlayTimeSec;

		m_nPlayTimeSec = nPlayTime;
		m_nTotalPlayTimeSec += nAddedPlayTime;
		m_DBCachingData.nAddedPlayTime += nAddedPlayTime;
	}

	DBCharCachingData* GetDBCachingData() { return &m_DBCachingData; }

protected:
	MMatchCharBattleTimeRewardInfoMap m_CharBRInfoMap;

	MGamePlayInfo m_CharGamePlayInfo;
public:
	MMatchCharBattleTimeRewardInfoMap& GetBRInfoMap() { return m_CharBRInfoMap; }
	MGamePlayInfo* GetCharGamePlayInfo() { return &m_CharGamePlayInfo; }

};

class MMatchTimeSyncInfo {
protected:
	int				m_nFoulCount;
	unsigned long	m_nLastSyncClock;
public:
	MMatchTimeSyncInfo()				{ m_nFoulCount=0; m_nLastSyncClock=0; }
	virtual ~MMatchTimeSyncInfo()		{}

	int GetFoulCount()					{ return m_nFoulCount; }
	void AddFoulCount()					{ m_nFoulCount++; }
	void ResetFoulCount()				{ m_nFoulCount = 0; }
	unsigned long GetLastSyncClock()	{ return m_nLastSyncClock; }
	void Update(unsigned long nClock)	{ m_nLastSyncClock = nClock; }
};

// MatchObject�� ���Ӿȿ��� ����ϴ� ������
struct MMatchObjectGameInfo
{
	bool		bJoinedGame;		// ���ӿ� ���������� ���� - �������� ����������
};


class MMatchDisconnStatusInfo
{
public :
	MMatchDisconnStatusInfo() 
	{
		m_DisconnStatus						= MMDS_CONNECTED;
		m_dwLastDisconnStatusUpdatedTime	= timeGetTime();
		m_bIsSendDisconnMsg					= false;
		m_dwDisconnSetTime					= 0;
		m_bIsUpdateDB						= false;
	}

	~MMatchDisconnStatusInfo() {}

	const MMatchDisconnectStatus	GetStatus()				{ return m_DisconnStatus; }
	const DWORD						GetLastUpdatedTime()	{ return m_dwLastDisconnStatusUpdatedTime; }
	const DWORD						GetMsgID()				{ return m_dwMsgID; }
	const MMatchHackingType			GetHackingType()		{ return m_HackingType; } 
	const MMatchBlockLevel			GetBlockLevel()			{ return m_BlockLevel; }
	const string&					GetEndDate()			{ return m_strEndDate; }
	const string&					GetComment()			{ return m_strComment; }
		
	const bool	IsSendDisconnMsg()	{ return m_bIsSendDisconnMsg; }
	void		SendCompleted()		{ m_bIsSendDisconnMsg = false; }	// MMatchServer���� Ŀ�ǵ� ó���� ���ؼ� ���...
																		// IsSendDisconnMsg�� ���� ���� �޽����� ������ �ϴ��� �˻���,
																		// Ŀ�ǵ带 �����Ŀ��� SendCompleted()�� ȣ���Ͽ� ������ �ߺ����� ������ ���� ���´�.
																		// �� ���� ����� ������ ���� ��. -by SungE. 2006-03-07.

	const bool IsUpdateDB()			{ return m_bIsUpdateDB; }
	void UpdateDataBaseCompleted()	{ m_bIsUpdateDB = false; } // Update�Ǹ��� ���� BlockType�� ���������� false�� ����.

	const bool IsDisconnectable( const DWORD dwTime = timeGetTime() )
	{
		if( (MMDS_DISCONNECT == GetStatus()) && (MINTERVAL_DISCONNECT_STATUS_MIN < (dwTime - m_dwDisconnSetTime)) )
			return true;
		return false;
	}
	
	void SetStatus( const MMatchDisconnectStatus Status )	
	{
		m_DisconnStatus = Status; 
		if( MMDS_DISCONN_WAIT == Status )
			SendDisconnMsgPrepared();

		if( MMDS_DISCONNECT == Status )
			m_dwDisconnSetTime = (timeGetTime() - 2000);
		
	}
	void SetUpdateTime( const DWORD dwTime )					{ m_dwLastDisconnStatusUpdatedTime = dwTime; }
	void SetMsgID( const DWORD dwMsgID )						{ m_dwMsgID = dwMsgID; }
	void SetHackingType( const MMatchHackingType HackingType )	{ m_HackingType = HackingType; m_bIsUpdateDB = true; } // ���ο� BlockType�� �����Ǹ� 
																													   //  DB������Ʈ �غ� ������.
	void SetBlockLevel( const MMatchBlockLevel BlockLevel )		{ m_BlockLevel = BlockLevel; }
	void SetEndDate( const string& strEndDate )					{ m_strEndDate = strEndDate; }
	void SetComment( const string& strComment )					{ m_strComment = strComment; }
	
	void Update( const DWORD dwTime )
	{
		if( (dwTime - GetLastUpdatedTime()) > MINTERVAL_DISCONNECT_STATUS_MIN ) 
		{
			if( MMDS_DISCONN_WAIT == GetStatus() ) {
				SetStatus( MMDS_DISCONNECT );		
			}
			
			SetUpdateTime( dwTime );
		}
	}

private :
	void SendDisconnMsgPrepared()	{ m_bIsSendDisconnMsg = true; }

private :
	MMatchDisconnectStatus	m_DisconnStatus;
	DWORD					m_dwLastDisconnStatusUpdatedTime;
	DWORD					m_dwDisconnSetTime;
	DWORD					m_dwMsgID;
	MMatchHackingType		m_HackingType;
	MMatchBlockLevel		m_BlockLevel;	// Level������ ���� ��, �α��۾��� �ϴµ����� ������.
	string					m_strEndDate;
	string					m_strComment;
	bool					m_bIsSendDisconnMsg;
	bool					m_bIsUpdateDB;

	const static DWORD MINTERVAL_DISCONNECT_STATUS_MIN;
};


struct MMatchObjectChannelInfo
{
	MUID			uidChannel;
	MUID			uidRecentChannel;
	bool			bChannelListTransfer;
	MCHANNEL_TYPE	nChannelListType;
	unsigned long	nChannelListChecksum;
	int				nTimeLastChannelListTrans;
	void Clear()
	{
		uidChannel = MUID(0,0);
		uidRecentChannel = MUID(0,0);
		bChannelListTransfer = false;
		nChannelListType = MCHANNEL_TYPE_PRESET;
		nChannelListChecksum = 0;
		nTimeLastChannelListTrans = 0;
	}
};

class MAsyncJob;
class MBMatchGameguard;


struct MASYNCJOBQ
{
	list<MAsyncJob*>	DBJobQ;
	bool				bIsRunningAsyncJob;
	int					nLastJobID;
};


#define MAX_CHAT_FLOOD_COUNT	5
#define MAX_BAN_CHAT_TICK		30 * 1000

class MMatchChatFloodInfo 
{
protected:
	DWORD			m_dwBanTick;
	unsigned short	m_dwBanCount;

	list<int>	m_ChatFloodList;
public:
	MMatchChatFloodInfo() : m_dwBanTick(0), m_dwBanCount(0)
	{}

	~MMatchChatFloodInfo()
	{
		m_dwBanTick = 0;
		m_dwBanCount = 0;
		
		m_ChatFloodList.clear();
	}

	bool PushChatTickCount()
	{
		DWORD dwFirstTick = 0;
		DWORD dwCurTickCount = GetTickCount();

		if( m_ChatFloodList.size() >= MAX_CHAT_FLOOD_COUNT )
		{
			dwFirstTick = m_ChatFloodList.front();
			m_ChatFloodList.pop_front();			
		}

		m_ChatFloodList.push_back(dwCurTickCount);

		// �� ó���� Tick�� �����Ϸ��� Tick���� ���̰� 1�� ���ϸ�.. Flooding�̴�!
		return (dwCurTickCount - dwFirstTick < 1000)? true : false;
	}

	bool IsBanUser()
	{
		DWORD dwCurTickCount = GetTickCount();

		if( dwCurTickCount - m_dwBanTick < MAX_BAN_CHAT_TICK) return true;

		m_dwBanTick = 0;
		return false;
	}

	void SetBanUser()				{ m_dwBanTick = GetTickCount(); m_dwBanCount++;}
};

class MMatchObjectDuelTournamentCharInfo
{
protected:
	bool m_bIsChallenge;
	bool m_bIsSettingData;

	int m_nTP;
	int m_nWins;
	int m_nLoses;
	int m_nRanking;
	int m_nRankingIncrease;
	int m_nQuaterFinalCount;
	int m_nQuaterFinalWins; 
	int m_nSemiFinalCount;
	int m_nSemiFinalWins;
	int m_nFinalCount;
	int m_nFinalWins;
	int	m_nLeaveCount;
	int	m_nLastWeekGrade;					// �����ʸ�Ʈ ������ ��� ([1~10] 1�� ����� ���, 0�� ��ȿ��)

	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

	list<DTRankingInfo*> m_SideRankingList;
public:
	MMatchObjectDuelTournamentCharInfo()
	{
		m_nTP = 0;
		m_nWins = 0;
		m_nLoses = 0;
		m_nRanking = 0;
		m_nRankingIncrease = 0;
		m_nQuaterFinalCount = 0;
		m_nQuaterFinalWins = 0;
		m_nSemiFinalCount = 0;
		m_nSemiFinalWins = 0;
		m_nFinalCount = 0;
		m_nFinalWins = 0;

		m_nLeaveCount = 0;
		m_nLastWeekGrade = 0;

		m_bIsChallenge = false;
		m_bIsSettingData = false;

		memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	MMatchObjectDuelTournamentCharInfo(MMatchObjectDuelTournamentCharInfo *pDTCharInfo)
	{
		SetCharInfo(pDTCharInfo->GetTP()
			, pDTCharInfo->GetWins()
			, pDTCharInfo->GetLoses()
			, pDTCharInfo->GetRanking()
			, pDTCharInfo->GetRankingIncrease()
			, pDTCharInfo->GetFinalCount()
			, pDTCharInfo->GetFinalWins()
			, pDTCharInfo->GetSemiFinalCount()
			, pDTCharInfo->GetSemiFinalWins()
			, pDTCharInfo->GetQuaterFinalCount()
			, pDTCharInfo->GetQuaterFinalWins()
			, pDTCharInfo->GetLeaveCount()
			, pDTCharInfo->GetLastWeekGrade()
			, pDTCharInfo->GetTimeStamp());
	}

	~MMatchObjectDuelTournamentCharInfo()
	{
		RemoveSideRankingAll();
	}

	void SetCharInfo(int nTP, int nWins, int nLoses, int nRanking, int nRankingIncrease, 
		int nFinalCount, int nFinalWins, int nSemiFinalCount, int nSemiFinalWins, int nQuaterFinalCount, int nQuaterFinalWins, int nLeaveCount, int nLastWeekGrade, char* szTimeStamp)
	{
		m_nTP = nTP;
		m_nWins = nWins;
		m_nLoses = nLoses;
		m_nRanking = nRanking;
		m_nRankingIncrease = nRankingIncrease;
		m_nQuaterFinalCount = nQuaterFinalCount;
		m_nQuaterFinalWins = nQuaterFinalWins;
		m_nSemiFinalCount = nSemiFinalCount;
		m_nSemiFinalWins = nSemiFinalWins;
		m_nFinalCount = nFinalCount;
		m_nFinalWins = nFinalWins;

		m_nLeaveCount = nLeaveCount;
		m_nLastWeekGrade = nLastWeekGrade;

		m_bIsChallenge = false;
		m_bIsSettingData = true;

		memcpy(m_szTimeStamp, szTimeStamp, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}

	list<DTRankingInfo*>* GetSideRankingList()	{ return &m_SideRankingList;}

	void AddSideRankingInfo(DTRankingInfo* pRankingInfo)
	{
		DTRankingInfo *pNewInfo = new DTRankingInfo;
		memcpy(pNewInfo, pRankingInfo, sizeof(DTRankingInfo));
		m_SideRankingList.push_back(pNewInfo);
	}

	void RemoveSideRankingAll()
	{	
		list<DTRankingInfo*>::iterator iter = m_SideRankingList.begin();
		for(; iter != m_SideRankingList.end();){
			DTRankingInfo* pInfo = (*iter);
			iter = m_SideRankingList.erase(iter);
			delete pInfo;
		}
	}

	int GetTP()					{ return m_nTP; }
	int GetWins()				{ return m_nWins; }
	int GetLoses()				{ return m_nLoses; }
	int GetRanking()			{ return m_nRanking; }
	int GetRankingIncrease()	{ return m_nRankingIncrease; }
	int GetQuaterFinalCount()	{ return m_nQuaterFinalCount; }
	int GetQuaterFinalWins()	{ return m_nQuaterFinalWins; }
	int GetSemiFinalCount()		{ return m_nSemiFinalCount; }
	int GetSemiFinalWins()		{ return m_nSemiFinalWins; }
	int GetFinalCount()			{ return m_nFinalCount; }
	int GetFinalWins()			{ return m_nFinalWins; }
	int GetLeaveCount()			{ return m_nLeaveCount; }
	int GetLastWeekGrade()		{ return m_nLastWeekGrade; }
	char *GetTimeStamp()		{ return m_szTimeStamp; }
	
	void SetTP(int nTP)					{ m_nTP = nTP; }
	void SetLastWeekGrade(int grade)	{ m_nLastWeekGrade = grade; }

	void IncreaseWins()					{ ++m_nWins; }
	void IncreaseLoses()				{ ++m_nLoses; }
	void IncreaseQuaterFinalCount()		{ ++m_nQuaterFinalCount; }
	void IncreaseQuaterFinalWins()		{ ++m_nQuaterFinalWins; }
	void IncreaseSemiFinalCount()		{ ++m_nSemiFinalCount; }
	void IncreaseSemiFinalWins()		{ ++m_nSemiFinalWins; }
	void IncreaseFinalCount()			{ ++m_nFinalCount; }
	void IncreaseFinalWins()			{ ++m_nFinalWins; }
	void IncreaseLeaveCount()			{ ++m_nLeaveCount; }

	bool IsChallengeDuelTournament()	{ return m_bIsChallenge; }
	void SetChallengeDuelTournament(bool bValue) { m_bIsChallenge = bValue; }

	bool IsJoinDuelTournament()	{ return m_bIsChallenge; }
	void SetJoinDuelTournament(bool bValue) { m_bIsChallenge = bValue; }

	bool IsSettingData() { return m_bIsSettingData; }
};

class MMatchObject : public MObject {
protected:
	MMatchAccountInfo			m_AccountInfo;		// ���� ����
	MMatchCharInfo*				m_pCharInfo;		// ĳ���� ����
	MMatchFriendInfo*			m_pFriendInfo;		// ģ������
	MMatchPlace					m_nPlace;			// ��ġ ����
	MMatchTimeSyncInfo			m_nTimeSyncInfo;	// ���ǵ��� ����	
	MMatchObjectChannelInfo		m_ChannelInfo;		// ä�� ����
	MMatchObjectGameInfo		m_GameInfo;			// ���Ӿȿ����� ����
	MMatchDisconnStatusInfo		m_DisconnStatusInfo;// �������� �������� ���� ����.
	MMatchObjectHShieldInfo		m_HSieldInfo;

	bool			m_bHacker;						// ���� ����� ����
	bool			m_bBridgePeer;
	bool			m_bRelayPeer;
	MUID			m_uidAgent;

	DWORD			m_dwIP;
	char 			m_szIP[64];
	unsigned int	m_nPort;
	bool			m_bFreeLoginIP;					// �ο����� ������� �α��� ���� Ŭ���̾�Ʈ

	unsigned char	m_nPlayerFlags;					// �ֹ߼� �Ӽ��� (MTD_PlayerFlags)
	unsigned long	m_nUserOptionFlags;				// �Ӹ�,�ʴ� �źε��� �ɼ�

	MUID			m_uidStage;
	MUID			m_uidChatRoom;

	bool			m_bStageListTransfer;
	unsigned long	m_nStageListChecksum;
	unsigned long	m_nStageListLastChecksum;
	int				m_nTimeLastStageListTrans;
	int				m_nStageCursor;

	MRefreshClientChannelImpl		m_RefreshClientChannelImpl;
	MRefreshClientClanMemberImpl	m_RefreshClientClanMemberImpl;

	MMatchObjectStageState	m_nStageState;	// ���濡���� ��������
	MMatchTeam		m_nTeam;
	int				m_nLadderGroupID;
	bool			m_bLadderChallenging;	// Ŭ���� ����� ��������� ����

//	bool			m_bStageMaster;	
	bool			m_bEnterBattle;
	bool			m_bAlive;
	unsigned int	m_nDeadTime;

	bool			m_bNewbie;				// �ڽ��� ĳ���͵��� �ְ����� 10�����̻��̸� �Թ�ä�ο� �� �� ����.
	bool			m_bForcedEntried;		// ������ �÷��̾����� ����.
	bool			m_bLaunchedGame;

	unsigned int			m_nKillCount;	// �Ѷ��� ���μ�
	unsigned int			m_nDeathCount;	// �Ѷ��� ������
	unsigned long int		m_nAllRoundKillCount;	// ��ü ������ ų��
	unsigned long int		m_nAllRoundDeathCount;	// ��ü ������ ������

	bool			m_bWasCallVote;		// �� ���� �ȿ��� ��ǥ���Ǹ� �ߴ°�?

	bool			m_bDBFriendListRequested;		// ģ������Ʈ DB�� ������û�� �ߴ��� ����
	unsigned long int	m_nTickLastPacketRecved;	// �ֱ� ��Ŷ �����ð�
	unsigned long int	m_nLastHShieldMsgRecved;
	bool			m_bHShieldMsgRecved;
	string				m_strCountryCode3; // filter���� ������.

	//DWORD	m_dwLastHackCheckedTime;
	//DWORD	m_dwLastRecvNewHashValueTime;
	//bool	m_bIsRequestNewHashValue;

	DWORD	m_dwLastSpawnTime;				// ���������� ���� �Ǿ��� �ð�

	unsigned long int m_nLastPingTime;		// ����Ʈ���, �� ���� �ð�
	unsigned long int m_nQuestLatency;		// ����Ʈ���

	bool				m_bIsLoginCompleted;

	unsigned long		m_dwLastSendSuicideCommand;	// ������ ������ MC_MATCH_REQUEST_SUICIDE�� �������� ������ �ִ�.	
													// �̰��� ���� ���ؼ�, ���� �ð��� ������ ������ �ϴ� ������� �����Ͽ���.

	DWORD				m_dwLastSendGambleItemTime;

	bool				m_IsSendFirstGameguardRequest;
	bool				m_IsRecvFirstGameguardResponse;

public :
	MASYNCJOBQ			m_DBJobQ;


protected:
	void UpdateChannelListChecksum(unsigned long nChecksum)	{ m_ChannelInfo.nChannelListChecksum = nChecksum; }
	unsigned long GetChannelListChecksum()					{ return m_ChannelInfo.nChannelListChecksum; }

	void UpdateStageListChecksum(unsigned long nChecksum)	{ m_nStageListChecksum = nChecksum; }
	unsigned long GetStageListChecksum()					{ return m_nStageListChecksum; }
	MMatchObject() : MObject() 
	{
	}
	void DeathCount()				{ m_nDeathCount++; m_nAllRoundDeathCount++; }
	void KillCount()				{ m_nKillCount++; m_nAllRoundKillCount++; }

//	void CheckClientHashValue( const DWORD dwTime );
public:
	MMatchObject(const MUID& uid);
	virtual ~MMatchObject();

	char* GetName() { 
		if (m_pCharInfo)
			return m_pCharInfo->m_szName; 
		else
			return "Unknown";
	}
	char* GetAccountName()			{ return m_AccountInfo.m_szUserID; }

	bool IsLoginCompleted() { return m_bIsLoginCompleted; }
	void LoginCompleted() { m_bIsLoginCompleted = true; }
	void LoginNotCompleted() { m_bIsLoginCompleted = false; }

	bool GetBridgePeer()			{ return m_bBridgePeer; }
	void SetBridgePeer(bool bValue)	{ m_bBridgePeer = bValue; }
	bool GetRelayPeer()				{ return m_bRelayPeer; }
	void SetRelayPeer(bool bRelay)	{ m_bRelayPeer = bRelay; }
	const MUID& GetAgentUID()		{ return m_uidAgent; }
	void SetAgentUID(const MUID& uidAgent)	{ m_uidAgent = uidAgent; }

	void SetPeerAddr(DWORD dwIP, char* szIP, unsigned short nPort)	{ m_dwIP=dwIP; strcpy(m_szIP, szIP); m_nPort = nPort; }
	DWORD GetIP()					{ return m_dwIP; }
	char* GetIPString()				{ return m_szIP; }
	unsigned short GetPort()		{ return m_nPort; }
	bool GetFreeLoginIP()			{ return m_bFreeLoginIP; }
	void SetFreeLoginIP(bool bFree)	{ m_bFreeLoginIP = bFree; }

	void ResetPlayerFlags()						{ m_nPlayerFlags = 0; }
	unsigned char GetPlayerFlags()				{ return m_nPlayerFlags; }
	bool CheckPlayerFlags(unsigned char nFlag)	{ return (m_nPlayerFlags&nFlag?true:false); }
	void SetPlayerFlag(unsigned char nFlagIdx, bool bSet)	
	{ 
		if (bSet) m_nPlayerFlags |= nFlagIdx; 
		else m_nPlayerFlags &= (0xff ^ nFlagIdx);
	}

	void SetUserOption(unsigned long nFlags)	{ m_nUserOptionFlags = nFlags; }
	bool CheckUserOption(unsigned long nFlag)	{ return (m_nUserOptionFlags&nFlag?true:false); }

	MUID GetChannelUID()						{ return m_ChannelInfo.uidChannel; }
	void SetChannelUID(const MUID& uid)			{ SetRecentChannelUID(m_ChannelInfo.uidChannel); m_ChannelInfo.uidChannel = uid; }
	MUID GetRecentChannelUID()					{ return m_ChannelInfo.uidRecentChannel; }
	void SetRecentChannelUID(const MUID& uid)	{ m_ChannelInfo.uidRecentChannel = uid; }

	MUID GetStageUID()					{ return m_uidStage; }
	void SetStageUID(const MUID& uid)	{ m_uidStage = uid; }
	MUID GetChatRoomUID()				{ return m_uidChatRoom; }
	void SetChatRoomUID(const MUID& uid){ m_uidChatRoom = uid; }

	bool CheckChannelListTransfer()	{ return m_ChannelInfo.bChannelListTransfer; }
	void SetChannelListTransfer(const bool bVal, const MCHANNEL_TYPE nChannelType=MCHANNEL_TYPE_PRESET);

	bool CheckStageListTransfer()	{ return m_bStageListTransfer; }
	void SetStageListTransfer(bool bVal)	{ m_bStageListTransfer = bVal; UpdateStageListChecksum(0); }

	MRefreshClientChannelImpl* GetRefreshClientChannelImplement()		{ return &m_RefreshClientChannelImpl; }
	MRefreshClientClanMemberImpl* GetRefreshClientClanMemberImplement()	{ return &m_RefreshClientClanMemberImpl; }

	MMatchTeam GetTeam()			{ return m_nTeam; }
	void SetTeam(MMatchTeam nTeam);
	MMatchObjectStageState GetStageState()	{ return m_nStageState; }
	void SetStageState(MMatchObjectStageState nStageState)	{ m_nStageState = nStageState; }
	bool GetEnterBattle()			{ return m_bEnterBattle; }
	void SetEnterBattle(bool bEnter){ m_bEnterBattle = bEnter; }
	bool CheckAlive()				{ return m_bAlive; }
	void SetAlive(bool bVal)		{ m_bAlive = bVal; }
	void SetKillCount(unsigned int nKillCount) { m_nKillCount = nKillCount; }
	unsigned int GetKillCount()		{ return m_nKillCount; }
	void SetDeathCount(unsigned int nDeathCount) { m_nDeathCount = nDeathCount; }
	unsigned int GetDeathCount()	{ return m_nDeathCount; }
	unsigned int GetAllRoundKillCount()	{ return m_nAllRoundKillCount; }
	unsigned int GetAllRoundDeathCount()	{ return m_nAllRoundDeathCount; }
	void SetAllRoundKillCount(unsigned int nAllRoundKillCount) { m_nAllRoundKillCount = nAllRoundKillCount; }
	void SetAllRoundDeathCount(unsigned int nAllRoundDeathCount) { m_nAllRoundDeathCount = nAllRoundDeathCount; }
	void FreeCharInfo();
	void FreeFriendInfo();
	void OnDead();
	void OnKill();
	bool IsEnabledRespawnDeathTime(unsigned int nNowTime);

	void SetForcedEntry(bool bVal) { m_bForcedEntried = bVal; }
	bool IsForcedEntried() { return m_bForcedEntried; }
	void SetLaunchedGame(bool bVal) { m_bLaunchedGame = bVal; }
	bool IsLaunchedGame() { return m_bLaunchedGame; }
	void CheckNewbie(int nCharMaxLevel);
	bool IsNewbie()					{ return m_bNewbie; }
	bool IsHacker()					{ return m_bHacker; }
	void SetHacker(bool bHacker)	{ m_bHacker = bHacker; }

	inline bool WasCallVote()						{ return m_bWasCallVote; }
	inline void SetVoteState( const bool bState )	{ m_bWasCallVote = bState; }

	inline unsigned long int	GetTickLastPacketRecved()		{ return m_nTickLastPacketRecved; }
	inline unsigned long int	GetLastHShieldMsgRecved()		{ return m_nLastHShieldMsgRecved; }
	inline bool				GetHShieldMsgRecved()			{ return m_bHShieldMsgRecved; }
	inline void				SetHShieldMsgRecved(bool bSet)	{ m_bHShieldMsgRecved = bSet; }


	void UpdateTickLastPacketRecved();
	void UpdateLastHShieldMsgRecved();

	//void SetLastRecvNewHashValueTime( const DWORD dwTime )	
	//{ 
	//	m_dwLastRecvNewHashValueTime = dwTime; 
	//	m_bIsRequestNewHashValue = false; 
	//}

	DWORD GetLastSpawnTime( void)					{ return m_dwLastSpawnTime;		}
	void SetLastSpawnTime( DWORD dwTime)			{ m_dwLastSpawnTime = dwTime;	}

	inline unsigned long int	GetQuestLatency();
	inline void					SetQuestLatency(unsigned long int l);
	inline void					SetPingTime(unsigned long int t);

	const DWORD GetLastSendSuicideCmdTime()							{ return m_dwLastSendSuicideCommand; }
	inline void SetLastSendSuicideCmdTime( const DWORD dwTime )		{ m_dwLastSendSuicideCommand = dwTime; }

	const DWORD GetLastSendGambleItemTime() const { return m_dwLastSendGambleItemTime; }
	void SetLastSendGambleItemTime( const DWORD dwCurTime )	{ m_dwLastSendGambleItemTime = dwCurTime; }
public:
	// Ladder ����
	int GetLadderGroupID()			{ return m_nLadderGroupID; }
	void SetLadderGroupID(int nID)	{ m_nLadderGroupID = nID; }
	void SetLadderChallenging(bool bVal) { m_bLadderChallenging = bVal; }
	bool IsLadderChallenging()			{ return m_bLadderChallenging; }		// Ŭ���� ����� ��������� ����
public:
	MMatchAccountInfo* GetAccountInfo() { return &m_AccountInfo; }
	MMatchCharInfo* GetCharInfo()	{ return m_pCharInfo; }
	void SetCharInfo(MMatchCharInfo* pCharInfo);
	
	MMatchFriendInfo* GetFriendInfo()	{ return m_pFriendInfo; }
	void SetFriendInfo(MMatchFriendInfo* pFriendInfo);
	bool DBFriendListRequested()	{ return m_bDBFriendListRequested; }
	MMatchPlace GetPlace()			{ return m_nPlace; }
	void SetPlace(MMatchPlace nPlace);
	MMatchTimeSyncInfo* GetSyncInfo()	{ return &m_nTimeSyncInfo; }
//	MMatchObjectAntiHackInfo* GetAntiHackInfo()		{ return &m_AntiHackInfo; }
	MMatchDisconnStatusInfo& GetDisconnStatusInfo() { return  m_DisconnStatusInfo; }
	MMatchObjectHShieldInfo* GetHShieldInfo()		{ return &m_HSieldInfo; }

	/// ƽ ó��
	virtual void Tick(unsigned long int nTime);

	void OnStageJoin();
	void OnEnterBattle();
	void OnLeaveBattle();
	void OnInitRound();

	void SetStageCursor(int nStageCursor);
	const MMatchObjectGameInfo* GetGameInfo() { return &m_GameInfo; }

	//filter
	void			SetCountryCode3( const string strCountryCode3 ) { m_strCountryCode3 = strCountryCode3; }
	const string&	GetCountryCode3() const							{ return m_strCountryCode3; }

	void DisconnectHacker( MMatchHackingType eType );
// 	void SetXTrapHackerDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );
// 	void SetHShieldHackerDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );
// 	void SetBadFileCRCDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );
// 	void SetBadUserDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );
// 	void SetGameguardHackerDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );
// 	void SetDllInjectionDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );
// 	void SetInvalidStageSettingDisconnectWaitInfo( const MMatchDisconnectStatus DisStatus = MMDS_DISCONN_WAIT );

public:
	enum MMO_ACTION
	{
		MMOA_STAGE_FOLLOW,
		MMOA_MAX
	};
	bool CheckEnableAction(MMO_ACTION nAction);		// �ش� �׼��� �������� ���� - �ʿ��Ҷ����� �߰��Ѵ�.

	bool m_bQuestRecvPong;
	DWORD m_dwHShieldCheckCount;

public :
	void DeleteGameguard();

	const bool IsSendFirstGameguardRequest() const	{ return m_IsSendFirstGameguardRequest; }
	void FirstGameguardReqeustIsSent()				{ m_IsSendFirstGameguardRequest = true; }

	const bool IsRecvFirstGameguardResponse() const { return m_IsRecvFirstGameguardResponse; }
	void FirstGameguardResponseIsRecved()			{ m_IsRecvFirstGameguardResponse = true; }

	void ResetCustomItemUseCount();
	const bool IsHaveCustomItem();
	const bool IncreaseCustomItemUseCount();

	bool IsEquipCustomItem(int nItemId);


///< ä��â ���� �� ��� ��� ����
///< ȫ����(2009.08.05)
protected :
	MMatchChatFloodInfo m_ChatFloodInfo;
public:
	bool CheckChatFlooding()		{ return m_ChatFloodInfo.PushChatTickCount();}
	bool IsChatBanUser()			{ return m_ChatFloodInfo.IsBanUser();}
	void SetChatBanUser()			{ m_ChatFloodInfo.SetBanUser();}

///< ��� ��ʸ�Ʈ ����
///< ȫ����(2009.09.25)
protected :
	MMatchObjectDuelTournamentCharInfo *m_pDuelTournamentCharInfo;

public:	
	MMatchObjectDuelTournamentCharInfo* GetDuelTournamentCharInfo()		{ return m_pDuelTournamentCharInfo; }
	void SetDuelTournamentCharInfo(MMatchObjectDuelTournamentCharInfo *pDTCharInfo);	
	void FreeDuelTournamentInfo();	

	bool IsChallengeDuelTournament() { 
		if( m_pDuelTournamentCharInfo == NULL ) return false;
		return m_pDuelTournamentCharInfo->IsChallengeDuelTournament(); 
	}

	void SetChallengeDuelTournament(bool bValue) { 
		if( m_pDuelTournamentCharInfo == NULL ) return;
		m_pDuelTournamentCharInfo->SetChallengeDuelTournament(bValue); 
	}

	bool IsJoinDuelTournament() { 
		if( m_pDuelTournamentCharInfo == NULL ) return false;
		return m_pDuelTournamentCharInfo->IsJoinDuelTournament(); 
	}

	void SetJoinDuelTournament(bool bValue) { 
		if( m_pDuelTournamentCharInfo == NULL ) return;
		m_pDuelTournamentCharInfo->SetJoinDuelTournament(bValue); 
	}

protected:
	MMatchObjectCharBuff	m_CharBuffInfo;

public:
	void FreeCharBuff()							{ m_CharBuffInfo.FreeCharBuffInfo(); }
	MMatchObjectCharBuff*	GetCharBuff()		{ return &m_CharBuffInfo; }
	
///< Punishment ����
///< ȫ����(2010.08.09)
protected:
	MMatchAccountPenaltyInfo m_AccountPenaltyInfo;

public:
	MMatchAccountPenaltyInfo* GetAccountPenaltyInfo() { return &m_AccountPenaltyInfo; }

protected:
	unsigned int m_nLastCheckBattleTimeReward;

	void SetLastCheckBattleTimeReward(unsigned int nVal)	{ m_nLastCheckBattleTimeReward = nVal; }
	unsigned int GetLastCheckBattleTimeReward()				{ return m_nLastCheckBattleTimeReward; }

	void SetBattleTimeReward(bool bVal);
	void BattleTimeReward(unsigned int nTime);

protected:
	void ResetGamePlayInfo() { if( m_pCharInfo != NULL ) m_pCharInfo->GetCharGamePlayInfo()->Reset(); }
};

class MMatchObjectList : public map<MUID, MMatchObject*>{};


// ĳ���� �����Ҷ� �ִ� �⺻ ������
struct MINITIALCOSTUME
{
	// ����
	unsigned int nMeleeItemID;
	unsigned int nPrimaryItemID;
	unsigned int nSecondaryItemID;
	unsigned int nCustom1ItemID;
	unsigned int nCustom2ItemID;

	// ��� ������
	unsigned int nChestItemID;
	unsigned int nHandsItemID;
	unsigned int nLegsItemID;
	unsigned int nFeetItemID;
};

#define MAX_COSTUME_TEMPLATE		6
const MINITIALCOSTUME g_InitialCostume[MAX_COSTUME_TEMPLATE][2] = 
{ 
	{{1, 5001, 4001, 30301, 0,     21001, 0, 23001, 0},	{1, 5001, 4001, 30301, 0,     21501, 0, 23501, 0}},	// �ǳ���Ʈ
	{{2, 5002, 0,    30301, 0,     21001, 0, 23001, 0},	{2, 5002, 0,    30301, 0,     21501, 0, 23501, 0}},	// ��������
	{{1, 4005, 5001, 30401, 0,     21001, 0, 23001, 0},	{1, 4005, 5001, 30401, 0,     21501, 0, 23501, 0}},	// �ּ���
	{{2, 4001, 0,    30401, 0,     21001, 0, 23001, 0},	{2, 4001, 0,    30401, 0,     21501, 0, 23501, 0}},	// ��ī��Ʈ
	{{2, 4002, 0,    30401, 30001, 21001, 0, 23001, 0},	{2, 4002, 0,    30401, 30001, 21501, 0, 23501, 0}},	// ��������Ʈ
	{{1, 4006, 0,	 30101, 30001, 21001, 0, 23001, 0},	{1, 4006, 4006, 30101, 30001, 21501, 0, 23501, 0}}	// ����
};

/*
const unsigned int g_InitialHair[4][2] = 
{
	{10000, 10022},
	{10001, 10023},
	{10002, 10024},
	{10003, 10025}
};
*/

#define MAX_COSTUME_HAIR		5
const string g_szHairMeshName[MAX_COSTUME_HAIR][2] = 
{
	{"eq_head_01", "eq_head_pony"},
	{"eq_head_02", "eq_head_hair001"},
	{"eq_head_08", "eq_head_hair04"},
	{"eq_head_05", "eq_head_hair006"},
	{"eq_head_08", "eq_head_hair002"}		// �̰� ���� ������ - ���߿� �ٸ� �𵨷� ��ü�ص� ��
};

#define MAX_COSTUME_FACE		4
const string g_szFaceMeshName[MAX_COSTUME_FACE][2] = 
{
	// �⺻
	{"eq_face_01", "eq_face_001"},
	{"eq_face_02", "eq_face_002"},
	{"eq_face_04", "eq_face_003"},
	{"eq_face_05", "eq_face_004"},
/*
	{"eq_face_newface01", "eq_face_newface01"},
	{"eq_face_newface02", "eq_face_newface02"},
	{"eq_face_newface03", "eq_face_newface03"},
    {"eq_face_newface04", "eq_face_newface04"},
	{"eq_face_newface05", "eq_face_newface05"},
	{"eq_face_newface06", "eq_face_newface06"},
	{"eq_face_newface07", "eq_face_newface07"},
	{"eq_face_newface08", "eq_face_newface08"},
	{"eq_face_newface09", "eq_face_newface09"},
	{"eq_face_newface10", "eq_face_newface10"},
	{"eq_face_newface11", "eq_face_newface11"},
	{"eq_face_newface12", "eq_face_newface12"},
	{"eq_face_newface13", "eq_face_newface13"},
	{"eq_face_newface14", "eq_face_newface14"}, 
	{"eq_face_newface15", "eq_face_newface15"}, */
};
//���ڴ� eq_face_newface01 - 13
//���ڴ� eq_face_newface01 - 15

// MC_MATCH_STAGE_ENTERBATTLE Ŀ�ǵ忡�� ���̴� �Ķ��Ÿ
enum MCmdEnterBattleParam
{
	MCEP_NORMAL = 0,
	MCEP_FORCED = 1,		// ����
	MCEP_END
};

// ���� �� �ִ� ���������� üũ
bool IsEquipableItem(unsigned long int nItemID, int nPlayerLevel, MMatchSex nPlayerSex);

inline bool IsEnabledObject(MMatchObject* pObject) 
{
	if ((pObject == NULL) || (pObject->GetCharInfo() == NULL)) return false;
	return true;
}

inline bool IsAdminGrade(MMatchUserGradeID nGrade) 
{
	if ((nGrade == MMUG_EVENTMASTER) || 
		(nGrade == MMUG_ADMIN) || 
		(nGrade == MMUG_DEVELOPER))
		return true;

	return false;
}

inline bool IsAdminGrade(MMatchObject* pObject) 
{
	if (pObject == NULL) return false;

	return IsAdminGrade(pObject->GetAccountInfo()->m_nUGrade);
}


//////////////////////////////////////////////////////////////////////////


unsigned long int MMatchObject::GetQuestLatency()
{
	unsigned long int nowTime = timeGetTime();

	unsigned long int ret = nowTime - m_nLastPingTime;

	if (ret > m_nQuestLatency)
		m_nQuestLatency = ret;

	return m_nQuestLatency;
}

void MMatchObject::SetQuestLatency(unsigned long int l)
{
	m_nQuestLatency = l - m_nLastPingTime;
}

void MMatchObject::SetPingTime(unsigned long int t)
{
	m_nLastPingTime = t;
}

#endif
