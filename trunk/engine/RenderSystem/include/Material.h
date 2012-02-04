#pragma once
#include "RenderSystemCommon.h"

class CMaterial
{
public:
	CMaterial()
	{
		memset(uTexture,0,sizeof(unsigned long)*8);
	}
public:
	std::string		strShader;
	std::string		strTexture[8];
	unsigned long	uTexture[8];
};