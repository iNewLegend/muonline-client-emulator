#ifndef MOBJECTTYPES_H
#define MOBJECTTYPES_H

/// ������Ʈ�� Ÿ��
enum MObjectType 
{
	MOT_NONE		= 0,
	MOT_PC			= 1,
	MOT_NPC			= 2,		
	MOT_ITEM		= 3,
	MOT_CHARACTER
};

/// ĳ���� ����
enum MCharacterRace
{
	MCR_NONE		= 0,
	MCR_HUMAN,
	MCR_ELF,
	MCR_DARKELF,
	MCR_DWARF,
	MCR_OGRE
};
typedef unsigned long int MCharacterRaces;

/// ĳ���� Ŭ����
enum MCharacterClass
{
	MCC_NONE			= 0,
	MCC_FIGHTER,
	MCC_ROGUE,
	MCC_ACOLYTE,
	MCC_MAGE,

	// 2������
	MCC_KNIGHT,
	MCC_PALADIN,
	MCC_BESERKER,
	MCC_WARRIOR,
	MCC_RANGER,
	MCC_ASSASSIN,
	MCC_HUNTER,
	MCC_SHADOWWALKER,
	MCC_SCOUT,
	MCC_THIEF,
	MCC_CLERIC,
	MCC_MONK,
	MCC_DOCTOR,
	MCC_SHAMON,
	MCC_DRUID,
	MCC_SORCERER,
	MCC_ENCHANTER,
	MCC_WIZARD,
	MCC_MAGICIAN,
	MCC_WARLOCK
};
typedef unsigned long int MCharacterClasses;

/// ĳ������ Ÿ��
enum MCharacterType
{
	MCT_NONE		= 0,
	MCT_HUMANOID	= 1,	// �÷��̾�
};

enum MCharacterMoveMode
{
	MCMM_WALK		= 0,
	MCMM_RUN		= 1
};

enum MCharacterMode
{
	MCM_PEACE		= 0,
	MCM_OFFENSIVE	= 1
};

enum MCharacterState
{
	MCS_STAND		= 0,
	MCS_SIT			= 1,
	MCS_DEAD		= 2
};

/// ĳ���� ����
enum MCharacterAbility
{
	MCAB_LEVITATE			= 0,
	MCAB_MOUNT				= 1,
	MCAB_INVISIBILITY		= 2,
	MCAB_STEALTH			= 4,
	MCAB_SEE_INVISIBILITY	= 8,
	MCAB_DETECT				= 16,
	MCAB_INVINCIBILITY		= 32,
	MCAB_DISABLE_ACTIVE		= 64,
	MCAB_DISABLE_PASSIVE	= 128,
	MCAB_STUN				= 256,
	MCAB_SLEEP				= 512
};
typedef unsigned long int MCharacterStates;


/*
/// ĳ���� ���°�
enum MCharacterStatus
{
	MCS_NONE			= 0,
	MCS_SIT				= 1,
	MCS_RUN				= 2,
	MCS_MOVE			= 3,

	MCS_ATTACK			= 5,
	MCS_SKILL			= 6,
	MCS_USEDISCIPLINE	= 7,
	MCS_CONCENTRATE		= 8,

	MCS_LEVITATE		= 17,
	MCS_POISON			= 18,
	MCS_CURSE			= 19,
	MCS_STEALTH			= 20,
	MCS_INVISIBILITY	= 21,
	MCS_INVINCIBILITY	= 22,
	MCS_PARALYSIS		= 23,
	MCS_SILENCE			= 24,
	MCS_STUN			= 25,
	MCS_SLEEP			= 26
};
*/



/// ĳ���� �⺻ Ư��ġ
struct MCharacterBasicAttr
{
	int			nSTR;
	int			nCON;
	int			nDEX;
	int			nAGI;
	int			nINT;
	int			nCHA;
	MCharacterBasicAttr(): nSTR(0), nCON(0), 
			   nDEX(0), nAGI(0), nINT(0), nCHA(0) {    }
};

struct MCharacterRepeatInfo
{
	int			nLastTime;
	float		fHP;
	float		fEN;
	MCharacterRepeatInfo(): nLastTime(0), fHP(0), fEN(0) {	}
};

struct MCHARACTERDATA
{
	// �ĺ�
	char		szName[24];
	char		szSurname[24];
	int			iClass;				// enum
	int			iRace;				// enum
	int			iType;				// enum
	int			iSex;				// enum
	int			iSpecialization;	// enum
	int			iBindingSpot;		// pos

	// ����ġ
	int			iCurrentXP;
	int			iNextXP;

	// ����
	int			iRelationships;		// list
	int			iRshipRaise;		// list
	int			iRshipLower;		// list

	// ���׷�
	int			iMR;
	int			iFR;
	int			iCR;
	int			iPR;
	int			iLR;

	// ����
	int			iBUFFS;		// list
	int			iSkillList;	// list

	// ����
	int			iAC;
	int			iMFlee;
	int			iRFlee;
	int			iSFlee;
	int			iHPRegen;
	int			iENRegen;

	// ���ݷ�
	int			iMATK;
	int			iRATK;
	int			iMCritical;
	int			iRCritical;
	int			iMAspd;
	int			iRAspd;
	int			iMAccu;
	int			iRAccu;

	// �̵�
	int			iDSight;
	int			iNSight;
	int			iSpd;

	// ����
	int			iLevitate;			// flag
	int			iInvisibility;		// flag
	int			iInvincibility;		// flag
	int			iStun;				// flag
	int			iStealth;			// flag
	int			iPoisoned;			// flag

	// �̹�
	int			iIVMelee;			// flag
	int			iIVRange;			// flag
	int			iIVMagic;			// flag
	int			iIVCold;			// flag
	int			iIVLightning;		// flag
	int			iIVPoison;			// flag
	int			iIVFire;			// flag
	int			iIVStun;			// flag
	int			iIVSleep;			// flag
	int			iIVCrit;			// flag
	int			iIVParalysis;		// flag

	// ����ȭ
	int			iOCS;
	int			iDCS;
	int			iRCS;
	int			iECS;
	int			iOMS;
	int			iDMS;
	int			iEMS;
};


#endif