#include "Stdafx.h"
#include "QuestInfo.h"
#include "GameMain.h"
#include "LogProc.h"
#include "xml.h"
#include "Messages.h"
#include "gObjMonster.h"
#include "DsProtocol.h"

Messages * qMsg = new Messages();

char sQuestString[4][50] = { "START", "ON_GOING", "FINSH", "UNKNOW"};	
char szQuestTemp[300] = {0};

CQuest::CQuest()
{ 
	this->Init();
}

void CQuest::Init()
{
	this->questCount = 0;
	memset(this->quests, -1, sizeof(this->quests));

	for ( int i=0;i<MAX_QUESTS;i++)
	{
		this->quests[i].QuestsObjectCount = 0;
		this->questCount = 0;
		this->quests[i].QuestKilledCount = 0;
		for(int sub = 0 ; sub < MAX_SUB_QUEST ; sub++)
		{
			this->quests[i].ItemsObject[sub].QuestPrizeCount = 0;
			this->quests[i].ItemsObject[sub].QuestMobCount = 0;
		}
	}
}

bool CQuest::LoadFile(char * szFileName)
{
	xmlDocPtr hQuestFile = xmlParseFile(szFileName);

	if ( hQuestFile != NULL )
	{
		xmlNodePtr nodeRoot = NULL;
		nodeRoot = xmlDocGetRootElement(hQuestFile);

		if( xmlStrcmp(nodeRoot->name, (const xmlChar*)"quests") == 0 )
		{
			xmlNodePtr NodeChild = nodeRoot->children;
			xmlNodePtr SubNode;
			xmlNodePtr xSubNode;

			while(NodeChild)
			{
				if(xmlStrcmp(NodeChild->name, (const xmlChar*)"quest") == 0)
				{
					this->questCount++;
					int Index = xmlReadInt(NodeChild,"index");

					this->quests[Index].index = Index;
					
					this->quests[Index].level = xmlReadInt(NodeChild,"level");

					xmlReadString( NodeChild, "name", this->quests[Index].name, MAX_QUEST_NAME);
					this->quests[Index].npcID = xmlReadInt( NodeChild , "npc" );

					this->quests[Index].zen = xmlReadInt( NodeChild , "zen" );

					this->quests[Index].dw = xmlReadInt( NodeChild , "dw" );

					this->quests[Index].dk = xmlReadInt( NodeChild , "dk" );

					this->quests[Index].elf = xmlReadInt( NodeChild , "elf" );

					this->quests[Index].dl = xmlReadInt( NodeChild , "dl" );

					this->quests[Index].mg = xmlReadInt( NodeChild , "mg" );

					SubNode = NodeChild->children;
					
					while(SubNode)
					{
						if(xmlStrcmp(SubNode->name, (const xmlChar*)"item") == 0)
						{
							this->quests[Index].QuestsObjectCount++;

							int subIndex = xmlReadInt(SubNode,"index");
							this->quests[Index].ItemsObject[subIndex].index = subIndex;
							
							this->quests[Index].ItemsObject[subIndex].itemType = xmlReadInt(SubNode,"itemType");
	
							this->quests[Index].ItemsObject[subIndex].itemSubType = xmlReadInt(SubNode,"itemSubType");

							this->quests[Index].ItemsObject[subIndex].itemCount = xmlReadInt(SubNode,"count");

							xSubNode = SubNode->children;

							while(xSubNode)
							{
								if(xmlStrcmp(xSubNode->name, (const xmlChar*)"mob") == 0)
								{
									this->quests[Index].ItemsObject[subIndex].QuestMobCount++;

									int xSub = xmlReadInt(xSubNode,"index");
									this->quests[Index].ItemsObject[subIndex].QuestMob[xSub].index = xSub;

									this->quests[Index].ItemsObject[subIndex].QuestMob[xSub].minLevel = xmlReadInt(xSubNode, "minLevel");

									this->quests[Index].ItemsObject[subIndex].QuestMob[xSub].maxLevel = xmlReadInt(xSubNode, "maxLevel");

									this->quests[Index].ItemsObject[subIndex].QuestMob[xSub].count = xmlReadInt(xSubNode, "count");
									
								}
								if(xmlStrcmp(xSubNode->name, (const xmlChar*)"prize") == 0)
								{
								
									this->quests[Index].ItemsObject[subIndex].QuestPrizeCount++;

									int pSub = xmlReadInt(xSubNode,"index");

									this->quests[Index].ItemsObject[subIndex].QuestPrize[pSub].index = pSub;

									this->quests[Index].ItemsObject[subIndex].QuestPrize[pSub].points = xmlReadInt(xSubNode, "points");


									this->quests[Index].ItemsObject[subIndex].QuestPrize[pSub].ClassUp = xmlReadInt(xSubNode, "ClassUp");

									this->quests[Index].ItemsObject[subIndex].QuestPrize[pSub].itemCount = xmlReadInt(xSubNode, "ItemCount");

									this->quests[Index].ItemsObject[subIndex].QuestPrize[pSub].itemType = xmlReadInt(xSubNode, "ItemType");

									this->quests[Index].ItemsObject[subIndex].QuestPrize[pSub].itemSubType = xmlReadInt(xSubNode, "itemSubType");

								}
								xSubNode = xSubNode->next;
							}
						}
						SubNode = SubNode->next;
					}
				}

				NodeChild = NodeChild->next;
			}
		}
		else
		{
			MsgBox("[Quests.xml] :: File is corrupted.");
			return false;
		}
	}
	else
	{
		MsgBox("[Quests.xml]  :: File is missing.");
		return false;
	}
	return true;
}

bool CQuest::IsQuest(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUESTS )
	{
		return false;
	}
	
	if ( this->quests[QuestIndex].index == -1 )
	{
		return false;
	}

	return true;
}

int CQuest::GetQuestState(LPOBJ lpObj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUESTS )
	{
		return false;
	}

	/*int index = (QuestIndex) /4;
	int shift = (QuestIndex % 4) *2;
	return (lpObj->m_Quest[index] >> shift) &3;*/
	return lpObj->q_QuestState;
}


BYTE CQuest::ReSetQuestState(LPOBJ lpObj, int QuestIndex)
{

	return 0;
}

void CQuest::QuestInfoSave(LPOBJ lpObj)
{
	int questcount = this->GetQeustCount();
	for ( int i = 0 ; i<questcount;i++)
	{
			LogAddTD("[Quest] QuestSave [%s][%s] index(%d) name(%s) state(%s)",
			lpObj->AccountID, lpObj->Name, i, this->quests[i].name,
			sQuestString[this->GetQuestState(lpObj, i)]);
	}
}

bool CQuest::SetQuestState(LPOBJ lpObj, int QuestIndex, int State)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUESTS )
	{
		return false;
	}
	lpObj->q_QuestIndex = QuestIndex;
	lpObj->q_QuestState = State;

	return true;
}

Q_Types CQuest::GetQuestType(int QuestIndex , int SubIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUESTS )
	{
		return QUEST_OBJECT_INVAILD;
	}

	for ( int i = 0 ; i != this->questCount ; i++)
	{
		if( i == QuestIndex )
		{
			for(int n = 0 ; n != this->quests[QuestIndex].QuestsObjectCount ; n++)
			{
				if(this->quests[i].ItemsObject[n].index == SubIndex)
					return QUEST_OBJECT_TYPE_ITEM; 
				if(this->quests[i].MobsObject[n].index == SubIndex)
					return QUEST_OBJECT_TYPE_MONSTER; 
			}
		}
	}
	return QUEST_OBJECT_INVAILD;

}
bool CQuest::IsClass(int QuestIndex,int Class)
{
	QuestAtt * lpQuest;
	lpQuest = &this->quests[QuestIndex];

	
	if( lpQuest != NULL )
	{
	switch (Class)
	{
		case 0:
			if(lpQuest->dw == 1)
				return true;
		case 256:
			if(lpQuest->dw == 2)
				return true;
		case 16:
			if(lpQuest->dk == 1)
				return true;
		case 273:
			if(lpQuest->dk == 2)
				return true;
		case 32:
			if(lpQuest->elf == 1)
				return true;
		case 289:
			if(lpQuest->elf == 2)
				return true;
		case 48:
			if(lpQuest->mg == 1)
				return true;
		case 64:
			if(lpQuest->dl == 1)
				return true;
		}
	}

	return false;

}

bool CQuest::NpcTalk(LPOBJ lpNpc, LPOBJ lpObj)
{
	int iIndex = lpObj->m_Index;
	if( lpObj->q_QuestIndex == 0xFF)
	{
		this->SetQuestState(lpObj, 0 , QUEST_STATE_START);
	}
	int i = lpObj->q_QuestIndex;

	if(this->quests[i].npcID == lpNpc->Class)
	{

		if(lpObj->Level < this->quests[i].level)
		{
			wsprintf(szQuestTemp, " You're need be level %d for quest!",this->quests[i].level);
			ChatTargetSend(lpNpc,szQuestTemp, iIndex);
			return true;
		}
		if(lpObj->Money < this->quests[i].zen)
		{
			wsprintf(szQuestTemp, " you need zen %d for quest!",this->quests[i].zen);
			ChatTargetSend(lpNpc,szQuestTemp, iIndex);
			return true;
		}
		if(this->IsClass(i, lpObj->DbClass ))
		{
			for(int x = 0 ; x != this->quests->QuestsObjectCount ; x++)
			{
				switch(this->GetQuestType(i,x))
				{
					case QUEST_OBJECT_TYPE_ITEM:
					{
						switch(lpObj->q_QuestState)
						{
							case QUEST_STATE_START:
							{
								int ItemType = this->quests[i].ItemsObject[x].itemType;
								int ItemSubType = this->quests[i].ItemsObject[x].itemSubType;
								LPITEM_ATTRIBUTE Item = &ItemAttribute[ITEMGET(ItemType, ItemSubType) ];
								qMsg->Notice(iIndex , "[Quest] Quest Start. " );
								qMsg->Notice(iIndex , "[Quest] %s go find %s !!!",this->quests[i].name,Item->Name);
								this->SetQuestState(lpObj, i , QUEST_STATE_ONGOING );
								if(this->quests[i].zen > 0)
								{
									lpObj->Money -= this->quests[i].zen;
									GCMoneySend( iIndex , lpObj->Money );
								}
								qMsg->PM( iIndex , "[Quest] Remmber you can do quest only without party");
								qMsg->PM( iIndex , "[Quest] you need free slot in your invenotry for quest item");
								return true;
							}
							case QUEST_STATE_ONGOING:
							{
								int ItemType = this->quests[i].ItemsObject[x].itemType;
								int ItemSubType = this->quests[i].ItemsObject[x].itemSubType;
								int GetCount = this->quests[i].ItemsObject[x].itemCount;

								LPITEM_ATTRIBUTE Item = &ItemAttribute[ITEMGET(ItemType, ItemSubType) ];
								qMsg->PM(iIndex , "[Quest] You do not have enough required items");
								qMsg->PM(iIndex , "[Quest] Search %d %s",GetCount,Item->Name);
								wsprintf(szQuestTemp, "Search %d %s",GetCount,Item->Name);
								ChatTargetSend(lpNpc , szQuestTemp , iIndex );
								return true;

							}
							case QUEST_STATE_FINISH:
							{

								int ItemType = this->quests[i].ItemsObject[x].itemType;

								int ItemSubType = this->quests[i].ItemsObject[x].itemSubType;

								int Count = gObjGetItemCountInIventory(iIndex, ITEMGET(ItemType,ItemSubType));

								int GetCount = this->quests[i].ItemsObject[x].itemCount;

								if(Count >= GetCount)
								{
									LPITEM_ATTRIBUTE Item = &ItemAttribute[ITEMGET(ItemType, ItemSubType) ];
									qMsg->Notice(iIndex, "[Quest] You have been finished %s ",this->quests[i].name);
									gObjDelteItemCountInInventory(iIndex ,ItemType , ItemSubType , Count );
									int PrizeCount = this->quests[i].ItemsObject[x].QuestPrizeCount;
									int uPoint = 0;
									int uClassUp = 0;
									for(int p = 0 ; p != PrizeCount ; p++)
									{
										int Points = this->quests[i].ItemsObject[x].QuestPrize[p].points;
										int ClassUp = this->quests[i].ItemsObject[x].QuestPrize[p].ClassUp;
										if(Points > 0)
											uPoint += Points;
										if(ClassUp > 0 )
											uClassUp += ClassUp;
													
									}
									if(uPoint > 0)
									{
										qMsg->Msg(iIndex , "[Quest] you have received %d points",uPoint);
										lpObj->LevelUpPoint += uPoint;
										GCLevelUpMsgSend(iIndex, 201);
										this->SetQuestState(lpObj, lpObj->q_QuestIndex+1 , QUEST_STATE_START);
									}
									if(uClassUp > 0)
									{
										qMsg->PM(iIndex , "[Quest] congratulation you win Class up :)");
										lpObj->ChangeUP += 1;
										lpObj->DbClass |= 1;
										gObjMakePreviewCharSet(lpObj->m_Index);
										BYTE btClass = (lpObj->Class * 32) & 224 ;
										btClass |= (lpObj->ChangeUP * 16) & 16;
										if(lpObj->ChangeUP == 1)
											GCSendQuestPrize(lpObj->m_Index, 201 , btClass);
										if(lpObj->ChangeUP == 2)
											GCSendQuestPrize(lpObj->m_Index, 204 , btClass);
										GCMagicAttackNumberSend(lpObj, 76 , lpObj->m_Index , 0 );
									}
									
								}
							}
						}
						break;
					}
				}
			}	
		}
	}
	
			

	return false;
}

bool CQuest::MonsterItemDrop(LPOBJ lpObj)
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
	int type;
	int level = 0;
	int x;
	int y;
	float dur = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	LPOBJ lpTarget = &gObj[MaxHitUser];

	int qIndex = lpTarget->q_QuestIndex;

	if(lpTarget->q_QuestState != QUEST_STATE_ONGOING)
	{
		return false;
	}
	QuestAtt * lpQuest = &this->quests[qIndex];

	if ( lpQuest != NULL )
	{
		for(int i = 0 ; i != lpQuest->QuestsObjectCount; i++)
		{
			QuestObjectItem * lpQitem = &lpQuest->ItemsObject[i];
			for(int n = 0 ; n != lpQitem->QuestMobCount; n++)
			{
				if(lpQuest->QuestKilledCount >= lpQitem->QuestMob[n].count)
				{
					dur = 0;
					x = lpObj->X;
					y = lpObj->Y;
					type = ItemGetNumberMake(lpQitem->itemType,lpQitem->itemSubType);

					CItem uItem;
					uItem.Convert(type ,0,0,0,0,0,0,CURRENT_DB_VERSION);


					BYTE uPos =	gObjInventoryInsertItem(lpTarget->m_Index,uItem);
					
					GCInventoryItemOneSend(lpTarget->m_Index, uPos);
				
					LogAddTD("[Quest] Quest Item Insert [%s]: [%s][%s] (%s) (%d,%d)", lpObj->Name,lpTarget->AccountID, lpTarget->Name, lpQuest->name, lpQitem->itemType,lpQitem->itemSubType);
					qMsg->Notice(lpTarget->m_Index,  "[Quest] You're item in invnotry!! ");
					this->SetQuestState(lpTarget , qIndex , QUEST_STATE_FINISH );
					lpQuest->QuestKilledCount = 0;
				}
				else if(lpObj->Level >= lpQitem->QuestMob[n].minLevel && lpObj->Level <= lpQitem->QuestMob[n].maxLevel)
				{
				
					lpQuest->QuestKilledCount++;
					qMsg->Msg(lpTarget->m_Index , "[Quest] you need kill more %d monsters",lpQitem->QuestMob[n].count - lpQuest->QuestKilledCount);
				}
			}
		}
		
	}



	return false;
}