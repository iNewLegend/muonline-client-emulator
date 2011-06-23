// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#ifndef SPROTOCOL_H
#define SPROTOCOL_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "protocol.h"
#include "EDSProtocol.h"
#include "..\..\Protocol\JoinProtocol.h"
// EventControl
//-----------------------------
#define DRAGON_EVENT		0
#define XMAS_EVENT			1
#define FIRECRACKER_EVENT	2
#define HEARTOFLOVE_EVENT	3
#define MEDAL_EVENT			4

// OptionControl
//-----------------------------
#define OPTION_RELOAD		0
#define MONSTER_RELOAD		1
#define SHOP_RELOAD			2
#define ALLOPTION_RELOAD	3
#define EVENTINFO_RELOAD	4
#define ETCINFO_RELOAD		5
#define OPTION_CONTROL		6


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Bill Search Result
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x06
 */
struct SDHP_BILLSEARCH_RESULT
{
	PBMSG_HEAD h;
	char Id[10];
	short Number;
	unsigned char cCertifyType;
	unsigned char PayCode;
	char EndsDays[12];
	int EndTime;
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Bill Kill User
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x07, 0x09
 */
struct SDHP_BILLKILLUSER
{
	struct PBMSG_HEAD h;
	char Id[10];
	short Number;
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x08
 */
struct SDHP_OTHERJOINMSG
{
	struct PBMSG_HEAD h;
	char AccountID[10];
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x20
 */
struct SDHP_EXITMSG
{
	PBMSG_HEAD h;
	unsigned char ExitCode[3];
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x30
 */
struct SDHP_EVENTSTART
{
	PBMSG_HEAD h;
	unsigned char Event;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x40
 */
struct SDHP_EVENTCONTROL
{
	PBMSG_HEAD h;
	unsigned char Event;
	unsigned char Stat;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x41
 */
struct SDHP_OPTIONCONTROL
{
	PBMSG_HEAD h;
	unsigned char Option;
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x30
 */
struct SDHP_LOVEHEARTEVENT
{
	PBMSG_HEAD h;
	char Account[10];
	char Name[10];
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x61
 */
struct SDHP_LOVEHEARTEVENT_RESULT
{
	PBMSG_HEAD h;
	unsigned char Result;
	char Name[10];
	int Number;
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x31
 */
struct SDHP_LOVEHEARTCREATE
{
	PBMSG_HEAD h;
	unsigned char x;
	unsigned char y;
	unsigned char MapNumber;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> 
 *  Code : 0xC1
 *	HeadCode : 0x60
 */
/*struct PMSG_JG_MEMO_SEND
{
	PWMSG_HEAD h;
	char Name[10];
	char TargetName[10];
	char Subject[32];
	short MemoSize;
	char Memo[1000];
};*/




void SProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen);
void GJServerLogin();
void JGServerLoginResult( SDHP_RESULT * lpMsg);
void GJPUserClose(char* szAccountID);
void GJPUserClose(LPOBJ lpObj);
void JGPAccountRequest(SDHP_IDPASSRESULT * lpMsg);
void GJPAccountFail(short number, char* Id, int DBNumber, int UserNumber);
void GJPAccountBlock(short number, char * Id, int DBNumber, int UserNumber, int BlockCode);
void GJPBillCeckRecv(SDHP_BILLSEARCH_RESULT * lpMsg);
void GJPUserKillRecv(SDHP_BILLKILLUSER * lpMsg);
void JGOtherJoin(SDHP_OTHERJOINMSG * lpMsg);
void GJPUserDisconnectRecv( SDHP_BILLKILLUSER * lpMsg);
void GJPUserDisconnectRecv( SDHP_BILLKILLUSER * lpMsg);
void JGPExitCodeRecv( SDHP_EXITMSG * lpMsg );
void JGPEventStart( SDHP_EVENTSTART * lpMsg );
void JGPEventControl( SDHP_EVENTCONTROL * lpMsg );
void JGPOptionControl( SDHP_OPTIONCONTROL * lpMsg );
void LoveHeartEventSend(int aIndex, LPSTR AcountId, LPSTR Name);
void LoveHeartEventRecv( SDHP_LOVEHEARTEVENT_RESULT * lpMsg );
void LoveHeartCreateSend();
void AllNoticeRecv( SDHP_NOTICE * lpMsg);
void UserNoticeRecv( SDHP_USER_NOTICE * lpMsg);
void GJReqMapSvrMove(int iIndex, WORD wDesMapSvrCode, WORD wMapNumber, BYTE btX, BYTE btY);
void JGAnsMapSvrMove(PMSG_ANS_MAPSVRMOVE * lpMsg);
void GJReqMapSvrAuth(int iIndex, LPSTR lpszAccountID, LPSTR lpszCharName, int iJA1, int iJA2, int iJA3, int iJA4);
void JGAnsMapSvrAuth(PMSG_ANS_MAPSVRAUTH * lpMsg);
void GJNotifyMaxUserCount();
void JGPSendMail(PMSG_JG_MEMO_SEND * lpMsg);
void GJUpdateMatchDBUserCharacters(LPOBJ lpObj);


#endif