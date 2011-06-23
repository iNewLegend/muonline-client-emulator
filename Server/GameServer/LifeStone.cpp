// LifeStone.cpp: implementation of the CLifeStone class.
//	GS-CS	1.00.19	JPN	0x00562650
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LifeStone.h"

#include "CastleSiege.h"
#include "Gamemain.h"
#include "user.h"
#include "MapClass.h"

#include "logproc.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if (GS_CASTLE==1)
CLifeStone g_CsNPC_LifeStone;

CLifeStone::CLifeStone()
{
	return;
}

CLifeStone::~CLifeStone()
{
	return;
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

int CLifeStone::CreateLifeStone(int iIndex)
{
	LPOBJ lpObj = &gObj[iIndex];
	int iMonsterIndex = -1;
	BYTE cX = lpObj->X;
	BYTE cY = lpObj->Y;

	if ( g_CastleSiege.GetCastleState() != 7 )
	{
		MsgOutput(iIndex, lMsg.Get(1645));
		return FALSE;
	}

	if ( lpObj->GuildStatus != 0x80 )
		return FALSE;

	if ( lpObj->m_btCsJoinSide < 2 )
	{
		MsgOutput(iIndex, lMsg.Get(1641));
		return FALSE;
	}

	if ( lpObj->lpGuild->lpLifeStone  )
	{
		MsgOutput(iIndex, lMsg.Get(1642));
		return FALSE;
	}

	if ( cX > 160 && cX < 192 && cY > 187 && cY < 217 )
	{
		MsgOutput(iIndex, lMsg.Get(1644));
		return FALSE;
	}

	BYTE btMapAttr = MapC[lpObj->MapNumber].GetAttr(cX, cY);

	if ( gObj[iIndex].MapNumber != MAP_INDEX_CASTLESIEGE )
	{
		MsgOutput(iIndex, lMsg.Get(1627));
		return FALSE;
	}

	iMonsterIndex = gObjAddMonster(lpObj->MapNumber);

	if ( iMonsterIndex >= 0 )
	{
		MONSTER_ATTRIBUTE * MAttr = gMAttr.GetAttr(278);

		if ( MAttr == NULL )
		{
			gObjDel(iMonsterIndex);
			return FALSE;
		}

		gObjSetMonster(iMonsterIndex, 278);

		gObj[iMonsterIndex].Live = TRUE;
		gObj[iMonsterIndex].Life = MAttr->m_Hp;
		gObj[iMonsterIndex].MaxLife = MAttr->m_Hp;
		gObj[iMonsterIndex].m_PosNum = -1;
		gObj[iMonsterIndex].X = cX;
		gObj[iMonsterIndex].Y = cY;
		gObj[iMonsterIndex].MTX = cX;
		gObj[iMonsterIndex].MTY = cY;
		gObj[iMonsterIndex].TX = cX;
		gObj[iMonsterIndex].TY = cY;
		gObj[iMonsterIndex].m_OldX = cX;
		gObj[iMonsterIndex].m_OldY = cY;
		gObj[iMonsterIndex].StartX = cX;
		gObj[iMonsterIndex].StartY = cY;
		gObj[iMonsterIndex].MapNumber = lpObj->MapNumber;
		gObj[iMonsterIndex].m_MoveRange = 0;
		gObj[iMonsterIndex].Level = MAttr->m_Level;
		gObj[iMonsterIndex].Type = OBJ_MONSTER;
		gObj[iMonsterIndex].MaxRegenTime = 1000;
		gObj[iMonsterIndex].Dir = 1;
		gObj[iMonsterIndex].RegenTime = GetTickCount();
		gObj[iMonsterIndex].m_Attribute = 0;
		gObj[iMonsterIndex].DieRegen = 0;
		gObj[iMonsterIndex].m_btCsNpcType = OBJ_NPC;
		gObj[iMonsterIndex].m_btCsJoinSide = lpObj->m_btCsJoinSide;
		gObj[iMonsterIndex].lpGuild = lpObj->lpGuild;
		gObj[iMonsterIndex].m_btCreationState = 0;
		gObj[iMonsterIndex].lpGuild->lpLifeStone = &gObj[iMonsterIndex];

		MsgOutput(iIndex, lMsg.Get(1646));

		LogAddTD("[CastleSiege] LifeStone is created - [%s] [%s][%s] (Map:%d)(X:%d, Y:%d)",
			lpObj->lpGuild->Name, lpObj->AccountID, lpObj->Name, lpObj->MapNumber, lpObj->X, lpObj->Y);

		lpObj->m_btLifeStoneCount++;

	}
	else
	{
		MsgOutput(iIndex, lMsg.Get(1647));
		return FALSE;
	}

	return TRUE;
}

int CLifeStone::DeleteLifeStone(int iIndex)
{
	if ( iIndex < 0 || iIndex > OBJMAX-1 )
		return FALSE;

	LPOBJ lpLifeStone = &gObj[iIndex];

	if ( lpLifeStone->lpGuild )
	{
		LogAddTD("[CastleSiege] LifeStone is broken - [%s]", lpLifeStone->lpGuild->Name);
		lpLifeStone->lpGuild->lpLifeStone = NULL;
	}

	return TRUE;
}

int CLifeStone::SetReSpawnUserXY(int iUserIndex)
{
	if ( !gObjIsConnected(iUserIndex) )
		return FALSE;

	LPOBJ lpUser = &gObj[iUserIndex];

	if ( lpUser->MapNumber != MAP_INDEX_CASTLESIEGE )
		return FALSE;

	if ( lpUser->lpGuild == NULL )
		return FALSE;

	BYTE btCsJoinSide = lpUser->m_btCsJoinSide;

	if ( lpUser->lpGuild->lpLifeStone == NULL )
		return FALSE;

	LPOBJ lpLifeStone = lpUser->lpGuild->lpLifeStone;
	lpUser->MapNumber = MAP_INDEX_CASTLESIEGE;
	lpUser->X = lpLifeStone->X;
	lpUser->Y = lpLifeStone->Y;

	return TRUE;
}

void CLifeStone::LifeStoneAct(int iIndex)
{
	if ( !gObjIsConnected(iIndex))
		return;

	LPOBJ lpObj = &gObj[iIndex];

	lpObj->m_btLifeStoneCount++;
	BYTE btCreationState = lpObj->m_btCreationState;

	if ( btCreationState < 60 )
		lpObj->m_btCreationState = lpObj->m_btLifeStoneCount / 12;
	else
		lpObj->m_btCreationState = 5;

	if ( btCreationState != lpObj->m_btCreationState )
		GCSendObjectCreationState(iIndex);

	if ( lpObj->m_btCreationState < 5 )
		return;

	if ( lpObj->VPCount < 1 ) 
		return;

	int tObjNum = -1;

	for (int i=0;i<MAX_VIEWPORT;i++)
	{
		tObjNum = lpObj->VpPlayer[i].number;

		if ( tObjNum >= 0 )
		{
			if ( gObj[tObjNum].Type == OBJ_USER && gObj[tObjNum].Live )
			{
				if ( gObj[tObjNum].m_btCsJoinSide == 1 )
				{
					if ( abs(lpObj->Y - gObj[tObjNum].Y) <= 3 &&
						 abs(lpObj->X - gObj[tObjNum].X) <= 3 )
					{
						BOOL bLifeChange = FALSE;
						BOOL bManaChange = FALSE;
						BOOL bBpChange = FALSE;

						/*gObj[tObjNum].Life += 100.0f;
						gObj[tObjNum].Mana += 100.0f;
						gObj[tObjNum].BP += 100;*/

						if ( gObj[tObjNum].Life < (gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife))
						{
							gObj[tObjNum].Life += ( ( gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife ) * (lpObj->m_btCsNpcRgLevel+1) ) / 100.0f;

							if ( gObj[tObjNum].Life > (gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife))
								gObj[tObjNum].Life = gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife;

							bLifeChange = TRUE;
						}

						if ( gObj[tObjNum].Mana < (gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana))
						{
							gObj[tObjNum].Mana += ( ( gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana ) * (lpObj->m_btCsNpcRgLevel+1) ) / 100.0f;

							if ( gObj[tObjNum].Mana > (gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana))
								gObj[tObjNum].Mana = gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana;

							bManaChange = TRUE;
						}

						if ( gObj[tObjNum].BP < (gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP))
						{
							gObj[tObjNum].BP += ( ( gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP ) * (lpObj->m_btCsNpcRgLevel+1) ) / 100;

							if ( gObj[tObjNum].BP > (gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP))
								gObj[tObjNum].BP = gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP;

							bBpChange = TRUE;
						}

						if (bLifeChange )
							GCReFillSend(tObjNum, gObj[tObjNum].Life, 0xFF, 1, gObj[tObjNum].iShield);

						if (bManaChange ||bBpChange ) 
							GCManaSend(tObjNum, gObj[tObjNum].Mana, 0xFF, 0, gObj[tObjNum].BP);
					}
				}
			}
		}
	}
}

#endif