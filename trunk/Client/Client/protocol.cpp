#include "ClientNetWork.h"
#include "protocol.h"
#include "MainRoot.h"
#include "SimpleModulus.h"
#include "UIDisplayRoleChoose.h"
#include "Monster.h"
#include "Npc.h"
#include "PlayerMe.h"
#include "World.h"
#include "UIChatList.h"
#include "GlobalFunction.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ProtocolCore(unsigned char protoNum, const unsigned char* aRecv, int aLen)
{
	switch(protoNum)
	{
		case 0xF1:
		{
			switch(aRecv[3])
			{
				case 0x00: /* (SERVER->CLIENT) JOIN_RESULT */
				{
					SCPJoinResult(*((PMSG_JOINRESULT*)aRecv));
				}
				break;

				case 0x01: /* (SERVER->CLIENT) LOGIN_RESULT */
				{
				
					SCLoginResult(*((PMSG_RESULT*)aRecv));
				}
				break;

				default:
					{
						char szTemp[15];
						OutputDebugString("0xF1 msg:");
						OutputDebugString(itoa(aRecv[0],szTemp,16));
						OutputDebugString(itoa(aRecv[1],szTemp,16));
						OutputDebugString(itoa(aRecv[2],szTemp,16));
						OutputDebugString(itoa(aRecv[4],szTemp,16));
						OutputDebugString(itoa(aRecv[4],szTemp,16));
						OutputDebugString(itoa(aRecv[6],szTemp,16));
						OutputDebugString("\n");
					}
					break;
			}
		}
		break; /* CASE OxF1 */

		case 0xF3:
		{
			switch(aRecv[3])
			{
				case 0x00: /* (SERVER->CLIENT) CHAR_LIST_RESULT */
				{
					SCCharList(aRecv);
				}
				break;

 				case 0x01:
 				{
					SCCharCreateResult(*((PMSG_CHARCREATERESULT*)aRecv));
 				}
 				break;

				case 0x02:
					{
						CharDeleteResult(*((PMSG_RESULT*)aRecv));
					}
					break;

				case 0x03: /* (SERVER->CLIENT) ENTER_WORLD */
				{
					SCEnterWorld(*((PMSG_CHARMAPJOINRESULT*)aRecv));
				}
				break;

				default:
					{
						char szTemp[15];
						OutputDebugString("0xF3 msg:");
						OutputDebugString(itoa(aRecv[0],szTemp,16));
						OutputDebugString(itoa(aRecv[1],szTemp,16));
						OutputDebugString(itoa(aRecv[2],szTemp,16));
						OutputDebugString(itoa(aRecv[4],szTemp,16));
						OutputDebugString(itoa(aRecv[4],szTemp,16));
						OutputDebugString(itoa(aRecv[6],szTemp,16));
						OutputDebugString("\n");
					}
					break;
			}
		}
		break; /* CASE 0xF3 */

		case GMSG_CHAT:
		{
			SCChatMsg(*((PMSG_CHATDATA*)aRecv));
		}
		break;

		case GMSG_NOTICE:
		{
			SCNotice(*((PMSG_NOTICE*)aRecv));
		}
		break;

		case GMSG_CHECK_MAINEXE:
		{
			SCCheckMainExe(*((PMSG_CHECK_MAINEXE*)aRecv));
		}
		break;
	
		case GMSG_CHAR_VIEWPORT_CREATE:
		{
			SCPlayerViewportCreate(aRecv);
		}
		break;

		case GMSG_MONSTER_VIEWPORT_CREATE:
		{
			SCMonsterViewportCreate(aRecv);
		}
		break;

		case GMSG_VIEWPORT_DESTROY:
		{
			SCViewportDestroy(aRecv);
		}
		break;

		case GMSG_MOVE:
		{
			SCMove(*((PMSG_RECVMOVE*)aRecv));
		}
		break;
	
		case GMSG_ATTACK:
		{
			SCAttack(*((PMSG_ATTACKRESULT*)aRecv));
		}
		break;
	
		case GMSG_KILL_PLAYER:
		{
			SCKillPlayer(*((PMSG_KILLPLAYER*)aRecv));
		}
		break;

		case GMSG_DIE_PLAYER:
		{
			SCDiePlayer(*((PMSG_DIEPLAYER*)aRecv));
		}
		break;
	
		case GMSG_ACTION:
		{
			SCAction(*((PMSG_ACTIONRESULT*)aRecv));
		}
		break;

		case GMSG_TELEPORT:
		{
			SCTeleport(*((PMSG_TELEPORT_RESULT*)aRecv));
		}
		break;

		case GMSG_ITEM_VIEWPORT_CREATE:
			{
				SCItemViewportCreate(aRecv);	
			}
			break;

		case GMSG_ITEM_VIEWPORT_DESTROY:
			{
				SCItemViewportDestroy(aRecv);	
			}
			break;

		case GMSG_REFILL:
			{
				SCRefill(*((PMSG_REFILL*)aRecv));
			}
			break;

		case GMSG_MANA_SEND:
			{
				SCManaSend(*((PMSG_MANASEND*)aRecv));
			}
			break;
			
		default:
			{
				char szTemp[15];
				OutputDebugString("unkonw msg:");
				OutputDebugString(itoa(aRecv[0],szTemp,16));
				OutputDebugString(itoa(aRecv[1],szTemp,16));
				OutputDebugString(itoa(aRecv[2],szTemp,16));
				OutputDebugString(itoa(aRecv[4],szTemp,16));
				OutputDebugString(itoa(aRecv[4],szTemp,16));
				OutputDebugString(itoa(aRecv[6],szTemp,16));
				OutputDebugString("\n");
			}
			break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CSConnectServer(const char* szIP, int nPort)
{
	return NETWORK.Connect(szIP, nPort);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCChatMsg(PMSG_CHATDATA & msg)
{
	std::wstring wcsMessgae;
	// ----
	if(msg.chatid[0] > 0)
	{
		wcsMessgae.append(L"[color=128,128,255]");
		wcsMessgae.append(s2ws(msg.chatid));
		wcsMessgae.append(L" : [/color]");
		wcsMessgae.append(s2ws(msg.chatmsg));
		//swprintf(temp, L"%s : %s", wID, wMsg);
		// ----
		CUIChatList::getInstance().addMessage(wcsMessgae.c_str());
	}
	else
	{
		wcsMessgae.append(L"[color=255,255,128]");
		wcsMessgae.append(s2ws(msg.chatmsg));
		wcsMessgae.append(L" : [/color]");
		// ----
		CUIChatList::getInstance().addMessage(wcsMessgae.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCNotice(PMSG_NOTICE & msg)
{
	if(msg.type == 1)
	{
		std::wstring wcsMessgae;
		wcsMessgae.append(L"[color=128,128,255]");
		wcsMessgae.append(s2ws(msg.Notice));
		wcsMessgae.append(L" : [/color]");
		// ----
		CUIChatList::getInstance().addMessage(wcsMessgae.c_str());
	}

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCPJoinResult(PMSG_JOINRESULT& msg)
{
	ULONG uPlayerID = MAKE_NUMBERW(msg.NumberH, msg.NumberL)+0x8000;
	// ----
	CPlayerMe::getInstance().setID(uPlayerID);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSLoginServer(const char* szUser, const char* szPassword)
{
	PMSG_IDPASS msg(szUser,szPassword,GetTickCount(),"10308","sb6VoJorGDeU503D");
	// ----
	BuxConvert(msg.Id,		10);
	BuxConvert(msg.Pass,	10);
	// ----
	NETWORK.send(& msg);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCLoginResult(PMSG_RESULT & msg)
{
	switch(msg.result)
	{
		case eIDPASSRESULT_BADPASS:
		{
			LUA_MessageBox(L"Error : The username or password is Invalid");
		}
		break;

		case eIDPASSRESULT_SUCCESS:
		{
			CMainRoot::getInstance().getMainDialog().postMsg("ROLE_CHOOSE");
			// ---
			CSGetCharList();
		}
		break;

		case eIDPASSRESULT_BADID:
		{
			LUA_MessageBox(L"Error : The username or password is Invalid");
		}
		break;

		case eIDPASSRESULT_INUSE:
		{
			LUA_MessageBox(L"Error : The username is already in use");
		}
		break;

		case eIDPASSRESULT_MAXSERVER:
		{
			LUA_MessageBox(L"Error : Server is busy, please try later or select another server");
		}
		break;

		case eIDPASSRESULT_ACCBANED:
		{
			LUA_MessageBox(L"Error : The username is banned");
		}
		break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSGetCharList()
{
	PMSG_DEFAULT2 msg = {0};
	// ----
	msg.h.set(0xF3, sizeof(msg));
	// ----
	msg.subcode = 0x00;
	// ----
	NETWORK.send(& msg);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCCharList(const unsigned char * msg)
{
	PMSG_CHARLISTCOUNT * pCLCount	= (PMSG_CHARLISTCOUNT*) msg;
	PMSG_CHARLIST * pCList			= (PMSG_CHARLIST*)(msg + sizeof(PMSG_CHARLISTCOUNT));
	// ----
	PMSG_CHARLIST * pChar			= NULL;
	// ----
	int	aIndex						= -1;
	// ----
	CUIDisplayRoleList::getInstance().clearAllRole();
	for(size_t i = 0 ; i< pCLCount->Count ; i++)
	{
		pChar = & pCList[i];
		// ----
		if (aIndex==-1)
		{
			CUIDisplayRoleList::getInstance().setSelectIndex(pChar->Index);
		}
		aIndex = pChar->Index;
		// ----
		CRole * pPlayer = (CRole*)CWorld::getInstance().getRole(aIndex);
		// ----
		if (!pPlayer)
		{
			pPlayer = new CRole();
			pPlayer->setRoleName(s2ws(pChar->Name).c_str());
			pPlayer->setLevel(pChar->Level);
			pPlayer->setSet(pChar->CharSet);
			pPlayer->setPos(Vec3D(aIndex,0,0));
			pPlayer->setActionState(CRole::STAND);
			BBox localBox(-2.0f,-2.0f,-2.0f, 2.0f, 2.0f, 2.0f);
			pPlayer->setLocalBBox(localBox);
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSCharCreate(const char* szName, unsigned char uClass)
{
	PMSG_CHARCREATE msg(szName, uClass<<5);
	// ----
	NETWORK.send(& msg);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCCharCreateResult(PMSG_CHARCREATERESULT & msg)
{
	CUIDisplayRoleList::getInstance().setSelectIndex(msg.pos);
	// ----
	switch(msg.Result)
	{
	case 0:
		{
			LUA_MessageBox(L"创建人物失败！");
		}
		break;

	case 1:
		{
			CRole * pPlayer = (CRole*)CWorld::getInstance().getRole(msg.pos);
			// ----
			if (!pPlayer)
			{
				pPlayer = new CRole();
				// ---
				pPlayer->setID(msg.pos);
				pPlayer->setRoleName(s2ws(msg.Name).c_str());
				pPlayer->setLevel(msg.Level);
				pPlayer->setPos(Vec3D(msg.pos,0,0));
				pPlayer->setActionState(CRole::STAND);
				BBox localBox(-2.0f,-2.0f,-2.0f, 2.0f, 2.0f, 2.0f);
				pPlayer->setLocalBBox(localBox);
				CWorld::getInstance().addRole(pPlayer);
			}
		}
		break;

	case 2:
		{
			LUA_MessageBox(L"可创建人物数量已达到上限！");
		}
		break;

	default:
		{
			wchar_t wszTemp[255];
			swprintf(wszTemp,L"创建人物失败！ID=%d",msg.Result);
			LUA_MessageBox(wszTemp);
		}
		break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CharDelete(const char* szName, const char* szLastJoominNumber)
{
	if(szName)
	{
		PMSG_CHARDELETE msg(szName, szLastJoominNumber);
		NETWORK.send(& msg);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CharDeleteResult(PMSG_RESULT & msg)
{
	if (msg.result==0)
	{
		LUA_MessageBox(L"删除人物失败！");
	}
	else
	{
		CRole* pPlayer = (CRole*)*CWorld::getInstance().getFocusNodes().getChildObj().begin();
		if (pPlayer)
		{
			CWorld::getInstance().removeChild(pPlayer);
			delete pPlayer;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSEnterWorld(const char* szName)
{
	if (szName)
	{
		PMSG_CHARMAPJOIN msg(szName);
		NETWORK.send(& msg);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCEnterWorld(PMSG_CHARMAPJOINRESULT& msg)
{
	CWorld::getInstance().create(msg.data.MapNumber);
	// ----
	CPlayerMe::getInstance().setCellPos(msg.data.MapX,msg.data.MapY);
	// ----
	CWorld::getInstance().addRole(&CPlayerMe::getInstance());
	// ----
	CPlayerMe::getInstance().setCharacterData(msg.data);
	// ----
	CMainRoot::getInstance().getMainDialog().postMsg("ENTER_WORLD");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSMoveDataLoadingOK()
{
	PMSG_DEFAULT2 msg;
	// ----
	msg.h.set(0xF3,sizeof(msg));
	// ----
	msg.subcode = 0x12;
	// ----
	NETWORK.send(& msg);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSChat(const char* szChat)
{
	int chatLen = strlen(szChat);
	// ----
	if((chatLen <= 60) && (chatLen > 0))
	{
		PMSG_CHATDATA msg = {0};
		// ----
		memset(& msg, 0x00, sizeof(msg));
		// ----
		msg.h.set(0x00, sizeof(msg));
		// ----
		memcpy(msg.chatmsg, szChat, chatLen);
		// ----
		NETWORK.send(& msg);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCCheckMainExe(PMSG_CHECK_MAINEXE & msg)
{
	PMSG_CHECK_MAINEXE_RESULT msgRet(0);
	// ----
	NETWORK.send(& msgRet);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSLiveClient()
{
	PMSG_CLIENTTIME msg(GetTickCount() ,0x1F, 0x1F);
	// ----
	NETWORK.send(& msg);
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCPlayerViewportCreate(const unsigned char * msg)
{
	PWMSG_COUNT* pCount = (PWMSG_COUNT*)msg;
	msg					+= sizeof(PWMSG_COUNT);
	// ----
	LONG uID			= 0;
	size_t j			= 0;
	// ----
	PMSG_VIEWPORTCREATE * pPlayerViewport   = NULL;
	CRole * pPlayer							= NULL;
	// ----
	for(size_t i = 0 ; i < pCount->count ; i++)
	{
		pPlayerViewport	= (PMSG_VIEWPORTCREATE*)(msg);
		// ----
		msg				+= sizeof(PMSG_VIEWPORTCREATE);
		// ----
		ULONG uID		= MAKE_NUMBERW(pPlayerViewport->NumberH, pPlayerViewport->NumberL);
		// ----
		 pPlayer		= (CRole*)CWorld::getInstance().getRole(uID);
		// ----
		// ----
		if(pPlayer == NULL)
		{
			pPlayer = new CRole();
			// ---
			pPlayer->setID(uID);
			//pPlayer->setLevel(pChar->Level);
			pPlayer->setActionState(CRole::STAND);
			CWorld::getInstance().addRole(pPlayer);
		}
		// ----20%
		// Set Name For Test
		char szName[256];
		sprintf(szName,"%s[br]%d[br]PK%d[br]ViewSkillCount=%d",
			pPlayerViewport->Id,uID,
			pPlayerViewport->DirAndPkLevel&0xF,
			pPlayerViewport->btViewSkillCount);
		pPlayer->setRoleName(s2ws(szName).c_str());
		// ----
		// Equip
		pPlayer->setSet(pPlayerViewport->CharSet);
		// ----
		// Postion
		pPlayer->setCellPos(pPlayerViewport->X ,pPlayerViewport->Y);
		// ---- 
		// # Warning! not sure, just let him dont still dead.
		// ---- 
		//pPlayer->setDir(S2CDIR(pPlayerViewport->DirAndPkLevel >> 4));
		// ----
		// Move to Target
		pPlayer->setTargetCellPos(pPlayerViewport->TX, pPlayerViewport->TY);
		pPlayer->setTargetDir(S2CDIR(pPlayerViewport->DirAndPkLevel >> 4));
		// ----0%
		for(j = 0 ; j < pPlayerViewport->btViewSkillCount ; ++j)
		{
			msg++;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCMonsterViewportCreate(const unsigned char * msg)
{
	PWMSG_COUNT * pCount	= (PWMSG_COUNT*)msg;
	msg						+= sizeof(PWMSG_COUNT);
	// ----
	ULONG uID				= 0;
	ULONG uMonsterType		= 0;
	size_t j				= 0;
	// ----
	PMSG_MONSTER_VIEWPORTCREATE * pMonsterViewport = NULL;
	CMonster * pMonster							   = NULL;
	// ----
	for(size_t  i= 0 ; i < pCount->count ; i++)
	{
		pMonsterViewport	= (PMSG_MONSTER_VIEWPORTCREATE*)(msg);
		msg					+=sizeof(PMSG_MONSTER_VIEWPORTCREATE);
		// ----
		uID					= MAKE_NUMBERW(pMonsterViewport->NumberH, pMonsterViewport->NumberL);
		uMonsterType		= MAKE_NUMBERW(pMonsterViewport->Type_HI, pMonsterViewport->Type_LO);
		// ----
		pMonster			= (CMonster*)CWorld::getInstance().getRole(uID);
		// ----
		if(pMonster == NULL)
		{
			if(uMonsterType > 200)
			{
				pMonster = new CNpc();
			}
			else
			{
				pMonster = new CMonster();
			}
			// ----
			pMonster->setID(uID);
			pMonster->setType(uMonsterType);
			CWorld::getInstance().addRole(pMonster);
		}
		// ----
		// Move to Target
		pMonster->setCellPos(pMonsterViewport->X, pMonsterViewport->Y);
		pMonster->setTargetCellPos(pMonsterViewport->TX, pMonsterViewport->TY);
		//pMonster->setDir(S2CDIR(pMonsterViewport->Path >> 4));
		pMonster->setTargetDir(S2CDIR(pMonsterViewport->Path >> 4));

		// ----0%
		for(j = 0 ; j < pMonsterViewport->btViewSkillCount ; j++)
		{
			msg++;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCViewportDestroy(const unsigned char* msg)
{
	PWMSG_COUNT* pCount = (PWMSG_COUNT*)msg;
	msg					+=sizeof(PWMSG_COUNT);
	// ----
	ULONG uID			= 0;
	// ----
	PMSG_VIEWPORTDESTROY* pViewportDestroy = NULL;
	// ----
	for(size_t i = 0 ; i < pCount->count ; i++)
	{
		pViewportDestroy	= (PMSG_VIEWPORTDESTROY*)(msg);
		msg					+=sizeof(PMSG_VIEWPORTDESTROY);
		// ----
		uID					= MAKE_NUMBERW(pViewportDestroy->NumberH, pViewportDestroy->NumberL);
		// ----
		CWorld::getInstance().delRole(uID);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSMove(int x, int y, std::deque<char> path, unsigned char uTargetDir)
{
	if((path.size() > 0) && (path.size() < 14))
	{
		PMSG_MOVE msg = {0};
		// ----
		msg.h.set(GMSG_MOVE, sizeof(msg));
		// ----
		msg.X		= x;
		msg.Y		= y;
		msg.Path[0] = C2SDIR(uTargetDir) << 4;
		msg.Path[0] |= path.size();
		// ----
		UCHAR uDir	= 0x00;
		// ----
		for(size_t i = 0 ; i < path.size() ; i++)
		{
			uDir = C2SDIR(path[i]);
			// ----
			if((i % 2) == 0)
			{
				msg.Path[i / 2 + 1] = uDir << 4;
			}
			else
			{
				msg.Path[i / 2 + 1] |= uDir;
			}
		}
		// ----
		NETWORK.send(& msg);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCMove(PMSG_RECVMOVE & msg)
{
	ULONG uID		= MAKE_NUMBERW(msg.NumberH, msg.NumberL);
	CRole* pRole	= CWorld::getInstance().getRole(uID);
	// ----
	if(pRole)
	{
		pRole->setTargetCellPos(msg.X, msg.Y);
		pRole->setTargetDir(S2CDIR(msg.Path >> 4));
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSAttack(unsigned char AttackAction, int Number, unsigned char DirDis)
{
	PMSG_ATTACK msg(AttackAction, Number, DirDis);
	// ----
	NETWORK.send(& msg);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCAttack(PMSG_ATTACKRESULT & msg)
{
	ULONG uID		= MAKE_NUMBERW(msg.NumberH,msg.NumberL);
	CRole* pRole	= CWorld::getInstance().getRole(uID);
	// ----
	if(pRole)
	{
		ULONG uDamage		= MAKE_NUMBERW(msg.DamageH, msg.DamageL);
		ULONG uShieldDamage = MAKE_NUMBERW(msg.btShieldDamageH ,msg.btShieldDamageH);
		// ----
		pRole->damage(uDamage,msg.DamageType,uShieldDamage);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCKillPlayer(PMSG_KILLPLAYER & msg)
{
 	ULONG uID		= MAKE_NUMBERW(msg.NumberH, msg.NumberL);
 	CRole* pRole	= CWorld::getInstance().getRole(uID);
	// ----
 	if(pRole)
 	{
		ULONG uDamage = MAKE_NUMBERW(msg.DamageH, msg.DamageL);
		// ----
		pRole->damage(uDamage, 0, 0);
 	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCDiePlayer(PMSG_DIEPLAYER & msg)
{
	ULONG uID		= MAKE_NUMBERW(msg.NumberH, msg.NumberL);
	ULONG uKillerID = MAKE_NUMBERW(msg.KillerNumberH, msg.KillerNumberL);
	// ----
	CRole* pRole = CWorld::getInstance().getRole(uID);
	// ----
	if(pRole)
	{
		pRole->setActionState(CRole::DIE);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCAction(PMSG_ACTIONRESULT & msg)
{
	ULONG uID		= MAKE_NUMBERW(msg.NumberH, msg.NumberL);
	CRole* pRole	= CWorld::getInstance().getRole(uID);
	// ----
	if(pRole)
	{
		pRole->setDir(S2CDIR(msg.Dir));
		// ----
		switch(msg.ActionNumber)
		{
			case 0x7A:
			{
				pRole->setActionState(CRole::HIT1);
			}
			break;

			case 0x78:
			{
			
				pRole->setActionState(CRole::HIT1);
			}
			break;

			default:
			{
				pRole->setActionState(CRole::HIT1);
			}
			break;
		}
		// ----
		pRole->setActionState(CRole::HIT1);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCTeleport(PMSG_TELEPORT_RESULT & msg)
{
	CWorld::getInstance().create(msg.MapNumber);
	// ----
	CPlayerMe::getInstance().setCellPos(msg.MapX,msg.MapY);
	CPlayerMe::getInstance().setActionState(CRole::STAND);
	CPlayerMe::getInstance().setDir(S2CDIR(msg.Dir >> 4));
	// ----
	CSMoveDataLoadingOK();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCItemViewportCreate(const unsigned char* msg)
{

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCItemViewportDestroy(const unsigned char* msg)
{

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCRefill(PMSG_REFILL & msg)//20%
{
	char szTemp[256];
	sprintf(szTemp, "IPos=%d, Life=%d, Shield=%d", msg.IPos, MAKE_NUMBERW(msg.LifeH,msg.LifeL), MAKE_NUMBERW(msg.btShieldH,msg.btShieldL));
	OutputDebugString(szTemp);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCManaSend(PMSG_MANASEND & msg)//20%
{
	char szTemp[256];
	sprintf(szTemp, "IPos=%d, Mana=%d, BP=%d", msg.IPos, MAKE_NUMBERW(msg.ManaH,msg.ManaL), MAKE_NUMBERW(msg.BPH,msg.BPL));
	OutputDebugString(szTemp);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------