#include "stdafx.h"
#include "MAsyncDBJob_Event.h"


bool MAsyncDBJob_EventLog::Input( AsyncEventObjVec& vEventObj, const string& strEventName, const string& strAnnounce )
{
	if( vEventObj.empty() ) 
		return false;

	m_vEventObj.swap( vEventObj );
	m_strEventName = strEventName;
	m_strAnnounce = strAnnounce;

	return true;
}


void MAsyncDBJob_EventLog::Run( void* pContext )
{
	MMatchDBMgr* pDBMgr = reinterpret_cast< MMatchDBMgr* >( pContext );
	if( 0 == pDBMgr ) 
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	AsyncEventObjVec::iterator	it, end;

	end = m_vEventObj.end();
	for( it = m_vEventObj.begin(); it != end; ++it )
	{
		if( !pDBMgr->InsertEvent(it->dwAID, it->dwCID, m_strEventName) )
			it->uidUser = MUID(0, 0);
	}
	
	SetResult(MASYNC_RESULT_SUCCEED);
}