#ifndef _MMATCH_EVENT_FACTORY
#define _MMATCH_EVENT_FACTORY


#include "MBaseGameType.h"
#include "MMatchEventManager.h"


#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;
using std::string;


#define EVENT_LIST_XML_FILE_NAME "EventList.xml"


struct EventServerType
{
	DWORD				dwOrder;
	MMatchServerMode	ServerType;
};


struct EventGameType
{
	DWORD			dwOrder;
	MMATCH_GAMETYPE GameType;
};


struct EventData
{
	EventData()
	{
		dwEventListID	= 0;
		dwEventID		= 0;
		dwElapsedTime	= 0;
		dwPercent		= 0;
		dwRate			= 0;
		fXPBonusRatio	= 0.0f;
		fBPBonusRatio	= 0.0f;
	}

	DWORD					dwEventListID;
	DWORD					dwEventID;
	string					strName;
	string					strAnnounce;
	EVENT_TYPE				EventType;
	DWORD					dwGameType;
	DWORD					dwElapsedTime;
	DWORD					dwPercent;
	DWORD					dwRate;
	MMatchServerMode		ServerType;
	SYSTEMTIME				Start;
	SYSTEMTIME				End;
	float					fXPBonusRatio;
	float					fBPBonusRatio;
	vector< EventPartTime > EventPartTimeVec;
};


typedef vector< EventData >			EventDataVec;
typedef map< DWORD, EventDataVec >	GameTypeEventMap;


class MMatchEventFactory
{
public :
	MMatchEventFactory();
	~MMatchEventFactory();

	bool InsertEventData( const EventData& EvnData );

	bool GetEventList( const DWORD dwGameType, EventPtrVec& EvnPtrVec );

private :
	bool MakeEventList( const EventDataVec& EvnDataVec, EventPtrVec& epc );
	MMatchEvent* CreateEvent( const DWORD dwEventID );

	void CreateFailMLog( MMatchEvent* pEvent, const DWORD dwEventID );

private :
	EventDataVec		m_vAllGameTypeEventData;
	GameTypeEventMap	m_mGameTypeEvent;
};


class MMatchEventFactoryManager
{
public :
	MMatchEventFactoryManager();
	~MMatchEventFactoryManager();

	bool LoadEventListXML( MZFileSystem* pFileSystem, const string& strFileName );
	bool LoadEventListXML( const string& strFileName );

	bool		GetEventList( const DWORD dwGameType, const EVENT_TYPE EventType, EventPtrVec& EvnPtrVec );
	const WORD	GetLoadEventSize() const { return m_LoadEventSize; }

	void SetUsableState( const bool bIsUsable ) { m_bIsUsable = bIsUsable; }

	static MMatchEventFactoryManager& GetInstance()
	{
		static MMatchEventFactoryManager EventFactoryManager;
		return EventFactoryManager;
	}

private :
	MMatchEventFactory& GetOnBeginEventFactory()	{ return m_OnBeginEventFactory; }
	MMatchEventFactory& GetOnGameEventFactory()		{ return m_OnGameEventFactory; }
	MMatchEventFactory& GetOnEndEventFactory()		{ return m_OnEndEventFactory; }
	MMatchEventFactory& GetCustomEventFactory()		{ return m_CustomEventFactory; }

	bool InsertEvent( const EventData& EvnData );

	void ParseLocale( MXmlElement& chrElement );
	void ParseEvent( MXmlElement& chrElement );
	void ParseServerType( MXmlElement& chrElement, vector< EventServerType >& vServerType );
	void ParseGameType( MXmlElement& chrElement, vector< EventGameType >& vGameType );
	void ParseStartEndTime( MXmlElement& chrElement, SYSTEMTIME& stTime );
	void ParseEventPartTime( MXmlElement& chrElement, vector<EventPartTime>& EventPartTimeVec );
	
private :
	MMatchEventFactory m_OnBeginEventFactory;	// ���� ����� ������ �����ϴ� ������ �̺�Ʈ.
	MMatchEventFactory m_OnGameEventFactory;	// ���� ����� ������ ������ ���� ���������� �̺�Ʈ.
	MMatchEventFactory m_OnEndEventFactory;		// ������ ������ ������ �̺�Ʈ.
	MMatchEventFactory m_CustomEventFactory;	// ���� ȣ���ϴ� �̺�Ʈ.

	bool m_bIsUsable;

	static WORD m_LoadEventSize;
};

#endif