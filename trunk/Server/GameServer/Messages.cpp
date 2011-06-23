#include "stdafx.h"
#include "Messages.h"
#include "giocp.h"
#include "User.h"
#include "Protocol.h"

LPOBJ lpObj;
Messages  g_Msg;

Messages::Messages(void)
{

}

Messages::~Messages(void)
{
}

void Messages::DataSendAll(unsigned char* Packet, int Len)
{
	for (int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
		if (gObjIsConnected(i))
			DataSend(i, Packet, Len);
}

void Messages::DataSendMap(int Map, unsigned char* Packet, int Len)
{
	for (int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
		if (gObjIsConnected(i))
		{
			lpObj = &gObj[i];
			if (lpObj->MapNumber == Map)
				DataSend(i, Packet, Len);
		}
}

void Messages::DataSendNear(int aIndex, unsigned char* Packet, int Len)
{
	lpObj = &gObj[aIndex];
	int MyMap = lpObj->MapNumber;
	int MyX = lpObj->X;
	int MyY = lpObj->Y;
	for (int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
		if (gObjIsConnected(i))
		{
			lpObj = &gObj[aIndex];
			int Map = lpObj->MapNumber;
			int X = lpObj->X;
			int Y = lpObj->Y;
			if (MyMap == Map)
				if (X < (MyX + 6) && X > (MyX - 6))
					if (Y < (MyY + 6) && Y > (MyY - 6))
						DataSend(i, Packet, Len);
		}
}

void Messages::Notice(int aIndex, const char* Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x0D;
	*(Packet+3) = 0x00;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	DataSend(aIndex, Packet, Len);
	free (Packet);
}

void Messages::NoticeMap(int Map, const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x0D;
	*(Packet+3) = 0x00;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	this->DataSendMap(Map, Packet, Len);
	free (Packet);
}

void Messages::NoticeAll(const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	AllSendServerMsg(Message);
}
void Messages::GreenNoticeAll(const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x0D;
	*(Packet+3) = 0x02;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	this->DataSendAll(Packet, Len);
	free (Packet);
}

void Messages::PM(int aIndex, const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x02;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	DataSend(aIndex, Packet, Len);
	free (Packet);
}

void Messages::PMAll(const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x02;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	this->DataSendAll(Packet, Len);
	free (Packet);
}

void Messages::PMMap(int Map, const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x02;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	this->DataSendMap(Map, Packet, Len);
	free (Packet);
}

void Messages::Msg(int aIndex, const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x00;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	DataSend(aIndex, Packet, Len);
	free (Packet);
}

void Messages::MsgAll(const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x00;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	this->DataSendAll(Packet, Len);
	free (Packet);
}

void Messages::MsgMap(int Map, const char *Format, ...)
{
	char Message[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Message, Format, pArguments);
	va_end(pArguments);
	BYTE *Packet;
	Packet = (BYTE*) malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet+2) = 0x00;
	memcpy((Packet+13), Message, strlen(Message));
	int Len = (strlen(Message) + 0x13);
	*(Packet+1) = Len;
	this->DataSendMap(Map, Packet, Len);
	free (Packet);
}