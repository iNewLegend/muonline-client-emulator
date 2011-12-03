#include "stdafx.h"
#include "MVoteDiscussImpl.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MLex.h"


///////////////////////////////////////////////////////
// MVoteDiscussBuilder
MVoteDiscuss* MVoteDiscussBuilder::Build(const MUID& uidDrafter, const MUID& uidStage, const char* pszDiscuss, const char* pszArg)
{
	if ( (stricmp(pszDiscuss, "joke") == 0) || (stricmp(pszDiscuss, "���") == 0) ) {
		MVoteDiscussJoke* pDiscuss = new MVoteDiscussJoke(uidStage);
		pDiscuss->m_strJoke = pszArg;
		return pDiscuss;
	}
	else if ( (stricmp(pszDiscuss, "kick") == 0) || (stricmp(pszDiscuss, "�߹�") == 0) ) {
		MVoteDiscussKick* pDiscuss = new MVoteDiscussKick(uidStage);
		pDiscuss->m_strTarget = pszArg;
		return pDiscuss;
	}
	return NULL;
}

///////////////////////////////////////////////////////
// MVoteDiscussJoke
MVoteDiscussJoke::MVoteDiscussJoke(const MUID& uidStage) : MVoteDiscuss(uidStage)
{
}

bool MVoteDiscussJoke::OnJudge(bool bJudge)
{
	MMatchServer* pServer = MMatchServer::GetInstance();

	char szMsg[ VOTE_ARG_STRING_LEN ] = {0,};

	sprintf(szMsg, "%s", m_strJoke.c_str());

	MCommand* pCmd = pServer->CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUInt(0));
	pCmd->AddParameter(new MCmdParamStr(szMsg));
	pServer->RouteToStage(GetStageUID(), pCmd);

	return true;
}

///////////////////////////////////////////////////////
// MVoteDiscussKick
MVoteDiscussKick::MVoteDiscussKick(const MUID& uidStage) : MVoteDiscuss(uidStage)
{
}

bool MVoteDiscussKick::OnJudge(bool bJudge)
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	MMatchStage* pStage = pServer->FindStage(GetStageUID());
	if (pStage == NULL) return false;

	if (bJudge == true) {
		if ( pStage->KickBanPlayer(m_strTarget.c_str()) ) {
		}
	}

	return true;
}
