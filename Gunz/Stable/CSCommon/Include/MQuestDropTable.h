#ifndef _MQUESTDROPTABLE_H
#define _MQUESTDROPTABLE_H

#include "MQuestConst.h"

/// ��� ������ Ÿ��
enum MQuestDropItemType
{
	QDIT_NA			= 0,	///< �������� ����
	QDIT_WORLDITEM	= 1,	///< HP, AP���� �Ϲ����� ���������
	QDIT_QUESTITEM	= 2,	///< ����Ʈ ������
	QDIT_ZITEM		= 3,	///< �Ϲ� ������
};

/// ��� ������ ����
struct MQuestDropItem
{
	MQuestDropItemType	nDropItemType;
	int					nID;
	int					nRentPeriodHour;
	// int					nMonsetBibleIndex;	// � ������ ���Ͱ� �������� ����߷ȴ��� �� �������� ������ �ִ� ������ ������ ����.
											// ���� ������ ���ؼ� ���.

	MQuestDropItem() : nDropItemType(QDIT_NA), nID(0), nRentPeriodHour(0) {}
	void Assign(MQuestDropItem* pSrc)		// ����
	{
		nDropItemType	= pSrc->nDropItemType;
		nID				= pSrc->nID;
		nRentPeriodHour = pSrc->nRentPeriodHour;
	}
};

#define MAX_DROPSET_RATE		1000		///< ��� ���� 0.001���� ���� ����

/// ��� ������ ��
class MQuestDropSet
{
private:
	int						m_nID;
	char					m_szName[16];
	MQuestDropItem			m_DropItemSet[MAX_QL+1][MAX_DROPSET_RATE];
	int						m_nTop[MAX_QL+1];
	set<int>				m_QuestItems;			// �� ��Ʈ�� ������ �ִ� ����Ʈ ������ ��Ʈ - Ŭ���̾�Ʈ�� ����Ϸ��� ����
public:
	/// ������
	MQuestDropSet()
	{
		m_nID = 0;
		m_szName[0] = 0;
		memset(m_DropItemSet, 0, sizeof(m_DropItemSet));
		for (int i = 0; i <= MAX_QL; i++)
		{
			m_nTop[i] = 0;
		}
	}
	int GetID() { return m_nID; }									///< ID ��ȯ
	const char* GetName() { return m_szName; }						///< �̸� ��ȯ
	void SetID(int nID) { m_nID = nID; }							///< ID ����
	void SetName(const char* szName) { strcpy(m_szName, szName); }	///< �̸� ����
	/// ��ӵǴ� ������ �߰�
	/// @param pItem		��ӵ� ������ ����
	/// @param nQL			����Ʈ ����
	/// @param fRate		���� ����
	void AddItem(MQuestDropItem* pItem, int nQL, float fRate);
	/// ��ӵ� �������� �����Ѵ�.
	/// @param outDropItem		��ӵ� ������ ��ȯ��
	/// @param nQL				����Ʈ ����
	bool Roll(MQuestDropItem& outDropItem, int nQL);

	set<int>& GetQuestItems() { return m_QuestItems; }				///< ��ӵ� ������ ����
};


/// ��� ���̺� ������ Ŭ����
class MQuestDropTable : public map<int, MQuestDropSet*>
{
private:
	void ParseDropSet(MXmlElement& element);
	void ParseDropItemID(MQuestDropItem* pItem, const char* szAttrValue);
public:
	MQuestDropTable();													///< ������
	~MQuestDropTable();													///< �Ҹ���

	void Clear();
	
	bool ReadXml(const char* szFileName);								///< xml���� ������ �д´�. 
	bool ReadXml(MZFileSystem* pFileSystem,const char* szFileName);		///< xml���� ������ �д´�. 
	/// ��� ���̺� ID�� QL�� �������� ��ӵ� �������� �����Ѵ�.
	/// @param outDropItem		��ӵ� ������ ��ȯ��
	/// @param nDropTableID		��� ���̺� ID
	/// @param nQL				����Ʈ ����
	bool Roll(MQuestDropItem& outDropItem, int nDropTableID, int nQL);
	MQuestDropSet* Find(int nDropTableID);								///< ��� ������ �� ���� ��ȯ
};

#endif