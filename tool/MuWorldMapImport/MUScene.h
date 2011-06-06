#pragma once
#include "Scene.h"

class DLL_EXPORT CMUScene:public CScene
{
public:
	static bool checkKeyByFile(std::string& strInfo);
	static bool regKey(const std::string& strUser,const std::string& strKey, std::string& strInfo);
	static bool checkKey(const std::string& strUser,const std::string& strKey, std::string& strInfo);
};