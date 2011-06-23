/////////////////////////////////
//  ItemSocketSystem.h
#if !defined(AFX_ITEMSOCKET_H__CED31E6F_47DE_4400_9F20_1F64E5D4E79F__INCLUDED_)
#define AFX_ITEMSOCKET_H__CED31E6F_47DE_4400_9F20_1F64E5D4E79F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h" // perai

#define MAX_SOCKET_OPTION    50
#define MAX_SOCKET_BONUS     5

enum eItemSocketEffect
{
    ITEM_SOCKET_INCREASE_DAMAGE = 0,
    ITEM_SOCKET_INCREASE_ATTACK_SPEED = 1,
	ITEM_SOCKET_INCREASE_MAX_DAMAGE_AND_SKILL_POWER = 2,
	ITEM_SOCKET_INCREASE_MIN_DAMAGE_AND_SKILL_POWER = 3,
	ITEM_SOCKET_INCREASE_DAMAGE_AND_SKILL_POWER = 4,
	ITEM_SOCKET_DECREASE_AG_CONSUMPTION = 5,
	ITEM_SOCKET_INCREASE_DEFENSE_SUCCESS_RATE = 10,
	ITEM_SOCKET_INCREASE_DEFENSE = 11,
	ITEM_SOCKET_INCREASE_SHIELD_DEFENSE = 12,
	ITEM_SOCKET_INCREASE_DAMAGE_REDUCTION = 13,
	ITEM_SOCKET_INCREASE_DAMAGE_REFLECT = 14,
	ITEM_SOCKET_INCREASE_RECOVER_LIFE = 16,
	ITEM_SOCKET_INCREASE_RECOVER_MANA = 17,
	ITEM_SOCKET_INCREASE_SKILL_POWER = 18,
	ITEM_SOCKET_INCREASE_ATTACK_SUCCESS_RATE = 19,
	ITEM_SOCKET_INCREASE_ITEM_DURABILITY = 20,
	ITEM_SOCKET_INCREASE_LIFE_AUTORECOVERY = 21,
	ITEM_SOCKET_INCREASE_MAX_HP = 22,
	ITEM_SOCKET_INCREASE_MAX_MP = 23,
	ITEM_SOCKET_INCREASE_MANA_AUTORECOVERY = 24,
	ITEM_SOCKET_INCREASE_MAX_AG = 25,
	ITEM_SOCKET_INCREASE_AG_AUTORECOVERY = 26,
	ITEM_SOCKET_INCREASE_EXCELLENT_DAMAGE = 29,
	ITEM_SOCKET_INCREASE_EXCELLENT_DAMAGE_RATE = 30,
	ITEM_SOCKET_INCREASE_CRITICAL_DAMAGE = 31,
	ITEM_SOCKET_INCREASE_CRITICAL_DAMAGE_RATE = 32,
	ITEM_SOCKET_INCREASE_STRENGTH = 36,
};

struct ITEM_SOCKET_INFO
{
    int Index;
	char Name[50];
	int Type;
	int BonusType;
	int Bonus[5];
	int index2;
	int unkopt2;
	int unkopt3;
};

struct ITEM_SOCKET_EFFECT
{
    short IncreaseDamage;
	short IncreaseMaxDamage;
	short IncreaseMinDamage;
	short IncreaseAttackSpeed;
	short IncreaseSkillPower;
	short IncreaseMaxSkillPower;
	short IncreaseMinSkillPower;
	short IncreaseAGReduction;
	short IncreaseDefenseSuccessRate;
	short IncreaseDefense;
	short IncreaseShieldDefense;
	short IncreaseDamageReflect;
	short IncreaseDamageReduction;
	short IncreaseRecoverLife;
	short IncreaseRecoverMana;
	short IncreaseAttackSuccessRate;
	short IncreaseMaxHP;
	short IncreaseMaxMP;
	short IncreaseMaxAG;
	short IncreaseExcellentDamageRate;
	short IncreaseExcellentDamage;
	short IncreaseCriticalDamageRate;
	short IncreaseCriticalDamage;
	short res1;
	short res2[4];
};

class CItemSocket  
{
public:
	CItemSocket();
	virtual ~CItemSocket();

	BOOL Load(char* filename);

	void SetInfo(ITEM_SOCKET_INFO info);
	ITEM_SOCKET_INFO* GetInfo(int index);

	int GetOptionCount(int type);
	BYTE CalcOption(int type,int level,int SeedOption);

	void InitEffect(struct  ITEM_SOCKET_EFFECT* lpEffect);

	void ApplyEffect( OBJECTSTRUCT* lpObj);

	void CalcEffect(BYTE option,struct ITEM_SOCKET_EFFECT* lpEffect);

	bool IsSeason4Item(int index);

	void SeedExtract(OBJECTSTRUCT* lpObj);
	void SeedSphere(OBJECTSTRUCT* lpObj);
	void SeedCalc(OBJECTSTRUCT* lpObj,BYTE info);
	void NewMix4(OBJECTSTRUCT* lpObj,BYTE info);

private:
	ITEM_SOCKET_INFO m_ItemSocketInfo[MAX_SOCKET_OPTION];
};

extern CItemSocket gItemSocket;

#endif