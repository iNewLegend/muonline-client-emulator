/////////////////////////////////
// 
//   ItemSocketSystem.cpp Addon
//   Thanks to Nuke
//
/////////////////////////////////

#include "StdAfx.h"
#include "User.h"
#include "GameMain.h"
#include "ItemSocketSystem.h"
#include "logproc.h"
#include "..\common\WzMemScript.h"
#include "..\include\Readscript.h"
#include "..\common\winutil.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "DSProtocol.h"

CItemSocket gItemSocket;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemSocket::CItemSocket()
{
    for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
	    this->m_ItemSocketInfo[n].Index = -1;
	}
}

CItemSocket::~CItemSocket()
{

	return;

}

BOOL CItemSocket::Load(char * lpszFileName)
{ // Revisão 29.08.2009
	SMDFile = fopen(lpszFileName, "r");

	if ( SMDFile == NULL )
	{ // Checar se arquivo existe, caso não existir enviar menssagem de erro
		MsgBox("[SocketItemSystem] Info file Load Fail [%s]", lpszFileName);
		return FALSE;
	}

	int Token;
	int type = -1;

    while ( true )
	{
	    Token = GetToken();

        if( Token == 2)
		{
            break;
		}

		type = TokenNumber;

		while ( true )
		{
		    if( type == 0 )
			{
			    Token = GetToken();

				if ( strcmp("end", TokenString ) == 0)
				{
					break;
				}

				ITEM_SOCKET_INFO SocketR;
		        memset( &SocketR, 0, sizeof(SocketR) );

		        SocketR.Index = TokenNumber;

				Token = GetToken();
		        SocketR.Type = TokenNumber;

				Token = GetToken();
		        SocketR.unkopt3 = TokenNumber; // Melhorar quando decompilar o .98

		        Token = GetToken();
				strcpy(&SocketR.Name[0], TokenString );

//				for( int SO = 0; so < MAX_SOCKET_BONUS; SO++ )

				Token = GetToken();
		        SocketR.BonusType = TokenNumber;

		        Token = GetToken();
		        SocketR.Bonus[0] = TokenNumber;

		        Token = GetToken();
		        SocketR.Bonus[1] = TokenNumber;

		        Token = GetToken();
		        SocketR.Bonus[2] = TokenNumber;

		        Token = GetToken();
		        SocketR.Bonus[3] = TokenNumber;

		        Token = GetToken();
		        SocketR.Bonus[4] = TokenNumber;

				Token = GetToken();
				SocketR.unkopt2 = TokenNumber; // Melhorar quando decompilar o .98

		        this->SetInfo(SocketR);
			}
			else if( type == 1)
			{
			    Token = GetToken();

				if(strcmp("end",TokenString ) == 0)
				{
					break;
				}

				Token = GetToken();
				Token = GetToken();
				Token = GetToken();
			}
			else if( type == 2)
			{
			    Token = GetToken();

				if(strcmp("end",TokenString ) == 0)
				{
					break;
				}

				//Token = GetToken();
				//Token = GetToken();
				//Token = GetToken();
				//Token = GetToken();
				//Token = GetToken();
				//Token = GetToken();
				//Token = GetToken();
				//Token = GetToken();
			//	Token = GetToken(); // ta ai seu provavel problema
			}
			else
			{
			    break;
			}
		}
    }

	fclose(SMDFile);
	LogAdd("%s file load!", lpszFileName);
	return TRUE;
}

void CItemSocket::SetInfo(ITEM_SOCKET_INFO info)
{
    if(info.Index >= MAX_SOCKET_OPTION)
	{
		LogAddC( 2, "[SocketSystem] Numero maximo de Socket Excedido ( Index: [%d]  Max Socket: [%d] ).", info.Index, MAX_SOCKET_OPTION );
		//CloseClient(aIndex);
	    return;
	}

    this->m_ItemSocketInfo[info.Index] = info;
}

//void CItemSocket::SetFileInformationPart1( int sIndex, int sType, int Value1, char* Name, int bType, BYTE* Bonus )
//{
//}

ITEM_SOCKET_INFO* CItemSocket::GetInfo(int index)
{
    return &this->m_ItemSocketInfo[index];
}

int CItemSocket::GetOptionCount(int type)
{
	int count = 0;

    for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
	    if(this->m_ItemSocketInfo[n].Index >= 0 && this->m_ItemSocketInfo[n].Type == type)
		{
		    count++;
		}
	}

	return count;
}

BYTE CItemSocket::CalcOption(int type,int level,int SeedOption)
{
    BYTE option = 0xFE;

	int count = SeedOption;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
	    if(this->m_ItemSocketInfo[n].Index >= 0 && this->m_ItemSocketInfo[n].Type == type)
		{
            if(count == 0)
			{
			    option = this->m_ItemSocketInfo[n].Index;
				break;
			}

			count--;
		}
	}

	return (option+(level*MAX_SOCKET_OPTION)); //50
}

void CItemSocket::InitEffect(ITEM_SOCKET_EFFECT* lpEffect)
{
    memset(lpEffect,0,sizeof(ITEM_SOCKET_EFFECT));
}

void CItemSocket::ApplyEffect(LPOBJ lpObj)
{
	int SocketCount[6] = {0,0,0,0,0,0};

    ITEM_SOCKET_EFFECT* lpEffect = &lpObj->ItemSocketEffect;

	this->InitEffect(lpEffect);

    for(int n=0;n < 7;n++)
	{
	    if(lpObj->pInventory[n].IsItem() == 0 || lpObj->pInventory[n].IsSocketItem() == 0 || lpObj->pInventory[n].m_Durability == 0.0f || lpObj->pInventory[n].m_IsValidItem == 0)
		{
		    continue;
		}

		for(int i=0;i < 5;i++)
		{ // REVISAR!
		    if(lpObj->pInventory[n].m_ItemSocket[i] < 0xFE)
			{
			    this->CalcEffect(lpObj->pInventory[n].m_ItemSocket[i],lpEffect);
			}
		}
	}

	lpObj->m_AttackDamageMinLeft += lpEffect->IncreaseDamage;
	lpObj->m_AttackDamageMinLeft += lpEffect->IncreaseMinDamage;
	lpObj->m_AttackDamageMaxLeft += lpEffect->IncreaseDamage;
	lpObj->m_AttackDamageMaxLeft += lpEffect->IncreaseMaxDamage;

	lpObj->m_AttackDamageMinRight += lpEffect->IncreaseDamage;
	lpObj->m_AttackDamageMinRight += lpEffect->IncreaseMinDamage;
	lpObj->m_AttackDamageMaxRight += lpEffect->IncreaseDamage;
	lpObj->m_AttackDamageMaxRight += lpEffect->IncreaseMaxDamage;

	lpObj->m_MagicDamageMin += lpEffect->IncreaseSkillPower;
	lpObj->m_MagicDamageMin += lpEffect->IncreaseMinSkillPower;
	lpObj->m_MagicDamageMax += lpEffect->IncreaseSkillPower;
	lpObj->m_MagicDamageMax += lpEffect->IncreaseMaxSkillPower;

	lpObj->m_AttackSpeed += lpEffect->IncreaseAttackSpeed;
	lpObj->m_MagicSpeed += lpEffect->IncreaseAttackSpeed;

	lpObj->m_SuccessfulBlocking += (lpObj->m_SuccessfulBlocking*lpEffect->IncreaseDefenseSuccessRate)/100;

	lpObj->m_Defense += lpEffect->IncreaseDefense;

	lpObj->DamageReflect += lpEffect->IncreaseDamageReflect;

	lpObj->DamageMinus += lpEffect->IncreaseDamageReduction;

	lpObj->AddLife += (lpObj->MaxLife*lpEffect->IncreaseMaxHP)/100;

	lpObj->AddMana += (lpObj->MaxMana*lpEffect->IncreaseMaxMP)/100; 

	lpObj->AddBP += (lpObj->MaxBP*lpEffect->IncreaseMaxAG)/100;

	lpObj->m_ExcelentDamage += lpEffect->IncreaseExcellentDamageRate;

	lpObj->m_CriticalDamage += lpEffect->IncreaseCriticalDamageRate;
}

void CItemSocket::CalcEffect(BYTE option,ITEM_SOCKET_EFFECT* lpEffect)
{
	int index = 0;
    int level = 0;

	if(option > 0)
	{
		index = option%MAX_SOCKET_OPTION;
	    level = option/MAX_SOCKET_OPTION;
	}

	int bonus = this->m_ItemSocketInfo[index].Bonus[level];

	switch(index)
	{
	    case ITEM_SOCKET_INCREASE_DAMAGE:
			lpEffect->IncreaseDamage += bonus;
	        break;
        case ITEM_SOCKET_INCREASE_ATTACK_SPEED:
			lpEffect->IncreaseAttackSpeed += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_MAX_DAMAGE_AND_SKILL_POWER:
			lpEffect->IncreaseMaxDamage += bonus;
			lpEffect->IncreaseMaxSkillPower += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_MIN_DAMAGE_AND_SKILL_POWER:
			lpEffect->IncreaseMinDamage += bonus;
			lpEffect->IncreaseMinSkillPower += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_DAMAGE_AND_SKILL_POWER:
			lpEffect->IncreaseDamage += bonus;
			lpEffect->IncreaseSkillPower += bonus;
	        break;
	    case ITEM_SOCKET_DECREASE_AG_CONSUMPTION:
			lpEffect->IncreaseAGReduction += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_DEFENSE_SUCCESS_RATE:
			lpEffect->IncreaseDefenseSuccessRate += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_DEFENSE:
			lpEffect->IncreaseDefense += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_SHIELD_DEFENSE:
			lpEffect->IncreaseShieldDefense += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_DAMAGE_REDUCTION:
            lpEffect->IncreaseDamageReduction += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_DAMAGE_REFLECT:
			lpEffect->IncreaseDamageReflect += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_RECOVER_LIFE:
            lpEffect->IncreaseRecoverLife += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_RECOVER_MANA:
			lpEffect->IncreaseRecoverMana += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_SKILL_POWER:
			lpEffect->IncreaseSkillPower += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_ATTACK_SUCCESS_RATE:
			lpEffect->IncreaseAttackSuccessRate += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_ITEM_DURABILITY:
	        break;
	    case ITEM_SOCKET_INCREASE_LIFE_AUTORECOVERY:
	        break;
	    case ITEM_SOCKET_INCREASE_MAX_HP:
			lpEffect->IncreaseMaxHP += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_MAX_MP:
			lpEffect->IncreaseMaxMP += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_MANA_AUTORECOVERY:
	        break;
	    case ITEM_SOCKET_INCREASE_MAX_AG:
			lpEffect->IncreaseMaxAG += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_AG_AUTORECOVERY:
	        break;
	    case ITEM_SOCKET_INCREASE_EXCELLENT_DAMAGE:
			lpEffect->IncreaseExcellentDamage += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_EXCELLENT_DAMAGE_RATE:
			lpEffect->IncreaseExcellentDamageRate += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_CRITICAL_DAMAGE:
			lpEffect->IncreaseCriticalDamage += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_CRITICAL_DAMAGE_RATE:
			lpEffect->IncreaseCriticalDamageRate += bonus;
	        break;
	    case ITEM_SOCKET_INCREASE_STRENGTH:
	        break;
	    default:
		    break;
	}
}

bool CItemSocket::IsSeason4Item(int index)
{
    if(index >= ITEMGET(0,26) && index <= ITEMGET(0,28))
	{
	    return 1;
	}

    if(index == ITEMGET(2,16) || index == ITEMGET(2,17))
	{
	    return 1;
	}

	if(index == ITEMGET(4,23))
	{
	    return 1;
	}

	if(index == ITEMGET(5,30) || index == ITEMGET(5,31))
	{
	    return 1;
	}

	if(index >= ITEMGET(6,17) && index <= ITEMGET(6,20))
	{
	    return 1;
	}

	int section = index/512;

	if(section >= 7 && section <= 11)
	{
	    int type = index%512;

		if(type >= 45 && type <= 53)
		{
		    return 1;
		}
	}

	return 0;
}

void CItemSocket::SeedExtract(LPOBJ lpObj) //GOOD
{
    int ChaosCount = 0;
	int HarmonyCount = 0;
	int CreationCount = 0;
	int ExcItemCount = 0;
	int SetItemCount = 0;

	lpObj->ChaosLock = 1;

    for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
	    if(lpObj->pChaosBox[n].IsItem() == 0)
		{
		    continue;
		}

        if(lpObj->pChaosBox[n].m_Type == ITEMGET(12,15))
		{
		    ChaosCount++;
		}
		else if(lpObj->pChaosBox[n].m_Type == ITEMGET(14,42))
		{
		    HarmonyCount++;
		}
		else if(lpObj->pChaosBox[n].m_Type == ITEMGET(14,22))
		{
		    CreationCount++;
		}
		else if(lpObj->pChaosBox[n].IsExtItem() != 0 && lpObj->pChaosBox[n].m_Level >= 4)
		{
		    ExcItemCount++;
		}
		else if(lpObj->pChaosBox[n].IsSetItem() != 0 && lpObj->pChaosBox[n].m_Level >= 4)
		{
		    SetItemCount++;
		}
	}

    PMSG_CHAOSMIXRESULT pMsg;

    PHeadSetB((LPBYTE)&pMsg.h,0x86,sizeof(pMsg));

	if(ChaosCount != 1 || HarmonyCount != 1 || CreationCount != 1 || ExcItemCount != 1 || SetItemCount != 1)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_INCORRECT_MIX_ITEMS;
        DataSend(lpObj->m_Index,(BYTE*)&pMsg,pMsg.h.size);
		return;
	}

	lpObj->ChaosSuccessRate = 90;

	lpObj->ChaosMoney = 1000000;
	
	int TaxMoney = (lpObj->ChaosMoney*  g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < lpObj->ChaosMoney)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_NOT_ENOUGH_ZEN;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

	int type = rand()%6;
	int count = this->GetOptionCount(type);
	
	if(count == 0)
	{
        lpObj->ChaosLock = 0;

        pMsg.Result = CB_ERROR;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		LogAddTD("[%s][%s] Seed Extract Mix Failed",lpObj->AccountID,lpObj->Name);
		return;
	}

    lpObj->Money -= lpObj->ChaosMoney;
    GCMoneySend(lpObj->m_Index,lpObj->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

    if((rand()%100) < lpObj->ChaosSuccessRate)
	{
		int index = ITEMGET(12,60)+type;
		int level = rand()%count;

	    ItemSerialCreateSend(lpObj->m_Index,-1,0,0,index,level,0,0,0,0,-1,0,0);
		LogAddTD("[%s][%s] Seed Extract Mix Success - Seed Info ( Index : %d, Level : %d )",lpObj->AccountID,lpObj->Name,index,level);
	}
	else
	{
	    ChaosBoxInit(lpObj);
        GCUserChaosBoxSend(lpObj,0);

	    pMsg.Result = CB_ERROR;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
	    LogAddTD("[%s][%s] Seed Extract Mix Failed",lpObj->AccountID,lpObj->Name);
	}

    lpObj->ChaosLock = 0;
}

void CItemSocket::SeedSphere(LPOBJ lpObj)
{
    int ChaosCount = 0;
	int CreationCount = 0;
	int SeedCount = 0;
	int SeedType = 0;
	int SeedLevel = 0;
	int SphereCount = 0;
	int SphereType = 0;

	lpObj->ChaosLock = 1;

    for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
	    if(lpObj->pChaosBox[n].IsItem() == 0)
		{
		    continue;
		}

        if(lpObj->pChaosBox[n].m_Type == ITEMGET(12,15))
		{
		    ChaosCount++;
		}
		else if(lpObj->pChaosBox[n].m_Type == ITEMGET(14,22))
		{
		    CreationCount++;
		}
		else if(lpObj->pChaosBox[n].m_Type >= ITEMGET(12,60) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,66))
		{
		    SeedCount++;
			SeedType = lpObj->pChaosBox[n].m_Type - ITEMGET(12,60);
			SeedLevel = lpObj->pChaosBox[n].m_Level;
		}
		else if(lpObj->pChaosBox[n].m_Type >= ITEMGET(12,70) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,74))
		{
		    SphereCount++;
			SphereType = lpObj->pChaosBox[n].m_Type - ITEMGET(12,70);
		}
	}

    PMSG_CHAOSMIXRESULT pMsg;

    PHeadSetB((LPBYTE)&pMsg.h,0x86,sizeof(pMsg));

	if(ChaosCount != 1 || CreationCount != 1 || SeedCount != 1 || SphereCount != 1)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_INCORRECT_MIX_ITEMS;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

	lpObj->ChaosSuccessRate = 85-(SphereType*5);

	lpObj->ChaosMoney = 1000000;
	
	int TaxMoney = (lpObj->ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < lpObj->ChaosMoney)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_NOT_ENOUGH_ZEN;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

    lpObj->Money -= lpObj->ChaosMoney;
    GCMoneySend(lpObj->m_Index,lpObj->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

    if((rand()%100) < lpObj->ChaosSuccessRate)
	{
		int index = ITEMGET(12,100)+(SphereType*6)+SeedType;
		int level = SeedLevel;

	    ItemSerialCreateSend(lpObj->m_Index,-1,0,0,index,level,0,0,0,0,-1,0,0);
        LogAddTD("[%s][%s] SeedSphere Composite Success - Seed Info ( Index : %d, Level : %d )",lpObj->AccountID,lpObj->Name,index,level);
	}
	else
	{
	    ChaosBoxInit(lpObj);
        GCUserChaosBoxSend(lpObj,0);

	    pMsg.Result = 0;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
	    LogAddTD("[%s][%s] SeedSphere Composite Failed",lpObj->AccountID,lpObj->Name);
	}

    lpObj->ChaosLock = 0;
}

void CItemSocket::SeedCalc(LPOBJ lpObj,BYTE info)
{
    int ChaosCount = 0;
	int CreationCount = 0;
	int SeedSphereCount = 0;
	int SeedSphereType = 0;
	int SeedSphereLevel = 0;
	int SeedSphereOption = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	lpObj->ChaosLock = 1;

    for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
	    if(lpObj->pChaosBox[n].IsItem() == 0)
		{
		    continue;
		}

        if(lpObj->pChaosBox[n].m_Type == ITEMGET(12,15))
		{
		    ChaosCount++;
		}
		else if(lpObj->pChaosBox[n].m_Type == ITEMGET(14,22))
		{
		    CreationCount++;
		}
		else if(lpObj->pChaosBox[n].m_Type >= ITEMGET(12,100) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,129))
		{
		    SeedSphereCount++;
			SeedSphereType = (lpObj->pChaosBox[n].m_Type-ITEMGET(12,100))%6;
	        SeedSphereLevel = (lpObj->pChaosBox[n].m_Type-ITEMGET(12,100))/6;
	        SeedSphereOption = lpObj->pChaosBox[n].m_Level; 
		}
		else if(this->IsSeason4Item(lpObj->pChaosBox[n].m_Type) != 0 && lpObj->pChaosBox[n].m_ItemSocket[info] == 0xFE)
		{
	        ItemCount++;
			ItemSlot = n;
		}
	}

    PMSG_CHAOSMIXRESULT pMsg;

    PHeadSetB((LPBYTE)&pMsg.h,0x86,sizeof(pMsg));

	if(ChaosCount != 1 || CreationCount != 1 || SeedSphereCount != 1 || ItemCount != 1)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_INCORRECT_MIX_ITEMS;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

	lpObj->ChaosMoney = 1000000;
	
	int TaxMoney = (lpObj->ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < lpObj->ChaosMoney)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_NOT_ENOUGH_ZEN;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

	BYTE option = this->CalcOption(SeedSphereType,SeedSphereLevel,SeedSphereOption);

	if(option == 0xFE)
	{
        lpObj->ChaosLock = 0;

        pMsg.Result = CB_ERROR;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

    lpObj->Money -= lpObj->ChaosMoney;
    GCMoneySend(lpObj->m_Index,lpObj->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->pChaosBox[ItemSlot];

	item.m_ItemSocket[info] = option;

	pMsg.Result = CB_SUCCESS;

	ItemByteConvert(pMsg.ItemInfo,item);

	ChaosBoxInit(lpObj);

    gObjChaosBoxInsertItemPos(lpObj->m_Index,item,0,-1);
	gObjChaosItemSet(lpObj->m_Index,0,1);

	DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);

	lpObj->ChaosLock = 0;
}

void CItemSocket::NewMix4(LPOBJ lpObj,BYTE info)
{
    int ItemCount = 0;
	int ItemSlot = 0;

	lpObj->ChaosLock = 1;

    for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
	    if(this->IsSeason4Item(lpObj->pChaosBox[n].m_Type) != 0 && lpObj->pChaosBox[n].m_ItemSocket[info] < 0xFE)
		{
	        ItemCount++;
			ItemSlot = n;
		}
	}

    PMSG_CHAOSMIXRESULT pMsg;

    PHeadSetB((LPBYTE)&pMsg.h,0x86,sizeof(pMsg));

	if(ItemCount != 1)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_INCORRECT_MIX_ITEMS;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

	lpObj->ChaosMoney = 1000000;
	
	int TaxMoney = (lpObj->ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < lpObj->ChaosMoney)
	{
	    lpObj->ChaosLock = 0;

        pMsg.Result = CB_NOT_ENOUGH_ZEN;
        DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);
		return;
	}

    lpObj->Money -= lpObj->ChaosMoney;
    GCMoneySend(lpObj->m_Index,lpObj->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->pChaosBox[ItemSlot];

	item.m_ItemSocket[info] = 0xFE;

	pMsg.Result = CB_SUCCESS;

	ItemByteConvert(pMsg.ItemInfo,item);

	ChaosBoxInit(lpObj);

    gObjChaosBoxInsertItemPos(lpObj->m_Index,item,0,-1);
	gObjChaosItemSet(lpObj->m_Index,0,1);

	DataSend(lpObj->m_Index,(BYTE *)&pMsg, pMsg.h.size);

	lpObj->ChaosLock = 0;
}