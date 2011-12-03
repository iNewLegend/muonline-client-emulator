#include "stdafx.h"

#include "ZItemSlotView.h"
#include "ZItemIconBitmap.h"
#include "ZIDLResource.h"
#include "ZApplication.h"
#include "ZMyInfo.h"
#include "ZMyItemList.h"
#include "ZPost.h"
#include "MComboBox.h"
#include "ZShopEquipInterface.h"
#include "ZShopEquipItem.h"
#include "ZShopEquipListbox.h"


ZItemSlotView::ZItemSlotView(const char* szName, MWidget* pParent, MListener* pListener)
: MButton(szName, pParent, pListener)
{
	m_nItemID = -1;
	m_nItemCount = 0;

	m_nParts = MMCIP_END;
	m_pBackBitmap = NULL;
	m_bSelectBox = false;
	m_bKindable = false;
	m_bDragAndDrop = false;
	m_bHorizonalInverse = false;
	m_szItemSlotPlace[0] = '\0';
}

ZItemSlotView::~ZItemSlotView(void)
{
}


void ZItemSlotView::SetDefaultText(MMatchCharItemParts nParts)
{
	switch (nParts)
	{
	case MMCIP_HEAD:			SetText("< Head >");			 break;
	case MMCIP_CHEST:			SetText("< Chest >");			 break;
	case MMCIP_HANDS:			SetText("< Hands >");			 break;
	case MMCIP_LEGS:			SetText("< Legs >");			 break;
	case MMCIP_FEET:			SetText("< Feet >");			 break;
	case MMCIP_FINGERL:			SetText("< Left Finger >");		 break;
	case MMCIP_FINGERR:			SetText("< Right Finger >");	 break;	
#ifdef _AVATAR_ENABLE
	case MMCIP_AVATAR:			SetText("< Avatar >");			 break;
#endif
	case MMCIP_MELEE:			SetText("< Melee >");			 break;
	case MMCIP_PRIMARY:			SetText("< Primary Weapon >");	 break;
	case MMCIP_SECONDARY:		SetText("< Secondary Weapon >"); break;
	case MMCIP_CUSTOM1:			SetText("< Item 1 >");			 break;
	case MMCIP_CUSTOM2:			SetText("< Item 2 >");			 break;
	/*
	case MMCIP_COMMUNITY1:		SetText("< Community 1 >");		 break;
	case MMCIP_COMMUNITY2:		SetText("< Community 2 >");		 break;
	case MMCIP_LONGBUFF1:		SetText("< LongBuff 1 >");		 break;
	case MMCIP_LONGBUFF2:		SetText("< LongBuff 2 >");		 break;*/

	default : SetText(""); break;
	}
}

void ZItemSlotView::OnDraw(MDrawContext* pDC)
{
	if ( m_nParts == MMCIP_END) return;

	unsigned int nItemID;

	if (m_nItemID != -1) {
		nItemID = m_nItemID;
	} else {
		nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(m_nParts);
	}

	MMatchItemDesc* pItemDesc = NULL;
	if (nItemID != 0) 
		pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

	MRECT r;

	// Draw select box
	if ( m_bSelectBox)
	{
		r = GetClientRect();
		r.x -= 5;
		r.y -= 5;
		r.w += 10;
		r.h += 10;
		pDC->SetColor( MCOLOR( 0x60808080));
		pDC->FillRectangle( r);
	}

	// Draw BItmap
	int w, h, x;
	r = GetRect();
	w = min ( r.w, r.h);
	h = min ( r.w, r.h);
	x = 0;
	if (m_bHorizonalInverse)
		x = r.w - w;

	if ( (nItemID == 0) || (pItemDesc == NULL))
	{
		if ( m_pBackBitmap != NULL)
		{
			pDC->SetBitmap(m_pBackBitmap);
			pDC->Draw(x, 0, w, h);
		}
	}
	else
	{
		MBitmap* pIconBitmap = GetItemIconBitmap(pItemDesc);

		if (pIconBitmap)
		{
			pDC->SetBitmap(pIconBitmap);
			pDC->Draw( x, 0, w, h);
		}
	}

	// Draw Item box
	if (m_bKindable) {
		pDC->SetColor( MCOLOR( 0xFFFFFFFF));
		pDC->Rectangle( x, 0, w, h);
	} else {
		pDC->SetColor( MCOLOR( 0xFF303030));
		pDC->Rectangle( x, 0, w, h);
	}

	// Draw Name
	MAlignmentMode am = MAM_VCENTER;
	r = GetClientRect();
	int margin = int(10 * MGetWorkspaceWidth() / 800.f);
	r.x += w + margin;
	r.w -= (w + margin);

	if (m_bHorizonalInverse) {
		am = MAM_VCENTER | MAM_RIGHT;
		r.x = 0;
	}

	if ( pItemDesc != NULL)
	{
		pDC->SetColor( MCOLOR(0xFFC0C0C0));

		int nCount;
		char szItemName[256] = {0, };

		ZMyItemNode* pItem = ZGetMyInfo()->GetItemList()->GetEquipedItem(m_nParts);
		if( pItem != NULL ) { nCount = pItem->GetItemCount(); } 
		else				{ nCount = m_nItemCount; }

		switch(m_nParts)
		{
		case MMCIP_HEAD:
		case MMCIP_CHEST:
		case MMCIP_HANDS:
		case MMCIP_LEGS:
		case MMCIP_FEET:
		case MMCIP_FINGERL:
		case MMCIP_FINGERR:
		case MMCIP_AVATAR:
		case MMCIP_MELEE:
		case MMCIP_PRIMARY:
		case MMCIP_SECONDARY:
			sprintf(szItemName, "%s", pItemDesc->m_pMItemName->Ref().m_szItemName);
			break;

		case MMCIP_CUSTOM1:
		case MMCIP_CUSTOM2:
			if( pItemDesc->IsSpendableItem() )  sprintf(szItemName, "%s (x%d)", pItemDesc->m_pMItemName->Ref().m_szItemName, nCount);
			else								sprintf(szItemName, "%s", pItemDesc->m_pMItemName->Ref().m_szItemName);
			break;

		case MMCIP_COMMUNITY1:
		case MMCIP_COMMUNITY2:
			if( pItemDesc->IsSpendableItem() )  sprintf(szItemName, "%s (x%d)", pItemDesc->m_pMItemName->Ref().m_szItemName, nCount);
			else								sprintf(szItemName, "%s", pItemDesc->m_pMItemName->Ref().m_szItemName);
			break;

		case MMCIP_LONGBUFF1:
		case MMCIP_LONGBUFF2:
			if( pItemDesc->IsSpendableItem() )  sprintf(szItemName, "%s (x%d)", pItemDesc->m_pMItemName->Ref().m_szItemName, nCount);
			else								sprintf(szItemName, "%s", pItemDesc->m_pMItemName->Ref().m_szItemName);
			break;

		default:
			_ASSERT(0);
			break;
		}

		pDC->Text(r, szItemName, am);
	}
	else
	{
		if (m_bSelectBox)	pDC->SetColor(MCOLOR(0xFF202020));
		else				pDC->SetColor(MCOLOR(0xFF404040));

		pDC->Text(r, m_szName, am);
	}
}

void ZItemSlotView::SetParts(MMatchCharItemParts nParts)
{
	SetDefaultText(nParts);
	m_nParts = nParts;
}

bool ZItemSlotView::IsDropable(MWidget* pSender)
{
	if (pSender == NULL)
		return false;

	return true;
}


bool ZItemSlotView::IsEquipableItem(unsigned long int nItemID, int nPlayerLevel, MMatchSex nPlayerSex)
{
	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc == NULL) return false;

	// ���� ���� ����
	if (pItemDesc->m_nResSex.Ref() != -1)
	{
		if (pItemDesc->m_nResSex.Ref() != int(nPlayerSex)) return false;
	}

	// ���� ���� ����
	if (pItemDesc->m_nResLevel.Ref() > nPlayerLevel) return false;


	return true;
}

bool ZItemSlotView::OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	if ( (pSender == NULL) || ( m_bDragAndDrop == false))
		return false;

	if (strcmp(pSender->GetClassName(), MINT_EQUIPMENTLISTBOX)==0) {

		// widget id ����
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		ZShopEquipListbox* pWidget = (ZShopEquipListbox*)pResource->FindWidget("EquipmentList");
		if (pWidget==NULL) return false;

		ZShopEquipListItem* pListItem = (ZShopEquipListItem*)pWidget->GetSelItem();
		if (!pListItem) return false;

		if (ZSEIT_MATCH != pListItem->GetItemData()->GetType()) return false;

		ZShopEquipItem_Match* pWrappedMatchItem = (ZShopEquipItem_Match*)pListItem->GetItemData();
		MMatchItemDesc* pItemDesc = pWrappedMatchItem->GetDesc();
		if (pItemDesc == NULL) return false;
		
		IShopEquipItemHandle_Sell* pHandleSell = pWrappedMatchItem->GetHandleSell();
		MUID uidItem = pHandleSell->GetItemUID();

		if (!IsSuitableItemSlot(pItemDesc->m_nSlot, m_nParts)) return false;
		if (!IsEquipableItem(pItemDesc->m_nID, 99, ZGetMyInfo()->GetSex())) return false;

		ZGetGameInterface()->GetShopEquipInterface()->Equip( m_nParts, uidItem );
	}
	
	else if (strcmp(pSender->GetClassName(), MINT_ITEMSLOTVIEW)==0) {	// Equip ���Կ��� Equip ��������
		ZItemSlotView* pWidget = (ZItemSlotView*)pSender;
		MUID uidItem = ZGetMyInfo()->GetItemList()->GetEquipedItemUID(pWidget->GetParts());
		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(ZGetMyInfo()->GetItemList()->GetItemID(uidItem));
		if (pItemDesc == NULL) return false;
		if (!IsSuitableItemSlot(pItemDesc->m_nSlot, m_nParts)) return false;
		if (!IsEquipableItem(pItemDesc->m_nID, 99, ZGetMyInfo()->GetSex())) return false;

		ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), pWidget->GetParts());
		ZGetGameInterface()->GetShopEquipInterface()->Equip(m_nParts, uidItem);
	}

	if (strcmp( m_szItemSlotPlace, "SACRIFICE0") == 0)
		ZApplication::GetStageInterface()->OnDropSacrificeItem( 0);
	else if ( strcmp( m_szItemSlotPlace, "SACRIFICE1") == 0)
		ZApplication::GetStageInterface()->OnDropSacrificeItem( 1);

	return true;
}

bool ZItemSlotView::OnEvent(MEvent* pEvent, MListener* pListener)
{
	bool bRet = MButton::OnEvent(pEvent, pListener);

	m_bSelectBox = false;

	// Check rect range
	MRECT r = GetClientRect();
	if ( r.InPoint( pEvent->Pos) == false)
		return bRet;


	if ( pEvent->nMessage == MWM_LBUTTONDOWN)
	{
		m_bSelectBox = true;

		// ���� �� ��� ������ ������ ���
		unsigned long int nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(m_nParts);
		MMatchItemDesc* pItemDesc = NULL;
		if (nItemID != 0)
		{
			pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

			// ������ ���� ������Ʈ
			ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetEquipedItem( m_nParts);
			MTextArea* pTextArea = (MTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget( GetItemDescriptionWidgetName());
			
			ZShopEquipItem_Match::FillItemDesc(pItemDesc, pTextArea, pItemNode);

			// �巡�� & ���
			if (pItemDesc && m_bDragAndDrop)
			{
				MBitmap* pIconBitmap = GetItemIconBitmap(pItemDesc);
				Mint::GetInstance()->SetDragObject(this, pIconBitmap, pItemDesc->m_pMItemName->Ref().m_szItemName, 
					pItemDesc->m_pMItemName->Ref().m_szItemName);
			}

			ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( pItemDesc->m_nSlot);
		}


		// ��� ������ ������ ���
		if ( (strcmp( m_szItemSlotPlace, "SACRIFICE0") == 0) || (strcmp( m_szItemSlotPlace, "SACRIFICE1") == 0))
		{
			int nSlotNum = (strcmp( m_szItemSlotPlace, "SACRIFICE0") == 0) ? 0 : 1;

			if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ nSlotNum].IsExist())
				return bRet;

			// �巡�� & ���
			if ( m_bDragAndDrop)
			{
				Mint::GetInstance()->SetDragObject( this,
													ZApplication::GetStageInterface()->m_SacrificeItem[ nSlotNum].GetIconBitmap(),
													ZApplication::GetStageInterface()->m_SacrificeItem[ nSlotNum].GetName(),
													ZApplication::GetStageInterface()->m_SacrificeItem[ nSlotNum].GetName());
			}
		}
	}

	else if ( pEvent->nMessage == MWM_LBUTTONUP)
	{
		ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( MMIST_NONE);
	}
	
	// ����Ŭ���� ������ ����
	else if ( pEvent->nMessage == MWM_LBUTTONDBLCLK)
	{
		unsigned long int nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(m_nParts);
		MMatchItemDesc* pItemDesc = NULL;
		if (nItemID != 0)
		{
			pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

			if (pItemDesc && m_bDragAndDrop)
			{
				ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), m_nParts);
				// �������� �����ش�.
				// ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());
			}
		}

		// ��� ������ ������ ���
		if ( (strcmp( m_szItemSlotPlace, "SACRIFICE0") == 0) || (strcmp( m_szItemSlotPlace, "SACRIFICE1") == 0))
		{
			ZApplication::GetStageInterface()->OnRemoveSacrificeItem( (strcmp( m_szItemSlotPlace, "SACRIFICE0") == 0) ? 0 : 1);
		}
	}

	return bRet;
}


void ZItemSlotView::SetBackBitmap(MBitmap* pBitmap)
{
	m_pBackBitmap = pBitmap;
	if( !GetStretch() && m_pBackBitmap!=NULL)
	{
		SetSize(m_pBackBitmap->GetWidth(), m_pBackBitmap->GetHeight());
	}

}

void ZItemSlotView::SetIConBitmap(MBitmap* pBitmap)
{
}

void ZItemSlotView::EnableDragAndDrop( bool bEnable)
{
	m_bDragAndDrop = bEnable;
}

void ZItemSlotView::SetKindable( bool bKindable)
{
	m_bKindable = bKindable;
}

void ZItemSlotView::OnMouseIn( void )
{
	MButton::OnMouseIn();

	// ���� �� ��� ������ ������ ��� ���� ǥ��
	unsigned long int nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(m_nParts);
	MMatchItemDesc* pItemDesc = NULL;
	if (nItemID == 0) return;

	const char* szItemDescription = GetItemDescriptionWidgetName();
	MTextArea* pItemDescTextArea = (MTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget(szItemDescription);
	if (!pItemDescTextArea) return;

	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

	// ������ ���� ������Ʈ
	ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetEquipedItem( m_nParts);
	MTextArea* pTextArea = (MTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget( szItemDescription);
	ZShopEquipItem_Match::FillItemDesc(pItemDesc, pTextArea, pItemNode);

	// ������ ��ġ
	MRECT rcSlotView = GetRect();
	MRECT rcTextArea = pItemDescTextArea->GetRect();
	MPOINT posDesc(rcSlotView.x, rcSlotView.y);
	posDesc = MClientToScreen(GetParent(), posDesc);
	posDesc.x = rcSlotView.x + rcSlotView.w + CONVERT800(20);	// �ϴ� ���Ժ� ��������
	if (posDesc.y+rcTextArea.h > MGetWorkspaceHeight())			// ȭ�� �ϴܿ� ©���� �ʰ�
		posDesc.y = MGetWorkspaceHeight() - rcTextArea.h;
	pItemDescTextArea->SetPosition(posDesc);
	pItemDescTextArea->SetZOrder(MZ_TOP);
	ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(true, pItemDescTextArea, this);
}

void ZItemSlotView::OnMouseOut( void )
{
	MButton::OnMouseOut();

	MTextArea* pItemDescTextArea = (MTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget(GetItemDescriptionWidgetName());
	if (!pItemDescTextArea) return;

	ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(false, pItemDescTextArea, this);
}

const char* ZItemSlotView::GetItemDescriptionWidgetName()
{
	static char szItemDescription[128];
	sprintf( szItemDescription, "%s_ItemDescription", m_szItemSlotPlace);
	return szItemDescription;
}