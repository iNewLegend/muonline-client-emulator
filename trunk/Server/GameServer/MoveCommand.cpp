// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-N 0.99.60T 0x004F2D50
//	GS-N	1.00.18	JPN	0x0051D480	-	Completed
#include "stdafx.h"

#include "MoveCommand.h"
#include "logproc.h"
#include "DevilSquare.h"
#include "KalimaGate.h"
#include "ChaosCastle.h"
#include "CastleSiegeSync.h"
#include "MapClass.h"
//added by leo123
#include "CManager.h"
// Added by huihui
#include "..\shared\CSVFile\CSVFile.h"

CMoveCommand gMoveCommand;


CMoveCommand::CMoveCommand()
{
}


CMoveCommand::~CMoveCommand()
{
}

int CMoveCommand::Load(const char* filename)
{
	CSVFile CSVRead;  
	if (!CSVRead.Open(true, filename))  
	{
		MsgBox("MoveLevel %s File Load Error", filename);
		return false;  
	}  

	while (CSVRead.CSVReadNextRow())
	{
		std::string strName;  
		CSVRead.CSVRead("MapName", strName);
		MOVE_MOVE_LEVEL& moveLevel = m_mapMoveLevel[strName];

		CSVRead.CSVRead("Level",moveLevel.MoveLevel);
		CSVRead.CSVRead("MapID",moveLevel.MapNumber);
		CSVRead.CSVRead("startX",moveLevel.X);
		CSVRead.CSVRead("startY",moveLevel.Y);
		CSVRead.CSVRead("endX",moveLevel.TX);
		CSVRead.CSVRead("endY",moveLevel.TY);
		CSVRead.CSVRead("Zen",moveLevel.Zen);
	}
	return true;
}


int  CMoveCommand::GetMoveLevel(int mapnumber, int x, int y, int Class)
{
	for (std::map<std::string,MOVE_MOVE_LEVEL>::const_iterator it = m_mapMoveLevel.begin();it!=m_mapMoveLevel.end();++it)
	{
		if ( x >= it->second.X && x <= it->second.TX && y >= it->second.Y && y <= it->second.TY )
		{
			if ( Class == 4 || Class == 3 )
			{
				return it->second.MoveLevel * 2 / 3;
			}
			else
			{
				return it->second.MoveLevel;
			}
		}
	}
	return -1;
}



BOOL CMoveCommand::CheckMainToMove(LPOBJ lpObj)
{
	if ( BC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( KALIMA_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( DS_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMoveCommand::CheckEquipmentToMove(LPOBJ lpObj, int iTargetMapNumber)
{
	if ( iTargetMapNumber == MAP_INDEX_ATHLANSE )
	{
		if ( lpObj->pInventory[8].IsItem() != FALSE )
		{
			if ( lpObj->pInventory[8].m_Type == ITEMGET(13,2) ) // Uniria
			{
				return FALSE;
			}

			if ( lpObj->pInventory[8].m_Type == ITEMGET(13,3) ) // Dino
			{
				return FALSE;
			}
		}
	}

	if ( iTargetMapNumber == MAP_INDEX_ICARUS )
	{
		if ( (lpObj->pInventory[8].m_Type != ITEMGET(13,3) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(13,30) &&
			 lpObj->pInventory[8].m_Type != ITEMGET(13,37) &&
			 (lpObj->pInventory[7].m_Type < ITEMGET(12,0) || lpObj->pInventory[7].m_Type > ITEMGET(12,6) && lpObj->pInventory[7].m_Type < ITEMGET(12,36)) ) ||
			 lpObj->pInventory[8].m_Type == ITEMGET(13,2) ||
			 lpObj->pInventory[11].m_Type == ITEMGET(13,10) ||
			 lpObj->pInventory[10].m_Type == ITEMGET(13,10) ||
			 lpObj->pInventory[11].m_Type == ITEMGET(13,40) ||
			 lpObj->pInventory[10].m_Type == ITEMGET(13,40)  )
		{
			return FALSE;
		}

		if ( lpObj->m_RecallMon >= 0 )
		{
			GCRecallMonLife(lpObj->m_Index, 60, 0);
			gObjMonsterCallKill(lpObj->m_Index);
		}
	}

	return TRUE;
}

BOOL CMoveCommand::CheckInterfaceToMove(LPOBJ lpObj)
{
	if ( lpObj->m_IfState.use == 1 )
	{
		return FALSE;
	}

	if ( lpObj->m_bPShopOpen == true )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMoveCommand::Move(LPOBJ lpObj, LPSTR mapname)
{
	LogAddTD("[%s][%s] Map Request %s"	, lpObj->AccountID, lpObj->Name,mapname);
	int m_number = -1;
	int overlevel = 0;
	int subMoney = 2000;

	std::map<std::string,MOVE_MOVE_LEVEL>::const_iterator it = m_mapMoveLevel.find(mapname);
	if (it==m_mapMoveLevel.end())
	{
		LogAddTD("[%s][%s] Invaild Map Index"	, lpObj->AccountID, lpObj->Name);
		return FALSE;
	}

	const MOVE_MOVE_LEVEL& moveLevel = it->second;

	if ( moveLevel.MapNumber == 23 )
	{
		if ( g_CastleSiegeSync.GetCastleState() == 7 )
		{
			return FALSE;
		}

		if ( g_CastleSiegeSync.CheckCastleOwnerMember(lpObj->m_Index) == FALSE && g_CastleSiegeSync.CheckCastleOwnerUnionMember(lpObj->m_Index) == FALSE)
		{
			return FALSE;
		}
	}

	int NeedLevel = moveLevel.MoveLevel;

	if ( lpObj->Class == 4 || lpObj->Class == 3 )
	{
		if ( NeedLevel > 0 )
		{
			NeedLevel = NeedLevel *2 / 3;
		}
	}

	if ( moveLevel.MapNumber >= 0 )
	{
		if ( NeedLevel <= lpObj->Level )
		{
			if ( (lpObj->Money - moveLevel.Zen ) >= 0 )
			{
				int X = GetMapRandPosition(moveLevel.X ,moveLevel.TX);
				int Y = GetMapRandPosition(moveLevel.Y ,moveLevel.TY);

				gObjTeleport(lpObj->m_Index, moveLevel.MapNumber, X , Y );
				lpObj->m_bPShopRedrawAbs = true;
				lpObj->Money -= moveLevel.Zen;

				LogAddTD("[%s][%s] Use [%s %s] Success : reduces money %d - %d"
						, lpObj->AccountID, lpObj->Name,
						lMsg.Get(MSGGET(11, 185)), mapname, lpObj->Money, NeedLevel);

				g_Manger.OnMoveMap(lpObj->m_Index, moveLevel.MapNumber);

				GCMoneySend(lpObj->m_Index, lpObj->Money );
					
				return TRUE;
			}
			else
			{
				::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 102)), lpObj->m_Index, 1);
			}
		}
		else
		{
			char szTemp[256];

			wsprintf(szTemp, lMsg.Get(MSGGET(4, 93)), NeedLevel);
			::GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
		}
	}

	return FALSE;

}

int  GetKalimaGateLevel(LPOBJ lpObj)
{
	if ( lpObj->Class == 3 || lpObj->Class == 4 )
	{
		for ( int i=0;i<KALIMA_FLOORS;i++)
		{
			if ( lpObj->Level >= g_sttKALIMAGATE_LEVEL[i].iLOWER_BOUND_MAGUMSA && lpObj->Level <= g_sttKALIMAGATE_LEVEL[i].iUPPER_BOUND_MAGUMSA )
			{
				return i;
			}
		}
	}
	else
	{
		for ( int i=0;i<KALIMA_FLOORS;i++)
		{
			if ( lpObj->Level >= g_sttKALIMAGATE_LEVEL[i].iLOWER_BOUND && lpObj->Level <= g_sttKALIMAGATE_LEVEL[i].iUPPER_BOUND )
			{
				return i;
			}
		}
	}

	return -1;
}

BOOL CMoveCommand::MoveFree2Kalima(LPOBJ lpObj)
{
	int iIdx = GetKalimaGateLevel(lpObj);

	if ( iIdx >= 6 )
		iIdx = 5;

	if ( iIdx < 0 )
	{
		return FALSE;
	}

	int iGateNumber = iIdx + 88;

	return gObjMoveGate(lpObj->m_Index, iGateNumber);
}