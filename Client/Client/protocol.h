#pragma once /* protocol.h */
//-----------------------------------------------------------------------------------------------------------

#include "..//..//Protocol//prodef.h"
#include "..//..//Protocol//GameProtocol.h"
#include "..//..//Protocol//JoinProtocol.h"

#include <vector>
#include <deque>
//-----------------------------------------------------------------------------------------------------------

#define WM_GM_JOIN_CLIENT_MSG_PROC		0x403
// ----
#define C2SDIR(a)						(((a)+1)&0x07)
#define S2CDIR(a)						(((a)-1)&0x07)
//-----------------------------------------------------------------------------------------------------------

void ProtocolCore			(unsigned char protoNum, const unsigned char * aRecv, int aLen);
// ----
void SCChatMsg				(PMSG_CHATDATA & msg);												// 0x00
void SCNotice				(PMSG_NOTICE & msg);												// 0x0D
// ----
bool CSConnectServer		(const char * szIP, int nPort);										// It's not a msg, it's socket connection.
void SCPJoinResult			(PMSG_JOINRESULT & msg);
// ----
void CSLoginServer			(const char * pszAccount, const char * pszPsw);
void SCLoginResult			(PMSG_RESULT & msg);
// ----
void CSGetCharList			();																	// 0xF3
void SCCharList				(const unsigned char * msg);										// (leo123:  not sure) 0xF3
// ----
void CSRoleCreate			(const char* szName, unsigned char uClass);
// ----
void CSEnterWorld			(unsigned char uIndex);
void SCEnterWorld			(PMSG_CHARMAPJOINRESULT & msg);
// ----
void CSMoveDataLoadingOK	();																	// 0xF3:0x12
// ----
void CSChat(const char* szChat);
// ----
void SCCheckMainExe			(PMSG_CHECK_MAINEXE& msg);											// 0x3
// ----
void CSLiveClient			();																	// 0xE
// ----
void SCPlayerViewportCreate	(const unsigned char* msg);											// 0x12
// ----
void SCMonsterViewportCreate(const unsigned char* msg);											// 0x13
// ----
void SCViewportDestroy		(const unsigned char* msg);											// 0x14
// ----
void CSMove					(int x, int y, std::deque<char> path, unsigned char uTargetDir);	// 0xD7
void SCMove					(PMSG_RECVMOVE & msg);												// 0xD7
// ----
void CSAttack				(unsigned char AttackAction, int Number, unsigned char DirDis);
void SCAttack				(PMSG_ATTACKRESULT & msg);
// ----
void SCKillPlayer			(PMSG_KILLPLAYER & msg);											// 0x16
// ----
void SCDiePlayer			(PMSG_DIEPLAYER & msg);												// 0x17
// ----
void SCAction				(PMSG_ACTIONRESULT & msg);											// 0x18
// ----
void SCTeleport				(PMSG_TELEPORT_RESULT & msg);										// 0x1C
// ----
void SCItemViewportCreate	(const unsigned char* msg);											// 0x20
// ----
void SCItemViewportDestroy	(const unsigned char* msg);											// 0x21
// ----
void SCRefill				(PMSG_REFILL & msg);												// 0x26
// ----
void SCManaSend				(PMSG_MANASEND & msg);												// 0x27
//-----------------------------------------------------------------------------------------------------------

//void MsgSendV2(LPOBJ lpObj, unsigned char* Msg, int size);
//void CGLiveClient(PMSG_CLIENTTIME* lpClientTime, short aIndex);
//void GCCheckMainExeKeySend(int aIndex);
//WORD EncryptCheckSumKey(WORD wSource);
//void CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT* lpMsg, int aIndex);
//void PEchoProc(unsigned char* aMsg, int aLen, short aIndex);
//void GCResultSend(int aIndex, unsigned char headcode, unsigned char result);
//void ChatSend(LPOBJ lpObj, char* szChat);
//void vChatSend(LPOBJ lpObj, char* szChat, ...);
//void AllSendServerMsg(char* chatmsg);
//void DataSendAll(unsigned char* lpMsg, int iMsgSize);
//void ChatTargetSend(LPOBJ lpObj, char* szChat, int senduser);
//void PChatProc(PMSG_CHATDATA* lpChat, short aIndex);
//void PMacProc(PMSG_MAC_ADDR * lpMac, short aIndex);
//void CGChatRecv(PMSG_CHATDATA_NUMBER* lpMsg, int aIndex);
//void GCServerMsgSend(unsigned char msg, int aIndex);
//void GCServerMsgStringSend(char* szMsg, int aIndex, BYTE type);
//void GCServerMsgStringSendGuild(_GUILD_INFO_STRUCT* lpNode, char* szMsg, unsigned char type);
//void GCEventStateSend(int aIndex, unsigned char state, unsigned char event);
//void GCMapEventStateSend(int map, unsigned char state, unsigned char event);
//void CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, int aIndex);
//void SCPJoinResultSend(int aIndex, unsigned char result);
//void CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, int aIndex);
//void CSPJoinIdPassRequestTEST(PMSG_IDPASS* lpMsg, int aIndex);
//void GCJoinBillCheckSend(char* AccountId, int aIndex);
//void GCJoinResult(unsigned char result, int aIndex);
//void GCJoinSocketResult(unsigned char result, SOCKET Socket);
//void CGClientCloseMsg(PMSG_CLIENTCLOSE* lpMsg, int aIndex);
//void GCCloseMsgSend(int aIndex, unsigned char result);
//void CGClientMsg(PMSG_CLIENTMSG* lpMsg, int aIndex);
//void CGPCharacterCreate(PMSG_CHARCREATE* lpMsg, int aIndex);
//void CGPCharDel(PMSG_CHARDELETE* lpMsg, int aIndex);
//void CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN* lpMsg, int aIndex);
//void GCLevelUpMsgSend(int aIndex, int iSendEffect);
//void CGLevelUpPointAdd(PMSG_LVPOINTADD* lpMsg, int aIndex);
//void LevelUpPointAdd(byte nType, int nPointCount, int aIndex);// huihui
//void GCInventoryItemOneSend(int aIndex, int pos);
//void GCPkLevelSend(int aIndex, unsigned char pklevel);
//void GCMagicListOneSend(int aIndex, char Pos, unsigned char type, unsigned char level, unsigned short skill, BYTE btListType);
//void GCMagicListOneDelSend(int aIndex, char Pos, BYTE type, BYTE level, WORD skill, BYTE btListType);
//void GCMagicListMultiSend(LPOBJ lpObj, BYTE btListType);
//void GCEquipmentSend(int aIndex);
//void GCRecallMonLife(int aIndex, int maxlife, int life);
//void GCTimeViewSend(int aIndex, int second);
//void GCGoalSend(int aIndex, char* Name1, unsigned char score1, char* Name2, unsigned char score2);
//void GCSkillKeyRecv(PMSG_SKILLKEY* lpMsg, int aIndex);
//void GCSkillKeySend(int aIndex, unsigned char* keybuffer, unsigned char GO, unsigned char Qk, unsigned char Wk, unsigned char Ek, /*<regrel ebp+0x20>*/ /*|0x1|*/ unsigned char ChatWnd);
//void GCMoneySend(int aIndex, unsigned long money);
//void GCItemInventoryPutSend(int aIndex, unsigned char result, unsigned char iteminfo1, unsigned char iteminfo2);
//void CGItemGetRequest(PMSG_ITEMGETREQUEST* lpMsg, int aIndex);
//BOOL CGItemDropRequest(PMSG_ITEMTHROW* lpMsg, int aIndex, int drop_type);
//void GCItemMoveResultSend(int aIndex, unsigned char result, unsigned char pos, unsigned char* const ItemInfo);
//void CGInventoryItemMove(PMSG_INVENTORYITEMMOVE* lpMsg, int aIndex);
//void GCEquipmentChange(int aIndex, int pos);
//void CGTalkRequestRecv(PMSG_TALKREQUEST* lpMsg, int aIndex);
//void GCUserWarehouseSend(LPOBJ lpObj);
//void CGBuyRequestRecv(PMSG_BUYREQUEST* lpMsg, int aIndex);
//void CGSellRequestRecv(PMSG_SELLREQUEST* lpMsg, int aIndex);
//int  GetNeedMoneyItemDurRepaire(class CItem* DurItem, int RequestPos);
//void ItemDurRepaire(LPOBJ lpObj, class CItem* DurItem, int pos, int RequestPos);
//void CGModifyRequestItem(PMSG_ITEMDURREPAIR* lpMsg, int aIndex);
//void CGTradeRequestSend(PMSG_TRADE_REQUEST* lpMsg, int aIndex);
//void CGTradeResponseRecv(PMSG_TRADE_RESPONSE* lpMsg, int aIndex);
//BOOL GCTradeResponseSend(BYTE response, int aIndex, LPSTR id, WORD level, int GuildNumber);
//int GCTradeOtherDel(int aIndex, BYTE tradeindex);
//int GCTradeOtherAdd(int aIndex, BYTE tradeindex, LPBYTE iteminfo);
//void CGTradeMoneyRecv(PMSG_TRADE_GOLD* lpMsg, int aIndex);
//BOOL GCTradeMoneyOther(int aIndex, DWORD money);
//BOOL GCTradeOkButtonSend(int aIndex, unsigned char flag);
//void CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON* lpMsg, int aIndex);
//void CGTradeCancelButtonRecv(int aIndex);
//void CGTradeResult(int aIndex, unsigned char result);
//void CGPShopReqSetItemPrice(PMSG_REQ_PSHOP_SETITEMPRICE* lpMsg, int aIndex);
//void CGPShopAnsSetItemPrice(int aIndex, BYTE btResult, BYTE btItemPos);
//void CGPShopReqOpen(PMSG_REQ_PSHOP_OPEN* lpMsg, int aIndex);
//void CGPShopAnsOpen(int aIndex, BYTE btResult);
//void CGPShopReqClose(int aIndex);
//void CGPShopAnsClose(int aIndex, BYTE btResult);
//void CGPShopReqBuyList(PMSG_REQ_BUYLIST_FROM_PSHOP* lpMsg, int aSourceIndex);
//void CGPShopAnsBuyList(int aSourceIndex, int aTargetIndex, BYTE btResult, bool bResend);
//void CGPShopReqBuyItem(PMSG_REQ_BUYITEM_FROM_PSHOP* lpMsg, int aSourceIndex);
//void CGPShopAnsBuyItem(int aSourceIndex, int aTargetIndex, int iItemPos, BYTE btResult);
//void CGPShopReqCloseDeal(PMSG_REQ_PSHOPDEAL_CLOSE* lpMsg, int aIndex);
//void CGPShopAnsSoldItem(int aSourceIndex, int aTargetIndex, int iItemPos);
//void CGPShopAnsDealerClosedShop(int aSourceIndex, int aTargetIndex);
//void CGPartyRequestRecv(PMSG_PARTYREQUEST* lpMsg, int aIndex);
//void CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT* lpMsg, int aIndex);
//void CGPartyList(int aIndex);
//void CGPartyListAll(int pnumber);
//void CGPartyDelUser(PMSG_PARTYDELUSER* lpMsg, int aIndex);
//void GCPartyDelUserSend(int aIndex);
//void CGGuildRequestRecv(PMSG_GUILDJOINQ* lpMsg, int aIndex);
//void CGGuildRequestResultRecv(PMSG_GUILDQRESULT* lpMsg, int aIndex);
//void CGGuildListAll(int pnumber);
//void CGGuildDelUser(PMSG_GUILDDELUSER* lpMsg, int aIndex);
//void GCGuildDelUserResult(int aIndex, unsigned char Result);
//void GCGuildMasterQuestionSend(int aIndex);
//void CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER* lpMsg, int aIndex);
//void GCGuildMasterManagerRun(int aIndex);
//void CGGuildMasterInfoSave(int aIndex,PMSG_GUILDINFOSAVE* lpMsg);
//void CGGuildMasterCreateCancel(int aIndex);
//void GCGuildViewportNowPaint(int aIndex, char* guildname, unsigned char* Mark, int isGuildMaster);
//void GCGuildViewportDelNow(int aIndex, int isGuildMaster);
//void GCManagerGuildWarEnd(char* GuildName);
//void GCManagerGuildWarSet(char* GuildName1, char* GuildName2, int type);
//void GCGuildWarRequestResult(char* GuildName, int aIndex, int type);
//void GCGuildWarRequestSend(char* GuildName, int aIndex, int type);
//void GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT* lpMsg, int aIndex);
//void GCGuildWarDeclare(int aIndex, char* _guildname);
//void GCGuildWarEnd(int aIndex, unsigned char result, char* _guildname);
//void GCGuildWarScore(int aIndex);
//void CGWarehouseMoneyInOut(int aIndex,PMSG_WAREHOUSEMONEYINOUT* lpMsg);
//void GCWarehouseInventoryMoneySend(int aIndex, unsigned char result, int money, int wmoney);
//void CGWarehouseUseEnd(int aIndex);
//void GCWarehouseStateSend(int aIndex, unsigned char state);
//void GCWarehouseRecivePassword(int aIndex,PMSG_WAREHOUSEPASSSEND* lpMsg);
//void GCUserChaosBoxSend(LPOBJ lpObj, int iChaosBoxType);
//void CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, int aIndex);
//void CGChaosBoxUseEnd(int aIndex);
//void PMoveProc(PMSG_MOVE* lpMove, int aIndex);
//void RecvPositionSetProc(PMSG_POSISTION_SET* lpMove, int aIndex);
//void CGAttack(PMSG_ATTACK* lpMsg, int aIndex);
//void GCDamageSend(int aIndex, int damage, int iShieldDamage);
//void GCDamageSend(int aIndex, int TargetIndex, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage);
//void GCKillPlayerExpSend(int aIndex, int TargetIndex, int exp, int AttackDamage, int MSBFlag);
//void GCDiePlayerSend(LPOBJ lpObj, int TargetIndex, WORD skill, int KillerIndex);
//void GCActionSend(LPOBJ lpObj, BYTE ActionNumber, int aIndex, int aTargetIndex);
//void CGActionRecv(PMSG_ACTION* lpMsg, int aIndex);
//void CGMagicAttack(PMSG_MAGICATTACK* lpMsg, int aIndex);
//void GCMagicAttackNumberSend(LPOBJ lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess);
//void GCMagicCancelSend(LPOBJ lpObj, int MagicNumber);
//void GCMagicCancel(PMSG_MAGICCANCEL * lpMsg, int aIndex);
//void GCUseEffectItem(LPOBJ lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
//void GCUseEffectItem2(LPOBJ lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
//void CGMagicCancel(PMSG_MAGICCANCEL* lpMsg, int aIndex);
//void GCUseMonsterSkillSend(LPOBJ lpObj, LPOBJ lpTargetObj, int iSkillNumber);
//void GCStateInfoSend(LPOBJ lpObj, unsigned char state, int ViewSkillState);
//void CGTeleportRecv(PMSG_TELEPORT* lpMsg, int aIndex);
//void CGTargetTeleportRecv(PMSG_TARGET_TELEPORT* lpMsg, int aIndex);
//void GCTeleportSend(LPOBJ lpObj, unsigned char MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir);
//void CGBeattackRecv(unsigned char* lpRecv, int aIndex, int magic_send);
//void CGDurationMagicRecv(PMSG_DURATION_MAGIC_RECV* lpMsg, int aIndex);
//void ObjectMapJoinPositionSend(short aIndex);
//void CGUseItemRecv(PMSG_USEITEM* lpMsg, int aIndex);
//void GCReFillSend(int aIndex, WORD Life, BYTE Ipos, unsigned char flag,  WORD wShield);
//void GCManaSend(int aIndex, short Mana, BYTE Ipos, unsigned char flag,  WORD BP);
//void GCInventoryItemDeleteSend(int aIndex, BYTE pos, unsigned char flag);
//void GCItemUseSpecialTimeSend(int aIndex, unsigned char number, int time);
//void GCItemDurSend(int aIndex, BYTE pos, unsigned char dur, unsigned char flag);
//void GCItemDurSend2(int aIndex, BYTE pos, unsigned char dur, unsigned char flag);
//void CGWeatherSend(int aIndex, BYTE weather);
//void GCServerCmd(int aIndex, BYTE type, unsigned char Cmd1, unsigned char Cmd2);
//void GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, int aIndex);
//void GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, int aIndex);
//void AllSendMsg(unsigned char* Msg, int size);
//void AllSendSameMapMsg(unsigned char* Msg, int size, unsigned char mapnumber);
//void GCSendPing(int aIndex);
//void GCPingSendRecv(PMSG_PING_RESULT* aRecv, int aIndex);
//void GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, int aIndex);
//void GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, int aIndex);
//void GCUseEndEventChipRescv(int aIndex);
//void GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, int aIndex);
//void CGRequestQuestInfo(int aIndex);
//void GCSendQuestInfo(int aIndex, int QuestIndex);
//void CGSetQuestState(PMSG_SETQUEST* lpMsg, int aIndex);
//void GCSendQuestPrize(int aIndex, unsigned char Type, unsigned char Count);
//void CGCloseWindow(int aIndex);
//void CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, int iIndex);
//void CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, int iIndex);
//void CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC* lpMsg, int aIndex);
//void CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, int aIndex);
//void CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, int aIndex);
//void CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER* lpMsg, int aIndex);
//void GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM* aRecv, int aIndex);
//void GCNPggSendCheckSum(int aIndex,_GG_AUTH_DATA* pggAuthData);
//void GCNPggCheckSumRecv(PMSG_NPROTECTGGCHECKSUM* lpMsg, int aIndex);
//void CGDuelStartRequestRecv(PMSG_REQ_START_DUEL* lpMsg, int aIndex);
//void CGDuelEndRequestRecv(PMSG_REQ_END_DUEL* lpMsg, int aIndex);
//void CGDuelOkRequestRecv(PMSG_ANS_DUEL_OK* lpMsg, int aIndex);
//void GCSendDuelScore(int aIndex1, int aIndex2);
//void GCSendGetItemInfoForParty(int aIndex, class CMapItem* lpItem);
//void GCSendEffectInfo(int aIndex, BYTE btType);
//void CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND* lpMsg, int aIndex);
//void CGRequestPetItemInfo(PMSG_REQUEST_PET_ITEMINFO* lpMsg, int aIndex);
//void GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT* aRecv, int aIndex);
//void CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ* aRecv, int aIndex);
//void CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ* aRecv, int aIndex);
//void CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ* aRecv, int aIndex);
//void CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS* aRecv, int aIndex);
//void CGUnionList(PMSG_UNIONLIST_REQ* aRecv, int aIndex);
//void CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, int aIndex);
//void CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH* lpMsg, int iIndex);
//void GCAnsMapSvrAuth(int iIndex, int iResult);
//void CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE* lpMsg, int iIndex);
//void GCAnsCastleSiegeState(int iIndex, int iResult, char* lpszGuildName, char* lpszGuildMaster);
//void CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE* lpMsg, int iIndex);
//void GCAnsRegCastleSiege(int iIndex, int iResult, char* lpszGuildName);
//void CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE* lpMsg, int iIndex);
//void GCAnsGiveUpCastleSiege(int iIndex, int iResult, int bGiveUp, int iMarkCount, char* lpszGuildName);
//void CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO* lpMsg, int iIndex);
//void GCAnsGuildRegInfo(int iIndex, int iResult,CSP_ANS_GUILDREGINFO* lpMsgResult);
//void CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK* lpMsg, int iIndex);
//void GCAnsRegGuildMark(int iIndex, int iResult,CSP_ANS_GUILDREGMARK* lpMsgResult);
//void CGReqNpcBuy(PMSG_REQ_NPCBUY* lpMsg, int iIndex);
//void GCAnsNpcBuy(int iIndex, int iResult, int iNpcNumber, int iNpcIndex);
//void CGReqNpcRepair(PMSG_REQ_NPCREPAIR* lpMsg, int iIndex);
//void GCAnsNpcRepair(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP);
//void CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE* lpMsg, int iIndex);
//void GCAnsNpcUpgrade(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue);
//void CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO* lpMsg, int iIndex);
//void GCAnsTaxMoneyInfo(int iIndex, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money);
//void CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE* lpMsg, int iIndex);
//void GCAnsTaxRateChange(int iIndex, int iResult, BYTE btTaxType, int iTaxRate);
//void CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT* lpMsg, int iIndex);
//void GCAnsMoneyDrawOut(int iIndex, int iResult, __int64 i64Money);
//void GCAnsCsGateState(int iIndex, int iResult, int iGateIndex);
//void CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE* lpMsg, int iIndex);
//void GCAnsCsGateOperate(int iIndex, int iResult, int iGateIndex, int iGateOperate);
//void GCAnsCsGateCurState(int iIndex, int iGateIndex, int iGateOperate);
//void GCAnsCsAccessSwitchState(int iIndex, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState);
//void GCAnsCsAccessCrownState(int iIndex, BYTE btCrownState);
//void GCAnsCsNotifyStart(int iIndex, BYTE btStartState);
//void GCAnsCsNotifyProgress(int iIndex, BYTE btCastleSiegeState, char* lpszGuildName);
//void GCAnsCsMapSvrTaxInfo(int iIndex, BYTE btTaxType, BYTE btTaxRate);
//void CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA* lpMsg, int iIndex);
//void GCAnsCsMiniMapData(int iIndex, BYTE btResult);
//void CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA* lpMsg, int iIndex);
//void CGReqCsSendCommand(PMSG_REQ_CSCOMMAND* lpMsg, int iIndex);
//void GCAnsCsSendCommand(int iCsJoinSide, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand);
//void GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute);
//void GCAnsSelfCsLeftTimeAlarm(int iIndex, BYTE btHour, BYTE btMinute);
//void CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER* lpMsg, int iIndex);
//void GCAnsCsSetEnterHuntZone(int iIndex, BYTE btResult, BYTE btEnterHuntZone);
//void CGReqNpcDbList(PMSG_REQ_NPCDBLIST* lpMsg, int iIndex);
//void CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST* lpMsg, int iIndex);
//void CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST* lpMsg, int iIndex);
//void CGReqWeaponUse(PMSG_REQ_USEWEAPON* aRecv, int iIndex);
//void CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE* aRecv, int iIndex);
//void CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER* aRecv, int iIndex);
//void CGReqJewelMix(PMSG_REQ_JEWEL_MIX* lpMsg, int iIndex);
//void GCAnsJewelMix(int iIndex, int iResult);
//void CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX* lpMsg, int iIndex);
//void GCAnsJewelUnMix(int iIndex, int iResult);
//void CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, int iIndex);
//void GCAnsCrywolfInfo(int iIndex, BYTE btOccupationState, BYTE btCrywolfState);
//void CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, int iIndex);
//void CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, int iIndex);
//void CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, int iIndex);
//void GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, int iIndex);
//void GCMsgTest(int aIndex, char* Message, unsigned char* Photo, unsigned char Dir, unsigned char Action);
//void WereWolfQuarrelCheck(int aIndex);
//void GateKeeperCheck(int aIndex);
//#if(GS_CASTLE==1)
//void CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE* aRecv, int aIndex);
//void GCSendObjectCreationState(int iObjectIndex);
//#endif