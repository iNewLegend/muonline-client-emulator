#pragma once
#include "InterfacePlug.h"
#include "iScene.h"

int getMapIDFromFilename(const std::string& strFilename);
class CMyPlug : public CDataPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);

	virtual const char * getTitle(){return "Mu Scene File";}
	virtual const char * getFormat() {return ".obj";}
	virtual bool importData(iRenderNode* pRenderNode, const char* szFilename);
	virtual void release();
private:
	bool importObjectResources(const char* szFilename, const std::string& strPath);
	bool importObjectResourcesFormDir(const std::string& strPath);

	//bool exportObjectResources(iScene * pScene, const std::string& strFilename, const std::string& strPath);
	//bool exportObjectResourcesFormDir(iScene * pScene,const std::string& strPath);
	bool exportObject(iRenderNode * pScene, const std::string& strFilename);

	std::map<int,std::string>	m_mapObjectName;
	bool importSceneTerrainData(iRenderNode* pRenderNode, iSceneData * pTerrainData,  const char* szFilename);
	bool importTerrainData(iSceneData * pTerrainData, const std::string& strFilename);

	bool exportTerrainAtt(iSceneData * pTerrainData, const std::string& strFilename);
	bool exportTerrainLightmap(iSceneData * pTerrainData, const std::string& strFilename);
	bool exportTerrainHeight(iSceneData * pTerrainData, const std::string& strFilename);
	bool exportTerrainData(iSceneData * pTerrainData, const std::string& strFilename);
	bool exportTiles(iSceneData * pTerrain, const std::string& strFilename, const std::string& strPath);
};