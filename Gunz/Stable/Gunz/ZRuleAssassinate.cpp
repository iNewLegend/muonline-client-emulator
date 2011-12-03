#include "stdafx.h"
#include "ZRuleAssassinate.h"
#include "ZMatch.h"
#include "ZGame.h"
#include "ZGlobal.h"
#include "MMatchTransDataType.h"

ZRuleAssassinate::ZRuleAssassinate(ZMatch* pMatch) : ZRuleTeamDeathMatch(pMatch)
{

}

ZRuleAssassinate::~ZRuleAssassinate()
{

}

bool ZRuleAssassinate::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_COMMANDER:
		{
			MUID uidRedCommander, uidBlueCommander;

			pCommand->GetParameter(&uidRedCommander,		0, MPT_UID);
			pCommand->GetParameter(&uidBlueCommander,		1, MPT_UID);

			AssignCommander(uidRedCommander, uidBlueCommander);
		}
		break;

	}

	return false;
}

void ZRuleAssassinate::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{
	MTD_RuleInfo_Assassinate* pAssassinateRule = (MTD_RuleInfo_Assassinate*)pInfo;
	AssignCommander(pAssassinateRule->uidRedCommander, pAssassinateRule->uidBlueCommander);
}

void ZRuleAssassinate::AssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander)
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZGetEffectManager()->Clear();
		ZCharacter* pCharacter = (*itor).second;
		pCharacter->m_dwStatusBitPackingValue.Ref().m_bCommander = false;
	}	

	ZCharacter* pRedChar = ZGetGame()->m_CharacterManager.Find(uidRedCommander);
	ZCharacter* pBlueChar = ZGetGame()->m_CharacterManager.Find(uidBlueCommander);

	if(pRedChar) {
		ZGetEffectManager()->AddCommanderIcon(pRedChar,0);
		pRedChar->m_dwStatusBitPackingValue.Ref().m_bCommander = true;
	}
	if(pBlueChar) {
		ZGetEffectManager()->AddCommanderIcon(pBlueChar,1);
		pBlueChar->m_dwStatusBitPackingValue.Ref().m_bCommander = true;
	}

#ifdef _DEBUG
	//// DEBUG LOG ////
	const char *szUnknown = "unknown";
	char szBuf[128];
	sprintf(szBuf, "RedCMDER=%s , BlueCMDER=%s \n", 
		pRedChar ? pRedChar->GetProperty()->GetName() : szUnknown , 
		pBlueChar ? pBlueChar->GetProperty()->GetName() : szUnknown );
	OutputDebugString(szBuf);
	///////////////////
#endif
}
