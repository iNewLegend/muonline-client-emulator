#pragma once
#include "Manager.h"
#include "Color.h"
#include "matrix.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "RenderSystem.h"
#include "InterfacePlug.h"
struct QuadVertex
{
	enum _FVF { FVF = (FVF_XYZRHW | FVF_TEX1) };
	Vec4D p;
	Vec2D t;
};

class CSceneEffect
{
public:
	CSceneEffect(); 
	~CSceneEffect();
public:
	void clearTextures();
	void Reset(const CRect<int>& rc);
	float GetSceneExposure();
	void render(iRenderNode* pRenderNode);
	void renderTargetBegin();
	void renderTargetGlow();
	void renderGammaCorrection();
	void renderTargetBloom();
	void renderTargetEnd();
	void compose(const CRect<int>& rcDest);
	void setDebug(int flag){m_nFlag = flag;}
protected:
	CRect<int> m_Rect;
	int	m_nWidth;
	int m_nHeight;
	CTexture*	m_pSystemRT;

	CTexture*	m_pDiffuseRT;
	CTexture*	m_pPosMRT;
	CTexture*	m_pNormalMRT;
	CTexture*	m_pLightRT;

	CTexture*	m_pSceneRT4x;
	CTexture*	m_pSceneRT8x1;
	CTexture*	m_pSceneRT8x2;

	//new
	CTexture*	m_pSceneCopyTexture;

	// HDR
	bool m_bHDR;
	// 模糊度
	float m_fBloomVal;
	QuadVertex m_Quad[4];
	QuadVertex m_Quad4x[4];
	QuadVertex m_Quad8x[4];
	// 适应的光
	float m_fAdaptedLum;
	// 适应的光
	float m_fHDRKey;

	bool m_bInitialized;
	int		m_nFlag;
};