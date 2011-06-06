#pragma once
#include "RenderSystem.h"
#include "Vec3D.h"
#include "Color.h"

struct SkyboxVertex
{
	enum _FVF {FVF=(FVF_XYZ|FVF_DIFFUSE|FVF_TEX1|FVF_TEXCOORDSIZE3(0))};
	Vec3D		p;
	unsigned long		c;
	Vec3D		t;
};

class CSkyBox
{
public:
	CSkyBox();
	~CSkyBox();
	void Render();
	CTexture* m_pCubeMap;
protected:
	SkyboxVertex	m_vBox[6][4];
};