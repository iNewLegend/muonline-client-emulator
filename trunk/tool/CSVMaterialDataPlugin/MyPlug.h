#pragma once
#include "InterfaceModel.h"

class CMyPlug : public CModelPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);
	virtual const char * getTitle(){return "CSV Materila Data File";}
	virtual const char * getFormat() {return ".csv";}
	virtual iRenderNode* importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename);
	virtual void release();
private:
};