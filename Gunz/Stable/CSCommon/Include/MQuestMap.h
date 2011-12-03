#ifndef _MQUEST_MAP_H
#define _MQUEST_MAP_H


#include "MQuestConst.h"

/// ���� ��ũ ����
struct MQuestSectorLink
{
	char			szName[64];					///< �̸�
	vector<int>		vecTargetSectors;			///< ����� �� �ִ� ���� ����
	MQuestSectorLink() 
	{
		szName[0] = 0;
	}
};


/// ���� ����ũ ����
struct MQuestSectorBacklink
{
	int nSectorID;
	int nLinkIndex;
};


/// ����Ʈ �� ���� ����
struct MQuestMapSectorInfo
{
	int								nID;								///< ID
	char							szTitle[64];						///< ���� �̸�
	bool							bBoss;								///< ������ ����
	int								nLinkCount;							///< ��ũ��
	MQuestSectorLink				Links[MAX_SECTOR_LINK];				///< ��ũ ����
	vector<MQuestSectorBacklink>	VecBacklinks;						///< ����ũ��
	int								nSpawnPointCount[MNST_END];			///< ��������Ʈ ����

	MQuestMapSectorInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nLinkCount = 0;
		bBoss = false;
		memset(nSpawnPointCount, 0, sizeof(nSpawnPointCount));
	}
};


/// ����Ʈ �� ��Ʈ ����
struct MQuestMapsetInfo
{
	int				nID;									///< ID
	char			szTitle[64];							///< ��Ʈ �̸�
	int				nLinkCount;								///< ��ũ��
	vector<int>		vecSectors;								///< ������ �ִ� ����

	MQuestMapsetInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nLinkCount = 0;
	}
};

typedef	std::map<int, MQuestMapsetInfo*>		MQuestMapsetMap;
typedef	std::map<int, MQuestMapSectorInfo*>		MQuestMapSectorMap;

/// ����Ʈ �� ������
class MQuestMapCatalogue
{
private:
	// ��� ����
	MQuestMapsetMap			m_MapsetInfo;
	MQuestMapSectorMap		m_SectorInfo;

	// �Լ�
	void InsertMapset(MQuestMapsetInfo* pMapset);
	void InsertSector(MQuestMapSectorInfo* pSector);
	void ParseMapset(MXmlElement& element);
	void ParseMapsetSector1Pass(MXmlElement& elementMapset, MQuestMapsetInfo* pMapset);
	void ParseSector(MXmlElement& element, MQuestMapSectorInfo* pSector);
	void InitBackLinks();
public:
	MQuestMapCatalogue();													///< ������
	~MQuestMapCatalogue();													///< �Ҹ���

	void Clear();															///< �ʱ�ȭ
	bool ReadXml(const char* szFileName);									///< xml���� �������� �д´�. (������)
	bool ReadXml(MZFileSystem* pFileSystem,const char* szFileName);			///< xml���� �������� �д´�. (Ŭ���̾�Ʈ��)
	void DebugReport();														///< �������� ����� �����Ǿ����� Ȯ���Ѵ�.

	MQuestMapSectorInfo*	GetSectorInfo(int nSector);						///< ���� ���� ��ȯ
	MQuestMapsetInfo*		GetMapsetInfo(int nMapset);						///< �ʼ� ���� ��ȯ
	MQuestMapSectorInfo*	GetSectorInfoFromName(char* szSectorTitle);		///< ���� �̸����� ���� ���� ��ȯ

	inline MQuestMapsetMap*		GetMapsetMap();

	bool IsHacked();											///< �ӽ���ŷ������..
};


inline MQuestMapsetMap* MQuestMapCatalogue::GetMapsetMap()
{
	return &m_MapsetInfo;
}


#endif