#ifndef MSKILL_H
#define MSKILL_H


/// ��ų Ÿ��
enum MSkillType{
	MST_PASSIVE_COMBAT = 0,		///<	�нú� ��ų�� ��Ƽ���̼��� �ʿ����.
	MST_ACTIVE_COMBAT = 1,		///<	��Ƽ�꽺ų �� ������ ���õ� ��ų�̴�. �� Ÿ���� ��ų�� ���� �����쿡 ǥ�õ��� ������, DURATION�� ���� �� �ִ�.
	MST_ACTIVE_SPELL = 2,		///<	��Ƽ�꽺ų �� ������ ���¸� ������ ��ų�̴�. �� Ÿ���� ��ų�� Duration�� 0���� ū ��� ���� �����쿡 ǥ�õǾ�����.
};

/// ��ų ���� ����
enum MSkillFomulaType{
	MSFT_NORMAL = 0,
	MSFT_COMPLEX_BENEFIT = 1,
	MSFT_COMPLEX_DETRIMENT = 2,
};

enum MPrimaSkill{
	MSK_NA = 0,
	MSK_OCS = 1,
	MSK_DCS = 2,
	MSK_RCS = 3,
	MSK_ECS = 4,
	MSK_OMS = 5,
	MSK_DMS = 6,
	MSK_EMS = 7,
};

enum MTargetType{
	MTT_NONE = 0,
	MTT_FRIENDLY_TARGET = 1,
	MTT_UNFRIENDLY_TARGET = 2,
	MTT_FRIENDLY_GROUP = 3,
	MTT_SPECIFIED = 255
};

enum MSpecies{
	MSPC_NA = 0,
	MSPC_HUMANOID = 1,
	MSPC_ABERRATION = 2,
	MSPC_ANIMAL = 3,
	MSPC_CONSTRUCT = 4,
	MSPC_DRAGON = 5,
	MSPC_ELEMENTAL = 6,
	MSPC_FEY = 7,
	MSPC_GIANT = 8,
	MSPC_PLANT = 9,
	MSPC_DEVIL = 10,
	MSPC_UNDEAD = 11,
	MSPC_VAMPIRE = 12,
	MSPC_VERMIN = 13,
};

enum MWeaponType{
	MWT_NA = 0,
	MWT_ONE_HANDED_SLASH = 1,
	MWT_TWO_HANDED_SLASH = 2,
	MWT_ONE_HANDED_BLUNT = 3,
	MWT_TWO_HANDED_BLUNT = 4,
	MWT_ONE_HANDED_PIERCING = 5,
	MWT_TWO_HANDED_PIERCING = 6,
	MWT_ONE_HANDED_RANGE_WEAPONT = 7,
	MWT_TWO_HANDED_RANGE_WEAPONT = 8,
	MWT_ONE_HANDED_HAND_TO_HAND = 9,
};

enum MSF_MOD {
	MSF_MOD_HEALTH,
	MSF_MOD_STAT,
	MSF_MOD_RESIST,
	MSF_MOD_DEFENSE,
	MSF_MOD_OFFENSE,
	MSF_MOD_NORMAL,
	MSF_MOD_STATUS,
	MSF_MOD_PRIMA,
	MSF_MOD_KEY
};



#endif
