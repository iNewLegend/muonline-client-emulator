#pragma once
#include "InterfacePlug.h"

class CMyPlug : public CDataPlugBase  
{
public:
	CMyPlug(void);
	~CMyPlug(void);
	virtual const char * getTitle(){return "CSV Materila Data File";}
	virtual const char * getFormat() {return ".csv";}
	virtual bool importData(iRenderNode* pRenderNode, const char* szFilename);
	virtual void release();
private:
};