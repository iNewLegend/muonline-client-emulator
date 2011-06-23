/////////////////////////////////////
// MuMsg.cpp
// Decompiled by : Deathway
// Date : 2007-05-12
// Function : Decrypt the message.wtf
// Status : Fully Functional
/////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "MuMsg.h"

MUMSG_API int nMsg=0;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" MUMSG_API int fnMsg(void)
{
	return 42;
}

CMsg::CMsg() // Correct
{
	memset(&this->szDefaultMsg  , 0, sizeof(this->szDefaultMsg ) );
	this->Msghead = NULL;
	strcpy(this->szDefaultMsg , "Msg error");
}

CMsg::~CMsg() // Correct
{
	this->lMsgFree();
}

void  CMsg::XorBuffer(char* buff, int len) // Correct
{
	if (len <=0)
	{
		return;
	}

	for(int iCounter=0;iCounter<len;iCounter++)
	{
		buff[iCounter] = buff[iCounter]^0xCA;	// Buffer
	}
}

void CMsg::DataFileLoadVer01(FILE* filename)	// SubIndex;Index;Len(WORD);LPSTR
{
	unsigned char szTemp[256];
	unsigned short uIndex;
	unsigned short uLen;

	int Max=this->LoadHeader.count;

	if (Max <= 0)
	{
		MessageBox(NULL, "�д��� �������ǰ��� ���� �Դϴ�."	, "Error", MB_OK|MB_APPLMODAL); // Modification error data while reading
		return;
	}
	else
	{
		while ( Max != 0 )
		{
			memset(&szTemp, 0, sizeof(szTemp) );

			fread(&uIndex , 2, 1, filename);
			fread(&uLen , 2, 1, filename);
			fread(&szTemp[0] , uLen , 1, filename);
			szTemp[uLen]=0;

			this->XorBuffer((char*)szTemp , uLen );
			this->lMsgListAdd( uIndex, (unsigned char*) szTemp);
			Max--;
		} 
	}
}

void CMsg::LoadWTF(const char* filename)
{
	FILE* WTFFile;
	char szBufferError[128];
	
	WTFFile =fopen(filename, "rb");
	
	if (WTFFile == 0 )
	{
		wsprintf(szBufferError, "Could not open %s.", filename);	// %s file does not exist
		MessageBox(NULL, szBufferError, "Error", MB_OK|MB_APPLMODAL);
		return;
	}
	else
	{
		if (this->lMsgListInit() != 0)
		{
			fread(&this->LoadHeader, 28, 1, WTFFile);

			if (this->LoadHeader.headcode != 0xCC)
			{
				MessageBox(NULL, "TextCode Type Wrong.", "Error", MB_OK|MB_APPLMODAL);
			}
			else if ( (this->LoadHeader.version -1) != 0)
			{
				MessageBox(NULL, "�������� �ʴ� ���� ������ �Դϴ�.", "Error", MB_OK|MB_APPLMODAL);
			}
			else
			{
				DataFileLoadVer01(WTFFile);
			}
			fclose(WTFFile);
		}
	}
}

int CMsg::lMsgListInit()
{
	LPMSG_STRUCT msg;

	if ( (msg =new MSG_STRUCT) == 0 )
	{
		MessageBox(NULL, "Memory Allocation Error (MuMsg)", "Error", MB_OK);
		return 0;
	}
	else
	{
		msg->next  =NULL;
		msg->number  =0;
		msg->msg = NULL;
		memset(this->MsgIndex, 0, sizeof(this->MsgIndex));
		return 1;
	}
	return 1;
}

void CMsg::lMsgFree()
{
	for ( int n = 0; n < 32768; n++ )
	{
		if ( this->MsgIndex[n] != 0 )
		{
			delete this->MsgIndex[n]->msg ;
			delete this->MsgIndex[n];
		}
	}
}

LPMSG_STRUCT  CMsg::lMsgListNew()
{
	return new MSG_STRUCT;
}

int CMsg::lMsgListAdd(int index, UCHAR* smsg)
{
	int MsgLen=strlen((char*)smsg);
	char* pPointer;
	
	if ( MsgLen > 0 )
	{
		this->MsgIndex[index]=this->lMsgListNew();
	
		if (this->MsgIndex[index] == 0)
		{
			MessageBox(NULL, "Memory Allocation Error #2(MuMsg)", "Error", MB_OK); // Memory Allocation Error
			return 0;
		}

		pPointer = new char[MsgLen+1];
		
		if ( pPointer  != 0 )
		{
			this->MsgIndex[index]->number=index;

			if ( index >= 0 || index < 32767 )
			{
				this->MsgIndex[index]->msg = (unsigned char*)pPointer;
				strcpy((char*)this->MsgIndex[index]->msg , (char*)smsg);
				return 1;
			}
			else
			{
				MessageBox(NULL, "Message Index Table Make Error", "Error", MB_OK);
				return 0;
			}
		}
	}
	return 1;
}

void CMsg::lMsgListPrint()
{
	for ( int n = 0; n<32768 ; n++)
	{
		if ( this->MsgIndex[n] != 0 )
		{
			MessageBox(NULL, (char*)this->MsgIndex[n]->msg, "Message", MB_OK);
		}
	}
}

LPSTR CMsg::Get(int index)
{
	if ( index >= 0 && index < 32768 )
	{
		if ( this->MsgIndex[index] == 0 )
		{
			return this->szDefaultMsg;
		}
			
		if ( this->MsgIndex[index]->msg == 0 )
		{
			return this->szDefaultMsg;
		}

		if ( *this->MsgIndex[index]->msg == 0 )
		{
			return this->szDefaultMsg;
		}

		return (char*)this->MsgIndex[index]->msg;
	}
	return (char*)this->szDefaultMsg;
}