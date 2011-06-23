#pragma once
#include "InterfaceScene.h"

class CMyPlug : public CModelPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);

	virtual const char * getTitle(){return "Mu Scene File";}
	virtual const char * getFormat() {return ".map";}
	virtual iRenderNode* importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename);
	virtual void release();
private:
	bool importTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);
	bool importObjectResources(const char* szFilename, const std::string& strPath);
	bool importObjectResourcesFormDir(const std::string& strPath);

	bool exportTerrainAtt(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainLightmap(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainHeight(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTiles(iTerrainData * pTerrain, const std::string& strFilename, const std::string& strPath);
	//bool exportObjectResources(iScene * pScene, const std::string& strFilename, const std::string& strPath);
	//bool exportObjectResourcesFormDir(iScene * pScene,const std::string& strPath);
	bool exportObject(iRenderNode * pScene, const std::string& strFilename);

	std::map<int,std::string>	m_mapObjectName;
};