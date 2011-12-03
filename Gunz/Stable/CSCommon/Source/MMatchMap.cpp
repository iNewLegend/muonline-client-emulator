#include "stdafx.h"
#include "MMatchMap.h"



#define FILENAME_MAP		"map.xml"


#define MMAP_MAP			"MAP"

#define MMAP_ID				"id"
#define MMAP_NAME			"MapName"
#define MMAP_IMAGENAME		"MapImageName"
#define MMAP_BANNERNAME		"BannerName"
#define MMAP_EXPRATIO		"ExpRatio"
#define MMAP_MAXPLAYERS		"MaxPlayers"
#define MMAP_ONLYDUELMAP	"bOnlyDuelMap"


MMapDesc::MMapDesc()
{

}


MMapDesc* MMapDesc::GetInstance()
{
	static MMapDesc m_MapDescMgr;
	return &m_MapDescMgr; 
}

bool MMapDesc::Initialize(const char* szFileName)
{
	MXmlDocument	xmlIniData;	
	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName))
	{
		xmlIniData.Destroy();
		return false;
	}

	int iCount, num =0 ;
	MXmlElement		aParent, aChild;
	aParent = xmlIniData.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256]="";

	for (int i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,MMAP_MAP)==0)
		{
			if (szTagName[0] == '#') continue;

			_ASSERT( 0 <= num && num < MMATCH_MAP_COUNT);

			aChild.GetAttribute(&m_MapVectors[num].nMapID, MMAP_ID);
			aChild.GetAttribute(m_MapVectors[num].szMapName , MMAP_NAME);
			aChild.GetAttribute(m_MapVectors[num].szMapImageName , MMAP_IMAGENAME);
			aChild.GetAttribute(m_MapVectors[num].szBannerName , MMAP_BANNERNAME);
			aChild.GetAttribute(&m_MapVectors[num].fExpRatio , MMAP_EXPRATIO);
			aChild.GetAttribute(&m_MapVectors[num].nMaxPlayers , MMAP_MAXPLAYERS);
			aChild.GetAttribute(&m_MapVectors[num].bOnlyDuelMap , MMAP_ONLYDUELMAP);

			num++;
		}
	}

	return true;
}





bool MMapDesc::Initialize(MZFileSystem* pFileSystem, const char* szFileName)
{
	MXmlDocument	xmlIniData;	
	xmlIniData.Create();

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

	CHAR* buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		return false;
	}


	int iCount, num =0 ;
	MXmlElement		aParent, aChild;
	aParent = xmlIniData.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256]="";

	for (int i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,MMAP_MAP)==0)
		{
			if (szTagName[0] == '#') continue;
			
			aChild.GetAttribute(&m_MapVectors[num].nMapID, MMAP_ID);
			aChild.GetAttribute(m_MapVectors[num].szMapName , MMAP_NAME);
			aChild.GetAttribute(m_MapVectors[num].szMapImageName , MMAP_IMAGENAME);
			aChild.GetAttribute(m_MapVectors[num].szBannerName , MMAP_BANNERNAME);
			aChild.GetAttribute(&m_MapVectors[num].fExpRatio , MMAP_EXPRATIO);
			aChild.GetAttribute(&m_MapVectors[num].nMaxPlayers , MMAP_MAXPLAYERS);
			aChild.GetAttribute(&m_MapVectors[num].bOnlyDuelMap , MMAP_ONLYDUELMAP);
			
			num++;
		}
	}
    
	if(buffer)
	{
		delete[] buffer;
		buffer = NULL; 
	}

	return true;
}


int MMapDesc::GetMaxPlayers( const int nMapID)
{
	if (MIsCorrectMap(nMapID))
		return m_MapVectors[nMapID].nMaxPlayers;
	else
		return 0;
}



int MMapDesc::GetMapID( const int nMapID)
{
	if (MIsCorrectMap(nMapID))
		return m_MapVectors[nMapID].nMapID;
	else
		return 0;  
}

float MMapDesc::GetExpRatio( const int nMapID)
{
	if (MIsCorrectMap(nMapID))
		return m_MapVectors[nMapID].fExpRatio;
	else
		return 0;
}


bool MMapDesc::MIsCorrectMap(const int nMapID)
{
	if ((nMapID < 0) || (nMapID >= MMATCH_MAP_MAX)) return false;
	return true;
}


const char* MMapDesc::GetMapName(const int nMapID)
{
	if (MIsCorrectMap(nMapID))
		return m_MapVectors[nMapID].szMapName;
	else
		return 0;
}


const char* MMapDesc::GetMapImageName(const char* szMapName)
{
	for ( int i = 0;  i < MMATCH_MAP_COUNT;  i++)
	{
		if ( stricmp( szMapName, m_MapVectors[ i].szMapName) == 0)
			return m_MapVectors[ i].szMapImageName;
	}

	return 0;
}

const char* MMapDesc::GetBannerName(const char* szMapName)
{
	for ( int i = 0;  i < MMATCH_MAP_COUNT;  i++)
	{
		if ( stricmp( szMapName, m_MapVectors[ i].szMapName) == 0)
			return m_MapVectors[ i].szBannerName;
	}

	return 0;
}

bool MMapDesc::IsMapOnlyDuel( const int nMapID)
{
	if ( MIsCorrectMap(nMapID))
		return m_MapVectors[ nMapID].bOnlyDuelMap;
	else
		return 0;
}