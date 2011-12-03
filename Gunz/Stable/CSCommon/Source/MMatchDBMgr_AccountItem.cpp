#include "stdafx.h"
#include "MMatchDBMgr.h"

///////////////////////////////////////////////////////////////////////////////
// SQL Query ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TCHAR g_szDB_SELECT_ACCOUNT_ITEM[] = _T("{CALL spSelectAccountItem (%d)}");				
// Desc  : ������ �߾����� ������ ���� ��������
// Param :	@AID	INT

TCHAR g_szDB_DELETE_EXPIRED_ACCOUNT_ITEM[]		= _T("{CALL spDeleteExpiredAccountItem (%d, %d)}");
// Desc  : ����� �߾����� ������ ����
// Param :	@AID	INT
//			@AIID	INT

TCHAR g_szDB_BRING_ACCOUNTITEM[] = _T("{CALL spBringAccountItem (%d, %d, %d)}");
// Desc  : �߾����� ������ �� ĳ���ͷ� ��������
// Param :	@AID	INT
//			@CID	INT
//			@AIID	INT

TCHAR g_szDB_BRING_ACCOUNTITEM_STACKABLE[] = _T("{CALL spBringAccountItemStackable (%d, %d, %d, %d, %d)}");
// Desc  : �߾����� ������ �� ĳ���ͷ� ��������
// Param :	@AID	INT
//			@CID	INT
//			@AIID	INT
//			@CIID	INT
//			@Count	INT

TCHAR g_szDB_BRING_BACK_ACCOUNTITEM[] = _T("{CALL spBringBackAccountItem (%d, %d, %d)}");
// Desc  : �߾����࿡ �� ĳ�� �Ϲ� ������ �ֱ�
// Param :	@AID		INT
//			@CID		INT
//			@CIID		INT

TCHAR g_szDB_BRING_BACK_ACCOUNTITEM_STACKABLE[] = _T("{CALL spBringBackAccountItemStackable (%d, %d, %d, %d)}");
// Desc  : �߾����࿡ �� ĳ�� �Ϲ� ������ �ֱ�
// Param :	@AID		INT
//			@CID		INT
//			@CIID		INT
//			@ItemCnt	INT	

bool MMatchDBMgr::GetAccountItemInfo(const int nAID, MAccountItemNode* pOut, int* poutNodeCount, int nMaxNodeCount,
									 MAccountItemNode* pOutExpiredItemList, int* poutExpiredItemCount, int nMaxExpiredItemCount)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;


	CString strSQL;
	strSQL.Format(g_szDB_SELECT_ACCOUNT_ITEM, nAID);
	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);

		// db���̺귯���� ����ϴ� ��� �κп��� ���ܰ� �߻��� �� �ִ�.
		if (rs.IsOpen() == FALSE) return false;

		int nodecount = 0;
		int nExpiredItemCount=0;		// �Ⱓ����� ������ ����

		int		nItemDescID					= 0;
		DWORD	dwAIID						= 0;
		WORD	wMaxUsableHour				= RENT_PERIOD_UNLIMITED;
		bool	bIsRentItem					= false;
		int		nRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
		int		nCnt						= -1;

		for( ; ! rs.IsEOF(); rs.MoveNext() ) {
			nItemDescID = (int) rs.Field("ItemID").AsInt();
			dwAIID		= (int) rs.Field("AIID").AsInt();

			// �� �����۵��� ��ŷ���� �̾Ƴ��߸��Ⱦ�����
			if( 300010 < nItemDescID && 300040 > nItemDescID ) { continue; }

			if( rs.Field("Cnt").IsNull() ) {
				nCnt = 1;
			} else {
				nCnt = rs.Field("Cnt").AsInt();
			}			

			if( rs.Field("RentHourPeriod").IsNull() ) {	
				wMaxUsableHour = RENT_PERIOD_UNLIMITED;							///< NULL�̸� ������ ���������� ���.
			} else {
				wMaxUsableHour = (WORD)rs.Field("RentHourPeriod").AsShort();	///< ������ �������� RENT_PERIOD_UNLIMITED���� ���� �ȴ�. - by SungE 2007-06-14
			}

			// �Ⱓ�� ������ ����. �ִ� ��� �ð��� 0�̸� ������ �������̴�. - by SungE 2007-06-14
			if( RENT_PERIOD_UNLIMITED == wMaxUsableHour ) {
				bIsRentItem = false;
				nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED; // �Ⱓ�������� �����ð�(��)�� ���������� ����.
			} else {
				// ���� �Ⱓ�� NULL�̾ ���������� �Ǵ��Ѵ�.
				if( rs.Field("RentPeriodRemainder").IsNull() ) {
					nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED;
					bIsRentItem = false;
				} else {
					nRentMinutePeriodRemainder = (int)rs.Field("RentPeriodRemainder").AsInt();
					bIsRentItem = true;
				}
			}

			MMatchItemDesc *pItemDesc;
			if( (pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemDescID)) != NULL )	//< Normal Item
			{							
				if( bIsRentItem && pItemDesc->IsSpendableItem() ) {
					mlog( "Renewal - MMatchDBMgr::GetAccountItemInfo - AID(%u), AIID(%u), ItemID(%u), ItemCount(%d), RentHourPeriod(%d)\n", 
						nAID, dwAIID, nItemDescID, nCnt, nRentMinutePeriodRemainder );
					_ASSERT( 0 );


					bIsRentItem = false;
					nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED;
				}
			} 
			else if( NULL != MGetMatchServer()->GetGambleMachine().GetGambleItemByGambleItemID(nItemDescID) )	//< Gamble Item
			{	
				if( bIsRentItem ) {					
					mlog( "Renewal - MMatchDBMgr::GetAccountItemInfo - AID(%u), AIID(%u), ItemID(%u), ItemCount(%d), RentHourPeriod(%d)\n", 
						nAID, dwAIID, nItemDescID, nCnt, nRentMinutePeriodRemainder );					
					ASSERT( 0 );

					bIsRentItem = false;
					nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED;
				}				
			} else { ASSERT( 0 ); }


			if ((bIsRentItem) && (nRentMinutePeriodRemainder < 0)) {
				if (nExpiredItemCount < nMaxExpiredItemCount) {					
					pOutExpiredItemList[nExpiredItemCount].nAIID	= dwAIID;
					pOutExpiredItemList[nExpiredItemCount].nItemID  = (unsigned long int)nItemDescID; 
					nExpiredItemCount++;
				}
			} else {
				pOut[nodecount].nAIID						= dwAIID;
				pOut[nodecount].nItemID						= (unsigned long int)nItemDescID;
				pOut[nodecount].nRentMinutePeriodRemainder  = nRentMinutePeriodRemainder;
				pOut[nodecount].nCount						= rs.Field("Cnt").AsInt();

				nodecount++;
				if (nodecount >= nMaxNodeCount) break;
			}
		}

		*poutNodeCount = nodecount;
		*poutExpiredItemCount = nExpiredItemCount;
	} 
	catch(CDBException* e) {
		bException = true;
		Log("MMatchDBMgr::GetAccountItemInfo - %s\n", e->m_strError);
		return false;
	}

	_STATUS_DB_END(25);
	return true;
}

bool MMatchDBMgr::DeleteExpiredAccountItem(const int nAID, const int nAIID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_DELETE_EXPIRED_ACCOUNT_ITEM, nAID, nAIID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(49);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::DeleteExpiredAccountItem - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::DeleteExpiredAccountItem - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}

bool MMatchDBMgr::BringAccountItem(const int nAID, const int nCID, const int nAIID, unsigned long int* poutCIID, 
								   unsigned long int* poutItemID, bool* poutIsRentItem, int* poutRentMinutePeriodRemainder, WORD& wRentHourPeriod )
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_BRING_ACCOUNTITEM, nAID, nCID, nAIID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) { return false; }

	_STATUS_DB_END(26);

	int nRet = rs.Field("Ret").AsInt();	
	if( nRet < 0 ) {
		*poutCIID = 0;
		return false;
	}

	wRentHourPeriod = (WORD)rs.Field("RentHourPeriod").AsShort();

	*poutCIID = rs.Field("ORDERCIID").AsLong();
	*poutItemID = rs.Field("ItemID").AsLong();	

	if( RENT_PERIOD_UNLIMITED != wRentHourPeriod ) 
	{	///< �Ⱓ�� ������ ����. �ִ� ��� �ð��� 0�̸� ������ �������̴�. - by SungE 2007-06-14		
		*poutIsRentItem = true;
		*poutRentMinutePeriodRemainder = (int)rs.Field("RentPeriodRemainder").AsInt();
	}
	else
	{
		*poutIsRentItem					= false;
		*poutRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
	}

	return true;
}

bool MMatchDBMgr::BringAccountItemStackable(const int nAID, const int nCID, const int nAIID, const int nCIID, const int nCount, unsigned long int* poutCIID, 
							   unsigned long int* poutItemID, bool* poutIsRentItem, int* poutRentMinutePeriodRemainder, unsigned int* poutCount, WORD& wRentHourPeriod )
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_BRING_ACCOUNTITEM_STACKABLE, nAID, nCID, nAIID, nCIID, nCount);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) { return false; }

	_STATUS_DB_END(26);

	int nRet = rs.Field("Ret").AsInt();	
	if( nRet < 0 ) {
		*poutCIID = 0;
		return false;
	}

	wRentHourPeriod = (WORD)rs.Field("RentHourPeriod").AsShort();

	*poutCIID	= rs.Field("ORDERCIID").AsLong();
	*poutItemID = rs.Field("ItemID").AsLong();
	*poutCount	= rs.Field("Cnt").AsInt();

	if( RENT_PERIOD_UNLIMITED != wRentHourPeriod ) 
	{	///< �Ⱓ�� ������ ����. �ִ� ��� �ð��� 0�̸� ������ �������̴�. - by SungE 2007-06-14		
		*poutIsRentItem = true;
		*poutRentMinutePeriodRemainder = (int)rs.Field("RentPeriodRemainder").AsInt();
	}
	else
	{
		*poutIsRentItem					= false;
		*poutRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
	}

	return true;
}

bool MMatchDBMgr::BringBackAccountItem(const int nAID, const int nCID, const int nCIID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_BRING_BACK_ACCOUNTITEM, nAID, nCID, nCIID);		
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) { return false; }

	int nRet = rs.Field("Ret").AsInt();
	_STATUS_DB_END(32);

	if( nRet != 0 ) {
		Log("MMatchDBMgr::BringBackAccountItem - FAILED(%d), AID(%d), CID(%d), CIID(%d)\n", nRet, nAID, nCID, nCIID);
		Log("MMatchDBMgr::BringBackAccountItem - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}

bool MMatchDBMgr::BringBackAccountItemStackable(const int nAID, const int nCID, const int nCIID, const int nItemCnt)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_BRING_BACK_ACCOUNTITEM_STACKABLE, nAID, nCID, nCIID, nItemCnt);		
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) { return false; }

	int nRet = rs.Field("Ret").AsInt();
	_STATUS_DB_END(32);

	if( nRet != 0 ) {
		Log("MMatchDBMgr::BringBackAccountItemStackable - FAILED(%d), AID(%d), CID(%d), Cnt(%d)\n", nRet, nAID, nCID, nItemCnt);
		Log("MMatchDBMgr::BringBackAccountItemStackable - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}
