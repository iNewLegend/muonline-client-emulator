/***********************************************************************
  ZStageInterface.cpp
  
  ��  �� : �������� �������̽��� �����ϴ� Ŭ����. �ڵ� �������� ���� ����
           �и�����(��� ���� ������ �� �и� ������. -_-;).
  �ۼ��� : 11, MAR, 2004
  �ۼ��� : �ӵ�ȯ
************************************************************************/


#include "stdafx.h"							// Include stdafx.h
#include "ZStageInterface.h"				// Include ZStageInterface.h
#include "ZStageSetting.h"					// Include ZStageSetting.h
#include "ZGameInterface.h"
#include "ZPlayerListBox.h"
#include "ZCombatMenu.h"
#include "ZShopEquipListbox.h"
#include "ZMyItemList.h"
#include "ZItemSlotView.h"
#include "ZMessages.h"
#include "ZLanguageConf.h"


/* �ؾ��� �͵�...

 1. ZStageSetting ���� ��ƾ�� ����� �Űܿ;� �ϴµ�...  -_-;
 2. ��ư UI�ʵ� ���� ����� �Űܿ;� �ϴµ� ���� �����ִ�...  -_-;
*/


/***********************************************************************
  ZStageInterface : public
  
  desc : ������
************************************************************************/
ZStageInterface::ZStageInterface( void)
{
	m_bPrevQuest = false;
	m_bDrawStartMovieOfQuest = false;
	m_pTopBgImg = NULL;
	m_pStageFrameImg = NULL;
	m_pItemListFrameImg = NULL;
	m_nListFramePos = 0;
	m_nStateSacrificeItemBox = 0;
	m_pRelayMapListFrameImg = NULL;
	m_nRelayMapListFramePos = 0;
	m_bRelayMapRegisterComplete = true;
	m_bEnableWidgetByRelayMap = true;
}


/***********************************************************************
  ~ZStageInterface : public
  
  desc : �Ҹ���
************************************************************************/
ZStageInterface::~ZStageInterface( void)
{
	if ( m_pTopBgImg != NULL)
	{
		delete m_pTopBgImg;
		m_pTopBgImg = NULL;
	}

	if ( m_pStageFrameImg != NULL)
	{
		delete m_pStageFrameImg;
		m_pStageFrameImg = NULL;
	}
}


/***********************************************************************
  Create : public
  
  desc : �ʱ�ȭ
************************************************************************/
void ZStageInterface::OnCreate( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// �ʱ�ȭ ���ְ�
	m_bPrevQuest = false;
	m_bDrawStartMovieOfQuest = false;
	m_nStateSacrificeItemBox = 0;		// Hide
	m_nGameType = MMATCH_GAMETYPE_DEATHMATCH_SOLO;
	m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();
	m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

	ReadSenarioNameXML();

	MPicture* pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage0");
	if ( pPicture)
		pPicture->SetOpacity( 255);
	pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage1");
	if ( pPicture)
		pPicture->SetOpacity( 255);

	pPicture = (MPicture*)pResource->FindWidget( "Stage_MainBGTop");
	if ( pPicture)
		pPicture->SetBitmap( MBitmapManager::Get( "main_bg_t.png"));
	pPicture = (MPicture*)pResource->FindWidget( "Stage_FrameBG");
	if ( pPicture)
	{
		m_pStageFrameImg = new MBitmapR2;
		((MBitmapR2*)m_pStageFrameImg)->Create( "stage_frame.png", RGetDevice(), "interface/loadable/stage_frame.png");

		if ( m_pStageFrameImg != NULL)
			pPicture->SetBitmap( m_pStageFrameImg->GetSourceBitmap());
	}
	pPicture = (MPicture*)pResource->FindWidget( "Stage_ItemListBG");
	if ( pPicture)
	{
		m_pItemListFrameImg = new MBitmapR2;
		((MBitmapR2*)m_pItemListFrameImg)->Create( "itemlistframe.tga", RGetDevice(), "interface/loadable/itemlistframe.tga");

		if ( m_pItemListFrameImg != NULL)
			pPicture->SetBitmap( m_pItemListFrameImg->GetSourceBitmap());
	}
	MWidget* pWidget = (MWidget*)pResource->FindWidget( "Stage_ItemListView");
	if ( pWidget)
	{
		MRECT rect;
		rect = pWidget->GetRect();
		rect.x = -rect.w;
		m_nListFramePos = rect.x;
		pWidget->SetBounds( rect);
	}
	MLabel* pLabel = (MLabel*)pResource->FindWidget( "Stage_SenarioName");
	if ( pLabel)
		pLabel->SetText( "");
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_SenarioNameImg");
	if ( pWidget)
		pWidget->Show( false);
	MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_SacrificeItemListbox");
	if ( pListBox)
		pListBox->RemoveAll();
	MTextArea* pDesc = (MTextArea*)pResource->FindWidget( "Stage_ItemDesc");
	if ( pDesc)
	{
		pDesc->SetTextColor( MCOLOR(0xFF808080));
		//pDesc->SetText( "�������� ȭ�� �߾ӿ� �ִ� �ΰ��� ���ܿ� ����������ν� ���� ������ ������ �� �ֽ��ϴ�.");
		char szText[256];
		sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
		pDesc->SetText(szText);
	}

	{ // �����̸� "RelayMap" �ʱ�ȭ
		pPicture = (MPicture*)pResource->FindWidget( "Stage_RelayMapListBG" );
		if(pPicture)
		{ // f�����̸� ����Ʈ ��� �̹���
			m_pRelayMapListFrameImg = new MBitmapR2;
			((MBitmapR2*)m_pRelayMapListFrameImg)->Create( "relaymaplistframe.tga", RGetDevice(), "interface/loadable/relaymaplistframe.tga");

			if(m_pRelayMapListFrameImg != NULL)
				pPicture->SetBitmap(m_pRelayMapListFrameImg->GetSourceBitmap());
		}
		MWidget* pWidget = (MWidget*)pResource->FindWidget( "Stage_RelayMapListView");
		if ( pWidget)
		{
			MRECT rect;
			rect = pWidget->GetRect();
			rect.x = -rect.w;
			m_nRelayMapListFramePos = rect.x;
			pWidget->SetBounds( rect);
		}

		MComboBox* pCombo = (MComboBox*)pResource->FindWidget("Stage_RelayMapType");			
		if ( pCombo)
			pCombo->CloseComboBoxList();
		pCombo = (MComboBox*)pResource->FindWidget("Stage_RelayMapRepeatCount");					
		if ( pCombo)
			pCombo->CloseComboBoxList();

		MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_RelayMapListbox");
		if ( pListBox)
			pListBox->RemoveAll();
		pListBox = (MListBox*)pResource->FindWidget( "Stage_MapListbox");
		if ( pListBox)
			pListBox->RemoveAll();
	}

	ZApplication::GetGameInterface()->ShowWidget( "Stage_Flame0", false);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_Flame1", false);

	// ���� ����� ä�� �� �����ٰ� �ٸ� �� ���� ���� ����°� ���� 
	MComboBox* pCombo = (MComboBox*)pResource->FindWidget("StageType");			
	if ( pCombo)
		pCombo->CloseComboBoxList();

	// �ʸ���Ʈ �� ä�� �� �����ٰ� �ٸ� �� ���� ������ �ƴѵ��� �� �ٲ����� ���� ����
	pCombo = (MComboBox*)pResource->FindWidget("MapSelection");					
	if ( pCombo)
		pCombo->CloseComboBoxList();


	// ä�� ����Ʈ �ڽ��� �ݾƹ���
	pWidget = (MWidget*)pResource->FindWidget( "ChannelListFrame");
	if ( pWidget)
		pWidget->Show( false);


	// ȭ�� ���� �ѹ� ���ֻ�~
	UpdateSacrificeItem();
	SerializeSacrificeItemListBox();

	// QL �ʱ�ȭ
	OnResponseQL( 0);


	ZApplication::GetGameInterface()->ShowWidget( "Stage_Lights0", false);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_Lights1", false);
}


/***********************************************************************
  OnDestroy : public
  
  desc : ������ Ȥ�� �ý����� ��û���κ��� �������� ȭ���� ���� �����ϴ� �Լ�
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnDestroy( void)
{
	ZApplication::GetGameInterface()->ShowWidget( "Stage", false);

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_MainBGTop");
	if ( pPicture)
		pPicture->SetBitmap( MBitmapManager::Get( "main_bg_t.png"));
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_FrameBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemListBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapListBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	if ( m_pTopBgImg != NULL)
	{
		delete m_pTopBgImg;
		m_pTopBgImg = NULL;
	}
	if ( m_pStageFrameImg != NULL)
	{
		delete m_pStageFrameImg;
		m_pStageFrameImg = NULL;
	}
	if ( m_pItemListFrameImg != NULL)
	{
		delete m_pItemListFrameImg;
		m_pItemListFrameImg = NULL;
	}
	if (m_pRelayMapListFrameImg != NULL)
	{
		delete m_pRelayMapListFrameImg;
		m_pRelayMapListFrameImg = NULL;
	}

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

//	m_SenarioDesc.clear();
}

void ZStageInterface::OnStageCharListSettup()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	for ( MStageCharSettingList::iterator itor = ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.begin();
		itor != ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.end();  ++itor) 
	{
		MSTAGE_CHAR_SETTING_NODE* pCharNode = (*itor);

		ZPlayerListBox* pItem = (ZPlayerListBox*)pResource->FindWidget( "StagePlayerList_");
		if ( pItem)
		{
			bool bMaster = false;

			if ( ZGetGameClient()->GetMatchStageSetting()->GetMasterUID() == pCharNode->uidChar)
				bMaster = true;

			pItem->UpdatePlayer( pCharNode->uidChar,(MMatchObjectStageState)pCharNode->nState,bMaster,MMatchTeam(pCharNode->nTeam));
		}
	}
}


/***********************************************************************
  OnStageInterfaceSettup : public
  
  desc : ������ Ȥ�� �ý����� ��û���κ��� �������� ȭ���� ���� �����ϴ� �Լ�
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnStageInterfaceSettup( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	ZStageSetting::InitStageSettingGameType();
/*
	// �� ������ �����Ѵ�.
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( "MapSelection");
	if ( pCB)
	{
		int nSelected = pCB->GetSelIndex();

		InitMaps( pCB);

		if ( nSelected >= pCB->GetCount())
			nSelected = pCB->GetCount() - 1;

		pCB->SetSelIndex( nSelected);
	}
*/

	// CharListView�� Add, Remove, Update�� ZGameClient::OnObjectCache ���� �����Ѵ�.
	MSTAGE_CHAR_SETTING_NODE* pMyCharNode = NULL;
	bool bMyReady = false;		// Ready �������� �ƴ���...
	for ( MStageCharSettingList::iterator itor = ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.begin();
		itor != ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.end();  ++itor) 
	{
		MSTAGE_CHAR_SETTING_NODE* pCharNode = (*itor);

		// �� �ڽ��� ���
		if ( pCharNode->uidChar == ZGetGameClient()->GetPlayerUID()) 
		{
			pMyCharNode = pCharNode;
			if (pMyCharNode->nState == MOSS_READY)
				bMyReady = true;
			else
				bMyReady = false;
		}

		ZPlayerListBox* pItem = (ZPlayerListBox*)pResource->FindWidget( "StagePlayerList_");
		if ( pItem)
		{
			bool bMaster = false;

			if ( ZGetGameClient()->GetMatchStageSetting()->GetMasterUID() == pCharNode->uidChar)
				bMaster = true;
			
			pItem->UpdatePlayer( pCharNode->uidChar,(MMatchObjectStageState)pCharNode->nState,bMaster,MMatchTeam(pCharNode->nTeam));
		}
	}

	// ���������� ��ư ����(���ӽ���, ����, �غ�Ϸ�)�� �����Ѵ�.
	ChangeStageButtons( ZGetGameClient()->IsForcedEntry(), ZGetGameClient()->AmIStageMaster(), bMyReady);

	// ����������...
	ChangeStageGameSetting( ZGetGameClient()->GetMatchStageSetting()->GetStageSetting());
	
	// ���� ����� ��쿡...
	if ( !ZGetGameClient()->AmIStageMaster() && ( ZGetGameClient()->IsForcedEntry()))
	{
		if ( pMyCharNode != NULL)
			ChangeStageEnableReady( bMyReady);
	}

	// ���� �������� ���Դµ� �ٸ� ��� �� ������ ���� ������ �Ǿ��ٸ� ���Ը�� ����
	if ( (ZGetGameClient()->AmIStageMaster() == true) && ( ZGetGameClient()->IsForcedEntry()))
	{
		if ( ZGetGameClient()->GetMatchStageSetting()->GetStageState() == STAGE_STATE_STANDBY)
		{
			ZGetGameClient()->ReleaseForcedEntry();

			// �������̽�����
			ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", true);	// �漳�� ��ư
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", true);			// �ʼ��� �޺��ڽ�
			ZApplication::GetGameInterface()->EnableWidget( "StageType", true);				// ���ӹ�� �޺��ڽ�
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", true);		// �ִ��ο� �޺��ڽ�
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", true);		// ���Ƚ�� �޺��ڽ�
		}
		else	// �������ε� �ٸ������ �������̸� �������̽� Disable
		{
			// �������̽�����
			ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);	// �漳�� ��ư
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", false);			// �ʼ��� �޺��ڽ�
			ZApplication::GetGameInterface()->EnableWidget( "StageType", false);			// ���ӹ�� �޺��ڽ�
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);		// �ִ��ο� �޺��ڽ�
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", false);		// ���Ƚ�� �޺��ڽ�
		}
	}

	/// �����̸��̸鼭 ���������� ��� �������϶� ���� ��ư�� ��� OFF �����ش�.
	if(	ZGetGameClient()->AmIStageMaster() && 
		!m_bEnableWidgetByRelayMap && 
		ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->bIsRelayMap)
	{
		ZApplication::GetGameInterface()->EnableWidget( "GameStart", false);
		// ZApplication::GetGameInterface()->EnableWidget( "StageReady", false);	// �غ�Ϸ��ư(��ȹ�� ���� ���氡��)

		// �������̽�����
		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller",	false);		// �漳�� ��ư
		ZApplication::GetGameInterface()->EnableWidget( "MapSelection",			false);		// �ʼ��� �޺��ڽ�
		ZApplication::GetGameInterface()->EnableWidget( "StageType",			false);		// ���ӹ�� �޺��ڽ�
		ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer",		false);		// �ִ��ο� �޺��ڽ�
		ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount",		false);		// ���Ƚ�� �޺��ڽ�

		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller",		false);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", false);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapType",		false);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapRepeatCount", false);
	}


	// ȭ�� ����� �� �̹��� �����ϱ�
	MPicture* pPicture = 0;
	MBitmap* pBitmap = 0;
	char szMapName[256];
 	pPicture = (MPicture*)pResource->FindWidget( "Stage_MainBGTop");
	if ( pPicture)
	{
		if( 0 == strcmp(MMATCH_MAPNAME_RELAYMAP, ZGetGameClient()->GetMatchStageSetting()->GetMapName()))
		{
			sprintf( szMapName, "interface/loadable/%s", MGetMapDescMgr()->GetMapImageName( MMATCH_DEFAULT_STAGESETTING_MAPNAME));
		}
		else
			sprintf( szMapName, "interface/loadable/%s", MGetMapDescMgr()->GetMapImageName( ZGetGameClient()->GetMatchStageSetting()->GetMapName()));

		if ( m_pTopBgImg != NULL)
		{
			delete m_pTopBgImg;
			m_pTopBgImg = NULL;
		}

		m_pTopBgImg = new MBitmapR2;
		((MBitmapR2*)m_pTopBgImg)->Create( "TopBgImg.png", RGetDevice(), szMapName);

		if ( m_pTopBgImg != NULL)
			pPicture->SetBitmap( m_pTopBgImg->GetSourceBitmap());
	}
	
	// ����â�� ���ӹ��� �����ϱ�
	MLabel* pLabel = (MLabel*)pResource->FindWidget( "StageNameLabel");
	if ( pLabel != 0)
	{
		char szStr[ 256];
		sprintf( szStr, "%s > %s > %03d:%s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_STAGE), ZGetGameClient()->GetStageNumber(), ZGetGameClient()->GetStageName());
		pLabel->SetText( szStr);
	}

	// ���ϴ� ��Ʈ�������� ���� �ٲٱ�
#define SDM_COLOR			MCOLOR(255,0,0)
#define TDM_COLOR			MCOLOR(0,255,0)
#define SGD_COLOR			MCOLOR(0,0,255)
#define TGD_COLOR			MCOLOR(255,255,0)
#define ASSASIN_COLOR		MCOLOR(255,0,255)
#define TRAINING_COLOR		MCOLOR(0,255,255)
#define QUEST_COLOR			MCOLOR(255,255,255)
#define SURVIVAL_COLOR		MCOLOR(255,255,255)

	MCOLOR color;
	switch ( ZGetGameClient()->GetMatchStageSetting()->GetGameType() )
	{	
		case MMATCH_GAMETYPE_ASSASSINATE:
			color = ASSASIN_COLOR;
			break;

		case MMATCH_GAMETYPE_DEATHMATCH_SOLO:
			color = SDM_COLOR;
			break;

		case MMATCH_GAMETYPE_DEATHMATCH_TEAM:
		case MMATCH_GAMETYPE_DEATHMATCH_TEAM2:
			color = TDM_COLOR;
			break;

		case MMATCH_GAMETYPE_GLADIATOR_SOLO:
			color = SGD_COLOR;
			break;

		case MMATCH_GAMETYPE_GLADIATOR_TEAM:
			color = TGD_COLOR;
			break;

		case MMATCH_GAMETYPE_TRAINING:
			color = TRAINING_COLOR;
			break;

#ifdef _QUEST
		case MMATCH_GAMETYPE_SURVIVAL:
			color = QUEST_COLOR;
			break;

		case MMATCH_GAMETYPE_QUEST:
			color = SURVIVAL_COLOR;
			break;
#endif
		case MMATCH_GAMETYPE_BERSERKER:
			color = SDM_COLOR;
			break;

		case MMATCH_GAMETYPE_DUEL:
			color = SDM_COLOR;
			break;
		case MMATCH_GAMETYPE_DUELTOURNAMENT:
			color = SDM_COLOR;
			break;

		default:
			_ASSERT(0);
			color = MCOLOR(255,255,255,255);
	}
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripBottom");
	if(pPicture != NULL && !pPicture->IsAnim())
	{		
        pPicture->SetBitmapColor( color );
		if(!(pPicture->GetBitmapColor().GetARGB() == pPicture->GetReservedBitmapColor().GetARGB()))
			pPicture->SetAnimation( 2, 700.0f);
	}
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripTop");
	if(pPicture != NULL && !pPicture->IsAnim())	
	{
		pPicture->SetBitmapColor( color );
		if(!(pPicture->GetBitmapColor().GetARGB() == pPicture->GetReservedBitmapColor().GetARGB()))
			pPicture->SetAnimation( 3, 700.0f);		
	}
}

void ZStageInterface::SetStageRelayMapImage()
{
	MPicture* pPicture = 0;
	MBitmap* pBitmap = 0;
	char szMapName[256];
	pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_MainBGTop");
	if(!pPicture) return;
	MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelayMapListBox == NULL) return;
	if( 0 < pRelayMapListBox->GetCount())
	{
		sprintf( szMapName, "interface/loadable/%s", MGetMapDescMgr()->GetMapImageName( pRelayMapListBox->GetString(pRelayMapListBox->GetStartItem())));
		if ( m_pTopBgImg != NULL)
		{
			delete m_pTopBgImg;
			m_pTopBgImg = NULL;
		}

		m_pTopBgImg = new MBitmapR2;
		((MBitmapR2*)m_pTopBgImg)->Create( "TopBgImg.png", RGetDevice(), szMapName);

		if ( m_pTopBgImg != NULL)
			pPicture->SetBitmap( m_pTopBgImg->GetSourceBitmap());
	}
}

void ZStageInterface::SetEnableWidgetByRelayMap(bool b)
{
	if(!ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->bIsRelayMap)
	{	// �����̸��� �ƴϸ� �����̸ʿ� ���� ��Ȱ��ȭ ó���� �� �ʿ� ����.
		m_bEnableWidgetByRelayMap = true;
		return;
	}

	m_bEnableWidgetByRelayMap = b;
}

/***********************************************************************
  ChangeStageGameSetting : public
  
  desc : �̰͵� ���� ���� �������̽��� �����ϴ°� ������... ���̷��� ���� ������������? -_-;
         �ַ� ȭ���� ��ü���� UI�� �����Ѵ�.
  arg  : pSetting = �������� ���� ����
  ret  : none
************************************************************************/
void ZStageInterface::ChangeStageGameSetting( const MSTAGE_SETTING_NODE* pSetting)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	m_nGameType = pSetting->nGameType;

	// Set map name
	SetMapName( pSetting->szMapName);

	// Is team game?
	ZApplication::GetGameInterface()->m_bTeamPlay = ZGetGameTypeManager()->IsTeamGame( pSetting->nGameType);


	// ���� ��� ���� Ȯ��
	MComboBox* pCombo = (MComboBox*)pResource->FindWidget( "StageObserver");
	MButton* pObserverBtn = (MButton*)pResource->FindWidget( "StageObserverBtn");
	MLabel* pObserverLabel = (MLabel*)pResource->FindWidget( "StageObserverLabel");
	if ( pCombo && pObserverBtn && pObserverLabel)
	{
		if ( pCombo->GetSelIndex() == 1)
		{
			pObserverBtn->SetCheck( false);
			pObserverBtn->Enable( false);
			pObserverLabel->Enable( false);
		}
		else
		{
			pObserverBtn->Enable( true);
			pObserverLabel->Enable( true);
		}
	}

	// û��, ȫ�� ���� ����
	ZApplication::GetGameInterface()->UpdateBlueRedTeam();

	// ���� ��Ŀ� ���� UI�� �����Ѵ�
	MAnimation* pAniMapImg = (MAnimation*)pResource->FindWidget( "Stage_MapNameBG");
	bool bQuestUI = false;
	if ( (pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||			// ������ġ �������̰ų�...
		 (pSetting->nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) ||				// Į�� �������̰ų�...
		 (pSetting->nGameType == MMATCH_GAMETYPE_BERSERKER) ||					// ����Ŀ����̰ų�...
		 (pSetting->nGameType == MMATCH_GAMETYPE_TRAINING) ||					// Ʈ���̴��̰ų�...
		 (pSetting->nGameType == MMATCH_GAMETYPE_DUEL))							// ����� �̸�...
	{
		// �� �̸� ��� �̹��� ��ȯ
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame( 0);

		// ����Ʈ UI ����
		bQuestUI = false;
	}
	else if ( (pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||		// ������ġ �����̰ų�...
		(pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||			// ���ѵ�����ġ �����̰ų�...
		 (pSetting->nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||				// Į�� �����̰ų�...
		 (pSetting->nGameType == MMATCH_GAMETYPE_ASSASSINATE))					// �ϻ��� �̸�...
	{
		// �� �̸� ��� �̹��� ��ȯ
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame( 1);

		// ����Ʈ UI ����
		bQuestUI = false;
	}
	else if ( pSetting->nGameType == MMATCH_GAMETYPE_SURVIVAL)					// �����̹� ����̸�...
	{
		// �� �̸� ��� �̹��� ��ȯ
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame( 0);

		// ����Ʈ UI ����
		bQuestUI = false;
	}
	else if ( pSetting->nGameType == MMATCH_GAMETYPE_QUEST)						// ����Ʈ ����̸�...
	{
		// �� �̸� ��� �̹��� ��ȯ
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame( 2);

		// ����Ʈ UI ����
		bQuestUI = true;
	}

	// �ʼ��ý� �����̸��̸� �����̸� ����Ʈ �ڽ��� �����ش�.
	 if(pSetting->bIsRelayMap)
		OpenRelayMapBox();
	else
		HideRelayMapBox();

	// ����Ʈ UI ����
	ZApplication::GetGameInterface()->ShowWidget( "Stage_SacrificeItemImage0", bQuestUI);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_SacrificeItemImage1", bQuestUI);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_QuestLevel", bQuestUI);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_QuestLevelBG", bQuestUI);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_SacrificeItemButton0", bQuestUI);
	ZApplication::GetGameInterface()->ShowWidget( "Stage_SacrificeItemButton1", bQuestUI);

	if ( m_bPrevQuest != bQuestUI)
	{
		ZApplication::GetGameInterface()->ShowWidget( "Stage_Lights0", bQuestUI);
		ZApplication::GetGameInterface()->ShowWidget( "Stage_Lights1", bQuestUI);

		m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

		UpdateSacrificeItem();

		if ( bQuestUI)
		{
//			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);
			ZPostRequestSacrificeSlotInfo( ZGetGameClient()->GetPlayerUID());
			ZPostRequestQL( ZGetGameClient()->GetPlayerUID());
			OpenSacrificeItemBox();
		}
		else
		{
			MLabel* pLabel = (MLabel*)pResource->FindWidget( "Stage_SenarioName");
			if ( pLabel)
				pLabel->SetText( "");
			ZApplication::GetGameInterface()->ShowWidget( "Stage_SenarioNameImg", false);
//			if (ZGetGameClient()->AmIStageMaster())
//				ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", true);

			HideSacrificeItemBox();
		}

		m_bPrevQuest = !m_bPrevQuest;
	}

	if ( (pSetting->nGameType == MMATCH_GAMETYPE_SURVIVAL) || (pSetting->nGameType == MMATCH_GAMETYPE_QUEST))
		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);


	// ���� ���� �޺��ڽ� ���̱�
//	bool bShowRound = true;
//	if ( ( pSetting->nGameType == MMATCH_GAMETYPE_SURVIVAL) || ( pSetting->nGameType == MMATCH_GAMETYPE_QUEST))
//		bShowRound = false;
	
//	ZApplication::GetGameInterface()->ShowWidget( "StageRoundCountLabelBG", bShowRound);
//	ZApplication::GetGameInterface()->ShowWidget( "StageRoundCountLabel", bShowRound);
//	ZApplication::GetGameInterface()->ShowWidget( "StageRoundCount", bShowRound);


	// ���� or Kill
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageRoundCountLabel");
	if ( pWidget)
	{
		if ((pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||		// ��������ġ �ͽ�Ʈ���� ������ Kill�� ǥ��.... by kammir 20081117
			(pSetting->nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_TRAINING) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_BERSERKER) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_DUEL))
			pWidget->SetText( ZMsg(MSG_WORD_KILL));

		else
			pWidget->SetText( ZMsg(MSG_WORD_ROUND));
	}


	// ������Ʈ ������Ʈ
	ZStageSetting::ShowStageSettingDialog( pSetting, false);


	// ������ �޴� ���� - ���ϴ� �κ����� �˼� ����...
	// ������ �޴����� ����Ʈ�� �����̹��϶� �������� ������ ��ư ��Ȱ��ȭ ��Ŵ
#ifdef _QUEST
	if ( ZGetGameTypeManager()->IsQuestDerived( pSetting->nGameType) || ZGetGameClient()->IsLadderGame())
		ZApplication::GetGameInterface()->GetCombatMenu()->EnableItem( ZCombatMenu::ZCMI_BATTLE_EXIT, false);
	else
		ZApplication::GetGameInterface()->GetCombatMenu()->EnableItem( ZCombatMenu::ZCMI_BATTLE_EXIT, true);
#endif
}


/***********************************************************************
  ChangeStageButtons : public
  
  desc : �������� ���� ��ư��(���ӽ���, ����, �غ�Ϸ�)�� ���¸� �����Ѵ�.
         ���� ������ ���õ� ������ UI�� �����Ѵ�.
  arg  : bForcedEntry = ���� ����(true or false)
         bMaster = ���� ����(true or false)
		 bReady = �غ� �Ϸ� ����(true or false)
  ret  : none
************************************************************************/
void ZStageInterface::ChangeStageButtons( bool bForcedEntry, bool bMaster, bool bReady)
{
	if ( bForcedEntry)	// ���� ���
	{
		ZApplication::GetGameInterface()->ShowWidget( "GameStart", false);
		ZApplication::GetGameInterface()->ShowWidget( "StageReady", false);

		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame", true);
		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame2", true);

		ChangeStageEnableReady( false);
	}
	else
	{
		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame", false);
		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame2", false);

		ZApplication::GetGameInterface()->ShowWidget( "GameStart", bMaster);
		ZApplication::GetGameInterface()->ShowWidget( "StageReady", !bMaster);

		if ( bMaster)
		{
			// ���� �����̶�� �漼���� �ٲ� �� �ִ� ������ ������ (UI enable).
			// �׷��� �׷����� �ȵǴ� ��찡 �ִµ� �װ� üũ�Ѵ�.
			bool bMustDisableUI_despiteMaster = false;
			
			// ����Ʈ��忡�� ������ ���ӽ����� ������ ��������� ��Ÿ�� �ִϸ��̼� ���� ������ �����θ� kick�ؼ�
			// �ٸ� ����� ���� ������ ���� ���, ���� ���������� ���̻� �漼���� �ٲ� ���� ����
			if ( m_bDrawStartMovieOfQuest)
				bMustDisableUI_despiteMaster = true;


			if (bMustDisableUI_despiteMaster)
				ChangeStageEnableReady( true);	// UI disable
			else
				ChangeStageEnableReady( false);	// UI enable
		}
		else
			ChangeStageEnableReady( bReady);
	}
}


/***********************************************************************
  ChangeStageEnableReady : public
  
  desc : ���������� �������̽�(��ư enable��)�� �Ѱų� ���� �Լ�
  arg  : true(=interface enable) or false(=interface disable)
  ret  : none
************************************************************************/
void ZStageInterface::ChangeStageEnableReady( bool bReady)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	ZApplication::GetGameInterface()->EnableWidget( "GameStart", !bReady);

	ZApplication::GetGameInterface()->EnableWidget( "StageTeamBlue",  !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "StageTeamBlue2", !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "StageTeamRed",  !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "StageTeamRed2", !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "Lobby_StageExit", !bReady);

	if ( (m_nGameType == MMATCH_GAMETYPE_SURVIVAL) || (m_nGameType == MMATCH_GAMETYPE_QUEST))
	{
		ZApplication::GetGameInterface()->EnableWidget( "Stage_SacrificeItemListbox", !bReady);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_PutSacrificeItem",     !bReady);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_SacrificeItemButton0", !bReady);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_SacrificeItemButton1", !bReady);
		if ( ZGetGameClient()->AmIStageMaster())
		{
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageType", !bReady);
		}
//		ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);
		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);
	}
	else
	{
		if ( ZGetGameClient()->AmIStageMaster())
		{
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageType", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapType", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapRepeatCount", !bReady);
		}
		else
		{
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageType", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapType", false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapRepeatCount", false);
		}
	}
    
	BEGIN_WIDGETLIST( "Stage_OptionFrame", pResource, MButton*, pButton);
	pButton->Enable( !bReady);
	END_WIDGETLIST();

	BEGIN_WIDGETLIST( "EquipmentCaller", pResource, MButton*, pButton);
	pButton->Enable( !bReady);
	END_WIDGETLIST();

	ZApplication::GetGameInterface()->EnableWidget( "StagePlayerList_", !bReady);
}


/***********************************************************************
  SetMapName : public
  
  desc : �� ���� �޺��ڽ��� �� �̸��� �ϳ��� ��Ͻ�Ų��.
  arg  : szMapName = ��Ͻ�ų �� �̸�
  ret  : none
************************************************************************/
void ZStageInterface::SetMapName( const char* szMapName)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if ( szMapName == NULL)
		return;

	MComboBox* pMapCombo = (MComboBox*)pResource->FindWidget( "MapSelection");
	if ( pMapCombo)
	{
		// �ϴ� �ӽ� �ϵ��ڵ�(�쿡��~ ��.��)
//		if ( m_nGameType == MMATCH_GAMETYPE_QUEST)
//			pMapCombo->SetText( "Mansion");
//		else
			pMapCombo->SetText( szMapName);
	}
}




/*
	���⼭���� ���� �߰��� ����...

	�ظ��Ѱ� �����ʳ� �ٸ� ������ �ڵ����� ȣ��ǵ��� �س����� ���� �׽�Ʈ�� �����
	�Ϻ��Ѱ� �ƴ�...  -_-;
*/



/***********************************************************************
  OpenSacrificeItemBox : public
  
  desc : ��� ������ ���� â ����
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OpenSacrificeItemBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxOpen");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxClose");
	if ( pButton)
		pButton->Show( true);

	m_nStateSacrificeItemBox = 2;
	GetSacrificeItemBoxPos();
}


/***********************************************************************
  CloseSacrificeItemBox : public
  
  desc : ��� ������ ���� â �ݱ�
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::CloseSacrificeItemBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	m_nStateSacrificeItemBox = 1;
	GetSacrificeItemBoxPos();
}


/***********************************************************************
  HideSacrificeItemBox : public
  
  desc : ��� ������ ���� â ���߱�
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::HideSacrificeItemBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	m_nStateSacrificeItemBox = 0;
	GetSacrificeItemBoxPos();
}


/***********************************************************************
  HideSacrificeItemBox : public
  
  desc : ��� ������ ���� â ��ġ ���ϱ�
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::GetSacrificeItemBoxPos( void)
{
//#ifdef _DEBUG
//	m_nListFramePos = 0;
//	return;
//#endif

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemListView");
	if ( pWidget)
	{
		MRECT rect;

		switch ( m_nStateSacrificeItemBox)
		{
			case 0 :		// Hide
				rect = pWidget->GetRect();
				m_nListFramePos = -rect.w;
				break;

			case 1 :		// Close
				rect = pWidget->GetRect();
				m_nListFramePos = -rect.w + ( rect.w * 0.14);
				break;

			case 2 :		// Open
				m_nListFramePos = 0;
				break;
		}
	}
}


/***********************************************************************
  OnSacrificeItem0 : public
  
  desc : ��� ������0�� ������ ��
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnSacrificeItem0( void)
{
}


/***********************************************************************
  OnSacrificeItem1 : public
  
  desc : ��� ������1�� ������ ��
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnSacrificeItem1( void)
{
}


/***********************************************************************
  UpdateSacrificeItem : protected
  
  desc : ����� ��� ������ �̹���, �������� ������Ʈ ��.
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::UpdateSacrificeItem( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// �������� ������ �ִ� ��� ������ �̹��� ����
	for ( int i = SACRIFICEITEM_SLOT0;  i <= SACRIFICEITEM_SLOT1;  i++)
	{
		char szWidgetNameItem[ 128];
		sprintf( szWidgetNameItem, "Stage_SacrificeItemImage%d", i);
		MPicture* pPicture = (MPicture*)pResource->FindWidget( szWidgetNameItem);
		if ( pPicture)
		{
			if ( m_SacrificeItem[ i].IsExist())
			{
				pPicture->SetBitmap( m_SacrificeItem[ i].GetIconBitmap());
				char szMsg[ 128];
				MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache( m_SacrificeItem[ i].GetUID());
				if ( pObjCache)
					sprintf( szMsg, "%s (%s)", m_SacrificeItem[ i].GetName(), pObjCache->GetName());
				else
					strcpy( szMsg, m_SacrificeItem[ i].GetName());
				pPicture->AttachToolTip( szMsg);
			}
			else
			{
				pPicture->SetBitmap( NULL);
				pPicture->DetachToolTip();
			}
		}
	}
}


/***********************************************************************
  SerializeSacrificeItemListBox : public
  
  desc : ��� ������ ����Ʈ �ڽ��� �ڷḦ �޴´�.
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::SerializeSacrificeItemListBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_SacrificeItemListbox");
	if ( !pListBox)
		return;

	int nStartIndex  = pListBox->GetStartItem();
	int nSelectIndex = pListBox->GetSelIndex();
	pListBox->RemoveAll();

	// ����Ʈ�� �߰�
	for ( MQUESTITEMNODEMAP::iterator questitem_itor = ZGetMyInfo()->GetItemList()->GetQuestItemMap().begin();
		  questitem_itor != ZGetMyInfo()->GetItemList()->GetQuestItemMap().end();
		  questitem_itor++)
	{
		ZMyQuestItemNode* pItemNode = (*questitem_itor).second;
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( pItemNode->GetItemID());
		if ( pItemDesc)
		{
			int nCount = pItemNode->m_nCount;
			if ( m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist() &&
				 (m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetUID() == ZGetGameClient()->GetPlayerUID()) &&
				 (pItemDesc->m_nItemID == m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetItemID()))
				nCount--;
			if ( m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist() &&
				 (m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetUID() == ZGetGameClient()->GetPlayerUID()) &&
				 (pItemDesc->m_nItemID == m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetItemID()))
				nCount--;

			if ( pItemDesc->m_bSecrifice && (nCount > 0))		// ��� �����۸� �߰�
			{
				pListBox->Add( new SacrificeItemListBoxItem( pItemDesc->m_nItemID,
															 ZApplication::GetGameInterface()->GetQuestItemIcon( pItemDesc->m_nItemID, true),
															 pItemDesc->m_szQuestItemName,
															 nCount,
															 pItemDesc->m_szDesc));
			}
		}
	}

	MWidget* pWidget = pResource->FindWidget( "Stage_NoItemLabel");
	if ( pWidget)
	{
		if ( pListBox->GetCount() > 0)
			pWidget->Show( false);
		else
			pWidget->Show( true);
	}

	pListBox->SetStartItem( nStartIndex);
	pListBox->SetSelIndex( min( (pListBox->GetCount() - 1), nSelectIndex));
}


/***********************************************************************
  OnDropSacrificeItem : public
  
  desc : ��� ������ ����
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnDropSacrificeItem( int nSlotNum)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pReadyBtn = (MButton*)pResource->FindWidget("StageReady");
	if(pReadyBtn) 
		if(pReadyBtn->GetCheck()) 
		{	// �غ� �Ϸ��ư�� �����ִٸ�
			ZApplication::GetStageInterface()->ChangeStageEnableReady(pReadyBtn->GetCheck());	// �ٸ� UI�� ���ش�.
			return;	// ����������� ���Կ� �ø��� �ʴ´�.
		}

	MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_SacrificeItemListbox");
	if ( !pListBox || (pListBox->GetSelIndex() < 0))
		return;

	SacrificeItemListBoxItem* pItemDesc = (SacrificeItemListBoxItem*)pListBox->Get( pListBox->GetSelIndex());
	if ( pItemDesc)
	{
		MTextArea* pDesc = (MTextArea*)pResource->FindWidget( "Stage_ItemDesc");

		// ������ ��������� ������ �ø�
		if ( ! m_SacrificeItem[ nSlotNum].IsExist())
		{
			ZPostRequestDropSacrificeItem( ZGetGameClient()->GetPlayerUID(), nSlotNum, pItemDesc->GetItemID());
			char szText[256];
			sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
			pDesc->SetText(szText);
			//if ( pDesc)
			//	pDesc->Clear();
		}

		// ������ ������� ������...
		else
		{
			if ( (m_SacrificeItem[ nSlotNum].GetUID()    != ZGetGameClient()->GetPlayerUID()) ||
				 (m_SacrificeItem[ nSlotNum].GetItemID() != pItemDesc->GetItemID()))
				ZPostRequestDropSacrificeItem( ZGetGameClient()->GetPlayerUID(), nSlotNum, pItemDesc->GetItemID());

			char szText[256];
			sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
			pDesc->SetText(szText);
			//if ( pDesc)
			//	pDesc->Clear();
		}
	}
}


/***********************************************************************
  OnRemoveSacrificeItem : public
  
  desc : ��� ������ ����
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnRemoveSacrificeItem( int nSlotNum)
{
	if ( !m_SacrificeItem[ nSlotNum].IsExist())
		return;

	ZPostRequestCallbackSacrificeItem( ZGetGameClient()->GetPlayerUID(),
									   nSlotNum,
									   m_SacrificeItem[ nSlotNum].GetItemID());

	MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
	char szText[256];
	sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
	pDesc->SetText(szText);
	//if ( pDesc)
	//	pDesc->Clear();
}


/***********************************************************************
  MSacrificeItemListBoxListener
  
  desc : ��� ������ ����Ʈ �ڽ� ������
************************************************************************/
class MSacrificeItemListBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// On select
		if ( MWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
		{
			MListBox* pListBox = (MListBox*)pWidget;

			// ������ ���� ������Ʈ
			SacrificeItemListBoxItem* pItemDesc = (SacrificeItemListBoxItem*)pListBox->GetSelItem();
			MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
			if ( pItemDesc && pDesc)
			{
				char szCount[ 128];
				sprintf( szCount, "%s : %d", ZMsg( MSG_WORD_QUANTITY), pItemDesc->GetItemCount());
				pDesc->SetTextColor( MCOLOR( 0xFFD0D0D0));
				pDesc->SetText( szCount);
				pDesc->AddText( "\n");
				pDesc->AddText( pItemDesc->GetItemDesc(), 0xFF808080);
			}

			return true;
		}


		// On double click
		else if ( MWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
		{
			// Put item
			if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ 0].IsExist())
				ZApplication::GetStageInterface()->OnDropSacrificeItem( 0);
			else if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ 1].IsExist())
				ZApplication::GetStageInterface()->OnDropSacrificeItem( 1);
		
			return true;
		}

		return false;
	}
};

MSacrificeItemListBoxListener g_SacrificeItemListBoxListener;

MListener* ZGetSacrificeItemListBoxListener( void)
{
	return &g_SacrificeItemListBoxListener;
}


/***********************************************************************
  OnDropCallbackRemoveSacrificeItem
  
  desc : ��� ������ ����
************************************************************************/
void OnDropCallbackRemoveSacrificeItem( void* pSelf, MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	if ( (pSender == NULL) || (strcmp(pSender->GetClassName(), MINT_ITEMSLOTVIEW)))
		return;

	ZItemSlotView* pItemSlotView = (ZItemSlotView*)pSender;
	ZApplication::GetStageInterface()->OnRemoveSacrificeItem( (strcmp( pItemSlotView->m_szItemSlotPlace, "SACRIFICE0") == 0) ? 0 : 1);
}


// �����̸� �������̽�
/***********************************************************************
OpenRelayMapBox : public

desc : �����̸� ���� â ����
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::OpenRelayMapBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxOpen");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxClose");
	if ( pButton)
		pButton->Show( true);

	SetRelayMapBoxPos(2);

	// ���÷��� �ڽ��� �ʸ���Ʈ�� ����
	ZApplication::GetStageInterface()->RelayMapCreateMapList();
}

/***********************************************************************
CloseRelayMapBox : public

desc : �����̸� ���� â �ݱ�
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::CloseRelayMapBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	SetRelayMapBoxPos(1);
}

/***********************************************************************
HideRelayMapBox : public

desc : �����̸� ���� â ���߱�
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::HideRelayMapBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	SetRelayMapBoxPos(0);
}

/***********************************************************************
GetRelayMapBoxPos : public

desc : �����̸� ���� â ��ġ ���ϱ�
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::SetRelayMapBoxPos( int nBoxPos)
{
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapListView");
	if ( pWidget)
	{
		MRECT rect;

		switch ( nBoxPos)
		{
		case 0 :		// Hide
			rect = pWidget->GetRect();
			m_nRelayMapListFramePos = -rect.w;
			break;

		case 1 :		// Close
			rect = pWidget->GetRect();
			m_nRelayMapListFramePos = -rect.w + ( rect.w * 0.14);
			break;

		case 2 :		// Open
			m_nRelayMapListFramePos = 0;
			break;
		}
	}
}

/***********************************************************************
PostRelayMapType : public

desc : �����̸� Ÿ�� ������Ʈ �� ������ ����
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::PostRelayMapElementUpdate( void)
{
	MComboBox* pCBRelayMapType = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if ( !pCBRelayMapType) return;
	MComboBox* pCBRelayMapTurnCount = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if ( !pCBRelayMapTurnCount) return;
	ZPostStageRelayMapElementUpdate(ZGetGameClient()->GetStageUID(), pCBRelayMapType->GetSelIndex(), pCBRelayMapTurnCount->GetSelIndex());
}

/***********************************************************************
PostRelayMapListUpdate : public

desc : �����̸� ����Ʈ ������Ʈ(�������� ����Ʈ�� ������.)
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::PostRelayMapInfoUpdate( void)
{
	MComboBox* pCBRelayMapType = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if ( !pCBRelayMapType) return;
	MComboBox* pCBRelayMapRepeatCount = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if ( !pCBRelayMapRepeatCount) return;

	MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelayMapListBox == NULL) return;

	if(pRelayMapListBox->GetCount() <= 0)	
	{
		ZApplication::GetGameInterface()->ShowMessage(MSG_GAME_RELAYMAP_ONE_OR_MORE_MAP_SELECT);
		return;
	}
	
	//�����̸� ����Ʈ ����
	void* pMapArray = MMakeBlobArray(sizeof(MTD_RelayMap), pRelayMapListBox->GetCount());
	int nMakeBlobCnt = 0;
	for(int i=0; i<pRelayMapListBox->GetCount(); i++)
	{
		MTD_RelayMap* pRelayMapNode = (MTD_RelayMap*)MGetBlobArrayElement(pMapArray, nMakeBlobCnt);
		for (int j = 0; j < MMATCH_MAP_COUNT; j++)
		{
			if(0 == strcmp(pRelayMapListBox->GetString(i), (char*)MGetMapDescMgr()->GetMapName(j)))
			{
				pRelayMapNode->nMapID = j;
				break;
			}
		}
		++nMakeBlobCnt;
	}

	ZPostStageRelayMapInfoUpdate(ZGetGameClient()->GetStageUID(), pCBRelayMapType->GetSelIndex(), pCBRelayMapRepeatCount->GetSelIndex(), pMapArray);
	MEraseBlobArray(pMapArray);

	ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(true);
	ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", false);
}

/***********************************************************************
PostRelayMapList : public

desc : �����̸� �������� �� �޺��ڽ� ����Ʈ�� ����� �ش�.
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::RelayMapCreateMapList()
{
	MComboBox* pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapSelection");
	if(pCombo == NULL) return;

	// �� ����Ʈ ����� �ֱ�
	MListBox* pMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_MapListbox");
	if(pMapListBox == NULL) return;

	pMapListBox->RemoveAll();	// ���� �����̸� ����Ʈ�� ��� �����ش�.
	for( int i = 0 ; i < pCombo->GetCount(); ++i )
	{
		if(strcmp(MMATCH_MAPNAME_RELAYMAP, pCombo->GetString(i)) == 0)
			continue;
		RelayMapList* pRelayMapList = new RelayMapList( pCombo->GetString(i), MBitmapManager::Get( "Mark_Arrow.bmp"));
		pMapListBox->Add( pRelayMapList);
	}


	// ������ �����̸� �������̸� �����̸� ����Ʈ�� ������Ʈ ���� �ʴ´�.
	if(!ZApplication::GetStageInterface()->GetIsRelayMapRegisterComplete())
		return;

	// �����̸� ����Ʈ ����� �ֱ�
	MListBox* pRelaMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelaMapListBox == NULL) return;
	RelayMap arrayRelayMapList[MAX_RELAYMAP_LIST_COUNT];
	memcpy(arrayRelayMapList, ZGetGameClient()->GetMatchStageSetting()->GetRelayMapList(), sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT);
	
	pRelaMapListBox->RemoveAll();	// ���� �����̸� ����Ʈ�� ��� �����ش�.
	for( int i = 0 ; i < ZGetGameClient()->GetMatchStageSetting()->GetRelayMapListCount(); ++i )
	{// �����̸� ����Ʈ�� �����͸� �߰����ش�.
		int nMapID = arrayRelayMapList[i].nMapID;
		RelayMapList* pRelayMapList = new RelayMapList( MGetMapDescMgr()->GetMapName(MGetMapDescMgr()->GetMapID(nMapID)), MBitmapManager::Get( "Mark_X.bmp"));
		pRelaMapListBox->Add( pRelayMapList);
	}
}

/***********************************************************************
MMapListBoxListener

desc : �� ����Ʈ �ڽ� ������
************************************************************************/
class MMapListBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// �������� �����Ͱ� �ƴϸ� �� ����Ʈ�� ��Ʈ�� �Ҽ�����.
		if(!ZGetGameClient()->AmIStageMaster())
			return false;
		// On select
		if ( MWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
		{
			// �ʸ���Ʈ���� ���õ� �������� �����̸� ����Ʈ�� �߰��Ѵ�.
			MListBox* pMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_MapListbox");
			if(pMapListBox == NULL) return false;
			MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
			if(pRelayMapListBox == NULL) return false;
			if(MAX_RELAYMAP_LIST_COUNT <= pRelayMapListBox->GetCount()) 
			{
				ZApplication::GetGameInterface()->ShowMessage( MSG_GAME_RELAYMAP_TOO_MANY_LIST_COUNT );
				return false;
			}

			RelayMapList* pMapList = (RelayMapList*)pMapListBox->GetSelItem();
			char szMapName[ MAPNAME_LENGTH ];
			strcpy(szMapName, pMapList->GetString());
			RelayMapList* pRelayMapList = new RelayMapList( szMapName, MBitmapManager::Get( "Mark_X.bmp"));

			pRelayMapListBox->Add( pRelayMapList);
			pRelayMapListBox->ShowItem(pRelayMapListBox->GetCount());	// ��ũ�� ��ġ ����

			// ������ �� ���� ����
			ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", true);

			return true;
		}

		// On double click
		else if ( MWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
		{
			return true;
		}

		return false;
	}
};
MMapListBoxListener g_MapListBoxListener;
MListener* ZGetMapListBoxListener( void)
{
	return &g_MapListBoxListener;
}

/***********************************************************************
MRelayMapListBoxListener

desc : �����̸� ����Ʈ �ڽ� ������
************************************************************************/
class MRelayMapListBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// �������� �����Ͱ� �ƴϸ� �����̸� ����Ʈ�� ��Ʈ�� �Ҽ�����.
		if(!ZGetGameClient()->AmIStageMaster())
			return false;
		// On select
		if ( MWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
		{
			MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
			if(pRelayMapListBox == NULL) return false;
			if(pRelayMapListBox->GetCount() > 1)
			{	// ���� �ּ��� �Ѱ��� �־�� �Ѵ�.
				pRelayMapListBox->Remove(pRelayMapListBox->GetSelIndex());

				// �����̸� ����Ʈ ��ũ�� ��ġ ����
				if(pRelayMapListBox->GetCount() <= pRelayMapListBox->GetShowItemCount())
					pRelayMapListBox->ShowItem(0);
				else
					pRelayMapListBox->SetStartItem(pRelayMapListBox->GetCount());
				
				// ������ �� ���� ����
				ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(false);
				ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", true);

				return true;
			}
			else
			{
				ZApplication::GetGameInterface()->ShowMessage(MSG_GAME_RELAYMAP_ONE_OR_MORE_MAP_SELECT);
				return false;
			}
		}

		// On double click
		else if ( MWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
		{
			return true;
		}
		return false;
	}
};
MRelayMapListBoxListener g_RelayMapListBoxListener;
MListener* ZGetRelayMapListBoxListener( void)
{
	return &g_RelayMapListBoxListener;
}

/***********************************************************************
  StartMovieOfQuest : public
  
  desc : ����Ʈ ���� �����Ҷ� ������ �������� ���� ������
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::StartMovieOfQuest( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	m_dwClockOfStartMovie = timeGetTime();

	// ȭ�� �ִϸ��̼� ����
	MAnimation* pAnimation = (MAnimation*)pResource->FindWidget( "Stage_Flame0");
	if ( pAnimation && m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist())
	{
		pAnimation->SetCurrentFrame( 0);
		pAnimation->Show( true);
		pAnimation->SetRunAnimation( true);
	}
	pAnimation = (MAnimation*)pResource->FindWidget( "Stage_Flame1");
	if ( pAnimation && m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist())
	{
		pAnimation->SetCurrentFrame( 0);
		pAnimation->Show( true);
		pAnimation->SetRunAnimation( true);
	}

	m_bDrawStartMovieOfQuest = true;
}


/***********************************************************************
  OnDrawStartMovieOfQuest : public
  
  desc : ����Ʈ ���� �����Ҷ� ������ �������� ���� �׸�
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnDrawStartMovieOfQuest( void)
{
	if ( !m_bDrawStartMovieOfQuest)
		return ;

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// ��� �ð��� ���Ѵ�.
	DWORD dwClock = timeGetTime() - m_dwClockOfStartMovie;

	// ��� ������ ���̵� �ƿ�
	int nOpacity = 255 - dwClock * 0.12f;
	if ( nOpacity < 0)
		nOpacity = 0;

	MPicture* pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage0");
	if ( pPicture && m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist())
		pPicture->SetOpacity( nOpacity);

	pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage1");
	if ( pPicture && m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist())
		pPicture->SetOpacity( nOpacity);

	// ���� �ð��� ��쿡...
	if ( dwClock > 3200)
	{
		m_bDrawStartMovieOfQuest = false;

		ZMyQuestItemMap::iterator itMyQItem;

		// �����ذ�... ����Ʈ ���ӽ����� ������ ���� �ش� ����Ʈ �������� ���ҵ� ����(�Źٸ�)�� ������ 
		// DB���� �޾ƿ��⶧���� ī��Ʈ ���Ҹ� �ϸ� �߸��Ȱ��� �ȴ�....20090318 by kammir
		// ���⼭ ���Կ� �ڽ��� �������� �÷��� ������ �ش� ������ ī��Ʈ ����.
		//if( ZGetGameClient()->GetUID() == m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetUID() )
		//{
		//	itMyQItem = ZGetMyInfo()->GetItemList()->GetQuestItemMap().find( m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetItemID() );
		//	itMyQItem->second->Decrease();
		//}
		//if( ZGetGameClient()->GetUID() == m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetUID() )
		//{
		//	itMyQItem = ZGetMyInfo()->GetItemList()->GetQuestItemMap().find( m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetItemID() );
		//	itMyQItem->second->Decrease();
		//}
		
		m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

		ZApplication::GetGameInterface()->SetState( GUNZ_GAME);
	}
}


/***********************************************************************
  IsShowStartMovieOfQuest : public
  
  desc : ����Ʈ ���� �����Ҷ� ������ �������� ���� �������� ���θ� ����.
  arg  : none
  ret  : true(=Quest mode start movie) or false(=none)
************************************************************************/
bool ZStageInterface::IsShowStartMovieOfQuest( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if ( m_nGameType == MMATCH_GAMETYPE_QUEST)
	{
		if ( m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist() || m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist())
			return true;
	}

	return false;
}


/***********************************************************************
  OnResponseDropSacrificeItemOnSlot : public
  
  desc : ��� �������� �ö�����
  arg  : none
  ret  : none
************************************************************************/
bool ZStageInterface::OnResponseDropSacrificeItemOnSlot( const int nResult, const MUID& uidRequester, const int nSlotIndex, const int nItemID )
{
#ifdef _QUEST_ITEM
	if( MOK == nResult)
	{
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID);
		MBitmap* pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon( nItemID, false);

		m_SacrificeItem[ nSlotIndex].SetSacrificeItemSlot( uidRequester, nItemID, pIconBitmap, pItemDesc->m_szQuestItemName, pItemDesc->m_nLevel);
		SerializeSacrificeItemListBox();

		UpdateSacrificeItem();
	}
	else if( ITEM_TYPE_NOT_SACRIFICE == nResult)
	{
		// ��� �������� �ƴ�.
		return false;
	}
	else if( NEED_MORE_QUEST_ITEM == nResult )
	{
		// ���� ������ �ִ� ������ �ʰ��ؼ� �÷� �������� �������.
	}
	else if( MOK != nResult )
	{
		// ����...
		return false;
	}
	else
	{
		// ���ǵ��� ���� error...
		ASSERT( 0 );
	}

#endif

	return true;
}


/***********************************************************************
  OnResponseCallbackSacrificeItem : public
  
  desc : ��� �������� ����������
  arg  : none
  ret  : none
************************************************************************/
bool ZStageInterface::OnResponseCallbackSacrificeItem( const int nResult, const MUID& uidRequester, const int nSlotIndex, const int nItemID )
{
#ifdef _QUEST_ITEM
	if( MOK == nResult )
	{
		m_SacrificeItem[ nSlotIndex].RemoveItem();
		SerializeSacrificeItemListBox();

		UpdateSacrificeItem();

		MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
		char szText[256];
		sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
		pDesc->SetText(szText);
		//if ( pDesc)
		//	pDesc->Clear();
	}
	else if( ERR_SACRIFICE_ITEM_INFO == nResult )
	{
		// Ŭ���̾�Ʈ���� ���� ������ �߸��� ����. ���� ����ó���� �ʿ��ϸ� ���⼭ ���ָ� ��.
	}

#endif

	return true;
}

#ifdef _QUEST_ITEM
///
// Fist : �߱���.
// Last : �߱���.
// 
// �����κ��� QL�� ������ ����.
///
bool ZStageInterface::OnResponseQL( const int nQL )
{
	ZGetQuest()->GetGameInfo()->SetQuestLevel( nQL);

	// �������� ������ �ִ� ����Ʈ ���� ǥ�� ����
	MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_QuestLevel");
	if ( pLabel)
	{
		char szText[125];
		sprintf( szText, "%s %s : %d", ZMsg( MSG_WORD_QUEST), ZMsg( MSG_CHARINFO_LEVEL), nQL);
		pLabel->SetText( szText);
	}

	return true;
}

bool ZStageInterface::OnStageGameInfo( const int nQL, const int nMapsetID, const unsigned int nScenarioID )
{
	if (nScenarioID != 0)
	{
		ZGetQuest()->GetGameInfo()->SetQuestLevel( nQL );
	}
	else
	{
		// �ó������� ������ �׳� 0���� ���̰� �Ѵ�.
		ZGetQuest()->GetGameInfo()->SetQuestLevel( 0 );
	}

	ZGetQuest()->GetGameInfo()->SetMapsetID( nMapsetID );
	ZGetQuest()->GetGameInfo()->SetSenarioID( nScenarioID );

	UpdateStageGameInfo(nQL, nMapsetID, nScenarioID);

	return true;
}

bool ZStageInterface::OnResponseSacrificeSlotInfo( const MUID& uidOwner1, const unsigned long int nItemID1, 
												   const MUID& uidOwner2, const unsigned long int nItemID2 )
{
	if ( (uidOwner1 != MUID(0,0)) && nItemID1)
	{
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID1);
		MBitmap* pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon( nItemID1, false);
		m_SacrificeItem[ SACRIFICEITEM_SLOT0].SetSacrificeItemSlot( uidOwner1, nItemID1, pIconBitmap, pItemDesc->m_szQuestItemName, pItemDesc->m_nLevel);
	}
	else
		m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();

	if ( (uidOwner2 != MUID(0,0)) && nItemID2)
	{
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID2);
		MBitmap* pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon( nItemID2, false);
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].SetSacrificeItemSlot( uidOwner2, nItemID2, pIconBitmap, pItemDesc->m_szQuestItemName, pItemDesc->m_nLevel);
	}
	else
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

	UpdateSacrificeItem();

	MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
	//if ( pDesc)
	//	pDesc->Clear();

	return true;
}


bool ZStageInterface::OnQuestStartFailed( const int nState )
{
	MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageChattingOutput");
	if ( pTextArea)
	{
		char text[256];
		sprintf(text, "^1%s", ZMsg(MSG_GANE_NO_QUEST_SCENARIO));
		pTextArea->AddText( text);
	}

/*
	if( MSQITRES_INV == nState )
	{
		// �ش� QL������ ��������� ���� ���̺��� ����. �̰��� ���� �ʴ� ��� �������� �÷��� �������.
		MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageChattingOutput");
		if ( pTextArea)
			pTextArea->AddText( "^1���� �����ִ� �������� ���ǿ� ���� �ʾ� ������ ������ �� �����ϴ�.");
	}
	else if( MSQITRES_DUP == nState )
	{
		// ���� ���Կ� ���� �������� �÷��� ����.
		MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageChattingOutput");
		if ( pTextArea)
			pTextArea->AddText( "^1���� ������ 2���� ���������Ƿ� ������ ������ �� �����ϴ�.");
	}
*/
	return true;
}


bool ZStageInterface::OnNotAllReady()
{
	return true;
}
#endif

void ZStageInterface::UpdateStageGameInfo(const int nQL, const int nMapsetID, const int nScenarioID)
{
	if (!ZGetGameTypeManager()->IsQuestOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) return;

	// �������� ������ �ִ� ����Ʈ ���� ǥ�� ����
	MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_QuestLevel");
	if ( pLabel)
	{
		char szText[125];
		sprintf( szText, "%s %s : %d", ZMsg( MSG_WORD_QUEST), ZMsg( MSG_CHARINFO_LEVEL), nQL);
		pLabel->SetText( szText);
	}


#define		MAPSET_NORMAL		MCOLOR(0xFFFFFFFF)
#define		MAPSET_SPECIAL		MCOLOR(0xFFFFFF40)			// Green
//#define		MAPSET_SPECIAL		MCOLOR(0xFFFF2020)		// Red

	// ���⼭ �ó����� �̸��� �����ش�.
	pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_SenarioName");
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_SenarioNameImg");
	MPicture* pPictureL = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_Lights0");
	MPicture* pPictureR = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_Lights1");
	if ( pLabel)
	{
		if (nScenarioID == 0)
		{
			// �ó������� ���� ���
			pLabel->SetText( "");
			if ( pWidget)
				pWidget->Show( false);
			if ( pPictureL) {
				pPictureL->Show( false);
			}
			if ( pPictureR) {
				pPictureR->Show( false);
			}
		}
		else
		{
			pLabel->SetAlignment( MAM_HCENTER | MAM_VCENTER);

			LIST_SCENARIONAME::iterator itr =  m_SenarioDesc.find( nScenarioID);
			if ( itr != m_SenarioDesc.end())
			{
				pLabel->SetText( (*itr).second.m_szName);
				pLabel->SetTextColor(MCOLOR(0xFFFFFF00));
				if ( pWidget)
					pWidget->Show( true);

				if ( pPictureL) {
					pPictureL->Show( true);
					pPictureL->SetBitmapColor(MAPSET_SPECIAL);
				}
				if ( pPictureR) {
					pPictureR->Show( true);
					pPictureR->SetBitmapColor(MAPSET_SPECIAL);
				}
			}
			else
			{
				// Ư���ó������� �������� ���Խó������̴�.
				pLabel->SetText("");
				pLabel->SetTextColor(MCOLOR(0xFFFFFFFF));
				if ( pWidget)
					pWidget->Show( false);

				if ( pPictureL) {
					pPictureL->Show( true);
					pPictureL->SetBitmapColor(MAPSET_NORMAL);
				}
				if ( pPictureR) {
					pPictureR->Show( true);
					pPictureR->SetBitmapColor(MAPSET_NORMAL);
				}

			}
		}
	}
}

/***********************************************************************
  SetSacrificeItemSlot : public
  
  desc : ��� ������ ���Կ� ������ ������ �Է�
  arg  : none
  ret  : none
************************************************************************/
void SacrificeItemSlotDesc::SetSacrificeItemSlot( const MUID& uidUserID, const unsigned long int nItemID, MBitmap* pBitmap, const char* szItemName, const int nQL)
{
	m_uidUserID = uidUserID;
	m_nItemID = nItemID;
	m_pIconBitmap = pBitmap;
	strcpy( m_szItemName, szItemName);
	m_nQL = nQL;
	m_bExist = true;
}


/***********************************************************************
  ReadSenarioNameXML : protected
  
  desc : ����Ʈ ��� ������ XML�� �д´�
  arg  : none
  ret  : true(=success) or false(=fail)
************************************************************************/
bool ZStageInterface::ReadSenarioNameXML( void)
{
	if ( (int)m_SenarioDesc.size())
		return true;


	// XML ������ ����
	MXmlDocument xmlQuestItemDesc;
	xmlQuestItemDesc.Create();

	char			*buffer;
	MZFile			mzFile;

	string strFileScenario("System/scenario.xml");
//#ifndef _DEBUG
	//strFileScenario += ".mef";
//#endif
	if( !mzFile.Open(strFileScenario.c_str(), ZApplication::GetFileSystem())) 
	{
		mlog("Error while read : %s", strFileScenario.c_str());
		xmlQuestItemDesc.Destroy();
		return false;
	} 

	buffer = new char[ mzFile.GetLength() + 1];
	buffer[ mzFile.GetLength()] = 0;

	mzFile.Read( buffer, mzFile.GetLength());

	if( !xmlQuestItemDesc.LoadFromMemory( buffer))
	{
		xmlQuestItemDesc.Destroy();
		return false;
	}
	delete[] buffer;
	mzFile.Close();

	int nStdScenarioCount = 1000;

	// �����͸� �о�´�
	MXmlElement rootElement = xmlQuestItemDesc.GetDocumentElement();
	for ( int i = 0;  i < rootElement.GetChildNodeCount();  i++)
	{
		MXmlElement chrElement = rootElement.GetChildNode( i);

		char szTagName[ 256];
		chrElement.GetTagName( szTagName);

		if ( szTagName[ 0] == '#')
			continue;


		char szAttrName[64];
		char szAttrValue[256];
		int nItemID = 0;
		MSenarioList SenarioMapList;

		bool bFind = false;


		// ���� �ó�����
		if ( !stricmp( szTagName, "STANDARD_SCENARIO"))			// �±� ����
		{
			// Set Tag
			for ( int k = 0;  k < chrElement.GetAttributeCount();  k++)
			{
				chrElement.GetAttribute( k, szAttrName, szAttrValue);

				SenarioMapList.m_ScenarioType = ST_STANDARD;	// Type

				if ( !stricmp( szAttrName, "title"))			// Title
					strcpy( SenarioMapList.m_szName, szAttrValue);

				else if ( !stricmp( szAttrName, "mapset"))		// Map set
					strcpy( SenarioMapList.m_szMapSet, szAttrValue);
			}

			nItemID = nStdScenarioCount++;					// ID

			bFind = true;
		}

		// Ư�� �ó�����
		else if ( !stricmp( szTagName, "SPECIAL_SCENARIO"))			// �±� ����
		{
			// Set Tag
			for ( int k = 0;  k < chrElement.GetAttributeCount();  k++)
			{
				chrElement.GetAttribute( k, szAttrName, szAttrValue);

				SenarioMapList.m_ScenarioType = ST_SPECIAL;		// Type

				if ( !stricmp( szAttrName, "id"))				// ID
					nItemID = atoi( szAttrValue);

				else if ( !stricmp( szAttrName, "title"))		// Title
					strcpy( SenarioMapList.m_szName, szAttrValue);

				else if ( !stricmp( szAttrName, "mapset"))		// Map set
					strcpy( SenarioMapList.m_szMapSet, szAttrValue);
			}

			bFind = true;
		}


		if ( bFind)
			m_SenarioDesc.insert( LIST_SCENARIONAME::value_type( nItemID, SenarioMapList));
	}

	xmlQuestItemDesc.Destroy();


	return true;
}

bool ZStageInterface::OnStopVote()
{
	ZGetGameClient()->SetVoteInProgress( false );
	ZGetGameClient()->SetCanVote( false );

#ifdef _DEBUG
	string str = ZMsg(MSG_VOTE_VOTE_STOP);
#endif

	ZChatOutput(ZMsg(MSG_VOTE_VOTE_STOP), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	return true;
}


void ZStageInterface::OnStartFail( const int nType, const MUID& uidParam )
{
	if( ALL_PLAYER_NOT_READY == nType )
	{
		// ��� ������ ���� ���� �ʾ���.
		ZGetGameInterface()->PlayVoiceSound( VOICE_PLAYER_NOT_READY, 1500);
	}
	else if( QUEST_START_FAILED_BY_SACRIFICE_SLOT == nType )
	{
		OnQuestStartFailed( uidParam.Low );

		ZGetGameInterface()->PlayVoiceSound( VOICE_QUEST_START_FAIL, 2800);
	}
	else if( INVALID_TACKET_USER == nType )
	{
		// �ڱ� �ڽ��̸� -_-;;
		//if( uidParam == ZGetMyUID() )
		//{
			char szMsg[ 128 ];

			sprintf( szMsg, MGetStringResManager()->GetErrorStr(MERR_CANNOT_START_NEED_TICKET), ZGetGameClient()->GetObjName(uidParam).c_str() );

			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
		//}

		// ����� ���� ���°ž�? -_-+ - by SungE
		//const MMatchPeerInfo* pPeer = ZGetGameClient()->GetPeers()->Find( uidParam );
		//if( 0 != pPeer )
		//{
		//	char szMsg[ 128 ];

		//	sprintf( szMsg, MGetStringResManager()->GetErrorStr(MERR_CANNOT_START_NEED_TICKET), pPeer->CharInfo.szName );

		//	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		//	ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
		//}
	}
	else if( INVALID_MAP == nType )
	{
		char szMsg[ 128 ];

			sprintf( szMsg, "INVALID MAP!" );
				//MGetStringResManager()->GetErrorStr(MERR_CANNOT_START_NEED_TICKET), 
				//ZApplication::GetInstance()->GetGameClient()->GetObjName(uidParam).c_str() );

			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
	}

	// Stage UI Enable
	ChangeStageEnableReady( false);
}
