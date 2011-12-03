#include "stdafx.h"
#include "MXml.h"
#include "MMatchLocale.h"
#include "MMatchConfig.h"
#include "MMatchEventFactory.h"


WORD MMatchEventFactoryManager::m_LoadEventSize = 0;


MMatchEventFactory::MMatchEventFactory()
{
}


MMatchEventFactory::~MMatchEventFactory()
{
}


bool MMatchEventFactory::InsertEventData( const EventData& EvnData )
{
	if( (MMatchEvent::GAME_TYPE_ALL != EvnData.dwGameType) &&
		((0 > EvnData.dwGameType) && (8 < EvnData.dwGameType)) )
		return false;

	if( MMatchEvent::GAME_TYPE_ALL == EvnData.dwGameType )
		m_vAllGameTypeEventData.push_back( EvnData );
	else
	{
		GameTypeEventMap::iterator itGameType = 
			m_mGameTypeEvent.find( EvnData.dwGameType );
		if( m_mGameTypeEvent.end() == itGameType )
		{
			EventDataVec edv;
			edv.push_back( EvnData );
			m_mGameTypeEvent.insert( GameTypeEventMap::value_type(EvnData.dwGameType, edv) );
		}
		else
			itGameType->second.push_back( EvnData );

		return true;
	}
	
	return false;
}


bool MMatchEventFactory::GetEventList( const DWORD dwGameType, EventPtrVec& EvnPtrVec )
{
	if( (MMatchEvent::GAME_TYPE_ALL != dwGameType) &&
		((0 > dwGameType) && (MMATCH_GAMETYPE_MAX < dwGameType)) )
		return false;

	if( !m_vAllGameTypeEventData.empty() )
	{
		if( !MakeEventList(m_vAllGameTypeEventData, EvnPtrVec) )
			return false;
	}

	if( !m_mGameTypeEvent.empty() && MMatchEvent::GAME_TYPE_ALL != dwGameType )
	{
		GameTypeEventMap::iterator itGameType = 
			m_mGameTypeEvent.find( dwGameType );
		if( m_mGameTypeEvent.end() != itGameType )
			return MakeEventList( itGameType->second, EvnPtrVec );
	}

	return true;
}


bool MMatchEventFactory::MakeEventList( const EventDataVec& EvnDataVec, EventPtrVec& EvnPtrVec )
{
	bool bIsComplete = true;
	MMatchEvent* pEvent;
	EventDataVec::const_iterator it, end;
	it = EvnDataVec.begin();
	for( end = EvnDataVec.end(); it != end; ++it )
	{
		if( !CheckUsableEventTimeByEndTime(it->End) )
			continue;

		pEvent = CreateEvent( it->dwEventID );
		if( 0 != pEvent )
		{
			if( pEvent->InitEvent() )
			{
				pEvent->Set( it->dwEventListID, it->EventType, it->dwGameType, 
					it->dwElapsedTime, it->dwPercent, it->dwRate, 
					it->Start, it->End, it->strName,
					it->strAnnounce,
					it->fXPBonusRatio,
					it->fBPBonusRatio,
					it->EventPartTimeVec );
				EvnPtrVec.push_back( pEvent );
				pEvent = 0;
			}
			else
			{
				mlog( "MMatchEventFactory::MakeEventList - Init����. ID:%u Name:%s.\n",
					pEvent->GetEventID(), pEvent->GetName().c_str() );
				delete pEvent;
				pEvent = 0;
				bIsComplete = false;
			}
		}
		else
			bIsComplete = false;
	}

	return bIsComplete;
}


MMatchEvent* MMatchEventFactory::CreateEvent( const DWORD dwEventID )
{
	switch( dwEventID )
	{
	case EID_PROBABILITY_PER_TIME :
		{
			MMatchProbabiltyEventPerTime* pEvent = new MMatchProbabiltyEventPerTime;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailMLog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else 
				return 0;
		}
		break;

	case EID_XP_BONUS :
		{
			MMatchXPBonusEvent* pEvent = new MMatchXPBonusEvent;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailMLog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else
				return 0;
		}
		break;

	case EID_BP_BONUS :
		{
			MMatchBPBonusEvent* pEvent = new MMatchBPBonusEvent;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailMLog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else
				return 0;
		}
		break;

	case EID_UINQUEPLAYERLIST :
		{
			MMatchUniquePlayerListEvent* pEvent = new MMatchUniquePlayerListEvent;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailMLog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else
				return NULL;
		}
		break;

	default :
		{
			ASSERT( 0 );
		}
		break;
	}

	return 0;
}


void MMatchEventFactory::CreateFailMLog( MMatchEvent* pEvent, const DWORD dwEventID )
{
	mlog( "Fail to create event : created event:%u, event:%u\n",
		pEvent->GetEventID(), dwEventID );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MMatchEventFactoryManager::MMatchEventFactoryManager()
{
	m_bIsUsable = false;
}


MMatchEventFactoryManager::~MMatchEventFactoryManager()
{
}


bool MMatchEventFactoryManager::LoadEventListXML( MZFileSystem* pFileSystem, const string& strFileName )
{
	MXmlDocument	xmlIniData;
	xmlIniData.Create();

	//	<-----------------
	char *buffer;
	MZFile mzf;

	if(pFileSystem) 
	{
		if(!mzf.Open(strFileName.c_str(),pFileSystem)) 
		{
			if(!mzf.Open(strFileName.c_str())) 
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else 
	{
		if(!mzf.Open(strFileName.c_str()))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
	mzf.Close();
	//	<------------------

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(EL_LOCALE, szTagName))
		{
			ParseLocale( chrElement );
		}
	}

	xmlIniData.Destroy();
	return true;
}


bool MMatchEventFactoryManager::LoadEventListXML( const string& strFileName )
{
	MXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(strFileName.c_str()))
	{
		xmlIniData.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	m_LoadEventSize = 0;

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(EL_LOCALE, szTagName))
		{
			ParseLocale( chrElement );
		}
	}

	xmlIniData.Destroy();
	return true;
}


void MMatchEventFactoryManager::ParseLocale( MXmlElement& chrElement )
{
	EventData ed;
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );
		
		if( 0 == stricmp(EL_COUNTRY, szAttrName) )
		{
			/*  // MBaseLocale.h // 
			enum MCountry
			{
				MC_KOREA			= 82,		// �ѱ�
				MC_US				= 1,		// �̱�(���ͳ׼ų�)
				MC_JAPAN			= 81,		// �Ϻ�
			};
			*/

			string strCountry;
			switch( MGetLocale()->GetCountry() )
			{
			case MC_KOREA :
				{
					strCountry = "kor";
				}
				break;

			case MC_US :
				{
					strCountry = "us";
				}
				break;

			case MC_JAPAN :
				{
					strCountry = "jpn";
				}
				break;

			case MC_BRAZIL :
				{
					strCountry = "brz";
				}
				break;

			case MC_INDIA :
				{
					strCountry = "ind";
				}
				break;

			default :
				{
					ASSERT( 0 );
				}
				break;
			}

			// ���� ������ ���� ���� Ÿ�Ը� �Ľ���. 
            // if( 0 == stricmp(strCountry.c_str(), szAttrValue) )
			// �����ڵ� ���� ����. �̹� �������� ������ ����.
			if( true )
			{
				MXmlElement chrNode;
				char szTag[ 128 ];
				const int nChrCnt = chrElement.GetChildNodeCount();
				for( int j = 0; j < nChrCnt; ++j )
				{
					chrNode = chrElement.GetChildNode( j );
					chrNode.GetTagName( szTag );

					if( 0 == stricmp(EL_EVENT, szTag) )
					{
						ParseEvent( chrNode );
					}
				}
			}
		}
	}
}


void MMatchEventFactoryManager::ParseEvent( MXmlElement& chrElement )
{
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	
	DWORD						dwEventListID = 0;
	DWORD						dwEventID = 0;
	string						strEventName;
	string						strAnnounce;
	EVENT_TYPE					EventType;
	DWORD						dwElapsedTime = 0;
	DWORD						dwPercent = 0;
	DWORD						dwRate = 0;
	vector< EventServerType >	vServerType;
	vector< EventGameType >		vGameType;
	SYSTEMTIME					Start, End;
	float						fXPBonusRatio = 0.0f;
	float						fBPBonusRatio = 0.0f;
	vector< EventPartTime >		EventPartTimeVec;

	memset( &Start, 0, sizeof(SYSTEMTIME) );
	memset( &End, 0, sizeof(SYSTEMTIME) );

	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_EVENT_LIST_ID, szAttrName) )
		{
			dwEventListID = static_cast< DWORD >( atoi(szAttrValue) );
			ASSERT( 0 < dwEventListID );
			continue;
		}

		if( 0 == stricmp(EL_EVENTID, szAttrName) )
		{
			dwEventID = static_cast< DWORD >( atol(szAttrValue) );
			if( NULL == MMatchEventDescManager::GetInstance().Find(dwEventID) )
			{
				ASSERT( 0 && "Event.xml without Event ID." );
				mlog( "MMatchEventFactoryManager::ParseEvent - Event.xml without Event ID(%u).\n",
					dwEventID );
				return;
			}
			continue;
		}

		if( 0 == stricmp(EL_NAME, szAttrName) )
		{
			strEventName = MGetStringResManager()->GetString( string(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_EVENTTYPE, szAttrName) )
		{
			EventType = static_cast< EVENT_TYPE >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_ELAPSEDTIME, szAttrName) )
		{
			dwElapsedTime = static_cast< DWORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_PERCENT, szAttrName) )
		{
			dwPercent = static_cast< DWORD >( atol(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_RATE, szAttrName) )
		{
			dwRate = static_cast< DWORD >( atol(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_ANNOUNCE, szAttrName) )
		{
			strAnnounce = MGetStringResManager()->GetString( string(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_XPBONUS_RATIO, szAttrName) )
		{
			fXPBonusRatio = static_cast<float>( atoi(szAttrValue) ) / 100.0f;
			continue;
		}

		if( 0 == stricmp(EL_BPBONUS_RATIO, szAttrName) )
		{
			fBPBonusRatio = static_cast<float>( atoi(szAttrValue) ) / 100.0f;
			continue;
		}
	}

	MXmlElement chrNode;
	char szTag[ 128 ];
	const int nChrNodeCnt = chrElement.GetChildNodeCount();

	EventPartTimeVec.clear();
	
	for( int j = 0; j < nChrNodeCnt; ++j )
	{
		chrNode = chrElement.GetChildNode( j );
		chrNode.GetTagName( szTag );

		if (szTag[0] == '#') continue;

		if( 0 == stricmp(EL_SERVERTYPE, szTag) )
		{
			ParseServerType( chrNode, vServerType );
			continue;
		}

		if( 0 == stricmp(EL_GAMETYPE, szTag) )
		{
			ParseGameType( chrNode, vGameType );
			continue;
		}

		if( 0 == stricmp(EL_STARTTIME, szTag) )
		{
			ParseStartEndTime( chrNode, Start );
			continue;
		}

		if( 0 == stricmp(EL_ENDTIME, szTag) )
		{
			ParseStartEndTime( chrNode, End );
			continue;
		}

		if( 0 == stricmp(EL_PART_TIME, szTag) )
		{
			ParseEventPartTime(chrNode, EventPartTimeVec );
			continue;
		}
	}

	// check start end time.
	if( !CheckUsableEventTimeByEndTime(End) )
	{
#ifdef _DEBUG
		mlog( "Time out Event(%u:%u.%u.%u.%u~%u.%u.%u.%u)\n", 
			dwEventID,
			Start.wYear, Start.wMonth, Start.wDay, Start.wHour,
			End.wYear, End.wMonth, End.wDay, End.wHour );
#endif
		return;
	}
	
	// check server type.
	vector< EventServerType >::iterator itSvrTyp, endSvrTyp;
	bool bUseEvent = false;
	endSvrTyp = vServerType.end();
	for( itSvrTyp = vServerType.begin(); itSvrTyp != endSvrTyp; ++itSvrTyp )
	{
		// ��� ������ ����.
		if( MSM_ALL == itSvrTyp->ServerType )
		{
			bUseEvent = true;
			continue;
		}

		// ���� ���� Ÿ�Կ��� ����.
		if( MGetServerConfig()->GetServerMode() == itSvrTyp->ServerType )
		{
			bUseEvent = true;
			continue;
		}
	}

	ASSERT( (0 < Start.wYear) && (0 < Start.wMonth) && (0 < Start.wDay) && (0 <= Start.wHour) &&
			(0 < End.wYear) && (0 < End.wMonth) && (0 < End.wDay) && (0 <= End.wHour) );

	// check game type.
	if( bUseEvent )
	{
		EventData ed;
		vector< EventGameType >::iterator itGmTyp, endGmTyp;
		endGmTyp = vGameType.end();
		for( itGmTyp = vGameType.begin(); itGmTyp != endGmTyp; ++itGmTyp )
		{
			// insert event.

			ed.dwEventListID	= dwEventListID;
			ed.dwEventID		= dwEventID;
			ed.EventType		= EventType;
			ed.dwGameType			= itGmTyp->GameType;
			ed.ServerType		= MGetServerConfig()->GetServerMode();
			ed.dwElapsedTime	= dwElapsedTime;
			ed.dwPercent		= dwPercent;
			ed.dwRate			= dwRate;
			ed.Start			= Start;
			ed.End				= End;
			ed.strName			= strEventName;
			ed.strAnnounce		= strAnnounce;
			ed.fXPBonusRatio	= fXPBonusRatio;
			ed.fBPBonusRatio	= fBPBonusRatio;

			ed.EventPartTimeVec.swap( EventPartTimeVec );

			InsertEvent( ed );

			++m_LoadEventSize;
		}		
	}
}


void MMatchEventFactoryManager::ParseServerType( MXmlElement& chrElement, vector< EventServerType >& vServerType )
{
	EventServerType ServerType;
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_ORDER, szAttrName) )
		{
			ServerType.dwOrder = static_cast< DWORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_TYPE, szAttrName) )
		{
			ServerType.ServerType = static_cast< MMatchServerMode >( atoi(szAttrValue) );
			continue;
		}
	}

	vServerType.push_back( ServerType );
}


void MMatchEventFactoryManager::ParseGameType( MXmlElement& chrElement, vector< EventGameType >& vGameType )
{
	EventGameType GameType;
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_ORDER, szAttrName) )
		{
			GameType.dwOrder = static_cast< DWORD >( atol(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_TYPE, szAttrName) )
		{
			GameType.GameType = static_cast< MMATCH_GAMETYPE >( atoi(szAttrValue) );
			continue;
		}
	}

	vGameType.push_back( GameType );
}


void MMatchEventFactoryManager::ParseStartEndTime( MXmlElement& chrElement, SYSTEMTIME& stTime )
{
	memset( &stTime, 0, sizeof(stTime) );

	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_YEAR, szAttrName) )
		{
			stTime.wYear =  static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_MONTH, szAttrName) )
		{
			stTime.wMonth = static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_DAY, szAttrName) )
		{
			stTime.wDay = static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_HOUR, szAttrName) )
		{
			stTime.wHour = static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}
	}
}


void MMatchEventFactoryManager::ParseEventPartTime( MXmlElement& chrElement, vector<EventPartTime>& EventPartTimeVec )
{
	EventPartTime ept;

	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		/*
		if( 0 == stricmp(EL_ORDER, szAttrName) )
		{
			continue;
		}
		*/

		if( 0 == stricmp(EL_START_HOUR, szAttrName) )
		{
			ept.btStartHour = static_cast< BYTE >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_END_HOUR, szAttrName) )
		{
			ept.btEndHour = static_cast< BYTE >( atoi(szAttrValue) );
			continue;
		}
	}

#ifdef _DEBUG
	// �ߺ��Ǵ� ������ �ִ��� �˻�.
	vector< EventPartTime >::iterator it, end;
	end = EventPartTimeVec.end();
	for( it = EventPartTimeVec.begin(); it != end; ++it )
	{
		if( (it->btStartHour <= ept.btStartHour) && (ept.btStartHour <= it->btEndHour) )
		{
			ASSERT( 0 );
		}
		
		if( (it->btStartHour <= ept.btEndHour) && (ept.btEndHour <= it->btEndHour) )
		{
			ASSERT( 0 );
		}
	}
#endif

	EventPartTimeVec.push_back( ept );
}


bool MMatchEventFactoryManager::GetEventList( const DWORD dwGameType, const EVENT_TYPE EventType, EventPtrVec& EvnPtrVec )
{
	if( !m_bIsUsable )
		return true;

	switch( EventType )
	{
	case ET_BEGIN :
		{
			return GetOnBeginEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	case ET_ONGAME :
		{
			return GetOnGameEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	case ET_END :
		{
			return GetOnEndEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	case ET_CUSTOM_EVENT :
		{
			return GetCustomEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	default :
		{
			ASSERT( 0 && "���ǵ��� ���� �̺�Ʈ Ÿ��." );
			return false;
		}
		break;
	}
	return false;
}


bool MMatchEventFactoryManager::InsertEvent( const EventData& EvnData )
{
	switch( EvnData.EventType )
	{
	case ET_BEGIN :
		{
			return GetOnBeginEventFactory().InsertEventData( EvnData );
		}
		break;

	case ET_ONGAME :
		{
			return GetOnGameEventFactory().InsertEventData( EvnData );
		}
		break;

	case ET_END :
		{
			return GetOnEndEventFactory().InsertEventData( EvnData );
		}
		break;

	case ET_CUSTOM_EVENT :
		{
			return GetCustomEventFactory().InsertEventData( EvnData );
		}
		break;

	default :
		{
			ASSERT( 0 && "���ǵ��� ���� �̺�Ʈ Ÿ��." );
			return false;
		}
		break;
	}
	return false;
}

