/***********************************************************************
  ZMonsterBookInterface.cpp
  
  ��  �� : ���� ���� �������̽�
  �ۼ��� : 29, MAR, 2004
  �ۼ��� : �ӵ�ȯ
************************************************************************/


#include "stdafx.h"							// Include stdafx.h
#include "ZMonsterBookInterface.h"			// Include ZMonsterBookInterface.h
#include "ZQuest.h"							// Include ZQuest.h
#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZItemIconBitmap.h"
#include "ZModule_Skills.h"



/***********************************************************************
  ZMonsterBookInterface : public
  
  desc : ������
************************************************************************/
ZMonsterBookInterface::ZMonsterBookInterface( void)
{
	m_pBookBgImg = NULL;
	m_pIllustImg = NULL;
	m_fCompleteRate = 0.0f;
}


/***********************************************************************
  ~ZMonsterBookInterface : public
  
  desc : �Ҹ���
************************************************************************/
ZMonsterBookInterface::~ZMonsterBookInterface( void)
{
	OnDestroy();
}


/***********************************************************************
  OnCreate : public
  
  desc : ���� ���� ���̱�
  arg  : none
  ret  : none
************************************************************************/
void ZMonsterBookInterface::OnCreate( void)
{
	// Get resource pointer
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();


	// ���� �ʱ�ȭ
	m_nTotalPageNum = 0;
	m_nCurrentPageNum = 0;


	// ����Ʈ�� �ʿ��� ���� �ε�
	LoadMonsterBookInfo();


	// �κ� UI ���߱�
	MWidget* pWidget = pResource->FindWidget( "Lobby");
	if ( pWidget)
		pWidget->Show( false);


	// �������� �׸���
	DrawPage();


	// ���� ���� ���̱�
	pWidget = pResource->FindWidget( "MonsterBook");
	if ( pWidget)
		pWidget->Show( true);

	
	// ��Ʈ�� �̹��� �ִϸ��̼�
	MPicture* pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_StripBottom");
 	if( pPicture)
		pPicture->SetAnimation( 0, 1000.0f);
	pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_StripTop");
	if( pPicture)
		pPicture->SetAnimation( 1, 1000.0f);
}


/***********************************************************************
  OnDestroy : public
  
  desc : ���� ���� ���߱�
  arg  : none
  ret  : none
************************************************************************/
void ZMonsterBookInterface::OnDestroy( void)
{
	// Get resource pointer
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();


	// ���� ���� ���߱�
	MWidget* pWidget = pResource->FindWidget( "MonsterBook");
	if ( pWidget)
		pWidget->Show( false);


	// ��� å �̹����� �޸𸮷κ��� �����Ѵ�
	if ( m_pBookBgImg != NULL)
	{
		// ��� å �̹����� �����ִ� ������ ��Ʈ�� �̹��� �����͸� �����Ѵ�
		MPicture* pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_BookBG");
		if ( pPicture)
			pPicture->SetBitmap( NULL);
	
		delete m_pBookBgImg;
		m_pBookBgImg = NULL;
	}


	// �ش� ������ �Ϸ���Ʈ �̹��� ����
	if ( m_pIllustImg)
	{
		MPicture* pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_MonsterIllust");
		if ( pPicture)
			pPicture->SetBitmap( NULL);
	
		delete m_pIllustImg;
		m_pIllustImg = NULL;
	}


	// ���� ���� ������ ���� ����Ʈ ����
	if ( !m_mapMonsterBookPage.empty())
	{
		for ( ZMonsterBookPageItr itr = m_mapMonsterBookPage.begin();  itr != m_mapMonsterBookPage.end();  itr++)
			delete (*itr).second;

		m_mapMonsterBookPage.clear();
	}


	// �κ� UI ���̱�
	pWidget = pResource->FindWidget( "Lobby");
	if ( pWidget)
		pWidget->Show( true);
}


/***********************************************************************
  OnPrevPage : public
  
  desc : ���� ������ �ѱ�� ��ư�� ������ ��
  arg  : none
  ret  : none
************************************************************************/
void ZMonsterBookInterface::OnPrevPage( void)
{
	// ������ ������ ��ȣ�� ���Ѵ�
	if ( m_nCurrentPageNum == 0)
		m_nCurrentPageNum = m_nTotalPageNum;
	else
		m_nCurrentPageNum--;


	// �������� �׸���
	DrawPage();
}


/***********************************************************************
  OnNextPage : public
  
  desc : ���� ������ �ѱ�� ��ư�� ������ ��
  arg  : none
  ret  : none
************************************************************************/
void ZMonsterBookInterface::OnNextPage( void)
{
	// ������ ������ ��ȣ�� ���Ѵ�
	if ( m_nCurrentPageNum == m_nTotalPageNum)
		m_nCurrentPageNum = 0;
	else
		m_nCurrentPageNum++;


	// �������� �׸���
	DrawPage();
}


/***********************************************************************
  DrawPage : protected
  
  desc : �������� �׸���
  arg  : none
  ret  : none
************************************************************************/
void ZMonsterBookInterface::DrawPage( void)
{
	// ������ ������ �����´�
	ZMonsterBookPageItr itr = m_mapMonsterBookPage.find( m_nCurrentPageNum);
	ZMonsterBookPageInfo* pPageInfo;
	if ( itr != m_mapMonsterBookPage.end())
		pPageInfo = (*itr).second;
	else
		pPageInfo = new ZMonsterBookPageInfo;


	// Get resource pointer
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();


	// ��� �̹��� �ε�
	if ( m_pBookBgImg != NULL)
	{
		delete m_pBookBgImg;
		m_pBookBgImg = NULL;
	}

	m_pBookBgImg = new MBitmapR2;
	if ( m_nCurrentPageNum == 0)
		((MBitmapR2*)m_pBookBgImg)->Create( "monsterIllust.png", RGetDevice(), "interface/MonsterIllust/book_firstbg.jpg");
	else
		((MBitmapR2*)m_pBookBgImg)->Create( "monsterIllust.png", RGetDevice(), "interface/MonsterIllust/book_bg.jpg");

	if ( m_pBookBgImg)
	{
		// �о�� ��Ʈ�� �̹��� �����͸� �ش� ������ �Ѱ��༭ ǥ���Ѵ�
		MPicture* pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_BookBG");
		if ( pPicture)
			pPicture->SetBitmap( m_pBookBgImg->GetSourceBitmap());
	}


	// �ش� ������ �Ϸ���Ʈ �̹����� ������Ʈ �Ѵ�
	if ( m_pIllustImg)
	{
		delete m_pIllustImg;
		m_pIllustImg = NULL;
	}
	MPicture* pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_MonsterIllust");
	if ( pPicture)
		pPicture->SetBitmap( NULL);


	m_pIllustImg = new MBitmapR2;
	char szFileName[ 256];
	sprintf( szFileName, "interface/MonsterIllust/monster_Illust%02d.jpg", pPageInfo->m_nID);
	((MBitmapR2*)m_pIllustImg)->Create( "monsterIllust.png", RGetDevice(), szFileName);
	if ( m_pIllustImg)
	{
		MPicture* pPicture = (MPicture*)pResource->FindWidget( "MonsterBook_MonsterIllust");
		if ( pPicture)
			pPicture->SetBitmap( m_pIllustImg->GetSourceBitmap());
	}


	// �ش� ������ �̸��� ������Ʈ �Ѵ�
	MLabel* pLabel = (MLabel*)pResource->FindWidget( "MonsterBook_MonsterName");
	if (pLabel)
	{
		pLabel->SetText( m_nCurrentPageNum ? pPageInfo->m_strName.data() : "");
		pLabel->Show( true);
	}


	// �ش� ������ ����� ������Ʈ �Ѵ�
	pLabel = (MLabel*)pResource->FindWidget( "MonsterBook_MonsterGrade");
	if ( pLabel)
	{
		char szGrade[ 64] = { 0, };

		if ( m_nCurrentPageNum)
		{
			sprintf( szGrade, "%s : ", ZMsg(MSG_WORD_GRADE));

			switch ( pPageInfo->m_nGrade)
			{
				case NPC_GRADE_REGULAR :
					strcat( szGrade, ZMsg(MSG_WORD_REGULAR));
					break;

				case NPC_GRADE_LEGENDARY :
					strcat( szGrade, ZMsg(MSG_WORD_LEGENDARY));
					break;

				case NPC_GRADE_BOSS :
					strcat( szGrade, ZMsg(MSG_WORD_BOSS));
					break;

				case NPC_GRADE_ELITE :
					strcat( szGrade, ZMsg(MSG_WORD_ELITE));
					break;

				case NPC_GRADE_VETERAN :
					strcat( szGrade, ZMsg(MSG_WORD_VETERAN));
					break;
			}
		}

		pLabel->SetText( szGrade);
		pLabel->Show( true);
	}


	// �ش� ������ ������ ������Ʈ �Ѵ�
	MTextArea* pTextArea = (MTextArea*)pResource->FindWidget( "MonsterBook_MonsterDesc");
	if ( pTextArea)
	{
		pTextArea->Clear();
		pTextArea->AddText( m_nCurrentPageNum ? pPageInfo->m_strDesc.data() : "", MCOLOR( 0xFF321E00));
		pTextArea->Show();
	}


	// �ش� ������ HP�� ������Ʈ �Ѵ�
	pLabel = (MLabel*)pResource->FindWidget( "MonsterBook_MonsterHP");
	if ( pLabel)
	{
		char szHP[ 128] = { 0, };

		if ( m_nCurrentPageNum)
		{
			strcpy( szHP, "HP : ");

			if ( pPageInfo->m_nHP > 200)
				strcat( szHP, ZMsg(MSG_WORD_VERYHARD));
			else if ( pPageInfo->m_nHP > 120)
				strcat( szHP, ZMsg(MSG_WORD_HARD));
			else if ( pPageInfo->m_nHP > 80)
				strcat( szHP, ZMsg(MSG_WORD_NORAML));
			else if ( pPageInfo->m_nHP > 30)
				strcat( szHP, ZMsg(MSG_WORD_WEAK));
			else
				strcat( szHP, ZMsg(MSG_WORD_VERYWEAK));
		}

		pLabel->SetText( szHP);
		pLabel->Show( true);
	}


	// �ش� ������ Ư���⸦ ������Ʈ �Ѵ�
	pTextArea = (MTextArea*)pResource->FindWidget( "MonsterBook_Attacks");
	if ( pTextArea)
	{
		pTextArea->Clear();

		for ( list<string>::iterator itrSkill = pPageInfo->m_Skill.begin();  itrSkill != pPageInfo->m_Skill.end();  itrSkill++)
			pTextArea->AddText( (*itrSkill).data(), MCOLOR( 0xFF321E00));
	}


	// ��� ������ ������Ʈ
	list<ZDropItemInfo*>::iterator  itrDropItem = pPageInfo->m_DropItem.begin();
	for ( int i = 0;  i < 10;  i++)
	{
		char szWidgetName[ 50];
		sprintf( szWidgetName, "MonsterBook_DropItem%d", i);
		MPicture* pPicture = (MPicture*)pResource->FindWidget( szWidgetName);
		if ( pPicture)
		{
			if ( itrDropItem != pPageInfo->m_DropItem.end())
			{
				pPicture->AttachToolTip( (*itrDropItem)->m_strName.data());
				pPicture->SetBitmap( (*itrDropItem)->m_pIcon);
				pPicture->Show( true);

				itrDropItem++;
			}
			else
			{
				pPicture->Show( false);
			}
		}
	}


	// ������ ��ȣ�� ������Ʈ �Ѵ�
	pLabel = (MLabel*)pResource->FindWidget( "MonsterBook_PageNumber");
	if ( pLabel)
	{
		char szPageNum[ 20] = { 0, };

		if ( m_nCurrentPageNum)
			sprintf( szPageNum, "- %d -", m_nCurrentPageNum);

		pLabel->SetText( szPageNum);
	}


	// �޼����� ǥ���Ѵ�
	pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "MonsterBook_Complete");
	if ( pLabel)
	{
		char szComplete[ 128] = { 0, };

		if ( m_nCurrentPageNum == 0)
			sprintf( szComplete, "%s : %.1f%%", ZMsg(MSG_WORD_RATE), m_fCompleteRate);

		pLabel->SetText( szComplete);
	}


	// ���� ������
	MWidget* pWidget = pResource->FindWidget( "MonsterBook_PrevPageButton");
	if ( pWidget)
		pWidget->Show( (m_nCurrentPageNum > 0) ? true : false);


	// ���� ������
	pWidget = pResource->FindWidget( "MonsterBook_NextPageButton");
	if ( pWidget)
		pWidget->Show( (m_nCurrentPageNum < m_nTotalPageNum) ? true : false);


	// �޸� ����
	if ( itr == m_mapMonsterBookPage.end())
		delete pPageInfo;
}


// LoadMonsterBookInfo
bool ZMonsterBookInterface::LoadMonsterBookInfo( void)
{
	// ���� ���� ������ ���� ����Ʈ ����
	if ( !m_mapMonsterBookPage.empty())
	{
		for ( ZMonsterBookPageItr itr = m_mapMonsterBookPage.begin();  itr != m_mapMonsterBookPage.end();  itr++)
			delete (*itr).second;

		m_mapMonsterBookPage.clear();
	}


	// NPC ������ ����
	ZGetQuest()->Load();
	MQuestNPCCatalogue* pNPCCatalogue = ZGetQuest()->GetNPCCatalogue();
	if ( pNPCCatalogue == NULL)
		return false;


	int nTotalItemCount = 0;
	int nTotalCollectItemCount = 0;
	m_nTotalPageNum = 0;


	for ( map<MQUEST_NPC, MQuestNPCInfo*>::iterator itr = pNPCCatalogue->begin();  itr != pNPCCatalogue->end();  itr++)
	{
		// �ϵ� �ڵ� : �̺�Ʈ�� NPC�� ��쿣 ���ش�
		if ( (*itr).first >= 100)
			continue;


		// ����Ʈ ��� ����
		MQuestNPCInfo* pNPCInfo = (*itr).second;
		ZMonsterBookPageInfo* pMonsterBookPageInfo = new ZMonsterBookPageInfo;

		pMonsterBookPageInfo->m_nID			= (*itr).first;
		pMonsterBookPageInfo->m_strName		= pNPCInfo->szName;
		pMonsterBookPageInfo->m_nGrade		= (int)pNPCInfo->nGrade;
		pMonsterBookPageInfo->m_strDesc		= pNPCInfo->szDesc;
		pMonsterBookPageInfo->m_nHP			= pNPCInfo->nMaxHP;


		// ��ų ������ ����
		for ( int i = 0;  i < pNPCInfo->nSkills;  i++)
		{
			ZSkillManager* pSkillMgr = ZGetApplication()->GetSkillManager();
			map<int,ZSkillDesc*>::iterator itrSkill = pSkillMgr->find( pNPCInfo->nSkillIDs[ i]);
			if ( itrSkill != pSkillMgr->end())
			{
				string strSkill = (*itrSkill).second->szName;

				pMonsterBookPageInfo->m_Skill.push_back( strSkill);
			}
		}


		// ��� ������ ����� ����
		MQuestDropSet* pDropItem = ZGetQuest()->GetDropTable()->Find( pNPCInfo->nDropTableID);
		if ( pDropItem)
		{
			// ��� ������ ����
			int nDropItemCount = (int)pDropItem->GetQuestItems().size();
			nTotalItemCount += nDropItemCount;


			// ������ ������ ����
			int nCollectItemCount = 0;


			for ( set<int>::iterator itrItem = pDropItem->GetQuestItems().begin();  itrItem != pDropItem->GetQuestItems().end();  itrItem++)
			{
				ZDropItemInfo* pItemNode = new ZDropItemInfo;

				// �������� ȹ�������� �ִ��� �˻�
				ZMyQuestItemMap::iterator itr = ZGetMyInfo()->GetItemList()->GetQuestItemMap().find( *itrItem);
				if ( itr != ZGetMyInfo()->GetItemList()->GetQuestItemMap().end())
				{
					// ȹ���� ������ ���� �߰�
					nCollectItemCount++;


					// ������ ������ ����
					MQuestItemDesc* pQItemDesc = GetQuestItemDescMgr().FindQItemDesc( *itrItem);
					if ( pQItemDesc)
					{
						pItemNode->m_strName	= pQItemDesc->m_szQuestItemName;
						pItemNode->m_pIcon		= ZGetGameInterface()->GetQuestItemIcon( *itrItem, false);
					}
					else
					{
						MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc( *itrItem);
						if ( pItemDesc)
						{
							pItemNode->m_strName	= pItemDesc->m_pMItemName->Ref().m_szItemName;
							pItemNode->m_pIcon		= GetItemIconBitmap( pItemDesc);
						}
					}
				}
				// ȹ���� ���� ������ ???�� ǥ��
				else
				{
					pItemNode->m_strName	= "?????";
					pItemNode->m_pIcon		= MBitmapManager::Get( "slot_icon_unknown.tga");
				}


				// ��� �߰�
				pMonsterBookPageInfo->m_DropItem.push_back( pItemNode);
			}


			// ��ü ������ ������ ���� �߰�
			nTotalCollectItemCount += nCollectItemCount;
	
			
			// ������ �޼��� ����
			if ( nDropItemCount > 0)
				pMonsterBookPageInfo->m_fCompleteRate = (float)nCollectItemCount / (float)nDropItemCount * 100.0f;
		}


		// ��� �߰�
		m_mapMonsterBookPage.insert( ZMonsterBookPage::value_type( m_nTotalPageNum + 1, pMonsterBookPageInfo));
		m_nTotalPageNum++;
	}


	// ��ü �޼���
	if ( nTotalItemCount > 0)
		m_fCompleteRate	= (float)nTotalCollectItemCount / (float)nTotalItemCount * 100.0f;


	return true;
}
