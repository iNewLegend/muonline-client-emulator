#include "stdafx.h"
//
//#include "ZEquipmentListBox.h"
//#include "ZPost.h"
//#include "ZApplication.h"
//#include "ZGameClient.h"
//#include "ZItemSlotView.h"
//#include "ZShopEx.h"
//#include "ZCharacterView.h"
//#include "MTextArea.h"
//#include "ZMyInfo.h"
//#include "ZMyItemList.h"
//#include "MComboBox.h"
//#include "ZItemMenu.h"
//#include "ZShopEquipInterface.h"
//
//
//bool ZGetIsCashItem( unsigned long nItemID)
//{
//	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
//	if (pItemDesc == NULL)
//	{
//		// Gamble ������
//		MMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(nItemID);
//		if( pGItemDesc ) {
//			if( pGItemDesc->IsCash() ) {
//				return true;
//			}
//		}
//
//		return false;
//	}
//	if (pItemDesc->IsCashItem())
//		return true;
//	return false;
//}
//
//
//
//// ���� �԰� �ִ� ������ �� ���������� �ٲ� ���� �� ����� �ɷ�ġ ��ȭ�� ǥ���Ѵ�
//// pNewItemDesc=NULL �� �ϸ� �⺻ �ɷ�ġ�� �ʱ�ȭ�Ѵ�
//
//bool ZEquipmentListBox_OLD::IsDropable(MWidget* pSender)
//{
//	if (pSender == NULL) return false;
//	if (strcmp(pSender->GetClassName(), MINT_ITEMSLOTVIEW)) return false;
//
//	return true;
//}
//
////bool ZEquipmentListBox_OLD::OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
////{
////	if (m_pOnDropFunc != NULL)
////	{
////		m_pOnDropFunc(this, pSender, pBitmap, szString, szItemString);
////	}
////
////	return true;
////}
//
//#define SHOW_DESCRIPTION		"showdesc"
//#define HIDE_DESCRIPTION		"hidedesc"
//
//// �ϵ��ڵ������� �Լ�.. shop�̳� equipment�Ŀ� ���� �˸��� ���������� �����̸��� ����
//char* ZEquipmentListBox_OLD::GetItemDescriptionWidgetName()
//{
//	if (GetParent() && 0==strcmp("Shop", GetParent()->m_szIDLName))
//		return "Shop_ItemDescription";
//
//	if (GetParent() && 0==strcmp("Equipment", GetParent()->m_szIDLName))
//		return "Equip_ItemDescription";
//
//	_ASSERT(0);
//	return "_noExistWidgetName_";
//}
//
//bool ZEquipmentListBox_OLD::OnEvent(MEvent* pEvent, MListener* pListener)
//{
//	MRECT rtClient = GetClientRect();
//
//	if(pEvent->nMessage==MWM_MOUSEMOVE)
//	{
//		SetupItemDescTooltip();
//		//return true;			  // �޽����� �Ծ������ ������ �ȳ��´�
//	}
//	else if(pEvent->nMessage==MWM_RBUTTONDOWN) {
//		// ���� ���ǰ� ���� ���� �ڵ��ΰ� �����ϴ�. �ּ�ó����
//		/*if(rtClient.InPoint(pEvent->Pos)==true) {
//			int nSelItem = FindItem(pEvent->Pos);
//			if ( (nSelItem != -1) && GetItemMenu())
//			{
//				SetSelIndex(nSelItem);
//
//				ZEquipmentListItem_OLD* pNode = (ZEquipmentListItem_OLD*)Get(nSelItem);
//				if (ZGetIsCashItem(pNode->GetItemID())) {
//					ZItemMenu* pMenu = GetItemMenu();
//					pMenu->SetupMenu();
//					pMenu->SetTargetName(pNode->GetString());
//					pMenu->SetTargetUID(pNode->GetUID());
//
//					// ������ ���� ����
//					if (m_pDescFrame && m_pDescFrame->IsVisible())
//						m_pDescFrame->Show(false);
//
//					// �˾��޴� ����
//					MPOINT posItem;
//					GetItemPos(&posItem, nSelItem);
//					MPOINT posMenu;
//					posMenu.x = GetRect().w/4;
//					posMenu.y = posItem.y + GetItemHeight()/4;
//					pMenu->Show(posMenu.x, posMenu.y, true);
//
//					return true;
//				}
//			}
//		}*/
//	}
//
//	return MMultiColListBox::OnEvent(pEvent, pListener);
//}
//
//void ZEquipmentListBox_OLD::SetupItemDescTooltip()
//{
//	const char* szTextAreaName = GetItemDescriptionWidgetName();
//	MTextArea* pItemDescTextArea = (MTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget(szTextAreaName);
//	if (pItemDescTextArea)
//	{
//		MPOINT ptInList = MScreenToClient(this, MEvent::LatestPos);
//		int idxItem = FindItem(ptInList);
//		if (idxItem!=-1)
//		{
//			if (m_idxItemLastTooltip==idxItem) return;
//
//			m_idxItemLastTooltip = idxItem;
//			ZEquipmentListItem_OLD* pItem = (ZEquipmentListItem_OLD*)Get(idxItem);
//			MRECT rcListBox = GetRect();
//			MRECT rcItem;
//			if (pItem && CalcItemRect(idxItem, rcItem))	// �׸��� ǥ�õǰ� �ִ� ������ �˾Ƴ� (����Ʈ ��ǥ��)
//			{
//				if (pItem->IsPtInRectToShowToolTip(rcItem, ptInList)) // �׸񿡰� �� ��ǥ�� ����� �̹��� �������� ���
//				{
//					// ������ ���� ���� ����
//					if (MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc( pItem->GetItemID() ))
//					{
//						ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem( pItem->GetUID() );
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription(pItemDesc, szTextAreaName, pItemNode);
//					}
//					if (MQuestItemDesc* pQuestItemDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc( pItem->GetItemID() ))
//					{
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pQuestItemDesc, szTextAreaName );
//					}
//					if (MMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(pItem->GetItemID()) )
//					{
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pGItemDesc, szTextAreaName );
//					}
//
//					// ������ ��ġ
//					MRECT rcTextArea = pItemDescTextArea->GetRect();
//					MPOINT posDesc(rcItem.x, rcItem.y);
//					posDesc = MClientToScreen(this, posDesc);
//					posDesc.x -= pItemDescTextArea->GetClientWidth();			// �ϴ� �������� ��������
//					if (posDesc.y+rcTextArea.h > rcListBox.y + rcListBox.h)		// ����Ʈ �ڽ� �ϴ��� �Ѿ�� �ʰ� ����
//						posDesc.y = rcListBox.y + rcListBox.h - rcTextArea.h;
//					if (posDesc.y < 0)											// �׷��ٰ� ȭ�� ���� �հ� ���� �ȵȴ�
//						posDesc.y = 0;
//					pItemDescTextArea->SetPosition(posDesc);
//					pItemDescTextArea->SetZOrder(MZ_TOP);
//					ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(true, pItemDescTextArea, this);
//					return;
//				}
//			}
//		}
//	}
//
//	m_idxItemLastTooltip = -1;
//	ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(false, pItemDescTextArea, this);
//}
//
//ZEquipmentListBox_OLD::ZEquipmentListBox_OLD(const char* szName, MWidget* pParent, MListener* pListener)
//: MMultiColListBox(szName, pParent, pListener)
//{
//	SetDesiredNumRow(4);
//
//	//	m_pOnDropFunc = NULL;
//
//	//	m_nLastItem=-1;
//	//	m_dwLastMouseMove=timeGetTime();
//	m_pDescFrame=NULL;
//
//	m_pItemMenu = NULL;
//	m_idxItemLastTooltip = -1;
//}
//
//ZEquipmentListBox_OLD::~ZEquipmentListBox_OLD(void)
//{
//	if (m_pItemMenu) {
//		delete m_pItemMenu;
//		m_pItemMenu = NULL;
//	}
//}
//
//void ZEquipmentListBox_OLD::AttachMenu(ZItemMenu* pMenu) 
//{ 
//	m_pItemMenu = pMenu;
//	((MPopupMenu*)m_pItemMenu)->Show(false);
//}
//
//// Listener //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//
///////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////
////
////class MShopPurchaseItemListBoxListener : public MListener{
////public:
////	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
////	{
////		MButton* pButton1 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuyConfirmCaller");
////		MButton* pButton2 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuySpendableItemConfirmCaller");
////		MButton* pButton3 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuyCashConfirmCaller");
////
////		if(MWidget::IsMsg(szMessage, MLB_ITEM_SEL) == true) {
////
////			ZEquipmentListBox_OLD* pEquipmentListBox = (ZEquipmentListBox_OLD*)pWidget;
////			ZEquipmentListItem_OLD* pListItem = (ZEquipmentListItem_OLD*)pEquipmentListBox->GetSelItem();
////			if (pListItem == NULL) return true;
////
////			int nMSID = pListItem->GetUID().Low;
////			int nItemID = ZGetShopEx()->GetItemID(nMSID);
////			MShopItemType nType = ZGetShopEx()->GetItemType(nMSID);
////
////			switch(nType)
////			{
////			case MSIT_SITEM :
////				{
////					MShopBaseItem* pBaseItem = ZGetShopEx()->GetItem(nMSID);
////					if (!pBaseItem) { _ASSERT(0); return false; }
////
////					MShopSetItem* pSetItem = MDynamicCast(MShopSetItem, pBaseItem);
////					if (!pSetItem) { _ASSERT(0); return false; }
////
////					ZCharacterView* pCharacterView = 
////						(ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformationShop");
////					if( pCharacterView == NULL ) return false;
////
////					pCharacterView->SetSelectMyCharacter();
////					MMatchItemDesc* pDesc;
////					for (int i=0; i<MAX_SET_ITEM_COUNT; ++i)
////					{
////						int partsItemId = pSetItem->GetItemID(i);
////						if (partsItemId != 0)
////						{
////							pDesc = MGetMatchItemDescMgr()->GetItemDesc(partsItemId);
////							if (!pDesc) { _ASSERT(0); continue; }
////
////							MMatchCharItemParts nCharItemParts = GetSuitableItemParts(pDesc->m_nSlot);
////							pCharacterView->SetParts(nCharItemParts, pDesc->m_nID);
////						}
////					}
////
////					// ������ ������ ���� ���� �ؽ�Ʈ�� ����
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( , "Shop_ItemDescription", NULL);
////					UpdateCharInfoTextWithNewItem( );
////
////					if(pButton1) { pButton1->Enable(false); pButton1->Show(false); }
////					if(pButton2) { pButton2->Enable(false);  pButton2->Show(false);  }
////					if(pButton3) { pButton1->Enable(true); pButton3->Show(true); }
////				}
////				return true;
////
////			case MSIT_GITEM :
////				{
////					MMatchGambleItemDesc* pDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(nItemID);
////					if( pDesc == NULL ) return false;
////
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pDesc, "Shop_ItemDescription" );
////					
////					if(pButton1) { pButton1->Enable(false); pButton1->Show(false); }
////					if(pButton2) { pButton2->Enable(true);  pButton2->Show(true);  }
////					if(pButton3) { pButton1->Enable(false); pButton3->Show(false); }
////				}
////				return true;
////
////			case MSIT_QITEM :
////				{
////#ifdef _QUEST_ITEM
////					MQuestItemDesc* pDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc( nItemID );
////					if( pDesc == NULL ) return false;
////
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pDesc, "Shop_ItemDescription");
////
////					if (pButton1) { pButton1->Enable(false); pButton1->Show(false); }
////					if (pButton2) { pButton2->Enable(true);  pButton2->Show(true);  }
////					if (pButton3) { pButton3->Enable(false); pButton3->Show(false); }
////#endif
////				}
////				return true;
////
////			case MSIT_ZITEM :
////				{
////					MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
////					if( pDesc == NULL ) return false;
////
////					ZCharacterView* pCharacterView = 
////						(ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformationShop");
////					if( pCharacterView == NULL ) return false;
////
////					MMatchCharItemParts nCharItemParts = GetSuitableItemParts(pDesc->m_nSlot);
////
////					pCharacterView->SetSelectMyCharacter();
////					pCharacterView->SetParts(nCharItemParts, pDesc->m_nID);
////
////					if (IsWeaponCharItemParts(nCharItemParts)) {
////						pCharacterView->ChangeVisualWeaponParts(nCharItemParts);
////					}
////
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pDesc, "Shop_ItemDescription", NULL);
////
////					UpdateCharInfoTextWithNewItem(pDesc);
////
////					if ( pDesc->IsCashItem() ) {
////						if( pButton1 ) { pButton1->Enable(false);  pButton1->Show(false);  }
////						if( pButton2 ) { pButton2->Enable(false); pButton2->Show(false); }
////						if( pButton3 ) { pButton3->Enable(true); pButton3->Show(true); }
////					} else {
////						if( pDesc->IsSpendableItem() ) {
////							if( pButton1 ) { pButton1->Enable(false); pButton1->Show(false); }
////							if( pButton2 ) { pButton2->Enable(true);  pButton2->Show(true);  }
////						} else { 
////							if( pButton1 ) { pButton1->Enable(true);  pButton1->Show(true);  }
////							if( pButton2 ) { pButton2->Enable(false); pButton2->Show(false); }
////						}
////						if( pButton3 ) { pButton3->Enable(false); pButton3->Show(false); }
////					}
////				}
////
////				return true;
////			}
////		}
////		else if ( MWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
////		{
////			MWidget* pWidget = (MWidget*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Shop_BuyConfirm");
////			//			if ( pWidget)
////			//				pWidget->Show( true);
////
////			return true;
////		}
////
////		return false;
////	}
////};
////MShopPurchaseItemListBoxListener g_ShopPurchaseItemListBoxListener;
////
////MListener* ZGetShopPurchaseItemListBoxListener(void)
////{
////	return &g_ShopPurchaseItemListBoxListener;
////}
//
//
//
//////////
////class MEquipmentItemListBoxListener : public MListener{
////public:
////	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
////	{
////		if ( MWidget::IsMsg(szMessage, MLB_ITEM_SEL)==true) {
////			unsigned long int nItemID = 0;
////
////			ZEquipmentListBox_OLD* pEquipmentListBox = (ZEquipmentListBox_OLD*)pWidget;
////			ZEquipmentListItem_OLD* pListItem = (ZEquipmentListItem_OLD*)pEquipmentListBox->GetSelItem();
////			if (pListItem != NULL) 
////				nItemID = ZGetMyInfo()->GetItemList()->GetItemID(pListItem->GetUID());
////
////			ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText();
////
////			// Gamble ���������� Ȯ��
////			MMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(pListItem->GetItemID());
////			if ( pGItemDesc )
////			{
////				ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pGItemDesc, "Equip_ItemDescription");
////
////				ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( MMIST_NONE);
////
////				MButton *pButton = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Equip");
////				if (pButton) pButton->Enable( true);
////
////				MButton *pButton1 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountItemBtn");
////				MButton *pButton2 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountSpendableItemConfirmOpen");
////				if (pButton1) { pButton1->Show(false); pButton1->Enable(false); }
////				if (pButton2) { pButton2->Show(true);  pButton2->Enable( pGItemDesc->IsCash() ? true : false); }
////
////				return true;
////			}
////
////
////			// �Ϲ� ������...
////			MButton* pButtonEquip = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Equip");
////			MButton* pButtonAccItemBtn1 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountItemBtn");
////			MButton* pButtonAccItemBtn2 = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountSpendableItemConfirmOpen");
////
////			MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
////			ZCharacterView* pCharacterView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformation");
////			if ((pCharacterView) && (pItemDesc)) {
////
////				UpdateCharInfoTextWithNewItem(pItemDesc);
////
////				MMatchCharItemParts nCharItemParts = GetSuitableItemParts(pItemDesc->m_nSlot);
////
////				pCharacterView->SetSelectMyCharacter();
////				pCharacterView->SetParts(nCharItemParts, pItemDesc->m_nID);
////
////				if (IsWeaponCharItemParts(nCharItemParts))
////					pCharacterView->ChangeVisualWeaponParts(nCharItemParts);
////
////				// gambleitem�� ������ ���ø� �ϱ� ������ ����Ʈ���� �׺� �����ۼ���ŭ ����� ������ ZItem�� ���ؽ��� �ȴ�.
////				//const int nGItemListCount = ZGetMyInfo()->GetItemList()->GetGambleItemCount();
////				//ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem( pEquipmentListBox->GetSelIndex() - nGItemListCount );				
////
////				ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem(pListItem->GetUID());				
////				if ( pItemDesc && pItemNode)
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pItemDesc, "Equip_ItemDescription", pItemNode);
////
////				ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( pItemDesc->m_nSlot);
////
////				if ( pButtonEquip && (pItemDesc->m_nSlot != MMIST_NONE))	pButtonEquip->Enable( true);
////				else														pButtonEquip->Enable( false);
////
////				// ĳ�� �������� ��� '�߾����࿡ ������'��ư Ȱ��ȭ, �ƴ� ��Ȱ��ȭ
////				if( pItemDesc->IsSpendableItem() ) {
////					if( ZGetIsCashItem(nItemID)) {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(false); pButtonAccItemBtn1->Enable(false); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(true);  pButtonAccItemBtn2->Enable(true);  }
////					} else {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(false); pButtonAccItemBtn1->Enable(false); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(true);  pButtonAccItemBtn2->Enable(false); }
////					} 					
////				} else {
////					if( ZGetIsCashItem(nItemID)) {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(true);  pButtonAccItemBtn1->Enable(true); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(false); pButtonAccItemBtn2->Enable(false);}
////					} else {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(true);  pButtonAccItemBtn1->Enable(false); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(false); pButtonAccItemBtn2->Enable(false); }
////					}
////				}
////			}
////
////			// ����Ʈ ������
////			MQuestItemDesc* pQuestItemDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc( pListItem->GetItemID());
////			if ( pQuestItemDesc) {
////				ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pQuestItemDesc, "Equip_ItemDescription");
////
////				ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( MMIST_NONE);
////
////				if ( pButtonEquip) pButtonEquip->Enable( false);
////
////				if (pButtonAccItemBtn1) pButtonAccItemBtn1->Enable( false);
////				if (pButtonAccItemBtn2) pButtonAccItemBtn2->Enable( false);
////			}
////
////			return true;
////		}
////		else if ( MWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
////		{
////			ZGetGameInterface()->GetShopEquipInterface()->Equip();		
////			return true;
////		}
////
////		return false;
////	}
////};
////MEquipmentItemListBoxListener g_EquipmentItemListBoxListener;
////
////MListener* ZGetEquipmentItemListBoxListener(void)
////{
////	return &g_EquipmentItemListBoxListener;
////}
//
//void ZEquipmentListItem_OLD::GetIconRect(MRECT& out, const MRECT& rcItem)
//{
//	int len = rcItem.h-4;
//	out.Set(2, 2, len, len);
//	//out.Set(2, 2, CONVERT800(60), CONVERT600(60));
//}
//
//void ZEquipmentListItem_OLD::OnDraw(MRECT& r, MDrawContext* pDC, bool bSelected, bool bMouseOver)
//{
//	// ����
//	if (bSelected)
//		pDC->SetColor(220, 220, 220);
//	else if (bMouseOver)
//		pDC->SetColor(60, 60, 60);
//	else
//		pDC->SetColor(30, 30, 30);
//
//	pDC->FillRectangle(r);
//
//	MRECT rcIcon;
//	GetIconRect(rcIcon, r);
//	rcIcon.x += r.x;
//	rcIcon.y += r.y;
//
//	// ������ ����
//	//if (!bSelected)
//	pDC->SetColor(15, 15, 15);
//	//else
//	//	pDC->SetColor(200, 200, 200);
//	pDC->FillRectangle(rcIcon);
//
//	pDC->SetBitmap(m_pBitmap);
//	pDC->Draw(rcIcon);
//
//	MRECT rc;	// ��Ʈ���� ������ ����
//	rc.x = rcIcon.x + rcIcon.w + 2;
//	rc.w = r.w - rcIcon.w - 6;
//	rc.y = r.y + 2;
//	rc.h = r.h - 4;
//
//	// �ؽ�Ʈ ��
//	if (!bSelected)
//		pDC->SetColor(200, 200, 200);
//	else
//		pDC->SetColor(20, 20, 20);
//
//	// �����۸�, ����
//	pDC->TextMultiLine2(rc, GetString(), CONVERT600(2), true, MAM_LEFT | MAM_TOP);
//	pDC->Text(rc, m_szPrice, MAM_RIGHT | MAM_BOTTOM);
//
//	// ���� (������ ���� ��� ���� ǥ��)
//	// ����� ������ ���� ��� ������ �׸��ڸ� �׷��� �þȼ��� ���δ�
//
//	pDC->SetColor(20, 20, 20);
//	pDC->Text(MRECT(rcIcon.x+1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, MAM_LEFT | MAM_BOTTOM);	// 1�ȼ��� ��������
//	pDC->Text(MRECT(rcIcon.x-1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, MAM_LEFT | MAM_BOTTOM);
//	pDC->Text(MRECT(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h+1), m_szLevel, MAM_LEFT | MAM_BOTTOM);
//	pDC->Text(MRECT(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h-1), m_szLevel, MAM_LEFT | MAM_BOTTOM);
//
//	if (m_bLevelResticted)
//		pDC->SetColor(200, 10, 10);
//	else
//		pDC->SetColor(200, 200, 200);
//
//	pDC->Text(rcIcon, m_szLevel, MAM_LEFT | MAM_BOTTOM);
//}
//
//bool ZEquipmentListItem_OLD::IsPtInRectToShowToolTip(MRECT& rcItem, MPOINT& pt)
//{
//	// pt�� ����� ������ ������ �ִ��� �Ǵ�
//	MRECT rcIcon;
//	GetIconRect(rcIcon, rcItem);
//	// ����Ʈ ��ǥ��� ��ȯ
//	rcIcon.x += rcItem.x;
//	rcIcon.y += rcItem.y;
//	// pt-in-rect ����
//	return (rcIcon.x < pt.x && pt.x < rcIcon.x+rcIcon.w &&
//		rcIcon.y < pt.y && pt.y < rcIcon.y+rcIcon.h);
//}
//
//int ZEquipmentListItem_OLD::GetSortHint()
//{
//	int itemID = GetItemID();
//
//	// ����Ʈ�ڽ��� �������� �������� �����ϵ��� ������ ���� ������ �����Ѵ�
//	MMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(itemID);
//	if (pGItemDesc) return 100000;
//
//	MShopBaseItem* pShopItem = ZGetShopEx()->GetItem(m_UID.Low);
//	if (MDynamicCast(MShopSetItem, pShopItem)) {
//		int nResLv = 0;// ��Ʈ�� ���� ������ �ٽ� ����ؾ� �Ѵ�. �׷��� �ٽ� ����������� �׳� ����Ʈ�����ۿ� ������ int�� ���� �ְ� ����
//		return 300000 + nResLv;
//	}
//
//	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(itemID);
//	if (pItemDesc) {
//		int hint = 200000;
//		int nResLv = pItemDesc->m_nResLevel.Ref();
//		if (pItemDesc->m_nType.Ref() == MMIT_MELEE)
//			return hint + 10000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_RANGE)
//			return hint + 20000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_EQUIPMENT)
//			return hint + 30000 + pItemDesc->m_nSlot * 100 + nResLv;
//
//		hint = 400000;
//		if (pItemDesc->m_nType.Ref() == MMIT_CUSTOM)
//			return hint + 40000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_COMMUNITY)
//			return hint + 50000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_LONGBUFF)
//			return hint + 60000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		
//		return hint + 70000 + nResLv;
//	}
//
//	MQuestItemDesc* pQuestItemDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc(itemID);
//	if (pQuestItemDesc) return 500000;
//
//	return 999999;
//}