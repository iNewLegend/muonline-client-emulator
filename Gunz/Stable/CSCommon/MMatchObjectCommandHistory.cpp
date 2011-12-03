#include "StdAfx.h"
#include "MMatchObjectCommandHistory.h"

MMatchObjectCommandHistory::~MMatchObjectCommandHistory()
{
	UIDHISTORYMAP::iterator itr = m_mapHistories.begin();
	while ( itr!= m_mapHistories.end() )
	{
		MOBJECTCOMMANDHISTORY* pHistory = itr->second;
		itr = m_mapHistories.erase( itr );
		delete pHistory;
	}
}

MOBJECTCOMMANDHISTORY* MMatchObjectCommandHistory::AddNew(MUID uid)
{
	MOBJECTCOMMANDHISTORY* pNew = new MOBJECTCOMMANDHISTORY;
	m_mapHistories.insert( UIDHISTORYMAP::value_type( uid, pNew ) );
	return pNew;
}

void MMatchObjectCommandHistory::SetCharacterInfo(MUID uid, const char* szName, unsigned int nCID)
{
	UIDHISTORYMAP::iterator itr = m_mapHistories.find( uid );

	MOBJECTCOMMANDHISTORY* pHistory = NULL;
	if( itr == m_mapHistories.end() )
		pHistory = AddNew(uid);
	else
		pHistory = itr->second;

	pHistory->m_strName = szName;
	pHistory->m_nCID = nCID;
}

void MMatchObjectCommandHistory::PushCommand(MUID uid, int nCommandID, DWORD dwCurrentTime, bool* pbFloodingSuspect)
{
	UIDHISTORYMAP::iterator itr = m_mapHistories.find( uid );

	MOBJECTCOMMANDHISTORY* pHistory = NULL;
	if( itr == m_mapHistories.end() )
		pHistory = AddNew(uid);
	else
		pHistory = itr->second;

	pHistory->m_dwLastTime = dwCurrentTime;
	pHistory->m_commands.push_back( pair <int, DWORD> ( nCommandID, dwCurrentTime ) );

	if(pbFloodingSuspect)
	{
		DWORD dwFirstCommandTime = pHistory->m_commands.front().second;
		
		// 1000ms ���Ͽ� MAX_COMMAND_HISTORY_COUNT �� �ʰ��ϸ� �÷������� ����
		if(dwCurrentTime - dwFirstCommandTime < 1000 &&
			pHistory->m_commands.size() > MAX_COMMAND_HISTORY_COUNT )
			*pbFloodingSuspect = true;
		else
			*pbFloodingSuspect = false;
	}

	if(pHistory->m_commands.size() > MAX_COMMAND_HISTORY_COUNT )
		pHistory->m_commands.pop_front();
}

MOBJECTCOMMANDHISTORY* MMatchObjectCommandHistory::GetCommandHistory(MUID uid)
{
	UIDHISTORYMAP::iterator itr = m_mapHistories.find( uid );
	if( itr == m_mapHistories.end() )
		return NULL;

	return itr->second;
}

void MMatchObjectCommandHistory::Update( DWORD dwCurrentTime )
{
	for( UIDHISTORYMAP::iterator itr = m_mapHistories.begin(); itr!= m_mapHistories.end(); )
	{
		MOBJECTCOMMANDHISTORY* pHistory = itr->second;
		// �ִ� �����ð��� ������ �����
		if( MAX_HISTORY_HOLD_TIME < (dwCurrentTime-pHistory->m_dwLastTime) )
		{
			itr = m_mapHistories.erase( itr );
			delete pHistory;
		}
		else
			++itr;
	}
}

bool MMatchObjectCommandHistory::Dump( MUID uid )
{
	MOBJECTCOMMANDHISTORY* pHistory = GetCommandHistory(uid);
	if(!pHistory)
		return false;

	mlog("dump %s ( cid %d ) cmd : ", pHistory->m_strName.c_str(), pHistory->m_nCID );
	for(list< pair <int, DWORD> >::iterator i = pHistory->m_commands.begin(); i != pHistory->m_commands.end(); ++i )
	{
		mlog(" %d", i->first);
	}
	mlog("\n");

	return true;
}
