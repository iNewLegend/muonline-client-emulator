// GuildWars.h: interface for the GuildWars class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _GUILDWARS_H
#define _GUILDWARS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct _GUILD_WARS_DATA
{
		int m_Index;
		int Map;
		int xSTART;
		int ySTART;
		int xEND;
		int yEND;
};

class GuildWars  
{
public:
	GuildWars();
	virtual ~GuildWars();
	void Init();
	bool Load(char* szFileName);

};

#endif /* _GUILDWARS_H */