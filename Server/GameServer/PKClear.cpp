#include "stdafx.h"
#include "PKClear.h"
#include "GameMain.h"
#include "LogProc.h"
#include "Messages.h"

// Credits Only to leo123

Messages * pkcMsg = new Messages();

CPKClear::CPKClear()
{
	this->Init();
}

void CPKClear::Init()
{
	this->LoactionsCount = 0;
	this->RequireCount = 0;
}
REQUIRE_TYPES CPKClear::GetRequireTypeByName(char *name)
{
	if (strcmp(name, "level") == 0)
		return REQUIRE_LEVEL;
	if (strcmp(name, "zen") == 0)
		return REQUIRE_ZEN;
	if (strcmp(name, "item") == 0)
		return REQUIRE_ITEM;
	if (strcmp(name, "pk") == 0)
		return REQUER_PK;
	return REQUIRE_UNKNOW;
}
bool CPKClear::LoadFile(char *szFileName)
{
	/*this->Init();
	xmlDocPtr hFile = xmlParseFile(szFileName);
	if (hFile)
	{
		xmlNodePtr nodeRoot = xmlDocGetRootElement(hFile);
		if( xmlStrcmp(nodeRoot->name, (const xmlChar*)"PKCLEAR") == 0)
		{
			xmlNodePtr subChild = NULL;
			subChild = nodeRoot->children;
			while(subChild)
			{
				if( xmlStrcmp(subChild->name, (const xmlChar*)"Loactions") == 0)
				{
					xmlNodePtr Loaction = NULL;
					Loaction = subChild->children;
					while(Loaction)
					{
						if (xmlStrcmp(Loaction->name, (const xmlChar*)"Loaction") == 0)
						{
							int index = this->LoactionsCount;
							this->sLoactions[index].index = index;

							this->sLoactions[index].Map = xmlReadInt(Loaction, "Map");
							this->sLoactions[index].X = xmlReadInt(Loaction, "X");
							this->sLoactions[index].Y = xmlReadInt(Loaction, "Y");
							this->sLoactions[index].Dir = xmlReadInt(Loaction, "Dir");
							this->LoactionsCount++;
						}
						Loaction = Loaction->next;
					}
				}
				if( xmlStrcmp(subChild->name, (const xmlChar*)"Requirements") == 0)
				{
					xmlNodePtr Requirements = NULL;
					Requirements = subChild->children;
					while(Requirements)
					{
						REQUIRE_TYPES type = GetRequireTypeByName((char*)Requirements->name);
						if (type != REQUIRE_UNKNOW)
						{
							int ReqIndex = this->RequireCount;

							sRequireAtt[ReqIndex].type = type;
							sRequireAtt[ReqIndex].value = xmlReadInt(Requirements, "value");
							sRequireAtt[ReqIndex].count = xmlReadInt(Requirements, "count");
							sRequireAtt[ReqIndex].minLevel = xmlReadInt(Requirements, "minLevel");
							sRequireAtt[ReqIndex].maxLevel = xmlReadInt(Requirements, "maxLevel");
							sRequireAtt[ReqIndex].SubValue = xmlReadInt(Requirements, "SubValue");

							this->RequireCount++;
						}
						Requirements = Requirements->next;
					}
				}
				subChild =  subChild->next;
			}
		}
	}
	else
	{
		MsgBox("[PKCLEAR.xml] :: File not found");
		return false;
	}
	LogAddTD("[PKCLEAR] :: File %s Loaded",szFileName);*/
	return true;
	
}

void CPKClear::AddGuards()
{
	for(int i = 0 ; i != this->LoactionsCount ; i++)
	{
		LoactionAtt * LocAtt = &this->sLoactions[i];
		int Index = gObjAddMonster(LocAtt->Map);
		LPOBJ mObj = &gObj[Index];
		mObj->MapNumber = LocAtt->Map;
		mObj->X = LocAtt->X;
		mObj->Y = LocAtt->Y;
		mObj->Dir = LocAtt->Dir;
		gObjSetMonster(Index, 249);
	}
}

bool CPKClear::CheckRequers(LPOBJ lpNpc, LPOBJ lpObj)
{
	int aIndex = lpObj->m_Index;

	if(!gObjIsConnected(aIndex))
		return false;

	int itemnum = 0;
	char szTemp[200] = {0};
	CItem * pItem = new CItem(); 
	for(int i = 0 ; i != this->RequireCount ; i++)
	{
		switch(this->sRequireAtt[i].type)
		{
			case REQUIRE_LEVEL:
				if(lpObj->Level < this->sRequireAtt[i].value)
				{
					wsprintf(szTemp, "You need be level %d",this->sRequireAtt[i].value);
					ChatTargetSend(lpNpc , szTemp , lpObj->m_Index );
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] Sorry, you don't have the required level");
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] next time think before killing someone ");
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] %s",szTemp);
					return false;
				}
				break;
			case REQUIRE_ZEN:
				if(lpObj->Money < this->sRequireAtt[i].value)
				{
					wsprintf(szTemp, "You need %d zen",this->sRequireAtt[i].value);
					ChatTargetSend(lpNpc , szTemp , lpObj->m_Index );
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] Sorry, you don't have the required ZEN");
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] next time think before killing someone ");
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] %s",szTemp);
					return false;
				}
				break;
			case REQUIRE_ITEM:
				itemnum = ITEMGET(this->sRequireAtt[i].value,this->sRequireAtt[i].SubValue);
				pItem->m_Type = itemnum;
				if((gObjGetItemCountInIventory(lpObj->m_Index, itemnum) < this->sRequireAtt[i].count))
				{
					wsprintf(szTemp, "You need %d %s(s)", this->sRequireAtt[i].count ,pItem->GetName());
					ChatTargetSend(lpNpc , szTemp , lpObj->m_Index );
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] You don't have the required item(s)");
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] next time think before killing someone ");
					pkcMsg->Msg(aIndex, "@@[PK CLEAR] %s",szTemp);
					return false;
				}
				break;
			case REQUER_PK:
				if(lpObj->m_PK_Level < this->sRequireAtt[i].value)
				{
					ChatTargetSend(lpNpc , "Hello! I am PK Clear Guard" , lpObj->m_Index );
					return false;
				}
				break;
		}
	}
	return true;
}

int CPKClear::GetValue(REQUIRE_TYPES Type)
{
	for(int i = 0 ; i != this->RequireCount ; i++)
		if(this->sRequireAtt[i].type == Type)
			return this->sRequireAtt[i].value;
	return -1;
}
void CPKClear::DelReq(LPOBJ lpObj)
{

	for(int i = 0 ; i != this->RequireCount ; i++)
	{
		switch(this->sRequireAtt[i].type)
		{
			case REQUIRE_ZEN:
				lpObj->Money -= this->sRequireAtt[i].value;
				GCMoneySend(lpObj->m_Index , lpObj->Money);
				break;
			case REQUIRE_ITEM:
				gObjDelteItemCountInInventory(lpObj->m_Index, this->sRequireAtt[i].value , this->sRequireAtt[i].SubValue , this->sRequireAtt[i].count);
				break;
		}
	}
}
bool CPKClear::NpcTallk(LPOBJ lpNpc, LPOBJ lpObj)
{
	int aIndex = lpObj->m_Index;
	if(lpObj->m_PK_Level > this->GetValue(REQUER_PK))
	{
		if( this->CheckRequers(lpNpc , lpObj ))
		{
			this->DelReq(lpObj);
			LogAddTD("[PK CLEAR] :: %s PK STATE Clear",lpObj->Name);
			GCMagicAttackNumberSend(lpObj, 72, lpObj->m_Index, 1);
			GCPkLevelSend(lpObj->m_Index, 0 );
			lpObj->m_PK_Level = 0;
			lpObj->m_PK_Count = 0;
			lpObj->m_PK_Time = 0;
			pkcMsg->Msg(aIndex, "@@[PK CLEAR] You're not a killer anymore");
			ChatTargetSend(lpNpc , "May the force be with you...",lpObj->m_Index);
		}
	}
	else
	{
		ChatTargetSend(lpNpc , "Hello! I am PK Clear Guard" , lpObj->m_Index );
	}
	return false;
}

