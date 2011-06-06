#pragma once
#include "InterfaceScene.h"

class CMyPlug : public CScenePlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);

	virtual const char * getTitle(){return "Torch Light Scene File";}
	virtual const char * getFormat() {return ".layout";}
	virtual int Execute(iScene * pTerrainData, bool bShowDlg, bool bSpecifyFileName);
	virtual int importData(iScene * pScene, const std::string& strFilename);
	virtual int exportData(iScene * pScene, const std::string& strFilename);

	virtual void release();
private:
	bool importTileSet(iScene * pScene, const std::string& strFilename, const std::string& strPath);
	bool importObject(iScene * pScene, const std::string& strFilename);

	bool exportTerrainData(iTerrainData * pTerrainData, const std::string& strFilename);
	bool exportTiles(iTerrainData * pTerrain, const std::string& strFilename, const std::string& strPath);
	bool exportObjectResources(iScene * pScene, const std::string& strFilename, const std::string& strPath);
	bool exportObjectResourcesFormDir(iScene * pScene,const std::string& strPath);
	bool exportObject(iScene * pScene, const std::string& strFilename);

	unsigned short					m_uMuFlgMap;
	unsigned long					m_uMuFlgAtt;
};