#pragma once
#include "windows.h"

class CDataPlugBase
{
public:
	CDataPlugBase(){};
	virtual ~CDataPlugBase(){};
	virtual const char * getTitle() = 0;
	virtual const char * getFormat() = 0;
	virtual void release() = 0;
#ifdef _DEBUG
	virtual bool isDebug(){return true;}
#else
	virtual bool isDebug(){return false;}
#endif
};