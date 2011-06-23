#ifndef MAGICDAMAGE_H
#define MAGICDAMAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_SKILL			256
#define MAX_REQ_SKILL_STATUS	3

#include "..\common\classdef.h"


enum eSkillType
{
	AT_SKILL_POISON = 0x1,
	AT_SKILL_METEO = 0x2,
	AT_SKILL_THUNDER = 0x3,
	AT_SKILL_FIREBALL = 0x4,
	AT_SKILL_FLAME = 0x5,
	AT_SKILL_TELEPORT = 0x6,
	AT_SKILL_SLOW = 0x7,
	AT_SKILL_STORM = 0x8,
	AT_SKILL_EVIL = 0x9,
	AT_SKILL_HELL = 0xa,
	AT_SKILL_POWERWAVE = 0xb,
	AT_SKILL_FLASH = 0xc,
	AT_SKILL_BLAST = 0xd,
	AT_SKILL_INFERNO = 0xe,
	AT_SKILL_TARGET_TELEPORT = 0xf,
	AT_SKILL_MAGICDEFENSE = 0x10,
	AT_SKILL_ENERGYBALL = 0x11,
	AT_SKILL_BLOCKING = 0x12,
	AT_SKILL_SWORD1 = 0x13,
	AT_SKILL_SWORD2 = 0x14,
	AT_SKILL_SWORD3 = 0x15,
	AT_SKILL_SWORD4 = 0x16,
	AT_SKILL_SWORD5 = 0x17,
	AT_SKILL_CROSSBOW = 0x18,
	AT_SKILL_BOW = 0x19,
	AT_SKILL_HEALING = 0x1a,
	AT_SKILL_DEFENSE = 0x1b,
	AT_SKILL_ATTACK = 0x1c,
	AT_SKILL_CALLMON1 = 0x1e,
	AT_SKILL_CALLMON2 = 0x1f,
	AT_SKILL_CALLMON3 = 0x20,
	AT_SKILL_CALLMON4 = 0x21,
	AT_SKILL_CALLMON5 = 0x22,
	AT_SKILL_CALLMON6 = 0x23,
	AT_SKILL_CALLMON7 = 0x24,
	AT_SKILL_WHEEL = 0x29,
	AT_SKILL_BLOWOFFURY = 0x2a,
	AT_SKILL_STRIKE = 0x2b,
	AT_SKILL_KNIGHTSPEAR = 0x2f,
	AT_SKILL_KNIGHTADDLIFE = 0x30,
	AT_SKILL_KNIGHTDINORANT = 0x31,
	AT_SKILL_ELFHARDEN = 0x33,
	AT_SKILL_PENETRATION = 0x34,
	AT_SKILL_DEFENSEDOWN = 0x37,
	AT_SKILL_SWORD6 = 0x38,
	AT_SKILL_5CROSSBOW = 0x36,
	AT_SKILL_EXPPOISON = 0x26,
	AT_SKILL_EXPICE = 0x27,
	AT_SKILL_EXPHELL = 0x28,
	AT_SKILL_EXPHELL_START = 0x3a,
	AT_SKILL_IMPROVE_AG_REFILL = 0x35,
	AT_SKILL_DEVILFIRE = 0x32,
	AT_SKILL_COMBO = 0x3b,
	AT_SKILL_SPEAR = 0x3c,
	AT_SKILL_FIREBURST = 0x3d,
	AT_SKILL_DARKHORSE_ATTACK = 0x3e,
	AT_SKILL_RECALL_PARTY = 0x3f,
	AT_SKILL_ADD_CRITICALDAMAGE = 0x40,
	AT_SKILL_ELECTRICSPARK = 0x41,
	AT_SKILL_LONGSPEAR = 0x42,
	AT_SKILL_RUSH = 0x2c,
	AT_SKILL_JAVALIN = 0x2d,
	AT_SKILL_DEEPIMPACT = 0x2e,
	AT_SKILL_ONE_FLASH = 0x39,
	AT_SKILL_DEATH_CANNON = 0x49,
	AT_SKILL_SPACE_SPLIT = 0x4a,
	AT_SKILL_BRAND_OF_SKILL = 0x4b,
	AT_SKILL_STUN = 0x43,
	AT_SKILL_REMOVAL_STUN = 0x44,
	AT_SKILL_ADD_MANA = 0x45,
	AT_SKILL_INVISIBLE = 0x46,
	AT_SKILL_REMOVAL_INVISIBLE = 0x47,
	AT_SKILL_REMOVAL_MAGIC = 0x48,
	AT_SKILL_FENRIR_ATTACK = 0x4c,
	AT_SKILL_INFINITY_ARROW = 0x4d,
	AT_SKILL_FIRESCREAM = 0x4e,
	AT_SKILL_EXPLOSION = 0x4f,
	AT_SKILL_SUMMON = 0xc8,
	AT_SKILL_IMMUNE_TO_MAGIC = 0xc9,
	AT_SKILL_IMMUNE_TO_HARM = 0xca,
	//season 3 illusion temple
	AT_SKILL_ITSHIELD = 0xd2,
	AT_SKILL_ITRESIST = 0xd3,
	AT_SKILL_ITTRACK = 0xd4,
	AT_SKILL_ITFREEZE = 0xd5,
	// Season 4 Skills =P
	AT_SKILL_DRAIN_LIFE = 0xd6,
	AT_SKILL_CHAIN_LIGHTING = 0xd7,
	AT_SKILL_ELECTRIC_SURGE = 0xd8,
	AT_SKILL_REFLEX = 0xd9,
	AT_SKILL_SWORD_POWER = 0xda,
	AT_SKILL_SLEEP = 0xdb,
	AT_SKILL_NIGHT = 0xdc,
	AT_SKILL_MAGICSPEED_UP = 0xdd,
	AT_SKILL_MAGICDEFENSE_UP = 0xde,
	AT_SKILL_SAHAMUTT = 0xdf,
	AT_SKILL_NEIL = 0xe0,
	AT_SKILL_GHOST_PHANTOM = 0xe1,
	AT_SKILL_RED_STORM = 0xe6,
	AT_SKILL_EXPLOTION = 0xe8,
	AT_SKILL_MAGIC_CIRCLE = 0xe9,
	AT_SKILL_REDUCE_DAMAGE_SHIELD = 0xea,
	AT_SKILL_FIVE_SHOT = 0xeb,
	AT_SKILL_SWORD_SLASH = 0xec,
	AT_SKILL_LIGHTING_STORM = 0xed,
	AT_SKILL_BIRDS = 0xee,
};


class CMagicDamage
{

private:

	int Get(int skill);
	void SetEx(int iSkill, int iSkillUseType, int iSkillBrand, int iKillCount, int* pReqStatus);
	void Set(char* name, int skill, int damage,  int rlevel,  int mana,  int bp,  int dis, int Attr,  int iSkillType, unsigned char* ReqClass, int rEnergy, int rLeadership, int rdelay);
	int CheckSkillAttr(int skill, int attr);

public:

	CMagicDamage();
	virtual ~CMagicDamage();

	void Init();
	void LogSkillList(char* Buffer, int iSize);
	void LogSkillList(char* filename);
	void LogSkillNameList(char* Buffer, int iSize);
	void LogSkillNameList(char* filename);
	int SkillGet(int skill);
	int SkillGetMana(int skill);
	int SkillGetRequireEnergy(int skill);
	int SkillGetRequireLevel(int skill);
	int SkillGetRequireClass(int Cclass, int ChangeUP, int skill);
	int SkillGetBP(int skill);
	int GetSkillAttr(int skill);
	int GetskillRequireLeadership(int skill);
	int GetDelayTime(int skill);
	int GetSkillType(int iSkill);
	int CheckStatus(int iSkill, int iGuildStatus);
	int CheckBrandOfSkill(int iSkill);
	int CheckKillCount(int iSkill, int iKillCount);
	int GetSkillDistance(int skill);

private:

	int m_Damage[MAX_SKILL];	// 4
	char m_Name[MAX_SKILL][50];	// 404
	unsigned char m_rLevel[MAX_SKILL];	// 3604
	unsigned short m_Mana[MAX_SKILL];	// 3704
	unsigned char m_Distance[MAX_SKILL];	// 3904
	int m_RequireEnergy[MAX_SKILL];	// 3A04
	unsigned char m_RequireClass[MAX_SKILL][MAX_TYPE_PLAYER];	// 3E04
	int m_RequireLevel[MAX_SKILL];	// 4304
	int m_RequireLeadership[MAX_SKILL];	// 4704
	int m_Delay[MAX_SKILL];	// 4B04
	WORD m_BrainPower[MAX_SKILL];	// 4F04
	int m_Attr[MAX_SKILL];	// 5104
	int m_iSkillType[MAX_SKILL];	// 5504
	int m_iSkillUseType[MAX_SKILL];	// 5904
	int m_iSkillBrand[MAX_SKILL];	// 5D04
	int m_iKillCount[MAX_SKILL];	// 6104
	int m_iRequireStatus[MAX_SKILL][MAX_REQ_SKILL_STATUS];	// 6504

	enum 
	{
		SKILL_TYPE_NONE = 0xff,
		SKILL_TYPE_PHYSICS = 0x0,
		SKILL_TYPE_MAGIC = 0x1,
	};

};

extern CMagicDamage MagicDamageC;

#endif