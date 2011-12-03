#pragma once

#include "MSingleton.h"
#include "MUtil.h"

// ���⿡ �ϳ� �߰� �Ͻø� �Ʒ� ��¡ ���̺� �ϳ� �߰��ϼž� �մϴ�
enum MMatchHackingType
{
	MMHT_NO = 0,	
	MMHT_XTRAP_HACKER,
	MMHT_HSHIELD_HACKER,
	MMHT_BADFILECRC,
	MMHT_BADUSER,
	MMHT_GAMEGUARD_HACKER,
	MMHT_GIVE_ONESELF_UP_DLLINJECTION,
	MMHT_INVALIDSTAGESETTING,
	MMHT_COMMAND_FLOODING,
	MMHT_COMMAND_BLOCK_BY_ADMIN,
	MMHT_SLEEP_ACCOUNT = 10,					///< �ϴ��� NHN ��û�� ���� �޸� ���� ó���̴�. (����� 10�̾�� �Ѵ�. �ٲ� �ȵȴ�)
	MMHT_END,
};

struct PUNISH_TABLE_ITEM {
	DWORD				dwMessageID;
	const char*			szComment;

	u_short				nDay;
	u_short				nHour;
	u_short				nMin;
	
	MMatchBlockLevel	eLevel;
};

class MPunishTable {
	static const PUNISH_TABLE_ITEM PUNISH_TABLE[MMHT_END];
public:
	static const PUNISH_TABLE_ITEM& GetPunish( MMatchHackingType eType );
};