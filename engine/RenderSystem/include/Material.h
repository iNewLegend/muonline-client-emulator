#pragma once
#include "RenderSystemCommon.h"
#include <map>

enum E_MATERIAL_RENDER_TYPE
{
	MATERIAL_NONE				= 0,
	MATERIAL_GEOMETRY			= 1<<0,
	MATERIAL_ALPHA				= 1<<1,
	MATERIAL_GLOW				= 1<<2,
	MATERIAL_BUMP				= 1<<3,
	MATERIAL_NORMAL				= MATERIAL_GEOMETRY+MATERIAL_ALPHA+MATERIAL_GLOW,
	MATERIAL_ALL				= MATERIAL_NORMAL+MATERIAL_BUMP,
	MATERIAL_RENDER_ALPHA_TEST	= 1<<4,
	MATERIAL_RENDER_NO_MATERIAL	= 1<<5,
	MATERIAL_TARGET_MESH		= 1<<6,
	MATERIAL_TARGET_PARTICLE	= 1<<7,
	MATERIAL_SHADOW				= 1<<8,
};

class CMaterial
{
public:
	CMaterial()
	{
		for (size_t i=0; i<8; ++i)
		{
			uTexture[i] = 0;
		}
	}

	E_MATERIAL_RENDER_TYPE getRenderType()
	{
		/*if (bAlphaTest)
		{
			return MATERIAL_GEOMETRY;
		}
		//else if (uDiffuse==0&&uNormal>0)
		//{
		//	return MATERIAL_BUMP;
		//}
		else if (bBlend)
		{
		return MATERIAL_GLOW;
		}*/
		return MATERIAL_GEOMETRY;
	}

	int	getOrder()
	{
		int nOrder=0;
		/*if (bAlphaTest&&bBlend)
		{
			nOrder--;
		}
		else if (bBlend)
		{
			nOrder-=2;
			if (nBlendSrc==SBF_ONE&&nBlendDest==SBF_ONE)
			{
				nOrder--;
			}
		}
		if (!bDepthTest)
		{
			nOrder-=10;
		}*/
		return nOrder;
	}
public:
	std::string		strShader;
	std::string		strTexture[8];
	unsigned long	uTexture[8];
};