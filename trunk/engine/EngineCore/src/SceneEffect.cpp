#include "SceneEffect.h"
#include "RenderSystem.h"
#include "Vec3D.h"
#include "Color.h"

CSceneEffect::CSceneEffect():
m_fBloomVal(0.2f),
m_fAdaptedLum(0.5f),
m_fHDRKey(0.3f),
m_bHDR(false),
m_bInitialized(false),
m_pDepthRenderTarget(NULL),
m_pGlowRenderTarget(NULL),
m_pSceneTex(NULL),
m_pExposureTexture(NULL),
m_pExposureTexture2(NULL),
m_pBackTexture(NULL),
m_pSceneCopyTexture(NULL),
m_nWidth(0),
m_nHeight(0)
{
}

CSceneEffect::~CSceneEffect()
{
	clearTextures();
}

void CSceneEffect::clearTextures()
{
	S_DEL(m_pGlowRenderTarget);
	S_DEL(m_pDepthRenderTarget);
	S_DEL(m_pSceneTex);
	S_DEL(m_pExposureTexture);
	S_DEL(m_pExposureTexture2);
	S_DEL(m_pBackTexture);
	// new 
//	{ // Fixed the scene texture release.
// 		CShader* pShader = GetRenderSystem().GetShaderMgr().getSharedShader();
// 		if (pShader)
// 		{
// 			pShader->setTexture("g_texScene",(CTexture*)NULL);
// 		}
//	}

	S_DEL(m_pSceneCopyTexture);
}

void CSceneEffect::Reset(const CRect<int>& rc)
{
	int nWidth = rc.getWidth();
	int nHeight = rc.getHeight();
	if (nWidth<=0||nHeight<=0)
	{
		return;
	}
	m_Rect = rc;

	clearTextures();

	m_nWidth = nWidth;
	m_nHeight= nHeight;

	CRenderSystem& R = CRenderSystem::getSingleton();
	//m_pDepthRenderTarget = R.GetTextureMgr().CreateRenderTarget(512, 512);

	m_pGlowRenderTarget = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	m_pSceneTex = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);
	m_pTexScene4x = R.GetTextureMgr().CreateRenderTarget(nWidth*0.25f,nHeight*0.25f);
	m_pTexScene8x1 = R.GetTextureMgr().CreateRenderTarget(nWidth*0.0625f,nHeight*0.0625f);
	m_pTexScene8x2 = R.GetTextureMgr().CreateRenderTarget(nWidth*0.0625f,nHeight*0.0625f);

	m_pSceneCopyTexture = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	//m_pExposureTexture = R.GetTextureMgr().CreateRenderTarget(1,1);
	m_pBackTexture = R.GetTextureMgr().CreateRenderTarget(512, 512);//.CreateDynamicTexture(512,512);

	m_Quad[0].t = Vec2D(0.0f, 1.0f);
	m_Quad[1].t = Vec2D(0.0f, 0.0f);
	m_Quad[2].t = Vec2D(1.0f, 1.0f);
	m_Quad[3].t = Vec2D(1.0f, 0.0f);

	m_Quad[0].p = Vec4D(-0.5f,				(float)nHeight-0.5f,	0.0f, 1.0f);
	m_Quad[1].p = Vec4D(-0.5f,				-0.5f,					0.0f, 1.0f);
	m_Quad[2].p = Vec4D((float)nWidth-0.5f,	(float)nHeight-0.5f,	0.0f, 1.0f);
	m_Quad[3].p = Vec4D((float)nWidth-0.5f,	-0.5f,					0.0f, 1.0f);

	m_Quad4x[0].t = Vec2D(0.0f, 1.0f);
	m_Quad4x[1].t = Vec2D(0.0f, 0.0f);
	m_Quad4x[2].t = Vec2D(1.0f, 1.0f);
	m_Quad4x[3].t = Vec2D(1.0f, 0.0f);

	m_Quad4x[0].p = Vec4D(-0.5f,					(float)nHeight*0.25f-0.5f,	0.0f, 1.0f);
	m_Quad4x[1].p = Vec4D(-0.5f,					-0.5f,						0.0f, 1.0f);
	m_Quad4x[2].p = Vec4D((float)nWidth*0.25f-0.5f,	(float)nHeight*0.25f-0.5f,	0.0f, 1.0f);
	m_Quad4x[3].p = Vec4D((float)nWidth*0.25f-0.5f,	-0.5f,						0.0f, 1.0f);
	

	m_Quad8x[0].t = Vec2D(0.0f, 1.0f);
	m_Quad8x[1].t = Vec2D(0.0f, 0.0f);
	m_Quad8x[2].t = Vec2D(1.0f, 1.0f);
	m_Quad8x[3].t = Vec2D(1.0f, 0.0f);

	m_Quad8x[0].p = Vec4D(-0.5f,						(float)nHeight*0.0625f-0.5f,	0.0f, 1.0f);
	m_Quad8x[1].p = Vec4D(-0.5f,						-0.5f,							0.0f, 1.0f);
	m_Quad8x[2].p = Vec4D((float)nWidth*0.0625f-0.5f,	(float)nHeight*0.0625f-0.5f,	0.0f, 1.0f);
	m_Quad8x[3].p = Vec4D((float)nWidth*0.0625f-0.5f,	-0.5f,							0.0f, 1.0f);

	m_bInitialized = true;
}

CTexture* CSceneEffect::getSceneTexture()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	CTexture* m_pRenderSystemTarget = R.GetRenderTarget();
	CRect<int> rcSrc;
	R.getViewport(rcSrc);
	R.StretchRect(m_pRenderSystemTarget,&rcSrc,m_pSceneCopyTexture,NULL, TEXF_LINEAR);
	S_DEL(m_pRenderSystemTarget);
	return m_pSceneCopyTexture;
}

void CSceneEffect::RenderTemporalBloom()
{
	if (false==m_bInitialized)
	{
		return;
	}
	//return;
	// 1:copy正常画面 2:与上一帧按混合比混合并输出 3:保存此帧用于下次混合
	CRenderSystem& R = CRenderSystem::getSingleton();

	CTexture* m_pRenderSystemTarget = R.GetRenderTarget();
	R.StretchRect(m_pRenderSystemTarget,NULL,m_pBackTexture,NULL, TEXF_LINEAR);
	//if(R.BeginFrame())
	{
		int nAlpha =227; min(m_fBloomVal*255,255);
		unsigned long dwFactor = (nAlpha<<24)+0xFF0000;
		R.SetTextureFactor(dwFactor);
		R.SetCullingMode(CULL_NONE);// CULL_CCW
		R.SetLightingEnabled(false);
		R.SetBlendFunc(false,BLENDOP_ADD,SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
		R.SetDepthBufferFunc(false,false);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE);
		R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TFACTOR);

		R.SetTextureColorOP(1,TBOP_DISABLE);
		R.SetTextureAlphaOP(1,TBOP_DISABLE);

		R.SetTexture(0,m_pBackTexture) ;
		R.SetFVF(QuadVertex::FVF);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));
	//	R.EndFrame();
	}
	//保存当前的画面 以备下一帧调用
	//R.EndFrame();
	R.StretchRect(m_pRenderSystemTarget,NULL,m_pBackTexture,NULL, TEXF_LINEAR);
	//m_pBackTexture->SaveToFile("D:/BackCopy.bmp");
	//R.BeginFrame();
	S_DEL(m_pRenderSystemTarget);
}

float CSceneEffect::GetSceneExposure()
{	/*const RECT rect0 = {0,0,255,255};
const RECT rect1 = {256,0,256+64,64};
const RECT rect2 = {256+64,0,256+64+16,16};
const RECT rect3 = {256+64+16,0,256+64+16+4,4};
const RECT rect4 = {0,0,1,1};

R.StretchRect(m_pSceneTexture, &rect0, m_pSceneTexture, &rect1, TEXF_LINEAR);
R.StretchRect(m_pSceneTexture, &rect1, m_pSceneTexture, &rect2, TEXF_LINEAR);
R.StretchRect(m_pSceneTexture, &rect2, m_pSceneTexture, &rect3, TEXF_LINEAR);
R.StretchRect(m_pSceneTexture, &rect3, m_pExposureTexture, &rect4, TEXF_LINEAR);

D3DXSaveSurfaceToFileA("F:/项目/NewGame/cc.bmp", D3DXIFF_BMP, m_pSceneTexture, NULL, NULL);*/
	///RECT rect;
	//rect.left = rand()%250 + 2;
	//rect.top = rand()%250 + 2;
	//rect.right = rect.left+1;
	//rect.bottom = rect.top+1;

	//R.StretchRect(m_pSceneTexture, &rect, m_pExposureTexture, NULL, TEXF_NONE);

	//R.GetRenderTargetData(m_pExposureTexture, m_pExposureTexture2);

	//D3DLOCKED_RECT lockedRect;
	//HRESULT hr = m_pExposureTexture2->LockRect(&lockedRect,
	//	0/*lock entire tex*/, D3DLOCK_DONOTWAIT | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_READONLY/*flags*/);

	//if (hr == S_OK)
	//{
	//	Color32 color = *((Color32*)lockedRect.pBits);
	//	// 计算像素的亮度
	//	float fLum = (0.27f*color.a + 0.67f*color.r + 0.06f * color.g)/255.0f;
	//	// 计算当前适应的亮度
	//	m_fAdaptedLum += (fLum - m_fAdaptedLum) * (1 - pow(0.98f, 10 * DXUTGetElapsedTime()));
	//}
	//else if (hr == D3DERR_WASSTILLDRAWING)
	//{
	//	hr = S_OK;
	//}
	//m_pExposureTexture2->UnlockRect();
	return m_fAdaptedLum;
}

void CSceneEffect::glowRenderTargetBegin()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	m_pRenderSystemTarget = R.GetRenderTarget();
	R.SetRenderTarget(m_pGlowRenderTarget);

	//RECT rect={0,0,255,255};
	//R.setViewport(rect);
}

void CSceneEffect::glowRenderTargetEnd()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	// Restore
	R.SetRenderTarget(m_pRenderSystemTarget);
	S_DEL(m_pRenderSystemTarget);
}

void CSceneEffect::renderTargetBegin()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	m_pRenderSystemTarget = R.GetRenderTarget();
	R.SetRenderTarget(m_pSceneTex);
	//R.SetSamplerFilter(0, TEXF_POINT, TEXF_POINT, TEXF_POINT);
}

void CSceneEffect::renderTargetGlow()// not good
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	// first: copy system render target to my render target.

	CRect<int> rect(0,0,m_nWidth,m_nHeight);
	R.StretchRect(m_pGlowRenderTarget, NULL, m_pSceneTex, &rect, TEXF_POINT);

	return;
	if (m_bHDR)
	{
		GetSceneExposure();
	}

	//if(R.BeginFrame())
	// Get the lum texture
	{
		
		// 一定要把空白区域值为黑色
		R.SetTexture(0, m_pSceneTex);
	//	R.SetFVF(SceneBloomVertex::FVF);


		R.SetBlendFunc(false);
		// Increase the contrast
		//R.SetTextureColorOP(0,TBOP_DOTPRODUCT3,TBS_TEXTURE,TBS_DIFFUSE);
		R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_TEXTURE);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));

		{
			int nInvExposure= (int)(m_fAdaptedLum * m_fAdaptedLum * 255.0f);
			Color32 clrFactor(nInvExposure,nInvExposure,nInvExposure,nInvExposure);
			R.SetTextureFactor(clrFactor);
		}

		// To reduce the brightness
		R.SetTextureColorOP(0,TBOP_SUBTRACT,TBS_TEXTURE,TBS_TFACTOR);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));


		//R.SetBlendFunc(true,BLENDOP_ADD,SBF_ONE,SBF_ONE);
		//R.SetTextureFactor(0x40404040)
		//R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_TFACTOR);
		//r->DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_FloodLumVB, sizeof(SceneBloomVertex));

		//	R.EndFrame();
	}
}

void CSceneEffect::renderGammaCorrection()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	R.SetRenderTarget(m_pSceneTex);
	//R.ClearBuffer(false,true,0x0);
	//R.GetShaderMgr().getSharedShader()->setTexture("g_texScene",m_pSceneTexture);
	R.SetTexture(0, m_pSceneTex);

	if(R.prepareMaterial("GammaCorrection"))
	{
		CRenderSystem& R = CRenderSystem::getSingleton();
		//R.SetTexture(0,g_samScene) ;
		R.SetFVF(QuadVertex::FVF);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));
		R.finishMaterial();
	}
}

void CSceneEffect::renderTargetBloom()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	R.SetFVF(QuadVertex::FVF);

	// ----
	Vec2D inv_width_height(1.0f/m_nWidth,1.0f/m_nHeight);
	Vec2D inv_width_height4x(1.0f/m_nWidth/4.0f,1.0f/m_nHeight/4.0f);
	Vec2D inv_width_height8x(1.0f/m_nWidth/8.0f,1.0f/m_nHeight/8.0f);

	// ----
	R.setShaderVec2D("inv_width_height",	inv_width_height4x);

	// Down Filter 4x
	if (R.prepareMaterial("Filter4"))
	{

		R.SetRenderTarget(m_pTexScene4x);
		R.SetTexture(0, m_pSceneTex);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));
	}

	// ----
	R.setShaderVec2D("inv_width_height",	inv_width_height8x);

	// Down Filter 4x
	if (R.prepareMaterial("Filter4"))
	{
		R.SetRenderTarget(m_pTexScene8x1);
		R.SetTexture(0, m_pTexScene4x);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	}

	// Bright Pass
	if (R.prepareMaterial("Bright"))
	{
		R.SetRenderTarget(m_pTexScene8x2);
		R.SetTexture(0, m_pTexScene8x1);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	}
	
	// Bloom Horizontal
	if (R.prepareMaterial("BloomH"))
	{
		R.SetRenderTarget(m_pTexScene8x1);
		R.SetTexture(0, m_pTexScene8x2);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	}

	// Bloom Vertical
	if (R.prepareMaterial("BloomV"))
	{
		R.SetRenderTarget(m_pTexScene8x2);
		R.SetTexture(0, m_pTexScene8x1);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	}

	// Bloom Horizontal
	if (R.prepareMaterial("BloomH"))
	{
		R.SetRenderTarget(m_pTexScene8x1);
		R.SetTexture(0, m_pTexScene8x2);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	}

	// Bloom Vertical
	if (R.prepareMaterial("BloomV"))
	{
		R.SetRenderTarget(m_pTexScene8x2);
		R.SetTexture(0, m_pTexScene8x1);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	}

	// ----
	R.setShaderVec2D("inv_width_height",	inv_width_height4x);

	// Up Filter 4x
 	if (R.prepareMaterial("Combine"))
 	{
 		R.SetRenderTarget(m_pTexScene4x);
 		R.SetTexture(0, m_pTexScene8x2);
 		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));
 	}

	R.setShaderVec2D("inv_width_height",	inv_width_height);
// 	// Combine 4x
// 	if (R.prepareMaterial("Combine"))
// 	{
// 		R.SetRenderTarget(m_pTexScene8x2);
// 		R.SetTexture(0, m_pSceneTex);
// 		R.SetTexture(0, m_pTexScene4x);
// 		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));
// 	}
}

void CSceneEffect::renderTargetEnd()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	// Restore
	R.SetRenderTarget(m_pRenderSystemTarget);
	S_DEL(m_pRenderSystemTarget);
}

void CSceneEffect::compose(const CRect<int>& rcDest)
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	QuadVertex QuadVB[4];
	float fU0 = 0.0f;
	float fV0 = 0.0f;
	float fU1 = 1.0f;
	float fV1 = 1.0f;

	float fX0 = (float)rcDest.left - 0.5f;
	float fY0 = (float)rcDest.top - 0.5f;
	float fX1 = (float)rcDest.right - 0.5f;
	float fY1 = (float)rcDest.bottom - 0.5f;

	QuadVB[0].t = Vec2D(fU0, fV1);
	QuadVB[1].t = Vec2D(fU0, fV0);
	QuadVB[2].t = Vec2D(fU1, fV1);
	QuadVB[3].t = Vec2D(fU1, fV0);

	QuadVB[0].p = Vec4D(fX0, fY1, 0.0f, 1.0f);
	QuadVB[1].p = Vec4D(fX0, fY0, 0.0f, 1.0f);
	QuadVB[2].p = Vec4D(fX1, fY1, 0.0f, 1.0f);
	QuadVB[3].p = Vec4D(fX1, fY0, 0.0f, 1.0f);

	// Combine 4x
	if (R.prepareMaterial("CombineAdd"))
	{
		//R.SetRenderTarget(m_pTexScene8x2);
		R.SetTexture(0, m_pSceneTex);
		R.SetTexture(1, m_pTexScene4x);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, sizeof(QuadVertex));
	}

	//R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
}