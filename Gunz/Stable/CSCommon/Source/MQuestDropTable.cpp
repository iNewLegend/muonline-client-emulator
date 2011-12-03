#include "stdafx.h"
#include "MQuestDropTable.h"
#include "MQuestItem.h"
#include "MMath.h"
#include "MZFileSystem.h"

void MQuestDropSet::AddItem(MQuestDropItem* pItem, int nQL, float fRate)
{
	if ((pItem->nDropItemType == QDIT_ZITEM) && (pItem->nRentPeriodHour == 0))
	{
		_ASSERT(0);	// �Ⱓ ������ �ȵǾ� �ִ�.
		return;
	}

	if (fRate > 1.0f) 
	{
		_ASSERT(0);		// ������ �߸��Ǿ� �ִ�.
		return;
	}

	int nRate = (int)(fRate * MAX_DROPSET_RATE);

	int idx = m_nTop[nQL];
	for (int i=0; i < nRate; i++)
	{
		if (idx < MAX_DROPSET_RATE)
		{
			m_DropItemSet[nQL][idx].Assign(pItem);

			m_nTop[nQL]++;
			idx++;
		}
	}

	if (pItem->nDropItemType == QDIT_QUESTITEM)
	{
		m_QuestItems.insert(pItem->nID);
	}
}


bool MQuestDropSet::Roll(MQuestDropItem& outDropItem, int nQL)
{
	int idx = RandomNumber(0, MAX_DROPSET_RATE-1);
	if (idx >= m_nTop[nQL]) return false;

	outDropItem.Assign(&m_DropItemSet[nQL][idx]);

	return true;
}


//////////////////////////////////////////////////////////////////////////////////

MQuestDropTable::MQuestDropTable()
{

}

MQuestDropTable::~MQuestDropTable()
{
	Clear();
}


//////////////////////////////////////////////////////

#define MTOK_DROPSET						"DROPSET"

// �⺻����
#define MTOK_DROPSET_ITEMSET				"ITEMSET"
#define MTOK_DROPSET_ITEM					"ITEM"
//#define MTOK_DROPSET_ZITEM					"ZITEM"
//#define MTOK_DROPSET_QUESTITEM				"QUESTITEM"
//#define MTOK_DROPSET_GAMEITEM				"GAMEITEM"

#define MTOK_DROPSET_ATTR_ID				"id"
#define MTOK_DROPSET_ATTR_QL				"QL"
#define MTOK_DROPSET_ATTR_NAME				"name"
#define MTOK_DROPSET_ATTR_RATE				"rate"
#define MTOK_DROPSET_ATTR_RENT_PERIOD		"rent_period"

//////////////////////////////////////////////////////

bool MQuestDropTable::ReadXml(MZFileSystem* pFileSystem,const char* szFileName)
{
	MXmlDocument	xmlIniData;
	xmlIniData.Create();

	char *buffer;
	MZFile mzf;

	if(pFileSystem) {
		if(!mzf.Open(szFileName,pFileSystem))  {
			if(!mzf.Open(szFileName))  {
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else  {

		if(!mzf.Open(szFileName)) {

			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer)) {
		xmlIniData.Destroy();
		return false;
	}

	delete[] buffer;
	mzf.Close();


	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_DROPSET)) {
			ParseDropSet(chrElement);
		}
	}

	xmlIniData.Destroy();

	return true;

}

bool MQuestDropTable::ReadXml(const char* szFileName)
{
	MXmlDocument xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName)) {
		xmlIniData.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;

	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_DROPSET))
		{
			ParseDropSet(chrElement);
		}
	}

	xmlIniData.Destroy();
	return true;
}


void MQuestDropTable::ParseDropSet(MXmlElement& element)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];

	MQuestDropSet* pDropSet = new MQuestDropSet();

	// NPC �±� �Ӽ��� --------------------
	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		element.GetAttribute(i, szAttrName, szAttrValue);
		if (!stricmp(szAttrName, MTOK_DROPSET_ATTR_ID))
		{
			int nID = atoi(szAttrValue);
			pDropSet->SetID(nID);
		}
		else if (!stricmp(szAttrName, MTOK_DROPSET_ATTR_NAME))
		{
			pDropSet->SetName(szAttrValue);
		}
	}	

	int iChildCount = element.GetChildNodeCount();
	MXmlElement chrElement;
	for (int i = 0; i < iChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		int nQL = -1;
		// ITEMSET �±� --------------------
		if (!stricmp(szTagName, MTOK_DROPSET_ITEMSET))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_DROPSET_ATTR_QL))
				{
					nQL = atoi(szAttrValue);

					int nChildItemCount = chrElement.GetChildNodeCount();
					MXmlElement ItemElement;
					for (int i = 0; i < nChildItemCount; i++)
					{
						ItemElement = chrElement.GetChildNode(i);
						ItemElement.GetTagName(szTagName);
						if (szTagName[0] == '#') continue;

						// GAMEITEM �±� --------------------
						if (!stricmp(szTagName, MTOK_DROPSET_ITEM))
						{
							MQuestDropItem item;
							float fRate = 0.0f;
							int nRentPeriodHour = 0;

							int nAttrCount = ItemElement.GetAttributeCount();
							for (int i = 0; i < nAttrCount; i++)
							{
								ItemElement.GetAttribute(i, szAttrName, szAttrValue);
								if (!stricmp(szAttrName, MTOK_DROPSET_ATTR_ID))
								{
									ParseDropItemID(&item, szAttrValue);
								}
								else if (!stricmp(szAttrName, MTOK_DROPSET_ATTR_RATE))
								{
									fRate = (float)atof(szAttrValue);
								}
								else if (!stricmp(szAttrName, MTOK_DROPSET_ATTR_RENT_PERIOD))
								{
									item.nRentPeriodHour = atoi(szAttrValue);
								}
							}

							if (nQL >= 0)
							{
								pDropSet->AddItem(&item, nQL, fRate);
							}
						}
					}
				}
			}
		}
	}


	insert(value_type(pDropSet->GetID(), pDropSet));
}


void MQuestDropTable::ParseDropItemID(MQuestDropItem* pItem, const char* szAttrValue)
{
	// id�� �׳� �ϵ��ڵ�..-_-��
	if (!stricmp(szAttrValue, "hp1"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 1;
	}
	else if (!stricmp(szAttrValue, "hp2"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 2;
	}
	else if (!stricmp(szAttrValue, "hp3"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 3;
	}
	else if (!stricmp(szAttrValue, "ap1"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 4;
	}
	else if (!stricmp(szAttrValue, "ap2"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 5;
	}
	else if (!stricmp(szAttrValue, "ap3"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 6;
	}
	else if (!stricmp(szAttrValue, "mag1"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 7;
	}
	else if (!stricmp(szAttrValue, "mag2"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 8;
	}
	else
	{
		// ���� �������� �ƴϸ� ������
		int nID = atoi(szAttrValue);

		if (IsQuestItemID(nID))
		{
			// ����Ʈ ������
			MQuestItemDesc* pQuestItemDesc = GetQuestItemDescMgr().FindQItemDesc(nID);
			if (pQuestItemDesc)
			{
				pItem->nDropItemType = QDIT_QUESTITEM;
				pItem->nID = nID;
			}
			else
			{
				_ASSERT(0);
			}
		}
		else
		{
			// �Ϲ� ������
			MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nID);
			if (pItemDesc)
			{
				pItem->nDropItemType = QDIT_ZITEM;
				pItem->nID = nID;
			}
			else
			{
				_ASSERT(0);
			}
		}
	}
}




bool MQuestDropTable::Roll(MQuestDropItem& outDropItem, int nDropTableID, int nQL)
{
	iterator itor = find(nDropTableID);
	if (itor != end())
	{
		MQuestDropSet* pDropSet = (*itor).second;
		return pDropSet->Roll(outDropItem, nQL);
	}

	return false;
}

void MQuestDropTable::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();

}

MQuestDropSet* MQuestDropTable::Find(int nDropTableID)
{
	iterator itor = find(nDropTableID);
	if (itor != end())
	{
		return (*itor).second;
	}

	return NULL;
}