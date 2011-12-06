// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
// Order by rpgsky 2010.7.13
#pragma once
#include "string.h"

#define MAX_ITEM_INFO	12


#define CHAR_SET_SIZE	18
#define CS_CLASS		0

//#pragma pack(1)
/*// Korean Protocol
enum E_GAME_POROTOCOL_MSG_KOREAN
{
	GMSG_MOVE				= 0xD3,
	GMSG_POSISTION_SET		= 0xDF,
	GMSG_ATTACK				= 0xD7,
	GMSG_BEATTACK			= 0x10,
};

// jp
#define Jpn_Walk		(0x1D)	//->0x09
#define Jpn_Move		(0xD6)	//->0x59
#define Jpn_Attack		(0xDC)	//->0x5B
#define Jpn_Magic		(0xD7)	//->0x5A

// tw
#define Tai_Walk		(0xD6)
#define Tai_Move		(0xDF)
#define Tai_Attack		(0xDD)
#define Tai_Magic		(0xD2)

*/

// China Protocol
enum e_GAME_POROTOCOL_MSG_CHINA
{
	GMSG_MOVE				= 0xD7,
	GMSG_POSISTION_SET		= 0xD2,
	GMSG_ATTACK				= 0xD9,
	GMSG_BEATTACK			= 0x1D,
};
enum e_GAME_POROTOCOL_MSG_COMMON
{
	GMSG_CHAT						= 0x00,

	GMSG_CHECK_MAINEXE				= 0x03,
	GMSG_VIEW_SKILL_STATE			= 0x07,		// Only S2C.
	GMSG_NOTICE						= 0x0D,
	GMSG_CLIENT_TIME				= 0x0E,		// Only C2S.
	GMSG_WEATHER					= 0x0F,		// Only S2C.

	GMSG_CHAR_VIEWPORT_CREATE		= 0x12,		// Only S2C. Dynamic Array
	GMSG_MONSTER_VIEWPORT_CREATE	= 0x13, 	// Only S2C. Dynamic Array
	GMSG_VIEWPORT_DESTROY			= 0x14,		// Only S2C. Dynamic Array

	GMSG_KILL_PLAYER				= 0x16,
	GMSG_DIE_PLAYER					= 0x17,		// Only S2C.
	GMSG_ACTION						= 0x18,
	GMSG_MAGIC_ATTACK				= 0x19,
	GMSG_MAGIC_CANCEL				= 0x1B,
	GMSG_TELEPORT					= 0x1C,
	GMSG_DURATION_MAGIC				= 0x1E,

	GMSG_SKILL_STATE				= 0x1F,     // rpgsky think its Only S2C. Dynamic Array
	GMSG_ITEM_VIEWPORT_CREATE		= 0x20,		// Only S2C. Dynamic Array
	GMSG_ITEM_VIEWPORT_DESTROY		= 0x21,		// Only S2C. Dynamic Array

	GMSG_REFILL						= 0x26,		// Only S2C.
	GMSG_MANA_SEND					= 0x27,		// Only S2C.
};

enum e_GAME_POROTOCOL_MSG_F3
{
	GMSG_MAGIC_LIST			= 0x11,
};
//////////////////////////////////////////////////////////////////////////

#define SET_CHAR_H(a,b) ((a)=((a)&0x0F)|((b)<<4))
#define SET_CHAR_L(a,b) ((a)=((a)&0xF0)|((b)&0x0F))
#define GET_CHAR_H(a) (((a)&0xF0)>>4)
#define GET_CHAR_L(a) ((a)&0x0F)

enum EQUIP_TYPE
{
	ET_HELM,
	ET_ARMOR,
	ET_PANT,
	ET_GLOVE,
	ET_BOOT,
	ET_BACK,
	ET_RIGHT,
	ET_LEFT,
	ET_MAX,
};
static const char* EQUIP_TYPE_NAME[ET_MAX]=
{
	"helm",
	"armor",
	"pant",
	"glove",
	"boot",
	"back",
	"left",
	"right",
};
//////////////////////////////////////////////////////////////////////////

struct PMSG_JOINRESULT
{
	PMSG_JOINRESULT(int aIndex, unsigned char result, const char* szClientVersion)
	{
		this->h.set(0xF1, sizeof(*this));
		this->scode = 0x00;
		this->result = result;
		this->NumberH = SET_NUMBERH(aIndex);
		this->NumberL = SET_NUMBERL(aIndex);
		this->CliVersion[0] = szClientVersion[0];
		this->CliVersion[1] = szClientVersion[1];
		this->CliVersion[2] = szClientVersion[2];
		this->CliVersion[3] = szClientVersion[3];
		this->CliVersion[4] = szClientVersion[4];
	}
	PBMSG_HEAD h;	// C1:F1
	unsigned char scode;	// 3
	unsigned char result;	// 4
	unsigned char NumberH;	// 5
	unsigned char NumberL;	// 6
	unsigned char CliVersion[5];	// 7
};

struct PMSG_KILLPLAYER_EXT
{
	PBMSG_HEAD h;	// C3:9C
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned short ExpH;	// 6
	unsigned short ExpL;	// 8
	unsigned char DamageH;	// A
	unsigned char DamageL;	// B
};

struct PMSG_NOTICE
{
	PBMSG_HEAD h;
	BYTE type;	// 3
	BYTE btCount;	// 4
	WORD wDelay;	// 6	
	int dwColor;	// 8
	BYTE btSpeed;	// C
	char Notice[256];	// D
};

struct PMSG_ANS_END_DUEL
{
	PBMSG_HEAD h;	// C1:AB
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	char szName[10];	// 5
};

/* * * * * * * * * * * * * * * * * * * * * 
*	Mu Send Warehouse Info Packet
*	Direction : GameServer -> Client
*  Code      : 0xC3
*	HeadCode  : 0x30
*/
struct PMSG_TALKRESULT
{
	PBMSG_HEAD h;
	unsigned char result;	// 3
	unsigned char level1;	// 4
	unsigned char level2;	// 5
	unsigned char level3;	// 6
	unsigned char level4;	// 7
	unsigned char level5;	// 8
	unsigned char level6;	// 9
	unsigned char level7;	// 9
};

/* * * * * * * * * * * * * * * * * * * * * 
*	Mu Result Packet
*	Direction : GameServer -> Client
*  Code     : 0xC1
*	HeadCode : 0xF3
*	SubCode  : 0x02
*/
struct PMSG_RESULT
{
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	unsigned char result;	// 4
};

/* * * * * * * * * * * * * * * * * * * * * 
*	Packet to Send Blod Castle State
*	Direction : GameServer -> Client
*	Code      : 0xC1
*	HeadCode  : 0x9B
*/
struct PMSG_STATEBLOODCASTLE
{
	PBMSG_HEAD h;	// C1:9B
	unsigned char btPlayState;		// 3
	unsigned short wRemainSec;	// 4
	unsigned short wMaxKillMonster;	// 6
	unsigned short wCurKillMonster;	// 8
	unsigned short wUserHaveWeapon;	// A
	unsigned char btWeaponNum;	// C
};

struct PMSG_KILLCOUNT
{
	PBMSG_HEAD2 h;
	unsigned char btKillCount;
};

struct SDHP_DBCHARINFOREQUEST
{
	PBMSG_HEAD h;	// C1:06
	char AccountID[11];	// 3
	char Name[11];	// E
	short Number;	// 1A
};

struct PMSG_ATTACKRESULT
{
	PBMSG_HEAD h;	// C1:DC
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char DamageH;	// 5
	unsigned char DamageL;	// 6
	unsigned char DamageType;	// 7
	unsigned char btShieldDamageH;	// 8
	unsigned char btShieldDamageL;	// 9
};

struct PMSG_RECV_POSISTION_SET
{
	PBMSG_HEAD h;	// C1:D6
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char X;	// 5
	unsigned char Y;	// 6
};

struct PMSG_USEREQUIPMENTCHANGED
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char ItemInfo[MAX_ITEM_INFO];	// 5
};

struct PMSG_SERVERCMD
{
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	unsigned char CmdType;	// 4
	unsigned char X;	// 5
	unsigned char Y;	// 6
};

struct PMSG_ITEMTHROW_RESULT
{
	PBMSG_HEAD h;	// C1:23
	unsigned char Result;	// 3
	unsigned char Ipos;	// 4
};

struct PMSG_GUILDCREATED_RESULT
{
	PBMSG_HEAD h;	// C1:56
	unsigned char Result;	// 3
	unsigned char btGuildType;	// 4
};

struct PMSG_SIMPLE_GUILDVIEWPORT_COUNT
{
	PWMSG_HEAD h;	// C2:65
	unsigned char Count;	// 4
};

struct PMSG_SIMPLE_GUILDVIEWPORT
{
	int GuildNumber;	// 0
	unsigned char btGuildStatus;	// 4
	unsigned char btGuildType;	// 5
	unsigned char btGuildRelationShip;	// 6
	unsigned char NumberH;	// 7
	unsigned char NumberL;	// 8
};

struct PMSG_RECVMOVE
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char X;	// 5
	unsigned char Y;	// 6
	unsigned char Path;	// 7
};

struct PMSG_MAGICATTACK_RESULT
{
	PMSG_MAGICATTACK_RESULT(int nMagicNumber, int nSourceNumber, int nTargetNumber)
	{
		this->h.setE(GMSG_MAGIC_ATTACK, sizeof(*this));
		this->MagicNumberH = SET_NUMBERH(nMagicNumber);
		this->MagicNumberL = SET_NUMBERL(nMagicNumber);
		this->SourceNumberH = SET_NUMBERH(nSourceNumber);
		this->SourceNumberL = SET_NUMBERL(nSourceNumber);
		this->TargetNumberH = SET_NUMBERH(nTargetNumber);
		this->TargetNumberL = SET_NUMBERL(nTargetNumber);
	}
	int GetMagicNumber()
	{
		return MAKE_NUMBERW(MagicNumberH, MagicNumberL);
	}
	PBMSG_HEAD h;	// C3:19
	unsigned char MagicNumberH;	// 3
	unsigned char MagicNumberL;	// 3
	unsigned char SourceNumberH;	// 4
	unsigned char SourceNumberL;	// 5
	unsigned char TargetNumberH;	// 6
	unsigned char TargetNumberL;	// 7
};

struct PMSG_BEATTACK_COUNT
{
	int GetMagicNumber()
	{
		return MAKE_NUMBERW(MagicNumberH, MagicNumberL);
	}
	PBMSG_HEAD h;	// C1:D7
	unsigned char MagicNumberH;
	unsigned char MagicNumberL;
	unsigned char X;	// 4
	unsigned char Y;	// 5
	unsigned char Serial;	// 6
	unsigned char Count;	// 7
};

struct PMSG_BEATTACK
{
	unsigned char NumberH;	// 0
	unsigned char NumberL;	// 1
	unsigned char MagicKey;	// 2
};

struct PMSG_SET_DEVILSQUARE
{
	PBMSG_HEAD h;	// C1:92
	unsigned char Type;	// 3
};

struct PMSG_GUILD_ASSIGN_STATUS_RESULT
{
	PBMSG_HEAD h;	// C1:E1
	unsigned char btType;	// 3
	unsigned char btResult;	// 4
	char szTagetName[10];	// 5
};

struct PMSG_GUILD_ASSIGN_TYPE_RESULT
{
	PBMSG_HEAD h;	// C1:E2
	unsigned char btGuildType;	// 3
	unsigned char btResult;	// 4
};

//////////////////////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//////////////////////////////////////////////////////////////////////////////

struct PMSG_CLIENTTIME
{
	PMSG_CLIENTTIME(unsigned long uTime,unsigned short uAttackSpeed,unsigned short uMagicSpeed)
	{
		this->h.setE(GMSG_CLIENT_TIME,sizeof(*this));
		this->Time=uTime;
		this->AttackSpeed=uAttackSpeed;
		this->MagicSpeed=uMagicSpeed;
	}
	PBMSG_HEAD h;
	unsigned long Time;	// 4
	unsigned short AttackSpeed;	// 8
	unsigned short MagicSpeed;	// A
};


struct PMSG_CHECK_MAINEXE
{
	PMSG_CHECK_MAINEXE(unsigned short key)
	{
		h.set(GMSG_CHECK_MAINEXE,sizeof(*this));
		this->m_wKey = key;
	}
	PBMSG_HEAD h;	// C1:03
	unsigned short m_wKey;	// 4
};

struct PMSG_CHECK_MAINEXE_RESULT
{
	PMSG_CHECK_MAINEXE_RESULT(unsigned long key)
	{
		h.set(GMSG_CHECK_MAINEXE,sizeof(*this));
		this->m_dwKey = key;
	}
	PBMSG_HEAD h;
	unsigned long m_dwKey;	// 4
};

struct PMSG_MAC_ADDR
{
	PBMSG_HEAD	h;
	char		MacAdddr[18];
};

struct PMSG_CHATDATA
{
	PBMSG_HEAD h;	//	
	char chatid[10];	//	3
	char chatmsg[60];	//	D
};

struct PMSG_CHATDATA_NUMBER
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	char chatmsg[60];	// 5
};

struct PMSG_CHATDATA_WHISPER
{
	PBMSG_HEAD h;	// C1:02
	char id[10];	// 3
	char chatmsg[60];	// D  
};

struct PMSG_IDPASS
{
	PMSG_IDPASS(const char* szID,const char* szPassword,unsigned long uTickCount,const char* szClientVersion,const char* szClientSerial)
	{
		memset(this,0,sizeof(*this));
		this->h.setE(0xF1, sizeof(*this));
		this->subcode = 1;
		this->TickCount = uTickCount;
		if (strlen(szID)<=10)
		{
			memcpy(this->Id,szID,strlen(szID));
		}
		if (strlen(szPassword)<=10)
		{
			memcpy(this->Pass,szPassword,strlen(szPassword));
		}
		if (strlen(szClientVersion)<=5)
		{
			memcpy(this->CliVersion,szClientVersion,strlen(szClientVersion));
		}
		if (strlen(szClientSerial)<=16)
		{
			memcpy(this->CliSerial,szClientSerial,strlen(szClientSerial));
		}
	}
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	char Id[10];	// 4
	char Pass[10];	// E
	unsigned long TickCount;	// 18
	unsigned char CliVersion[5];	// 1C
	unsigned char CliSerial[16];	// 21  
};

struct PMSG_CLIENTCLOSE
{
	PBMSG_HEAD h;
	unsigned char SubCode;	// 3
	unsigned char Flag;	// 4
};

struct PMSG_CLIENTMSG
{
	PBMSG_HEAD h;
	unsigned char SubCode;	// 3
	unsigned char Flag;	// 4
	unsigned char subFlag;	// 5
};

struct PMSG_CHARCREATE
{
	PMSG_CHARCREATE(const char* szName,unsigned char uClass)
	{
		this->h.set(0xF3, sizeof(*this));
		this->subcode = 3;
		memset(this->Name,0,10);
		if (strlen(szName)<=10)
		{
			memcpy(this->Name,szName,strlen(szName));
		}
		ClassSkin = uClass;
	}
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	char Name[10];	// 4
	unsigned char ClassSkin;	// E
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Get Char List Result Packet
 *	Direction : GameServer -> Client
 *  Code     : 0xC1
 *	HeadCode : 0xF3
 *	SubCode  : 0x01
 */
struct PMSG_CHARCREATERESULT
{
	PBMSG_HEAD h;	// C1:F3:01
	BYTE subcode;	// 3
	unsigned char Result;	// 4
	char Name[10];	// 5
	BYTE pos;	// F
	WORD Level;	// 10
	BYTE Class;	// 12
	union
	{
		BYTE Equipment[24];	// 13
	};
};


struct PMSG_CHARDELETE
{
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	char Name[10];	// 4
	char LastJoominNumber[10];	// E
};

struct PMSG_CHARMAPJOIN
{
	PMSG_CHARMAPJOIN(const char* szName)
	{
		this->h.set(0xF3, sizeof(*this));
		this->subcode = 3;
		memset(this->Name,0,10);
		if (strlen(szName)<=10)
		{
			memcpy(this->Name,szName,strlen(szName));
		}
	}
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	char Name[10];	// 4
};

struct PMSG_LVPOINTADD
{
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	unsigned char Type;	// 4
};

struct PMSG_SKILLKEY
{
	PBMSG_HEAD h;	// C1:F3:30
	unsigned char subcode;	// 3
	unsigned char SKillKey[10];	// 4
	unsigned char GameOption;	// E
	unsigned char QkeyDefine;	// F
	unsigned char WkeyDefine;	// 10
	unsigned char EkeyDefine;	// 11
	unsigned char ChatWindow;	// 12
};

struct PMSG_ITEMGETREQUEST
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
};

struct PMSG_ITEMTHROW
{
	PBMSG_HEAD h;
	unsigned char px;	// 3
	unsigned char py;	// 4
	unsigned char Ipos;	// 5
};

struct PMSG_INVENTORYITEMMOVE
{
	PBMSG_HEAD h;
	unsigned char sFlag;	// 3
	unsigned char source_item_num;	// 4
	unsigned char sItemInfo[MAX_ITEM_INFO];	// 5
	unsigned char tFlag;	// C
	unsigned char target_item_num;	// D
};

struct PMSG_TALKREQUEST
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
};

struct PMSG_BUYREQUEST
{
	PBMSG_HEAD h;
	unsigned char Pos;	// 3
};

struct PMSG_SELLREQUEST
{
	PBMSG_HEAD h;
	unsigned char Pos;	// 3
};

struct PMSG_ITEMDURREPAIR
{
	PBMSG_HEAD h;
	unsigned char Position;	// [0xFF:Repair ALL] 3
	unsigned char Requestpos;	// 4
};

struct PMSG_TRADE_REQUEST
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
};

struct PMSG_TRADE_RESPONSE
{
	PBMSG_HEAD h;	// C1:37
	unsigned char Response;	// 3
	unsigned char Id[10];	// 4
	unsigned short Level;	// E
	int GuildNumber;	// 10
};

struct PMSG_TRADE_GOLD
{
	PBMSG_HEAD h;
	unsigned long Money;	// 4
};

struct PMSG_TRADE_OKBUTTON
{
	PBMSG_HEAD h;	// C1:3C
	unsigned char Flag;	// 3
};

struct PMSG_REQ_PSHOP_SETITEMPRICE
{
	PBMSG_HEAD2 h;
	unsigned char btItemPos;	// 4
	unsigned char sItemPrice1;	// 5
	unsigned char sItemPrice2;	// 6
	unsigned char sItemPrice3;	// 7
	unsigned char sItemPrice4;	// 8
};

struct PMSG_REQ_PSHOP_OPEN
{
	PBMSG_HEAD2 h;
	char szPShopText[36];	// 4
};

struct PMSG_REQ_BUYLIST_FROM_PSHOP
{
	PBMSG_HEAD2 h;
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
	unsigned char btName[10];	// 6
};

struct PMSG_REQ_BUYITEM_FROM_PSHOP
{
	PBMSG_HEAD2 h;
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
	unsigned char btName[10];	// 6
	unsigned char btItemPos;	// 10
};

struct PMSG_REQ_PSHOPDEAL_CLOSE
{
	PBMSG_HEAD2 h;
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
	unsigned char btName[10];	// 6
};

struct PMSG_PARTYREQUEST
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
};

struct PMSG_PARTYREQUESTRESULT
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
};

struct PMSG_PARTYDELUSER
{
	PBMSG_HEAD h;
	unsigned char Number;	// 3
};

struct PMSG_GUILDJOINQ
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
};

struct PMSG_GUILDQRESULT
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
};

struct PMSG_GUILDDELUSER
{
	PBMSG_HEAD h;
	char Name[10];	// 3
	char JoominNumber[10];	// D
};

struct PMSG_GUILDMASTERANSWER
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
};

struct PMSG_GUILDINFOSAVE
{
	PBMSG_HEAD h;
	unsigned char btGuildType;	// 3
	char GuildName[8];	// 4
	unsigned char Mark[32];	// C
};

struct PMSG_GUILDWARSEND_RESULT
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
};

struct PMSG_WAREHOUSEMONEYINOUT
{
	PBMSG_HEAD h;
	unsigned char Type;	// [0x01:Deposit] [0x02:Withdraw] 3
	int Money;	// 4
};

struct PMSG_WAREHOUSEPASSSEND
{
	PBMSG_HEAD h;
	unsigned char Type;	// [0x00:Open Warehouse] [0x01:Set Pass] [0x02:Delete Pass] 3
	short Pass;	// 4
	char LastJoominNumber[10];	// 6
};

struct PMSG_CHAOSMIX
{
	PBMSG_HEAD h;
	unsigned char Type;	// 3
};

struct PMSG_MOVE
{
	PBMSG_HEAD h;	// C1:1D
	unsigned char X;	// 3
	unsigned char Y;	// 4
	unsigned char Path[8];	// 5
};

struct PMSG_POSISTION_SET
{
	PBMSG_HEAD h;
	unsigned char X;	// 3
	unsigned char Y;	// 4
};

struct PMSG_ATTACK
{
	PMSG_ATTACK(unsigned char AttackAction, int Number, unsigned char DirDis)
	{
		this->h.set(GMSG_ATTACK,sizeof(*this));
		this->NumberH = SET_NUMBERH(Number);
		this->NumberL = SET_NUMBERL(Number);
		this->AttackAction = AttackAction;
		this->DirDis = DirDis;
	}
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char AttackAction;	// 5
	unsigned char DirDis;	// 6
};

struct PMSG_KILLPLAYER
{
	PBMSG_HEAD h;	// C1:16
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char ExpH;	// 5
	unsigned char ExpL;	// 6
	unsigned char DamageH;	// 7
	unsigned char DamageL;	// 8
};

struct PMSG_DIEPLAYER
{
	PBMSG_HEAD h;	// C1:17
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char SkillH;	// 5
	unsigned char SkillL;	// 5
	unsigned char KillerNumberH;	// 6
	unsigned char KillerNumberL;	// 7
};

struct PMSG_ACTION
{
	PBMSG_HEAD h;
	unsigned char Dir;	// 3
	unsigned char ActionNumber;	// 4
	unsigned char iTargetIndexH;	// 5
	unsigned char iTargetIndexL;	// 6
};

struct PMSG_ACTIONRESULT
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char Dir;	// 5
	unsigned char ActionNumber;	// 6
	unsigned char TargetNumberH;	// 7
	unsigned char TargetNumberL;	// 8
};

struct PMSG_MAGICATTACK
{
	PMSG_MAGICATTACK(int nMagicNumber, int nSourceNumber, int nDis)
	{
		this->h.set(GMSG_MAGIC_CANCEL, sizeof(*this));
		this->MagicNumberH =SET_NUMBERH(nMagicNumber);
		this->MagicNumberL =SET_NUMBERL(nMagicNumber);
		this->NumberH = SET_NUMBERH(nSourceNumber);
		this->NumberL = SET_NUMBERL(nSourceNumber);
		this->DisH = SET_NUMBERH(nDis);
		this->DisL = SET_NUMBERL(nDis);
	}
	int GetMagicNumber()
	{
		return MAKE_NUMBERW(MagicNumberH, MagicNumberL);
	}
	PBMSG_HEAD h;
	unsigned char MagicNumberH;
	unsigned char MagicNumberL;
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
	unsigned char DisH;
	unsigned char DisL;	// 6
};

struct PMSG_MAGICCANCEL 
{
	PMSG_MAGICCANCEL(int nMagicNumber, int nSourceNumber)
	{
		this->h.set(GMSG_MAGIC_CANCEL, sizeof(*this));
		this->MagicNumberH =SET_NUMBERH(nMagicNumber);
		this->MagicNumberL =SET_NUMBERL(nMagicNumber);
		this->NumberH = SET_NUMBERH(nSourceNumber);
		this->NumberL = SET_NUMBERL(nSourceNumber);
	}
	int GetMagicNumber()
	{
		return MAKE_NUMBERW(MagicNumberH, MagicNumberL);
	}
	PBMSG_HEAD h;
	unsigned char MagicNumberH;
	unsigned char MagicNumberL;
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
};

/*struct PMSG_TELEPORT
{
PBMSG_HEAD h;
unsigned char MoveNumber;	// 3
unsigned char MapX;	// 4
unsigned char MapY;	// 5
};*/

struct PMSG_TELEPORT
{
	PBMSG_HEAD h;

	unsigned char Dir; // FUCK
	unsigned short MoveNumber;	// 3
	unsigned char MapX;	// 4
	unsigned char MapY;	// 5
};

struct PMSG_TELEPORT_RESULT
{
	PMSG_TELEPORT_RESULT(int nIndex, unsigned short MoveNumber, unsigned char MapNumber, unsigned char MapX, unsigned char MapY, unsigned char Dir )
	{
		this->h.setE(GMSG_TELEPORT, sizeof(*this));
		this->GateH = SET_NUMBERH(nIndex);
		this->GateL = SET_NUMBERL(nIndex);
		this->MoveNumber = MoveNumber;
		this->MapNumber = MapNumber;
		this->MapX = MapX;
		this->MapY = MapY;
		this->Dir = Dir;
	}
	PBMSG_HEAD h;	// C3:1C
	unsigned char GateH;
	unsigned char GateL;
	unsigned char MoveNumber;	// 3
	unsigned char MapNumber;	// 4
	unsigned char MapX;	// 5
	unsigned char MapY;	// 6
	unsigned char Dir;	// 7
};

struct PMSG_TARGET_TELEPORT
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	unsigned char MapX;	// 5
	unsigned char MapY;	// 6
};

struct PMSG_DURATION_MAGIC_RECV
{
	int GetMagicNumber()
	{
		return MAKE_NUMBERW(MagicNumberH, MagicNumberL);
	}
	PBMSG_HEAD h;
	unsigned char MagicNumberH;
	unsigned char MagicNumberL;
	unsigned char X;	// 4
	unsigned char Y;	// 5
	unsigned char Dir;	// 6
	unsigned char Dis;	// 7
	unsigned char TargetPos;	// 8
	unsigned char NumberH;	// 9
	unsigned char NumberL;	// A
	unsigned char MagicKey;	// B
};

struct PMSG_DURATION_MAGIC_SEND
{
	PMSG_DURATION_MAGIC_SEND(int nMagicNumber, int nSourceNumber, unsigned char X, unsigned char Y, unsigned char Dir)
	{
		this->h.setE(GMSG_DURATION_MAGIC, sizeof(*this));
		this->MagicNumberH =SET_NUMBERH(nMagicNumber);
		this->MagicNumberL =SET_NUMBERL(nMagicNumber);
		this->NumberH = SET_NUMBERH(nSourceNumber);
		this->NumberL = SET_NUMBERL(nSourceNumber);
		this->X = X;
		this->Y = Y;
		this->Dir = Dir;
		this->unknown = 0;
	}
	PBMSG_HEAD h;
	unsigned char MagicNumberH;
	unsigned char MagicNumberL;
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char X;
	unsigned char Y;
	unsigned char Dir;
	unsigned char unknown;
};

struct PMSG_USEITEM
{
	PBMSG_HEAD h;
	unsigned char inventoryPos;	// 3
	unsigned char invenrotyTarget;	// 4
	unsigned char btItemUseType;	// 5
};

struct PMSG_REQ_MOVEDEVILSQUARE
{
	PBMSG_HEAD h;
	unsigned char SquareNumber;	// 3
	unsigned char InvitationItemPos;	// 4
};

struct PMSG_REQ_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD h;
	unsigned char hEventType;	// 3
	unsigned char btItemLevel;	// 4
};

struct PMSG_PING_RESULT
{
	PBMSG_HEAD h;
};

struct PMSG_REGEVENTCHIP
{
	PBMSG_HEAD h;
	unsigned char Type;	// 3
	unsigned char ChipPos;	// 4
};

struct PMSG_GETMUTONUMBER
{
	PBMSG_HEAD h;
};

struct PMSG_EXCHANGE_EVENTCHIP
{
	PBMSG_HEAD h;
	unsigned char btType;	// [0x00:Change Renas] [0x01:Change Stones]	3
};

struct PMSG_SETQUEST 
{
	PBMSG_HEAD h;	// C1:A1
	unsigned char QuestIndex;	// 3
	unsigned char State;	// 4
};

struct PMSG_REQ_MOVEBLOODCASTLE
{
	PBMSG_HEAD h;
	unsigned char iBridgeNumber;	// 3
	unsigned char iItemPos;	// 4
};

struct PMSG_REQ_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;
	unsigned char iChaosCastleNumber;	// 4
	unsigned char iItemPos;	// 5
};

struct PMSG_REQ_REPOSUSER_IN_CC
{
	PBMSG_HEAD2 h;
	unsigned char btPX;	// 4
	unsigned char btPY;	// 5
};

struct PMSG_REQ_CL_ENTERCOUNT
{
	PBMSG_HEAD h;
	unsigned char btEventType;	// 3
};

struct PMSG_REQ_2ANV_LOTTO_EVENT
{
	PBMSG_HEAD h;
	char SERIAL1[5];	// 3
	char SERIAL2[5];	// 8
	char SERIAL3[5];	// D
};

struct PMSG_REQ_MOVE_OTHERSERVER
{
	PBMSG_HEAD h;
	char LastJoominNumber[10];	// 3
};

struct PMSG_PACKETCHECKSUM
{
	PBMSG_HEAD h;
	unsigned char funcindex;	// 3
	unsigned long CheckSum;	// 4
};


struct __GG_AUTH_DATA
{
	unsigned long dwIndex;
	unsigned long dwValue1;
	unsigned long dwValue2;
	unsigned long dwValue3;
};

struct PMSG_NPROTECTGGCHECKSUM
{
	PBMSG_HEAD h;	// C3:73
	__GG_AUTH_DATA m_ggadCheckSum;	// 4
};

struct PMSG_REQ_START_DUEL
{
	PBMSG_HEAD h;
	unsigned char NumberH;	// 3
	unsigned char NumberL;	// 4
	char szName[10];	// 5
};

struct PMSG_REQ_END_DUEL
{
	PBMSG_HEAD h;
};

struct PMSG_ANS_DUEL_OK
{
	PBMSG_HEAD h;
	bool bDuelOK;	// 3
	unsigned char NumberH;	// 4
	unsigned char NumberL;	// 5
	char szName[10];	// 6
};

struct PMSG_REQUEST_PET_ITEM_COMMAND
{
	PBMSG_HEAD h;
	unsigned char PetType;	// 3
	unsigned char Command;	// 4
	unsigned char NumberH;	// 5
	unsigned char NumberL;	// 6
};

struct PMSG_REQUEST_PET_ITEMINFO
{
	PBMSG_HEAD h;
	unsigned char PetType;	// 3
	unsigned char InvenType;	// 4
	unsigned char nPos;	// 5
};

struct PMSG_REQ_GUILDVIEWPORT
{
	PBMSG_HEAD h;
	int GuildNumber;	// 4
};

struct PMSG_GUILD_ASSIGN_STATUS_REQ
{
	PBMSG_HEAD h;
	unsigned char btType;	// 3
	unsigned char btGuildStatus;	// 4
	char szTagetName[10];	// 5
};

struct PMSG_GUILD_ASSIGN_TYPE_REQ
{
	PBMSG_HEAD h;
	unsigned char btGuildType;	// 3
};

struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ
{
	PBMSG_HEAD h;	// C1:E5
	unsigned char btRelationShipType;	// 3
	unsigned char btRequestType;	// 4
	unsigned char btTargetUserIndexH;	// 5
	unsigned char btTargetUserIndexL;	// 6
};

struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS
{
	PBMSG_HEAD h;	// C1:E6
	unsigned char btRelationShipType;	// 3
	unsigned char btRequestType;	// 4
	unsigned char btResult;	// 5
	unsigned char btTargetUserIndexH;	// 6
	unsigned char btTargetUserIndexL;	// 7
};

struct PMSG_UNIONLIST_REQ
{
	PBMSG_HEAD h;







};

struct PMSG_KICKOUT_UNIONMEMBER_REQ
{
	PBMSG_HEAD2 h;
	char szTargetGuildName[8];	// 4
};

struct PMSG_REQ_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;
	char szAccountID[12];	// 4
	char szCharName[12];	// 10
	int iJoinAuthCode1;	// 1C
	int iJoinAuthCode2;	// 20
	int iJoinAuthCode3;	// 24
	int iJoinAuthCode4;	// 28
	int iTickCount;	// 2C
	unsigned char btCliVersion[5];	// 30
	unsigned char btCliSerial[16];	// 35

};

struct PMSG_REQ_CASTLESIEGESTATE {

	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;



};

struct PMSG_REQ_REGCASTLESIEGE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_GIVEUPCASTLESIEGE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btGiveUp;










};

struct PMSG_REQ_GUILDREGINFO {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct CSP_ANS_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[8];	// 10
	int iRegMarkCount;	// 18
	unsigned char bIsGiveUp;	// 1C
	unsigned char btRegRank;	// 1D
};

struct PMSG_REQ_REGGUILDMARK {



	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btItemPos;

};

struct CSP_ANS_GUILDREGMARK {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ unsigned short wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iItemPos;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iRegMarkCount;

	// base classes -----------------------------------

	// friends ----------------------------------------

	// static functions -------------------------------

	// non-virtual functions --------------------------

	// virtual functions ------------------------------
};
// <size 0x20>

struct PMSG_REQ_NPCBUY {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcIndex;










};

struct PMSG_REQ_NPCREPAIR {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcIndex;










};

struct PMSG_REQ_NPCUPGRADE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iNpcUpType;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcUpValue;










};

struct PMSG_REQ_TAXMONEYINFO {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_TAXRATECHANGE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btTaxType;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btTaxRate1;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btTaxRate2;
	/*<thisrel this+0x7>*/ /*|0x1|*/ unsigned char btTaxRate3;
	/*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char btTaxRate4;










};

struct PMSG_REQ_MONEYDRAWOUT {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btMoney1;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btMoney2;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btMoney3;
	/*<thisrel this+0x7>*/ /*|0x1|*/ unsigned char btMoney4;










};

struct PMSG_REQ_CSGATEOPERATE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btOperate;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btIndex1;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btIndex2;










};

struct PMSG_REQ_MINIMAPDATA {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_STOPMINIMAPDATA {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_CSCOMMAND {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btTeam;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btX;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btY;
	/*<thisrel this+0x7>*/ /*|0x1|*/ unsigned char btCommand;










};

struct PMSG_REQ_CSHUNTZONEENTER {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btHuntZoneEnter;










};

struct PMSG_REQ_NPCDBLIST {




	PBMSG_HEAD h;
	/*<thisrel this+0x3>*/ /*|0x1|*/ unsigned char btMonsterCode;










};

struct PMSG_REQ_CSREGGUILDLIST {




	PBMSG_HEAD h;










};

struct PMSG_REQ_CSATTKGUILDLIST {




	PBMSG_HEAD h;










};

struct PMSG_REQ_USEWEAPON {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btObjIndexH;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btObjIndexL;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btTargetIndex;










};

struct PMSG_REQ_WEAPON_DAMAGE_VALUE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btObjIndexH;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btObjIndexL;










};

struct PMSG_REQ_GUILDMARK_OF_CASTLEOWNER {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_JEWEL_MIX {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btJewelType;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btJewelMix;










};

struct PMSG_REQ_JEWEL_UNMIX {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btJewelType;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btJewelLevel;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btJewelPos;










};

struct PMSG_REQ_CRYWOLF_INFO {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_CRYWOLF_ALTAR_CONTRACT {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btObjIndexH;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btObjIndexL;










};

struct PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_KANTURU_STATE_INFO {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct PMSG_REQ_ENTER_KANTURU_BOSS_MAP {




	/*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;










};

struct CSP_ANS_CSINITDATA { // <size 0x40>
	PWMSG_HEAD h;	// 0
	int iResult;		// 4
	unsigned short wMapSvrNum;	// 8
	unsigned short wStartYear;	// a
	unsigned char btStartMonth;	// c
	unsigned char btStartDay;	// d
	unsigned short wEndYear; // e
	unsigned char btEndMonth; // 10
	unsigned char btEndDay;	// 11
	unsigned char btIsSiegeGuildList;	// 12
	unsigned char btIsSiegeEnded;	// 13
	unsigned char btIsCastleOccupied;	// 14
	char szCastleOwnGuild[8]; // 15
	__int64 i64CastleMoney; // 20
	int iTaxRateChaos; // 28
	int iTaxRateStore; // 2c
	int iTaxHuntZone; // 30
	int iFirstCreate; // 34
	int iCount; // 38
};

struct CSP_CSINITDATA {// <size 0x1c>
	int iNpcNumber; // 0
	int iNpcIndex; // 4
	int iNpcDfLevel; // 8
	int iNpcRgLevel; // c
	int iNpcMaxHp; // 10
	int iNpcHp; // 14
	unsigned char btNpcX; // 18
	unsigned char btNpcY; // 19
	unsigned char btNpcDIR; // 1a
};

struct CSP_CALCREGGUILDLIST { // <size 0x18>
	char szGuildName[8]; // 0
	int iRegMarkCount; // 8
	int iGuildMemberCount; // c
	int iGuildMasterLevel; // 10
	int iSeqNum; // 14
};

struct CSP_CSGUILDUNIONINFO { // <size 0xc>
	char szGuildName[8]; // 0
	int iCsGuildID; // 8
};

struct CSP_CSLOADTOTALGUILDINFO { // <size 0x10>
	char szGuildName[8]; // 0
	int iCsGuildID; // 8
	int iGuildInvolved; // c
};

struct PMSG_CSATTKGUILDLIST { // <size 0xa>
	unsigned char btCsJoinSide; // 0
	unsigned char btGuildInvolved; // 1
	char szGuildName[8]; // 2
};

struct CSP_ANS_NPCDATA { // <size 0x14>
	PWMSG_HEAD h; // 0
	int iResult; // 4
	unsigned short wMapSvrNum; // 8
	int iIndex; // c
	int iCount; // 10
};

struct PMSG_ANS_NOTIFYCSSTART { // <size 0x5>
	PBMSG_HEAD2 h; // 0
	unsigned char btStartState; // 4
};

struct PMSG_ANS_NOTIFYCSPROGRESS { // <size 0xd>
	PBMSG_HEAD2 h; // 0
	unsigned char btCastleSiegeState; // 4
	unsigned char szGuildName[8]; // 5
};

struct PMSG_IT_MOVE_MAP
{
	PBMSG_HEAD2	h;
	unsigned char btIllusionIndex;
	unsigned char btIllusionTicketPos;

};

struct PMSG_NOTIFY_OBJECT_CREATION_STATE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btObjIndexH;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btObjIndexL;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btCreationState;
};


struct PMSG_ANS_CSREGGUILDLIST {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btResult;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iCount;
};

struct PMSG_CSREGGUILDLIST {

	/*<thisrel this+0x0>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char btRegMarks1;
	/*<thisrel this+0x9>*/ /*|0x1|*/ unsigned char btRegMarks2;
	/*<thisrel this+0xa>*/ /*|0x1|*/ unsigned char btRegMarks3;
	/*<thisrel this+0xb>*/ /*|0x1|*/ unsigned char btRegMarks4;
	/*<thisrel this+0xc>*/ /*|0x1|*/ unsigned char btIsGiveUp;
	/*<thisrel this+0xd>*/ /*|0x1|*/ unsigned char btSeqNum;
};

struct PMSG_ANS_CSCOMMAND {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btTeam;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btX;
	/*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btY;
	/*<thisrel this+0x7>*/ /*|0x1|*/ unsigned char btCommand;

};

struct PMSG_ANS_NPCDBLIST {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btResult;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iCount;

};

struct PMSG_ANS_SENDMINIMAPDATA {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iCount;

};

struct PMSG_ANS_SENDNPCMINIMAPDATA {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iCount;

};

struct PMSG_SENDNPCMINIMAPDATA {

	/*<thisrel this+0x0>*/ /*|0x1|*/ unsigned char btNpcType;
	/*<thisrel this+0x1>*/ /*|0x1|*/ unsigned char btX;
	/*<thisrel this+0x2>*/ /*|0x1|*/ unsigned char btY;

};

struct PMSG_ANS_GUARD_IN_CASTLE_HUNTZONE {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btResult;
	/*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btUsable;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iCurrentPrice;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iMaxPrice;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iUnitOfPrice;

};

struct PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE // 0x5
{
	PBMSG_HEAD2 h; // +0x0(0x4)
	unsigned char btResult; // +0x4(0x1)
};

struct PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iPrice;
};

//////////////////////////////////////////////////////////////////////////




/* * * * * * * * * * * * * * * * * * * * * 
*	Mu Char List Count Packet
*	Direction : GameServer -> Client
*  Code     : 0xC1
*	HeadCode : 0xF3
*	SubCode  : 0x00
*	BASE PACKET - Complete with PMSG_CHARLIST
*/
struct PMSG_CHARLISTCOUNT
{
	PBMSG_HEAD h;	// C1:F3:00
	unsigned char subcode;	// 3
	unsigned char MaxClass;	// 4
	unsigned char MoveCnt;	// 5
	unsigned char Count;	// 6
};

struct PMSG_CHARLIST
{
	unsigned char Index;	// 0
	char Name[10];	// 1
	unsigned short Level;	// C
	unsigned char CtlCode;	// E
	unsigned char CharSet[CHAR_SET_SIZE];	// F
	unsigned char btGuildStatus;	// 21

};

struct PMSG_REFILL
{
	PMSG_REFILL(unsigned char Ipos, int Life, int wShield)
	{
		this->h.set(GMSG_REFILL, sizeof(*this));
		this->IPos = Ipos;
		this->LifeH = SET_NUMBERH(Life);
		this->LifeL = SET_NUMBERL(Life);
		this->btShieldH = SET_NUMBERH(wShield);
		this->btShieldL = SET_NUMBERL(wShield);
		this->Flag = 0;	// #error Flag is Disabled
	}

	PBMSG_HEAD h;	// C1:26
	unsigned char IPos;	// 3
	unsigned char LifeH;	// 4
	unsigned char LifeL;	// 5
	unsigned char Flag;	// 6
	unsigned char btShieldH;	// 7
	unsigned char btShieldL;	// 8
};

struct PMSG_MANASEND
{
	PMSG_MANASEND(unsigned char Ipos, int Mana, int BP)
	{
		this->h.set(GMSG_MANA_SEND, sizeof(*this));
		this->IPos = Ipos;
		this->ManaH = SET_NUMBERH(Mana);
		this->ManaL = SET_NUMBERL(Mana);
		this->BPH = SET_NUMBERH(BP);
		this->BPL = SET_NUMBERL(BP);
	}
	PBMSG_HEAD h;	// C1:27
	unsigned char IPos;	// 3
	unsigned char ManaH;	// 4
	unsigned char ManaL;	// 5
	unsigned char BPH;	// 6
	unsigned char BPL;	// 7
};

struct PMSG_WEATHER
{
	PMSG_WEATHER(unsigned char weather)
	{
		this->h.set(GMSG_WEATHER,sizeof(*this));
		this->Weather = weather;
	}
	PBMSG_HEAD h;	// C1:0F
	unsigned char Weather;	// 3
};

/* * * * * * * * * * * * * * * * * * * * * 
*	Mu Get Character Info Packet
*	Direction : GameServer -> Client
*  Code     : 0xC3
*	HeadCode : 0xF3
*	SubCode  : 0x03
*/

struct CHARACTER_DATA
{
	unsigned char MapX;	// 4
	unsigned char MapY;	// 5
	unsigned char MapNumber;	// 6
	unsigned char Dir;	// 7
	// 1.03H经验值变64位
	unsigned __int64 Exp;	// 8
	unsigned __int64 NextExp;	// C
	//unsigned long Exp0;	// 8
	//unsigned long Exp;	// 8
	//unsigned long NextExp0;	// C
	//unsigned long NextExp;	// C
	unsigned short LevelUpPoint;	// 10
	unsigned short Str;	// 12
	unsigned short Dex;	// 14
	unsigned short Vit;	// 16
	unsigned short Energy;	// 18
	unsigned short Life;	// 1A
	unsigned short MaxLife;	// 1C
	unsigned short Mana;	// 1E
	unsigned short MaxMana;	// 20
	unsigned short wShield;	// 22
	unsigned short wMaxShield;	// 24
	unsigned short BP;	// 26
	unsigned short MaxBP;	// 28
	int Money;	// 2C
	unsigned char PkLevel;	// 30
	unsigned char CtlCode;	// 31
	short AddPoint;	// 32
	short MaxAddPoint;	// 34
	unsigned short Leadership;	// 36
	unsigned short wMinusPoint;	// 38
	unsigned short wMaxMinusPoint;	// 3A
};

struct PMSG_CHARMAPJOINRESULT
{
	PBMSG_HEAD h;	// C3:F3:03
	unsigned char subcode;	// 3
	CHARACTER_DATA data;
};

/* * * * * * * * * * * * * * * * * * * * * 
*	Mu Get Item List Packet
*	Direction : GameServer -> Client
*  Code     : 0xC4
*	HeadCode : 0xF3
*	SubCode  : 0x10
*	BASE PACKET - Complete with PMSG_INVENTORYLIST
*/
struct PMSG_INVENTORYLISTCOUNT
{
	PWMSG_HEAD h;	// C4:F3:10
	unsigned char subcode;	// 4
	unsigned char Count;	// 5
};

struct PMSG_INVENTORYLIST
{
	unsigned char Pos;	// 0
	unsigned char ItemInfo[MAX_ITEM_INFO];	// 1
};

struct PMSG_VIEWSKILLSTATE
{
	PMSG_VIEWSKILLSTATE(int nIndex, unsigned char btState, unsigned char btViewSkillState)
	{
		this->h.set(GMSG_VIEW_SKILL_STATE,sizeof(*this));
		this->State = btState;
		this->ViewSkillState = btViewSkillState;
		this->NumberH = SET_NUMBERH(nIndex);
		this->NumberL = SET_NUMBERL(nIndex);
	}
	PBMSG_HEAD h;	// C1:07
	unsigned char State;	// 3
	unsigned char NumberH;	// 8
	unsigned char NumberL;	// 9
	unsigned char ViewSkillState;	// 4
};


struct PWMSG_COUNT
{
	struct PWMSG_HEAD h;
	unsigned char count;
};

template <typename T>
struct DynamicArrayWMsg
{
	struct PWMSG_HEAD h;
	unsigned char count;

	unsigned char getCount()
	{
		return count;
	}

	T* getItem(int id)
	{
		return  (T*)( (&count)+sizeof(count)+id*sizeof(T) );
	}

	const T* getItem(int id)const
	{
		return  (T*)( (&count)+sizeof(count)+id*sizeof(T) );
	}
};

struct DynamicWMsg
{
	PWMSG_COUNT* pMsg;
	unsigned char* pPointer;

	DynamicWMsg(const unsigned char* msg)
	{
		pMsg = (PWMSG_COUNT*)msg;
		pPointer = (unsigned char*)msg;
		pPointer += sizeof(PWMSG_COUNT);
	}
	unsigned char getCount()
	{
		return pMsg->count;
	}

	template <typename T>
	T* getData()
	{
		T* ret = (T*)pPointer;
		pPointer += sizeof(T);
		return ret;
	}
};

// <size 0x4>

struct PMSG_ITEMVIEWPORTCREATE
{
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char px;
	unsigned char py;
	unsigned char ItemInfo[MAX_ITEM_INFO];
};
// <size 0xb>

struct PMSG_VIEWPORTCREATE
{
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char X;
	unsigned char Y;
	unsigned char CharSet[CHAR_SET_SIZE];	// F
	//unsigned long ViewSkillState;
	char Id[10];
	unsigned char TX;
	unsigned char TY;
	unsigned char DirAndPkLevel;
	unsigned char btViewSkillCount;
};
// <size 0x2c>

struct PMSG_VIEWPORTCREATE_CHANGE
{
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char X;
	unsigned char Y;
	unsigned char SkinH;
	unsigned char SkinL;
	//int ViewSkillState;
	char Id[10];
	unsigned char TX;
	unsigned char TY;
	unsigned char DirAndPkLevel;
	unsigned char CharSet[CHAR_SET_SIZE];	// F
	//unsigned char CharSet[18]; //para o illusion temple
	unsigned char btViewSkillCount;
};
// <size 0x1c>

struct PMSG_VIEWPORTDESTROY
{
	unsigned char NumberH;
	unsigned char NumberL;
};

/* 1.03H by huihui*/
struct PMSG_MONSTER_VIEWPORTCREATE
{
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char Type_HI;
	unsigned char Type_LO;
	unsigned char X;
	unsigned char Y;
	unsigned char TX;
	unsigned char TY;
	unsigned char Path;
	unsigned char btViewSkillCount;
};

struct PMSG_CALLMONSTER_VIEWPORTCREATE
{
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char Type_HI;
	unsigned char Type_LO;
	//int ViewState;
	unsigned char X;
	unsigned char Y;
	unsigned char TX;
	unsigned char TY;
	unsigned char Path;
	unsigned char Id[10];
	unsigned char btViewSkillCount;
};
// <size 0x18>

struct PMSG_GUILDVIEWPORT_USER
{
	unsigned char NumberH;
	unsigned char NumberL;
	unsigned char GNumberH;
	unsigned char GNumberL;
};
// <size 0x4>

struct PMSG_GUILDVIEWPORT
{
	unsigned char NumberH;
	unsigned char NumberL;
	char GuildName[8];
	unsigned char Mark[32];
};
// <size 0x2a>
//#pragma pack()