#include "stdafx.h"
#include "GMMng.h"
#include "LogToFile.h"
#include "logproc.h"
#include "GameMain.h"
#include "protocol.h"
#include "..\common\winutil.h"
#include "MoveCommand.h"
#include "GuildClass.h"
#include "TNotice.h"
#include "BattleSoccerManager.h"
#include "giocp.h"
#include "Kanturu.h"
#include "SkillAdditionInfo.h"
#include "Messages.h"
#include "DSProtocol.h"

Messages * MMSG = new Messages();
 
// GS-N 0.99.60T 0x004F0110
//	GS-N	1.00.18	JPN	0x00519F60	-	Completed

CLogToFile KUNDUN_GM_LOG( "KUNDUN_EVENT_GM_LOG", ".\\KUNDUN_EVENT_GM_LOG", 1);
CGMMng cManager;
char szuTemp[200];
char Message[100];

void GetLongParam(char* text, char* buff)
{
	char msg[255];
	strcpy(msg, text);
	LPCSTR Content;

	int sLen = strlen(msg);
	for(int i=0;i<sLen;i++)
	{
		if((char)msg[i]==0x20)
		{
			(char)msg[i] = 0x00;
			Content = msg+i+1;
			break;
		}
	}
	strcpy(buff, Content);
}

char GetParam(char id, char* txt, char* buff)
{
	char msg[255];
	strcpy( msg , txt );

	char cnt = 0;

	char * pch;
	pch = strtok ( msg , " " );
	while (pch != NULL)
	{
		if ( cnt == id )
		{
			strcpy ( buff , pch );
			return id;
		}

		pch = strtok ( NULL , " " );
		cnt ++;
	}

	return 0;
}

CGMMng::CGMMng()
{
	memset(szuTemp,0x00,200);
	this->ManagerInit();
}

CGMMng::~CGMMng()
{
	return;
}

void CGMMng::Init()
{
	this->cCommand.Init();

	this->cCommand.Add(lMsg.Get(MSGGET(11, 184)), 100);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 185)), 101);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 186)), 102);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 187)), 103);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 188)), 105);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 189)), 104);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 190)), 106);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 191)), 108);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 192)), 112);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 193)), 111);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 194)), 109);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 195)), 110);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 197)), 201);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 198)), 200);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 199)), 202);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 200)), 203);


	this->cCommand.Add(lMsg.Get(MSGGET(11, 204)), 100);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 205)), 101);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 206)), 102);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 207)), 103);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 208)), 105);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 209)), 104);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 210)), 106);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 211)), 108);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 212)), 112);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 213)), 111);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 214)), 109);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 215)), 110);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 217)), 201);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 218)), 200);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 219)), 202);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 220)), 203);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 201)), 214);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 202)), 215);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 203)), 216);

	this->cCommand.Add(lMsg.Get(MSGGET(11, 221)), 214);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 222)), 215);
	this->cCommand.Add(lMsg.Get(MSGGET(11, 223)), 216);

	// KUNDUN Commands
	
	this->cCommand.Add("/ÄïµÐ»óÅÂ", 320);	// #Ùò¶Ù×´Ì¬ 
	this->cCommand.Add("/ÄïµÐÇÇ", 321);	// #Ùò¶ÙÌåÓý 
	this->cCommand.Add("/ÄïµÐÈ¸º¹·®", 322);	// #Ùò¶ÙÊÕ»Ø½ð¶î 
	this->cCommand.Add("/ÄïµÐÃÊ´çÈ¸º¹·®", 323);	// #Ùò¶Ù»ØÊÕÁ¿Ã¿ 
	this->cCommand.Add("/ÄïµÐÈ¸º¹½Ã°£", 324);	// #Ùò¶ÙµÄ»Ö¸´Ê±¼ä

	#if(GS_CASTLE == 1)
	this->cCommand.Add("/cschangeowner", 331);
	this->cCommand.Add("/cssetregsiege", 332);
	this->cCommand.Add("/cssetregmark", 333);
	this->cCommand.Add("/cssetnotify", 334);
	this->cCommand.Add("/cssetstart", 335);
	this->cCommand.Add("/cssetend", 336);
	this->cCommand.Add("/cscurowner", 337);
	this->cCommand.Add("/cscurstate", 338);
	this->cCommand.Add("/cschangeside", 340);
	#endif

	// KANTURU Commands
	this->cCommand.Add("/KanturuReady", 369);
	this->cCommand.Add("/KanturuBattleMaya", 370);
	this->cCommand.Add("/¸¶¾ß¸ó½ºÅÍ1", 371);	// #translation 
	this->cCommand.Add("/¸¶¾ß1", 372);	// #translation 
	this->cCommand.Add("/¸¶¾ß¸ó½ºÅÍ2", 373);	// #translation 
	this->cCommand.Add("/¸¶¾ß2", 374);	// #translation 
	this->cCommand.Add("/¸¶¾ß¸ó½ºÅÍ3", 375);	// #translation 
	this->cCommand.Add("/¸¶¾ß3", 376);	// #translation 
	this->cCommand.Add("/¸¶¾ß3¼º°ø", 377);	// #translation 
	this->cCommand.Add(lMsg.Get(MSGGET(13, 60)), 378);	// "/»÷°ÜÂêÑÅÉúÃüÌå"
	this->cCommand.Add("/¸¶¾ß½ÇÆÐ", 379);	// #translation 
	this->cCommand.Add("/KanturuBattleNightmare", 380);
	this->cCommand.Add(lMsg.Get(MSGGET(13, 61)), 381);	// "/»÷°ÜÖäÔ¹Ä§Íõ"
	this->cCommand.Add("/³ªÀÌÆ®¸Þ¾î½ÇÆÐ", 382);	// #translation 
	this->cCommand.Add("/KanturuTower", 383);
	this->cCommand.Add("/KanturuTowerOpen", 384);
	this->cCommand.Add("/KanturuTowerClose", 385);
	this->cCommand.Add("/KanturuEnd", 386);

	this->cCommand.Add(lMsg.Get(MSGGET(13, 57)), 387);
	this->cCommand.Add(lMsg.Get(MSGGET(13, 58)), 388);
	this->cCommand.Add(lMsg.Get(MSGGET(13, 59)), 389);

	this->cCommand.Add("/FireWorks", 217);	// Fire Cracker Effect	// #translation 

	this->cCommand.Add("/Post", 500 ) ;
	this->cCommand.Add("/online", 501 ) ;
	this->cCommand.Add("/god", 502 ) ;
	this->cCommand.Add("/sadasdasdasdasdasd", 218 ) ;
	this->cCommand.Add("/makeset",503);
	this->WatchTargetIndex = -1;
}


void CGMMng::ManagerInit()
{
	for ( int n = 0;n<MAX_GAME_MASTER ; n++ )
	{
		this->ManagerIndex[n] = -1;
	}
}


int CGMMng::ManagerAdd(char* name, int aIndex)
{
	for ( int n=0 ; n<MAX_GAME_MASTER ; n++ )
	{
		if ( this->ManagerIndex[n] == -1 )
		{
			strcpy(this->szManagerName[n], name);
			this->ManagerIndex[n] = aIndex;
			return n;
		}
	}

	return -1;
}


void CGMMng::ManagerDel(char* name)
{
	for ( int n=0;n<MAX_GAME_MASTER;n++)
	{
		if ( this->ManagerIndex[n] != -1 )
		{
			if ( strcmp(this->szManagerName[n], name) == 0 )
			{
				this->ManagerIndex[n] = -1;
				break;
			}
		}
	}
}


void CGMMng::ManagerSendData(LPSTR szMsg, int size)
{
	for ( int n=0;n<MAX_GAME_MASTER;n++ )
	{
		if ( this->ManagerIndex[n] != -1 )
		{
			GCServerMsgStringSend(szMsg, this->ManagerIndex[n], 0);
		}
	}
}

void CGMMng::DataSend(unsigned char* szMsg, int size)
{
	for ( int n=0;n<MAX_GAME_MASTER;n++ )
	{
		if ( this->ManagerIndex[n] != -1 )
		{
			::DataSend(this->ManagerIndex[n], szMsg, size);
		}
	}
}


void CGMMng::BattleInfoSend(char* Name1, BYTE score1, char* Name2, BYTE score2)
{
	for ( int n=0;n<MAX_GAME_MASTER;n++ )
	{
		if ( this->ManagerIndex[n] != -1 )
		{
			GCGoalSend(this->ManagerIndex[n], Name1, score1, Name2, score2);
		}
	}
}


int CGMMng::GetCmd(char* szCmd)
{
	for ( int n	=0; n< MAX_GM_COMMAND ; n++ )
	{
		if ( this->cCommand.nCmdCode[n] > 0 )
		{
			if ( stricmp(szCmd, this->cCommand.szCmd[n]) == 0 )
			{
				return this->cCommand.nCmdCode[n];
			}
		}
	}

	return 0;
}



char* CGMMng::GetTokenString()
{
	return strtok(0, " ");
}


int CGMMng::GetTokenNumber()
{
	char * szToken = strtok(0, " ");

	if ( szToken != NULL )
	{
		return atoi(szToken);
	}

	return 0;
}




int CGMMng::ManagementProc(LPOBJ lpObj, char* szCmd, int aIndex)
{
	char * szCmdToken;
	char string[256];
	char szId[20];
	char * pId = szId;
	int len = strlen(szCmd);
	int command_number;

	if ( len < 1 || len > 250 )
	{
		return 0;
	}

	memset(szId, 0, sizeof(szId));
	strcpy(string, szCmd);
	szCmdToken = strtok(string, " ");
	command_number = this->GetCmd(szCmdToken);

	strcpy(Message, szCmd);

	if ( stricmp(szCmdToken, "/add0") == 0 )
	{
		int nAddStrength = 100;
		LevelUpPointAdd(0,nAddStrength,aIndex);
		GCSendEffectInfo(aIndex, 17);
	}
	if ( stricmp(szCmdToken, "/add1") == 0 )
	{
		int nAddStrength = 100;
		LevelUpPointAdd(1,nAddStrength,aIndex);
		GCSendEffectInfo(aIndex, 3);
	}
	if ( stricmp(szCmdToken, "/add2") == 0 )
	{
		int nAddStrength = 100;
		LevelUpPointAdd(2,nAddStrength,aIndex);
		GCSendEffectInfo(aIndex, 17);
	}
	if ( stricmp(szCmdToken, "/add3") == 0 )
	{
		int nAddStrength = 100;
		LevelUpPointAdd(3,nAddStrength,aIndex);
		GCSendEffectInfo(aIndex, 17);
	}
	switch ( command_number )
	{
		#if(GS_CASTLE == 1)
		case 331:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 0, pId);
		}
		break;
		case 332:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 1, pId);
		}
		break;
		case 333:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 2, pId);
		}
		break;
		case 334:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 3, pId);
		}
		break;
		case 335:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 4, pId);
		}
		break;
		case 336:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 5, pId);
		}
		break;
		case 337:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 6, pId);
		}
		break;
		case 338:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 7, pId);
		}
		break;
		case 340:
		{
			pId = this->GetTokenString();
			g_CastleSiege.OperateGmCommand(lpObj->m_Index, 8, pId);
		}
		break;
#endif
		case 502:
			{
				MMSG->Msg(lpObj->m_Index,"@@God of MuGlobal is Wolf");
				break;
			}
		case 501:
			{
				int OnlineCount = 0;
				for (int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
				{
					if(gObjIsConnected(n))
						OnlineCount++;
				}
				MMSG->Msg(lpObj->m_Index,"[%d] Online Players",OnlineCount);
				break;
			}
		case 500:
			{
				char* szuText = this->GetTokenString();
				if(szuText != NULL)
				{
					int uTick = GetTickCount();
					//if(lpObj->Money < 299999);
					//{
					//	MMSG->Msg(lpObj->m_Index,"@[POST] Eww ti co - bisplatnij sir toka v mi6elovki");
					//	return 0;
					//}
					//{
					//	MMSG->Msg(lpObj->m_Index,"@[POST] Eww ti co - level netu :(");
					//	return 0;
					//}
					if(lpObj->Money < 500000)
					{
						MMSG->Msg(lpObj->m_Index,"@[POST] You need 500k Zen to POST");
						return 0;
					}
					if(lpObj->Level < 50)
					{
						MMSG->Msg(lpObj->m_Index,"@[POST] You need 50 Level to POST");
						return 0;
					}
					else
					{
						char *Content = (char*) Message;
						GetLongParam(Message, Content);
						lpObj->Money -= 500000;
						GCMoneySend(lpObj->m_Index , lpObj->Money);
						wsprintf(szuTemp,"[POST] %s: %s",lpObj->Name,Message);
						MMSG->PMAll(szuTemp);
						return 0;
					}
				}
				else
				{
					MMSG->Msg(lpObj->m_Index,"@[POST] The Correct Syntax is: /post message");
					return 0;
				}
				break;
			}
			case 503:	// MakeSet CMD
			{
								if ( (lpObj->Authority & 2) != 2 && (lpObj->Authority & 0x20) != 0x20 )
				{
					return 0;
				}
				if (!strcmpi(Message, "/MakeSet"))
				{
					MMSG->Msg(aIndex, "Useage: /Make <Name> <Index> <Level> <Option> <Skill> <Luck> <Exc> <Ancient> <Amount> <Dur>");
					return 0;
				}
				//Name Index Level Option Skill Luck Exc Ancient Amount Dur
				char Name[100];
				memset(Name, 0, 100);
				int Index, Level, Option, Luck, Skill, Exc, Ancient, Amount, Dur; 
				char ParamBuff[100];
				memset(ParamBuff, 0, 100);
				GetParam(1, Message, ParamBuff);
				strcpy(Name, ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(2, Message, ParamBuff)) strcpy(ParamBuff, "0");
				Index = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(3, Message, ParamBuff)) strcpy(ParamBuff, "13");
				Level = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(4, Message, ParamBuff)) strcpy(ParamBuff, "4");
				Option = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(5, Message, ParamBuff)) strcpy(ParamBuff, "1");
				Luck = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(6, Message, ParamBuff)) strcpy(ParamBuff, "1");
				Skill = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(7, Message, ParamBuff)) strcpy(ParamBuff, "127");
				Exc = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(8, Message, ParamBuff)) strcpy(ParamBuff, "0");
				Ancient = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(9, Message, ParamBuff)) strcpy(ParamBuff, "1");
				Amount = atoi(ParamBuff);
				memset(ParamBuff, 0, 100);
				if (!GetParam(10, Message, ParamBuff)) strcpy(ParamBuff, "255");
				Dur = atoi(ParamBuff);
				int tIndex = gObjGetIndexByName(Name);
				if (!tIndex) return 0;
				for (int i=0; i<Amount; i++)
				{
					for (int type=7; type<12; type++)
						ItemSerialCreateSend(tIndex, lpObj->MapNumber, lpObj->X, lpObj->Y, type*512 + Index, Level, Dur, Skill, Luck, Option, tIndex, Exc, Ancient);
				}
				break;
			}
		case 218:
			{
				AllSendServerMsg("[MU GLOBAL ADMINISTRATORS]");
				AllSendServerMsg("Sacred & Wolf");
				AllSendServerMsg("www.muglobal.ru");
				break;
			}
		case 217:	//116:
			{
				if ( (lpObj->Authority & 2) != 2 && (lpObj->Authority & 0x20) != 0x20 )
				{
					return 0;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
					lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, "FIRECRACK.");
				int x = lpObj->X;
				int y = lpObj->Y;
				PMSG_SERVERCMD pMsg;

				PHeadSubSetB((LPBYTE)&pMsg,0xF3,0x40, sizeof(pMsg));
				pMsg.CmdType = 0;

				for ( int i=0;i<15;i++)
				{
					pMsg.X = x+(rand() % 5)*2 - 4;
					pMsg.Y = y+(rand() % 5)*2 - 4;
					MsgSendV2(lpObj,(UCHAR*)&pMsg, sizeof(pMsg));
					::DataSend(lpObj->m_Index ,(UCHAR*)&pMsg, sizeof(pMsg));
				}
			}
			break;

		case 216:	//115:
			{
				if ( (lpObj->Authority & 2) != 2 && (lpObj->Authority & 0x20) != 0x20 )
				{
					return 0;
				}

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return 0;
				}

				LPOBJ lpTargetObj = gObjFind(pId);

				if ( lpTargetObj == NULL )
				{
					return 0;
				}

				LogAddTD("Use GM Command -> [ %s ]	[ %s ]	[ %s ] / Target : [%s][%s] : %s ",
					lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, lpTargetObj->AccountID,
					lpTargetObj->Name, "User Watching");

				char szTemp[256];

				if ( this->WatchTargetIndex == lpTargetObj->m_Index )
				{
					this->WatchTargetIndex = -1;
					
					wsprintf(szTemp, "%s : °¨½ÃÇØÁ¦", lpTargetObj->Name);	// #translation
					GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
				}
				else
				{
					wsprintf(szTemp, "%s : °¨½Ã½ÃÀÛ", lpTargetObj->Name);	// #translation
					GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
					this->WatchTargetIndex = lpTargetObj->m_Index;
				}
			}
			break;

		case 215:	//114:
			{
				if ( (lpObj->Authority & 2) != 2 && (lpObj->Authority & 0x20) != 0x20 )
				{
					return 0;
				}

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return 0;
				}

				int map;
				int iX;
				int iY;
				LPOBJ lpTargetObj = gObjFind(pId);
				int iIndex;

				if ( lpTargetObj == NULL )
				{
					return 0;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
					lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, lpTargetObj->AccountID,
					lpTargetObj->Name, "User Tracking");
				map = lpTargetObj->MapNumber;
				iX = lpTargetObj->X;
				iY = lpTargetObj->Y;
				iIndex = lpObj->m_Index;

				if ( iIndex >= 0 )
				{
					gObjTeleport(iIndex, map, iX, iY);
				}
			}
			break;

		case 214:	//113:
			{
				if ( (lpObj->Authority & 2) != 2 )
				{
					return 0;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
					lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, 
					"User Stat (connection)");

				int lc151 = 0;
				int lc152 = 400;
				int iTokenNumber = this->GetTokenNumber();

				if ( iTokenNumber > 0 )
				{
					lc151 = iTokenNumber;
				}

				int iTokenNumber2 = this->GetTokenNumber();

				if ( iTokenNumber2 > 0 )
				{
					lc152 = iTokenNumber2;
				}

				gObjSendUserStatistic(lpObj->m_Index, lc151, lc152);
			}
			break;

		case 100:	//100:
			{
				if ( (lpObj->AuthorityCode &4) != 4 )
				{
					return 0;
				}

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return 0;
				}

				int iTargetIndex = gObjGetIndex(pId);
				

				if ( iTargetIndex >= 0 )
				{
					LPOBJ lpTargetObj = gObjFind(pId);

					if ( lpTargetObj == NULL )
					{
						return 0;
					}

					LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
						lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, lpTargetObj->AccountID,
						lpTargetObj->Name, "User Disconnect");
					LogAdd(lMsg.Get(MSGGET(1, 191)), pId);
					CloseClient(iTargetIndex);
				}
			}
			break;

		case 112:	//108:
			{
				if ( (lpObj->AuthorityCode &4) != 4 )
				{
					return 0;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
					lpObj->Ip_addr, lpObj->AccountID, lpObj->Name,
					"Guild Disconnect");

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return 0;
				}

				_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(pId);
				int iIndex;

				if ( lpGuild != NULL )
				{
					for ( int i=0;i<MAX_USER_GUILD ; i++ )
					{
						if ( lpGuild->Index[i] >= 0 )
						{
							iIndex = lpGuild->Index[i];

							if ( iIndex >= 0 )
							{
								LogAdd(lMsg.Get(MSGGET(1, 191)), pId);
								CloseClient(iIndex);
							}
						}
					}
				}
			}
			break;

		case 101:	//101:
			{
				if ( (lpObj->AuthorityCode &8) == 8 )
				{
					pId = this->GetTokenString();

					if ( pId == NULL )
					{
						return 0;
					}

					int iTokenNumber1 = this->GetTokenNumber();
					int iTokenNumber2 = this->GetTokenNumber();
					int iTokenNumber3 = this->GetTokenNumber();
					int iIndex = gObjGetIndex(pId);
					LPOBJ lpTargetObj;

					if ( iIndex >= 0 )
					{
						lpTargetObj = gObjFind(pId);

						if ( lpTargetObj == NULL )
						{
							return 0;
						}

						LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
							lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, lpTargetObj->AccountID,
							lpTargetObj->Name, "User SetPosition");

						gObjTeleport(iIndex, iTokenNumber1, iTokenNumber2, iTokenNumber3);
					}
				}
				else
				{
					pId = this->GetTokenString();

					if ( pId != NULL )
					{
						int lc165 = -1;
						int lc166 = 0;
						int lc167 = 0;

						if ( lpObj->Teleport != 0 )
						{
							GCServerMsgStringSend(lMsg.Get(MSGGET(6, 68)), lpObj->m_Index, 1);
							return 0;
						}

						if ( (lpObj->m_IfState.use) != 0 )
						{
							if ( lpObj->m_IfState.type  == 3 )
							{
								lpObj->TargetShopNumber = -1;
								lpObj->m_IfState.type = 0;
								lpObj->m_IfState.use = 0;
							}
						}

						if ( lpObj->m_IfState.use > 0 )
						{
							GCServerMsgStringSend(lMsg.Get(MSGGET(6, 68)), lpObj->m_Index, 1);
							return 0;
						}

						if ( gObj[aIndex].IsInBattleGround != false )
						{
							GCServerMsgStringSend(lMsg.Get(MSGGET(6, 68)), lpObj->m_Index, 1);
							return 0;
						}

						if ( lpObj->m_PK_Level >= 6 )
						{
							GCServerMsgStringSend(lMsg.Get(MSGGET(4, 101)), lpObj->m_Index, 1);
							return 0;
						}

						gMoveCommand.Move(lpObj, pId);
					}
				}
			}
			break;

		case 108:	//104:
			{
				if ( (lpObj->AuthorityCode &8)!= 8 )
				{
					return 0;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
					lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, 
					"Guild SetPosition");

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return 0;
				}

				int iTokenNumber1 = this->GetTokenNumber();
				int iTokenNumber2 = this->GetTokenNumber();
				int iTokenNumber3 = this->GetTokenNumber();
				_GUILD_INFO_STRUCT* lpGuild = Guild.SearchGuild(pId);
				int iIndex;

				if ( lpGuild != NULL )
				{
					for ( int i=0;i<MAX_USER_GUILD;i++)
					{
						if (lpGuild->Index[i] >= 0 )
						{
							iIndex = lpGuild->Index[i];
							gObjTeleport(iIndex, iTokenNumber1, iTokenNumber2++, iTokenNumber3);
						}
					}
				}
			}
			break;

		case 109:	//105:
			{
				if ( (lpObj->Authority &2)== 2 )
				{
					LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
						lpObj->Ip_addr, lpObj->AccountID, lpObj->Name,
						"Start BattleSoccer");

					BattleSoccerGoalStart(0);
				}
			}
			break;

		case 110:	//106:
			{
				if ( (lpObj->Authority &2) == 2 )
				{
					LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
						lpObj->Ip_addr, lpObj->AccountID, lpObj->Name,
						"Stop BattleSoccer");

					BattleSoccerGoalEnd(0);
				}
				else
				{
					if ( gObj[aIndex].lpGuild != NULL )
					{
						if (gObj[aIndex].lpGuild->WarType == 1 )
						{
							strcmp(gObj[aIndex].Name, gObj[aIndex].lpGuild->Names[0] );
						}
					}
				}
			}

			break;

		case 111:	//107:
			{
				if ( (lpObj->Authority & 2) == 2 )
				{
					LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
						lpObj->Ip_addr, lpObj->AccountID, lpObj->Name, "End GuildWar");

					char * szGuild = this->GetTokenString();

					if ( szGuild != NULL )
					{
						GCManagerGuildWarEnd(szGuild);
					}
				}
				else
				{
					if ( gObj[aIndex].lpGuild != NULL && gObj[aIndex].lpGuild->lpTargetGuildNode != NULL)
					{
						if ( strcmp( gObj[aIndex].Name, gObj[aIndex].lpGuild->Names[0] ) ==  0)
						{
							if ( gObj[aIndex].lpGuild->BattleGroundIndex >= 0 && gObj[aIndex].lpGuild->WarType == 1 )
							{
								::gObjAddMsgSendDelay(&gObj[aIndex], 7, aIndex, 10000, 0);

								char szTemp[100];

								wsprintf(szTemp, lMsg.Get(MSGGET(4, 129)), gObj[aIndex].lpGuild->Names[0] );
								::GCServerMsgStringSendGuild(gObj[aIndex].lpGuild, szTemp, 1);
								::GCServerMsgStringSendGuild(gObj[aIndex].lpGuild->lpTargetGuildNode, szTemp, 1);
							}
						}
					}
				}
			}
			break;

		case 104:	//102:
			{
				if ( (lpObj->AuthorityCode&0x20 ) != 0x20 )
				{
					return FALSE;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "Ban Chatting");

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return FALSE;
				}

				int Index = ::gObjGetIndex(pId);

				if ( Index >= 0 )
				{
					gObj[Index].Penalty |= 2;
				}

			}
			break;

		case 106:	//103:
			{
				if ( (lpObj->AuthorityCode & 32 ) != 32 )
				{
					return FALSE;
				}

				LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "Free Ban-Chatting");

				pId = this->GetTokenString();

				if ( pId == NULL )
				{
					return FALSE;
				}

				int Index = ::gObjGetIndex(pId);

				if ( Index >= 0 )
				{
					gObj[Index].Penalty &= ~2;
				}
			}
			break;

		case 200:	//109:
			{
				pId = this->GetTokenString();

				if ( pId != NULL )
				{
					if ( strlen(pId) >= 1 )
					{
						::GCGuildWarRequestResult(pId, aIndex, 0);
					}
				}
			}

			break;

		case 202:	//111:
			{
				if ( (lpObj->Authority & 2 ) == 2 )
				{
					LogAddTD("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
						lpObj->Name, "Set GuildWar");
		
					pId = this->GetTokenString();

					if ( pId != NULL )
					{
						char * Rival = this->GetTokenString();

						if ( Rival != NULL )
						{
							if ( strlen(pId) >= 1 )
							{
								if ( strlen(Rival) >= 1 )
								{
									::GCManagerGuildWarSet(pId, Rival, 1);
								}
							}
						}
					}
				}
				else
				{
					if ( gEnableBattleSoccer != FALSE )
					{
						pId = this->GetTokenString();

						if ( pId != NULL )
						{
							if ( strlen(pId) >= 1 )
							{
								::GCGuildWarRequestResult(pId, aIndex, 1);
							}
						}
					}
				}
			}

			break;

		case 201:	//110:
			{
				gObjBillRequest(&gObj[aIndex]);
			}
			break;

		case 203:	//112:
			{
				pId = this->GetTokenString();

				if ( pId != NULL )
				{
					BOOL bState;

					if ( strcmp(pId, "on" ) == 0 )
					{
						bState = TRUE;
					}
					else if ( strcmp(pId, "off") == 0 )
					{
						bState = FALSE;
					}

					if ( bState >= FALSE && bState <= TRUE )
					{
						::gObjSetTradeOption(aIndex, bState);
						::gObjSetDuelOption(aIndex, bState);
					}
				}
			}
			break;

		case 320:	//117:
			{
				if ( (lpObj->Authority &2) != 2 )
				{
					return FALSE;
				}

				LogAddTD("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐ»óÅÂº¸±â");	// #translation Require Translation
				KUNDUN_GM_LOG.Output("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐ»óÅÂº¸±â");	// #translation  Require Translation

				for ( int n=0;n<MAX_VIEWPORT;n++)
				{
					if ( lpObj->VpPlayer[n].state != 0 )
					{
						if ( lpObj->VpPlayer[n].type == OBJ_MONSTER )
						{
							if ( lpObj->VpPlayer[n].number >= 0 )
							{
								LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].number];

								if ( lpTarget->Class == 275 )
								{
									TNotice pNotice(1);

									pNotice.SendToUser(lpObj->m_Index, "ÄïµÐ HP = %7.0f / %7.0f", lpTarget->Life, lpTarget->MaxLife); //Require Translation
									pNotice.SendToUser(lpObj->m_Index, "ÄïµÐ HP ÃÊ´çÈ¸º¹·® = %d È¸º¹·® = %d È¸º¹½Ã°£ = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);	// Require Translation
								}
							}
						}
					}
				}

			}
			break;

		case 321:	//118:
			{
				if ( (lpObj->Authority &2 ) != 2 )
				{
					return FALSE;
				}

				LogAddTD("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHP¼³Á¤");	// Require Translation
				KUNDUN_GM_LOG.Output("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHP¼³Á¤");	// Require Translation

				int iLife = this->GetTokenNumber();

				for ( int n=0;n<MAX_VIEWPORT;n++)
				{
					if ( lpObj->VpPlayer[n].state != FALSE )
					{
						if ( lpObj->VpPlayer[n].type == OBJ_MONSTER )
						{
							if ( lpObj->VpPlayer[n].number >= 0 )
							{
								LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].number];

								if ( lpTarget->Class == 275 )
								{
									if  ( iLife <= 5000 )
									{
										iLife = 5000 ;
									}

									if ( iLife > lpTarget->MaxLife )
									{
										iLife = lpTarget->MaxLife;
									}

									lpTarget->Life = iLife;
								
									TNotice pNotice(1);

									pNotice.SendToUser(lpObj->m_Index, "ÄïµÐ HP = %7.0f / %7.0f", lpTarget->Life, lpTarget->MaxLife); //Require Translation
								}
							}
						}
					}
				}
			}
			break;

		case 322:	//119:
			{
				if ( (lpObj->Authority &2 ) != 2 )
				{
					return FALSE;
				}

				LogAddTD("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHPÈ¸º¹·®¼³Á¤");	// Require Translation
				KUNDUN_GM_LOG.Output("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHPÈ¸º¹·®¼³Á¤");	// Require Translation

				int RefillHP = this->GetTokenNumber();

				if ( RefillHP <= 0 || RefillHP > 5000000 )
				{
					return 0;
				}

				giKundunRefillHP = RefillHP;

				TNotice pNotice(0);

				pNotice.SendToUser(lpObj->m_Index, "ÄïµÐ HP ÃÊ´çÈ¸º¹·® = %d È¸º¹·® = %d È¸º¹½Ã°£ = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);	// Require Translation

			}

			break;

		case 323:	//120:
			{
				if ( (lpObj->Authority &2 ) != 2 )
				{
					return FALSE;
				}

				LogAddTD("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHPÃÊ´çÈ¸º¹·®¼³Á¤");	// Require Translation
				KUNDUN_GM_LOG.Output("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHPÃÊ´çÈ¸º¹·®¼³Á¤");	// Require Translation

				int RefillHPSec = this->GetTokenNumber();

				if ( RefillHPSec <= 0 || RefillHPSec > 10000 )
				{
					return 0;
				}

				giKundunRefillHPSec = RefillHPSec;

				TNotice pNotice(0);

				pNotice.SendToUser(lpObj->m_Index, "ÄïµÐ HP ÃÊ´çÈ¸º¹·® = %d È¸º¹·® = %d È¸º¹½Ã°£ = %d",
					giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);	// Require Translation

			}

			break;

		case 324:	//121:
			{
				if ( (lpObj->Authority &2 ) != 2 )
				{
					return FALSE;
				}

				LogAddTD("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHPÈ¸º¹½Ã°£¼³Á¤");	// Require Translation
				KUNDUN_GM_LOG.Output("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", lpObj->Ip_addr, lpObj->AccountID,
					lpObj->Name, "ÄïµÐHPÈ¸º¹½Ã°£¼³Á¤");	// Require Translation

				int RefillHPTime = this->GetTokenNumber();

				if ( RefillHPTime < 0 || RefillHPTime > 60000 )
				{
					return 0;
				}

				giKundunRefillHPTime = RefillHPTime;

				TNotice pNotice(0);

				pNotice.SendToUser(lpObj->m_Index, "ÄïµÐ HP ÃÊ´çÈ¸º¹·® = %d È¸º¹·® = %d È¸º¹½Ã°£ = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);	// Require Translation

			}
			break;
		case 369:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 0);
			break;
		case 370:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 1);
			break;
		case 371:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 2);
			break;
		case 372:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 3);
			break;
		case 373:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 4);
			break;
		case 374:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 5);
			break;
		case 375:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 6);
			break;
		case 376:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 7);
			break;
		case 377:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 8);
			break;
		case 378:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 9);
			break;
		case 379:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 10);
			break;
		case 380:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 11);
			break;
		case 381:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 12);
			break;
		case 382:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 13);
			break;
		case 383:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 14);
			break;
		case 384:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 15);
			break;
		case 385:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 16);
			break;
		case 386:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 17);
			break;
		case 387:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 18);
			break;
		case 388:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 19);
			break;
		case 389:
			g_Kanturu.OperateGmCommand(lpObj->m_Index, 20);
			break;
	}
	return 0;
}







void CGMMng::GetInfinityArrowMPConsumption(LPOBJ lpObj)
{
	MsgOutput(lpObj->m_Index, "ÀÎÇÇ´ÏÆ¼ ¾Ö·Î¿ì MP ¼Ò¸ð·®[+0:%d] [+1:%d] [+2:%d]",
		g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus0(),
		g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus1(),
		g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus2());
}

					


void CGMMng::ControlInfinityArrowMPConsumption0(LPOBJ lpObj, int iValue)
{
	g_SkillAdditionInfo.SetInfinityArrowMPConsumptionPlus0(iValue);
	MsgOutput(lpObj->m_Index, "ÀÎÇÇ´ÏÆ¼ ¾Ö·Î¿ì MP ¼Ò¸ð·® º¯°æ(+0) : %d",iValue);

}	


void CGMMng::ControlInfinityArrowMPConsumption1(LPOBJ lpObj, int iValue)
{
	g_SkillAdditionInfo.SetInfinityArrowMPConsumptionPlus1(iValue);
	MsgOutput(lpObj->m_Index, "ÀÎÇÇ´ÏÆ¼ ¾Ö·Î¿ì MP ¼Ò¸ð·® º¯°æ(+1) : %d",iValue);

}

void CGMMng::ControlInfinityArrowMPConsumption2(LPOBJ lpObj, int iValue)
{
	g_SkillAdditionInfo.SetInfinityArrowMPConsumptionPlus2(iValue);
	MsgOutput(lpObj->m_Index, "ÀÎÇÇ´ÏÆ¼ ¾Ö·Î¿ì MP ¼Ò¸ð·® º¯°æ(+2) : %d",iValue);

}


void CGMMng::SetInfinityArrowTime(LPOBJ lpObj, int iValue)
{
	if ( lpObj->Class == CLASS_ELF && lpObj->Type == OBJ_USER && lpObj->ChangeUP == 1 )
	{
		lpObj->m_iMuseElfInfinityArrowSkillTime = iValue;
		MsgOutput(lpObj->m_Index, "ÀÎÇÇ´ÏÆ¼ ¾Ö·Î¿ì ½Ã°£ °­Á¦ ¼³Á¤ : %dÃÊ", iValue);
	}
	else
	{
		MsgOutput(lpObj->m_Index, "220·¹º§ ÀÌ»ó ¹ÂÁî¿¤ÇÁ¸¸ »ç¿ë°¡´ÉÇÕ´Ï´Ù.");
	}
}



void CGMMng::ControlFireScreamDoubleAttackDistance(LPOBJ lpObj, int iValue)
{
	g_SkillAdditionInfo.SetFireScreamExplosionAttackDistance(iValue);
	MsgOutput(lpObj->m_Index, "ÆÄÀÌ¾î½ºÅ©¸² ´õºíµ¥¹ÌÁö Æø¹ß°Å¸® º¯°æ:%d", iValue);
}

