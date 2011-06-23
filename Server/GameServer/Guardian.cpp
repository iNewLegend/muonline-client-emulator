#include "stdafx.h"
#include "Guardian.h"
#include "GameMain.h"
#include "logproc.h"
#include "User.h"

// GS_N 0.99.60T 0x00519F60
//	GS-N	1.00.18	JPN	0x0054B8D0	-	Completed

CGuardian g_CsNPC_Guardian;

CGuardian::CGuardian()
{
	return;
}

CGuardian::~CGuardian()
{
	return;
}

BOOL CGuardian::CreateGuardian(int iIndex)
{	
	MsgOutput(iIndex, lMsg.Get( MSGGET(6,104) ));
	return true;
}

void CGuardian::GuardianAct(int iIndex)
{
	return;
}

