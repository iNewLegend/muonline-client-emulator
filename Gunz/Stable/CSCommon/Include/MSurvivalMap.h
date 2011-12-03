#ifndef _MSURVIVAL_MAP_H
#define _MSURVIVAL_MAP_H
//// ������� ����

#include "MMatchGlobal.h"


// �����̹� �� ����
enum MSURVIVAL_MAP
{
	MSURVIVAL_MAP_HALL2 = 0,
	MSURVIVAL_MAP_ROOM3,

	MSURVIVAL_MAP_END
};

// �����̹� �� ����
struct MSurvivalMapInfo
{
	MSURVIVAL_MAP		nID;

	// �� �̸��� ���ǵ� ���̷� ����. - by SungE 2007-04-02
	// char				szName[64];
	char				szName[ MAPNAME_LENGTH ];
};


class MSurvivalMapCatalogue
{
private:
	// ��� ����
	MSurvivalMapInfo		m_MapInfo[MSURVIVAL_MAP_END];

	// �Լ�
	void SetMap(MSURVIVAL_MAP nMap, const char* szMapName);
	void Clear();
public:
	MSurvivalMapCatalogue();
	~MSurvivalMapCatalogue();
	MSurvivalMapInfo* GetInfo(MSURVIVAL_MAP nMap);
};



#endif