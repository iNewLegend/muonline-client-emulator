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
};

class CMaterial
{
public:
	CMaterial():
		uShader(-1),
		bLightingEnabled(false),
		vAmbient(0.6f,0.6f,0.6f,0.6f),
		vDiffuse(1.0f,1.0f,1.0f,1.0f),
		bDepthWrite(true),
		bAlphaTest(false),
		uAlphaTestValue(0x80),
		bBlend(false),
		uCull(0),
		vTexAnim(0.0f,0.0f),
		m_fOpacity(1.0f),
		cEmissive(255,255,255,255),
		vUVScale(0.0f,0.0f)
	{
		for (size_t i=0; i<8; ++i)
		{
			uTexture[i] = -1;
		}
	}

	E_MATERIAL_RENDER_TYPE getRenderType()
	{
		if (bAlphaTest)
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
		}
		return MATERIAL_GEOMETRY;
	}

	int	getOrder()
	{
		int nOrder=0;
		if (m_fOpacity<1.0f)
		{
			nOrder--;
		}
		if (bAlphaTest&&bBlend)
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
		}
		return nOrder;
	}

	void SetEmissiveColor(const Color32& color)
	{
		cEmissive = color;
	}
	// Texture
	void setTexture(int id, const char* szFilename)
	{
		strTexture[id]	= szFilename;
		uTexture[id]	= -1;
	}
	const std::string& getTexture(int id)
	{
		return strTexture[id];
	}
	// Shader
	void setShader(const std::string& strFilename)
	{
		strShader=strFilename;
		uShader=-1;
	}
	const std::string& getShader()
	{
		return strShader;
	}
private:
	std::string		strTexture[8];
	std::string		strShader;
public:
	// ----
	// # texture
	// ----
	unsigned long	uTexture[8];
	// ----
	unsigned long	uShader;
	// ----
	bool			bLightingEnabled;
	// ----
	unsigned char	uCull;
	// ----
	bool			bBlend;
	int				nBlendOP;
	int				nBlendSrc;
	int				nBlendDest;
	// ----
	bool			bAlphaTest;
	int				nAlphaTestCompare;
	unsigned char	uAlphaTestValue;
	// ----
	bool			bDepthTest;
	bool			bDepthWrite;
	// ----
	struct TextureOP 
	{
		int			nColorOP;
		int			nColorSrc1;
		int			nColorSrc2;
		int			nAlphaOP;
		int			nAlphaSrc1;
		int			nAlphaSrc2;
	};
	// ----
	TextureOP		textureOP[8];
	// ----
	// # color
	// ----
	Vec4D			vAmbient;
	Vec4D			vDiffuse;
	// ----
	Vec2D			vTexAnim;
	float			m_fOpacity;
	Color32			cEmissive;
	Vec2D			vUVScale; // for terrain's tile, temp
};