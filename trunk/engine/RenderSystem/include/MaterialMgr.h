#pragma once
#include "Material.h"

class CMaterialMgr
{
public:
	CMaterial& getItem(const char* szMaterialName)
	{
		return m_Items[szMaterialName];
	}
public:
	std::map<std::string, CMaterial> m_Items;
};