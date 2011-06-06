#pragma once
#include "InterfaceModel.h"

class CMyPlug : public CModelPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);
	virtual const char * getTitle(){return "OGRE Model File";}
	virtual const char * getFormat() {return ".mesh";}
	virtual int Execute(iModelData * pModelData, bool bShowDlg, bool bSpecifyFileName);
	virtual bool importData(iModelData * pModelData, const std::string& strFilename);
	virtual bool exportData(iModelData * pModelData, const std::string& strFilename);

	virtual void release();
private:
};