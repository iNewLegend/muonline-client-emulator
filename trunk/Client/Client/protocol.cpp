#include "ClientNetWork.h"
#include "protocol.h"
#include "MainRoot.h"
#include "SimpleModulus.h"
#include "UIDisplayRoleChoose.h"
#include "Monster.h"
#include "Npc.h"
#include "PlayerMe.h"
#include "World.h"
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

				}
				break;

				case 0x03: /* (SERVER->CLIENT) ENTER_WORLD */
				{
					SCEnterWorld(*((PMSG_CHARMAPJOINRESULT*)aRecv));
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
	//wchar_t wID[20]		= {0};
	//wchar_t wMsg[70]	= {0};
	//wchar_t temp[100]	= {0};
	// ----
	std::wstring wcsID = s2ws(msg.chatid);
	std::wstring wcsMsg = s2ws(msg.chatmsg);
	std::wstring wcsMessgae;
	// ----
	//MultiByteToWideChar(1, 0, msg.chatid, 10, wID, sizeof(wID));
	//MultiByteToWideChar(1, 0, msg.chatmsg, 60, wMsg, sizeof(wMsg));
	// ----
	if(msg.chatid[0] > 0)
	{
		wcsMessgae.append(L"[color=128,128,255]");
		wcsMessgae.append(wcsID);
		wcsMessgae.append(L" : [/color]");
		wcsMessgae.append(wcsMsg);
		//swprintf(temp, L"%s : %s", wID, wMsg);
		CWorld::getInstance().m_Messages.addTextBase(wcsMessgae.c_str(), CColors::White());
	}
	else
	{
		CWorld::getInstance().m_Messages.addTextBase(wcsMsg.c_str(), CColors::Yellow());
	}
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCNotice(PMSG_NOTICE & msg)
{
	wchar_t wMsg[300]	= {0};
	// ----
	MultiByteToWideChar(1258, 0, msg.Notice, sizeof(msg.Notice), wMsg, sizeof(wMsg));
	// ----
	if(msg.type == 1)
	{
		CWorld::getInstance().m_Messages.addTextBase(wMsg, CColors::LightBlue());
	}

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCPJoinResult(PMSG_JOINRESULT& msg)
{
	ULONG uPlayerID = MAKE_NUMBERW(msg.NumberH, msg.NumberL);
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
	if(pCLCount->Count <= MAX_VISUAL_ROLE)
	{
		for(size_t i = 0 ; i< pCLCount->Count ; i++)
		{
			pChar = & pCList[i];
			// ----
			aIndex = pChar->Index;
			// ----
			if(aIndex > MAX_VISUAL_ROLE)
			{
				continue;
			}
			// ----
			S_DEL(CUIDisplayRoleChoose::getInstance().m_pRole[aIndex]);
			// ----
			CRole * pRole = new CRole();
			// ----
			pRole->setRoleName(s2ws(pChar->Name).c_str());
			pRole->setLevel(pChar->Level);
			pRole->setSet(pChar->charSet);
			pRole->setPos(Vec3D(i,0,0));
			pRole->setActionState(CRole::STAND);
			pRole->updateWorldMatrix();
			// ----
			CUIDisplayRoleChoose::getInstance().m_pRole[aIndex] = pRole;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSRoleCreate(const char* szName, unsigned char uClass)
{
	PMSG_CHARCREATE msg(szName, uClass<<5);
	// ----
	NETWORK.send(& msg);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSEnterWorld(unsigned char uIndex)
{
	if(uIndex <= MAX_VISUAL_ROLE)
	{
		CRole* pRole = CUIDisplayRoleChoose::getInstance().m_pRole[uIndex];
		// ----
		if(pRole != NULL)
		{
			PMSG_CHARMAPJOIN msg(ws2s(pRole->getName()).c_str());
			// ----
			NETWORK.send(& msg);
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SCEnterWorld(PMSG_CHARMAPJOINRESULT& msg)
{
	CWorld::getInstance().create(msg.MapNumber);
	// ----
	CPlayerMe::getInstance().setCellPos(msg.MapX,msg.MapY);
	// ----
	CWorld::getInstance().addRole(&CPlayerMe::getInstance());
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
		if(uID > 40000)
		{
			pPlayer = & CPlayerMe::getInstance();
		}
		// ----
		if(pPlayer == NULL)
		{
			pPlayer = new CRole();
			// ---
			pPlayer->setID(uID);
			//pPlayer->setLevel(pChar->Level);
			pPlayer->setRoleName(s2ws(pPlayerViewport->Id).c_str());
			pPlayer->setSet(pPlayerViewport->charSet);
			pPlayer->setCellPos(pPlayerViewport->X ,pPlayerViewport->Y);
			// ---- 
			// # Warning! not sure, just let him dont still dead.
			// ---- 
			pPlayer->setActionState(CRole::STAND);
			pPlayer->setDir(S2CDIR(pPlayerViewport->DirAndPkLevel >> 4));
			// ----
			CWorld::getInstance().addRole(pPlayer);
		}
		else
		{
			pPlayer->setRoleName(s2ws(pPlayerViewport->Id).c_str());
			pPlayer->setSet(pPlayerViewport->charSet);
			pPlayer->setCellPos(pPlayerViewport->X, pPlayerViewport->Y);
			// ---- 
			// # Warning! not sure, just let him dont still dead.
			// ---- 
			pPlayer->setActionState(CRole::STAND); 
			pPlayer->setDir(S2CDIR(pPlayerViewport->DirAndPkLevel>>4));
		}
		// ----
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
			pMonster->setCellPos(pMonsterViewport->X, pMonsterViewport->Y);
			pMonster->setDir(S2CDIR(pMonsterViewport->Path >> 4));
			// ----
			CWorld::getInstance().addRole(pMonster);
		}
		else
		{
			pMonster->setCellPos(pMonsterViewport->X, pMonsterViewport->Y);
			pMonster->setDir(S2CDIR(pMonsterViewport->Path >> 4));
		}
		// ----
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

void CSMove(int x, int y, std::vector<unsigned char> path, unsigned char uTargetDir)
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