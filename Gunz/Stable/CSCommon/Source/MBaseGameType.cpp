#include "stdafx.h"
#include "MBaseGameType.h"


#define MMATCH_GAMETYPE_DEATHMATCH_SOLO_STR		"Death Match(Solo)"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM_STR		"Death Match(Team)"
#define MMATCH_GAMETYPE_GLADIATOR_SOLO_STR		"Gladiator(Solo)"
#define MMATCH_GAMETYPE_GLADIATOR_TEAM_STR		"Gladiator(Team)"
#define MMATCH_GAMETYPE_ASSASSINATE_STR			"Assassinate"
#define MMATCH_GAMETYPE_TRAINING_STR			"Training"
#define MMATCH_GAMETYPE_CLASSIC_SOLO_STR		"Classic(Solo)"
#define MMATCH_GAMETYPE_CLASSIC_TEAM_STR		"Class(Team)"
#define MMATCH_GAMETYPE_SURVIVAL_STR			"Survival"
#define MMATCH_GAMETYPE_QUEST_STR				"Quest"
#define MMATCH_GAMETYPE_BERSERKER_STR			"Berserker"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR	"Death Match(Solo Unlimit)"
#define MMATCH_GAMETYPE_DUEL_STR				"Duel"
#define MMATCH_GAMETYPE_DUELTOURNAMENT_STR		"Duel Tournament"

// ���÷��� ���ϸ�� ��� ��� (enum MMATCH_GAMETYPE �� ������ ���ƾ���)
const char* MMatchGameTypeAcronym[MMATCH_GAMETYPE_MAX] = {
	"DM",		// DEATHMATCH_SOLO	
	"DMT",		// DEATHMATCH_TEAM	
	"GL",		// GLADIATOR_SOLO	
	"GLT",		// GLADIATOR_TEAM	
	"ASSA",		// ASSASSINATE		
	"TR",		// TRAINING		
	"SURV",		// SURVIVAL		
	"QST",		// QUEST			
	"DMBSK",	// BERSERKER (DeathMatch Berserker)		
	"DMTE",		// DEATHMATCH_TEAM2 (DeathMatch Team Extream)
	"DUEL",		// DUEL			
	"DUELT",	// DUELTOURNAMENT	
};

void MMatchGameTypeInfo::Set(const MMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		    const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio)
{
	MMatchGameTypeInfo::nGameTypeID = a_nGameTypeID;
	strcpy(MMatchGameTypeInfo::szGameTypeStr, a_szGameTypeStr);
	MMatchGameTypeInfo::fGameExpRatio = a_fGameExpRatio;
	MMatchGameTypeInfo::fTeamMyExpRatio = a_fTeamMyExpRatio;
	MMatchGameTypeInfo::fTeamBonusExpRatio = a_fTeamBonusExpRatio;
}

void MMatchGameTypeInfo::AddMap(int nMapID)
{
	MapSet.insert(set<int>::value_type(nMapID));
}

void MMatchGameTypeInfo::AddAllMap()
{
	for (int i = 0; i < MMATCH_MAP_MAX; i++)
	{
		AddMap(i);
	}
}

MBaseGameTypeCatalogue::MBaseGameTypeCatalogue()
{
#define _InitGameType(index, id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio)		\
m_GameTypeInfo[index].Set(id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio);

//				index,							id,									����Ÿ���̸�,					����ġ��� ����, ���� ���� ����ġ �����, ���� �� ����ġ �����
_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_SOLO,	MMATCH_GAMETYPE_DEATHMATCH_SOLO,	MMATCH_GAMETYPE_DEATHMATCH_SOLO_STR,  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM,	MMATCH_GAMETYPE_DEATHMATCH_TEAM,	MMATCH_GAMETYPE_DEATHMATCH_TEAM_STR,  1.0f,			0.8f,					0.3f);
_InitGameType(MMATCH_GAMETYPE_GLADIATOR_SOLO,	MMATCH_GAMETYPE_GLADIATOR_SOLO,		MMATCH_GAMETYPE_GLADIATOR_SOLO_STR,   0.5f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_GLADIATOR_TEAM,	MMATCH_GAMETYPE_GLADIATOR_TEAM,		MMATCH_GAMETYPE_GLADIATOR_TEAM_STR,   0.5f,			0.8f,					0.3f);
_InitGameType(MMATCH_GAMETYPE_ASSASSINATE,		MMATCH_GAMETYPE_ASSASSINATE,		MMATCH_GAMETYPE_ASSASSINATE_STR,	  1.0f,			0.8f,					0.3f);
_InitGameType(MMATCH_GAMETYPE_TRAINING,			MMATCH_GAMETYPE_TRAINING,			MMATCH_GAMETYPE_TRAINING_STR,		  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_SURVIVAL,			MMATCH_GAMETYPE_SURVIVAL,			MMATCH_GAMETYPE_SURVIVAL_STR,		  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_QUEST,			MMATCH_GAMETYPE_QUEST,				MMATCH_GAMETYPE_QUEST_STR,			  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_BERSERKER,		MMATCH_GAMETYPE_BERSERKER,			MMATCH_GAMETYPE_BERSERKER_STR,		  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM2,	MMATCH_GAMETYPE_DEATHMATCH_TEAM2,	MMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR, 1.0f,			0.6f,					0.5f);
_InitGameType(MMATCH_GAMETYPE_DUEL,				MMATCH_GAMETYPE_DUEL,				MMATCH_GAMETYPE_DUEL_STR,			  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_DUELTOURNAMENT,	MMATCH_GAMETYPE_DUELTOURNAMENT,		MMATCH_GAMETYPE_DUELTOURNAMENT_STR,	  0.0f,			0.0f,					0.0f);

/*
#ifdef _CLASSIC
_InitGameType(MMATCH_GAMETYPE_CLASSIC_SOLO, MMATCH_GAMETYPE_CLASSIC_SOLO,
			  MMATCH_GAMETYPE_CLASSIC_SOLO_STR,		 1.0f,	1.0f,	0.0f);
_InitGameType(MMATCH_GAMETYPE_CLASSIC_TEAM, MMATCH_GAMETYPE_CLASSIC_TEAM,
			  MMATCH_GAMETYPE_CLASSIC_TEAM_STR,		 1.0f,	0.8f,	0.3f);
#endif
*/

	// �� ����Ÿ�Կ��� �÷��� ������ �� - ����� ��� ���� �� ����
	for (int i = 0; i < MMATCH_GAMETYPE_MAX; i++)
	{
		m_GameTypeInfo[i].AddAllMap();
	}
}

MBaseGameTypeCatalogue::~MBaseGameTypeCatalogue()
{

}



