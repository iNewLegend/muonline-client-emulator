#pragma once
#include "InterfacePlug.h"

class CMyPlug : public CDataPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);
	virtual const char * getTitle(){return "Particle Data File";}
	virtual const char * getFormat() {return ".par";}
	virtual bool importData(iRenderNode* pRenderNode, const char* szFilename);
	virtual void release();
private:
};