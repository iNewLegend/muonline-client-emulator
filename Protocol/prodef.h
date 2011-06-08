//-----------------------------------------------------------------------------------------------------------
#ifndef PRODEF_H
#define PRODEF_H
//-----------------------------------------------------------------------------------------------------------

#ifndef MAX_ACCOUNT_LEN
	#define MAX_ACCOUNT_LEN		10
	#define MAX_GUILD_LEN		8
#endif
//-----------------------------------------------------------------------------------------------------------

#define JS_ONLY_VIP_MEMBERS		0x02
#define JS_BAD_CLIENT_VERSION	0x06
#define MAX_CHAT_LEN			60
//-----------------------------------------------------------------------------------------------------------

#define SET_NUMBERH(x)		( (unsigned char)((unsigned long)(x)>>(unsigned long)8) )
#define SET_NUMBERL(x)		( (unsigned char)((unsigned long)(x) & 0xFF) )
#define SET_NUMBERHW(x)		( (unsigned short)((unsigned long)(x)>>(unsigned long)16) )
#define SET_NUMBERLW(x)		( (unsigned short)((unsigned long)(x) & 0xFFFF) )
#define MAKE_NUMBERW(x,y)	( (unsigned short)(((unsigned char)((y)&0xFF)) |   ((unsigned char)((x)&0xFF)<<8 ))  )
#define MAKE_NUMBERDW(x,y)	( (unsigned long)(((unsigned short)((y)&0xFFFF)) | ((unsigned short)((x)&0xFFFF)<<16))  )
//-----------------------------------------------------------------------------------------------------------

#ifndef _QWORD_DEFINED 
	#define _QWORD_DEFINED 
	typedef __int64 QWORD, *LPQWORD; 
#endif
//-----------------------------------------------------------------------------------------------------------

#define MAKEQWORD(a, b)		((QWORD)( ((QWORD) ((unsigned long) (A))) << 32 | ((unsigned long) (B)))) 
#define LODWORD(L)			((unsigned long)(L)) 
#define HIDWORD(L)			((unsigned long)(((QWORD)(L) >> 32)))
//-----------------------------------------------------------------------------------------------------------

#define MACRO1(value)		(( value & 0xFF) & 0xFF)
#define MACRO2(value)		(( value & 0xFFFF ) & 0xFFFF)
//-----------------------------------------------------------------------------------------------------------

#define MONTH_MACRO(value)	( (( (value)   )<0)?FALSE:(( (value)  )>12)?FALSE:TRUE  )
#define DAY_MACRO(value)	( (( (value - 1)   )<0)?FALSE:(( (value - 1)  )>30)?FALSE:TRUE  )
#define HOUR_MACRO(value)	( (( (value)   )<0)?FALSE:(( (value)  )>23)?FALSE:TRUE  )
#define MINUTE_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>59)?FALSE:TRUE  )
//-----------------------------------------------------------------------------------------------------------

typedef struct _PHEADB
{
	unsigned char uPacketType;
	unsigned char uSize;
	unsigned char uHead;

} PHEADB, PHEADBE, *LPHEADB, *LPHEADBE; 
//-----------------------------------------------------------------------------------------------------------

typedef struct _PHEADSUBB
{
	unsigned char uPacketType;
	unsigned char uSize;
	unsigned char uHead;
	unsigned char uSubHead;

} PHEADSUBB, * LPPHEADSUBB;
//-----------------------------------------------------------------------------------------------------------

typedef struct _PHEADW
{
	unsigned char uPacketType;
	unsigned char uSize[2];
	unsigned char uHead;

} PHEADW, * LPPHEADW;
//-----------------------------------------------------------------------------------------------------------

typedef struct _PHEADSUBW
{
	unsigned char uPacketType;
	unsigned char uSize[2];
	unsigned char uHead;
	unsigned char uSubHead;

} PHEADSUBW, *LPPHEADSUBW;
//-----------------------------------------------------------------------------------------------------------

struct PBMSG_HEAD
{
public:
	void set (unsigned char head, unsigned char size)
	{
		this->c			= 0xC1;
		this->size		= size;
		this->headcode	= head;
	};
	// ----
	void setE (unsigned char head, unsigned char size)
	{
		this->c			= 0xC3;
		this->size		= size;
		this->headcode	= head;
	};
	// ----
	unsigned char c;
	unsigned char size;
	unsigned char headcode;
};
//-----------------------------------------------------------------------------------------------------------

struct PWMSG_HEAD
{
public:
	void set(unsigned char head, int size)
	{
		this->c			= 0xC2;
		this->sizeH		= SET_NUMBERH(size);
		this->sizeL		= SET_NUMBERL(size);
		this->headcode	= head;
	};
	// ----
	void setE(unsigned char head, int size)
	{
		this->c			= 0xC4;
		this->sizeH		= SET_NUMBERH(size);
		this->sizeL		= SET_NUMBERL(size);
		this->headcode	= head;
	};
	// ----
	unsigned char c;
	unsigned char sizeH;
	unsigned char sizeL;
	unsigned char headcode;
};
//-----------------------------------------------------------------------------------------------------------

struct PBMSG_HEAD2
{
public:
	void set(unsigned char head, unsigned char sub, unsigned char size)
	{
		this->c			= 0xC1;
		this->size		= size;
		this->headcode	= head;
		this->subcode	= sub;
	};
	// ----
	unsigned char c;
	unsigned char size;
	unsigned char headcode;
	unsigned char subcode;
};
//-----------------------------------------------------------------------------------------------------------

struct PWMSG_HEAD2
{
public:
	void set(unsigned char head, unsigned char sub, int size)
	{
		this->c			= 0xC2;
		this->sizeH		= SET_NUMBERH(size);
		this->sizeL		= SET_NUMBERL(size);
		this->headcode	= head;
		this->subcode	= sub;
	}
	// ----
	unsigned char c;
	unsigned char sizeH;
	unsigned char sizeL;
	unsigned char headcode;
	unsigned char subcode;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_DEFAULT
{
	PBMSG_HEAD h;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_DEFAULT2
{
	PBMSG_HEAD h;
	unsigned char subcode;
};
//-----------------------------------------------------------------------------------------------------------

struct PMSG_DEFRESULT
{
	PBMSG_HEAD h;
	unsigned char result;	// 3
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_SERVERINFO
{
	struct PBMSG_HEAD h;
	unsigned char Type;
	unsigned short Port;
	char ServerName[50];
	unsigned short ServerCode;
};
//-----------------------------------------------------------------------------------------------------------

struct SDHP_RESULT
{
	struct PBMSG_HEAD h;
	unsigned char Result;
	unsigned long ItemCount;
};
//-----------------------------------------------------------------------------------------------------------
#endif /* PRODEF_H */
//-----------------------------------------------------------------------------------------------------------