#ifndef GAMESERVER_QUEST_H
#define GAMESERVER_QUEST_H

#include "User.h"

#define MAX_QUEST_NAME			30
#define MAX_SUBQUEST_OBJECTS	50
#define MAX_SUB_QUEST			20
#define MAX_QUESTS				200

#define QUEST_OBJECT_TYPES		Q_Types

enum QUEST_STATES
{
	QUEST_STATE_START =		0x00,
	QUEST_STATE_ONGOING =	0x01,
	QUEST_STATE_FINISH =	0x02,
};

enum QUEST_OBJECT_TYPES
{
	QUEST_OBJECT_INVAILD = -1,
	QUEST_OBJECT_TYPE_ITEM = 0x00,
	QUEST_OBJECT_TYPE_MONSTER = 0x01,
	QUEST_OBJECT_PLAYER_KILL = 0x02,
};


struct q_SubPrize
{
	int				index;
	int				points;
	int				ClassUp;
	int				itemType;
	int				itemSubType;
	int				itemCount;

};

struct Q_SubMob
{
	int				index;
	int				minLevel;
	int				maxLevel;
	int				count;
};


struct QuestObjectItem
{
	int				index;
	int				itemType;
	int				itemSubType;
	int				itemCount;
	int				QuestPrizeCount;
	int				QuestMobCount;
	Q_SubMob		QuestMob[MAX_SUB_QUEST];
	q_SubPrize		QuestPrize[MAX_SUB_QUEST];

};

struct QuestObjectMob
{
	int				index;
	int				id;
	int				count;
};

struct QuestAtt
{
	int					index;
	char				name[MAX_QUEST_NAME];
	int					level;
	int					npcID;
	int					zen;
	int					QuestsObjectCount;
	int					QuestKilledCount;
	int					dw;
	int					dk;
	int					elf;
	int					dl;
	int					mg;
	QUEST_STATES		State;
	QuestObjectItem		ItemsObject[MAX_SUBQUEST_OBJECTS];
	QuestObjectMob		MobsObject[MAX_SUBQUEST_OBJECTS];
};

class CQuest
{
public:
	CQuest();
	//~CQuest();
	void		Init();
	bool		LoadFile(char* szFileName) ;
	bool		IsQuest(int QuestIndex) ;
	bool		SetQuestState(LPOBJ lpObj, int QuestIndex, int State);
	int			GetQuestState(LPOBJ lpObj, int QuestIndex) ;;
	BYTE		ReSetQuestState(LPOBJ lpObj, int QuestIndex) ;
	void		QuestInfoSave(LPOBJ lpObj) ;
	bool		NpcTalk(LPOBJ lpNpc, LPOBJ lpObj) ;
	bool		MonsterItemDrop(LPOBJ lpObj) ;
	Q_Types		GetQuestType(int QuestIndex , int SubIndex) ;
	bool		IsClass(int QuestIndex,int Class);



	int			GetQeustCount(){return questCount;}


private:
	int				questCount;
	QuestAtt		quests[MAX_QUESTS];
};

#endif