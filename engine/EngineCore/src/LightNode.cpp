#include "LightNode.h"
#include "SkeletonNode.h"
#include "SkeletonData.h"
#include "RenderSystem.h"

CLightNode::CLightNode()
{
}

CLightNode::~CLightNode()
{
}

void CLightNode::render(int nRenderType)const
{
	CRenderNode::render(nRenderType);
	// ----
	if ((RF_LIGHT&nRenderType) == 0)
	{
		return;
	}
	// ----
	CRenderSystem& R = CRenderSystem::getSingleton();
	// ----
	Vec3D vPos = m_mRealMatrix*Vec3D(0.0f,0.0f,0.0f);
	R.setShaderFloatArray("gColor",	&vPos, 3);
	int nLightData = (int)m_pData;
	float lightData[4];
	float fRand = 1.0f - rand()/(float)RAND_MAX*(float)((nLightData>>24)&0xF)/16.0f;
	lightData[0] = fRand*((nLightData>>16)&0xFF)/255.0f;
	lightData[1] = fRand*((nLightData>>8)&0xFF)/255.0f;
	lightData[2] = fRand*(nLightData&0xFF)/255.0f;
	lightData[3] = 1.0f/(nLightData>>28);
	R.setShaderFloatArray("g_vPointLight",	&vPos, 3);
	R.setShaderFloatArray("ld",	lightData, 4);
	// ----
	CRect<int> rcDest;
	R.getViewport(rcDest);
	// ----
	float QuadVB[4][6] =
	{
		{(float)rcDest.left - 0.5f,		(float)rcDest.bottom - 0.5f,	0.0f, 1.0f, 0.0f, 1.0f},
		{(float)rcDest.left - 0.5f,		(float)rcDest.top - 0.5f,		0.0f, 1.0f, 0.0f, 0.0f},
		{(float)rcDest.right - 0.5f,	(float)rcDest.bottom - 0.5f,	0.0f, 1.0f, 1.0f, 1.0f},
		{(float)rcDest.right - 0.5f,	(float)rcDest.top - 0.5f,		0.0f, 1.0f, 1.0f, 0.0f}
	};
	// ----
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, 24);
}