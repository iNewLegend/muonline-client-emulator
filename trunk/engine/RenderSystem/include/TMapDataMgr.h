#pragma once
#include "manager.h"
#include "DataPlugsMgr.h"
#include "FileSystem.h"

template <class T, class TPLUG>
class TMapDataMgrMgr
{
public:
	TMapDataMgrMgr(const char* szFilename)
	{
		m_DataPlugsMgr.loadPlugs(szFilename);
	}

	~TMapDataMgrMgr(){;}

	T& getItem(const char* szMaterialName)
	{
		return m_Items[szMaterialName];
	}

	bool load(const char* szFilename, const char* szParentDir=NULL)
	{
		// 判断格式--根据文件后缀名
		std::string strExt = GetExtension(szFilename);
		TPLUG* pModelPlug = (TPLUG*)m_DataPlugsMgr.getPlugByExtension(strExt.c_str());
		if (pModelPlug)
		{
			return pModelPlug->importData(m_Items,szFilename, szParentDir);
		}
		return false;
	}

protected:
	std::map<std::string, T> m_Items;
private:
	CDataPlugsMgr m_DataPlugsMgr;
};