#ifndef _MQUEST_SCENARIO_H
#define _MQUEST_SCENARIO_H


#include "MQuestConst.h"

struct MQuestScenarioInfoMapJaco
{
	MQUEST_NPC	nNPCID;
	float		fRate;
};


struct MQuestScenarioInfoMaps
{
	int										nKeySectorID;			///< ���� ���� ID
	int										nKeyNPCID;				///< Ű NPC ID (����Ʈ��忡�� ���� ���Ϳ� �����ϴ� ������NPC)
	bool									bKeyNPCIsBoss;			///< Ű NPC�� �������� ����
	vector<int>								vecNPCSetArray;			///< NPC Set Array

	vector<int>								vecKeyNPCArray;			///< �� ������ KeyNpc�� (�����̹� ����)
	vector<int>								vecSectorXpArray;		///< �� ������ XP (�����̹� ����)
	vector<int>								vecSectorBpArray;		///< �� ������ BP (�����̹� ����)

	// �����濡�� �� ����
	int										nJacoCount;				///< 1ȸ ������ ������ ������
	unsigned int							nJacoSpawnTickTime;		///< ���� ���� ƽ Ÿ��
	int										nJacoMinNPCCount;		///< �̰������϶��� ������ �������� �ʴ´�.
	int										nJacoMaxNPCCount;		///< �̰������϶��� ������ �������� �ʴ´�.
	vector<MQuestScenarioInfoMapJaco>		vecJacoArray;			///< �����濡�� ���� ������

	MQuestScenarioInfoMaps()
	{
		nKeySectorID = 0;
		nKeyNPCID = 0;
		bKeyNPCIsBoss = false;
		nJacoCount = 0;
		nJacoSpawnTickTime = 9999999;
		nJacoMinNPCCount = 0;
		nJacoMaxNPCCount = 0;
	}
};

/// ����Ʈ �ó����� ����
struct MQuestScenarioInfo
{
	int				nID;										///< �ó����� ID
	char			szTitle[64];								///< �ó����� �̸�
	int				nQL;										///< �䱸 ����Ʈ ����
	float			fDC;										///< ���̵� ���(DC)
	int				nResSacriItemCount;							///< �ó������� ���� ��� ������ ����
	unsigned int	nResSacriItemID[MAX_SCENARIO_SACRI_ITEM];	///< �ó������� ���� ��� ������
	int				nMapSet;									///< �ʼ�
	bool			bSpecialScenario;							///< Ư���ó��������� ����

	int				nXPReward;									///< XP ����ġ
	int				nBPReward;									///< BP ����ġ
	int				nRewardItemCount;							///< Ư�� ������ ����
	int				nRewardItemID[MAX_SCENARIO_REWARD_ITEM];	///< Ư�� ������ ����
	float			fRewardItemRate[MAX_SCENARIO_REWARD_ITEM];	///< Ư�� ������ ���� Ȯ��
	int				nSectorXP;									///< ���ͺ� ���ʽ� XP ����ġ
	int				nSectorBP;									///< ���ͺ� ���ʽ� BP ����ġ
	int				nRepeat;									///< �ó����� �ݺ� Ƚ�� (�����̹���)
	int				nMaxSpawn;									///< ������ �ִ� NPC ���� Ƚ�� (�����̹���)
	int				nMaxSpawnSameTime;							///< ������ �ִ� ���� NPC ������ (�����̹���)

	MQuestScenarioInfoMaps		Maps[SCENARIO_STANDARD_DICE_SIDES];

	/// ������
	MQuestScenarioInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nQL = 0;
		fDC = 0.0f;
		nResSacriItemCount = 0;
		memset(nResSacriItemID, 0, sizeof(nResSacriItemID));
		nMapSet = 0;
		nXPReward = 0;
		nBPReward = 0;
		nRewardItemCount = 0;
		memset(fRewardItemRate, 0, sizeof(fRewardItemRate));
		bSpecialScenario = false;

		for (int i = 0; i < SCENARIO_STANDARD_DICE_SIDES; i++)
		{
			Maps[i].nKeySectorID = 0;
			Maps[i].nKeyNPCID = 0;
		}

		nSectorXP = -1;
		nSectorBP = -1;
		nRepeat = 0;
		nMaxSpawn = 1;
		nMaxSpawnSameTime = 1;
	}

	/// ���� �� ��ȯ
	int GetSectorCount(int nDice)
	{
		return (int)Maps[nDice].vecNPCSetArray.size();
	}
};

/// �ó����� ���� ������
class MQuestScenarioCatalogue : public map<int, MQuestScenarioInfo*>
{
private:
	// ��� ����
	int		m_nDefaultStandardScenarioID;
	// �Լ�
	void Clear();
	void Insert(MQuestScenarioInfo* pScenarioInfo);
	void ParseSpecialScenario(MXmlElement& element);
	void ParseStandardScenario(MXmlElement& element);
	void ParseNPCSetArray(MXmlElement& element, vector<int>& vec);
	void ParseKeyNPCArray(MXmlElement& element, vector<int>& vec);
	void ParseSectorXpBpArray(MXmlElement& element, vector<int>& vec);
	void ParseJaco(MXmlElement& element, MQuestScenarioInfoMaps* pMap);
	void ParseRewardItem(MXmlElement& element, MQuestScenarioInfo* pScenarioInfo);
	void ParseSacriItem(MXmlElement& element, MQuestScenarioInfo* pScenarioInfo);
	void ParseMap(MXmlElement& element, MQuestScenarioInfo* pScenarioInfo);
	int CalcStandardScenarioID(int nMapsetID, int nQL);
public:
	MQuestScenarioCatalogue();											///< ������
	~MQuestScenarioCatalogue();											///< �Ҹ���

	bool ReadXml(const char* szFileName);								///< xml�κ��� npc������ �д´�.
	bool ReadXml(MZFileSystem* pFileSystem,const char* szFileName);		///< xml�κ��� npc������ �д´�.
	

	MQuestScenarioInfo* GetInfo(int nScenarioID);						///< �ó����� ���� ��ȯ
	/// ���� �ó����� ��ȯ
	/// @param nQL				����Ʈ ����
	/// @param nDice			�ֻ��� ����
	int GetStandardScenarioID(int nMapsetID, int nQL);

	/// Ư�� �ó����� �˻�
	/// @param nMapsetID		�ʼ�
	/// @param nQL				����Ʈ ����
	bool FindSpecialScenarioID(int nMapsetID, int nPlayerQL, unsigned int* SacriQItemIDs, unsigned int* outScenarioID);

	unsigned int MakeScenarioID(int nMapsetID, int nPlayerQL, unsigned int* SacriQItemIDs);


	const int GetDefaultStandardScenarioID() { return m_nDefaultStandardScenarioID; }
};


#endif