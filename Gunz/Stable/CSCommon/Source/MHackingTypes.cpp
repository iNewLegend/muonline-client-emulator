#include "stdafx.h"
#include "MHackingTypes.h"

const PUNISH_TABLE_ITEM MPunishTable::PUNISH_TABLE[MMHT_END] =
{
	{ 0,	  "none",							0, 0, 0,	MMBL_NO },		// MMHT_NO = 0,							
	{ 130001, "irregularity player",			0, 0, 10,	MMBL_LOGONLY },	// MMHT_XTRAP_HACKER, 10���� ���� ����.
	{ 130001, "x-trap hacking detected.",		0, 1, 0,	MMBL_LOGONLY },	// MMHT_HSHIELD_HACKER, 1�ð� �Ŀ� ���� ����.
	{ 130001, "hackshield hacking detected.",	0, 1, 0,	MMBL_LOGONLY },	// MMHT_BADFILECRC,
	{ 130004, "bad filecrc.",					0, 0, 0,	MMBL_LOGONLY },	// MMHT_BADUSER,
	{ 130001, "gameguard hacker",				0, 0, 0,	MMBL_LOGONLY },	// MMHT_GAMEGUARD_HACKER,
	{ 130001, "dll injectoin",					0, 0, 0,	MMBL_LOGONLY },	// MMHT_GIVE_ONESELF_UP_DLLINJECTION,
	{ 130001, "invalid stage setting.",			0, 0, 0,	MMBL_LOGONLY },	// MMHT_INVALIDSTAGESETTING,
	{ 130005, "command flooding.",				0, 1, 0,	MMBL_ACCOUNT },	// MMHT_COMMAND_FLOODING, 1�ð� �Ŀ� ���� ����.
	{ 130006, "block by admin",					0, 1, 0,	MMBL_ACCOUNT },	// MMHT_COMMAND_BLOCK_BY_ADMIN, 1�ð� �Ŀ� ���� ����.
	{ 130007, "Sleep Account, Block by Admin",	0, 1, 0,	MMBL_ACCOUNT },	// MMHT_SLEEP_ACCOUNT, ��ڿ� ���� �޸� �������� �з��� ȸ��. ������ ���� �Ұ���
};

const PUNISH_TABLE_ITEM& MPunishTable::GetPunish( MMatchHackingType eType )
{
	if(eType<0 || eType >=MMHT_END) return PUNISH_TABLE[0];

	return PUNISH_TABLE[ eType ];
}
