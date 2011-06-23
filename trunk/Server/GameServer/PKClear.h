#ifndef GAMESERVER_PKCLEAR_H
#define GAMESERVER_PKCLEAR_H

#include "User.h"

#define MAX_LOACTIONS 20
#define MAX_REQUIRE 30

enum REQUIRE_TYPES
{
	REQUIRE_LEVEL		= 0x00,
	REQUIRE_ZEN			= 0x01,
	REQUIRE_ITEM		= 0x02,
	REQUER_PK			= 0x03,
	REQUIRE_UNKNOW		= 0xFF,
};

struct LoactionAtt
{
	int index;
	int Map;
	int X;
	int Y;
	int Dir;
};

struct RequireAtt
{
	REQUIRE_TYPES	type;
	int				value;
	int				count;
	int				minLevel;
	int				maxLevel;
	int				SubValue;
};

class CPKClear
{
public:
	CPKClear();
	void			Init();
	bool			LoadFile(char *szFileName);
	void			AddGuards();
	bool			NpcTallk(LPOBJ lpNpc, LPOBJ lpObj);
	void			DelReq(LPOBJ lpObj);
	int				GetValue(REQUIRE_TYPES Type);


private:
	REQUIRE_TYPES	GetRequireTypeByName(char *name);
	bool			CheckRequers(LPOBJ lpNpc, LPOBJ lpObj);
	LoactionAtt sLoactions[MAX_LOACTIONS];
	RequireAtt  sRequireAtt[MAX_REQUIRE];
	int			LoactionsCount;
	int			RequireCount;


};

#endif /* GAMESERVER_PKCLEAR_H */