#pragma once
#include "Manager.h"
#include "Color.h"
#include "matrix.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "RenderSystem.h"
struct QuadVertex
{
	enum _FVF { FVF = (FVF_XYZRHW | FVF_TEX1) };
	Vec4D p;
	Vec2D t;
};

struct SceneBloomVertex
{
	enum _FVF { FVF=(FVF_XYZRHW|FVF_DIFFUSE|FVF_TEX1) };
	Vec4D		p;
	Color32		c;
	Vec2D		t;
};

#define BLOOM_MODEL 1
class CSceneEffect
{
public:
	CSceneEffect(); 
	~CSceneEffect();
public:
	void clearTextures();
	void Reset(const CRect<int>& rc);
	void RenderTemporalBloom();
	void RenderBloom();
	float GetSceneExposure();

	void glowRenderTargetBegin();
	void glowRenderTargetEnd();

	void renderTargetBegin();
	void renderTargetGlow();
	void renderGammaCorrection();
	void renderTargetBloom();
	void renderTargetEnd();
	void compose(const CRect<int>& rcDest);

	// new
	CTexture* getSceneTexture();
protected:
	CRect<int> m_Rect;
	int	m_nWidth;
	int m_nHeight;
	CTexture*	m_pGlowRenderTarget;
	CTexture*	m_pRenderSystemTarget;
	CTexture*	m_pDepthRenderTarget;
	// 泛光渲染到贴图
	CTexture*	m_pSceneTexture;
	CTexture*	m_pTextureScene1;

	CTexture*	m_pExposureTexture;
	CTexture*	m_pExposureTexture2;

	//new
	CTexture*	m_pSceneCopyTexture;

	// 画面备份贴图
	CTexture*	m_pBackTexture;

	// HDR
	bool m_bHDR;
	// 模糊度
	float m_fBloomVal;
	QuadVertex m_QuadVB[4];
	// 适应的光
	float m_fAdaptedLum;
	// 适应的光
	float m_fHDRKey;
	// 泛滥亮度
	SceneBloomVertex m_FloodLumVB[4];
	// 横纵模糊
	SceneBloomVertex m_BloomHVB[78];
	SceneBloomVertex m_BloomVVB[78];

	WORD m_BloomIB[13*6];
	bool m_bInitialized;
};