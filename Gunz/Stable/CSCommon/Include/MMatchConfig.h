#ifndef _MMATCHCONFIG_H
#define _MMATCHCONFIG_H

#include <string>
#include <list>
#include <set>
using namespace std;

#include "MMatchMap.h"
#include "MMatchGlobal.h"
#include "MMatchKillTrackerConfig.h"
#include "MMatchPowerLevelingConfig.h"


enum NHN_SERVERMODE
{
	NSM_ALPHA = 1,
	NSM_REAL,

	NSM_END,
};

class MMatchConfig
{
private:
	char				m_szDB_DNS[64];				///< DB DNS
	char				m_szDB_UserName[64];		///< DB Username
	char				m_szDB_Password[64];		///< DB Password

	int					m_nMaxUser;					///< �ִ� ������
	int					m_nServerID;
	char				m_szServerName[256];		///< �����̸�
	int					m_nServerPort;				///< ������Ʈ
	int					m_nServerUDPPort;			///< ����UDP��Ʈ

	MMatchServerMode	m_nServerMode;				///< �������
	bool				m_bRestrictionMap;			///< �������� �ִ��� ���� - default : false
	set<int>			m_EnableMaps;				///< �������� ������� ������ ��
	list<string>		m_FreeLoginIPList;			///< �����ο� ���� IP
	bool				m_bCheckPremiumIP;			///< �����̾� IP üũ
	string				m_strCountry;				
	string				m_strLanguage;

	// enabled ������ - ini���� �������� �ʴ´�.
	bool				m_bEnabledCreateLadderGame;	///< Ŭ���� ������������ ����

	// ini���� �����ϴ� enabled
	bool				m_bEnabledSurvivalMode;		///< �����̹� ��� Ȱ��ȭ
	bool				m_bEnabledDuelTournament;	///< ��� ��ʸ�Ʈ Ȱ��ȭ

	// -- �Ϻ� �ݸ��� ����
	char				m_NJ_szDBAgentIP[64];
	int					m_NJ_nDBAgentPort;
	int					m_NJ_nGameCode;

	// filter ��� ����.
	bool				m_bUseFilter;				/// ���� ����� ������(0:������. 1:���.)
	bool				m_bAcceptInvalidIP;			/// ����Ʈ�� ���� IP��� ����.(0:������� ����. 1:���.)

	// evironment.
	bool				m_bIsUseHShield;
	bool				m_bIsUseXTrap;
	bool				m_bIsUseEvent;
	bool				m_bIsUseFileCrc;
	bool				m_bIsUseMD5;
	bool				m_bBlockFlooding;			///< Ŀ�ǵ� flooding �� �Ͼ�� ���Ѵ�


	bool				m_bIsUseBlockHancking;
	bool				m_bIsUseItemConsistency;

	// �����̹� ��ŷ ����Ʈ �޾ƿ��� �ð�
	DWORD				m_dwSurvivalRankingDailyRequestHour;		// 0~23
	DWORD				m_dwSurvivalRankingDailyRequestMin;			// 0~59

	// �����ʸ�Ʈ ������
	DWORD				m_dwDuelTournamentMatchMakingInterval;			// ���ʸ��� ��Ī �۾��� ������ ���ΰ�
	DWORD				m_dwDuelTournamentMatchMaingAcceptableTpGap;	// �⺻������ TP ���̰� �� �� ������ �������� ��Ī�� �����ϴ�
	DWORD				m_dwDuelTournamentMatchMaingWaitLimit;			// �� �ð����� ���� ����� ������ TP���̸� �����ϰ� ���� ��Ī���ش�
	DWORD				m_dwDuelTournamentServiceStartTime;				// �����ʸ�Ʈ ���� ���� �ð�(�Ϸ� ����) ���� ex)0~23�̸� 24�ð� ��� �����Ѵ�.
	DWORD				m_dwDuelTournamentServiceEndTime;				// �����ʸ�Ʈ ���� ������ �ð�(�Ϸ� ����) ����
	bool				m_bSendLoginUserToDuelTournamentChannel;		// �α����� ������ �����ʸ�Ʈ ä�η� ���� ���ΰ�
	

	// BattleTime Reward ���� ������
	DWORD			m_dwBRDescriptionRefreshInterval;


	// debug.
	list<string>		m_DebugLoginIPList;			///< Debug�� IP.
	bool				m_bIsDebugServer;

	// keeper ip.
	string				m_strKeeperIP;				/// Keeper�� server���� ��ſ��� Ű���� ��û���� �˻��ϱ� ���ؼ�.

	bool				m_bIsUseTicket;				/// ����� ����������� �������� ����.(0:�Ϲݼ���, 1:���� ����� ��� ����)

	bool				m_bIsComplete;

	bool				m_bIsUseNHNUSAAuth;
	bool				m_bIsUseGameguard;

	NHN_SERVERMODE		m_NHNServerMode;

	DWORD				m_dwMonitorUDPIP;
	USHORT				m_nUDPPort;
	
	MMatchKillTrackerConfig		m_KillTrackConfig;
	MMatchPowerLevelingConfig	m_PowerLevelingConfig;


	bool				m_bIsUseResourceCRC32CacheCheck;

	bool m_bUseLoopLog;
	DWORD m_dwMaxLoopTimeGap;

private:
	bool GetPrivateProfileBool(const char* szAppName, const char* szKeyName, 
							   bool bDefault, const char* szFileName);
	void AddFreeLoginIP(const char* szIP);
	void AddDebugLoginIP( const char* szIP );
	void ReadEnableMaps();
	void TrimStr(const char* szSrcStr, char* outStr);

	bool InitKillTrackerConfig();
	bool InitPowerLevelingConfig();

	const bool LoadMonitorIPnPort();
	const bool LoadKeeperIP();

	void InitLoopLogConfig();

public:
	MMatchConfig();
	virtual ~MMatchConfig();
	static MMatchConfig* GetInstance();
	bool Create();
	void Destroy();
	void Clear();

	// get
	const char* GetDB_DNS()							{ return m_szDB_DNS; }
	const char* GetDB_UserName()					{ return m_szDB_UserName; }
	const char* GetDB_Password()					{ return m_szDB_Password; }
	const int GetMaxUser()							{ return m_nMaxUser; }
	const int GetServerID()							{ return m_nServerID; }
	const char* GetServerName()						{ return m_szServerName; }
	const int GetServerPort()						{ return m_nServerPort; }
	const int GetServerUDPPort()					{ return m_nServerUDPPort; }
	const MMatchServerMode		GetServerMode()		{ return m_nServerMode; }
	bool IsResMap()									{ return m_bRestrictionMap; }	// �������� �ִ��� ����
	bool IsEnableMap(const MMATCH_MAP nMap)										// �÷��̰����� ������ ����
	{
		if (!m_bRestrictionMap) return true;
		if (m_EnableMaps.find(nMap) != m_EnableMaps.end()) return true;
		return false;
	}
	const IsDebugServer()							{ return m_bIsDebugServer; }
	bool CheckFreeLoginIPList(const char* pszIP);
	bool IsDebugLoginIPList( const char* pszIP );
	bool CheckPremiumIP()							{ return m_bCheckPremiumIP; }

	bool IsEnabledCreateLadderGame()				{ return m_bEnabledCreateLadderGame; }
	void SetEnabledCreateLadderGame(bool bVal)		{ m_bEnabledCreateLadderGame = bVal; }

	bool IsEnabledSurvivalMode()					{ return m_bEnabledSurvivalMode; }
	bool IsEnabledDuelTournament()					{ return m_bEnabledDuelTournament; }

	const char* GetNJDBAgentIP()					{ return m_NJ_szDBAgentIP; }
	int GetNJDBAgentPort()							{ return m_NJ_nDBAgentPort; }
	int GetNJDBAgentGameCode()						{ return m_NJ_nGameCode; }

	const bool IsUseFilter() const						{ return m_bUseFilter; }
	const bool IsAcceptInvalidIP() const				{ return m_bAcceptInvalidIP; }
	void SetUseFilterState( const bool bUse )			{ m_bUseFilter = bUse; }
	void SetAcceptInvalidIPState( const bool bAccept )	{ m_bAcceptInvalidIP = bAccept; }

	const bool IsUseHShield() const	{ return m_bIsUseHShield; }
	const bool IsUseXTrap() const	{ return m_bIsUseXTrap; }
	const bool IsUseEvent() const	{ return m_bIsUseEvent; }
	const bool IsUseFileCrc() const { return m_bIsUseFileCrc; }
	const bool IsUseMD5() const { return m_bIsUseMD5; }
	const bool IsUseBlockFlooding() const { return m_bBlockFlooding; }

	bool IsKeeperIP( const string& strIP )				{ return m_strKeeperIP == strIP; }

	const string& GetKeeperIP() { return m_strKeeperIP; }

	const string& GetCountry()	{ return m_strCountry; }
	const string& GetLanguage() { return m_strLanguage; }

	const bool IsUseTicket()	{ return m_bIsUseTicket; }

	const bool IsUseGamegaurd() { return m_bIsUseGameguard; }
	const bool IsUseNHNUSAAuth()	{ return m_bIsUseNHNUSAAuth; }

	const bool IsComplete() { return m_bIsComplete; }

	const NHN_SERVERMODE GetNHNServerMode() { return m_NHNServerMode; }

	const bool IsClanServer() { return (MSM_CLAN == m_nServerMode); }

	const DWORD GetMonitorUDPIP() { return m_dwMonitorUDPIP; }
	const USHORT GetMonitorUDPPORT()	{ return m_nUDPPort; }

	MMatchKillTrackerConfig& GetKillTrackerConfig() { return m_KillTrackConfig; }
	MMatchPowerLevelingConfig& GetPowerLevelingConfig() { return m_PowerLevelingConfig; }

	const bool IsUseResourceCRC32CacheCheck() { return m_bIsUseResourceCRC32CacheCheck; }

	const bool IsUseLoopLog()		{ return m_bUseLoopLog; }
	const DWORD GetLoopTimeGap()	{ return m_dwMaxLoopTimeGap; }

	bool IsBlockHacking()		{ return m_bIsUseBlockHancking;   }
	bool IsItemConsistency()	{ return m_bIsUseItemConsistency; }

	DWORD GetSurvivalRankingDalyRequestTimeHour() const		{ return m_dwSurvivalRankingDailyRequestHour; }
	DWORD GetSurvivalRankingDalyRequestTimeMinute() const	{ return m_dwSurvivalRankingDailyRequestMin; }

	DWORD GetDuelTournamentMatchMakingInterval() const			{ return m_dwDuelTournamentMatchMakingInterval; }
	DWORD GetDuelTournamentMatchMakingAcceptableTpGap() const	{ return m_dwDuelTournamentMatchMaingAcceptableTpGap; }
	DWORD GetDuelTournamentMatchMakingWaitLimit() const			{ return m_dwDuelTournamentMatchMaingWaitLimit; }
	DWORD GetDuelTournamentServiceStartTime() const				{ return m_dwDuelTournamentServiceStartTime; }
	DWORD GetDuelTournamentServiceEndTime() const					{ return m_dwDuelTournamentServiceEndTime; }

	DWORD GetBRDescriptionRefreshInterval() const			{ return m_dwBRDescriptionRefreshInterval; }

	bool IsSendLoginUserToDuelTournamentChannel() const  { return m_bSendLoginUserToDuelTournamentChannel; }
};

inline MMatchConfig* MGetServerConfig() { return MMatchConfig::GetInstance(); }

inline bool QuestTestServer() { return (MGetServerConfig()->GetServerMode() == MSM_TEST); }


#define SERVER_CONFIG_FILENAME			"./server.ini"


#define SERVER_CONFIG_SERVERMODE_NORMAL			"normal"
#define SERVER_CONFIG_SERVERMODE_CLAN			"clan"
#define SERVER_CONFIG_SERVERMODE_LADDER			"ladder"
#define SERVER_CONFIG_SERVERMODE_EVENT			"event"
#define SERVER_CONFIG_SERVERMODE_TEST			"test"

#define SERVER_CONFIG_DEFAULT_NJ_DBAGENT_IP			"210.174.197.180"
#define SERVER_CONFIG_DEFAULT_NJ_DBAGENT_PORT		7500
#define SERVER_CONFIG_DEFAULT_NJ_DBAGENT_GAMECODE	1013

#define SERVER_CONFIG_DEFAULT_USE_HSHIELD	"0"
#define SERVER_CONFIG_DEFAULT_USE_XTRAP		"0"
#define SERVER_CONFIG_DEFAULT_USE_EVENT		"1"
#define SERVER_CONFIG_DEFAULT_USE_FILECRC	"0"
#define SERVER_CONFIG_DEFAULT_BLOCK_FLOODING "0"
#define SERVER_CONFIG_DEFAULT_USE_KILTRACKER	"0"
#define SERVER_CONFIG_DEFAULT_USE_RESOURCECRC32CACHECHECK "0"

#define SERVER_CONFIG_DEBUG_DEFAULT			"0"

#define SERVER_CONFIG_USE_TICKET			"0"

#endif
