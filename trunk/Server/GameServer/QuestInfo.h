// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef QUESTINFO_H
#define QUESTINFO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\classdef.h"
#include "user.h"

#define MAX_QUEST_INFO 200
#define MAX_SUBINFO_QUEST 10
#define MAX_CONDITION_QUEST 10
#define ITEM_QUEST_DROP_PROBABILITY 1000
#define QUEST_COMPENSATION_CHANGEUP 201
#define QUEST_COMPENSATION_STATUP 200
#define QUEST_COMPENSATION_PLUSSSTAT 202
#define QUEST_COMPENSATION_COMBOSKILL 203
#define QUEST_COMPENSATION_CHANGEUP2 204
#define QUEST_MINLEVEL_PLUSSTAT 220
#define QUEST_STATE_SIZE 50

typedef struct QUEST_SUB_INFO
{
	int QuestType;	// 0
	int NeedType;	// 4
	int NeedSubType;	// 8
	int ItemLevel;	// C
	int NeedNumber;	// 10
	int NeedTargetMinLevel;	// 14
	int NeedTargetMaxLevel;	// 18
	int NeedDropRate;	// 1C
	int RewardType;	// 20
	int RewardSubType;	// 24
	int RewardCount;	// 28
	int LinkConditionIndex;	// 2C
	int ContextBeforeReg;	// 30
	int ContextAfterReg;	// 34
	int ContextCompleteQuest;	// 38
	int ContextQuestClear;	// 3C
	BYTE RequireClass[MAX_TYPE_PLAYER];	// 40

} QUEST_SUB_INFO, * LPQUEST_SUB_INFO;


typedef struct QUEST_CONDITION
{
	int Index;	// 0
	int NeedQuestIndex;	// 4
	int MinLevel;	// 8
	int MaxLevel;	// C
	int ReqStr;	// 10
	int NeedZen;	// 14
	int StartContext;	// 18

/*	int iConditionalIndex;
	int iQuestIndex;
	int iLevelMin;
	int iLevelMax;
	int iReqStrength;
	int iReqZen;
	int iStartContextTextNumber;*/

} QUEST_CONDITION, * LPQUEST_CONDITION;


typedef struct QUEST_INFO	
{
	char Name[50];	// 0
	int QuestIndex;	// 34
	int QuestStartType;	// 38
	int QuestStartSubType;	// 3C
	int QuestSubInfoCount;	// 40
	QUEST_SUB_INFO QuestSubInfo[MAX_SUBINFO_QUEST];	// 44
	int QuestConditionCount;	// 314
	QUEST_CONDITION QuestCondition[MAX_CONDITION_QUEST];	// 318

} QUEST_INFO, * LPQUEST_INFO;

class CQuestInfo
{
public:

	CQuestInfo();
	virtual ~CQuestInfo();

	void Init();
	int IsQuest(int QuestIndex);
	int GetQeustCount(){return m_QuestCount;}
	int LoadQuestInfo(char* Buffer, int iSize);
	int LoadQuestInfo(char* filename);
	void InitQuestItem();
	LPQUEST_INFO GetQuestInfo(int QuestIndex);
	LPQUEST_SUB_INFO GetSubquestInfo(LPOBJ lpObj, LPQUEST_INFO lpQuestInfo, int subquestindex);
	LPQUEST_CONDITION GetQuestCondition(LPQUEST_INFO lpQuestInfo, int conditionindex);
	int GetQuestState(LPOBJ lpObj, int QuestIndex);
	BYTE GetQuestStateBYTE(LPOBJ lpObj, int QuestIndex);
	unsigned char SetQuestState(LPOBJ lpObj, int QuestIndex, int State);
	unsigned char ReSetQuestState(LPOBJ lpObj, int QuestIndex);
	unsigned char QuestClearConditionCheck(LPOBJ lpObj, int QuestIndex);
	unsigned char QuestRunConditionCheck(LPOBJ lpObj, int QuestIndex);
	int CompareCondition(LPOBJ lpObj, LPQUEST_CONDITION lpCondition);
	int NpcTalk(LPOBJ lpNpc, LPOBJ lpObj);
	int MonsterItemDrop(LPOBJ lpObj);
	int QuestAccept(LPOBJ lpObj, int QuestIndex);
	int QuestClear(LPOBJ lpObj, int QuestIndex);
	void QuestInfoSave(LPOBJ lpObj);
	int GetQuestKillCount(LPOBJ lpObj, int uClass);


private:

	int m_QuestCount;	// 4
	QUEST_INFO QuestInfo[MAX_QUEST_INFO];	// 8

};


extern CQuestInfo g_QuestInfo;

#endif