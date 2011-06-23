#include "StdAfx.h"
#include "CManager.h"
#include "Protocol.h"
#include "Messages.h"
#include "GameMain.h"
#include "LogProc.h"
#include "EDSprotocol.h"
#include "..\include\readscript.h"

//---------------------------------------------------------------------------
// ## Made By Leo123;

cMang g_Manger;
//---------------------------------------------------------------------------

char szTemp[1024] = {0};
char MangCharSet[18] = {24 , 13 , 14 , 51 , 51 , 63 , 31 , -1 , -1 , 3 , -2 , 0 , -96 , -63 , 17 , 17 , 8 , 0 };
//---------------------------------------------------------------------------

cMang::cMang()
{
	// -----
}
//---------------------------------------------------------------------------

void cMang::Init()
{
	m_DataOnConnectCount	= 0;
	m_DataOnLevelUpCount	= 0;
	m_DataOnMapMoveCount	= 0;
	// -----
	memset(m_DataOnConnect, 0x00 , sizeof(m_DataOnConnect));
	memset(m_DataOnLevelUP, 0x00 , sizeof(m_DataOnLevelUP));
	memset(m_DataOnMapMove, 0x00 , sizeof(m_DataOnMapMove));
}
//---------------------------------------------------------------------------

bool cMang::LoadFile(char * szFileName)
{
	Init();
	// -----
	bool bReturn		= true;
	int	Token			= 0;
	int Type			= 0;
	int Level			= 0;
	int Map				= 0;
	int X				= 0;
	int Y				= 0;
	int	IsMove			= 0;
	int BonusPoints		= 0;
	// -----
	char Msg[MAX_MSG_LEN];
	memset(Msg, 0x00, sizeof(Msg));
	// -----
	SMDFile = fopen(szFileName, "r");
	// -----
	if ( SMDFile == NULL )
	{
		MsgBox("[cMang] Info file Load Fail [%s]", szFileName);
		bReturn = false;;
	}
	else
	{
		while (true)
		{
			Token = GetToken();
			// -----
			if ( Token == 2 )
			{
				break;
			}
			// -----
			Type = TokenNumber;
			// -----
			while(true)
			{
				if(Type == DATA_TYPE_ONCONNECT)
				{
					Token 		= GetToken();
					// -----
					if ( strcmp("end", TokenString) == 0 )
					{
						break;
					}
					// -----
					Level		= TokenNumber;
					// -----	
					Token		= GetToken();
					IsMove		= TokenNumber;
					// -----	
					Token		= GetToken();
					Map			= TokenNumber;
					// -----
					Token		= GetToken();
					X			= TokenNumber;					
					// -----
					Token		= GetToken();
					Y			= TokenNumber;			
					// -----
					Token		= GetToken();
					memcpy(Msg, TokenString, sizeof(Msg));
					// -----
					InsertData(Level, IsMove, Map, X, Y, Msg);
				}
				else if(Type == DATA_TYPE_ONLEVELUP)
				{
					Token 		= GetToken();
					// -----
					if ( strcmp("end", TokenString) == 0 )
					{
						break;
					}
					// -----
					Level		= TokenNumber;
					// -----	
					Token		= GetToken();
					IsMove		= TokenNumber;
					// -----	
					Token		= GetToken();
					Map			= TokenNumber;
					// -----	
					Token		= GetToken();
					X			= TokenNumber;
					// -----	
					Token		= GetToken();
					Y			= TokenNumber;	
					// -----	
					Token		= GetToken();
					BonusPoints	= TokenNumber;	
					// -----						
					Token		= GetToken();
					memcpy(Msg, TokenString, sizeof(Msg));
					// -----		
					InsertData(Level, IsMove, Map, X, Y, BonusPoints, Msg);
				}
				else if(Type == DATA_TYPE_ONMAPMOVE)
				{
					Token 		= GetToken();
					// -----
					if ( strcmp("end", TokenString) == 0 )
					{
						break;
					}
					// -----
					Map			= TokenNumber;
					// -----
					Token		= GetToken();
					memcpy(Msg, TokenString, sizeof(Msg));
					// -----
					InsertData(Map, Msg);
				}
			}
		}
		fclose(SMDFile);
		// -----
		LogAdd("%s file load!", szFileName);
	}
	// -----
	return bReturn;
}
//---------------------------------------------------------------------------

bool cMang::InsertData(int IsMove, int Level, int Map, int X, int Y, char * Msg)
{
	/* On Connect */
	if(m_DataOnConnectCount == MAX_DATA)
	{
		return false;
	}
	// -----
	_DATA_ONCONNECT * lpData = &m_DataOnConnect[m_DataOnConnectCount];
	// -----
	lpData->m_IsData		= true;
	lpData->m_IsMove		= IsMove;
	lpData->m_Level			= Level;
	lpData->m_Map			= Map;
	lpData->m_X				= X;
	lpData->m_Y				= Y;
	// -----
	memcpy(lpData->m_Msg, Msg , sizeof(lpData->m_Msg));
	// -----
	m_DataOnConnectCount++;
	// -----
	return true;
}
//---------------------------------------------------------------------------

bool cMang::InsertData(int Level, int IsMove, int Map, int X, int Y, int BonusPoints, char * Msg)
{
	/* On Level Up */
	if(m_DataOnLevelUpCount == MAX_DATA)
	{
		return false;
	}
	// -----
	_DATA_ONLEVELUP * lpData = &m_DataOnLevelUP[m_DataOnLevelUpCount];
	// -----
	lpData->m_IsData		= true;
	lpData->m_Level			= Level;
	lpData->m_IsMove		= IsMove;
	lpData->m_Map			= Map;
	lpData->m_X				= X;
	lpData->m_Y				= Y;
	lpData->m_BonusPoints 	= BonusPoints;
	// -----
	memcpy(lpData->m_Msg, Msg , sizeof(lpData->m_Msg));
	// -----
	m_DataOnLevelUpCount++;
	// -----
	return true;
}
//---------------------------------------------------------------------------

bool cMang::InsertData(int MapID, char * Msg)
{
	/* On Map Move */
	if(m_DataOnMapMoveCount == MAX_DATA)
	{
		return false;
	}
	// -----
	_DATA_ONMAPMOVE * lpData = &m_DataOnMapMove[m_DataOnMapMoveCount];
	// -----
	lpData->m_IsData		= true;
	lpData->m_MapID			= MapID;
	// -----
	memcpy(lpData->m_Msg, Msg , sizeof(lpData->m_Msg));
	// -----
	m_DataOnMapMoveCount++;
	// -----
	return true;
}
//---------------------------------------------------------------------------

bool cMang::OnConnect(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	// -----
	for(int i = 0 ; i != MAX_DATA ; i++)
	{
		_DATA_ONCONNECT * lpData = &m_DataOnConnect[i];
		// -----
		if(lpData == NULL)
		{
			break;
		}
		// -----
		if(lpData->m_IsData == false)
		{
			break;
		}
		// -----
		if(lpData->m_Level == 0) // for every level
		{
			g_Msg.Msg(aIndex, lpData->m_Msg);
			// -----
			if(lpData->m_IsMove > 0)
			{
				gObjTeleport(aIndex, lpData->m_Map, lpData->m_X, lpData->m_Y);
			}
		}
		// -----
		if(lpData->m_Level == lpObj->Level) // for currect level
		{
			MailAdd(aIndex, 224 , "[Mang System] Message");
			MsgSend(aIndex, 224 , lpData->m_Msg);
			//g_Msg.Msg(aIndex, lpData->m_Msg);
			// -----
			if(lpData->m_IsMove > 0)
			{
				gObjTeleport(aIndex, lpData->m_Map, lpData->m_X, lpData->m_Y);
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------

bool cMang::OnLevelUp(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	// -----
	for(int i = 0 ; i != MAX_DATA ; i++)
	{
		_DATA_ONLEVELUP * lpData = &m_DataOnLevelUP[i];
		// -----
		if(lpData == NULL)
		{
			break;
		}
		// -----
		if(lpData->m_IsData == false)
		{
			break;
		}
		// -----
		if(lpData->m_Level == lpObj->Level)
		{
			MailAdd(aIndex, 224 , "[Mang System] Message");
			MsgSend(aIndex, 224 , lpData->m_Msg);
			// -----
			if(lpData->m_IsMove > 0)
			{
				gObjTeleport(aIndex, lpData->m_Map, lpData->m_X, lpData->m_Y);
			}
			// -----
			lpObj->LevelUpPoint += lpData->m_BonusPoints;
		}
	}
	return true;
}
//---------------------------------------------------------------------------

bool cMang::OnMoveMap(int aIndex,int MapTarget)
{
	for(int i = 0 ; i != MAX_DATA ; i++)
	{
		_DATA_ONMAPMOVE * lpData = &m_DataOnMapMove[i];
		// -----
		if(lpData == NULL)
		{
			break;
		}
		// -----
		if(lpData->m_IsData == false)
		{
			break;
		}
		// -----
		if(lpData->m_MapID == MapTarget)
		{
			g_Msg.Msg(aIndex, lpData->m_Msg);
		}
	}
	return true;
}
//---------------------------------------------------------------------------

bool cMang::MailAdd(int aIndex,int MsgIndex,char* subject)
{
	PMSG_FRIEND_MEMO_LIST pMsg;
	pMsg.h.setE(0xC6, sizeof(pMsg));

	char date[31]="1";
	char name[10] = "<Angel>";

	memcpy(pMsg.Date, date, sizeof(pMsg.Date));
	memcpy(pMsg.Name, name, sizeof(pMsg.Name));
	memset(pMsg.Subject , 0x00 , 33 );
	strcpy(pMsg.Subject , subject );
	pMsg.Number = MsgIndex;	// seting the msg index ^^ (need make some thing)
	pMsg.read = 1;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));

	return true;
}
//---------------------------------------------------------------------------

bool cMang::ReadMsgReq(int aIndex,int MsgIndex )
{
	if(MsgIndex == 224)
	{
		// ANTI CRASH XD
		this->MsgSend(aIndex,  224 ,szTemp);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------

bool cMang::MsgSend(int aIndex ,int MsgIndex , char* Message , ...)
{
	char Format[1024];
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf(Format, Message, pArguments);
	va_end(pArguments);
	/* 
		This is function for pop up the mail
		It Using only if the msg in on the mail
	*/

	PMSG_FRIEND_READ_MEMO pMsg;

	int nsize = sizeof(pMsg)-1000;
	nsize +=  strlen(Format);

	pMsg.h.setE(0xC7, nsize);

	pMsg.MemoIndex = MsgIndex;
	pMsg.MemoSize = strlen(Format);
			
	pMsg.Dir = 3;
	pMsg.Action = 1;
	//memcpy(pMsg.Photo, lpMsg->Photo,sizeof(pMsg.Photo));
	memcpy(pMsg.Photo , MangCharSet , 18 );
	memcpy(pMsg.Memo, Format, strlen(Format));

	DataSend(aIndex, (LPBYTE)&pMsg, nsize);

	return true;
}
//---------------------------------------------------------------------------