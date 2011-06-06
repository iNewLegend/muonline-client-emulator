#pragma once
#include "InterfaceModel.h"

class CMyPlug : public CModelPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);
	virtual const char * getTitle(){return "Mu Model File";}
	virtual const char * getFormat() {return ".bmd";}
	virtual iRenderNode* importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename);
	virtual void release();
private:
};