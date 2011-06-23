// CrywolfStatue.cpp: implementation of the CCrywolfStatue class.
//	GS-N	1.00.18	JPN	0x00567640	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrywolfStatue.h"
#include "CrywolfAltar.h"
#include "CrywolfUtil.h"
#include "Gamemain.h"
#include "user.h"

static CCrywolfUtil UTIL;
CCrywolfStatue g_CrywolfNPC_Statue;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrywolfStatue::CCrywolfStatue()
{

}

CCrywolfStatue::~CCrywolfStatue()
{

}



int CCrywolfStatue::GetStatueViewState(int iPriestNumber)
{
	int iViewState = 0;

	switch ( iPriestNumber )
	{
		case 0:	iViewState = eVS_NONE;	break;
		case 1:	iViewState = eVS_POISON;	break;
		case 2:	iViewState = eVS_ICE;	break;
		case 3:	iViewState = eVS_ATTACK_AURA;	break;
		case 4:	iViewState = eVS_DEFENSE_AURA;	break;
		case 5:	iViewState = eVS_SOUL_BARRIER;	break;
	}

	return iViewState;
}



void CCrywolfStatue::CrywolfStatueAct(int iIndex)
{
	if ( !gObjIsConnected(iIndex))
		return;

	if ( gObj[iIndex].Type != OBJ_NPC || CHECK_CLASS(204, gObj[iIndex].Class) == FALSE )
		return;

	LPOBJ lpObj = &gObj[iIndex];
	int iContractedAlterCount = g_CrywolfNPC_Altar.GetContractedAltarCount();
	int iPriestHPSum = g_CrywolfNPC_Altar.GetPriestHPSum();
	int iPriestMaxHPSum = g_CrywolfNPC_Altar.GetPriestMaxHPSum();

	if ( iContractedAlterCount == 0 || this->m_Shield.m_iShieldHP == 0 )
	{
		if ( this->m_Shield.m_iShieldState == 1 )
		{
			UTIL.SendCrywolfUserAnyMsg(2, lMsg.Get(MSGGET(13, 5)));
			this->m_Shield.m_iShieldState = 0;
			this->m_Shield.m_iShieldHP = 0;
			this->m_Shield.m_iShieldMaxHP = 0;
			this->m_Shield.m_iPriestNumber = iContractedAlterCount;
			lpObj->m_ViewSkillState[this->GetStatueViewState(iContractedAlterCount)] = 1;
			GCStateInfoSend(lpObj, 1, this->GetStatueViewState(iContractedAlterCount));
		}
		else if ( this->m_Shield.m_iPriestNumber != iContractedAlterCount )
		{
			this->m_Shield.m_iShieldState = 1;
			this->m_Shield.m_iShieldHP = iPriestHPSum;
			this->m_Shield.m_iShieldMaxHP = iPriestMaxHPSum;
			this->m_Shield.m_iPriestNumber = iContractedAlterCount;
			UTIL.SendCrywolfUserAnyMsg(2, lMsg.Get(MSGGET(13, 6)), iContractedAlterCount, iPriestHPSum);

			lpObj->m_ViewSkillState[this->GetStatueViewState(iContractedAlterCount)] = 1;
			GCStateInfoSend(lpObj, 1, this->GetStatueViewState(iContractedAlterCount));
		}

		return;
	}
	else if ( this->m_Shield.m_iShieldState == 0 )
	{
		UTIL.SendCrywolfUserAnyMsg(2, lMsg.Get(MSGGET(13, 7)));

			lpObj->m_ViewSkillState[this->GetStatueViewState(iContractedAlterCount)] = 1;
			GCStateInfoSend(lpObj, 1, this->GetStatueViewState(iContractedAlterCount));
	}
	else if ( this->m_Shield.m_iPriestNumber != iContractedAlterCount )
	{
		UTIL.SendCrywolfUserAnyMsg(2, lMsg.Get(MSGGET(13, 6)), iContractedAlterCount, iPriestHPSum);

			lpObj->m_ViewSkillState[this->GetStatueViewState(iContractedAlterCount)] = 1;
			GCStateInfoSend(lpObj, 1, this->GetStatueViewState(iContractedAlterCount));
	}

	this->m_Shield.m_iShieldState = 1;
	this->m_Shield.m_iShieldHP = iPriestHPSum;
	this->m_Shield.m_iShieldMaxHP = iPriestMaxHPSum;
	this->m_Shield.m_iPriestNumber = iContractedAlterCount;
}