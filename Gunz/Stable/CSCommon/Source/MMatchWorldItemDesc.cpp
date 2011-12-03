#include "stdafx.h"
#include "MMatchWorldItemDesc.h"
#include "MZFileSystem.h"
#include "FileInfo.h"
#include "MCRC32.h"

#define MWICTOK_WORLDITEM	"WORLDITEM"
#define MWICTOK_ID			"id"
#define MWICTOK_NAME		"name"
#define MWICTOK_TYPE		"TYPE"
#define MWICTOK_TIME		"TIME"
#define MWICTOK_AMOUNT		"AMOUNT"
#define MWICTOK_MODELNAME	"MODELNAME"



const DWORD MMatchMapsWorldItemSpawnInfoSet::GetCRC32()
{
	return MCRC32::BuildCRC32( (BYTE*)this, DWORD(sizeof(MMatchMapsWorldItemSpawnInfoSet)) );
}


MMatchWorldItemDescMgr::MMatchWorldItemDescMgr()
{
}

MMatchWorldItemDescMgr::~MMatchWorldItemDescMgr()
{
	Clear();
}

bool MMatchWorldItemDescMgr::ReadXml(const char* szFileName)
{
	MXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName))
	{
		xmlIniData.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MWICTOK_WORLDITEM))
		{
			ParseWorldItem(chrElement);
		}
	}

	xmlIniData.Destroy();
	return true;
}

bool MMatchWorldItemDescMgr::ReadXml(MZFileSystem* pFileSystem, const char* szFileName)
{
	MXmlDocument	xmlIniData;
	xmlIniData.Create();

	//	<-----------------
	char *buffer;
	MZFile mzf;

	if(pFileSystem) 
	{
		if(!mzf.Open(szFileName,pFileSystem)) 
		{
			if(!mzf.Open(szFileName)) 
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else 
	{
		if(!mzf.Open(szFileName))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
	mzf.Close();
	//	<------------------

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MWICTOK_WORLDITEM))
		{
			ParseWorldItem(chrElement);
		}
	}

	xmlIniData.Destroy();
	return true;
}

void MMatchWorldItemDescMgr::Clear()
{
	while (!empty())
	{
		MMatchWorldItemDesc* pDesc = (*begin()).second;
		delete pDesc; pDesc = NULL;
		erase(begin());
	}
}

MMatchWorldItemDesc* MMatchWorldItemDescMgr::GetItemDesc(short nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

MMatchWorldItemDescMgr* MMatchWorldItemDescMgr::GetInstance()
{
	static MMatchWorldItemDescMgr m_WorldItemDescMgr;
	return &m_WorldItemDescMgr;
}


void MMatchWorldItemDescMgr::ParseWorldItem(MXmlElement& element)
{
	MMatchWorldItemDesc* pNewWorldItemDesc = new MMatchWorldItemDesc;
	memset(pNewWorldItemDesc, 0, sizeof(MMatchWorldItemDesc));

	int n = 0;
	element.GetAttribute(&n, MWICTOK_ID);	pNewWorldItemDesc->m_nID = n;
	element.GetAttribute(pNewWorldItemDesc->m_szDescName, MWICTOK_NAME);	
	element.GetChildContents(pNewWorldItemDesc->m_szModelName, MWICTOK_MODELNAME);
		
	char szType[128];
	element.GetChildContents(szType, MWICTOK_TYPE);
	if (!stricmp(szType, "hp"))				pNewWorldItemDesc->m_nItemType = WIT_HP;
	else if (!stricmp(szType, "ap"))		pNewWorldItemDesc->m_nItemType = WIT_AP;
	else if (!stricmp(szType, "bullet"))	pNewWorldItemDesc->m_nItemType = WIT_BULLET;
	else if (!stricmp(szType, "quest"))		pNewWorldItemDesc->m_nItemType = WIT_QUEST;
	else if (!stricmp(szType, "hpap"))		pNewWorldItemDesc->m_nItemType = WIT_HPAP;
	else if (!stricmp(szType, "client"))	pNewWorldItemDesc->m_nItemType = WIT_CLIENT;	
	else pNewWorldItemDesc->m_nItemType = WIT_HP;


	int nTime = 0;
	element.GetChildContents(&nTime, MWICTOK_TIME); pNewWorldItemDesc->m_nTime = (unsigned long int)nTime;
	element.GetChildContents(&pNewWorldItemDesc->m_fAmount, MWICTOK_AMOUNT);

/*
	// ����������� bullet�� �ƴϸ� �߰����� �ʴ´�.
	if (pNewWorldItemDesc->m_nItemType != WIT_BULLET)
	{
		delete pNewWorldItemDesc;
		return;
	}
*/

	insert(value_type(pNewWorldItemDesc->m_nID, pNewWorldItemDesc));
}



/////////////////////////////////////////////////////////////////////////////////////////////
// MMatchMapsWorldItemSpawnInfo /////////////////////////////////////////////////////////////
#define FILENAME_WORLDITEM_SPAWN	"spawn.xml"

#define MMSITOK_GAMETYPE	"GAMETYPE"
#define MMSITOK_SPAWN		"SPAWN"
#define MMSITOK_ITEM		"item"
#define MMSITOK_POSITION	"POSITION"
#define MMSITOK_TIME		"timesec"
#define MMSITOK_ID			"id"
#define MMSITOK_NAME		"name"

MMatchMapsWorldItemSpawnInfo::MMatchMapsWorldItemSpawnInfo()
{
	Clear();
}
MMatchMapsWorldItemSpawnInfo::~MMatchMapsWorldItemSpawnInfo()
{
}

bool MMatchMapsWorldItemSpawnInfo::Read()
{
	char szFileName[256];
	for (int i = 0; i < MMATCH_MAP_COUNT; i++)
	{
		sprintf(szFileName, "Maps/%s/%s", MGetMapDescMgr()->GetMapName(i), FILENAME_WORLDITEM_SPAWN);

		if (IsExist(szFileName))
		{
			if (!ReadXml(szFileName, i))
			{
				return false;
			}
		}
	}
	
	return true;
}

bool MMatchMapsWorldItemSpawnInfo::ReadXml(const char* szFileName, int nMapID)
{
	MXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName))
	{
		xmlIniData.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MMSITOK_GAMETYPE))
		{
			ParseSpawnInfo(chrElement, nMapID);
		}
	}

	xmlIniData.Destroy();
	return true;
}

void MMatchMapsWorldItemSpawnInfo::Clear()
{
	memset(m_MapsSpawnInfo, 0, sizeof(MMatchMapsWorldItemSpawnInfoSet) * MMATCH_MAP_COUNT);
}

MMatchMapsWorldItemSpawnInfo* MMatchMapsWorldItemSpawnInfo::GetInstance()
{
	static MMatchMapsWorldItemSpawnInfo m_stMapsWISP;
	return &m_stMapsWISP;
}

void MMatchMapsWorldItemSpawnInfo::ParseSpawnInfo(MXmlElement& element, int nMapID)
{
	if ((nMapID < 0) || (nMapID >= MMATCH_MAP_COUNT)) return;

	MXmlElement spawnElement;
	char szGameTypeID[64] = "";
	element.GetAttribute(szGameTypeID, MMSITOK_ID);

	int nGameTypeChildCount = element.GetChildNodeCount();
	for (int j = 0; j < nGameTypeChildCount; j++)
	{
		spawnElement = element.GetChildNode(j);
		char szSpawnTagName[256];
		spawnElement.GetTagName(szSpawnTagName);
		if (szSpawnTagName[0] == '#') continue;
		
		
		int nItemID = 0;
		float x, y, z;
		int nTime = 0;

		char szItemName[256] = "";
		spawnElement.GetAttribute(szItemName, MMSITOK_ITEM);
		for (MMatchWorldItemDescMgr::iterator itor = MGetMatchWorldItemDescMgr()->begin();
			itor != MGetMatchWorldItemDescMgr()->end(); ++itor)
		{
			MMatchWorldItemDesc* pDesc = (*itor).second;
			if (!stricmp(pDesc->m_szDescName, szItemName))
			{
				nItemID = (*itor).first;
				break;
			}
		}

		spawnElement.GetAttribute(&nTime, MMSITOK_TIME);

		char szPosition[128];
		spawnElement.GetChildContents(szPosition, MMSITOK_POSITION);
		int nCnt = sscanf( szPosition, "%f %f %f", &x, &y, &z );
		if (nCnt != 3)
		{
			_ASSERT(0);
		}

		if (nItemID > 0)
		{
			SetMapsSpawnInfo(nMapID, szGameTypeID, nItemID, x, y, z, (unsigned long int)nTime);
		}
		
	}

}

void MMatchMapsWorldItemSpawnInfo::SetMapsSpawnInfo(int nMapID, char* szGameTypeID,
													int nItemID, float x, float y, float z, 
													unsigned long int nCoolTime)
{
	int nSpawnID;
	if (nCoolTime == 0) nCoolTime = 99999999;

	if (!stricmp(szGameTypeID, "solo"))
	{
		nSpawnID = m_MapsSpawnInfo[nMapID].m_nSoloSpawnCount;

		if (nSpawnID >= MAX_WORLDITEM_SPAWN) return;

		m_MapsSpawnInfo[nMapID].SoloSpawnInfo[nSpawnID].nItemID = nItemID;
		m_MapsSpawnInfo[nMapID].SoloSpawnInfo[nSpawnID].x = x;
		m_MapsSpawnInfo[nMapID].SoloSpawnInfo[nSpawnID].y = y;
		m_MapsSpawnInfo[nMapID].SoloSpawnInfo[nSpawnID].z = z;
		m_MapsSpawnInfo[nMapID].SoloSpawnInfo[nSpawnID].nCoolTime = nCoolTime;

		m_MapsSpawnInfo[nMapID].m_nSoloSpawnCount++;
	}
	else if (!stricmp(szGameTypeID, "team"))
	{
		nSpawnID = m_MapsSpawnInfo[nMapID].m_nTeamSpawnCount;
		if (nSpawnID >= MAX_WORLDITEM_SPAWN) return;

		m_MapsSpawnInfo[nMapID].TeamSpawnInfo[nSpawnID].nItemID = nItemID;
		m_MapsSpawnInfo[nMapID].TeamSpawnInfo[nSpawnID].x = x;
		m_MapsSpawnInfo[nMapID].TeamSpawnInfo[nSpawnID].y = y;
		m_MapsSpawnInfo[nMapID].TeamSpawnInfo[nSpawnID].z = z;
		m_MapsSpawnInfo[nMapID].TeamSpawnInfo[nSpawnID].nCoolTime = nCoolTime;

		m_MapsSpawnInfo[nMapID].m_nTeamSpawnCount++;
	}
}
