#pragma once
#include "InterfaceDataPlugsBase.h"
#include <string>
#include <vector>

typedef struct{
	CDataPlugBase * pObj;
	HINSTANCE hIns;
}DATA_PLUG_ST, * LPDATA_PLUG_ST;

class CDataPlugsMgr
{
public:
	CDataPlugBase* getPlugByExtension(const char* szExt);
	bool loadPlugs(const char* szFilename);
	std::string getAllExtensions();
private:
	bool loadPlug(const char* szFilename);
	std::vector<DATA_PLUG_ST> m_setPlugObj;
};