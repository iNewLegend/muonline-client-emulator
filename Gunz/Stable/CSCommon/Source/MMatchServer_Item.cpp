#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchItem.h"
#include "MAgentObject.h"
#include "MMatchNotify.h"
#include "Msg.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchTransDataType.h"
#include "MMatchFormula.h"
#include "MMatchConfig.h"
#include "MCommandCommunicator.h"
#include "MMatchShop.h"
#include "MMatchTransDataType.h"
#include "MDebug.h"	
#include "MMatchAuth.h"
#include "MMatchStatus.h"
#include "MAsyncDBJob.h"
#include "MAsyncDBJob_GetAccountItemList.h"


// �÷��̾�� ������ ����
bool MMatchServer::DistributeZItem(const MUID& uidPlayer, const unsigned long int nItemID, bool bRentItem, int nRentPeriodHour, int nItemCount)
{
	MMatchObject* pObject = GetObject(uidPlayer);
	if (!IsEnabledObject(pObject)) return false;

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if ( pItemDesc == NULL ) return false;

	if ( !pItemDesc->IsSpendableItem() ) 
	{
		unsigned long int nNewCIID = 0;
		if (!m_MatchDBMgr.InsertDistributeItem(pObject->GetCharInfo()->m_nCID, nItemID, bRentItem, nRentPeriodHour, &nNewCIID)) return false;

		// ������Ʈ�� ������ �߰�
		int nRentMinutePeriodRemainder = nRentPeriodHour * 60;

		MUID uidNew = MMatchItemMap::UseUID();
		pObject->GetCharInfo()->m_ItemList.CreateItem(uidNew, nNewCIID, nItemID, bRentItem, nRentMinutePeriodRemainder, nRentPeriodHour);
	}
	else 
	{
		// TodoH(��)
		// �ϴ� �Ҹ� �������� ������ ���� �������� �ʴ´�.

		return false;
	}

	return true;
}


// ���� ���� ������Ʈ���� �������� ����
bool MMatchServer::RemoveExpiredCharItem(MMatchObject* pObject, MUID& uidItem)
{
	MMatchItem* pItem = pObject->GetCharInfo()->m_ItemList.GetItem(uidItem);
	if (pItem == NULL) return false;

	// ��񿡼� ������ ����
	if (!m_MatchDBMgr.DeleteExpiredCharItem(pObject->GetCharInfo()->m_nCID, pItem->GetCIID())) {
		return false;
	}

	// ���� ������̸� ��ü
	MMatchCharItemParts nCheckParts = MMCIP_END;
	if (pObject->GetCharInfo()->m_EquipedItem.IsEquipedItem(uidItem, nCheckParts)) {
		pObject->GetCharInfo()->m_EquipedItem.Remove(nCheckParts);
	}

	// ������Ʈ���� ������ ����
	pObject->GetCharInfo()->m_ItemList.RemoveItem(uidItem);

	return true;
}


void MMatchServer::OnRequestAccountItemList(const MUID& uidPlayer)
{
	ResponseAccountItemList(uidPlayer);
}
void MMatchServer::ResponseAccountItemList(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL) || (pObj->GetAccountInfo() == NULL)) return;

	MAsyncDBJob_GetAccountItemList* pGetAccountItemListJob = new MAsyncDBJob_GetAccountItemList(uidPlayer);
	if( NULL == pGetAccountItemListJob ) return;

	pGetAccountItemListJob->Input( uidPlayer, pObj->GetAccountInfo()->m_nAID );
	pObj->m_DBJobQ.DBJobQ.push_back( pGetAccountItemListJob );
}


//void MMatchServer::ResponseTakeoffItem(const MUID& uidPlayer, const MMatchCharItemParts parts)
//{
//	MMatchObject* pObj = GetObject(uidPlayer);
//	if (pObj == NULL) return;
//	int nResult = MOK;
//
//	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
//	if (pCharInfo == NULL) return;
//	if (pCharInfo->m_EquipedItem.IsEmpty(parts)) return;
//
//
//	
//	MMatchItem* pItem = pCharInfo->m_EquipedItem.GetItem(parts);
//	if( NULL == pItem )
//	{
//		return;
//	}
//
//	MMatchItemDesc* pItemDesc = pItem->GetDesc();
//	if (pItemDesc == NULL) 
//	{
//		return;
//	}
//
//	// ���� üũ
//	int nWeight=0, nMaxWeight=0;
//	pCharInfo->m_EquipedItem.GetTotalWeight(&nWeight, &nMaxWeight);
//	nMaxWeight = pCharInfo->m_nMaxWeight + nMaxWeight - pItemDesc->m_nMaxWT;
//	nWeight -= pItemDesc->m_nWeight;
//
//	if (nWeight > nMaxWeight) nResult = MERR_CANNOT_TAKEOFF_ITEM_BY_WEIGHT;
//
//	if( MOK == nResult )
//	{
//		bool bRet = false;
//		if (!m_MatchDBMgr.UpdateEquipedItem(pCharInfo->m_nCID, parts, 0, 0, &bRet))
//		{
//			mlog("DB Query(ResponseEquipItem > UpdateEquipedItem) Failed\n");
//			nResult = MERR_CANNOT_TAKEOFF_ITEM;
//		}
//
//		if( bRet )
//		{
//			pCharInfo->m_EquipedItem.Remove( parts );
//		}
//		else
//		{
//			nResult = MERR_CANNOT_TAKEOFF_ITEM;
//		}
//	}
//
//	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_TAKEOFF_ITEM, MUID(0,0));
//	pNew->AddParameter(new MCommandParameterInt(nResult));
//	RouteToListener(pObj, pNew);	
//
//	if( MOK == nResult )
//	{
//		ResponseCharacterItemList( uidPlayer );
//
//		// ���� �������� ���̸� ���������� �˷��� ������ Look�� ������Ʈ ����� �Ѵ�.
//		if( FindStage(pObj->GetStageUID())  )
//		{
//			MCommand* pEquipInfo = CreateCommand( MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, MUID(0, 0) );
//			pEquipInfo->AddParameter( new MCmdParamUID(uidPlayer) );
//			pEquipInfo->AddParameter( new MCmdParamInt(parts) );
//			pEquipInfo->AddParameter( new MCmdParamInt(0) );
//			RouteToStage( pObj->GetStageUID(), pEquipInfo );
//		}
//	}
//}
//
//
//bool MMatchServer::ResponseCharacterItemList(const MUID& uidPlayer)
//{
//	MMatchObject* pObj = GetObject(uidPlayer);
//	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) 
//	{
//		mlog("ResponseCharacterItemList > pObj or pObj->GetCharInfo() IS NULL\n");
//		return false;
//	}
//
//	// ���� DB�� ������Ʈ �Ǿ��ٸ� ������ �ٽ� �����Ѵ�.
//	// �뷮�� ũ���� ���� �Ͼ�� ���� �ƴϴ�. - by SungE 2007-06-28
//	if( SendGambleItemList(pObj) )
//	{
//		ResponseShopItemList( uidPlayer, 0, 0 );
//	}
//
//	// ������ ��� �＼���� ���߾����� ��񿡼� ������ ������ �����´�
//	if (!pObj->GetCharInfo()->m_ItemList.IsDoneDbAccess())
//	{
//		if (!m_MatchDBMgr.GetCharItemInfo(pObj->GetCharInfo()))
//		{
//			mlog("DB Query(ResponseCharacterItemList > GetCharItemInfo) Failed\n");
//			return false;
//		}
//	}
//
//	if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
//	{
//		if( !pObj->GetCharInfo()->m_QuestItemList.IsDoneDbAccess() )
//		{
//			if( !m_MatchDBMgr.GetCharQuestItemInfo(pObj->GetCharInfo()) )
//			{
//				mlog( "ResponseCharacterItemList - GetCharQuestItemInfo fail.\n" );
//				return false;
//			}
//		}
//	}
//
//	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_CHARACTER_ITEMLIST, uidPlayer);
//
//	// �ٿ�Ƽ ����
//	pNew->AddParameter(new MCommandParameterInt(pObj->GetCharInfo()->m_nBP));
//
//	// ����� ������ ����
//	int nRealEquipedItemCount = 0;
//	int nIndex = 0;
//	void* pEquipItemArray = MMakeBlobArray(sizeof(MUID), MMCIP_END);
//	for (int i = 0; i < MMCIP_END; i++)
//	{
//		MUID* pUID = (MUID*)MGetBlobArrayElement(pEquipItemArray, nIndex++);
//
//		if (!pObj->GetCharInfo()->m_EquipedItem.IsEmpty(MMatchCharItemParts(i)))
//		{
//			*pUID = pObj->GetCharInfo()->m_EquipedItem.GetItem(MMatchCharItemParts(i))->GetUID();
//			nRealEquipedItemCount++;
//		}
//		else
//		{
//			*pUID = MUID(0,0);
//		}
//	}
//
//	pNew->AddParameter(new MCommandParameterBlob(pEquipItemArray, MGetBlobArraySize(pEquipItemArray)));
//	MEraseBlobArray(pEquipItemArray);
//
//
//	// ���� �ִ� ������ ����Ʈ ����
//	int nItemCount = pObj->GetCharInfo()->m_ItemList.GetCount();
//
//	void*			pItemArray					= MMakeBlobArray(sizeof(MTD_ItemNode), nItemCount);
//	MMatchItemMap*	pItemList					= &pObj->GetCharInfo()->m_ItemList;
//	MMatchItem*		pItem						= NULL;
//	MTD_ItemNode*	pItemNode					= NULL;
//	unsigned long	nPassTime					= 0;
//	int				nPassMinuteTime				= 0;
//	int				iMaxUseHour					= 0;
//	int				nRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
//
//	nIndex = 0;
//	for (MMatchItemMap::iterator itor = pItemList->begin(); itor != pItemList->end(); ++itor)
//	{
//		pItem = (*itor).second;
//
//		pItemNode					= (MTD_ItemNode*)MGetBlobArrayElement(pItemArray, nIndex++);
//		nPassTime					= MGetTimeDistance(pItem->GetRentItemRegTime(), GetTickTime());
//		nPassMinuteTime				= nPassTime / (1000 * 60);
//		iMaxUseHour					= 0;		// �ִ� ���ð� ������(�ǸŰ��� ����� ����)
//		nRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
//
//		if (pItem->IsRentItem())
//		{
//			nRentMinutePeriodRemainder	= pItem->GetRentMinutePeriodRemainder() - nPassMinuteTime;
//			iMaxUseHour					= pItem->GetRentHourPeriod();
//		}
//		
//		Make_MTDItemNode(pItemNode
//			, pItem->GetUID()
//			, pItem->GetDescID()
//			, nRentMinutePeriodRemainder
//			, iMaxUseHour);	
//
//		//pItemNode->uidItem						= pItem->GetUID();
//		//pItemNode->nItemID						= pItem->GetDescID();
//		//pItemNode->nRentMinutePeriodRemainder	= nRentMinutePeriodRemainder;		// �ʴ���
//		//pItemNode->iMaxUseHour					= iMaxUseHour;						// �ִ� ���ð� �߰�
//	}
//
//
//	pNew->AddParameter(new MCommandParameterBlob(pItemArray, MGetBlobArraySize(pItemArray)));
//	MEraseBlobArray(pItemArray);
//
//	// ������ �ִ� �׺� ������ ����Ʈ ����
//	int nGambleItemCount = pObj->GetCharInfo()->m_GambleItemManager.GetCount();
//	void* pGambleItemArray = MMakeBlobArray(sizeof(MTD_GambleItemNode), nGambleItemCount);
//
//	for (int j = 0; j < nGambleItemCount; ++j)
//	{
//		const MMatchCharGambleItem* pGambleItem = pObj->GetCharInfo()->m_GambleItemManager.GetGambleItem(j);
//		MTD_GambleItemNode* pSendGambleItem = (MTD_GambleItemNode*)MGetBlobArrayElement(pGambleItemArray, j);
//		if( pSendGambleItem != NULL )
//		{
//			pSendGambleItem->uidItem = pGambleItem->GetUID();
//			pSendGambleItem->nItemID = pGambleItem->GetGambleItemID();			
//		}
//		else
//		{
//			ASSERT( 0 );
//		}
//	}
//
//	pNew->AddParameter(new MCommandParameterBlob(pGambleItemArray, MGetBlobArraySize(pGambleItemArray)));
//	MEraseBlobArray(pGambleItemArray);
//
//	PostSafeQueue( pNew );
//
//	return true;
//}

void MMatchServer::OnRequestUseSpendableNormalItem(const MUID& uidPlayer, const MUID& uidItem)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	MMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);	///< UID�� ������ ã��
	if( pItem == NULL ) {
		mlog("Use Spendable Item Failed[CID : %d, MUID(%d%d)]\n", pCharInfo->m_nCID, uidItem.High, uidItem.Low);
		return;
	}

	if( pItem->GetDesc()->IsSpendableItem() ) 
	{ 
		UseSpendableItem(uidPlayer, uidItem);	
	}	
}

void MMatchServer::UseSpendableItem(const MUID& uidPlayer, const MUID& uidItem)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	MMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);	///< UID�� ������ ã��
	if( pItem == NULL ) {
		mlog("Use Spendable Item Failed[CID : %d, MUID(%d%d)]\n", pCharInfo->m_nCID, uidItem.High, uidItem.Low);
		return;
	}

	if( pItem->GetItemCount() > 0 ) 
	{
		pItem->DecreaseCountWithCaching(1);

		/*
		if( pItem->GetItemCount() == 0 ) 
		{
			UpdateCharItemDBCachingData(pObj);
			
			MMatchCharItemParts nCheckParts = MMCIP_END;
			if (pCharInfo->m_EquipedItem.IsEquipedItem(uidItem, nCheckParts)) {
				pCharInfo->m_EquipedItem.Remove(nCheckParts);
			}

			if (!m_MatchDBMgr.UpdateEquipedItem(pCharInfo->m_nCID, parts, 0, 0))
			{
				mlog("DB Query(ResponseEquipItem > UpdateEquipedItem) Failed\n");
				// nResult = MERR_CANNOT_TAKEOFF_ITEM;
			}
			ResponseTakeoffItem

			pCharInfo->m_ItemList.RemoveItem(uidItem);

			ResponseCharacterItemList(uidPlayer);
		}
		*/

#ifdef _DEBUG
		mlog("Item UID(%d%d)�� ���Ǿ����ϴ�. ������ �׸� ����\n", uidItem.High, uidItem.Low);
#endif
	} else {
		mlog("Item UID(%d%d)�� ������������ ���Ǿ����ϴ�.\n", uidItem.High, uidItem.Low);
	}
}

/*
void MMatchServer::OnRequestUseSpendableBuffItem(const MUID& uidPlayer, const MUID& uidItem)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	MMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);	///< UID�� ������ ã��
	if( pItem == NULL ) {
		mlog("Use Spendable Item Failed[CID : %d, MUID(%d%d)]\n", pCharInfo->m_nCID, uidItem.High, uidItem.Low);
		return;
	}

	if( pItem->GetDesc()->IsSpendableItem() == false ) {
		_ASSERT(0); 
		return; 
	} 

	MMatchBuffDesc* pBuffDesc = MGetMatchBuffDescMgr()->GetBuffDesc(pItem->GetDesc()->m_nID);
	if( pBuffDesc == NULL ) {
		_ASSERT(0);
		return;
	}

	int nBuffID		= pBuffDesc->m_nBuffID;
	int nBuffPeriod	= pBuffDesc->m_nBuffPeriod.Ref();

	bool bResult = false;
	if( pBuffDesc->m_nBuffPeriodType.Ref() == MMBPT_LONG ) {
		_ASSERT(0);
		return;
	} else if( pBuffDesc->m_nBuffPeriodType.Ref() == MMBPT_SHORT ) {
		bResult = pObj->GetCharBuff()->ApplyShortBuffInfo(nBuffID, nBuffPeriod);
	} else {
		_ASSERT(0);
		return;
	}

	if( bResult ) 
	{
		UseSpendableItem(uidPlayer, uidItem);

		MCommand *pCmd = CreateCommand( MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM, uidPlayer );
		pCmd->AddParameter(new MCommandParameterUID(uidItem));
		pCmd->AddParameter(new MCommandParameterInt(MOK));
		PostSafeQueue(pCmd);

		PostCmdCharacterBuffInfo(uidPlayer);
	} 
	else 
	{
		MCommand *pCmd = CreateCommand( MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM, uidPlayer );
		pCmd->AddParameter(new MCommandParameterUID(uidItem));
		pCmd->AddParameter(new MCommandParameterInt(MERR_CANNOT_APPLY_BUFF_INFO));
		PostSafeQueue(pCmd);
	}	
}*/
//���������ӽ��ּ� 
/*
void MMatchServer::PostCmdCharacterBuffInfo(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;		

	MCommand *pCmd = CreateCommand(MC_MATCH_SPENDABLE_BUFF_ITEM_STATUS, uidPlayer);	

	void* pCharBuffInfoArray = MMakeBlobArray(sizeof(MTD_CharBuffInfo), 1);
	MTD_CharBuffInfo* pMTD_CharBuffInfo = (MTD_CharBuffInfo*)MGetBlobArrayElement(pCharBuffInfoArray, 0);
	CopyCharBuffInfoForTrans(pMTD_CharBuffInfo, pCharInfo, pObj);

	pCmd->AddParameter(new MCommandParameterUID(pObj->GetUID()));
	pCmd->AddParameter(new MCommandParameterBlob(pCharBuffInfoArray, MGetBlobArraySize(pCharBuffInfoArray)));
	Post(pCmd);

	MEraseBlobArray(pCharBuffInfoArray);
}*/