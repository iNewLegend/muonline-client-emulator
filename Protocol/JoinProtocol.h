//-----------------------------------------------------------------------------------------------------------
#ifndef JOINPROTOCOL_H
#define JOINPROTOCOL_H
//-----------------------------------------------------------------------------------------------------------
// Order by rpgsky 2010.7.13

enum e_JOIN_POROTOCOL_MSG
{
	JMSG_SERVER_LOGIN		= 0x00,
	JMSG_IDPASS				= 0x01,
	JMSG_JOIN_FAIL			= 0x02,
	JMSG_COMMAND_BLOCK		= 0x04,
	JMSG_USER_CLOSE_BY_ID	= 0x05,
	JMSG_MAP_SVR_MOVE		= 0x7A,
	JMSG_MAP_SVR_AUTH		= 0x7B,
	JMSG_NOTIFY_MAX_USER	= 0x7C,
};

//-----------------------------------------------------------------------------------------------------------

struct SDHP_IDPASS
{
	PBMSG_HEAD	h;				// C1:01
	char		Id[10];
	char		Pass[10];	
	short		Number;
	char		IpAddress[17];
};
//-----------------------------------------------------------------------------------------------------------

enum e_IDPASSRESULT														// #
{
	eIDPASSRESULT_BADPASS			=0x00, // your password is wrong. 
	eIDPASSRESULT_SUCCESS			=0x01, // Success. 
	eIDPASSRESULT_BADID				=0x02, // id does not exist. 
	eIDPASSRESULT_INUSE				=0x03, // account is use 
	eIDPASSRESULT_MAXSERVER			=0x04, // maxium active user reached
	eIDPASSRESULT_ACCBANED			=0x05, // account banned
	eIDPASSRESULT_FIX_EXP			=0x0C, // fixed expiration 
	eIDPASSRESULT_INUSE_DET_EXP		=0x0D, // Determination of Expiration 
	eIDPASSRESULT_TEMPBLOCK			=0x0E, // a temporary block status (MuManager used) 
	eIDPASSRESULT_BADCOUNTRY		=0xd2, // IP is not a local IP (not country for player)
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_IDPASSRESULT												// #
{
	SDHP_IDPASSRESULT(unsigned char result, short number, int userNumber, int DBNumber, const char* szAccountID, const char* JoominNumber)
	{
		h.set(JMSG_IDPASS, sizeof(*this));
		// ----
		this->result		= result;
		this->Number		= number;
		this->UserNumber	= userNumber;
		this->DBNumber		= DBNumber;
		// ----
		memcpy(this->Id, szAccountID, 10);
		memcpy(this->JoominNumber, JoominNumber , 13);
	}
	PBMSG_HEAD	h;				// C1:01
	UCHAR		result;
	short		Number;
	char		Id[10];
	int			UserNumber;
	int			DBNumber;
	char		JoominNumber[13];
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_JOINFAIL
{
	PBMSG_HEAD	h;				// C1:02
	short		Number;
	char		Id[11];
	int			UserNumber;
	int			DBNumber;
};

//-----------------------------------------------------------------------------------------------------------

struct SDHP_COMMAND_BLOCK
{
	PBMSG_HEAD	h;				// C1:04
	short		Number;
	char		Id[11];
	int			UserNumber;
	int			DBNumber;
	UCHAR		BlockCode;
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_USERCLOSE_ID
{
	PBMSG_HEAD	h;				// C1:05
	char		szId[10];
	char		szName[10];
	USHORT		Level;
	UCHAR		DbClass;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_REQ_MAPSVRMOVE
{
	PBMSG_HEAD	h;				// C1:7A
	int			iIndex;
	char		szAccountID[11];
	char		szCharName[11];
	USHORT		wCurMapSvrCode;
	USHORT		wDstMapSvrCode;
	USHORT		wMapNumber;
	UCHAR		btX;
	UCHAR		btY;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_ANS_MAPSVRMOVE
{
	PBMSG_HEAD	h;				// C1:7A
	int			iIndex;
	char		szAccountID[11];
	char		szCharName[11];
	USHORT		wCurMapSvrCode;
	USHORT		wDstMapSvrCode;
	USHORT		wMapNumber;
	UCHAR		btX;
	UCHAR		btY;
	int			iResult;
	int			iJoinAuthCode1;
	int			iJoinAuthCode2;
	int			iJoinAuthCode3;
	int			iJoinAuthCode4;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_REQ_MAPSVRAUTH
{
	PBMSG_HEAD	h;				// C1:7B
	int			iIndex;
	char		szAccountID[11];
	char		szCharName[11];
	USHORT		wDstMapSvrCode;
	int			iJoinAuthCode1;
	int			iJoinAuthCode2;
	int			iJoinAuthCode3;
	int			iJoinAuthCode4;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_ANS_MAPSVRAUTH
{
	PBMSG_HEAD	h;				// C1:7B
	int			iIndex;
	char		szAccountID[11];
	char		szCharName[11];
	WORD		wPrevMapSvrCode;
	WORD		wMapNumber;
	BYTE		btX;
	BYTE		btY;
	int			iResult;
	int			iUserNumber;
	int			iDBNumber;
	char		cJoominNumber[13];
	UCHAR 		btBlockCode;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_NOTIFY_MAXUSER

{
	PBMSG_HEAD	h;				// C1:7C
	int			iSvrCode;
	int			iMaxUserCount;
	int			iCurUserCount;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_USERNOTICE			// Can't found in GS.
{
	PBMSG_HEAD	h;
	int			ClientIndex;
	char		Notice[60];
	char		AccountID[10];
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_SETENTIRENOTICE		// Can't found in GS.
{
	PBMSG_HEAD	h;
	int			ClientIndex;
	char		Notice[60];
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_USERNOTICE_RESULT	// Can't found in GS.
{
	PBMSG_HEAD	h;
	int			ClientIndex;
	BYTE		Result;
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_NOTICE
{
	PBMSG_HEAD	h;				// C1:62
	char		Notice[61];
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_USER_NOTICE
{
	SDHP_USER_NOTICE(const char* szAccountID, const char* szNotice)
	{
		h.set(0x41, sizeof(*this));//0x41, no 63
		// ----
		memcpy(this->szId, szAccountID, 10);
		memcpy(this->Notice, szNotice, 60);
	}
	PBMSG_HEAD	h;				// C1:63
	char		szId[10];
	char		Notice[61];
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_USER_DISCONNECT
{
	PBMSG_HEAD h;
	char	AccountID[10];
	short	aIndex;
};
//-----------------------------------------------------------------------------------------------------------
#endif /* JOINPROTOCOL_H */
//-----------------------------------------------------------------------------------------------------------