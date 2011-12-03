#ifndef _MMATCHWORLDITEMDESC_H
#define _MMATCHWORLDITEMDESC_H

#include "MXml.h"
#include <map>
using namespace std;

#define WORLDITEM_NAME_LENGTH		256

enum MMATCH_WORLD_ITEM_TYPE
{
	WIT_HP			= 0,
	WIT_AP			= 1,
	WIT_BULLET		= 2,
	WIT_HPAP		= 3,
	WIT_CLIENT		= 4,	// Ŭ���̾�Ʈ ���� ���������

	WIT_QUEST		= 5,	// ����Ʈ ������ �ڽ�
	WIT_BOUNTY		= 6,	// ����Ʈ���� ������ �ٿ�Ƽ ������

	WIT_END
};


struct MMatchWorldItemDesc
{
	short					m_nID;
	MMATCH_WORLD_ITEM_TYPE	m_nItemType;
	float					m_fAmount;
	unsigned long int		m_nTime;
	char					m_szModelName[WORLDITEM_NAME_LENGTH];
	char					m_szDescName[WORLDITEM_NAME_LENGTH];
};

class MMatchWorldItemDescMgr;
class MZFileSystem;


/// ���� ������ Ÿ�� ���
class MMatchWorldItemDescMgr : public map<short, MMatchWorldItemDesc*>
{
private:
protected:
	void ParseWorldItem(MXmlElement& element);
public:
	MMatchWorldItemDescMgr();
	virtual ~MMatchWorldItemDescMgr();
	bool ReadXml(const char* szFileName);
	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	void Clear();
	MMatchWorldItemDesc* GetItemDesc(short nID);
	static MMatchWorldItemDescMgr* GetInstance();
};

inline MMatchWorldItemDescMgr* MGetMatchWorldItemDescMgr() 
{ 
	return MMatchWorldItemDescMgr::GetInstance();
}


/////////////////////////////////////////////////////////////////////////////////////


#include "MMatchWorldItem.h"
#include "MMatchStage.h"

#define MAX_WORLDITEM_SPAWN		100

// �� �ʿ��� ������ �ִ� ��������
struct MMatchMapsWorldItemSpawnInfoSet
{
	MMatchWorldItemSpawnInfo	SoloSpawnInfo[MAX_WORLDITEM_SPAWN];
	MMatchWorldItemSpawnInfo	TeamSpawnInfo[MAX_WORLDITEM_SPAWN];
	int							m_nSoloSpawnCount;
	int							m_nTeamSpawnCount;

	const DWORD					GetCRC32();
};

// ��ü ���� ��������
class MMatchMapsWorldItemSpawnInfo
{
private:
	void ParseSpawnInfo(MXmlElement& element, int nMapID);
	void SetMapsSpawnInfo(int nMapID, char* szGameTypeID, int nItemID, float x, float y, float z, 
						  unsigned long int nCoolTime);
	bool ReadXml(const char* szFileName, int nMapID);
protected:
public:
	MMatchMapsWorldItemSpawnInfoSet		m_MapsSpawnInfo[MMATCH_MAP_COUNT];
	MMatchMapsWorldItemSpawnInfo();
	virtual ~MMatchMapsWorldItemSpawnInfo();
	
	bool Read();
	void Clear();
	static MMatchMapsWorldItemSpawnInfo* GetInstance();
};



inline MMatchMapsWorldItemSpawnInfo* MGetMapsWorldItemSpawnInfo() 
{ 
	return MMatchMapsWorldItemSpawnInfo::GetInstance();
}

#endif