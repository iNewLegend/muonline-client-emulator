#ifndef _MMATCHQUEST_H
#define _MMATCHQUEST_H

#include "MBaseQuest.h"
#include "MSurvivalRankInfo.h"

/// ������ ����Ʈ �ְ� ���� Ŭ����
class MMatchQuest : public MBaseQuest
{
protected:
	virtual bool OnCreate();				///< �ʱ�ȭ
	virtual void OnDestroy();				///< ����
	

	MQuestNPCSetCatalogue		m_NPCSetCatalogue;						///< NPC Set ����
	MQuestScenarioCatalogue		m_ScenarioCatalogue;					///< ����Ʈ �ó����� ����
	MQuestScenarioCatalogue		m_SurvivalScenarioCatalogue;			///< �����̹� �ó����� ����
	MSurvivalRankInfo			m_SurvivalRankInfo;						///< �����̹� ������ ��ŷ ���

public:
	MMatchQuest();														///< ������
	virtual ~MMatchQuest();												///< �Ҹ���

	inline MQuestNPCSetInfo* GetNPCSetInfo(int nID);					///< NPC Set ���� ��ȯ
	inline MQuestNPCSetInfo* GetNPCSetInfo(const char* szName);			///< NPC Set ���� ��ȯ
	inline MQuestScenarioCatalogue* GetScenarioCatalogue();				///< ����Ʈ �ó����� ���� ��ȯ
	inline MQuestScenarioInfo*		GetScenarioInfo(int nScenarioID);	///< ����Ʈ �ó����� ���� ��ȯ
	inline MQuestScenarioCatalogue* GetSurvivalScenarioCatalogue();				///< �����̹� �ó����� ���� ��ȯ
	inline MQuestScenarioInfo*		GetSurvivalScenarioInfo(int nScenarioID);	///< �����̹� �ó����� ���� ��ȯ
	inline MSurvivalRankInfo*		GetSurvivalRankInfo();						///< �����̹� ��ŷ ��� ��ȯ

};




inline MQuestNPCSetInfo* MMatchQuest::GetNPCSetInfo(int nID)
{
	return m_NPCSetCatalogue.GetInfo(nID);
}

inline MQuestNPCSetInfo* MMatchQuest::GetNPCSetInfo(const char* szName)
{
	return m_NPCSetCatalogue.GetInfo(szName);
}

inline MQuestScenarioCatalogue* MMatchQuest::GetScenarioCatalogue()
{
	return &m_ScenarioCatalogue;
}

inline MQuestScenarioInfo* MMatchQuest::GetScenarioInfo(int nScenarioID)
{
	return m_ScenarioCatalogue.GetInfo(nScenarioID);
}

inline MQuestScenarioCatalogue* MMatchQuest::GetSurvivalScenarioCatalogue()
{
	return &m_SurvivalScenarioCatalogue;
}

inline MQuestScenarioInfo* MMatchQuest::GetSurvivalScenarioInfo(int nScenarioID)
{
	return m_SurvivalScenarioCatalogue.GetInfo(nScenarioID);
}

inline MSurvivalRankInfo* MMatchQuest::GetSurvivalRankInfo()
{
	return &m_SurvivalRankInfo;
}

#endif