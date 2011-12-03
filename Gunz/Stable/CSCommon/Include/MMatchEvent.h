#ifndef _MMATCH_EVENT
#define _MMATCH_EVENT


#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

#include "MUID.h"


class MMatchObject;


#define EVENT_XML_FILE_NAME "Event.xml"


#define EV_EVENT	"Event"
#define EV_EVENTID	"id"
#define EV_DESC		"Description"


#define EL_LOCALE			"Locale"
#define EL_COUNTRY			"country"
#define EL_EVENT			"Event"
#define EL_EVENT_LIST_ID	"id"
#define EL_EVENTID			"EventID"
#define EL_NAME				"name"
#define EL_EVENTTYPE		"event_type"
#define EL_ELAPSEDTIME		"elapsed_time"
#define EL_PERCENT			"percent"
#define EL_RATE				"rate"
#define EL_ANNOUNCE			"announce"
#define EL_SERVERTYPE		"ServerType"
#define EL_GAMETYPE			"GameType"
#define EL_STARTTIME		"StartTime"
#define EL_ENDTIME			"EndTime"
#define EL_XPBONUS_RATIO	"xp_bonus_ratio"
#define EL_BPBONUS_RATIO	"bp_bonus_ratio"
#define EL_PART_TIME		"PartTime"
#define		EL_ORDER			"order"
#define		EL_TYPE				"type"
#define		EL_YEAR				"year"
#define		EL_MONTH			"month"
#define		EL_DAY				"day"
#define		EL_HOUR				"hour"
#define		EL_START_HOUR		"start_hour"
#define		EL_END_HOUR			"end_hour"




enum EVENT_TYPE
{
	ET_BEGIN	= 0,		// : ������ �����Ҷ�.
	ET_ONGAME	= 1,		//  : ���� ����� ������ ������ �ؾ� Ȱ��ȭ�Ǵ� �̺�Ʈ.
	ET_END		= 2,		// : ������ �����Ŀ�.

	ET_CUSTOM_EVENT	= 100,	// : ���� ȣ��.

	ET_MAX,
};


enum EVENT_ID
{
	EID_PROBABILITY_PER_TIME = 1,
	EID_XP_BONUS,
	EID_BP_BONUS,
	EID_UINQUEPLAYERLIST,

	EID_END,
};


struct EventPartTime
{
	BYTE btStartHour;
	BYTE btEndHour;
};


class MMatchEvent
{
public :
	const static DWORD		GAME_TYPE_ALL = 100;


	virtual ~MMatchEvent();

	const DWORD GetEventID() const		{ return m_dwEventID; }
	const DWORD GetEventListID() const	{ return m_dwEventListID; }
	const string& GetName()				{ return m_strName; }
	// ������ �̺�Ʈ ó����ƾ�� ����� ���ؼ�. 
	
	bool DoItNow( const DWORD dwCurTime );

	void Set( const DWORD dwEventListID, const DWORD dwEventType, const DWORD dwGameType, const DWORD dwCheckElapsedTime, const DWORD dwPercent, 
		const DWORD dwRate, const SYSTEMTIME& Start, const SYSTEMTIME& End, const string& strName, const string& strAnnounce,
		const float fXPBonusRatio, const float fBPBonusRatio, const vector<EventPartTime>& EventPartTimeVec );

	void StartNewEvent() { m_vEventObj.clear(); }
	void Run();
	void CheckEventObj( MMatchObject* pObj, const DWORD dwCurTime );

	void Reset();

	void SetLastCheckTime( const DWORD dwCurTime );

	// ������ ���� �ʱ�ȭ�� �ʿ��Ҷ��� �̰��� ������ �ϸ� �ȴ�.
	// MMatchEventŬ������ �����ÿ� �� �Լ��� ȣ��Ǿ� ���� �˻�ȴ�.
	virtual bool InitEvent() { return true; }
	// ������ ��ƾ�� �ƴ� ���� ó���� ����ϴ� �̺�Ʈ�� �� �Լ��� �������ؼ� �����.
	// ó���� �ʿ��� ����Ÿ�Ե����ʹ� pContext�� �Ѱܼ� ó��.
	virtual bool CustomCheckEventObj( MMatchObject* pObj, void* pContext ) { return true; }

protected :
	MMatchEvent() { Reset(); }
	MMatchEvent( const DWORD dwEventID ) : m_dwEventID ( dwEventID ), m_dwLastCheckTime( 0 ) {}
	
	bool CheckEventTime();
	bool CheckElapsedTimeIsOverflow( const DWORD dwCurTime );
	bool CheckEventPartTime();

protected : 
	virtual void OnCheckEventObj( MMatchObject* pObj, const DWORD dwCurTime ) {}
	virtual void OnRun() {}

protected :
	DWORD					m_dwEventListID;
	DWORD					m_dwEventID;
	DWORD					m_dwEventType;
	DWORD					m_dwGameType;
	DWORD					m_dwCheckElapsedTime;
	DWORD					m_dwLastCheckTime;
	DWORD					m_dwPercent;
	DWORD					m_dwRate;
	float					m_fXPBonusRatio;
	float					m_fBPBonusRatio;
	SYSTEMTIME				m_Start;
	SYSTEMTIME				m_End;
	string					m_strName;
	string					m_strAnnounce;
	vector< MUID >			m_vEventObj;
	vector< EventPartTime >	m_EventPartTimeVec;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 

class MMatchEventDesc
{
public :
	MMatchEventDesc() {}
	~MMatchEventDesc() {}

	void Set( const DWORD dwEventID, const string& strDesc );

	const DWORD GetEventID() const			{ return m_dwEventID; }
	const string& GetDescription() const	{ return m_strDesc; }

private :
	DWORD	m_dwEventID;
	string	m_strDesc;
};


class MMatchEventDescManager : private map< DWORD, const MMatchEventDesc* >
{
public :
	MMatchEventDescManager() {}
	~MMatchEventDescManager()
	{
		ClearEventDesc();
	}
	
	bool LoadEventXML( MZFileSystem* pFileSystem, const string& strFileName );
	bool LoadEventXML( const string& strFileName );

	const MMatchEventDesc* Find( const DWORD dwEventID );
	void ClearEventDesc();

	static MMatchEventDescManager& GetInstance()
	{
		static MMatchEventDescManager EventDescManager;
		return EventDescManager;
	}

private :
	void ParseEvent( MXmlElement& chrElement );
	bool Insert( const DWORD dwEventID, MMatchEventDesc* pEventDesc );
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �� �Ʒ������� �̺�Ʈ�� �߰� �Ѵ�. ���Ŀ� �̺�Ʈ�� �������� ������ ���Ϸ� �и��Ǿ�� ��.

class MMatchProbabiltyEventPerTime : public MMatchEvent
{
public :
	MMatchProbabiltyEventPerTime() : 
	   MMatchEvent( EID_PROBABILITY_PER_TIME ) {}
	virtual ~MMatchProbabiltyEventPerTime();
	
protected :
	void OnCheckEventObj( MMatchObject* pObj, const DWORD dwCurTime );
	void OnRun();

private :
};


class MMatchXPBonusEvent : public MMatchEvent
{
public :
	MMatchXPBonusEvent() : 
	   MMatchEvent( EID_XP_BONUS ) {}
   virtual ~MMatchXPBonusEvent();

   bool CustomCheckEventObj( MMatchObject* pObj, void* pContext );
};


class MMatchBPBonusEvent : public MMatchEvent
{
public :
	MMatchBPBonusEvent() :
	   MMatchEvent( EID_BP_BONUS ) {}
   virtual ~MMatchBPBonusEvent();

   bool CustomCheckEventObj( MMatchObject* pObj, void* pContext );
};


class MMatchUniquePlayerListEvent : public MMatchEvent
{
private :
	map< MUID, DWORD > m_UniquePlayerList;

public :
	MMatchUniquePlayerListEvent() : MMatchEvent( EID_UINQUEPLAYERLIST ) {}
	~MMatchUniquePlayerListEvent() {}

	void OnCheckEventObj( MMatchObject* pObj, const DWORD dwCurTime );
	void OnRun();
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool CheckUsableEventTimeByStartTime( const SYSTEMTIME& StartTime );
bool CheckUsableEventTimeByEndTime( const SYSTEMTIME& EndTime );
bool CheckUsableEventPartTime( const EventPartTime& ept );

#endif