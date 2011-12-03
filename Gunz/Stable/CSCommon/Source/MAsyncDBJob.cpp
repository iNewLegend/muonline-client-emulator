#include "stdafx.h"
#include "MMatchServer.h"
#include "MAsyncDBJob.h"
#include "MSharedCommandTable.h"
#include "MBlobArray.h"
#include "MMatchConfig.h"
#include "MMatchQuestGameLog.h"

void MAsyncDBJob_Test::Run(void* pContext)
{
	char szLog[128];
	sprintf(szLog, "Thread=%d , MAsyncDBJob_Test BEGIN \n", GetCurrentThreadId());
	mlog(szLog);

	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->GetDatabase()->IsOpen()) {
		mlog("m_DB is not opened \n");
	}
	
	CString strSQL;
	strSQL.Format("SELECT TOP 5 * from Character");

	CODBCRecordset rs(pDBMgr->GetDatabase());

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;
		mlog("AsyncTest: Exception rs.Open \n");
		mlog(e->m_strError);
	}

	try
	{
		while(!rs.IsEOF()) {
			rs.MoveNext();
		}
	}	
	catch(CDBException* e)
	{
		bException = true;
		mlog(e->m_strError);
		mlog("AsyncTest: Exception rs.MoveNext \n");
	}

	int nCount = rs.GetRecordCount();

	sprintf(szLog, "Thread=%d , MAsyncDBJob_Test END RecordCount=%d \n", 
			GetCurrentThreadId(), nCount);
	mlog(szLog);
}

void MAsyncDBJob_GetAccountCharList::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->GetAccountCharList(m_nAID, m_CharList, &m_nCharCount))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}

void MAsyncDBJob_GetCharInfo::Run(void* pContext)
{
	_ASSERT(m_pCharInfo);
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	//int nWaitHourDiff;

	if (!pDBMgr->GetCharInfoByAID(m_nAID, m_nCharIndex, m_pCharInfo)) {
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	unsigned int nEquipedItemID[MMCIP_END];
	unsigned int nEquipedItemCIID[MMCIP_END];

	if( !pDBMgr->GetCharEquipmentInfoByAID(m_nAID, m_nCharIndex, nEquipedItemID, nEquipedItemCIID)) {
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	for(int i = 0; i < MMCIP_END; i++) {
		m_pCharInfo->m_nEquipedItemCIID[i] = nEquipedItemCIID[i];
	}

#ifdef _DELETE_CLAN
	// Ŭ���� ���Ե� ĳ�����̸� ����û�� Ŭ������ �˻縦 ����� �Ѵ�.
	if( 0 != m_pCharInfo->m_ClanInfo.m_nClanID ) 
	{
		/*
		// nWaitHourDiff�� ���� 0�̸� ���� Ŭ��.
		// 0 �̻��̸� ����û�� ������ Ŭ���̴�. 
		// ��� ��û�� Ŭ���� ���� Ŭ������ �ٲ��ֱ� ���ؼ���, 
		//  Clan���̺��� DeleteDate�� NULL�� ������ ��� �Ѵ�. - by SungE.

		if( UNDEFINE_DELETE_HOUR == nWaitHourDiff )
		{
			// ���� Ŭ��.
		}
		else if( 0 > nWaitHourDiff )
		{
			SetDeleteState( MMCDS_WAIT );
		}
		//else if( MAX_WAIT_CLAN_DELETE_HOUR < nWaitHourDiff )
		//{
		//	// Ŭ�������� DB���� ����.
		//	// ���۾��� DB�� Agent server�۾����� �ϰ� ó�����Ѵ�.
		//}
		else if( 0 <= nWaitHourDiff) 
		{
			// ���� DB�� �������� �ʰ�, ������ �Ϲ� ������ ó����.
			SetDeleteState( MMCDS_NORMAL );
			m_pCharInfo->m_ClanInfo.Clear();
		}
		*/
	}
#endif

	// ��񿡼� ������ ������ �����´�. 
	// �̰��� �����ս� ������ ���߿� �÷��̾ �ڱ� ������ ���� �Ҷ��� �����;� �ҵ�	
	m_pCharInfo->ClearItems();
	if (!pDBMgr->GetCharItemInfo(m_pCharInfo))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

#ifdef _QUEST_ITEM
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		m_pCharInfo->m_QuestItemList.Clear();
		if( !pDBMgr->GetCharQuestItemInfo(m_pCharInfo) )
		{
			mlog( "MAsyncDBJob_GetCharInfo::Run - ��񿡼� ����Ʈ ������ ����� �������µ� ��������.\n" );
			SetResult(MASYNC_RESULT_FAILED);
			return;
		}
	}
#endif

	if( !pDBMgr->GetCharBRInfoAll(m_pCharInfo->m_nCID, m_pCharInfo->GetBRInfoMap()) )
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}



void MAsyncDBJob_UpdateCharClanContPoint::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;


	if (!pDBMgr->UpdateCharClanContPoint(m_nCID, m_nCLID, m_nAddedContPoint))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}


	SetResult(MASYNC_RESULT_SUCCEED);
}



void MAsyncDBJob_GetAccountCharInfo::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->GetAccountCharInfo(m_nAID, m_nCharNum, &m_CharInfo)) {
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	unsigned int nEquipedItemID[MMCIP_END];
	unsigned int nEquipedItemCIID[MMCIP_END];

	if( !pDBMgr->GetCharEquipmentInfoByAID(m_nAID, m_nCharNum, nEquipedItemID, nEquipedItemCIID)) {
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	for(int i = 0; i < MMCIP_END; i++) {
		m_CharInfo.nEquipedItemDesc[i] = nEquipedItemID[i];
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}

void MAsyncDBJob_CreateChar::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	// ĳ���� ���� �α״� ĳ���͸� �����ϴ� SP�ȿ��� ����.
	if( !pDBMgr->CheckDuplicateCharactername(&m_nResult, m_szCharName) ) 
	{
		SetResult(MASYNC_RESULT_FAILED);
	}

	if( m_nResult == MOK ) 
	{
		if( !pDBMgr->CreateCharacter(&m_nResult, m_nAID, m_szCharName, m_nCharNum, m_nSex, m_nHair, m_nFace, m_nCostume) ) 
		{
			SetResult(MASYNC_RESULT_FAILED);
		}
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}

void MAsyncDBJob_DeleteChar::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->DeleteCharacter(m_nAID, m_nCharNum, m_szCharName))
	{
		m_nDeleteResult = MERR_CANNOT_DELETE_CHAR;
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	m_nDeleteResult = MOK;
	SetResult(MASYNC_RESULT_SUCCEED);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void MAsyncDBJob_InsertGameLog::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;
/*
	pDBMgr->InsertGameLog(m_szGameName, m_szMap, 
							m_szGameType,
							m_nRound,
							m_nMasterCID,
							m_nPlayerCount,
							m_szPlayers);
*/

	if( !pDBMgr->InsertGameLog(m_nMasterCID, m_szMap, m_szGameType, &m_nID) )
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}

/*
bool MAsyncDBJob_InsertGameLog::Input(const char* szGameName, const char* szMap, const char* szGameType,const int nRound, 
										const unsigned int nMasterCID, const int nPlayerCount, const char* szPlayers)
{
	strcpy(m_szGameName, szGameName);
	strcpy(m_szMap, szMap);
	strcpy(m_szGameType, szGameType);
	m_nRound = nRound;
	m_nMasterCID = nMasterCID;
	m_nPlayerCount = nPlayerCount;
	strcpy(m_szPlayers, szPlayers);

	return true;
}
*/
bool MAsyncDBJob_InsertGameLog::Input(const unsigned int nMasterCID, const char* szMap, const char* szGameType)
{
	m_nMasterCID = nMasterCID;

	strcpy(m_szMap, szMap);
	strcpy(m_szGameType, szGameType);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void MAsyncDBJob_CreateClan::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->CreateClan(m_szClanName, 
							m_nMasterCID, 
							m_nMember1CID, 
							m_nMember2CID, 
							m_nMember3CID, 
							m_nMember4CID, 
							&m_bDBResult, 
							&m_nNewCLID))
	{
		SetResult(MASYNC_RESULT_FAILED);

		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}

bool MAsyncDBJob_CreateClan::Input(const TCHAR* szClanName, 
									const int nMasterCID, 
									const int nMember1CID, 
									const int nMember2CID,
									const int nMember3CID, 
									const int nMember4CID,
									const MUID& uidMaster,
									const MUID& uidMember1,
									const MUID& uidMember2,
									const MUID& uidMember3,
									const MUID& uidMember4)
{
	strcpy(m_szClanName, szClanName);
	m_nMasterCID = nMasterCID;
	m_nMember1CID = nMember1CID;
	m_nMember2CID = nMember2CID;
	m_nMember3CID = nMember3CID;
	m_nMember4CID = nMember4CID;

	m_uidMaster = uidMaster;
	m_uidMember1 = uidMember1;
	m_uidMember2 = uidMember2;
	m_uidMember3 = uidMember3;
	m_uidMember4 = uidMember4;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void MAsyncDBJob_ExpelClanMember::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	// ������ ���󿡼� ���� ����
	if (!pDBMgr->ExpelClanMember(m_nCLID, m_nClanGrade, m_szTarMember, &m_nDBResult))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}

bool MAsyncDBJob_ExpelClanMember::Input(const MUID& uidAdmin, int nCLID, int nClanGrade, const char* szTarMember)
{
	m_uidAdmin = uidAdmin;
	strcpy(m_szTarMember, szTarMember);
	m_nCLID = nCLID;
	m_nClanGrade = nClanGrade;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

MAsyncDBJob_InsertQuestGameLog::~MAsyncDBJob_InsertQuestGameLog()
{
	vector< MQuestPlayerLogInfo* >::iterator it, end;
	it = m_Player.begin();
	end = m_Player.end();
	for(; it != end; ++it )
		delete (*it);
}


bool MAsyncDBJob_InsertQuestGameLog::Input( const char* pszStageName, 
										    const int nScenarioID,
										    const int nMasterCID,
											MMatchQuestGameLogInfoManager* pQGameLogInfoMgr,
											const int nTotalRewardQItemCount,
											const int nElapsedPlayerTime )
{
	if( 0 == pQGameLogInfoMgr )
		return false;

	strcpy( m_szStageName, pszStageName );

	m_nScenarioID				= nScenarioID;
	m_nMasterCID				= nMasterCID;
    m_nElapsedPlayTime			= nElapsedPlayerTime;
	m_nTotalRewardQItemCount	= nTotalRewardQItemCount;

	int										i;
	MQuestPlayerLogInfo*					pPlayer;
	MMatchQuestGameLogInfoManager::iterator itPlayer, endPlayer;
	
	m_Player.clear();
	memset( m_PlayersCID, 0, 12 );

	i = 0;
	itPlayer  = pQGameLogInfoMgr->begin();
	endPlayer = pQGameLogInfoMgr->end();
	for(; itPlayer != endPlayer; ++itPlayer )
	{
		if( nMasterCID != itPlayer->second->GetCID() )
			m_PlayersCID[ i++ ] = itPlayer->second->GetCID();

		if( itPlayer->second->GetUniqueItemList().empty() )
			continue;	// ���������� �׳� ����.
		
		if( 0 == (pPlayer = new MQuestPlayerLogInfo) )
		{
			continue;
		}

		pPlayer->SetCID( itPlayer->second->GetCID() );
		pPlayer->GetUniqueItemList().insert( itPlayer->second->GetUniqueItemList().begin(),
											 itPlayer->second->GetUniqueItemList().end() );

		m_Player.push_back( pPlayer );

		if( MAX_QUEST_LOG_PLAYER_COUNT <= i )
		{
			_ASSERT( 0 && "�ִ��ο� �ʰ�" );
			break;
		}
	}

	return true;
}


void MAsyncDBJob_InsertQuestGameLog::Run( void* pContext )
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		MMatchDBMgr* pDBMgr = reinterpret_cast< MMatchDBMgr* >( pContext );

		int nQGLID;

		// �켱 ����Ʈ ���� �α׸� ������.
		if( !pDBMgr->InsertQuestGameLog(m_szStageName, 
			m_nScenarioID,
			m_nMasterCID, m_PlayersCID[0], m_PlayersCID[1], m_PlayersCID[2],
			m_nTotalRewardQItemCount,
			m_nElapsedPlayTime,
			nQGLID) )
		{
			SetResult(MASYNC_RESULT_FAILED);
			return;
		}

		// ����ũ �����ۿ� ���� �����ʹ� QUniqueItemLog�� ���� ������ ����� ��.
		int											i;
		int											nCID;
		int											nQIID;
		int											nQUItemCount;
		QItemLogMapIter								itQUItem, endQUItem;
		vector< MQuestPlayerLogInfo* >::iterator	itPlayer, endPlayer;

		itPlayer  = m_Player.begin();
		endPlayer = m_Player.end();

		for( ; itPlayer != endPlayer; ++itPlayer )
		{
			if( (*itPlayer)->GetUniqueItemList().empty() )
				continue;	// ����ũ �������� ������ ���� ������ ����.

			nCID		= (*itPlayer)->GetCID();
			itQUItem	= (*itPlayer)->GetUniqueItemList().begin();
			endQUItem	= (*itPlayer)->GetUniqueItemList().end();

			for( ; itQUItem != endQUItem; ++itQUItem )
			{
				nQIID			= itQUItem->first;
				nQUItemCount	= itQUItem->second;

				for( i = 0; i < nQUItemCount; ++i )
				{
					if( !pDBMgr->InsertQUniqueGameLog(nQGLID, nCID, nQIID) )
					{
						mlog( "MAsyncDBJob_InsertQuestGameLog::Run - ����ũ ������ �α� ���� ����. CID:%d QIID:%d\n", 
							nCID, nQIID );

						SetResult(MASYNC_RESULT_FAILED);
					}
				}
			}

			// �۾��� ���� ������ �޸𸮿��� �����Ѵ�.
			delete (*itPlayer);
		}
	}

	m_Player.clear();
	
	SetResult(MASYNC_RESULT_SUCCEED);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

MAsyncDBJob_UpdateQuestItemInfo::~MAsyncDBJob_UpdateQuestItemInfo()
{
	m_QuestItemList.Clear();
}


bool MAsyncDBJob_UpdateQuestItemInfo::Input( const int nCID, MQuestItemMap& QuestItemList, MQuestMonsterBible& QuestMonster )
{
	m_nCID = nCID;

	MQuestItemMap::iterator it, end;

	m_QuestItemList.Clear();

	it  = QuestItemList.begin();
	end = QuestItemList.end();

	for( ; it != end; ++it )
	{
		MQuestItem* pQuestItem = it->second;
		m_QuestItemList.CreateQuestItem( pQuestItem->GetItemID(), pQuestItem->GetCount(), pQuestItem->IsKnown() );
	}

	m_QuestItemList.SetDBAccess( QuestItemList.IsDoneDbAccess() );
	m_QuestMonster.Clear();
	return true;
}


void MAsyncDBJob_UpdateQuestItemInfo::Run( void* pContext )
{
	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		if( m_QuestItemList.IsDoneDbAccess() )
		{
			MMatchDBMgr* pDBMgr = reinterpret_cast< MMatchDBMgr* >( pContext );
			if( !pDBMgr->UpdateQuestItem(m_nCID, m_QuestItemList, m_QuestMonster) )
			{
				SetResult( MASYNC_RESULT_FAILED );
				return;
			}
		}
	}

	SetResult( MASYNC_RESULT_SUCCEED );
}


bool MAsyncDBJob_SetBlockHacking::Input( const DWORD dwAID
										, const DWORD dwCID
										, const BYTE btBlockType
										, const BYTE btBlockLevel
										, const string& strComment
										, const string& strIP
										, const string& strEndDate
										, const BYTE nServerID
										, const string& strChannelName )
{
	m_dwAID				= dwAID;
	m_dwCID				= dwCID;
	m_btBlockType 		= btBlockType;
	m_btBlockLevel		= btBlockLevel;
	m_strComment		= strComment;
	m_strIP				= strIP;
	m_strEndDate		= strEndDate;
	m_nServerID			= nServerID;
	m_strChannelName	= strChannelName;

	return true;
}

void MAsyncDBJob_SetBlockHacking::Run( void* pContext )
{
	MMatchDBMgr* pDBMgr = reinterpret_cast< MMatchDBMgr* >( pContext );

	if( !pDBMgr->SetBlockHacking(
		m_dwAID
		, m_dwCID
		, m_btBlockType
		, m_strIP
		, m_strEndDate
		, m_nServerID
		, m_strChannelName
		, m_strComment
		, m_btBlockLevel) )
	{
		SetResult( MASYNC_RESULT_FAILED );
		return;
	}


	//if( MMBL_ACCOUNT == m_btBlockLevel )
	//{
	//	if( !pDBMgr->SetBlockAccount( m_dwAID, m_dwCID, m_btBlockType, m_strComment, m_strIP, m_strEndDate) )
	//	{
	//		SetResult( MASYNC_RESULT_FAILED );
	//		return;
	//	}
	//}
	//else if( MMBL_LOGONLY == m_btBlockLevel )
	//{
	//	if( pDBMgr->InsertBlockLog(m_dwAID, m_dwCID, m_btBlockType, m_strComment, m_strIP) )
	//	{
	//		SetResult( MASYNC_RESULT_FAILED );
	//		return;
	//	}
	//}

	SetResult( MASYNC_RESULT_SUCCEED );
}


bool MAsyncDBJob_ResetAccountHackingBlock::Input( const DWORD dwAID, const BYTE btBlockType )
{
	m_dwAID			= dwAID;
	m_btBlockType	= btBlockType;

	return true;
}


void MAsyncDBJob_ResetAccountHackingBlock::Run( void* pContext )
{
	MMatchDBMgr* pDBMgr = reinterpret_cast< MMatchDBMgr* >( pContext );

	if( !pDBMgr->ResetAccountHackingBlock(m_dwAID, m_btBlockType) )
	{
		SetResult( MASYNC_RESULT_FAILED );
		return;
	}

	SetResult( MASYNC_RESULT_SUCCEED );
}