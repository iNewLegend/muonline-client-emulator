#ifndef _MBASEQUEST_H
#define _MBASEQUEST_H

// �̰����� ����Ʈ���� Ŭ���̾�Ʈ�� ������ �������� ����ϴ� ���� �ֵ��� �Ѵ�.
//////////////////////////////////////////////////////////////////////////////

#include "MQuestNPC.h"
#include "MQuestMap.h"
#include "MQuestScenario.h"
//#include "MSurvivalMap.h"
#include "MQuestDropTable.h"

#include <map>
using namespace std;


struct MQuestNPCInfo;

/// ����Ʈ ���巹������ ����ϴ� ���� ���
struct MQuestLevelSectorNode
{
	int		nSectorID;
	int		nNextLinkIndex;

	// ���⿡ �߰����� ����
};

////////////////////////////////////////////////////////////////////////////////////////////
/// ������ Ŭ���̾�Ʈ ����Ʈ �ְ� ����Ŭ������ ���� �θ� Ŭ����
class MBaseQuest
{
private:
	bool m_bCreated;
protected:
	
	MQuestMapCatalogue			m_MapCatalogue;								///< ����Ʈ �� ����
	MQuestMapCatalogue			m_SurvivalMapCatalogue;						///< �����̹� �� ����
	MQuestNPCCatalogue			m_NPCCatalogue;								///< NPC ����
	MQuestDropTable				m_DropTable;								///< ��� ���̺� ����
	virtual bool OnCreate();												///< Create()ȣ��� �Ҹ��� �Լ�
	virtual void OnDestroy();												///< Destroy()ȣ��� �Ҹ��� �Լ�

	void ProcessNPCDropTableMatching();		// npc.xml�� Droptable�� ��Ī��Ų��.
public:
	MBaseQuest();															///< ������
	virtual ~MBaseQuest();													///< �Ҹ���
	inline MQuestMapSectorInfo*		GetSectorInfo(int nSectorID);			///< ���� ���� ��ȯ - ����Ʈ
	inline MQuestMapSectorInfo*		GetSurvivalSectorInfo(int nSectorID);	///< ���� ���� ��ȯ - �����̹�
	inline MQuestNPCInfo*			GetNPCInfo(MQUEST_NPC nNPC);			///< NPC ���� ��ȯ
	inline MQuestNPCInfo*			GetNPCPageInfo( int nPage);				///< NPC ���� ��ȯ
	//inline MSurvivalMapInfo*		GetSurvivalMapInfo(MSURVIVAL_MAP nMap);	///< �����̹� ���� �� ���� ��ȯ
	inline MQuestDropTable*			GetDropTable();							///< ����Ʈ ������ ��� ���̺� ���� ��ȯ
	bool Create();															///< �ʱ�ȭ
	int GetNumOfPage( void)			{ return (int)m_NPCCatalogue.size(); }
	void Destroy();															///< ����

	MQuestNPCInfo* GetNPCIndexInfo( int nMonsterBibleIndex )
	{
		return m_NPCCatalogue.GetIndexInfo( nMonsterBibleIndex );
	}

	inline MQuestMapCatalogue* GetMapCatalogue();
	inline MQuestMapCatalogue* GetSurvivalMapCatalogue();
	inline MQuestNPCCatalogue* GetNPCCatalogue();
};





// inline functions //////////////////////////////////////////////////////////////////////////
inline MQuestMapSectorInfo* MBaseQuest::GetSectorInfo(int nSectorID)
{
	return m_MapCatalogue.GetSectorInfo(nSectorID);
}

inline MQuestMapSectorInfo* MBaseQuest::GetSurvivalSectorInfo(int nSectorID)
{
	return m_SurvivalMapCatalogue.GetSectorInfo(nSectorID);
}

inline MQuestNPCInfo* MBaseQuest::GetNPCInfo(MQUEST_NPC nNPC)
{
	return m_NPCCatalogue.GetInfo(nNPC);
}

inline MQuestNPCInfo* MBaseQuest::GetNPCPageInfo( int nPage)
{
	return m_NPCCatalogue.GetPageInfo(nPage);
}

//inline MSurvivalMapInfo* MBaseQuest::GetSurvivalMapInfo(MSURVIVAL_MAP nMap)
//{
//	return m_SurvivalMapCatalogue.GetInfo(nMap);
//}

inline MQuestDropTable* MBaseQuest::GetDropTable()
{
	return &m_DropTable;
}

inline MQuestMapCatalogue* MBaseQuest::GetMapCatalogue()
{
	return &m_MapCatalogue;
}

inline MQuestMapCatalogue* MBaseQuest::GetSurvivalMapCatalogue()
{
	return &m_SurvivalMapCatalogue;
}

inline MQuestNPCCatalogue* MBaseQuest::GetNPCCatalogue()
{
	return &m_NPCCatalogue;
}


#endif