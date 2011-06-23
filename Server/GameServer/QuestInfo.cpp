// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#include "stdafx.h"
#include "QuestInfo.h"
#include "..\include\readscript.h"
#include "..\common\WzMemScript.h"
#include "logproc.h"
#include "GameServer.h"
#include "GameMain.h"
#include "protocol.h"
#include "GameServerAuth.h"
#include "gObjMonster.h"
#include "DSProtocol.h"
#include "Messages.h"

Messages *qMsg = new Messages();

// GS-N 0.99.60T 0x0046EBF0
//	GS-N	1.00.18	JPN	0x00485BA0	-	Completed

//CQuestInfo::QuestClear Weird type of compilation
unsigned char QuestBitMask[8];
CQuestInfo g_QuestInfo;




char sQuestString[4][50] = { "None", "Accept", "Clear", "None"};	// THIS IS NOT THE PLACE OF THIS VARIABLE




CQuestInfo::CQuestInfo()
{
	QuestBitMask[0]=0xFC;
	QuestBitMask[1]=0xFC;
	QuestBitMask[2]=0xF3;
	QuestBitMask[3]=0xF3;
	QuestBitMask[4]=0xCF;
	QuestBitMask[5]=0xCF;
	QuestBitMask[6]=0x3F;
	QuestBitMask[7]=0x3F;

	this->Init();
}


CQuestInfo::~CQuestInfo()
{
	return;
}

void CQuestInfo::Init()
{
	this->m_QuestCount = 0;
	memset(this->QuestInfo, -1, sizeof(this->QuestInfo));

	for ( int i=0;i<MAX_QUEST_INFO;i++)
	{
		this->QuestInfo[i].QuestConditionCount=0;
		this->QuestInfo[i].QuestSubInfoCount= 0;
	}
}

BOOL CQuestInfo::LoadQuestInfo(char * filename)
{
	int Token;
	int n;

	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox("load error %s", filename);
		return false;
	}

	this->Init();

	n = 0;

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 1 )
			{
				int Type = TokenNumber;

				if ( Type == 0 )
				{
					Token = GetToken();
					auto int iQuestIndex = TokenNumber;
					this->QuestInfo[iQuestIndex].QuestIndex = iQuestIndex;

					Token = GetToken();
					this->QuestInfo[iQuestIndex].QuestStartType = TokenNumber;

					Token = GetToken();
					this->QuestInfo[iQuestIndex].QuestStartSubType = TokenNumber;

					Token = GetToken();
					strcpy( this->QuestInfo[iQuestIndex].Name, TokenString);

					this->QuestInfo[iQuestIndex].QuestSubInfoCount = 0;

					while ( true )
					{
						int iSubInfoCount = this->QuestInfo[iQuestIndex].QuestSubInfoCount;

						Token = GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", TokenString) == 0 )
							{
								break;
							}
						}
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].QuestType = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedType = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedSubType = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ItemLevel = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedNumber = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMinLevel = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMaxLevel = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedDropRate = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardType = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardSubType = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardCount = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].LinkConditionIndex = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextBeforeReg = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextAfterReg = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextCompleteQuest = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextQuestClear = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[0] = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[1] = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[2] = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[3] = TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[4] = TokenNumber;

						this->QuestInfo[iQuestIndex].QuestSubInfoCount++;

					
					}
					
					LogAdd("[Quest] (%s) Load : Index %d, SubInfoCount %d",
						this->QuestInfo[iQuestIndex].Name, iQuestIndex,
						this->QuestInfo[iQuestIndex].QuestSubInfoCount );
						this->m_QuestCount++;
				}
				else if ( Type == 1 )
				{
					Token = GetToken();
					auto int iQuestIndex = TokenNumber;

					while ( true )
					{
						auto int iFailInfoCount = this->QuestInfo[iQuestIndex].QuestConditionCount;
						Token = GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", TokenString) == 0 )
							{
								break;
							}
						}
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].Index = TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedQuestIndex = TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MinLevel = TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MaxLevel = TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].ReqStr = TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedZen = TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].StartContext = TokenNumber;

							this->QuestInfo[iQuestIndex].QuestConditionCount++;
						
					}
				}
				else
				{
					MsgBox("Quest Info Read Fail");
					fclose(SMDFile);
					return false;
				}
			}
		}
	}

	fclose(SMDFile);
	this->InitQuestItem();
	return true;
}


BOOL CQuestInfo::LoadQuestInfo(char* Buffer, int iSize)
{

	CWzMemScript WzMemScript;
	int Token;
	int n;

	WzMemScript.SetBuffer(Buffer, iSize);
	this->Init();

	n = 0;

	while ( true )
	{
		Token = WzMemScript.GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 1 )
			{
				int Type = WzMemScript.GetNumber();

				if ( Type == 0 )
				{
					Token = WzMemScript.GetToken();
					auto int iQuestIndex = WzMemScript.GetNumber();
					this->QuestInfo[iQuestIndex].QuestIndex = iQuestIndex;

					Token = WzMemScript.GetToken();
					this->QuestInfo[iQuestIndex].QuestStartType = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					this->QuestInfo[iQuestIndex].QuestStartSubType = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					strcpy( this->QuestInfo[iQuestIndex].Name, WzMemScript.GetString());

					this->QuestInfo[iQuestIndex].QuestSubInfoCount = 0;

					while ( true )
					{
						int iSubInfoCount = this->QuestInfo[iQuestIndex].QuestSubInfoCount;

						Token = WzMemScript.GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", WzMemScript.GetString()) == 0 )
							{
								break;
							}
						}
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].QuestType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedSubType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ItemLevel = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedNumber = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMinLevel = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMaxLevel = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedDropRate = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardSubType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardCount = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].LinkConditionIndex = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextBeforeReg = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextAfterReg = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextCompleteQuest = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextQuestClear = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[0] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[1] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[2] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[3] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[4] = WzMemScript.GetNumber();

						this->QuestInfo[iQuestIndex].QuestSubInfoCount++;

					
					}
					
					LogAdd("[Quest] (%s) Load : Index %d, SubInfoCount %d",
						this->QuestInfo[iQuestIndex].Name, iQuestIndex,
						this->QuestInfo[iQuestIndex].QuestSubInfoCount );
						this->m_QuestCount++;
				}
				else if ( Type == 1 )
				{
					Token = WzMemScript.GetToken();
					auto int iQuestIndex = WzMemScript.GetNumber();

					while ( true )
					{
						auto int iFailInfoCount = this->QuestInfo[iQuestIndex].QuestConditionCount;
						Token = WzMemScript.GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", WzMemScript.GetString()) == 0 )
							{
								break;
							}
						}	
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].Index = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedQuestIndex = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MinLevel = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MaxLevel = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].ReqStr = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedZen = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].StartContext = WzMemScript.GetNumber();

							this->QuestInfo[iQuestIndex].QuestConditionCount++;
						
					}
				}
				else
				{
					MsgBox("Quest Info Read Fail");
					fclose(SMDFile);
					return false;
				}
			}
		}
	}

	this->InitQuestItem();
	return true;
}


void CQuestInfo::InitQuestItem()
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;

	for (int i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		for ( int n=0;n<lpQuestInfo->QuestSubInfoCount;n++)
		{
			lpSubInfo = &lpQuestInfo->QuestSubInfo[n];

			if ( lpSubInfo != NULL )
			{
				if ( lpSubInfo->QuestType == 1 )
				{
					LPITEM_ATTRIBUTE p = &ItemAttribute[ITEMGET(lpSubInfo->NeedType, lpSubInfo->NeedSubType) ];
					p->QuestItem = TRUE;
					LogAddTD("[Quest] [SetQuestItem] %s", p->Name);
				}
			}
		}
		foundquest++;
		if ( foundquest == questcount )
		{
			return;
		}
	}
}


BOOL CQuestInfo::IsQuest(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	if ( this->QuestInfo[QuestIndex].QuestIndex == -1 )
	{
		return false;
	}

	return true;
}


					
int CQuestInfo::GetQuestState(LPOBJ lpObj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	int index = (QuestIndex) /4;
	int shift = (QuestIndex % 4) *2;
	return (lpObj->m_Quest[index] >> shift) &3;
}


BYTE CQuestInfo::GetQuestStateBYTE(LPOBJ lpObj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}
	
	int index = (QuestIndex / 4);
	return lpObj->m_Quest[index];
}


BYTE CQuestInfo::SetQuestState(LPOBJ lpObj, int QuestIndex, int State)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return -1;
	}

	int QuestState = this->GetQuestState(lpObj, QuestIndex);

	if ( QuestState == 3 || QuestState == 0)
	{
		BYTE btCond = this->QuestRunConditionCheck(lpObj, QuestIndex);

		if ( btCond != 0 )
		{
			return btCond;
		}

		this->QuestAccept(lpObj, QuestIndex);
		LogAddTD("[Quest] AcceptQuest [%s][%s] (%s)", lpObj->AccountID, lpObj->Name,
			this->GetQuestInfo(QuestIndex)->Name);
		State = 1;
	}
	else if ( QuestState == 1 )
	{
		BYTE btCond = this->QuestClearConditionCheck(lpObj, QuestIndex);

		if ( btCond != 0 )
		{
			return btCond;
		}

		this->QuestClear(lpObj, QuestIndex);
		State = 2;
		LogAddTD("[Quest] ClearQuest [%s][%s] (%s)", lpObj->AccountID, lpObj->Name,
			this->GetQuestInfo(QuestIndex)->Name);
	}
	else
	{
		return -1;
	}

	int index = (QuestIndex / 4);
	int shift =  (QuestIndex % 4) *2;
	lpObj->m_Quest[index] &= QuestBitMask[shift];
	lpObj->m_Quest[index] |= (State&3) << shift;
	return 0;
}

BYTE CQuestInfo::ReSetQuestState(LPOBJ lpObj, int QuestIndex)
{
	int State = 3;
	int index = (QuestIndex/4);
	int shift = (QuestIndex % 4)*2;
	lpObj->m_Quest[index] &= QuestBitMask[shift];
	lpObj->m_Quest[index] |= (State&3) << shift;

	return 0;
}


LPQUEST_INFO CQuestInfo::GetQuestInfo(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}
	
	if ( this->QuestInfo[QuestIndex].QuestIndex == -1 )
	{
		return NULL;
	}

	return &this->QuestInfo[QuestIndex];
}

LPQUEST_SUB_INFO CQuestInfo::GetSubquestInfo(LPOBJ lpObj, LPQUEST_INFO lpQuestInfo, int subquestindex)
{
	if ( lpQuestInfo == NULL )
	{
		return NULL;
	}

	if ( subquestindex < 0 || subquestindex > MAX_SUBINFO_QUEST )
	{
		return NULL;
	}

	LPQUEST_SUB_INFO lpSubInfo = &lpQuestInfo->QuestSubInfo[subquestindex];
	int Class = lpObj->Class;
	int ChangeUP = lpObj->ChangeUP;
	int requireclass = lpSubInfo->RequireClass[Class];

	if ( requireclass == 0 )
	{
		return NULL;
	}

	if ( requireclass > 1 )
	{
		if ( requireclass > (ChangeUP + 1) )
		{
			return NULL;
		}
	}

	return lpSubInfo;
}

LPQUEST_CONDITION CQuestInfo::GetQuestCondition(LPQUEST_INFO lpQuestInfo, int conditionindex)
{
	if ( lpQuestInfo == NULL )
	{
		return NULL;
	}

	if ( conditionindex < 0 || conditionindex > MAX_CONDITION_QUEST )
	{
		return NULL;
	}

	return &lpQuestInfo->QuestCondition[conditionindex];
}

BYTE CQuestInfo::QuestClearConditionCheck(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return -1;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	BOOL bFoundSubQuest = FALSE;
	

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			bFoundSubQuest = TRUE;

			if ( lpSubInfo->QuestType == 1 )
			{
				int NumberItemQuestFound = gObjGetItemCountInIventory(lpObj->m_Index, lpSubInfo->NeedType, lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);

				if ( NumberItemQuestFound < lpSubInfo->NeedNumber)
				{
					return 1;
				}
			}
			else if ( lpSubInfo->QuestType == 2)
			{
				int NumberMonsterKillCount = this->GetQuestKillCount(lpObj, lpSubInfo->NeedType);

				if ( NumberMonsterKillCount < lpSubInfo->NeedNumber)
				{
					return 1;
				}
			}
		}
	}

	if ( bFoundSubQuest == FALSE )
	{
		return -1;
	}

	return 0;
}

int CQuestInfo::GetQuestKillCount(LPOBJ lpObj, int uClass)
{
	if (lpObj->m_Quest[1] == 0xF6 || lpObj->m_Quest[1] == 0xDA)
	{
		return lpObj->m_Quest[(uClass%100)-5];
	}
	else
	{
		return 0;
	}
}

BYTE CQuestInfo::QuestRunConditionCheck(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return -1;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	LPQUEST_CONDITION lpCondition;
	BOOL bFoundSubQuest = FALSE;


	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			bFoundSubQuest = TRUE;

			for ( int i=0;i<concount;i++)
			{
				lpCondition = this->GetQuestCondition(lpQuestInfo, i);

				if ( lpCondition != NULL && lpCondition->Index == -1)
				{
					if ( this->CompareCondition(lpObj, lpCondition) == FALSE )
					{
						return lpCondition->StartContext;
					}
				}
				else if ( lpCondition != NULL && lpCondition->Index == lpSubInfo->LinkConditionIndex )
				{
					if ( this->CompareCondition(lpObj, lpCondition) == 0)
					{
						return lpCondition->StartContext;
					}
				}
				else if ( lpCondition == NULL )
				{
					return -1;
				}
			}

		}
	}

	if ( bFoundSubQuest == FALSE )
	{
		return -1;
	}

	return 0;
}

BOOL CQuestInfo::CompareCondition(LPOBJ lpObj, LPQUEST_CONDITION lpCondition)
{
	if ( lpCondition->NeedQuestIndex != -1 )
	{
		if ( this->GetQuestState(lpObj, lpCondition->NeedQuestIndex) != 2 )
		{
			return FALSE;
		}
	}

	if ( lpCondition->MinLevel != 0 )
	{
		if ( lpCondition->MinLevel > lpObj->Level )
		{
			return FALSE;
		}
	}

	if ( lpCondition->MaxLevel != 0 )
	{
		if ( lpCondition->MaxLevel < lpObj->Level )
		{
			return FALSE;
		}
	}

	if ( lpCondition->NeedZen > lpObj->Money )
	{
		return FALSE;
	}

	if ( lpCondition->ReqStr > lpObj->Strength + lpObj->AddStrength)
	{
		return FALSE;
	}

	return TRUE;

}




BOOL CQuestInfo::NpcTalk(LPOBJ lpNpc, LPOBJ lpObj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	int queststate = -1;
	int questindex = -1;

	for ( int i = 0; i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		if ( lpQuestInfo->QuestStartType == 2 ) // NPC Click
		{
			if ( lpNpc->Class == lpQuestInfo->QuestStartSubType)
			{
				if ( queststate == -1 )
				{
					questindex = lpQuestInfo->QuestIndex;
					queststate = this->GetQuestState(lpObj, lpQuestInfo->QuestIndex);
				}
				else if ( queststate == 2 )
				{
					for ( int n=0;n<lpQuestInfo->QuestSubInfoCount ;n++)
					{
						LPQUEST_SUB_INFO lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, n);

						if ( lpSubInfo != NULL )
						{
							questindex = lpQuestInfo->QuestIndex;
							queststate = this->GetQuestState(lpObj, lpQuestInfo->QuestIndex);
						}
					}
				}
			}
		}

		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	if ( queststate != -1 )
	{
		GCSendQuestInfo(lpObj->m_Index, questindex);
		lpObj->m_IfState.type = 10;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		return true;
	}

	return false;
}



BOOL CQuestInfo::MonsterItemDrop(LPOBJ lpObj)
{
	int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

	if ( MaxHitUser == -1 )
	{
		return false;
	}

	int partycount = gParty.GetPartyCount(gObj[MaxHitUser].PartyNumber);

	if ( partycount > 0 )
	{
		return false;
	}

	LPOBJ lpTarget = &gObj[MaxHitUser];
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int type;
	int level;
	int x;
	int y;
	float dur = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	

	for ( int i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		for ( int n =0;n<lpQuestInfo->QuestSubInfoCount;n++)
		{
			lpSubInfo = this->GetSubquestInfo(lpTarget, lpQuestInfo, n);
			
			if ( lpSubInfo != NULL )
			{
				if ( lpSubInfo->QuestType == 1 && lpSubInfo->NeedTargetMinLevel != -1 ) //Third Quest Fix, only READS Monster Level Min and Max if Level Min is != 1
				{
					if ( lpObj->Level >= lpSubInfo->NeedTargetMinLevel)
					{
						if ( lpObj->Level <= lpSubInfo->NeedTargetMaxLevel )
						{
							if ( this->GetQuestState(lpTarget, lpQuestInfo->QuestIndex) == TRUE )
							{
								if ( (rand() % ITEM_QUEST_DROP_PROBABILITY) < lpSubInfo->NeedDropRate)
								{
									int itemcount = gObjGetItemCountInIventory(MaxHitUser, lpSubInfo->NeedType,
										lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);

									if ( itemcount >= lpSubInfo->NeedNumber)
									{
										continue;
									}

									dur = 0;
									x = lpObj->X;
									y = lpObj->Y;
									level = lpSubInfo->ItemLevel;
									type = ItemGetNumberMake(lpSubInfo->NeedType, lpSubInfo->NeedSubType);
									ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur, Option1,
										Option2, Option3, MaxHitUser, 0, 0);
									LogAddTD("[Quest] Quest Item Drop [%s]: [%s][%s] (%s) (%d,%d)", lpObj->Name,
										lpTarget->AccountID, lpTarget->Name, lpQuestInfo->Name, lpSubInfo->NeedType,
										lpSubInfo->NeedSubType);
									qMsg->Msg(lpObj->m_Index, "@[Quest] Your item droped.");
									return true;
								}
							}
						}
					}
				}
				//Start of S3 Quest
				if ( lpSubInfo->QuestType == 1 && lpSubInfo->NeedTargetMinLevel == -1 ) //Third Quest
				{
					if ( lpObj->Class == lpSubInfo->NeedTargetMaxLevel) //READS Monster Class on TargetMax Level Category (for Original Quest.txt)
					{
						if ( this->GetQuestState(lpTarget, lpQuestInfo->QuestIndex) == TRUE )
							{
								if ( (rand() % ITEM_QUEST_DROP_PROBABILITY) < lpSubInfo->NeedDropRate)
								{
									int itemcount = gObjGetItemCountInIventory(MaxHitUser, lpSubInfo->NeedType,
										lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);

									if ( itemcount >= lpSubInfo->NeedNumber)
									{
										continue;
									}

									dur = 0;
									x = lpObj->X;
									y = lpObj->Y;
									level = lpSubInfo->ItemLevel;
									type = ItemGetNumberMake(lpSubInfo->NeedType, lpSubInfo->NeedSubType);
									ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur, Option1,
										Option2, Option3, MaxHitUser, 0, 0);
									LogAddTD("[Season3 Quest] Quest Item Drop [%s]: [%s][%s] (%s) (%d,%d)", lpObj->Name,
										lpTarget->AccountID, lpTarget->Name, lpQuestInfo->Name, lpSubInfo->NeedType,
										lpSubInfo->NeedSubType);
									return true;
								}
							}
					}
				}
				if ( lpSubInfo->QuestType == 2 && lpSubInfo->NeedTargetMinLevel == -1 ) //Third Quest for Kill Count
				{
					if ( lpObj->Class == lpSubInfo->NeedTargetMaxLevel) //READS Monster Class on TargetMax Level Category (for Original Quest.txt)
					{
						if ( this->GetQuestState(lpTarget, lpQuestInfo->QuestIndex) == TRUE )
						{
							if( lpTarget->m_Quest[1] == 0xF6 && lpTarget->MapNumber == 41 )
							{
								if( lpTarget->m_Quest[30] > 10)
								{
									lpTarget->m_Quest[30] = 0;
								}
								if( lpTarget->m_Quest[31] > 10 )
								{
									lpTarget->m_Quest[31] = 0;
								}
								if( lpTarget->m_Quest[32] > 10 )
								{
									lpTarget->m_Quest[32] = 0;
								}

								if(lpObj->Class == 409 && lpTarget->m_Quest[30] < 10)
								{
									lpTarget->m_Quest[30] += 1;
									MsgOutput(lpTarget->m_Index, "[Quest] (%d/10) Balram(Hero)", lpTarget->m_Quest[30]);
								}

								if(lpObj->Class == 410 && lpTarget->m_Quest[31] < 10)
								{
									lpTarget->m_Quest[31] += 1;
									MsgOutput(lpTarget->m_Index, "[Quest] (%d/10) Death Spirit(Hero)", lpTarget->m_Quest[31]);
								}

								if(lpObj->Class == 411 && lpTarget->m_Quest[32] < 10)
								{
									lpTarget->m_Quest[32] += 1;
									lpTarget->m_Quest[34] += 1;
									MsgOutput(lpTarget->m_Index, "[Quest] (%d/10) Soram(Hero).", lpTarget->m_Quest[32]);
								}
							}
							if( lpTarget->m_Quest[1] == 0xDA && lpTarget->MapNumber == 42 )
							{
								if( lpTarget->m_Quest[34] > 1 )
								{
									lpTarget->m_Quest[34] = 0;
								}
									
								if(lpObj->Class == 412 && lpTarget->m_Quest[34] < 1)
								{
									lpTarget->m_Quest[34] += 1;
									MsgOutput(lpTarget->m_Index, "[Quest] (%d/1) Dark Elf(Hero)", lpTarget->m_Quest[34]);
								}
							}
						//End of S3 Quest
						}
					}
				}
			//
			}
		}

		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	return false;
}

BOOL CQuestInfo::QuestAccept(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return 0xFF;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	LPQUEST_CONDITION lpCondition;
	int NeedZen = 0;

	if ( QuestIndex > 3 && QuestIndex < 6 ) //Third Quest Fix
	{
		//Begin
		if( lpQuestInfo->QuestSubInfoCount > 0 )
		{
			lpSubInfo = lpQuestInfo->QuestSubInfo;

			if ( lpSubInfo != NULL )
			{
				for ( int n = 0;n<concount;n++)
				{
					lpCondition = this->GetQuestCondition(lpQuestInfo, n);
					NeedZen += lpCondition->NeedZen;
				}
			}
		}
		//End
	}
	else
	{
		//Begin
		for ( int subquest=0;subquest<subquestcount;subquest++)
		{
			lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

			if ( lpSubInfo != NULL )
			{
				for ( int n = 0;n<concount;n++)
				{
					lpCondition = this->GetQuestCondition(lpQuestInfo, n);
					NeedZen += lpCondition->NeedZen;
				}
			}
		}
		//End
	}
	
	lpObj->Money -= NeedZen;
	GCMoneySend(lpObj->m_Index, lpObj->Money);
	return true;
}

BOOL CQuestInfo::QuestClear(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

#if (FOREIGN_GAMESERVER==1)
	if ( szAuthKey[18] != AUTHKEY18 )
	{
		DestroyGIocp();
	}
#endif

	if ( lpQuestInfo == NULL )
	{
		return 0xFF;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			if ( lpSubInfo->RewardType == QUEST_COMPENSATION_CHANGEUP )
			{
				lpObj->LevelUpPoint += lpSubInfo->RewardCount;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );
				lpObj->ChangeUP = true;
				lpObj->DbClass |= 1;
				gObjMakePreviewCharSet(lpObj->m_Index);
				BYTE btClass = (lpObj->Class * 32) & 224 ;
				btClass |= (lpObj->ChangeUP * 16) & 16;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_CHANGEUP, btClass);
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), ChangeUp",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					lpSubInfo->RewardCount );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_STATUP )
			{
				lpObj->LevelUpPoint += lpSubInfo->RewardCount;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d)",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					lpSubInfo->RewardCount );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_PLUSSSTAT )
			{
				int level = lpObj->Level - QUEST_MINLEVEL_PLUSSTAT;

				if ( level <= 0 )
				{
					level = 0;
				}

				lpObj->LevelUpPoint += level;
				lpObj->PlusStatQuestClear = true;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_PLUSSSTAT, level );
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), PlusStat",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					level );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_COMBOSKILL )
			{
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_COMBOSKILL, 0 );
				lpObj->ComboSkillquestClear = true;
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] ComboSkill",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name);
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_CHANGEUP2 )
			{
				lpObj->LevelUpPoint += lpSubInfo->RewardCount;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount);
				lpObj->ChangeUP = 2;
				BYTE btClass;
				btClass = (lpObj->Class * 32) + 24;

				if (lpObj->DbClass != 2 || lpObj->DbClass != 18 || lpObj->DbClass != 34 || lpObj->DbClass != 50 || lpObj->DbClass != 66 ) //HermeX Great Anti-Hack :P
				{
					if (lpObj->DbClass == 48 || lpObj->DbClass == 64)
					{
						lpObj->DbClass += 2;
					}
					else
					{
						lpObj->DbClass += 1;
					}
				}
				gObjMakePreviewCharSet(lpObj->m_Index);
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_CHANGEUP2, btClass);
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), ChangeUp2",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					lpSubInfo->RewardCount );
			}

			if ( lpSubInfo->QuestType == 1 )
			{
				gObjDelteItemCountInInventory(lpObj->m_Index, lpSubInfo->NeedType, lpSubInfo->NeedSubType,
					lpSubInfo->NeedNumber);
			}
		}
	}
	return true;
}


void CQuestInfo::QuestInfoSave(LPOBJ lpObj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;

	for ( int i = 0 ; i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		LogAddTD("[Quest] QuestSave [%s][%s] index(%d) name(%s) state(%s)",
			lpObj->AccountID, lpObj->Name, i, lpQuestInfo->Name,
			sQuestString[this->GetQuestState(lpObj, i)]);
		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}
}