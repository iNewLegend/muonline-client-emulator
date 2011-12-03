#include "stdafx.h"
#include "MMatchDBMgr.h"
#include "MMatchObject.h"
#include "afxwin.h"
#include "MMatchTransDataType.h"
#include "MMatchFriendInfo.h"
#include <stdio.h>
#include "MErrorTable.h"
#include "MDebug.h"
#include "MMatchFriendInfo.h"
#include "MMatchClan.h"
#include "mmsystem.h"
#include "MInetUtil.h"
#include "mcrc32.h"
#include "MQuestConst.h"
#include "MQuestItem.h"
#include "MUtil.h"
// #include "MMatchGambleMachine.h"
#include "MMatchDBGambleItem.h"
#include "MMatchAccountPenaltyInfo.h"
// SQL ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ���� ���� - UserID, Password, UGradeID, PGradeID, Name, Email, RegNum, Sex, ZipCode, Address
// �ٲ���. - �ݸ��� ������ ���� �ȹٲ���, Cert�÷��� �߰��ؾ���
//TCHAR g_szDB_CREATE_ACCOUNT[] = _T("{CALL spInsertAccount ('%s', '%s', %d, %d, '%s', '%s', '%s', %d, '%s', '%s')}");
// UserID, Password, Cert, Name, Age, Sex 
TCHAR g_szDB_CREATE_ACCOUNT[] = _T("{CALL spInsertAccount ('%s', '%s', %d, '%s', %d, %d)}");
TCHAR g_szDB_CREATE_ACCOUNT_NETMARBLE[] = _T("{CALL spInsertAccount_Netmarble ('%s', '%s', %d, %d, %d)}");

// ���� �α��� ���� �޾ƿ���
// UserID
TCHAR g_szDB_GET_LOGININFO[] = _T("{CALL spGetLoginInfo ('%s')}");

// ���� �α��� ���� �޾ƿ��� - �ݸ��� ����
// @UserID, @UserCn
TCHAR g_szDB_GET_LOGININFO_NETMARBLE[] = _T("{CALL spGetLoginInfo_Netmarble ('%s', '%s')}");
TCHAR g_szDB_GET_LOGININFO_NETMARBLE2[] = _T("{CALL spGetLoginInfo_Netmarble2 ('%s', '%s')}");

// ���� ���� �޾ƿ���
TCHAR g_szDB_GET_ACCOUNTINFO[] = _T("{CALL spGetAccountInfo (%d)}");

// ĳ���� ����
TCHAR g_szDB_DELETE_CHAR[] = _T("{CALL spDeleteChar (%d, %d, '%s')}");

// ĳ���� ����Ʈ �޾ƿ���
TCHAR g_szDB_GET_CHARLIST[] = _T("{CALL spGetCharList (%d)}");

// ĳ���� ���� ������Ʈ
TCHAR g_szDB_SIMPLE_UPDATE_CHARINFO[] = _T("{CALL spSimpleUpdateChar (%d, '%s', %d, %d, %d)}");

// ���� �α�
// TCHAR g_szDB_INSERT_CONN_LOG[] = _T("{CALL spInsertConnLog (%d, '%s', '%s')}");
TCHAR g_szDB_INSERT_CONN_LOG[] = _T("{CALL spInsertConnLog (%d, %d, %d, %d, %d, '%s')}");

// ���� �α�
//TCHAR g_szDB_INSERT_GAME_LOG[] = _T("{CALL spInsertGameLog ('%s', '%s', '%s', %d, %d, %d, '%s')}");
TCHAR g_szDB_INSERT_GAME_LOG[] = _T("{CALL spInsertGameLog2 (%d, '%s', '%s')}");

// ���� �÷��� �α�
TCHAR g_szDB_INSERT_GAME_PLAYER_LOG[] = _T("{CALL InsertGamePlayerLog(%d, %d, %d, %d, %d, %d, %d)}");

// ų �α�
TCHAR g_szDB_INSERT_KILL_LOG[] = _T("{CALL spInsertKillLog (%d, %d)}");

// �ٿ�Ƽ�� ������ ���� �α�
TCHAR g_szDB_INSERT_ITEM_PURCHASE_BY_BOUNTY_LOG[] = _T("{CALL spInsertItemPurchaseLogByBounty (%d, %d, %d, %d, '%s')}");

// ĳ���� ���� �α�
TCHAR g_szDB_INSERT_CHAR_MAKING_LOG[] = _T("{CALL spInsertCharMakingLog (%d, '%s', '%s')}");

// ���� �α�
TCHAR g_szDB_INSERT_SERVER_LOG[] = _T("{CALL spInsertServerLog (%d, %d, %d, %u, %u)}");

// ���� ������ ����
//TCHAR g_szDB_UPDATE_SERVER_STATUS[] = _T("UPDATE ServerStatus Set CurrPlayer=%d, Time=GETDATE() WHERE ServerID=%d");
// ����: CurrPlayer, ServerID
TCHAR g_szDB_UPDATE_SERVER_STATUS[] = _T("{CALL spUpdateServerStatus (%d, %d)}");
TCHAR g_szDB_UPDATE_SERVER_STATUS_NETMARBLE[] = _T("{CALL spUpdateServerStatus_Netmarble (%d, %d, %d)}");

// �÷��̾� �α�
TCHAR g_szDB_INSERT_PLAYER_LOG[] = _T("{CALL spInsertPlayerLog (%d, %d, %d, %d, %d, %d)}");

// �ݸ��� TP �α�
TCHAR g_szDB_INSERT_NETMARBLE_TP_LOG[] = _T("{CALL spInsertTPLogNetmarble (%d, %d)}");

// ������ �α�
TCHAR g_szDB_INSERT_LEVELUP_LOG[] = _T("{CALL spInsertLevelUpLog (%d, %d, %d, %d, %d, %d)}");

// ������ �α�
TCHAR g_szDB_UPDATE_CHAR_PLAYINFO_LOG[] = _T("{CALL spCharUpdatePlayInfo (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}");

// ���� ������Ʈ
//TCHAR g_szDB_UPDATE_CHAR_LEVEL[] = _T("UPDATE Character Set Level=%d WHERE CID=%d");
// ����: Level, CID
TCHAR g_szDB_UPDATE_CHAR_LEVEL[] = _T("{CALL spUpdateCharLevel (%d, %d)}");

// BP ������Ʈ
//TCHAR g_szDB_UPDATE_CHAR_BP[] = _T("UPDATE Character SET BP=BP+(%d) WHERE CID=%d");
// ����: BPInc, CID
TCHAR g_szDB_UPDATE_CHAR_BP[] = _T("{CALL spUpdateCharBP (%d, %d)}");


// ĳ���� �÷��� �ð� ������Ʈ
//TCHAR g_szDB_UPDATE_CHAR_PLAYTIME[] = _T("UPDATE Character SET PlayTime=PlayTime+(%d), LastTime=GETDATE() WHERE CID=%d");
// ����: PlayTimeInc, CID
TCHAR g_szDB_UPDATE_CHAR_PLAYTIME[] = _T("{CALL	spUpdateCharPlayTime (%d, %d)}");

// ���ӽð�, IP����
// ����: IP, �ݸ���UserID
TCHAR g_szDB_UPDATE_LAST_CONNDATE[] = _T("{CALL	spUpdateLastConnDate ('%s', '%s')}");

// ģ�� �߰�
// ����: CID, FriendCID, Favorite
TCHAR g_szDB_ADD_FRIEND[] = _T("{CALL spAddFriend (%d, %d, %d)}");

// ����: CID, FriendCID
TCHAR g_szDB_REMOVE_FRIEND[] = _T("{CALL spRemoveFriend (%d, %d)}");

// ����: CID
TCHAR g_szDB_GET_FRIEND_LIST[] = _T("{CALL spGetFriendList (%d)}");

// ĳ���Ͱ� ���� Ŭ�� �˾ƿ���
// ����: CID
TCHAR g_szDB_GET_CHAR_CLAN[] = _T("{CALL spGetCharClan (%d)}");

// Ŭ���̸����� CLID �˾ƿ���
// ����: ClanName	varchar(24)
TCHAR g_szDB_GET_CLID_FROM_CLANNAME[] = _T("{CALL spGetCLIDFromClanName ('%s')}");

// Ŭ�� �����
// ����: ClanName	varchar(24), MasterCID, Member1CID, Member2CID, Member3CID, Member4CID
TCHAR g_szDB_CREATE_CLAN[] = _T("{CALL spCreateClan ('%s', %d, %d, %d, %d, %d)}");

// Ŭ�� ��� ����
// ����: CLID, ClanName, MasterCID
TCHAR g_szDB_RESERVE_CLOSE_CLAN[] = _T("{CALL spReserveCloseClan (%d, '%s', %d, '%s')}");

// Ŭ���� �߰�
// ����: CLID, JoinerCID, ClanGrade
TCHAR g_szDB_ADD_CLAN_MEMBER[] = _T("{CALL spAddClanMember (%d, %d, %d)}");

// Ŭ���� Ż��
// ����: CLID, LeaverCID
TCHAR g_szDB_REMOVE_CLAN_MEMBER[] = _T("{CALL spRemoveClanMember (%d, %d)}");

// Ŭ�� ��� ���� ����
// ����: CLID, MemberCID, ClanGrade
TCHAR g_szDB_UPDATE_CLAN_GRADE[] = _T("{CALL spUpdateClanGrade (%d, %d, %d)}");

// Ŭ�� ��� ���� Ż��
// ����: CLID, AdminClanGrade, MemberCharName
TCHAR g_szDB_EXPEL_CLAN_MEMBER[] = _T("{CALL spRemoveClanMemberFromCharName (%d, %d, '%s')}");

// Ŭ�� ���� ��� - Ŭ���������� ���
TCHAR g_szDB_GET_CLAN_INFO[] = _T("{CALL spGetClanInfo (%d)}");

/* ���� ���� */
// 4���� ID �˾ƿ���
// ����: Member1CID, Member2CID, Member3CID, Member4CID
TCHAR g_szDB_GET_LADDER_TEAM4_ID[] = _T("{CALL spGetTeamID4 (%d, %d, %d, %d)}");

// 4���� �¸� ���� ������Ʈ
// ����: WinnerTeamID, LoserTeamID, DrawFlag
TCHAR g_szDB_TEAM4_WIN_THE_GAME[] = _T("{CALL spTeam4WinTheGame (%d, %d, %d, %d, %d, %d)}");

// CID�� ������ ����� ����Ʈ ������
// ����: CID
TCHAR g_szDB_GET_LADDER_TEAM_MEMBERLIST[] = _T("{CALL spGetLadderTeamMemberByCID (%d)}");

/* Ŭ���� */
// Ŭ���� �¸� ���� ������Ʈ
// ����: WinnerCLID, LoserCLID, DrawFlag, WinnerPoint, LoserPoint, WinnerClanName, LoserClanName, 
//       RoundWins, RoundLosses, MapID, GameType
TCHAR g_szDB_WIN_THE_CLAN_GAME[] = _T("{CALL spWinTheClanGame (%d, %d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, '%s', '%s')}");

// Ŭ�� �⿩�� ������Ʈ
TCHAR g_szDB_UPDATE_CHAR_CLAN_CONTPOINT[] = _T("{CALL spUpdateCharClanContPoint (%d, %d, %d)}");

/* �̺�Ʈ */
// �̺�Ʈ ¯��ũ �ο�
// ���� GradeID, AID
TCHAR g_szDB_EVENT_JJANG_UPDATE[] = _T("UPDATE Account SET UGradeID=%d WHERE AID=%d");

// �ɸ����� ����Ʈ ������ ���� ��û.
TCHAR g_szDB_SELECT_QUEST_ITEM_INFO_BY_CID[] = _T( "{CALL spSelectCharQuestItemInfoByCID (%d)}" );
// Quest���� �α�.
// @GameName varchar(64)
// @Master int 
// @Player1 int 
// @Player2 int
// @Player3 int
// @TotalQItemCount smallint 
// @ScenarioID smallint 
// @GamePlayTime tinyint 
TCHAR g_szDB_INSERT_QUEST_GAME_LOG[] = _T( "{CALL spInsertQuestGameLog ('%s', %d, %d, %d, %d, %d, %d, %d)}" );

// ����ũ ����Ʈ ������ �α� ����.
// @QGLID int 
// @CID int
// @QIID int
TCHAR g_szDB_INSERT_QUINQUEITEMLOG[] = _T( "{CALL spInsertQUniqueItemLog (%d, %d, %d)}" );

// �����̾� IP �˻�
TCHAR g_szDB_CHECK_PREMIUM_IP[] = _T( "{CALL spCheckPremiumIP ('%s')}" );

// �̺�Ʈ ��÷���� CID ����.
// @AID int
// @CID int
// @EventName varchar(24)
TCHAR g_szInsertEvent [] = _T( "{CALL spInsertEvent (%u, %u, '%s')}" );

// @AID int
// @CID int
// @HackingType tinyint
// @IPPart1 tinyint
// @IPPart2 tinyint
// @IPPart3 tinyint
// @IPPart4 tinyint
// @EndHackingBlockDate smalldatetime
// @ServerID tinyint
// @ChannelName varchar(64)
// @Comment varchar(128)
// @HackingLevel tinyint
// TCHAR g_szSetBlockAccount[] = _T( "{CALL spSetBlockAccount (%u, %u, %u, '%s', '%s', '%s', 'u', '%s', '%s')}" );
TCHAR g_szSetHackingBlock[] = _T( "{CALL  LogDB..spSetHackingBlock (%u, %u, %u, %u, %u, %u, %u, '%s', %u, '%s', '%s', %u)}" );

// @AID int
// @BlockType tinyint
TCHAR g_szResetAccountHackingBlock[] = _T( "{CALL spResetAccountHackingBlock (%u, %u)}" );

// @AID int
// @CID int
// @BlockType tinyint
// @Comment varchar(256)
// @IP varchar(15)
// TCHAR g_szInsertBlockLog[] = _T( "{CALL spInsertBlockLog (%u, %u, %u,'%s', '%s')}" );

// @CID int
// @CLID int
// @DeleteDate smalldatetime
TCHAR g_szSetClanDeleteDate[] = _T( "{CALL spSetClanDeleteDate (%u, %u, '%s')}" );

// @CLID int
// @MasterCID int
// @DeleteName varchar(24)
// @WaitHour int = 168 -- 1����
TCHAR g_szDeleteExpiredClan[] = _T( "{CALL spDeleteExpiredClan (%u, %u, '%s', %u)}" );

TCHAR g_szAdminResetAllHackingBlock[] = _T( "{CALL spResetHackingBlock}" );

// @AID int
TCHAR g_szUpdateAccountLastLogoutTime[] = _T( "{CALL spUpdateAccountLastLogoutTime (%u)}" );

// @Name varchar(24)
TCHAR g_szCheckDuplicateCharName[] = _T( "{CALL spCheckDuplicateCharName ('%s')}" );

// @Name varchar(24)
TCHAR g_szGetCIDbyCharName[] = _T( "{CALL spGetCIDbyCharName ('%s')}" );

// @CID int
TCHAR g_szGetCharNameByCID[] = _T( "{CALL spGetCharNameByCID ('%u')}" );

// DB ���� Ȯ�� 
TCHAR g_szGetDBConnection[] = _T( "{CALL spGetDBConnection}" );

// 
TCHAR g_spGetGambleItemList[] = _T( "{CALL spGetGambleItemList}" );
TCHAR g_spGetGambleRewardItem[] = _T( "{CALL spGetGambleRewardItem}" );



// @AID int
//, @IsHacker tinyint
TCHAR g_spUpdateAccountPowerLevelingInfo[] = _T( "{CALL spUpdateAccountPowerLevelingInfo(%u, %u)}" );
///////////////////////////////////////////////////////////////////////////////
TCHAR g_spInsertSurvivalModeGameLog[] = _T( "{CALL spInsertSurvivalGameLog('%s', %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)}" );
TCHAR g_spGetSurvivalGroupRanking[] = _T( "{CALL spGetSurvivalGroupRanking}" );
TCHAR g_spGetSurvivalPrivateRanking[] = _T( "{CALL spGetSurvivalPrivateRanking(%u)}" );

///////////////////////////////////////////////////////////////////////////////
// Added By ȫ����(2009-09-25)
TCHAR g_spGetDTTimeStamp[] = _T( "{CALL spDTGetDTTimeStamp}" );

TCHAR g_spGetDTSideRankingInfo[]	= _T( "{CALL spDTGetDTSideRanking(%u)}" );
TCHAR g_spGetDTGroupRankingInfo[]	= _T( "{CALL spDTGetDTGroupRanking}" );

TCHAR g_spInsertDTCharacterInfo[]			= _T( "{CALL spDTInsertDTCharacterInfo(%u)}" );
TCHAR g_spGetDTCharacterInfo[]				= _T( "{CALL spDTGetDTCharacterInfo(%u)}" );
TCHAR g_spGetDTCharacterInfoPrevious[]		= _T( "{CALL spDTGetDTCharacterInfoPrevious(%u)}" );

TCHAR g_spUpdateDTCharacterInfo[]	= _T( "{CALL spDTUpdateDTCharacterInfo(%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}" );

TCHAR g_spInsertDTGameLog[]			= _T( "{CALL spDTInsertDTGameLog(%u, %d, %d, %d, %d, %d, %d, %d, %d, %d)}" );
TCHAR g_spUpdateDTGameLog[]			= _T( "{CALL spDTUpdateDTGameLog('%s', %d, %d)}" );
TCHAR g_spInsertDTGameLogDetail[]	= _T( "{CALL spDTInsertDTGameLogDetail(%d, '%s', %d, %d, %d, %d, %d, %d)}" );

///////////////////////////////////////////////////////////////////////////////
// Added By ȫ����(2010-08-10)
TCHAR g_spGetAccountPenaltyInfo[]		= _T( "{CALL spGetAccountPenaltyInfo(%d)}" );
TCHAR g_spInsertAccountPenaltyInfo[]	= _T( "{CALL spInsertAccountPenaltyInfo(%d, %d, %d, '%s')}" );





TCHAR g_szDB_GET_ALLITEM[] = _T("{CALL spGetAllItem}");	///< DB���� ��� Item ���̺� ������ ��������



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created, Modified, Arranged By ȫ����(2010-11-30)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//TCHAR g_szDB_UPDATE_SERVER_INFO[] = _T("UPDATE ServerStatus Set MaxPlayer=%d, ServerName='%s' WHERE ServerID=%d");
TCHAR g_szDB_UPDATE_SERVER_INFO[] = _T("{CALL spUpdateServerInfo (%d, '%s', %d)}");
// Desc  : ���� �ִ� �ο� ������Ʈ
// Param :	@ServerID		INT
//			@ServerName		VARCHAR(24)
//			@MaxPlayer		INT

TCHAR g_szDB_GET_ACCOUNT_CHARINFO[] = _T("{CALL spGetAccountCharInfo (%d, %d)}");
// Desc  : ���� ĳ���� ���� �޾ƿ���
// Param :	@AID		INT
//			@CharNum	SMALLINT

TCHAR g_szDB_CREATE_CHAR[] = _T("{CALL spInsertChar (%d, %d, '%s', %d, %d, %d, %d)}");
// Desc  : ĳ���� ����
// Param :	@AID		INT
//			@CharNum	SMALLINT
//			@Name		VARCHAR(24)
//			@Sex		TINYINT
//			@Hair		INT
//			@face		INT
//			@Costume	INT

TCHAR g_szDB_GET_CHARINFO_BY_AID[] = _T("{CALL spGetCharInfoByCharNum (%d, %d)}");
// Desc  : ĳ���� ���� �޾ƿ���
// Param :	@AID		INT
//			@CharNum	SMALLINT

TCHAR g_szDB_GET_CHAR_EQUIPMENT_INFO_BY_AID[] = _T("{CALL spGetCharEquipmentInfoByAID (%d, %d)}");
// Desc  : �ɸ��Ͱ� ������ ������ ���� �޾ƿ���
// Param :	@AID		INT
//			@CharNum	SMALLINT

TCHAR g_szDB_UPDATE_CHAR_INFO_DATA[] = _T("{CALL spUpdateCharInfoData (%d, %d, %d, %d, %d, %d)}");
// Desc  : �ɸ��� ���� ������Ʈ(XP, BP, Kill, Death, PlayTime)
// Param :	@CID			INT
//			@XPInc			INT
//			@BPInc			INT
//			@KillInc		INT
//			@DeathInc		INT
//			@PlayTimeInc	INT

TCHAR g_szDB_UPDATE_EQUIPITEM[] = _T("{CALL spUpdateEquipItem (%d, %d, %d, %d)}");
// Desc  : ĳ���� ������ ����ϱ�
// Param :  @CID		INT
//			@SlotID		INT
//			@CIID		INT
//			@ItemID		INT

TCHAR g_szDB_CLEARALL_EQUIPEDITEM[] = _T("{CALL spClearAllEquipedItem (%d)}");
// Desc  : ��� ��� ����
// Param :	@CID	INT

TCHAR g_szDB_SELECT_CHAR_ITEM[] = _T("{CALL spSelectCharItem (%d)}");						
// Desc  : ĳ���� ������ ���� ��������
// Param :  @CID	INT

TCHAR g_szDB_DELETE_EXPIRED_CHARACTER_ITEM[]	= _T("{CALL spDeleteExpiredCharacterItem (%d, %d)}");		
// Desc  : ����� ĳ���� ������ ����
// Param :	@CID	INT
//			@CIID	INT


TCHAR g_szDB_UPDATE_CHAR_SPEND_ITEM_COUNT[]	= _T("{CALL spUpdateCharItemSpendCount (%d, %d)}");
// Desc  : �Ҹ� ������ ��� ��, ������ ī��Ʈ ����
// Param :	@CIID		INT
//			@SpendCount INT  

TCHAR g_spChangeGambleItemToRewardNormalItem[] = _T( "{CALL spChangeGambleItemToRewardNormalItem (%u, %u, %u, %u, %u, %u)}" );
// Desc  : �׺� �Ŀ�, �׺� ������ ī��Ʈ ���� �� �Ϲ� ������ ����
// Param :	@CID					INT
//			@CIID					INT
//			@GIID					INT
//			@RewardItemID			INT
//			@RewardRentPeriod		INT

TCHAR g_spChangeGambleItemToRewardSpendableItem[] = _T( "{CALL spChangeGambleItemToRewardSpendableItem (%u, %u, %u, %u, %u, %u)}" );
// Desc  : �׺� �Ŀ�, �׺� ������ ī��Ʈ ���� �� �Ҹ� ������ ����
// Param :	@CID					INT
//			@CIID					INT
//			@GIID					INT
//			@RewardItemID			INT
//			@RewardCount			INT

TCHAR g_szDB_UPDATE_CHAR_GAMBLE_ITEM_DECREASE[]	= _T("{CALL spUpdateCharGambleItemCount(%d, %d, %d)}");
// Desc  : �׺� �Ŀ�, ������ ī��Ʈ ����(ī��Ʈ�� ���� ��ų ���� ���� ����)
// Param :	@CIID			INT
//			@GIID			INT
//			@GambleCount	INT  

TCHAR g_szDB_INSERT_DISTRIBUTE_ITEM[] = _T("{CALL spInsertDistributeItem(%d, %d, %d)}");			
// Desc  : ����Ʈ ������, �ɸ��� ������ ���� ����
// Param :	@CID			INT
//			@ItemID			INT
//			@RentHourPeriod INT

TCHAR g_szDB_BUY_BOUNTY_ITEM[] = _T("{CALL spBuyBountyItem (%d, %d, %d, %d, %d, %d)}");			
// Desc  : �������� ������ ����
// Param :	@AID	INT
//			@CID	INT
//			@AIID	INT
//			@CIID	INT
//			@Count	INT  

TCHAR g_szDB_SELL_ITEM_TO_BOUNTY[]	= _T("{CALL spSellItemToBounty (%d, %d, %d, %d, %d)}");
// Desc  : �������� �ٿ�Ƽ ������ �Ǹ�
// Param :	@CID		INT
//			@CIID		INT
//			@ItemID		INT
//			@Price		INT
//			@CharBP		INT

TCHAR g_szDB_SELL_SPENDABLEITEM_TO_BOUNTY[]	= _T("{CALL spSellSpendableItemToBounty (%d, %d, %d, %d, %d, %d)}");
// Desc  : �������� �ٿ�Ƽ ������ �Ǹ�
// Param :	@CID		INT
//			@CIID		INT
//			@ItemID		INT
//			@ItemCnt	INT
//			@Price		INT
//			@CharBP		INT

TCHAR g_szDB_SELL_CASH_ITEM_TO_BOUNTY[]	= _T("{CALL spSellCashItemToBounty (%d, %d, %d, %d, %d, %d)}");		
// Desc  : �������� ĳ�� ������ �Ǹ�
// Param :	@CID		INT
//			@CIID		INT
//			@ItemID		INT
//			@ItemCnt	INT
//			@Price		INT
//			@CharBP		INT


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created, Modified, Arranged By ȫ����(2011-04-19)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

TCHAR g_szDB_GET_BATTLE_TIME_REWARD_LIST[] = _T("{CALL spGetBattleTimeRewardDescription}");
// Desc  : BattleTime ���� �� �о����

TCHAR g_szDB_GET_BATTLE_TIME_REWARD_ITEM_LIST[] = _T("{CALL spGetBattleTimeRewardItemList}");
// Desc : BattleTime�� ���� ������ ����Ʈ �о����

TCHAR g_szDB_GET_CHAR_BR_INFO_ALL[] = _T("{CALL spGetCharBRInfoAll (%d)}");
// Desc  : BattleTime ���� �ɸ����� ��� ���� �о����
// Param :	@CID		INT

TCHAR g_szDB_GET_CHAR_BR_INFO[] = _T("{CALL spGetCharBRInfo (%d, %d)}");
// Desc  : BattleTime ���� �ɸ����� ���� �о����
// Param :	@CID		INT
//			@BRID		INT

TCHAR g_szDB_INSERT_CHAR_BR_INFO[] = _T("{CALL spInsertCharBRInfo (%d, %d, %d)}");
// Desc  : BattleTime ���� �ɸ����� ���� �����ϱ�
// Param :	@CID		INT
//			@BRID		INT
//			@BRTID		INT

TCHAR g_szDB_UPDATE_CHAR_BR_INFO[] = _T("{CALL spUpdateCharBRInfo (%d, %d, %d, %d, %d, %d)}");
// Desc  : BattleTime ���� �ɸ����� ���� �����ϱ�
// Param :	@CID			INT
//			@BRID			INT
//			@BRTID			INT
//			@RewardCount	INT
//			@BattleTime		INT
//			@KillCount		INT

TCHAR g_szDB_GET_REWARD_CHAR_BR[] = _T("{CALL spRewardCharBattleTimeReward (%d, %d, %d, %d, %d, %d, %d, %d, %d)}");
// Desc  : BattleTime �����ϱ�
// Param :	@CID				INT
//			@BRID				INT
//			@BRTID				INT
//			@BattleTime			INT
//			@KillCount			INT
//			@ItemID				INT
//			@ItemCnt			INT
//			@RentHourPeriod		INT
//			@IsSpendable		INT


MMatchDBMgr::MMatchDBMgr()
{
	m_nExceptionCnt = 0;
	m_strDSNConnect = "";
	m_DB.SetLogCallback( LogCallback );
}

MMatchDBMgr::~MMatchDBMgr()
{
}

bool MMatchDBMgr::CheckOpen()
{
	return m_DB.CheckOpen();
}

CString MMatchDBMgr::BuildDSNString(const CString strDSN, const CString strUserName, const CString strPassword)
{
	m_strDSNConnect = m_DB.BuildDSNString( strDSN, strUserName, strPassword );
	return m_strDSNConnect;
}

bool MMatchDBMgr::Connect()
{
	return Connect(m_strDSNConnect);
}

bool MMatchDBMgr::Connect(CString strDSNConnect)
{
	if( m_DB.Connect(strDSNConnect) )
	{
		m_CountryFilterDBMgr.SetDB( &m_DB );
		return true;
	}

	return false;
}

void MMatchDBMgr::Disconnect()
{
	m_DB.Disconnect();
}


void MMatchDBMgr::Log(const char *pFormat,...)
{
	va_list args;
	char msg[1024];

	va_start(args, pFormat);
	vsprintf(msg, pFormat, args);

	mlog(msg);

	va_end(args);
}

void MMatchDBMgr::ExceptionHandler(CString strSQL, CDBException* e)
{
	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format( "[%c] " );

	Log("\n%s MMatchDBMgr::ExceptionHandler\n", szTime.GetBuffer());
	Log("ErrSQL( %s ), ErrCode( %d )\n", strSQL.GetBuffer(), e->m_nRetCode);
	Log("ErrMsg - %s", e->m_strError);
	Log("%s\n", e->m_strStateNativeOrigin);

	m_nExceptionCnt++;
	if( m_nExceptionCnt > 5 )
	{		
		Log("%s MMatchDBMgr::ExceptionHandler - Disconnect\n", szTime.GetBuffer());
		Disconnect();

		Log("%s MMatchDBMgr::ExceptionHandler - Reconnect\n", szTime.GetBuffer());
		Log("%s MMatchDBMgr::ExceptionHandler - This(%d)\n", szTime.GetBuffer(), this);
		Connect();

		m_nExceptionCnt = 0;
	}
}

void MMatchDBMgr::LogCallback( const string& strLog )
{
	mlog( strLog.c_str() );
}

bool MMatchDBMgr::GetLoginInfo(const TCHAR* szUserID, unsigned int* poutnAID, TCHAR* poutPassword)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format(g_szDB_GET_LOGININFO, szUserID);

	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		bException = true;		
		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	_STATUS_DB_END(0);

	*poutnAID = rs.Field("AID").AsInt();
	strcpy(poutPassword , rs.Field("Password").AsString());
	return true;
}

bool MMatchDBMgr::GetLoginInfo_Netmarble(const TCHAR* szUserID, unsigned int* poutnAID, unsigned int* poutCCode, TCHAR* poutPassword)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format(g_szDB_GET_LOGININFO_NETMARBLE2, szUserID, poutPassword);

	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		bException = true;		
		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	*poutnAID = rs.Field("AID").AsInt();	
	strcpy(poutPassword , rs.Field("Password").AsString());
	*poutCCode	= rs.Field("CCode").AsInt();

	_STATUS_DB_END(0);

	return true;
}

bool MMatchDBMgr::CreateAccount(const TCHAR* szUserID, 
								const TCHAR* szPassword, 
								const int nCert,
								const TCHAR* szName,
								const int nAge, 
								const int nSex)

{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;	

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_CREATE_ACCOUNT, szUserID, szPassword, nCert, szName, nAge, nSex);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {		
		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(1);

	return true;
}

bool MMatchDBMgr::CreateAccount_Netmarble(const TCHAR* szUserID, 
										  const TCHAR* szPassword,
										  const int nAge, 
										  const int nSex,
										  const int nCCode,
										  int *nRet)

{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;	

	CString strSQL;
	strSQL.Format(g_szDB_CREATE_ACCOUNT_NETMARBLE, szUserID, szPassword, nAge, nSex, nCCode);
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {		
		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	*nRet = rs.Field("Ret").AsInt();
	_STATUS_DB_END(1);

	return true;
}

bool MMatchDBMgr::CheckDuplicateCharactername(int* pnOutResult, const TCHAR* szNewName)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);


	try {
		strSQL.Format( g_szCheckDuplicateCharName, szNewName );
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch (CDBException* e) {		
		ExceptionHandler(strSQL, e);
		*pnOutResult = MERR_UNKNOWN;
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() < 0)) {
		*pnOutResult = MERR_UNKNOWN;
		return false;
	}

	// ���� �̸��� ĳ���� �̹� �����ϴ��� üũ
	if( (TRUE == rs.IsOpen()) && (rs.GetRecordCount() > 0) ) {
		*pnOutResult = MERR_CLIENT_EXIST_CHARNAME;
		return false;
	}

	*pnOutResult = MOK;
	_STATUS_DB_END(2);

	return true;
}

bool MMatchDBMgr::CreateCharacter(int* pnOutResult,
								  const int nAID, const TCHAR* szNewName, const int nCharIndex, const int nSex,
								  const int nHair, const int nFace, const int nCostume)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_CREATE_CHAR, nAID, nCharIndex, szNewName,  nSex, nHair, nFace, nCostume);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {		
		ExceptionHandler(strSQL, e);
		*pnOutResult = MERR_UNKNOWN;
		return false;
	}

	int nRet = rs.Field("Ret").AsInt();
	if( nRet < 0 ) {
		Log("MMatchDBMgr::CreateCharacter - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::CreateCharacter - %s\n", strSQL.GetBuffer());
		*pnOutResult = MERR_UNKNOWN;
		return false;
	}

	*pnOutResult = MOK;
	_STATUS_DB_END(2);

	return true;
}

bool MMatchDBMgr::GetAccountCharList(const int nAID, MTD_AccountCharInfo* poutCharList, int* noutCharCount)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;


	CString strSQL;
	strSQL.Format(g_szDB_GET_CHARLIST, nAID);
	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;		
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE)
	{
		return false;
	}

	int t = 0;
	for( ; ! rs.IsEOF(); rs.MoveNext() )
	{
		if (t >= 4) break;

		memset(&poutCharList[t], 0, sizeof(MTD_AccountCharInfo));

		strcpy(poutCharList[t].szName, (LPCTSTR)rs.Field("Name").AsString());
		//strcpy(poutCharList[t].szClanName, (LPCTSTR)rs.Field("ClanName").AsString());

		poutCharList[t].nCharNum = rs.Field("CharNum").AsInt();
		poutCharList[t].nLevel = rs.Field("Level").AsInt();
		t++;
	}

	*noutCharCount = t;

	_STATUS_DB_END(3);

	return true;
}

bool MMatchDBMgr::GetAccountCharInfo(const int nAID, const int nCharIndex, MTD_CharInfo* poutCharInfo)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;	
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_ACCOUNT_CHARINFO, nAID, nCharIndex);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;

	int t = 0;

	memset(poutCharInfo, 0, sizeof(MTD_CharInfo));

	strcpy(poutCharInfo->szName, (LPCTSTR)rs.Field("Name").AsString());
	strcpy(poutCharInfo->szClanName, (LPCTSTR)rs.Field("ClanName").AsString());

	poutCharInfo->nCharNum = rs.Field("CharNum").AsInt();
	poutCharInfo->nLevel = rs.Field("Level").AsInt();
	poutCharInfo->nSex = rs.Field("Sex").AsInt();
	poutCharInfo->nHair = rs.Field("Hair").AsInt();
	poutCharInfo->nFace = rs.Field("Face").AsInt();
	poutCharInfo->nXP = (unsigned long)rs.Field("XP").AsLong();
	poutCharInfo->nBP = (unsigned long)rs.Field("BP").AsLong();	
	poutCharInfo->nDTLastWeekGrade = -1;

	_STATUS_DB_END(50);

	return true;
}

bool MMatchDBMgr::GetCharInfoByAID( const int nAID, const int nCharIndex, MMatchCharInfo* poutCharInfo )
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;	

	CString strSQL;	
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_CHARINFO_BY_AID, nAID, nCharIndex);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;

	CString db_strName = rs.Field("Name").AsString();
	strcpy(poutCharInfo->m_szName, (LPCTSTR)db_strName);

	poutCharInfo->m_nCharNum = (int)rs.Field("CharNum").AsInt();

	poutCharInfo->m_nCID	= (unsigned long)rs.Field("CID").AsLong();	
	poutCharInfo->m_nLevel	= (int)rs.Field("Level").AsInt();
	poutCharInfo->m_nSex	= (MMatchSex)rs.Field("Sex").AsInt();
	poutCharInfo->m_nHair	= rs.Field("Hair").AsInt();
	poutCharInfo->m_nFace	= rs.Field("Face").AsInt();

	poutCharInfo->m_nXP	= (unsigned long)rs.Field("XP").AsLong();
	poutCharInfo->m_nBP	= (unsigned long)rs.Field("BP").AsLong();
	poutCharInfo->m_nHP = (int)rs.Field("HP").AsInt();
	poutCharInfo->m_nAP = (int)rs.Field("AP").AsInt();
	poutCharInfo->m_nFR = (int)rs.Field("FR").AsInt();
	poutCharInfo->m_nCR = (int)rs.Field("CR").AsInt();
	poutCharInfo->m_nER = (int)rs.Field("ER").AsInt();
	poutCharInfo->m_nWR = (int)rs.Field("WR").AsInt();

	poutCharInfo->m_nTotalKillCount		= rs.Field("KillCount").AsInt();
	poutCharInfo->m_nTotalDeathCount	= rs.Field("DeathCount").AsInt();
	poutCharInfo->m_nTotalPlayTimeSec	= rs.Field("PlayTime").AsInt();

	// clan
	CString db_strClanName = rs.Field("ClanName").AsString();
	strcpy(poutCharInfo->m_ClanInfo.m_szClanName, (LPCTSTR)db_strClanName);

	poutCharInfo->m_ClanInfo.m_nClanID = (int)rs.Field("CLID").AsInt();

	if( -1 != rs.GetFieldID("rank") ) {
#ifdef _DEBUG
		mlog( "rank : %d\n", poutCharInfo->m_nRank );
#endif
		poutCharInfo->m_nRank = (unsigned int)rs.Field("rank").AsInt();
	} else {
#ifdef _DEBUG
		mlog( "can't find rank column\n" );
#endif
		poutCharInfo->m_nRank = 0;
	}

	poutCharInfo->m_ClanInfo.m_nGrade = (MMatchClanGrade)rs.Field("ClanGrade").AsInt();
	poutCharInfo->m_ClanInfo.m_nContPoint = (MMatchClanGrade)rs.Field("ClanContPoint").AsInt();

	_STATUS_DB_END(4);

	return true;
}

bool MMatchDBMgr::GetCharEquipmentInfoByAID(const int nAID, const int nCharIndex, unsigned int nEquipedItemID[MMCIP_END], unsigned int nEquipedItemCIID[MMCIP_END])
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;	
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_CHAR_EQUIPMENT_INFO_BY_AID, nAID, nCharIndex);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) { return false; }


	memset(nEquipedItemCIID, 0, sizeof(unsigned int) * MMCIP_END);
	memset(nEquipedItemID, 0, sizeof(unsigned int) * MMCIP_END);

	MMatchCharItemParts nParts;	
	for( ; ! rs.IsEOF(); rs.MoveNext() ) {
		nParts = MMatchCharItemParts(rs.Field("SlotID").AsInt());
		if( nParts < MMCIP_HEAD || nParts >= MMCIP_END ) {
			_ASSERT(0);
			continue;
		}

		nEquipedItemID[nParts] = rs.Field("ItemID").AsInt();
		nEquipedItemCIID[nParts] = rs.Field("CIID").AsInt();
	}
	_STATUS_DB_END(4);

	return true;
}

bool MMatchDBMgr::GetAccountInfo( const unsigned long int nAID, MMatchAccountInfo* poutAccountInfo, const int nServerID )
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;


	CString strSQL;
	strSQL.Format(g_szDB_GET_ACCOUNTINFO, nAID, nServerID);

	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		return false;
	}

	poutAccountInfo->m_nAID			= (unsigned long)rs.Field("AID").AsLong();
	poutAccountInfo->m_nUGrade		= (MMatchUserGradeID)rs.Field("UGradeID").AsInt();
	poutAccountInfo->m_nPGrade		= MMPG_FREE;
	strcpy(poutAccountInfo->m_szUserID, (LPCTSTR)rs.Field("UserID").AsString());

	poutAccountInfo->m_bIsPowerLevelingHacker = (1 == rs.Field("IsPowerLevelingHacker").AsInt());
	if( poutAccountInfo->m_bIsPowerLevelingHacker )
	{
		poutAccountInfo->m_nPowerLevelingRegTimeMin = (timeGetTime() / 60000) -
			static_cast<DWORD>(rs.Field("PowerLevelingRegPassedTimeMin").AsInt());
	}
	else
	{
		poutAccountInfo->m_nPowerLevelingRegTimeMin = 0;
	}

#ifdef _BLOCK_HACKER
	if( rs.Field("HackingType").IsNull() )
		poutAccountInfo->m_HackingType = MMHT_NO;
	else 
	{
		poutAccountInfo->m_HackingType = static_cast< MMatchHackingType >( rs.Field("HackingType").AsInt() );
		if( MMHT_NO != poutAccountInfo->m_HackingType || MMHT_SLEEP_ACCOUNT != poutAccountInfo->m_HackingType )
		{
			poutAccountInfo->m_EndBlockDate.wYear	= static_cast<WORD>(rs.Field("HackBlockYear").AsShort());
			poutAccountInfo->m_EndBlockDate.wMonth	= static_cast<WORD>(rs.Field("HackBlockMonth").AsShort());
			poutAccountInfo->m_EndBlockDate.wDay	= static_cast<WORD>(rs.Field("HackBlockDay").AsShort());
			poutAccountInfo->m_EndBlockDate.wHour	= static_cast<WORD>(rs.Field("HackBlockHour").AsShort());
			poutAccountInfo->m_EndBlockDate.wMinute	= static_cast<WORD>(rs.Field("HackBlockMin").AsShort());

			poutAccountInfo->m_dwHackingBlockEndTimeMS = timeGetTime() 
				+ (static_cast<DWORD>(rs.Field("HackingBlockTimeRemainderMin").AsInt()) * 60000);
		}
	}
#endif

	_STATUS_DB_END(5);

	return true;
}

bool MMatchDBMgr::DeleteCharacter(const int nAID, const nCharIndex, const TCHAR* szCharName)
{
	_STATUS_DB_START;

	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format(g_szDB_DELETE_CHAR, nAID, nCharIndex, szCharName);

	CODBCRecordset rs(&m_DB);

	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		return false;
	}

	int nRet = 0;
	nRet = rs.Field("Ret").AsInt();
	if (nRet != 1) return false;

	_STATUS_DB_END(6);
	return true;

}


bool MMatchDBMgr::SimpleUpdateCharInfo(MMatchCharInfo* pCharInfo)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_SIMPLE_UPDATE_CHARINFO, pCharInfo->m_nCID, pCharInfo->m_szName,
			pCharInfo->m_nLevel, pCharInfo->m_nXP, pCharInfo->m_nBP);

		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(7);
	return true;

}

bool MMatchDBMgr::BuyBountyItem(const unsigned int nCID, int nItemID, int nItemCount, int nPrice, const DWORD dwRentHourPeriod, const bool bIsSpendableItem, unsigned long int* poutCIID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_BUY_BOUNTY_ITEM, nCID, nItemID, nItemCount, nPrice, bIsSpendableItem, dwRentHourPeriod);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	_STATUS_DB_END(55);

	int nRet = rs.Field("Ret").AsInt();
	if( nRet < 0 ) {
		Log("MMatchDBMgr::BuyBountyItem - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::BuyBountyItem - %s\n", strSQL.GetBuffer());
		*poutCIID = 0;
		return false;
	}

	*poutCIID = rs.Field("ORDERCIID").AsLong();	
	return true;
}

bool MMatchDBMgr::GetCharItemInfo(MMatchCharInfo* pCharInfo)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;	
	CODBCRecordset rs(&m_DB);

	bool bException = false;

	try {
		strSQL.Format(g_szDB_SELECT_CHAR_ITEM, pCharInfo->m_nCID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		bException = true;

		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) { return false; }

	int		nItemDescID					= 0;
	DWORD	dwCIID						= 0;
	WORD	wMaxUsableHour				= RENT_PERIOD_UNLIMITED;
	bool	bIsRentItem					= false;
	int		nRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;

	// ���� �������� ���� �κ��̶� ������ �Ǹ� �ȵȴ�. - by SungE 2007-06-14
	const bool	bSpendingItem	= false;	// �Һ������ ���� (������ ������ false - ���� ������ �������� ����)
	int	nCnt			= -1;

	for( ; ! rs.IsEOF(); rs.MoveNext() ) {
		nItemDescID		= (int)rs.Field("ItemID").AsInt();
		dwCIID			= (DWORD)rs.Field("CIID").AsInt();		

		if( rs.Field("Cnt").IsNull() ) 	nCnt = 1;
		else							nCnt = rs.Field("Cnt").AsInt();


		if( nCnt <= 0 ) { continue; }										///< �� �����۵� ����
		if(  300010 < nItemDescID && 300040 > nItemDescID) { continue; }	///< �� �����۵��� ��ŷ���� �̾Ƴ��߸��Ⱦ�����		

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
				mlog( "Renewal - MMatchDBMgr::GetCharItemInfo - CID(%u), CIID(%u), ItemID(%u), ItemCount(%d), RentHourPeriod(%d)\n"
					, pCharInfo->m_nCID, dwCIID, nItemDescID, nCnt, nRentMinutePeriodRemainder );
				ASSERT( 0 );

				bIsRentItem = false;
				wMaxUsableHour = RENT_PERIOD_UNLIMITED;
				nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED;
			}

			if( dwCIID <= 0 ) {
				mlog( "Renewal - MMatchDBMgr::GetCharItemInfo - Invalid CIID(%d) - CID(%u), ItemID(%u), ItemCount(%d), RentHourPeriod(%d)\n"
					, dwCIID, pCharInfo->m_nCID, nItemDescID, nCnt, nRentMinutePeriodRemainder );
				ASSERT( 0 );
				continue;
			}

			pCharInfo->m_ItemList.CreateItem(MMatchItemMap::UseUID(), dwCIID, nItemDescID, bIsRentItem, nRentMinutePeriodRemainder, wMaxUsableHour, nCnt);

		} 
		else if( NULL != MGetMatchServer()->GetGambleMachine().GetGambleItemByGambleItemID(nItemDescID) )	//< Gamble Item
		{	
			if( !pCharInfo->m_GambleItemManager.AddGambleItem(MMatchItemMap::UseUID(), dwCIID, nItemDescID, nCnt) ) {
				mlog( "invalid gamble item. CID(%u), CIID(%u), ItemID(%u), ItemCount(%d)\n", pCharInfo->m_nCID, dwCIID, nItemDescID, nCnt );
				ASSERT( 0 );
				return false;
			}			
		} else { ASSERT( 0 ); }
	}

	pCharInfo->m_ItemList.SetDbAccess(true);	// ��񿡼� �����Դٰ� �˸�

	_STATUS_DB_END(10);
	return true;
}

bool MMatchDBMgr::InsertConnLog(const int nAID, const char* szIP, const string& strCountryCode3)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	if( 0 == szIP ) return false;

	// char szShortIP[64];
	// MConvertCompactIP(szShortIP, szIP);
	vector< BYTE > vIP;
	if( !SplitStrIP(szIP, vIP) || (4 != vIP.size()) ) 
	{
		mlog( "MMatchDBMgr::InsertConnLog - ���ڿ� IP(%s)���� ����. AID(%u)\n", 
			nAID, szIP );
		return false;
	}

	CString strSQL;

	try {
		strSQL.Format( g_szDB_INSERT_CONN_LOG, nAID, vIP[0], vIP[1], vIP[2], vIP[3], strCountryCode3.c_str() );
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(12);
	return true;
}

bool MMatchDBMgr::InsertGameLog(const unsigned int nMasterCID, const char* szMap, const char* szGameType, unsigned int* poutID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_INSERT_GAME_LOG, nMasterCID, szMap, szGameType);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) return false;

	*poutID = rs.Field("GameLogID").AsInt();

	_STATUS_DB_END(13);
	return true;
}

bool MMatchDBMgr::InsertGamePlayerLog(int nGameLogID, int nCID, int nPlayTime, int nKills, int nDeaths, int nXP, int nBP)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_INSERT_GAME_PLAYER_LOG, nGameLogID, nCID, nPlayTime, nKills, nDeaths, nXP, nBP);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(13);
	return true;
}

/*
bool MMatchDBMgr::InsertGameLog(const char* szGameName, const char* szMap, const char* szGameType,
								const int nRound, const unsigned int nMasterCID, 
								const int nPlayerCount, const char* szPlayers)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	try {
		string strStageName = m_DBFilter.Filtering( string(szGameName) );
		strSQL.Format(g_szDB_INSERT_GAME_LOG, &strStageName[0], szMap, szGameType, nRound, nMasterCID, nPlayerCount, szPlayers);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(13);
	return true;
}
*/
bool MMatchDBMgr::InsertKillLog(const unsigned int nAttackerCID, const unsigned int nVictimCID)
{
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_INSERT_KILL_LOG, nAttackerCID, nVictimCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	return true;
}

bool MMatchDBMgr::InsertChatLog(const unsigned long int nCID, const char* szMsg, unsigned long int nTime)
{
	return true;

	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format("INSERT Into ChatLog (CID, Msg, Time) Values (%u, '%s', GETDATE())", nCID, szMsg);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	return true;
}


// ĳ���� ���� ������Ʈ
bool MMatchDBMgr::UpdateCharLevel(const int nCID, const int nLevel)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_LEVEL, nLevel, nCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(14);
	return true;
}

// bp ������Ʈ
bool MMatchDBMgr::UpdateCharBP(const int nCID, const int nBPInc)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;


#ifdef _DEBUG
	mlog( "MMatchDBMgr::UpdateCharBP - Added bounty price:%d\n", nBPInc );
#endif

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_BP, nBPInc, nCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(15);
	return true;
}

bool MMatchDBMgr::InsertItemPurchaseLogByBounty(const unsigned long int nItemID, const unsigned long int nCID,
												const int nBounty, const int nCharBP, const _ITEMPURCHASE_TYPE nType)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;



	char szType[8] = "";

	switch (nType)
	{
	case IPT_BUY: strcpy(szType, "����"); break;
	case IPT_SELL: strcpy(szType, "�Ǹ�"); break;
	}

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_INSERT_ITEM_PURCHASE_BY_BOUNTY_LOG, nItemID, nCID, nBounty, nCharBP, szType);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(16);
	return true;

}

bool MMatchDBMgr::InsertCharMakingLog(const unsigned int nAID, const char* szCharName,
									  const _CHARMAKING_TYPE nType)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;



	char szType[8] = "";

	switch (nType)
	{
	case CMT_CREATE: strcpy(szType, "����"); break;
	case CMT_DELETE: strcpy(szType, "����"); break;
	}


	CString strSQL;

	try {
		strSQL.Format(g_szDB_INSERT_CHAR_MAKING_LOG, nAID, szCharName, szType);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(17);
	return true;
}


bool MMatchDBMgr::InsertServerLog(const int nServerID, const int nPlayerCount, const int nGameCount, const DWORD dwBlockCount, const DWORD dwNonBlockCount)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_INSERT_SERVER_LOG, nServerID, nPlayerCount, nGameCount, dwBlockCount, dwNonBlockCount);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(18);
	return true;
}

bool MMatchDBMgr::UpdateServerStatus(const int nServerID, const int nCurrPlayer)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_SERVER_STATUS, nCurrPlayer, nServerID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(19);
	return true;
}

bool MMatchDBMgr::UpdateServerStatus_Netmarble(const int nServerID, const int nCurrPlayer, const int nNatePlayer)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_SERVER_STATUS_NETMARBLE, nCurrPlayer, nNatePlayer, nServerID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(19);
	return true;
}

bool MMatchDBMgr::UpdateServerInfo(const int nServerID, const int nMaxPlayer, const char* szServerName)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_SERVER_INFO, nMaxPlayer, szServerName, nServerID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(20);
	return true;
}

bool MMatchDBMgr::InsertPlayerLog(const unsigned long int nCID,
								  const int nPlayTime, const int nKillCount, const int nDeathCount, const int nXP, const int nTotalXP)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_INSERT_PLAYER_LOG, nCID, nPlayTime, nKillCount, nDeathCount, nXP, nTotalXP);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(21);
	return true;
}

bool MMatchDBMgr::InsertNetmarbleTPLog(const unsigned long int nCID, const int nPlayTime)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_INSERT_NETMARBLE_TP_LOG, nCID, nPlayTime);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(21);
	return true;
}

bool MMatchDBMgr::InsertLevelUpLog(const int nCID, const int nLevel, const int nBP, 
								   const int nKillCount, const int nDeathCount, const int nPlayTime)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_INSERT_LEVELUP_LOG, nCID, nLevel, nBP, nKillCount, nDeathCount, nPlayTime);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(41);
	return true;

}

bool MMatchDBMgr::UpdateCharPlayTime(const unsigned long int nCID, const unsigned long int nPlayTime)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_PLAYTIME, nPlayTime, nCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(22);
	return true;
}

bool MMatchDBMgr::UpdateCharInfoData(const int nCID, const int nAddedXP, const int nAddedBP, 
									 const int nAddedKillCount, const int nAddedDeathCount, const int nAddedPlayTime)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_INFO_DATA, nCID, nAddedXP, nAddedBP, 
			nAddedKillCount, nAddedDeathCount, nAddedPlayTime);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(23);
	return true;
}

bool MMatchDBMgr::UpdateLastConnDate(const TCHAR* szUserID, const TCHAR* szIP)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_LAST_CONNDATE, szIP, szUserID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(24);
	return true;
}

bool MMatchDBMgr::ClearAllEquipedItem(const unsigned long nCID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_CLEARALL_EQUIPEDITEM, nCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(27);
	return true;


}

bool MMatchDBMgr::GetCharCID(const TCHAR* pszName, int* poutCID)
{
	_STATUS_DB_START;

	if (!CheckOpen()) return false;
	CODBCRecordset rs(&m_DB);

	CString strSQL;

	try	{		
		strSQL.Format(g_szGetCIDbyCharName, pszName);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch (CDBException* e)	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	// �����ϴ� ĳ���̸����� üũ
	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	*poutCID = rs.Field("CID").AsInt();

	_STATUS_DB_END(28);
	return true;
}

//// Friends ////
bool MMatchDBMgr::FriendAdd(const int nCID, const int nFriendCID, const int nFavorite)
{
	_STATUS_DB_START;

	if (!CheckOpen()) return false;
	CODBCRecordset rs(&m_DB);

	// ģ�� �߰�
	CString strSQL;
	strSQL.Format(g_szDB_ADD_FRIEND, nCID, nFriendCID, nFavorite);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(29);
	return true;
}

bool MMatchDBMgr::FriendRemove(const int nCID, const int nFriendCID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_REMOVE_FRIEND, nCID, nFriendCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(30);
	return true;
}

bool MMatchDBMgr::FriendGetList(const int nCID, MMatchFriendInfo* pFriendInfo)
{
#define MAX_FRIEND_LISTCOUNT 30

	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format(g_szDB_GET_FRIEND_LIST, nCID);
	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE)
	{
		return false;
	}

	int nFriendCID=0, nFavorite=0;
	int nodecount = 0;
	CString strName;

	for( ; ! rs.IsEOF(); rs.MoveNext() )
	{
		nFriendCID = (int)rs.Field("FriendCID").AsInt();
		nFavorite = (int)rs.Field("Favorite").AsInt();
		strName = rs.Field("Name").AsString();

		pFriendInfo->Add(nFriendCID, nFavorite, (LPCTSTR)strName);
	}

	_STATUS_DB_END(31);
	return true;
}


bool MMatchDBMgr::GetCharClan(const int nCID, int* poutClanID, TCHAR* poutClanName)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format(g_szDB_GET_CHAR_CLAN, nCID);

	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		return false;
	}

	*poutClanID = rs.Field("CLID").AsInt();
	strcpy(poutClanName, (LPCTSTR)rs.Field("ClanName").AsString());

	_STATUS_DB_END(33);

	return true;
}

bool MMatchDBMgr::GetClanIDFromName(const TCHAR* szClanName, int* poutCLID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format(g_szDB_GET_CLID_FROM_CLANNAME, szClanName);

	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;

		ExceptionHandler(strSQL, e);

		*poutCLID = 0;
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		*poutCLID = 0;
		return false;
	}

	*poutCLID = rs.Field("CLID").AsInt();

	_STATUS_DB_END(34);

	return true;
}

bool MMatchDBMgr::CreateClan(const TCHAR* szClanName, const int nMasterCID, const int nMember1CID, const int nMember2CID,
							 const int nMember3CID, const int nMember4CID, bool* boutRet, int* noutNewCLID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;


	CString strSQL;
	strSQL.Format(g_szDB_CREATE_CLAN, szClanName, nMasterCID, nMember1CID, nMember2CID, nMember3CID, nMember4CID);

	CODBCRecordset rs(&m_DB);


	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;

		ExceptionHandler(strSQL, e);
		return false;

		*boutRet = false;
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		*boutRet = false;
		return false;
	}

	*boutRet = rs.Field("Ret").AsBool();
	*noutNewCLID = rs.Field("NewCLID").AsInt();


	_STATUS_DB_END(35);
	return true;


}

bool MMatchDBMgr::ReserveCloseClan(const int nCLID, const TCHAR* szClanName, const int nMasterCID, const string& strDeleteDate )
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_RESERVE_CLOSE_CLAN, nCLID, szClanName, nMasterCID, strDeleteDate.c_str() );
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(36);
	return true;
}

bool MMatchDBMgr::AddClanMember(const int nCLID, const int nJoinerCID, const int nClanGrade, bool* boutRet)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);
	bool bException = false;

	try {
		strSQL.Format(g_szDB_ADD_CLAN_MEMBER, nCLID, nJoinerCID, nClanGrade);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		bException = true;

		ExceptionHandler(strSQL, e);

		*boutRet = false;
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		*boutRet = false;
		return false;
	}

	*boutRet = rs.Field("Ret").AsBool();


	_STATUS_DB_END(37);
	return true;

}

bool MMatchDBMgr::RemoveClanMember(const int nCLID, const int nLeaverCID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_REMOVE_CLAN_MEMBER, nCLID, nLeaverCID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(38);
	return true;


}


bool MMatchDBMgr::UpdateClanGrade(const int nCLID, const int nMemberCID, const int nClanGrade)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_CLAN_GRADE, nCLID, nMemberCID, nClanGrade);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(39);
	return true;

}

bool MMatchDBMgr::ExpelClanMember(const int nCLID, const int nAdminGrade, TCHAR* szMember, int* noutRet)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;


	CString strSQL;
	strSQL.Format(g_szDB_EXPEL_CLAN_MEMBER, nCLID, nAdminGrade, szMember);

	CODBCRecordset rs(&m_DB);


	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;

		ExceptionHandler(strSQL, e);

		*noutRet = ER_NO_MEMBER;
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		*noutRet = ER_NO_MEMBER;
		return false;
	}

	*noutRet = rs.Field("Ret").AsInt();

	/* Ret�� ���� : 1 - ����, 0 - �ش�Ŭ������ ����. , 2 - ������ ���� �ʴ�. */

	_STATUS_DB_END(40);
	return true;
}



bool MMatchDBMgr::GetLadderTeamID(const int nTeamTableIndex, const int* pnMemberCIDArray, const int nMemberCount, int* pnoutTID)
{
	_STATUS_DB_START;

	if (!CheckOpen()) return false;


	CString strSQL;

	// ����� 4�� ���ۿ� �������� �ʴ´�.
	if ((nTeamTableIndex == 4) && (nMemberCount == 4))
	{
		strSQL.Format(g_szDB_GET_LADDER_TEAM4_ID, pnMemberCIDArray[0], pnMemberCIDArray[1], pnMemberCIDArray[2], pnMemberCIDArray[3]);
	}

#ifdef _DEBUG
	else if ((nTeamTableIndex == 1) && (nMemberCount == 1))
	{
		// for test
		strSQL.Format(g_szDB_GET_LADDER_TEAM4_ID, pnMemberCIDArray[0], 1, 1, 1);
	}
	else if ((nTeamTableIndex == 2) && (nMemberCount == 2))
	{
		// for test
		strSQL.Format(g_szDB_GET_LADDER_TEAM4_ID, pnMemberCIDArray[0], pnMemberCIDArray[1], 1, 1);
	}
#endif
	else
	{
		return false;
	}

	CODBCRecordset rs(&m_DB);


	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		*pnoutTID = 0;
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		*pnoutTID = 0;
		return false;
	}

	char szFieldName[64] = "";

	//#ifdef _DEBUG
	strcpy(szFieldName, "TID4");	// ������ ��δ� TID4�� ������
	//#else
	//	sprintf(szFieldName, "TID%d", nTeamTableIndex);
	//#endif

	*pnoutTID = rs.Field(szFieldName).AsInt();


	_STATUS_DB_END(42);
	return true;


}


bool MMatchDBMgr::LadderTeamWinTheGame(const int nTeamTableIndex, const int nWinnerTID, const int nLoserTID, const bool bIsDrawGame,
									   const int nWinnerPoint, const int nLoserPoint, const int nDrawPoint)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	int nDrawGame = 0;
	if (bIsDrawGame) nDrawGame = 1;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_TEAM4_WIN_THE_GAME, nWinnerTID, nLoserTID, nDrawGame, nWinnerPoint, nLoserPoint, nDrawPoint);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(43);
	return true;
}

bool MMatchDBMgr::GetLadderTeamMemberByCID(const int nCID, int* poutTeamID, char** ppoutCharArray, int nCount)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CODBCRecordset rs(&m_DB);

	CString strSQL;

	try {
		strSQL.Format(g_szDB_GET_LADDER_TEAM_MEMBERLIST, nCID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		return false;
	}

	int nTeamID = rs.Field("TID4").AsInt();
	if (poutTeamID)
		*poutTeamID = nTeamID;

	int nIndex = 0;
	for( ; ! rs.IsEOF(); rs.MoveNext() )
	{
		if (nIndex >= nCount) break;

		CString strName = rs.Field("Name").AsString();
		if (ppoutCharArray)
			strcpy(ppoutCharArray[nIndex], (LPCTSTR)strName);
		nIndex++;
	}

	_STATUS_DB_END(44);
	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
bool MMatchDBMgr::UpdateCharLevel(const int nCID, const int nNewLevel, const int nBP, const int nKillCount, 
								  const int nDeathCount, const int nPlayTime, bool bIsLevelUp)
{
	bool ret = UpdateCharLevel(nCID, nNewLevel);

	if ((ret == true) && (bIsLevelUp) && (nNewLevel >= 10))		// 10�����̻� ���� �ø������� �α׸� �����.
	{

		InsertLevelUpLog(nCID, nNewLevel, nBP, nKillCount, nDeathCount, nPlayTime);
	}

	return ret;
}

bool MMatchDBMgr::UpdateCharPlayInfo(const int nAID, const int nCID, const int nXP, const int nLevel, const int nPlayingTimeSec, const int nTotalPlayTimeSec, 
									 const int nTotalKillCount, const int nTotalDeathCount, const int nBP, bool bIsLevelUp)
{
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_PLAYINFO_LOG, nAID, nCID, nXP, nLevel, nTotalPlayTimeSec, nPlayingTimeSec, nTotalKillCount, nTotalDeathCount, nBP, bIsLevelUp);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	return true;
}

bool MMatchDBMgr::GetClanInfo(const int nCLID, MDB_ClanInfo* poutClanInfo)
{
	_STATUS_DB_START;

	if (!CheckOpen()) return false;
	CODBCRecordset rs(&m_DB);

	CString strSQL;

	try {		
		strSQL.Format(g_szDB_GET_CLAN_INFO, nCLID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch (CDBException* e)	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	poutClanInfo->nCLID = rs.Field("CLID").AsInt();
	poutClanInfo->nLevel = rs.Field("Level").AsInt();
	strcpy(poutClanInfo->szClanName , rs.Field("Name").AsString());
	strcpy(poutClanInfo->szMasterName , rs.Field("ClanMaster").AsString());
	strcpy(poutClanInfo->szEmblemUrl , rs.Field("EmblemUrl").AsString());
	poutClanInfo->nTotalPoint = rs.Field("TotalPoint").AsInt();
	poutClanInfo->nPoint = rs.Field("Point").AsInt();
	poutClanInfo->nWins = rs.Field("Wins").AsInt();
	poutClanInfo->nLosses = rs.Field("Losses").AsInt();
	poutClanInfo->nTotalMemberCount = rs.Field("MemberCount").AsInt();
	poutClanInfo->nRanking = rs.Field("Ranking").AsInt();
	poutClanInfo->nEmblemChecksum = rs.Field("EmblemChecksum").AsInt();


	_STATUS_DB_END(45);
	return true;


}

bool MMatchDBMgr::WinTheClanGame(const int nWinnerCLID, const int nLoserCLID, const bool bIsDrawGame,
								 const int nWinnerPoint, const int nLoserPoint, const char* szWinnerClanName,
								 const char* szLoserClanName, const int nRoundWins, const int nRoundLosses,
								 const int nMapID, const int nGameType,
								 const char* szWinnerMembers, const char* szLoserMembers)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	int nDrawGame = 0;
	if (bIsDrawGame) nDrawGame = 1;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_WIN_THE_CLAN_GAME, nWinnerCLID, nLoserCLID, nDrawGame, nWinnerPoint, nLoserPoint,
			szWinnerClanName, szLoserClanName, nRoundWins, nRoundLosses, nMapID, nGameType,
			szWinnerMembers, szLoserMembers);

		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(46);
	return true;
}

bool MMatchDBMgr::UpdateCharClanContPoint(const int nCID, const int nCLID, const int nAddedContPoint)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_UPDATE_CHAR_CLAN_CONTPOINT, nCID, nCLID, nAddedContPoint);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(47);
	return true;
}

bool MMatchDBMgr::EventJjangUpdate(const int nAID, bool bJjang)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {
		MMatchUserGradeID nGrade = bJjang ? MMUG_STAR : MMUG_FREE;
		strSQL.Format(g_szDB_EVENT_JJANG_UPDATE, nGrade, nAID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(48);
	return true;
}

#define QUEST_ITEM_NUM_SIZE			sizeof( int )
#define SIZE_OF_SIMPLE_QUEST_ITEM	sizeof(SimpleQuestItem)


bool MMatchDBMgr::UpdateQuestItem( int nCID, MQuestItemMap& rfQuestIteMap, MQuestMonsterBible& rfQuestMonster )
{
	_STATUS_DB_START;

	CString strSQL;

	try
	{
		// 7000byte�� �Ѵ��� �˻���.
		if( QUEST_DATA < MCRC32::SIZE + MAX_DB_QUEST_ITEM_SIZE + MAX_DB_MONSTERBIBLE_SIZE )
		{
			ASSERT( 0 );
			Log( "MMatchDBMgr::UpdateQuestItem - �������� ũ�Ⱑ %d���� ŭ.", QUEST_DATA );
			return false;
		}

		unsigned char			szData[ QUEST_DATA ];
		unsigned char			szMonData[ MAX_DB_MONSTERBIBLE_SIZE ];
		MCRC32::crc_t			Crc32;
		int						i;
		MQuestItemMap::iterator	it, end;

		memset( szData, 0, QUEST_DATA );
		memset( szMonData, 0, MAX_DB_MONSTERBIBLE_SIZE );

		// ����Ʈ ������ ���� ����.
		end = rfQuestIteMap.end();
		for( i = 0, it = rfQuestIteMap.begin(); it != end; ++it )
		{
			MQuestItem* pQuestItem = it->second;

			// DB�� ������ �Ǵ� QuestItemŸ������ �˻�. ���� �������� �������� ����ɼ� ����.
			if( GetQuestItemDescMgr().IsQItemID(it->first) )
			{
				// �迭�� �ε����� 0���� ������ �ϰ�, ����Ʈ �������� ItemID�� 1���� �����Ѵ�.
				int nIndex = MCRC32::SIZE + pQuestItem->GetItemID() - MIN_QUEST_ITEM_ID;
				unsigned char nValue = 0;
				if (pQuestItem->GetCount() > 0) 
				{
					nValue = pQuestItem->GetCount() + MIN_QUEST_DB_ITEM_COUNT;
				}
				else if (pQuestItem->IsKnown()) 
				{
					nValue = MIN_QUEST_DB_ITEM_COUNT;
				}
				else nValue = 0;

				szData[ nIndex ] = 
					static_cast< unsigned char >( nValue );
			}
		}

		// ���� ���� ����.
		memcpy( szData + MCRC32::SIZE + MAX_DB_QUEST_ITEM_SIZE, &rfQuestMonster, MAX_DB_MONSTERBIBLE_SIZE );

		// make crc checksum.
		Crc32 = MCRC32::BuildCRC32( reinterpret_cast<BYTE*>(szData + MCRC32::SIZE), MAX_DB_QUEST_ITEM_SIZE + MAX_DB_MONSTERBIBLE_SIZE );
		if( 0 == Crc32 )
		{
			mlog( "MMatchDBMgr::UpdateQuestItem - CRC32 is 0 CID:%d\n", nCID );
			// ���� ���⼭ ������ ����� ���̶��, �̺κп� �������� ��� �����͸� �˻��� �����. 
		}

		// copy crc checksum.
		memcpy( szData, &Crc32, MCRC32::CRC::SIZE );

		strSQL.Format( "UPDATE Character SET QuestItemInfo = (?) WHERE CID = %d", nCID );

		CODBCRecordset rs( &m_DB );
		if( !rs.InsertBinary(strSQL, szData, MCRC32::CRC::SIZE + MAX_DB_QUEST_ITEM_SIZE + MAX_DB_MONSTERBIBLE_SIZE) )
		{
			Log( "MMatchDBMgr::UpdateQuestItem - Querry fail." );
			return false;
		}
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(51);

	return true;
}

bool MMatchDBMgr::GetCharQuestItemInfo( MMatchCharInfo* pCharInfo )
{
	_STATUS_DB_START;

	CString strSQL;

	try
	{
		if (!CheckOpen()) return false;

		if( 0 == pCharInfo )
			return false;

		CODBCRecordset rs( &m_DB );		
		strSQL.Format( g_szDB_SELECT_QUEST_ITEM_INFO_BY_CID, pCharInfo->m_nCID );

		CDBBinary bn = rs.SelectBinary( strSQL );
		// ������ �����ϸ� Index�� -1�� �����ؼ� ��ȯ����.
		if( -1 == bn.GetCurIndex() )
		{
			mlog( "MMatchDBMgr::GetCharQuestItemInfo - ����Ʈ ������ SELECT���� ����. UserName:%s CID:%d\n", 
				pCharInfo->m_szName, 
				pCharInfo->m_nCID );
			return false;
		}

		unsigned char	szData[ QUEST_DATA ];
		MCRC32::crc_t	Crc32;

		// �ݵ�� Begin()�� ȣ���ؾ� ��.
		bn.Begin();
		int nDataSize = bn.GetNextData(szData, QUEST_DATA);
		if( -1 == nDataSize )
		{
			Log( "MMatchDMBgr::GetQuestItem - �����͸� �����ϴµ� ����." );
			return false;
		}

		// copy crc check sum.
		memcpy( &Crc32, szData, MCRC32::CRC::SIZE );

		// �ɸ��͸� ó�� ����� ����Ʈ ������ �ʵ�� ���� ���� �ʵ尡 ��� 0�̱⿡ CRC32�˻縦 �Ҽ��� ����.
		//  �׷��Ƿ� ó�� 4Byte�� ��� 0�̸� �ʵ忡 �����Ͱ� ���°����� �Ǵ���.
		if( 0 == nDataSize )
		{
			pCharInfo->m_QuestItemList.Clear();
			memset( &pCharInfo->m_QMonsterBible, 0, sizeof(MQuestMonsterBible) );
			pCharInfo->m_QuestItemList.SetDBAccess( true );

			_STATUS_DB_END(52);
			return true;
		}

		if( Crc32 != MCRC32::BuildCRC32(reinterpret_cast<BYTE*>(szData + MCRC32::CRC::SIZE), nDataSize - MCRC32::CRC::SIZE) )
		{
			Log( "MMatchDBMgr::GetQuestItem - crc check error." );
			return false;
		}

		// ������ ������������ ���� ������ Ŭ����.
		pCharInfo->m_QuestItemList.Clear();

		// ������ �ݵ�� ���Ѿ� ��. ����Ʈ ������ ������ ���� ����.
		// ����Ʈ������ �ε�.
		for( int i = 0; i < MAX_DB_QUEST_ITEM_SIZE; ++i )
		{
			if( MIN_QUEST_DB_ITEM_COUNT > static_cast<int>(szData[MCRC32::CRC::SIZE + i]) )
				continue; // �ѹ��� ȹ���� ���� ���� ������.

			// �ѹ��̶� ȹ�������� �ִ� ������. ������ ��� ����. 
			unsigned long int nItemID = i + MIN_QUEST_ITEM_ID;
			int nQuestItemCount = static_cast<int>(szData[MCRC32::CRC::SIZE + i]);
			bool bKnownItem;

			if (nQuestItemCount > 0)
			{
				bKnownItem = true;
				nQuestItemCount--;		// DB�� ����� �� 1�� ��� 0�̴�.
			}
			else
			{
				bKnownItem = false;
			}

			if( !pCharInfo->m_QuestItemList.CreateQuestItem(nItemID, nQuestItemCount, bKnownItem) )
				mlog( "MMatchDBMgr::GetCharQuestItemInfo - ���ο� ����Ʈ ������ ���� ����.\n" ); // error...
		}

		// ���� ���� ����.
		memcpy( &pCharInfo->m_QMonsterBible, szData + MCRC32::SIZE + MAX_DB_QUEST_ITEM_SIZE, MAX_DB_MONSTERBIBLE_SIZE );
#ifdef _DEBUG
		{
			mlog( "\nStart %s's debug MonsterBible info.\n", pCharInfo->m_szName );
			for( int i = 0; i < (220000 - 210001); ++i )
			{
				if( pCharInfo->m_QMonsterBible.IsKnownMonster(i) )
				{
					MQuestItemDesc* pMonDesc = GetQuestItemDescMgr().FindMonserBibleDesc( i );
					if( 0 == pMonDesc )
					{
						mlog( "MMatchDBMgr::GetCharQuestItemInfo - %d Fail to find monster bible description.\n", i );
						continue;
					}
					else
					{
						mlog( "Get DB MonBibleID:%d MonName:%s\n", i, pMonDesc->m_szQuestItemName );
					}
				}
			}
			mlog( "End %s's debug MonsterBible info.\n\n", pCharInfo->m_szName );
		}
#endif

		pCharInfo->m_QuestItemList.SetDBAccess( true );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(52);

	return true;
}


bool MMatchDBMgr::InsertQuestGameLog( const char* pszStageName, 
									 const int nScenarioID,
									 const int nMasterCID, const int nPlayer1, const int nPlayer2, const int nPlayer3,
									 const int nTotalRewardQItemCount,
									 const int nElapsedPlayTime,
									 int& outQGLID )
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try
	{
		string strStageName = m_DBFilter.Filtering( string(pszStageName) );

		strSQL.Format( g_szDB_INSERT_QUEST_GAME_LOG, 
			&strStageName[0], nMasterCID, nPlayer1, nPlayer2, nPlayer3, nTotalRewardQItemCount, nScenarioID, nElapsedPlayTime );
		rs.Open( strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		return false;
	}

	outQGLID = rs.Field("ORDERQGLID").AsLong();

	_STATUS_DB_END(53);
	return true;
}


bool MMatchDBMgr::InsertQUniqueGameLog( const int nQGLID, const int nCID, const int nQIID )
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	if( (0 == nCID) || (0 == nQIID) )
		return false;

	CString strSQL;
	try
	{
		strSQL.Format( g_szDB_INSERT_QUINQUEITEMLOG, nQGLID, nCID, nQIID );
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(54);
	return true;
}

// �ְ� 54

bool MMatchDBMgr::GetCID( const char* pszCharName, int& outCID )
{
	if( 0 == pszCharName ) 
		return false;

	CString strSQL;
	strSQL.Format( g_szGetCIDbyCharName, pszCharName );

	CODBCRecordset rs(&m_DB);
	try
	{
		rs.Open( strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	outCID = rs.Field("CID").AsLong();

	return true;
}


bool MMatchDBMgr::GetCharName( const int nCID, string& outCharName )
{
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_szGetCharNameByCID, nCID );

	CODBCRecordset rs(&m_DB);
	try
	{
		rs.Open( strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	outCharName = rs.Field("Name").AsString().GetBuffer();

	return true;
}

bool MMatchDBMgr::CheckPremiumIP(const char* szIP, bool& outbResult)
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_CHECK_PREMIUM_IP, szIP);

	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE)
	{
		return false;
	}

	const bool IsGameRoom	= rs.Field( "IsGameRoom" ).AsBool();
	const int  nEtc			= rs.Field( "etc" ).AsInt();

	if( IsGameRoom )
	{
		outbResult = IsGameRoom;

		// �߰����� �۾��� ���⿡.
	}
	else 
	{
		outbResult = false;
	}

	/* 2006-08-31���� ������� ����. Ȥ�� �𸣴� ���ø� ���� �ٶ�.
	else if ((rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 	// ������� 0���̸� false
	{
	outbResult = false;
	}
	else
	{
	outbResult = true;
	}
	*/


	_STATUS_DB_END(57);
	return true;
}

bool MMatchDBMgr::GetIPContryCode( const string& strIP, 
								  DWORD& dwOutIPFrom, 
								  DWORD& dwOutIPTo, 
								  string& strOutCountryCode )
{
	_STATUS_DB_START;
	try 
	{
		m_CountryFilterDBMgr.GetIPContryCode( strIP, dwOutIPFrom, dwOutIPTo, strOutCountryCode );
	}
	catch( CDBException* e )
	{
		Log( "MMatchDBMgr::GetIPContryCode - %s\n", e->m_strError );
		return false;
	}
	_STATUS_DB_END(58);
	return true;
}


bool MMatchDBMgr::GetBlockCountryCodeList( BlockCountryCodeList& rfBlockCountryCodeList )
{
	_STATUS_DB_START;
	try
	{
		m_CountryFilterDBMgr.GetBlockCountryCodeList( rfBlockCountryCodeList );
	}
	catch( CDBException* e )
	{
		Log( "MMatchDBMgr::GetBlockCountryCodeList - %s\n", e->m_strError );
		return false;
	}
	_STATUS_DB_END(60);
	return true;
}


bool MMatchDBMgr::GetCustomIP( const string& strIP, DWORD& dwIPFrom, DWORD& dwIPTo, bool& bIsBlock, string& strCountryCode3, string& strComment )
{
	_STATUS_DB_START;
	try
	{
		m_CountryFilterDBMgr.GetCustomIP( strIP, dwIPFrom, dwIPTo, bIsBlock, strCountryCode3, strComment );
	}
	catch( CDBException* e )
	{
		Log( "MMatchDBMgr::GetCustomIP - %s\n", e->m_strError );
		return false;
	}
	_STATUS_DB_END(59);
	return true;

}


bool MMatchDBMgr::GetIPtoCountryList( IPtoCountryList& rfIPtoCountryList )
{
	_STATUS_DB_START;
	try
	{
		m_CountryFilterDBMgr.GetIPtoCountryList( rfIPtoCountryList );
	}
	catch( CDBException* e )
	{
		Log( "MMatchDBMgr::GetIPtoCountryList - %s\n", e->m_strError );
		return false;
	}
	_STATUS_DB_END(61);
	return true;
}


bool MMatchDBMgr::GetCustomIPList( CustomIPList& rfCustomIPList )
{
	_STATUS_DB_START;
	try
	{
		m_CountryFilterDBMgr.GetCustomIPList( rfCustomIPList );
	}
	catch( CDBException* e )
	{
		Log( "MMatchDBMgr::GetCustomIPList - %s\n", e->m_strError );
		return false;
	}
	_STATUS_DB_END(62);
	return true;
}

bool MMatchDBMgr::InsertEvent( const DWORD dwAID,  const DWORD dwCID, const string& strEventName )
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_szInsertEvent, dwAID, dwCID, strEventName.c_str() );
	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(63);

	return true;
}


bool MMatchDBMgr::SetBlockHacking( const DWORD dwAID
								  , const DWORD dwCID
								  , const BYTE btBlockType
								  , const string& strIP
								  , const string& strEndHackBlockerDate 
								  , const BYTE nServerID
								  ,	const string& strChannelName
								  , const string& strComment
								  , const BYTE nBlockLevel )
{
#ifdef _BLOCK_HACKER
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	vector<BYTE> vIP;
	vIP.reserve( 4 );
	if( !SplitStrIP(strIP, vIP) || (4 != vIP.size()) )
	{
		mlog( "MMatchDBMgr::SetBlockHacking - Invalid IP(AID:%u, IP:%s, Date:%s).\n"
			,dwAID, strIP.c_str(), MGetStrLocalTime().c_str() );

		// IP�� �������̾ �α״� ���ܾ� �ϱ⿡ 0���� ü����. - by SungE 2007-05-04
		vIP.clear();
		vIP.push_back( 0 );
		vIP.push_back( 0 );
		vIP.push_back( 0 );
		vIP.push_back( 0 );
	}

	CString strSQL;
	strSQL.Format( g_szSetHackingBlock
		, dwAID
		, dwCID
		, btBlockType
		, vIP[0], vIP[1], vIP[2], vIP[3]
		, strEndHackBlockerDate.c_str()
			, nServerID
			, strChannelName.c_str()
			, strComment.c_str()
			, nBlockLevel );
		try
		{
			m_DB.ExecuteSQL( strSQL );
		}
		catch( CDBException* e )
		{

			ExceptionHandler(strSQL, e);
			return false;
		}

		_STATUS_DB_END(64);
#endif
		return true;
}


bool MMatchDBMgr::ResetAccountHackingBlock( const DWORD dwAID, const BYTE btBlockType )
{
#ifdef _DEBUG
	// �� ������ �系���������� �����۵����� �ʾƼ� ���� �޸𸮸��� ����Ŵ
	// �ٸ� �޸𸮸��� ����ĥ ����� �־ ����׶��� �� ������ �����ϵ��� ����
	return true;
#endif

#ifdef _BLOCK_HACKER
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_szResetAccountHackingBlock, dwAID, btBlockType );
	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(65);
#endif
	return true;
}



//bool MMatchDBMgr::InsertBlockLog( const DWORD dwAID, const DWORD dwCID, const BYTE btBlockType, const string& strComment,
// 								  const string& strIP )
//{
//#ifdef _BLOCK_HACKER
//_STATUS_DB_START;
//	if( !CheckOpen() )
//		return false;
//
//	CString strSQL;
//	strSQL.Format( g_szInsertBlockLog, dwAID, dwCID, btBlockType, strComment.c_str(), strIP.c_str() );
//	try
//	{
//		m_DB.ExecuteSQL( strSQL );
//	}
//	catch( CDBException* e )
//	{
//		Log( "MMatchDBMgr::InsertBlockLog - %s.\n", e->m_strError );
//		return false;
//	}
//_STATUS_DB_END(66);
//#endif
//	return true;
//}


bool MMatchDBMgr::DeleteExpiredClan( const DWORD dwCID, const DWORD dwCLID, const string& strDeleteName, const DWORD dwWaitHour )
{
#ifdef _DELETE_CLAN
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_szDeleteExpiredClan, dwCLID, dwCID, strDeleteName.c_str(), dwWaitHour );
	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(67);
#endif
	return true;
}


bool MMatchDBMgr::SetDeleteTime( const DWORD dwMasterCID, const DWORD dwCLID, const string& strDeleteDate )
{
#ifdef _DELETE_CLAN
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	if( strDeleteDate.empty() )
		return false;

	CString strSQL;
	strSQL.Format( g_szSetClanDeleteDate, dwMasterCID, dwCLID, strDeleteDate.c_str() );
	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(68);
#endif

	return true;
}


bool MMatchDBMgr::AdminResetAllHackingBlock()
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_szAdminResetAllHackingBlock );

	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(69);

	return true;
}

bool MMatchDBMgr::UpdateAccountLastLoginTime( const DWORD dwAID )
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_szUpdateAccountLastLogoutTime, dwAID );

	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(70);

	return true;
}

bool MMatchDBMgr::GetDBConnection()	
{
	_STATUS_DB_START;
	if (!CheckOpen())
		return false;

	CString strSQL;
	strSQL.Format(g_szGetDBConnection);		
	CODBCRecordset rs(&m_DB);

	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) 
	{
		return false;
	}
	int rsResult = rs.Field("serverid").AsInt();
	if (rsResult != 1)
	{
		return false;
	}
	_STATUS_DB_END(71);
	return true;
}


bool MMatchDBMgr::GetGambleItemList( vector<MMatchGambleItem*>& vGambleItemList )
{
	_STATUS_DB_START;
	CString strSQL;
	strSQL.Format( g_spGetGambleItemList );
	CODBCRecordset rs(&m_DB);

	try
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) )  
	{
		return false;
	}

	DWORD	dwGIID			= 0;
	string	strName;
	string	strDesc;	
	DWORD	dwPrice			= 0;
	int		nStartTimeMin	= 0;
	int		nLifeTimeMin	= 0;
	bool	bIsOpened		= false;
	bool	bIsCash			= false;

	const int dwCurTimeMin = (MGetMatchServer()->GetGlobalClockCount()) / 60000;

	for( ;! rs.IsEOF(); rs.MoveNext() )
	{
		dwGIID			= (DWORD)rs.Field( "GIID" ).AsInt();
		strName			= rs.Field( "Name" ).AsString().GetBuffer();
		strDesc			= rs.Field( "Description" ).AsString().GetBuffer();
		dwPrice			= (DWORD)rs.Field( "Price" ).AsInt();
		nStartTimeMin	= (dwCurTimeMin + rs.Field( "StartDiffMin" ).AsInt());
		nLifeTimeMin	= rs.Field( "LifeTimeMin" ).AsInt();
		bIsOpened		= (1 == (DWORD)rs.Field( "Opened" ).AsInt());
		bIsCash			= (1 == rs.Field("IsCash").AsInt());

		MMatchGambleItem* pGItem = new MMatchGambleItem( dwGIID
			, strName
			, strDesc
			, nStartTimeMin
			, nLifeTimeMin
			, dwPrice
			, bIsCash
			, bIsOpened );
		if( NULL == pGItem )
			return false;

		vGambleItemList.push_back( pGItem );
	}
	_STATUS_DB_END(72);

	return true;
}


bool MMatchDBMgr::GetGambleRewardItemList( vector<MMatchGambleRewardItem*>& vGambleRewardItemList )
{
	_STATUS_DB_START;
	CString strSQL;
	strSQL.Format( g_spGetGambleRewardItem );
	CODBCRecordset rs(&m_DB);

	try
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) )  
	{
		return false;
	}

	DWORD dwGRIID			= 0;
	DWORD dwGIID			= 0;
	DWORD dwItemIDMale		= 0;
	DWORD dwItemIDFemale	= 0;
	DWORD dwItemCnt			= 0;
	WORD  wRentHourPeriod	= 0;
	WORD  wRatePerThousand	= 0;

	for( int nIndex = 0; ! rs.IsEOF(); rs.MoveNext() )
	{
		dwGRIID				= (DWORD)rs.Field("GRIID").AsInt();
		dwGIID				= (DWORD)rs.Field("GIID" ).AsInt();
		dwItemIDMale		= (DWORD)rs.Field("ItemIDMale").AsInt();
		dwItemIDFemale		= (DWORD)rs.Field("ItemIDFemale").AsInt();
		dwItemCnt			= (DWORD)rs.Field("ItemCnt").AsInt();
		wRentHourPeriod		= (WORD)rs.Field( "RentHourPeriod" ).AsInt();
		wRatePerThousand	= (WORD)rs.Field( "RatePerThousand" ).AsInt();

		MMatchGambleRewardItem* pGRItem = new MMatchGambleRewardItem( dwGRIID, dwGIID
			, dwItemIDMale, dwItemIDFemale, dwItemCnt, wRentHourPeriod, wRatePerThousand );

		if( NULL == pGRItem ) return false;

		vGambleRewardItemList.push_back( pGRItem );
	}
	_STATUS_DB_END(73);

	return true;
}

bool MMatchDBMgr::ChangeGambleItemToRewardNormalItem( const DWORD dwCID, const DWORD dwCIID, const DWORD dwGIID, const DWORD dwGRIID, 
													 const DWORD dwItemID, const DWORD dwRentHourPeriod, int &pOutCIID )
{
	CString strSQL;	
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format( g_spChangeGambleItemToRewardNormalItem, dwCID, dwCIID, dwGIID, dwGRIID, dwItemID, dwRentHourPeriod);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE) ) { return false; }

	pOutCIID = rs.Field("ORDERCIID").AsInt();;

	return true;
}

bool MMatchDBMgr::ChangeGambleItemToRewardSpendableItem( const DWORD dwCID, const DWORD dwCIID, const DWORD dwGIID, const DWORD dwGRIID, 
														const DWORD dwItemID, const DWORD dwItemCnt, int &pOutCIID )
{
	CString strSQL;	
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format( g_spChangeGambleItemToRewardSpendableItem, dwCID, dwCIID, dwGIID, dwGRIID, dwItemID, dwItemCnt);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE) ) { return false; }

	pOutCIID = rs.Field("ORDERCIID").AsInt();;

	return true;
}

bool MMatchDBMgr::UpdateAccountPowerLevelingInfo( const DWORD dwAID, const DWORD IsHacker )
{
	if( !CheckOpen() )
		return false;

	CString strSQL;
	strSQL.Format( g_spUpdateAccountPowerLevelingInfo, dwAID, IsHacker );

	try
	{
		m_DB.ExecuteSQL( strSQL );
	}
	catch( CDBException* e )
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	return true;
}
// �ְ� 75



//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu
//TCHAR g_spAddSurvivalModeGameLog[] = _T( "{CALL spSurvivalModeGameLogAdd('%s', %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)}" );
bool MMatchDBMgr::InsertSurvivalModeGameLog(char *szGameName, DWORD dwScenarioID, DWORD dwTotalRound, 
											DWORD dwMasterPlayerCID, DWORD dwMasterPlayerRankPoint,
											DWORD dw2PCID, DWORD dw2PRankPoint, 
											DWORD dw3PCID, DWORD dw3PRankPoint, 
											DWORD dw4PCID, DWORD dw4PRankPoint, 
											DWORD dwGamePlayTime)
{
	_STATUS_DB_START;
	if( !CheckOpen() )
		return false;

	CString strSQL;

	try
	{
		string strStageName = m_DBFilter.Filtering(string(szGameName));
		strSQL.Format( g_spInsertSurvivalModeGameLog, 
			&strStageName[0], dwScenarioID, dwTotalRound, 
			dwMasterPlayerCID, dwMasterPlayerRankPoint, 
			dw2PCID, dw2PRankPoint, 
			dw3PCID, dw3PRankPoint, 
			dw4PCID, dw4PRankPoint, 
			dwGamePlayTime );

		m_DB.ExecuteSQL(strSQL);
	}
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(51);	

	return true;
}



bool MMatchDBMgr::GetSurvivalModeGroupRanking(vector<RANKINGINFO*> pRankingVec[])
{
	_STATUS_DB_START;

	CString strSQL;
	strSQL.Format( g_spGetSurvivalGroupRanking );
	CODBCRecordset rs(&m_DB);

	try
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) )  
	{
		return false;
	}

	DWORD dwScenarioID	= 0;
	DWORD dwRanking		= 0;

	int Temp = rs.GetRowsetSize();
	for( int nIndex = 0; ! rs.IsEOF(); rs.MoveNext() )
	{
		dwScenarioID	= (DWORD)rs.Field( "SID" ).AsInt();
		dwRanking		= (DWORD)rs.Field( "Ranking" ).AsInt();

		//mlog("SID=%d Rank=%d User Name = %s\n", dwScenarioID, dwRanking, rs.Field("Name").AsString());

		// Notice By Hong KiJu - ȫ���ֿ��� �����ϼ���
		// �� ���� �� ����... �̷� ������ �ڵ��س�����..-_��
		// ���� �����丵 ����?!
		RANKINGINFO *pRankingInfo = new RANKINGINFO;
		pRankingInfo->dwCID			= (DWORD)rs.Field( "CID" ).AsInt();		
		pRankingInfo->dwRanking		= dwRanking;
		pRankingInfo->dwRankingPoint= (DWORD)rs.Field( "RP" ).AsInt();
		strcpy(pRankingInfo->szCharName, rs.Field("Name").AsString());

		if( dwScenarioID <= MAX_SURVIVAL_SCENARIO_COUNT)
		{
			pRankingVec[dwScenarioID - 1].push_back(pRankingInfo);
		}
		else
		{
			mlog("MMatchDBMgr::GetSurvivalModeGroupRanking - ScenarioID Error(%d)\n", dwScenarioID);
		}

	}

	_STATUS_DB_END(51);	

	return true;
}

bool MMatchDBMgr::GetSurvivalModePrivateRanking(DWORD dwCID, RANKINGINFO pRankingInfo[])
{
	_STATUS_DB_START;

	CString strSQL;
	strSQL.Format( g_spGetSurvivalPrivateRanking, dwCID );
	CODBCRecordset rs(&m_DB);

	try
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e)
	{

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) )  
	{
		return false;
	}

	DWORD dwScenarioID	= 0;

	for( int nIndex = 0; ! rs.IsEOF(); rs.MoveNext() )
	{
		// Notice By Hong KiJu - ȫ���ֿ��� �����ϼ���
		// �� ���� �� ����... �̷� ������ �ڵ��س�����..-_��
		// ���� �����丵 ����?!
		dwScenarioID	= (DWORD)rs.Field( "SID" ).AsInt();		

		if( dwScenarioID <= MAX_SURVIVAL_SCENARIO_COUNT)
		{
			pRankingInfo[dwScenarioID - 1].dwRanking = (DWORD)rs.Field( "Ranking" ).AsInt();
			pRankingInfo[dwScenarioID - 1].dwRankingPoint = (DWORD)rs.Field( "RP" ).AsInt();
		}
		else
		{
			mlog("MMatchDBMgr::GetSurvivalModeGroupRanking - ScenarioID Error(%d)\n", dwScenarioID);
		}
	}


	_STATUS_DB_END(51);	

	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////
// Added By Hong KiJu(2009-09-25)
bool MMatchDBMgr::GetDuelTournamentTimeStamp(char *szTimeStamp)
{
	_STATUS_DB_START;
	CString strSQL;
	strSQL.Format( g_spGetDTTimeStamp );
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) ) {return false;}

	memcpy(szTimeStamp, rs.Field("TimeStamp").AsString(), DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);

	_STATUS_DB_END(51);	
	return true;
}

bool MMatchDBMgr::GetDuelTournamentSideRankingInfo(DWORD dwCID, list<DTRankingInfo*> *pRankingList)
{
	_STATUS_DB_START;

	CString strSQL;
	strSQL.Format( g_spGetDTSideRankingInfo, dwCID );
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) ) {
		return false;
	}

	for( int nIndex = 0; ! rs.IsEOF(); rs.MoveNext() )
	{
		DTRankingInfo *pInfo = new DTRankingInfo;

		strcpy(pInfo->m_szCharName, rs.Field("Name").AsString());

		pInfo->m_nTP = rs.Field("TP").AsInt();	
		pInfo->m_nWins = rs.Field("Wins").AsInt();
		pInfo->m_nLoses = rs.Field("Loses").AsInt();
		pInfo->m_nRanking = rs.Field("Rank").AsInt();	
		pInfo->m_nRankingIncrease = rs.Field("RankingIncrease").AsInt();	
		pInfo->m_nFinalWins = rs.Field("FinalWins").AsInt();
		pInfo->m_nGrade = rs.Field("PreGrade").AsInt();

		pRankingList->push_back(pInfo);
	}

	_STATUS_DB_END(51);	
	return true;
}

bool MMatchDBMgr::GetDuelTournamentGroupRankingInfo(list<DTRankingInfo*> *pRankingList)
{
	_STATUS_DB_START;

	CString strSQL;
	strSQL.Format( g_spGetDTGroupRankingInfo );
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) ) {
		return false;
	}

	for( int nIndex = 0; ! rs.IsEOF(); rs.MoveNext() )
	{
		DTRankingInfo *pInfo = new DTRankingInfo;

		strcpy(pInfo->m_szCharName, rs.Field("Name").AsString());

		pInfo->m_nTP = rs.Field("TP").AsInt();	
		pInfo->m_nWins = rs.Field("Wins").AsInt();
		pInfo->m_nLoses = rs.Field("Loses").AsInt();
		pInfo->m_nRanking = rs.Field("Rank").AsInt();	
		pInfo->m_nRankingIncrease = rs.Field("RankingIncrease").AsInt();	
		pInfo->m_nFinalWins = rs.Field("FinalWins").AsInt();
		pInfo->m_nGrade = pInfo->m_nGrade = rs.Field("PreGrade").AsInt();

		pRankingList->push_back(pInfo);
	}
	_STATUS_DB_END(51);	
	return true;
}


bool MMatchDBMgr::InsertDuelTournamentCharInfo(DWORD dwCID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format( g_spInsertDTCharacterInfo, dwCID );
		m_DB.ExecuteSQL( strSQL );
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}
	_STATUS_DB_END(51);	

	return true;
}

bool MMatchDBMgr::GetDuelTournamentCharInfo(DWORD dwCID, MMatchObjectDuelTournamentCharInfo *pDTCharInfo)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format( g_spGetDTCharacterInfo, dwCID );
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	int nTP					= rs.Field("TP").AsInt();
	int nWins				= rs.Field("Wins").AsInt();
	int nLoses				= rs.Field("Loses").AsInt();
	int nRanking			= rs.Field("Ranking").AsInt();
	int nRankingIncrease	= rs.Field("RankingIncrease").AsInt();
	int nFinalCnt			= rs.Field("FinalGames").AsInt();
	int nFinalWins			= rs.Field("FinalWins").AsInt();
	int nSFinalCnt			= rs.Field("SFinalGames").AsInt();
	int nSFinalWins			= rs.Field("SFinalWins").AsInt();
	int nQFinalCnt			= rs.Field("QFinalGames").AsInt();
	int nQFinalWins			= rs.Field("QFinalWins").AsInt();
	int nLeaveCount			= rs.Field("LeaveCount").AsInt();
	int nPreGrade			= rs.Field("PreGrade").AsInt();

	char szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1] = {0, };
	memcpy(szTimeStamp, rs.Field("TimeStamp").AsString(), DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);

	pDTCharInfo->SetCharInfo(nTP, nWins, nLoses, nRanking, nRankingIncrease
		, nFinalCnt, nFinalWins, nSFinalCnt, nSFinalWins, nQFinalCnt, nQFinalWins, nLeaveCount, nPreGrade, szTimeStamp);

	_STATUS_DB_END(51);	

	return true;
}

bool MMatchDBMgr::UpdateDuelTournamentCharacterInfo(DWORD dwCID, char *szTimeStamp, MMatchObjectDuelTournamentCharInfo *pDTCharInfo)
{
	if(pDTCharInfo == NULL) return false;

	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format( g_spUpdateDTCharacterInfo
		, dwCID
		, szTimeStamp
		, pDTCharInfo->GetTP()
		, pDTCharInfo->GetWins()
		, pDTCharInfo->GetLoses()
		, pDTCharInfo->GetFinalCount()
		, pDTCharInfo->GetFinalWins()
		, pDTCharInfo->GetSemiFinalCount()
		, pDTCharInfo->GetSemiFinalWins()
		, pDTCharInfo->GetQuaterFinalCount()
		, pDTCharInfo->GetQuaterFinalWins()
		, pDTCharInfo->GetLeaveCount());
	CODBCRecordset rs(&m_DB);

	try {
		m_DB.ExecuteSQL(strSQL);
		if( pDTCharInfo->GetTP() < 0 ) {
			Log("MMatchDBMgr::UpdateDuelTournamentCharacterInfo - Wrong Values\n");
		}
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(51);	

	return true;
}

bool MMatchDBMgr::GetDuelTournamentPreviousCharacterInfo(DWORD dwCID, int *nPrevTP, int *nPrevWins, int *nPrevLoses, int *nPrevRanking, int *nPrevFinalWin)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format( g_spGetDTCharacterInfoPrevious, dwCID );
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	*nPrevTP = rs.Field("TP").AsInt();
	*nPrevWins = rs.Field("Wins").AsInt();
	*nPrevLoses = rs.Field("Loses").AsInt();
	*nPrevRanking = rs.Field("Ranking").AsInt();
	*nPrevFinalWin = rs.Field("FinalWins").AsInt();

	_STATUS_DB_END(51);	

	return true;
}

bool MMatchDBMgr::InsertDuelTournamentGameLog(MDUELTOURNAMENTTYPE nDTType, int nMatchFactor, int nPlayer1CID, int nPlayer2CID, int nPlayer3CID, int nPlayer4CID, 
											  int nPlayer5CID, int nPlayer6CID, int nPlayer7CID, int nPlayer8CID, int *nOutNumber, char *szOutTimeStamp)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format( g_spInsertDTGameLog, (int)nDTType, nMatchFactor, nPlayer1CID, nPlayer2CID, nPlayer3CID, nPlayer4CID, nPlayer5CID, nPlayer6CID, nPlayer7CID, nPlayer8CID );
	CODBCRecordset rs(&m_DB);

	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) ) return false;

	*nOutNumber = rs.Field("LogID").AsInt();
	strcpy(szOutTimeStamp, rs.Field("TimeStamp").AsString());


	_STATUS_DB_END(51);	
	return true;
}

bool MMatchDBMgr::UpdateDuelTournamentGameLog(char* szTimeStamp, int nLogID, int nChampCID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format( g_spUpdateDTGameLog, szTimeStamp, nLogID, nChampCID );
	CODBCRecordset rs(&m_DB);

	try {
		m_DB.ExecuteSQL(strSQL);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(51);	
	return true;
}

bool MMatchDBMgr::InsertDuelTournamentGameLogDetail(int nLogID, char* szTimeStamp, int nMatchType, int nPlayTime
													, int nWinnerCID, int nGainTP, int nLoserCID, int nLoseTP)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	strSQL.Format( g_spInsertDTGameLogDetail, nLogID, szTimeStamp, nMatchType, nPlayTime, nWinnerCID, nGainTP, nLoserCID, nLoseTP);
	CODBCRecordset rs(&m_DB);

	try {
		m_DB.ExecuteSQL(strSQL);
	}
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if( (rs.IsOpen() == FALSE) ) return false;

	_STATUS_DB_END(51);	
	return true;
}

bool MMatchDBMgr::GetAccountPenaltyInfo( const unsigned long int nAID, MMatchAccountPenaltyInfo* poutAccountPenaltyInfo)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CODBCRecordset rs(&m_DB);

	CString strSQL;

	try {		
		strSQL.Format(g_spGetAccountPenaltyInfo, nAID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) {
		return false;
	}

	SYSTEMTIME sysTime;
	for( int nIndex = 0; ! rs.IsEOF(); rs.MoveNext() )
	{
		int nPCode = rs.Field("PCode").AsInt();		
		rs.Field("PEndDate").AsDate().GetAsSystemTime(sysTime);

		poutAccountPenaltyInfo->SetPenaltyInfo((MPenaltyCode)nPCode, sysTime);
	}
	_STATUS_DB_END(5);
	return true;
}

bool MMatchDBMgr::InsertAccountPenaltyInfo( int nAID, int nPCode, int nPenaltyHour, char* szGMID )
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(g_spInsertAccountPenaltyInfo, nAID, nPCode, nPenaltyHour, szGMID);
		m_DB.ExecuteSQL(strSQL);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(5);
	return true;
}

bool MMatchDBMgr::InsertDistributeItem(const unsigned int nCID, 
									   int nItemDescID, 
									   bool bRentItem, 
									   int nRentPeriodHour,
									   unsigned long int* poutCIID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	// ������ �������̸� nRentPeriodHour=0�̴�.
	if (bRentItem == false) { nRentPeriodHour = 0; }

	CString strSQL;
	strSQL.Format(g_szDB_INSERT_DISTRIBUTE_ITEM, nCID, nItemDescID, nRentPeriodHour);
	CODBCRecordset rs(&m_DB);

	bool bException = false;
	try {
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} catch(CDBException* e) {
		bException = true;

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	_STATUS_DB_END(8);


	int nRet = rs.Field("Ret").AsInt();
	if( nRet < 0 ) {
		Log("MMatchDBMgr::InsertDistributeItem - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::InsertDistributeItem - %s\n", strSQL.GetBuffer() );
		*poutCIID = 0;
		return false;
	}

	*poutCIID = rs.Field("ORDERCIID").AsLong();	
	return true;
}

bool MMatchDBMgr::GetItemTable(map<int, MMatchItemDescForDatabase*>& ItemMapFromDatabase)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;	

	CString strSQL;
	strSQL.Format( g_szDB_GET_ALLITEM );
	CODBCRecordset rs(&m_DB);

	try { 
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 

	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) {
		return false;
	}

	for( ; ! rs.IsEOF(); rs.MoveNext() ) 
	{
		MMatchItemDescForDatabase *pItemDesc = new MMatchItemDescForDatabase;

		pItemDesc->m_nID = rs.Field("ItemID").AsInt();

		pItemDesc->m_nResSex   = rs.Field("ResSex").AsInt();
		pItemDesc->m_nResLevel = rs.Field("ResLevel").AsInt();

		pItemDesc->m_nSlot   = (MMatchItemSlotType)rs.Field("Slot").AsInt();
		pItemDesc->m_nWeight = rs.Field("Weight").AsInt();
		pItemDesc->m_nDamage = rs.Field("Damage").AsInt();
		pItemDesc->m_nDelay  = rs.Field("Delay").AsInt();

		pItemDesc->m_nControllability = rs.Field("Controllability").AsInt();
		pItemDesc->m_nMagazine		  = rs.Field("Magazine").AsInt();
		pItemDesc->m_nMaxBullet		  = rs.Field("MaxBullet").AsInt();
		pItemDesc->m_nReloadTime	  = rs.Field("ReloadTime").AsInt();

		pItemDesc->m_nHP	= rs.Field("HP").AsInt();
		pItemDesc->m_nAP	= rs.Field("AP").AsInt();

		if( rs.Field("IsCashItem").AsInt() == 0 )		pItemDesc->m_bIsCashItem = false;
		else if( rs.Field("IsCashItem").AsInt() == 1 )	pItemDesc->m_bIsCashItem = true;

		if( rs.Field("IsSpendableItem").AsInt() == 0 )		pItemDesc->m_bIsSpendableItem = false;
		else if( rs.Field("IsSpendableItem").AsInt() == 1 )	pItemDesc->m_bIsSpendableItem = true;

		ItemMapFromDatabase.insert(pair<int, MMatchItemDescForDatabase*>(pItemDesc->m_nID, pItemDesc));
	}


	_STATUS_DB_END(1);
	return true;
}

bool MMatchDBMgr::DeleteExpiredCharItem(const unsigned int nCID, unsigned int nCIID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_DELETE_EXPIRED_CHARACTER_ITEM, nCID, nCIID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(49);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::DeleteExpiredCharItem - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::DeleteExpiredCharItem - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}

bool MMatchDBMgr::UpdateCharGambleItemCount(unsigned int nCIID, unsigned int nGIID, unsigned int nGambleItemCount)
{
	_STATUS_DB_START;
	if( !CheckOpen() )	return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_GAMBLE_ITEM_DECREASE, nCIID, nGIID, nGambleItemCount);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 

	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;

	_STATUS_DB_END(1);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::UpdateCharGambleItemCount - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::UpdateCharGambleItemCount - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}

bool MMatchDBMgr::UpdateCharSpendItemCount(unsigned int nCIID, unsigned int nSpendCount)
{
	_STATUS_DB_START;
	if( !CheckOpen() )	return false;	

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_UPDATE_CHAR_SPEND_ITEM_COUNT, nCIID, nSpendCount);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;

	_STATUS_DB_END(1);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::UpdateCharSpendItemCounts - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::UpdateCharSpendItemCounts - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}

bool MMatchDBMgr::SellingItemToBounty(int nCID, int nCIID, int nSellItemID, int nSellPrice, int nCharBP)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_SELL_ITEM_TO_BOUNTY, nCID, nCIID, nSellItemID, nSellPrice, nCharBP);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;
	_STATUS_DB_END(56);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::SellingItemToBounty - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::SellingItemToBounty - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;	
}

bool MMatchDBMgr::SellingSpendableItemToBounty(int nCID, int nCIID, int nSellItemID, int nSellItemCnt, int nSellPrice, int nCharBP)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {		
		strSQL.Format(g_szDB_SELL_SPENDABLEITEM_TO_BOUNTY, nCID, nCIID, nSellItemID, nSellItemCnt, nSellPrice, nCharBP);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;
	_STATUS_DB_END(56);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::SellingSpendableItemToBounty - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::SellingSpendableItemToBounty - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;	
}

bool MMatchDBMgr::UpdateEquipedItem(const unsigned long nCID, MMatchCharItemParts parts, 
									unsigned long int nCIID, const unsigned long int nItemID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_UPDATE_EQUIPITEM, (int)nCID, (int)parts, (int)nCIID, (int)nItemID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	if ((rs.IsOpen() == FALSE) || (rs.GetRecordCount() <= 0) || (rs.IsBOF()==TRUE)) return false;
	_STATUS_DB_END(11);

	int nRet  = rs.Field("Ret").AsInt();
	if (nRet < 0) {
		Log("MMatchDBMgr::UpdateEquipedItem - FAILED(%d)\n", nRet);
		Log("MMatchDBMgr::UpdateEquipedItem - %s\n", strSQL.GetBuffer());
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Created By ȫ����(2011-04-19)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MMatchDBMgr::GetBattletimeRewardList(vector<MMatchBRDescription*>& vBattletimeRewardDescription)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_BATTLE_TIME_REWARD_LIST);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) return false;

	for( ;! rs.IsEOF(); rs.MoveNext() )
	{
		int nBRID = rs.Field("BRID").AsInt();
		int nBRTID = rs.Field("BRTID").AsInt();
		int nRewardMinutePeriod = rs.Field("RewardMinutePeriod").AsInt();
		int nRewardCount = rs.Field("RewardCount").AsInt();
		int nRewardKillCount = rs.Field("RewardKillCount").AsInt();

		string strName = rs.Field("Name").AsString();
		string strResetDesc = rs.Field("ResetDesc").AsString();

		MMatchBRDescription* pDesc = 
			new MMatchBRDescription(nBRID, strName, strResetDesc, nBRTID, nRewardMinutePeriod, nRewardCount, nRewardKillCount);

		vBattletimeRewardDescription.push_back( pDesc );
	}
	_STATUS_DB_END(11);

	return true;
}

bool MMatchDBMgr::GetBattletimeRewardItemList(vector<MMatchBRItem*>& vBattletimeRewardItem)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_BATTLE_TIME_REWARD_ITEM_LIST);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) return false;

	for( ; !rs.IsEOF(); rs.MoveNext() )
	{
		int nBRID = rs.Field("BRID").AsInt();
		int nBRIID = rs.Field("BRIID").AsInt();
		int nItemIDMale = rs.Field("ItemIDMale").AsInt();
		int nItemIDFemale = rs.Field("ItemIDFemale").AsInt();
		int nItemCnt = rs.Field("ItemCnt").AsInt();
		int nRentHourPeriod = rs.Field("RentHourPeriod").AsInt();
		int nRatePerThousand = rs.Field("RatePerThousand").AsInt();

		MMatchBRItem* pRewardItem = 
			new MMatchBRItem(nBRID, nBRIID, nItemIDMale, nItemIDFemale, nItemCnt, nRentHourPeriod, nRatePerThousand);

		vBattletimeRewardItem.push_back( pRewardItem );
	}

	_STATUS_DB_END(11);

	return true;
}

bool MMatchDBMgr::GetCharBRInfoAll(int nCID, MMatchCharBattleTimeRewardInfoMap& BRInfoMap)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_CHAR_BR_INFO_ALL, nCID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) return false;

	for( ; !rs.IsEOF(); rs.MoveNext() )
	{
		int nBRID			= rs.Field("BRID").AsInt();
		int nBRTID			= rs.Field("BRTID").AsInt();
		int nBattleTime		= rs.Field("BattleTime").AsInt();
		int nRewardCount	= rs.Field("RewardCount").AsInt();
		int nKillCount		= rs.Field("KillCount").AsInt();

		MMatchCharBRInfo* pInfo = new MMatchCharBRInfo(nBRID, nBRTID, nBattleTime, nRewardCount, nKillCount);
		pInfo->SetCheckSkip(false);

		BRInfoMap.Insert(pInfo->GetBRID(), pInfo);
	}

	_STATUS_DB_END(11);

	return true;
}

bool MMatchDBMgr::GetCharBRInfo(int nCID, int nBRID, MMatchCharBRInfo* poutInfo)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_CHAR_BR_INFO, nCID, nBRID);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) return false;

	for( ; !rs.IsEOF(); rs.MoveNext() )
	{
		int nBRID			= rs.Field("BRID").AsInt();
		int nBRTID			= rs.Field("BRTID").AsInt();
		int nBattleTime		= rs.Field("BattleTime").AsInt();
		int nRewardCount	= rs.Field("RewardCount").AsInt();
		int nKillCount		= rs.Field("KillCount").AsInt();

		poutInfo->SetBRInfo(nBRID, nBRTID, nBattleTime, nRewardCount, nKillCount);
	}

	_STATUS_DB_END(11);

	return true;
}

bool MMatchDBMgr::InsertCharBRInfo(int nCID, int nBRID, int nBRTID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_INSERT_CHAR_BR_INFO, nCID, nBRID, nBRTID);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(11);

	return true;
}

bool MMatchDBMgr::UpdateCharBRInfo(int nCID, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {
		strSQL.Format(g_szDB_UPDATE_CHAR_BR_INFO, nCID, nBRID, nBRTID, nRewardCount, nBattleTime, nKillCount);
		m_DB.ExecuteSQL( strSQL );
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(11);

	return true;
}

bool MMatchDBMgr::RewardCharBattleTimeReward(int nCID, int nBRID, int nBRTID, int nBattleTime, int nKillCount, int nItemID, int nItemCnt, int nRentHourPeriod, bool bIsSpendable, int* noutCIID)
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;
	CODBCRecordset rs(&m_DB);

	try {
		strSQL.Format(g_szDB_GET_REWARD_CHAR_BR, nCID, nBRID, nBRTID, nBattleTime, nKillCount, nItemID, nItemCnt, nRentHourPeriod, bIsSpendable);
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e) {
		ExceptionHandler(strSQL, e);
		return false;
	}

	if (rs.IsOpen() == FALSE) return false;

	*noutCIID  = rs.Field("Ret").AsInt();
	if (*noutCIID < 0) {
		Log("MMatchDBMgr::RewardCharBR - FAILED(%d)\n", *noutCIID);
		Log("MMatchDBMgr::RewardCharBR - %s\n", strSQL.GetBuffer());
		return false;
	}

	_STATUS_DB_END(11);

	return true;
}

#ifdef _DEBUG
TCHAR sp_q_test[] = _T("{CALL spqtest}");

bool MMatchDBMgr::test()
{
	_STATUS_DB_START;
	if (!CheckOpen()) return false;

	CString strSQL;

	try {		
		strSQL.Format(sp_q_test);
		m_DB.ExecuteSQL(strSQL);
	} 
	catch(CDBException* e) {

		ExceptionHandler(strSQL, e);
		return false;
	}

	_STATUS_DB_END(56);
	return true;
}
#endif