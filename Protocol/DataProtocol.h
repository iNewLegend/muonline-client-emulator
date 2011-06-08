//-----------------------------------------------------------------------------------------------------------
#ifndef DATAPROTCOL_H
#define DATAPROTCOL_H
//-----------------------------------------------------------------------------------------------------------
// Order by rpgsky 2010.7.13

#define CHAR_SET_SIZE 18
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CHARLISTCOUNT
{
	PWMSG_HEAD h;
	short Number;	// 4
	BYTE Count;	// 6
	int DbNumber;	// 8
	BYTE Magumsa;	// C
	char AccountId[11];	// D
	BYTE MoveCnt;	// 18
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CHARLIST
{	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE Class;	// E
	BYTE CtlCode;	// F
	BYTE dbInventory[48];	// 10
	BYTE DbVersion;	// 40
	BYTE btGuildStatus;	// 41
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CREATECHAR
{
	PBMSG_HEAD h;	// C1:04
	int UserNumber;	// 4
	int DBNumber;	// 8
	short Number;	// C
	char AccountId[10];	// E
	char Name[10];	// 18
	BYTE ClassSkin;	// 22
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CREATECHARRESULT
{
	PBMSG_HEAD h;	// C1:04
	unsigned char Result;	// 3
	short Number;	// 4
	char AccountId[10];	// 6
	char Name[10];	// 10
	BYTE Pos;	// 1A
	BYTE ClassSkin;	// 1B
	BYTE Equipment[24];	// 1C
	WORD Level;	// 34
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CHARDELETE
{
	PBMSG_HEAD h;	// C1:05
	short Number;	// 4
	char AccountID[10];	// 6
	char Name[10];	// 10
	BYTE Guild;	// [0:NoGuild] [1:Master] [2:Member] 1A
	char GuildName[8];	// 1B
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CHARDELETERESULT
{
	PBMSG_HEAD h;	// C1:05
	unsigned char Result;	// 3
	short Number;	// 4
	char AccountID[10];	// 6
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_DBCHARINFOREQUEST
{
	PBMSG_HEAD h;	// C1:06
	char AccountID[11];	// 3
	char Name[11];	// E
	short Number;	// 1A
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_DBCHAR_INFORESULT
{
	PWMSG_HEAD h;
	unsigned char result;	// 4
	short Number;	// 6
	char AccountID[10];	// 8
	char Name[11];	// 12
	BYTE Class;	// 1D
	short Level;	// 1E
	int LevelUpPoint;	// 20
	int Exp;	// 24
	int NextExp;	// 28
	int Money;	// 2C
	short Str;	// 30
	short Dex;	// 32
	short Vit;	// 34
	short Energy;	// 36
	WORD Life;	// 38
	WORD MaxLife;	// 3A
	WORD Mana;	// 3C
	WORD MaxMana;	// 3E
	BYTE dbInventory[1728];	// 40
	BYTE dbMagicList[180];	// 700
	BYTE MapNumber;	// 7B4
	BYTE MapX;	// 7B5
	BYTE MapY;	// 7B6
	BYTE Dir;	// 7B7
	int PkCount;	// 7B8
	int PkLevel;	// 7BC
	int PkTime;	// 7C0
	BYTE CtlCode;	// 7C4
	BYTE DbVersion;	// 7C5
	BYTE AccountCtlCode;	// 7C6
	BYTE dbQuest[50];	// 7C7
	WORD Leadership;	// 7FA
	WORD ChatLitmitTime;	// 7FC
	int iFruitPoint;	// 800
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_GETWAREHOUSEDB_SAVE
{
	PWMSG_HEAD h;
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[1920];	// 14
	BYTE DbVersion;	// 794
	short pw;	// 796
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_GETWAREHOUSEDB_RESULT
{
	PBMSG_HEAD h;
	char AccountID[10];	// 3
	short aIndex;	// E
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_ITEMCREATERECV
{
	PBMSG_HEAD h;
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	DWORD m_Number;	// 8
	short Type;	// C
	BYTE Level;	// E
	BYTE Dur;	// F
	BYTE Op1;	// 10
	BYTE Op2;	// 11
	BYTE Op3;	// 12
	BYTE NewOption;	// 13
	int aIndex;	// 14
	short lootindex;	// 18
	BYTE SetOption;	// 1A
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_SKILLKEYDATA_SEND
{
	PBMSG_HEAD h;	// C1:60
	int aIndex;	// 4
	char Name[10];	// 8
	BYTE SkillKeyBuffer[20];	// 12
	BYTE GameOption;	// 1C
	BYTE QkeyDefine;	// 1D
	BYTE WkeyDefine;	// 1E
	BYTE EkeyDefine;	// 1F
	BYTE ChatWindow;	// 20
	BYTE RkeyDefine;	// 21
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_CHARACTER_TRANSFER_RESULT
{
	PBMSG_HEAD h;
	char Account[10];	// 3
	short Number;	// E
	unsigned char Result;	// 10
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_CHARLISTCOUNT
{
	PBMSG_HEAD h;	// C1:F3:00
	BYTE subcode;	// 3
	BYTE MaxClass;	// 4
	BYTE MoveCnt;	// 5
	BYTE Count;	// 6
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_CHARLIST
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_GETCHARLIST
{
	PBMSG_HEAD h;	// C1:01
	char Id[10];	// 3
	short Number;	// E
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_DBCHAR_INFOSAVE
{
	PWMSG_HEAD h;	// C2:07
	char Name[11];	// 4
	short Level;	// 10
	BYTE Class;	// 12
	int LevelUpPoint;	// 14
	int Exp;	// 18
	int NextExp;	// 1C
	int Money;	// 20
	short Str;	// 24
	short Dex;	// 26
	short Vit;	// 28
	short Energy;	// 2A
	WORD Life;	// 2C
	WORD MaxLife;	// 2E
	WORD Mana;	// 30
	WORD MaxMana;	// 32
	BYTE dbInventory[1728];	// 34
	BYTE dbMagicList[180];	// 6F4
	BYTE MapNumber;	// 7A8
	BYTE MapX;	// 7A9
	BYTE MapY;	// 7AA
	BYTE Dir;	// 7AB
	int PkCount;	// 7AC
	int PkLevel;	// 7B0
	int PkTime;	// 7B4
	BYTE dbQuest[50];	// 7B8
	BYTE CharInfoSave;	// 7EA
	WORD Leadership;	// 7EC
	WORD ChatLitmitTime;	// 7EE
	int iFruitPoint;	// 7F0
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_GETWAREHOUSEDB
{
	PBMSG_HEAD	h;	// C1:08
	char		AccountID[10];	// 3
	short		aIndex;	// E
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_GETQUESTMONSTER_KILL
{
	struct PBMSG_HEAD	h;
	int					aIndex;
	char				Name[10];
	UCHAR				Result;
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_DBQUEST_KILLSAVE
{
  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int aIndex;
  /*<thisrel this+0x8>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x12>*/ /*|0x2|*/ char filler[2];
  /*<thisrel this+0x14>*/ /*|0x4|*/ int QuestDBIndex;
  /*<thisrel this+0x18>*/ /*|0x4|*/ int QuestMonsterIndex1;
  /*<thisrel this+0x1c>*/ /*|0x4|*/ int QuestMonsterKillCount1;
  /*<thisrel this+0x20>*/ /*|0x4|*/ int QuestMonsterIndex2;
  /*<thisrel this+0x24>*/ /*|0x4|*/ int QuestMonsterKillCount2;
  /*<thisrel this+0x28>*/ /*|0x4|*/ int QuestMonsterIndex3;
  /*<thisrel this+0x2c>*/ /*|0x4|*/ int QuestMonsterKillCount3;
  /*<thisrel this+0x30>*/ /*|0x4|*/ int QuestMonsterIndex4;
  /*<thisrel this+0x34>*/ /*|0x4|*/ int QuestMonsterKillCount4;
  /*<thisrel this+0x38>*/ /*|0x4|*/ int QuestMonsterIndex5;
  /*<thisrel this+0x3c>*/ /*|0x4|*/ int QuestMonsterKillCount5;
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_DBCHAR_ITEMSAVE
{
	PWMSG_HEAD h;	// C2:11
	char Name[10];	// 4
	BYTE dbInventory[1728];	// E
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_WAREHOUSEMONEY_SAVE
{
	PBMSG_HEAD h;	// C1:12
	char AccountID[10];	// 3
	short aIndex;	// E
	int Money;	// 10
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_ITEMCREATE
{
	PBMSG_HEAD h;	// C1:XX
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	short Type;	// 6
	BYTE Level;	// 8
	BYTE Dur;	// 9
	BYTE Op1;	// A
	BYTE Op2;	// B
	BYTE Op3;	// C
	BYTE NewOption;	// D
	int aIndex;	// 10
	short lootindex;	// 14
	BYTE SetOption;	// 16
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_ITEMMOVESAVE
{
	PBMSG_HEAD h;	// C1:53
	DWORD Serial;	// 4
	char ServerName[20];	// 8
	char Account[10];	// 1C
	char Name[10];	// 26
	char ItemName[21];	// 30
	BYTE X;	// 45
	BYTE Y;	// 46
	BYTE ItemLevel;	// 47
	BYTE ItemOp1;	// 48
	BYTE ItemOp2;	// 49
	BYTE ItemOp3;	// 4A
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_SKILLKEYDATA
{
	PBMSG_HEAD h;	// C1:60
	char Name[10];	// 3
	BYTE SkillKeyBuffer[20];	// D
	BYTE GameOption;	// 17
	BYTE QkeyDefine;	// 18
	BYTE WkeyDefine;	// 19
	BYTE EkeyDefine;	// 1A
	BYTE ChatWindow;	// 1B
	BYTE RkeyDefine;	// 1C
};
//-----------------------------------------------------------------------------------------------------------

struct Request_PetItem_Info
{
	BYTE nPos;	// 0
	int nSerial;	// 4
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_REQUEST_PETITEM_INFO
{
	PWMSG_HEAD h;	// C2:56
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
};
//-----------------------------------------------------------------------------------------------------------

struct Recv_PetItem_Info
{
	BYTE nPos;	// 0
	int nSerial;	// 4
	BYTE Level;	// 8
	int Exp;	// C
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_RECV_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
};
//-----------------------------------------------------------------------------------------------------------

struct Save_PetItem_Info
{
	int nSerial;	// 0
	BYTE Level;	// 3
	int Exp;	// 8
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_SAVE_PETITEM_INFO
{
	PWMSG_HEAD h;
	BYTE nCount;	// 4
};
//-----------------------------------------------------------------------------------------------------------

struct CSP_REQ_CASTLEDATA
{
	PBMSG_HEAD2 h;	// C1:80:00
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};
//-----------------------------------------------------------------------------------------------------------


struct CSP_REQ_OWNERGUILDMASTER
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};
//-----------------------------------------------------------------------------------------------------------

struct CSP_REQ_NPCBUY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcDfLevel;	// 14
	int iNpcRgLevel;	// 18
	int iNpcMaxHp;	// 1C
	int iNpcHp;	// 20
	BYTE btNpcX;	// 24
	BYTE btNpcY;	// 25
	BYTE btNpcDIR;	// 26
	int iBuyCost;	// 28
};
//-----------------------------------------------------------------------------------------------------------

struct CSP_REQ_NPCREPAIR
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iRepairCost;	// 14
};
//-----------------------------------------------------------------------------------------------------------

struct CSP_REQ_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcUpType;	// 14
	int iNpcUpValue;	// 18
	int iNpcUpIndex;	// 1C
};
//-----------------------------------------------------------------------------------------------------------
// LEO123 -----------------------------------------------------------------------
// I STOP HERE NOW POWER TO CHANGE STYLE 

struct CSP_REQ_TAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 3
	int iIndex;	// 8
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iTaxKind;	// C
	int iTaxRate;	// 10
};






/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_MONEYCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iMoneyChanged;	// C
};





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_SDEDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	WORD wStartYear;	// C
	BYTE btStartMonth;	// E
	BYTE btStartDay;	// F
	WORD wEndYear;	// 10
	BYTE btEndMonth;	// 12
	BYTE btEndDay;	// 13
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_SIEGEENDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsSiegeEnded;	// 8
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsCastleOccupied;	// 8
	char szOwnerGuildName[8];	// C
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_REGATTACKGUILD
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szEnemyGuildName[8];	// C
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLESIEGEEND
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDREGMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	int iItemPos;	// 14
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDRESETMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDSETGIVEUP
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	BOOL bIsGiveUp;	// 14
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_NPCREMOVE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iNpcNumber;	// 8
	int iNpcIndex;	// C
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleTributeMoney;	// 8
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_RESETCASTLETAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_RESETSIEGEGUILDINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_RESETREGSIEGEINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 2
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_CSINITDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_NPCDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_ALLGUILDREGINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_CALCREGGUILDLIST
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_CSLOADTOTALGUILDINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode	  :
 */
struct CSP_ANS_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode	  :
 */
struct CSP_ANS_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode	  :
 */
struct CSP_ANS_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
};




struct CWP_REQ_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
};




struct CWP_ANS_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
};




struct CWP_REQ_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};




struct CWP_ANS_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
};


struct CWP_REQ_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
};





struct CWP_ANS_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
};



struct SDHP_CHARACTER_TRANSFER
{
	PBMSG_HEAD h;	// C1:CF
	char Account[10];	// 3
	short Number;	// E
};

struct CSP_REQ_NPCSAVEDATA
{
	PWMSG_HEAD h;	// C1:84
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
};
struct CSP_ANS_CSINITDATA
{
	struct PWMSG_HEAD h;	// C2:81
	int iResult;	// 0x04
	unsigned short wMapSvrNum;	// 0x08
	unsigned short wStartYear;	// 0x0A
	unsigned char btStartMonth;	// 0x0C
	unsigned char btStartDay;	// 0x0D
	unsigned short wEndYear;	// 0x0E
	unsigned char btEndMonth;	// 0x10
	unsigned char btEndDay;		// 0x11
	unsigned char btIsSiegeGuildList;	// 0x12
	unsigned char btIsSiegeEnded;	// 0x13
	unsigned char btIsCastleOccupied; // 0x14
	char szCastleOwnGuild[0x8];	// 0x15
	__int64 i64CastleMoney;	// 0x20
	int iTaxRateChaos;	// 0x28
	int iTaxRateStore;	// 0x2C
	int iTaxHuntZone;	// 0x30
	int iFirstCreate;	// 0x34
	int iCount;	// 0x38
};
// size 0x3C

struct CSP_CSINITDATA
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;// 8
	int iNpcRgLevel;// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;//18
	unsigned char btNpcY;//19
	unsigned char btNpcDIR;//1A
};


struct CSP_ANS_OWNERGUILDMASTER
{
	PBMSG_HEAD2 h; // C1:80:1
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szCastleOwnGuild[0x8];	// 10
	char szCastleOwnGuildMaster[0xA];	// 18
};

struct CSP_ANS_NPCBUY
{
	PBMSG_HEAD2 h;	// C1:80:03
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iNpcNumber;	// 10
	int iNpcIndex;	// 14
	int iBuyCost;	// 18
};

struct CSP_ANS_NPCREPAIR
{
	PBMSG_HEAD2 h;	// C1:80:04
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iNpcNumber;	// 10
	int iNpcIndex;	// 14
	int iNpcMaxHp;	// 18
	int iNpcHpl;	// 1C
	int iRepairCost;	// 20
};

struct CSP_ANS_NPCUPGRADE
{
	PBMSG_HEAD2 h;	// C1:80:05
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iNpcNumber;	// 10
	int iNpcIndex;	// 14
	int iNpcUpType;	// 18
	int iNpcUpValue;	// 1C
	int iNpcUpIndex;	// 20
};

struct CSP_ANS_TAXINFO
{
	PBMSG_HEAD2 h;	// C1:80:06
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	__int64 i64CastleMoney;	// 10
	int iTaxRateChaos;	// 18
	int iTaxRateStore;	// 1C
	int iTaxHuntZone;	// 20
};

struct CSP_ANS_TAXRATECHANGE
{
	PBMSG_HEAD2 h;	// C1:80:07
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iTaxKind;	// 10
	int iTaxRate;	// 14
};

struct CSP_ANS_MONEYCHANGE
{
	PBMSG_HEAD2 h;	// C1:80:08
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iMoneyChanged;	// 10
	__int64 i64CastleMoney;	// 18
};

struct CSP_ANS_SDEDCHANGE
{
	PBMSG_HEAD2 h;	// C1:80:09
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	unsigned short wStartYear;	// 10
	unsigned char btStartMonth;	// 12
	unsigned char btStartDay;	// 13
	unsigned short wEndYear;	// 14
	unsigned char btEndMonth;	// 16
	char btEndDay;	// 17
};

struct CSP_ANS_GUILDREGINFO
{
	PBMSG_HEAD2 h;	// C1:80:0A
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int iRegMarkCount;	// 18
	unsigned char bIsGiveUp;	// 1C
	unsigned char btRegRank;	// 1D
};

struct CSP_ANS_SIEGEENDCHANGE
{
	PBMSG_HEAD2 h;	// C1:80:0B
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int bIsSiegeEnded;	// C
};

struct CSP_ANS_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;	// C1:80:0C
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int bIsCastleOccupied;	// C
	char szOwnerGuildName[0x8];	// 10
};

struct CSP_ANS_REGATTACKGUILD
{
	PBMSG_HEAD2 h;	// C1:80:0D
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szEnemyGuildName[0x8];	// 10
};

struct CSP_ANS_CASTLESIEGEEND
{
	PBMSG_HEAD2 h;	// C1:80:0E
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
};

struct CSP_ANS_GUILDREGMARK
{
	PBMSG_HEAD2 h;	// C1:80:10
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int iItemPos;	// 18
	int iRegMarkCount;	// 1C
};

struct CSP_ANS_GUILDRESETMARK
{
	PBMSG_HEAD2 h;	// C1:80:11
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int iRegMarkCount;	// 18
};

struct CSP_ANS_GUILDSETGIVEUP
{
	PBMSG_HEAD2 h;	// C1:80:12
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int bIsGiveUp;	// 18
	int iRegMarkCount;	// 1C
};

struct CSP_ANS_NPCREMOVE
{
	PBMSG_HEAD2 h;	// C1:80:16
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
};

struct CSP_ANS_RESETCASTLETAXINFO
{
	PBMSG_HEAD2 h;	// C1:80:19
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
};

struct CSP_ANS_RESETSIEGEGUILDINFO
{
	PBMSG_HEAD2 h;	// C1:80:1A
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
};

struct CSP_ANS_RESETREGSIEGEINFO
{
	PBMSG_HEAD2 h;	// C1:80:1B
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
};

struct CSP_ANS_ALLGUILDREGINFO
{
	struct PWMSG_HEAD h;	// C2:83
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iCount;	// 10
};

struct CSP_GUILDREGINFO
{
	char szGuildName[0x8];	// 0
	int iRegMarkCount;	// 8
	unsigned char bIsGiveUp;	// C
	unsigned char btRegRank;	// D
};

struct CSP_NPCSAVEDATA
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;	// 8
	int iNpcRgLevel;	// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;	// 18
	unsigned char btNpcY;	// 19
	unsigned char btNpcDIR;	// 1A
};

struct CSP_ANS_NPCSAVEDATA
{
	PBMSG_HEAD h;	// C1:84
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
};


struct CSP_ANS_CALCREGGUILDLIST
{
	PWMSG_HEAD h;	// C1:85
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iCount;	// C
};

struct CSP_CALCREGGUILDLIST
{
	char szGuildName[0x8];	// 0
	int iRegMarkCount;	// 8
	int iGuildMemberCount;	// C
	int iGuildMasterLevel;	// 10
	int iSeqNum;	// 14
};

struct CSP_REQ_CSGUILDUNIONINFO
{
	PWMSG_HEAD h;	// C2:86
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
};

struct CSP_CSGUILDUNIONINFO
{
	char szGuildName[0x8];	// 0
	int iCsGuildID;	// 8
};

struct CSP_ANS_CSGUILDUNIONINFO
{
	PWMSG_HEAD h;	// C2:86
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iCount;	// C
};


struct CSP_CSSAVETOTALGUILDINFO
{
	char szGuildName[0x8];	// 0
	int iCsGuildID;	// 8
	int iGuildInvolved;	// C
};

struct CSP_REQ_CSSAVETOTALGUILDINFO
{
	PWMSG_HEAD h;	// C2:87
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
	//CSP_CSSAVETOTALGUILDINFO GuildInfo;
};


struct CSP_ANS_CSSAVETOTALGUILDINFO
{
	PBMSG_HEAD h;	// C1:87
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
};

struct CSP_ANS_CSLOADTOTALGUILDINFO
{
	PWMSG_HEAD h;	// C2:88
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iCount;	// C
};

struct CSP_CSLOADTOTALGUILDINFO
{
	char szGuildName[0x8];	// 0

	int iCsGuildID;	// 8
	int iGuildInvolved;	// C
};


struct CSP_REQ_NPCUPDATEDATA
{
	PWMSG_HEAD h;	// C2:89
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
};

struct CSP_NPCUPDATEDATA
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;	// 8
	int iNpcRgLevel;	// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;	// 18
	unsigned char btNpcY;	// 19
	unsigned char btNpcDIR;	// 1A
};

struct CSP_ANS_NPCUPDATEDATA
{
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct PMSG_REQ_PERIODITEM_INSERT
{
	PBMSG_HEAD2 head;	// C1:D0:03
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	int iItemCode;	// C
	int iItemPeriodDate;	// 10
	BYTE btItemEffectType1;	// 14
	BYTE btItemEffectType2;	// 15
	char chUserID[11];	// 16
	char chExpireDate[20];	//21
	BYTE btItemOption;
};
//#pragma pack()
#endif