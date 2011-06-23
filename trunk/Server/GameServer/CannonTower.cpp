// CannonTower.cpp: implementation of the CCannonTower class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CannonTower.h"
#include "..\\common\\winutil.h"


#if (GS_CASTLE)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCannonTower g_CsNPC_CannonTower;

CCannonTower::CCannonTower()
{

}

CCannonTower::~CCannonTower()
{

}

void CCannonTower::CannonTowerAct(int iIndex) // 0x00560940  1.00.19
{	
	if( ( rand() % 2 ) != 0 ) 	// review this
	{
		return;
	}
	LPOBJ lpObj = &gObj[iIndex];
	int count = 0;
	int ASBOfs = 0;
	PMSG_BEATTACK_COUNT pCount;
	PMSG_BEATTACK pAttack;

	pCount.h.c = 0xC1;
	pCount.h.headcode = 0x1D;
	pCount.h.size = 0x00;

	pCount.MagicNumberH = SET_NUMBERH(0);
	pCount.MagicNumberL = SET_NUMBERL(0);
	//pCount.MagicNumber = 0x00;
	pCount.Count = 0x00;
	pCount.X = lpObj->X;
	pCount.Y = lpObj->Y;

	ASBOfs = sizeof(PMSG_BEATTACK_COUNT);

	unsigned char AttackSendBuff[256];

	while( true ) 
	{
		if( lpObj->VpPlayer2[count].state ) 
		{
			if( lpObj->VpPlayer2[count].type == OBJ_USER ) 
			{
				int tObjNum = lpObj->VpPlayer2[count].number;

				if( tObjNum >= 0 ) 
				{
					if( gObj[tObjNum].m_btCsJoinSide ) 
					{
						if( gObjCalDistance(lpObj, &gObj[tObjNum]) < 7 ) 
						{
							pAttack.NumberH = SET_NUMBERH(tObjNum);
							pAttack.NumberL = SET_NUMBERL(tObjNum);
							memcpy( (AttackSendBuff+ASBOfs), (PBYTE)&pAttack, sizeof(PMSG_BEATTACK));
							ASBOfs += sizeof(PMSG_BEATTACK);
							pCount.Count++;
						}
					}
				}
			}
		}
		count++;

		if( count > MAX_VIEWPORT-1 ) break;
	}
	if( pCount.Count > 0 ) 
	{
		pCount.h.size = ASBOfs;

		memcpy(AttackSendBuff, (LPBYTE)&pCount, sizeof(PMSG_BEATTACK_COUNT));
		CGBeattackRecv(AttackSendBuff, lpObj->m_Index, 1);

		struct PMSG_UNKNOW_STRUCT
		{
			PBMSG_HEAD h;	// C3:1E
			BYTE NumberX;	// 3
			BYTE NumberH;	// 4
			BYTE NumberL;	// 5
			BYTE X;			// 6
			BYTE Y;			// 7
			BYTE btUnk;		// 8
		};
		PMSG_UNKNOW_STRUCT pSend; // this is not the right Struct, but this will work
		pSend.h.setE(0x1E, sizeof(PMSG_UNKNOW_STRUCT));
		pSend.NumberX	= 0x32;
		pSend.X			= lpObj->X;
		pSend.Y			= lpObj->Y;
		pSend.btUnk		= 0;
		pSend.NumberH	= SET_NUMBERH(iIndex);
		pSend.NumberL	= SET_NUMBERL(iIndex);
			
		::MsgSendV2(lpObj, (PBYTE)&pSend, pSend.h.size);

	}
}


#endif