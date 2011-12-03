#ifndef MMATCHCHANNEL_H
#define MMATCHCHANNEL_H

#include <map>
#include <list>
using namespace std;

#include "MMatchGlobal.h"
#include "MUID.h"
#include "MMatchChannel.h"
#include "MPageArray.h"
#include "MSmartRefresh.h"
#include "MMatchChannelRule.h"


#define DEFAULT_CHANNEL_MAXPLAYERS			200
#define DEFAULT_CHANNEL_MAXSTAGES			100
#define MAX_CHANNEL_MAXSTAGES				500
#define NUM_PLAYERLIST_NODE					6
#define CHANNEL_NO_LEVEL					(-1)
#define CHANNELNAME_STRINGRESID_LEN			64		// �����δ� �̺��� ����� ������ �׷� ��찡 ������ �ʵ��� ASSERT�� Ȯ��

class MMatchObject;
class MMatchStage;
class MCommand;


enum MCHANNEL_TYPE {
	MCHANNEL_TYPE_PRESET	= 0,		// �Ϲ�ä��
	MCHANNEL_TYPE_USER		= 1,		// �缳ä��
	MCHANNEL_TYPE_PRIVATE	= 2,		// ���ä�� - ���� �Ⱦ���
	MCHANNEL_TYPE_CLAN		= 3,		// Ŭ��ä��
	MCHANNEL_TYPE_QUEST		= 4,		// ����Ʈä�� 2006-05-23�߰� - by SungE
	MCHANNEL_TYPE_DUELTOURNAMENT = 5,	// �����ʸ�Ʈä�� 2009-09-21�߰� - by ȫ����
	MCHANNEL_TYPE_MAX
};


// ä�� ����Ʈ �޶�� ��û�Ҷ� ������ ����ü
struct MCHANNELLISTNODE {
	MUID			uidChannel;						// ä�� UID
	short			nNo;							// ä�ι�ȣ
	unsigned char	nPlayers;						// �����ο�
	short			nMaxPlayers;					// �ִ��ο�
	short			nLevelMin;						// �ּҷ���
	short			nLevelMax;						// �ִ뷹��
	char			nChannelType;					// ä��Ÿ��
	char			szChannelName[CHANNELNAME_LEN];	// ä���̸�
	char			szChannelNameStrResId[CHANNELNAME_STRINGRESID_LEN];	// ä���̸��� ��Ʈ�����ҽ� ID (Ŭ���̾�Ʈ���� ����ä���� ������ �̸��� ǥ���ϵ��� �ϱ� ����)
	bool			bIsUseTicket;
	unsigned int	nTicketID;
};

// typedef map<string, MMatchObject*>			MObjectStrMap;
typedef map<int, MMatchStage*>				MChannelStageMap;
typedef MPageArray<MMatchObject*>			MChannelUserArray;


class MMatchChannel {
private:
	MUID			m_uidChannel;
	char			m_szChannelName[CHANNELNAME_LEN];
	char			m_szChannelNameStrResId[CHANNELNAME_STRINGRESID_LEN];
	MCHANNEL_TYPE	m_nChannelType;
	int				m_nMaxPlayers;
	int				m_nLevelMin;
	int				m_nLevelMax;
	int				m_nMaxStages;
	char			m_szRuleName[CHANNELRULE_LEN];
	MCHANNEL_RULE	m_nRuleType;
	//bool			m_bNewbieChannel;		// ����ä���� ���� �ʺ��� �� �� �ִ�.
	
	bool			m_bIsUseTicket;			// ����� ��� ����.
	bool			m_bIsTicketChannel;		// ������� ����ϴ� ä������.
    DWORD			m_dwTicketItemID;		// ����� ItemID.
	
	MUIDRefCache	m_ObjUIDCaches;			// ä����ü �÷��̾��
	MUIDRefCache	m_ObjUIDLobbyCaches;	// �κ� �ִ� �÷��̾��
//	MObjectStrMap	m_ObjStrCaches;

	MMatchStage*	m_pStages[MAX_CHANNEL_MAXSTAGES];
	list<int>		m_UnusedStageIndexList;

	MChannelUserArray			m_UserArray;
	MSmartRefresh				m_SmartRefresh;

	unsigned long	m_nChecksum;	// ��Ϲ� ���� ���ſ�
	unsigned long	m_nLastChecksumTick;

	unsigned long	m_nLastTick;
	unsigned long	m_nEmptyPeriod;

	void JoinLobby(const MUID& uid, const MMatchObject* pObj);
	void LeaveLobby(const MUID& uid);
protected:
	inline bool IsChecksumUpdateTime(unsigned long nTick);
	void UpdateChecksum(unsigned long nTick);
	unsigned long GetEmptyPeriod()	{ return m_nEmptyPeriod; }

public:
	bool CheckTick(unsigned long nClock);
	void Tick(unsigned long nClock);

	unsigned long GetChecksum()		{ return m_nChecksum; }
	bool CheckLifePeriod();

public:
	bool Create(const MUID& uid, const char* pszName, const char* pszRuleName, 
				MCHANNEL_TYPE nType=MCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, 
				int nLevelMin=CHANNEL_NO_LEVEL, int nLevelMax=CHANNEL_NO_LEVEL,
				const bool bIsTicketChannel = false, const DWORD dwTicketItemID = 0, const bool bIsUseTicket = false,
				const char* pszNameStrResId = NULL);
	void Destroy();

	const char* GetName()			{ return m_szChannelName; }
	const char* GetNameStringResId(){ return m_szChannelNameStrResId; }
	const char* GetRuleName()		{ return m_szRuleName; }
	MUID GetUID()					{ return m_uidChannel; }
	MCHANNEL_TYPE GetChannelType()	{ return m_nChannelType; }
	MCHANNEL_RULE GetRuleType()		{ return m_nRuleType; }

	int GetMaxPlayers()				{ return m_nMaxPlayers; }
	int GetLevelMin()				{ return m_nLevelMin; }
	int GetLevelMax()				{ return m_nLevelMax; }
	int	GetMaxStages()				{ return m_nMaxStages; }
	size_t GetObjCount()			{ return m_ObjUIDCaches.size(); }
	int GetPlayers();
	MUIDRefCache::iterator GetObjBegin()		{ return m_ObjUIDCaches.begin(); }
	MUIDRefCache::iterator GetObjEnd()			{ return m_ObjUIDCaches.end(); }
	MUIDRefCache::iterator GetLobbyObjBegin()	{ return m_ObjUIDLobbyCaches.begin(); }
	MUIDRefCache::iterator GetLobbyObjEnd()		{ return m_ObjUIDLobbyCaches.end(); }


	void AddObject(const MUID& uid, MMatchObject* pObj);
	void RemoveObject(const MUID& uid);
public:
	bool AddStage(MMatchStage* pStage);
	void RemoveStage(MMatchStage* pStage);
	bool IsEmptyStage(int nIndex);
	MMatchStage* GetStage(int nIndex);
	int GetPrevStageCount(int nStageIndex);	// nStageIndex�� �������� �ʴ� nStageIndex������ ������� �� ���� 
	int GetNextStageCount(int nStageIndex);	// nStageIndex�� �������� �ʴ� nStageIndex������ ������� �� ���� 
	
	//bool IsNewbieChannel()			{ return m_bNewbieChannel; }

	const bool	IsUseTicket()		{ return m_bIsUseTicket; }
	const bool	IsTicketChannel()	{ return m_bIsTicketChannel; }
	const DWORD GetTicketItemID()	{ return m_dwTicketItemID; }

public:
	MChannelUserArray* GetUserArray()	{ return &m_UserArray; }

public:
	void SyncPlayerList(MMatchObject* pObj, int nPage);
};


struct MMatchClanChannelTicketInfo
{
	MMatchClanChannelTicketInfo() 
	{
		m_bIsTicketChannel	= false;
		m_dwTicketItemID	= 0;
	}

	bool	m_bIsTicketChannel;
	DWORD	m_dwTicketItemID;
};


class MMatchChannelMap : public map<MUID, MMatchChannel*> {
private:
	MUID						m_uidGenerate;
	unsigned long				m_nChecksum;
	map<MUID, MMatchChannel*>	m_TypesChannelMap[MCHANNEL_TYPE_MAX];
	MMatchClanChannelTicketInfo m_ClanChannelTicketInfo;

	void Insert(const MUID& uid, MMatchChannel* pChannel)	{	insert(value_type(uid, pChannel));	}
	MUID UseUID()				{	m_uidGenerate.Increase();	return m_uidGenerate;	}
	
//	void UpdateChecksum(unsigned long nClock);

public:
	MMatchChannelMap()			{	m_uidGenerate = MUID(0,0);	m_nChecksum=0; }
	virtual ~MMatchChannelMap()	{	}
	void Destroy();
	
	MMatchChannel* Find(const MUID& uidChannel);
	MMatchChannel* Find(const MCHANNEL_TYPE nChannelType, const char* pszChannelName);

	bool Add(const char* pszChannelName, const char* pszRuleName, MUID* pAllocUID, MCHANNEL_TYPE nType=MCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, int nLevelMin=-1, int nLevelMax=-1,
		const bool bIsTicketChannel = false, const DWORD dwTicketItemID = 0, const bool bIsUseTicket = false, const char* pszChannelNameStrResId = NULL);
	bool Remove(const MUID& uidChannel, MMatchChannelMap::iterator* pNextItor);
	void Update(unsigned long nClock);

	unsigned long GetChannelListChecksum() { return m_nChecksum; }
	int GetChannelCount(MCHANNEL_TYPE nChannelType);
	
	map<MUID, MMatchChannel*>::iterator GetTypesChannelMapBegin(MCHANNEL_TYPE nType);
	map<MUID, MMatchChannel*>::iterator GetTypesChannelMapEnd(MCHANNEL_TYPE nType);

	const MMatchClanChannelTicketInfo& GetClanChannelTicketInfo() const { return m_ClanChannelTicketInfo; }

	void SetIsTicketClanChannel( const bool bIsTicketClanChannel )	{ m_ClanChannelTicketInfo.m_bIsTicketChannel = bIsTicketClanChannel; }
	void SetClanChannelTicketItemID( const DWORD dwTicketItemID )	{ m_ClanChannelTicketInfo.m_dwTicketItemID = dwTicketItemID; }
};


#endif