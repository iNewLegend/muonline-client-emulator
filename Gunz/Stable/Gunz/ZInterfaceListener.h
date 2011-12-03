#ifndef ZINTERFACELISTENER_H
#define ZINTERFACELISTENER_H

class MListener;

// ������ ���� ��ũ��
#define DECLARE_LISTENER(_FunctionName)	MListener* _FunctionName(void);

/// ������ ���� ��ũ��, BEGIN_ ~ END_ ���̿� ���ϴ� �ڵ带 �ִ´�.
#define BEGIN_IMPLEMENT_LISTENER(_FunctionName, _szMessageName)					\
	MListener* _FunctionName(void){										\
class ListenerClass : public MListener{									\
public:																	\
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage){	\
	if(MWidget::IsMsg(szMessage, _szMessageName)==true){

#define END_IMPLEMENT_LISTENER()			\
	return true;			\
	}							\
	return false;				\
	}								\
};									\
	static ListenerClass	Listener;	\
	return &Listener;					\
	}


// Gunz�� ���̴� ���� Listener Object ���
MListener* ZGetChatInputListener(void);
MListener* ZGetLoginListener(void);
MListener* ZGetLogoutListener(void);
MListener* ZGetExitListener(void);
MListener* ZGetChannelChatInputListener(void);
MListener* ZGetStageChatInputListener(void);
MListener* ZGetGameStartListener(void);
MListener* ZGetMapChangeListener(void);
MListener* ZGetMapSelectListener(void);
MListener* ZGetParentCloseListener(void);
//MListener* ZGetCombatChatInputListener(void);
MListener* ZGetStageCreateFrameCallerListener(void);
MListener* ZGetSelectCharacterComboBoxListener(void);


// �α���
DECLARE_LISTENER(ZGetLoginStateButtonListener)
DECLARE_LISTENER(ZGetGreeterStateButtonListener)
DECLARE_LISTENER(ZGetOptionFrameButtonListener)
DECLARE_LISTENER(ZGetRegisterListener)

// �κ�
/*
DECLARE_LISTENER(ZGetChannelPlayerOptionGroupAll)
DECLARE_LISTENER(ZGetChannelPlayerOptionGroupFriend)
DECLARE_LISTENER(ZGetChannelPlayerOptionGroupClan)
*/
//DECLARE_LISTENER(ZGetLobbyStageListListener)
DECLARE_LISTENER(ZGetLobbyPrevRoomListButtonListener)
DECLARE_LISTENER(ZGetLobbyNextRoomListPrevButtonListener)

/*
DECLARE_LISTENER(ZGetLobbyNextRoomNo1ButtonListener)
DECLARE_LISTENER(ZGetLobbyNextRoomNo2ButtonListener)
DECLARE_LISTENER(ZGetLobbyNextRoomNo3ButtonListener)
DECLARE_LISTENER(ZGetLobbyNextRoomNo4ButtonListener)
DECLARE_LISTENER(ZGetLobbyNextRoomNo5ButtonListener)
DECLARE_LISTENER(ZGetLobbyNextRoomNo6ButtonListener)
*/
DECLARE_LISTENER(ZGetLobbyNextRoomNoButtonListener)

/*
DECLARE_LISTENER(ZGetListenerLobbyPlayerListTabChannel);
DECLARE_LISTENER(ZGetListenerLobbyPlayerListTabFriend);
DECLARE_LISTENER(ZGetListenerLobbyPlayerListTabClan);
*/
DECLARE_LISTENER(ZGetPlayerListPrevListener);
DECLARE_LISTENER(ZGetPlayerListNextListener);

DECLARE_LISTENER(ZGetArrangedTeamGameListener);
DECLARE_LISTENER(ZGetArrangedTeamDialogOkListener);
DECLARE_LISTENER(ZGetArrangedTeamDialogCloseListener);
DECLARE_LISTENER(ZGetArrangedTeamGame_CancelListener);

DECLARE_LISTENER(ZGetLeaveClanOKListener);
DECLARE_LISTENER(ZGetLeaveClanCancelListener);

// Channel frame
DECLARE_LISTENER(ZGetChannelListJoinButtonListener)
DECLARE_LISTENER(ZGetChannelListCloseButtonListener)
DECLARE_LISTENER(ZGetChannelListListener)
DECLARE_LISTENER(ZGetPrivateChannelEnterListener);
DECLARE_LISTENER(ZGetMyClanChannel);
DECLARE_LISTENER(ZGetChannelList);
DECLARE_LISTENER(ZGetMyClanChannel);

// Stage
DECLARE_LISTENER(ZGetMapListListener)
DECLARE_LISTENER(ZGetStageListFrameCallerListener)
DECLARE_LISTENER(ZGetStageCreateBtnListener)
DECLARE_LISTENER(ZGetPrivateStageJoinBtnListener)
DECLARE_LISTENER(ZGetChannelListFrameCallerListener)
DECLARE_LISTENER(ZGetStageJoinListener)
DECLARE_LISTENER(ZGetStageSettingCallerListener)
DECLARE_LISTENER(ZGetStageSettingStageTypeListener)
DECLARE_LISTENER(ZGetStageSettingChangedComboboxListener)			// ��ȯ�̰� �ۼ�
DECLARE_LISTENER(ZGetStageTeamRedListener)
DECLARE_LISTENER(ZGetStageTeamBlueListener)
DECLARE_LISTENER(ZGetStageObserverBtnListener)
DECLARE_LISTENER(ZGetStageReadyListener)
DECLARE_LISTENER(ZGetStageSettingApplyBtnListener)
DECLARE_LISTENER(ZGetLobbyListener)
DECLARE_LISTENER(ZGetBattleExitButtonListener)
DECLARE_LISTENER(ZGetStageExitButtonListener)
DECLARE_LISTENER(ZGetCombatMenuCloseButtonListener)
DECLARE_LISTENER(ZGetPreviousStateButtonListener)
DECLARE_LISTENER(ZGetShopCallerButtonListener)
DECLARE_LISTENER(ZGetShopCloseButtonListener)
DECLARE_LISTENER(ZGetQuickJoinButtonListener)
DECLARE_LISTENER(ZGetDuelTournamentGameButtonListener)
DECLARE_LISTENER(ZGetDuelTournamentGame2TestButtonListener)
DECLARE_LISTENER(ZGetDuelTournamentGame4TestButtonListener)
DECLARE_LISTENER(ZGetDuelTournamentWaitCancelButtonListener)
DECLARE_LISTENER(ZGetLobbyCharInfoCallerButtonListener)
DECLARE_LISTENER(ZGetStageMapListSelectionListener)
//DECLARE_LISTENER(ZGetStageMapListCallerListener)
DECLARE_LISTENER(ZStageSacrificeItem0)
DECLARE_LISTENER(ZStageSacrificeItem1)
DECLARE_LISTENER(ZStagePutSacrificeItem)
DECLARE_LISTENER(ZStageSacrificeItemBoxOpen)
DECLARE_LISTENER(ZStageSacrificeItemBoxClose)

DECLARE_LISTENER(ZGetRelayMapTypeListener)
DECLARE_LISTENER(ZGetRelayMapTurnCountListener)
DECLARE_LISTENER(ZStageRelayMapBoxOpen)
DECLARE_LISTENER(ZStageRelayMapBoxClose)
DECLARE_LISTENER(ZGetRelayMapOKButtonListener)


DECLARE_LISTENER(ZGetStagePlayerListPrevListener);
DECLARE_LISTENER(ZGetStagePlayerListNextListener);

// �ɼǵ��� ZOptionInterface �� �ű�

// �ɼ�
// �ɼǵ��� ZOptionInterface �� �ű�

// ����
DECLARE_LISTENER(ZGetEquipmentCallerButtonListener)
DECLARE_LISTENER(ZGetEquipmentCloseButtonListener)
DECLARE_LISTENER(ZGetCharSelectionCallerButtonListener)
DECLARE_LISTENER(ZGetBuyButtonListener)
DECLARE_LISTENER(ZGetBuyItemDetailFrameGiftListener)
DECLARE_LISTENER(ZGetBuyItemDetailFrameCancelListener)
DECLARE_LISTENER(ZGetBuySpendableItemConfirmOpenListener)
DECLARE_LISTENER(ZGetSellButtonListener)
DECLARE_LISTENER(ZGetSellSpendableItemConfirmOpenListener)

DECLARE_LISTENER(ZGetCountableItemTradeDlgCloseListener)
DECLARE_LISTENER(ZGetCountableItemTradeDlgOkButtonListener)
DECLARE_LISTENER(ZGetCountableItemTradeDlgCountUpButtonListener)
DECLARE_LISTENER(ZGetCountableItemTradeDlgCountDnButtonListener)
DECLARE_LISTENER(ZGetCountableItemTradeDlgCountChangeListener)
DECLARE_LISTENER(ZGetSellCashItemConfirmDlgOkButtonListener)
DECLARE_LISTENER(ZGetSellCashItemConfirmDlgCancelListener)

DECLARE_LISTENER(ZGetEquipButtonListener)
DECLARE_LISTENER(ZGetEquipmentSearchButtonListener)
DECLARE_LISTENER(ZGetStageForcedEntryToGameListener)
DECLARE_LISTENER(ZGetAllEquipmentListCallerButtonListener)			// ���Թ�ư
DECLARE_LISTENER(ZGetMyAllEquipmentListCallerButtonListener)		// �ǸŹ�ư
DECLARE_LISTENER(ZGetCashEquipmentListCallerButtonListener)			// ĳ���� ��ư
DECLARE_LISTENER(ZGetShopEquipmentCallerButtonListener)				// ��->��� ��ư ����
DECLARE_LISTENER(ZGetSendAccountItemButtonListener)					// ������

DECLARE_LISTENER(ZGetBringAccountItemButtonListener)					// ��������

DECLARE_LISTENER(ZShopItemEquipmentTabOpen);						// �ɸ��� �Ϲ� ����� ��ư
DECLARE_LISTENER(ZShopWeaponEquipmentTabOpen);						// �ɸ��� ĸ�� ����� ��ư
DECLARE_LISTENER(ZShopListFrameClose)								// ListFrame �ݱ� ��ư
DECLARE_LISTENER(ZShopListFrameOpen)								// ListFrame ���� ��ư

//DECLARE_LISTENER(ZGetShopAskButtonListener);						// ������
//DECLARE_LISTENER(ZGetShopGiftButtonListener);						// �����ϱ�
DECLARE_LISTENER(ZGetShopCachRechargeButtonListener);				// �����ϱ�
DECLARE_LISTENER(ZGetShopSearchCallerButtonListener);				// �˻�

// ���
DECLARE_LISTENER(ZGetEquipmentCharacterTabButtonListener);			// ĳ���� �� ����
DECLARE_LISTENER(ZGetEquipmentAccountTabButtonListener);			// �߾����� �� ����
DECLARE_LISTENER(ZGetEquipmentShopCallerButtonListener);			// ��� -> �� ��ư ����
DECLARE_LISTENER(ZGetLevelConfirmListenter);						// �߾����� �������� �������� Ȯ��
DECLARE_LISTENER(ZEquipItemEquipmentTabOpen);						// �ɸ��� �Ϲ� ����� ��ư
DECLARE_LISTENER(ZEquipWeaponEquipmentTabOpen);					// �ɸ��� ĸ�� ����� ��ư
DECLARE_LISTENER(ZEquipListFrameClose)								// ListFrame �ݱ� ��ư
DECLARE_LISTENER(ZEquipListFrameOpen)								// ListFrame ���� ��ư
DECLARE_LISTENER(ZEquipmetRotateBtn)								// ĳ���� �� ȸ�� ���� ��ư


// ĳ���� ���� ����
DECLARE_LISTENER(ZGetSelectCharacterButtonListener);
DECLARE_LISTENER(ZGetShowCreateCharacterButtonListener);
DECLARE_LISTENER(ZGetDeleteCharacterButtonListener);
DECLARE_LISTENER(ZGetConfirmDeleteCharacterButtonListener);
DECLARE_LISTENER(ZGetCloseConfirmDeleteCharButtonListener);

DECLARE_LISTENER(ZGetSelectCharacterButtonListener0);
DECLARE_LISTENER(ZGetSelectCharacterButtonListener1);
DECLARE_LISTENER(ZGetSelectCharacterButtonListener2);
DECLARE_LISTENER(ZGetSelectCharacterButtonListener3);

DECLARE_LISTENER(ZGetShowCharInfoGroupListener);
DECLARE_LISTENER(ZGetShowEquipInfoGroupListener);

// �ʼ��� ����
DECLARE_LISTENER(ZGetMapComboListener);
DECLARE_LISTENER(ZGetSelectMapPrevButtonListener);
DECLARE_LISTENER(ZGetSelectMapNextButtonListener);

// ���õ� ĳ���� ������
DECLARE_LISTENER(ZGetSelectCameraLeftButtonListener);
DECLARE_LISTENER(ZGetSelectCameraRightButtonListener);

// ĳ���� ���� ȭ��
DECLARE_LISTENER(ZGetCreateCharacterButtonListener);
DECLARE_LISTENER(ZGetCancelCreateCharacterButtonListener);
DECLARE_LISTENER(ZChangeCreateCharInfoListener);
DECLARE_LISTENER(ZGetCreateCharacterLeftButtonListener);
DECLARE_LISTENER(ZGetCreateCharacterRightButtonListener);

// ���÷��� ������ ����
DECLARE_LISTENER(ZReplayOk);
DECLARE_LISTENER(ZGetReplayCallerButtonListener);
DECLARE_LISTENER(ZGetReplayViewButtonListener);
DECLARE_LISTENER(ZGetReplayExitButtonListener);
DECLARE_LISTENER(ZGetReplayFileListBoxListener);


// ���� ���� ����
DECLARE_LISTENER(ZGetGameResultQuit);


// ���� ����
DECLARE_LISTENER(ZGetMonsterInterfacePrevPage);
DECLARE_LISTENER(ZGetMonsterInterfaceNextPage);
DECLARE_LISTENER(ZGetMonsterInterfaceQuit);


// ��Ÿ
//DECLARE_LISTENER(ZGetListenerGamePlayerListTabGame);
//DECLARE_LISTENER(ZGetListenerGamePlayerListTabFriend);
DECLARE_LISTENER(ZGetRoomListListener);
DECLARE_LISTENER(ZGetMonsterBookCaller);


// 112 Ȯ��â
DECLARE_LISTENER(ZGet112ConfirmEditListener);
DECLARE_LISTENER(ZGet112ConfirmOKButtonListener);
DECLARE_LISTENER(ZGet112ConfirmCancelButtonListener);

// Ŭ�� ����
DECLARE_LISTENER(ZGetClanSponsorAgreementConfirm_OKButtonListener);
DECLARE_LISTENER(ZGetClanSponsorAgreementConfirm_CancelButtonListener);
DECLARE_LISTENER(ZGetClanSponsorAgreementWait_CancelButtonListener);
DECLARE_LISTENER(ZGetClanJoinerAgreementConfirm_OKButtonListener);
DECLARE_LISTENER(ZGetClanJoinerAgreementConfirm_CancelButtonListener);
DECLARE_LISTENER(ZGetClanJoinerAgreementWait_CancelButtonListener);

// Ŭ�� ���� ���̾�α�
DECLARE_LISTENER(ZGetLobbyPlayerListTabClanCreateButtonListener);
DECLARE_LISTENER(ZGetClanCreateDialogOk);
DECLARE_LISTENER(ZGetClanCreateDialogClose);

// Ȯ�� ���̾˷α�
DECLARE_LISTENER(ZGetClanCloseConfirmListenter);
DECLARE_LISTENER(ZGetClanLeaveConfirmListenter);

// ���ǿ�û ��� ��ư
DECLARE_LISTENER(ZGetProposalAgreementWait_CancelButtonListener);
DECLARE_LISTENER(ZGetProposalAgreementConfirm_OKButtonListener);
DECLARE_LISTENER(ZGetProposalAgreementConfirm_CancelButtonListener);
// ���ǽ¶�

// ��� ���� �� ����� Ȯ�� ���̾�α�
DECLARE_LISTENER(ZGetLanguageChangeConfirmListenter);


#endif
