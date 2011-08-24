#pragma once
#include "InterfacePlug.h"

class CMyPlug : public CDataPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);
	virtual const char * getTitle(){return "Mu Model File";}
	virtual const char * getFormat() {return ".bmd";}
	virtual bool importData(iRenderNode* pRenderNode, const char* szFilename);
	virtual void release();
private:
};