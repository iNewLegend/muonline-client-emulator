#pragma once
#include "InterfacePlug.h"
#include "iScene.h"

class CMyPlug : public CDataPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);

	virtual const char * getTitle(){return "Mu Terrain File";}
	virtual const char * getFormat() {return ".map";}
	virtual bool importData(iRenderNode* pRenderNode, const char* szFilename);
	virtual void release();
private:
	bool importTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);

	bool exportTerrainAtt(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainLightmap(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainHeight(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTiles(iTerrainData * pTerrain, const std::string& strFilename, const std::string& strPath);
};