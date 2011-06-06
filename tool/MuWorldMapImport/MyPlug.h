#pragma once
#include "InterfaceScene.h"

class CMyPlug : public CScenePlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);

	virtual const char * getTitle(){return "Mu Scene File";}
	virtual const char * getFormat() {return ".map";}
	virtual int Execute(iScene * pTerrainData, bool bShowDlg, bool bSpecifyFileName);
	virtual int importData(iScene * pScene, const std::string& strFilename);
	virtual int exportData(iScene * pScene, const std::string& strFilename);

	virtual void release();
private:
	bool importTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);
	bool importObjectResources(iScene * pScene, const char* szFilename, const std::string& strPath);
	bool importObjectResourcesFormDir(iScene * pScene,const std::string& strPath);
	bool importObject(iScene * pScene, const char* szFilename);

	bool exportTerrainAtt(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainLightmap(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainHeight(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTiles(iTerrainData * pTerrain, const std::string& strFilename, const std::string& strPath);
	bool exportObjectResources(iScene * pScene, const std::string& strFilename, const std::string& strPath);
	bool exportObjectResourcesFormDir(iScene * pScene,const std::string& strPath);
	bool exportObject(iScene * pScene, const std::string& strFilename);

	std::map<int,std::string>	m_mapObjectName;
};