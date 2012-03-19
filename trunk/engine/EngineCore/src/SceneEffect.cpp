#include "SceneEffect.h"
#include "RenderSystem.h"
#include "Vec3D.h"
#include "Color.h"

CSceneEffect::CSceneEffect():
m_fBloomVal(0.2f)
,m_fAdaptedLum(0.5f)
,m_fHDRKey(0.3f)
,m_bHDR(false)
,m_bInitialized(false)
,m_pPosMRT(NULL)
,m_pNormalMRT(NULL)
,m_pLightRT(NULL)
,m_pShadowMapRT(NULL)
,m_pDiffuseRT(NULL)
,m_pDiffuseCopyRT(NULL)
,m_nWidth(0)
,m_nHeight(0)
,m_nFlag(0)
{
}

CSceneEffect::~CSceneEffect()
{
	clearTextures();
}

void CSceneEffect::clearTextures()
{
	S_DEL(m_pLightRT);
	S_DEL(m_pShadowMapRT);
	S_DEL(m_pPosMRT);
	S_DEL(m_pNormalMRT);
	S_DEL(m_pDiffuseRT);
	S_DEL(m_pDiffuseCopyRT);
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

	m_pDiffuseRT	= R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);
	m_pPosMRT		= R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);
	m_pNormalMRT	= R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);
	m_pLightRT		= R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);
	m_pShadowMapRT	= R.GetTextureMgr().CreateRenderTarget(512,512);
	m_pDiffuseCopyRT= R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	m_pSceneRT2x	= R.GetTextureMgr().CreateRenderTarget(nWidth*0.5f,nHeight*0.5f);
	m_pSceneRT4x1	= R.GetTextureMgr().CreateRenderTarget(nWidth*0.25f,nHeight*0.25f);
	m_pSceneRT4x2	= R.GetTextureMgr().CreateRenderTarget(nWidth*0.25f,nHeight*0.25f);
	//m_pSceneRT8x1	= R.GetTextureMgr().CreateRenderTarget(nWidth*0.0625f,nHeight*0.0625f);
	//m_pSceneRT8x2	= R.GetTextureMgr().CreateRenderTarget(nWidth*0.0625f,nHeight*0.0625f);

	m_Quad[0].t = Vec2D(0.0f, 1.0f);
	m_Quad[1].t = Vec2D(0.0f, 0.0f);
	m_Quad[2].t = Vec2D(1.0f, 1.0f);
	m_Quad[3].t = Vec2D(1.0f, 0.0f);

	m_Quad[0].p = Vec4D(-0.5f,				(float)nHeight-0.5f,	0.0f, 1.0f);
	m_Quad[1].p = Vec4D(-0.5f,				-0.5f,					0.0f, 1.0f);
	m_Quad[2].p = Vec4D((float)nWidth-0.5f,	(float)nHeight-0.5f,	0.0f, 1.0f);
	m_Quad[3].p = Vec4D((float)nWidth-0.5f,	-0.5f,					0.0f, 1.0f);

	
	m_Quad2x[0].t = Vec2D(0.0f, 1.0f);
	m_Quad2x[1].t = Vec2D(0.0f, 0.0f);
	m_Quad2x[2].t = Vec2D(1.0f, 1.0f);
	m_Quad2x[3].t = Vec2D(1.0f, 0.0f);

	m_Quad2x[0].p = Vec4D(-0.5f,					(float)nHeight*0.5f-0.5f,	0.0f, 1.0f);
	m_Quad2x[1].p = Vec4D(-0.5f,					-0.5f,						0.0f, 1.0f);
	m_Quad2x[2].p = Vec4D((float)nWidth*0.5f-0.5f,	(float)nHeight*0.5f-0.5f,	0.0f, 1.0f);
	m_Quad2x[3].p = Vec4D((float)nWidth*0.5f-0.5f,	-0.5f,						0.0f, 1.0f);

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

void CSceneEffect::render(iRenderNode* pRenderNode)
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	CRect<int> rcRenderTarget(0,0,m_nWidth,m_nHeight);
	R.setViewport(rcRenderTarget);
	// ----	R.ClearBuffer(true, true, m_Fog.color);
	m_pSystemRT = R.GetRenderTarget();
	R.SetRenderTarget(0,m_pDiffuseRT);
	R.SetRenderTarget(1,m_pPosMRT);
	R.SetRenderTarget(2,m_pNormalMRT);
	// ----
	R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	R.SetSamplerFilter(2, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	// ----
	pRenderNode->render(RF_GEOMETRY);
	// ----
	R.SetRenderTarget(1,NULL);
	R.SetRenderTarget(2,NULL);
	R.SetRenderTarget(3,NULL);
	// ----
	R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_POINT, TEXF_LINEAR);
	R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_POINT, TEXF_LINEAR);
	R.SetSamplerFilter(2, TEXF_LINEAR, TEXF_POINT, TEXF_LINEAR);
	// ----
	R.SetFVF(QuadVertex::FVF);
	// ----
	Vec2D inv_width_height(1.0f/m_nWidth,1.0f/m_nHeight);
	Vec2D inv_width_height2x(2.0f/m_nWidth,2.0f/m_nHeight);
	Vec2D inv_width_height4x(4.0f/m_nWidth,4.0f/m_nHeight);
	Vec2D inv_width_height8x(16.0f/m_nWidth,16.0f/m_nHeight);

	// ----
	// DeferredLighting
	// ----
	R.SetShader("DeferredLighting");
	R.SetRenderTarget(0,m_pLightRT);
	R.SetTexture(0, m_pPosMRT);
	R.SetTexture(1, m_pNormalMRT);
	static int nLightMap = -1;
	if (nLightMap==-1)
	{
		nLightMap = R.GetTextureMgr().RegisterTexture("Data\\World1\\TerrainLight.OZJ");
	}
	R.SetTexture(2, nLightMap);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));

	R.SetShader("DeferredPoint");
	R.SetRenderTarget(0,m_pLightRT);
	R.SetTexture(0, m_pPosMRT);
	R.SetTexture(1, m_pNormalMRT);
	pRenderNode->render(RF_LIGHT);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));

	R.SetShader("DeferredCombine");
	R.SetRenderTarget(0,m_pDiffuseRT);
	R.SetTexture(0, m_pLightRT);
	R.SetTexture(1, m_pDiffuseRT);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));

	// ----
	// Render Glow
	// ----
	pRenderNode->render(RF_GLOW);

	// ----
	// Copy DiffuseRT For Bump
	// ----
	R.SetFVF(QuadVertex::FVF);
	R.SetShader("Combine");
	R.SetRenderTarget(0,m_pDiffuseCopyRT);
	R.SetTexture(0, m_pDiffuseRT);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));
	// Render Bump
	R.SetRenderTarget(0,m_pDiffuseRT);
	R.SetTexture(1, m_pDiffuseCopyRT);
	pRenderNode->render(RF_BUMP);

	  
	R.SetFVF(QuadVertex::FVF);
	// ----
	R.setShaderFloatArray("inv_width_height",	&inv_width_height, 2);
	float PixelCoordsDownFilter1[8] =
	{
		0.5*inv_width_height.x,  -0.5*inv_width_height.y,
		0.5*inv_width_height.x,   0.5*inv_width_height.y,
		-0.5*inv_width_height.x,  -0.5*inv_width_height.y,
		-0.5*inv_width_height.x,   0.5*inv_width_height.y,
	};
	//R.setShaderFloatArray("PixelCoords", PixelCoordsDownFilter1, 8);
	// Bright Pass & Down Filter 2x
	R.SetShader("Bright");
	R.SetRenderTarget(0,m_pSceneRT2x);
	R.SetTexture(0, m_pDiffuseRT);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad2x, sizeof(QuadVertex));

	// ----
	R.setShaderFloatArray("inv_width_height",	&inv_width_height2x, 2);

	float PixelCoordsDownFilter[32] =
	{
		1.5*inv_width_height2x.x,  -1.5*inv_width_height2x.y,
		1.5*inv_width_height2x.x,  -0.5*inv_width_height2x.y,
		1.5*inv_width_height2x.x,   0.5*inv_width_height2x.y,
		1.5*inv_width_height2x.x,   1.5*inv_width_height2x.y,

		0.5*inv_width_height2x.x,  -1.5*inv_width_height2x.y,
		0.5*inv_width_height2x.x,  -0.5*inv_width_height2x.y,
		0.5*inv_width_height2x.x,   0.5*inv_width_height2x.y,
		0.5*inv_width_height2x.x,   1.5*inv_width_height2x.y,

		-0.5*inv_width_height2x.x,  -1.5*inv_width_height2x.y,
		-0.5*inv_width_height2x.x,  -0.5*inv_width_height2x.y,
		-0.5*inv_width_height2x.x,   0.5*inv_width_height2x.y,
		-0.5*inv_width_height2x.x,   1.5*inv_width_height2x.y,

		-1.5*inv_width_height2x.x,  -1.5*inv_width_height2x.y,
		-1.5*inv_width_height2x.x,  -0.5*inv_width_height2x.y,
		-1.5*inv_width_height2x.x,   0.5*inv_width_height2x.y,
		-1.5*inv_width_height2x.x,   1.5*inv_width_height2x.y,
	};

	static int test = 0;
	if (test==0)
	{
		test++;
		R.setShaderFloatArray("PixelCoords", PixelCoordsDownFilter, 32);
	}
	// Down Filter 2x
	R.SetShader("Filter");
	R.SetRenderTarget(0,m_pSceneRT4x1);
	R.SetTexture(0, m_pSceneRT2x);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	R.setShaderFloatArray("inv_width_height",	&inv_width_height4x, 2);
	// Bloom Horizontal
	R.SetShader("BloomH");
	R.SetRenderTarget(0,m_pSceneRT4x2);
	R.SetTexture(0, m_pSceneRT4x1);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	// Bloom Vertical
	R.SetShader("BloomV");
	R.SetRenderTarget(0,m_pSceneRT4x1);
	R.SetTexture(0, m_pSceneRT4x2);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	// Bloom Horizontal
	R.SetShader("BloomH");
	R.SetRenderTarget(0,m_pSceneRT4x2);
	R.SetTexture(0, m_pSceneRT4x1);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	// Bloom Vertical
	R.SetShader("BloomV");
	R.SetRenderTarget(0,m_pSceneRT4x1);
	R.SetTexture(0, m_pSceneRT4x2);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	//// Down Filter 4x
	//R.SetShader("Filter4");
	//R.SetRenderTarget(0,m_pSceneRT4x);
	//R.SetTexture(0, m_pDiffuseRT);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	//// ----
	//R.setShaderVec2D("inv_width_height",	inv_width_height8x);

	//// Down Filter 4x
	//R.SetShader("Filter4");
	//R.SetRenderTarget(0,m_pSceneRT8x1);
	//R.SetTexture(0, m_pSceneRT4x);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));


	//// Bright Pass
	//R.SetShader("Bright");
	//R.SetRenderTarget(0,m_pSceneRT8x2);
	//R.SetTexture(0, m_pSceneRT8x1);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));
	//
	//// Bloom Horizontal
	//R.SetShader("BloomH");
	//R.SetRenderTarget(0,m_pSceneRT8x1);
	//R.SetTexture(0, m_pSceneRT8x2);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));

	//// Bloom Vertical
	//R.SetShader("BloomV");
	//R.SetRenderTarget(0,m_pSceneRT8x2);
	//R.SetTexture(0, m_pSceneRT8x1);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));

	//// Bloom Horizontal
	//R.SetShader("BloomH");
	//R.SetRenderTarget(0,m_pSceneRT8x1);
	//R.SetTexture(0, m_pSceneRT8x2);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));

	//// Bloom Vertical
	//R.SetShader("BloomV");
	//R.SetRenderTarget(0,m_pSceneRT8x2);
	//R.SetTexture(0, m_pSceneRT8x1);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad8x, sizeof(QuadVertex));

	//// ----
	//R.setShaderVec2D("inv_width_height",	inv_width_height4x);

	//// Up Filter 4x
	//R.SetShader("Combine");
	//R.SetRenderTarget(0,m_pSceneRT4x);
	//R.SetTexture(0, m_pSceneRT8x2);
	//R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad4x, sizeof(QuadVertex));

	R.setShaderFloatArray("inv_width_height",	&inv_width_height, 2);
// 	// Combine 4x
// 	if (R.prepareMaterial("Combine"))
// 	{
// 		R.SetRenderTarget(m_pTexScene8x2);
// 		R.SetTexture(0, m_pSceneTex);
// 		R.SetTexture(0, m_pTexScene4x);
// 		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));
// 	}

	// ----
 	//R.SetRenderTarget(0,m_pShadowMapRT);
 	//R.SetShader("depth");
 	//pRenderNode->render(Matrix::UNIT,MATERIAL_GEOMETRY|MATERIAL_RENDER_NO_MATERIAL);
// 	

	//renderGammaCorrection();
	//renderTargetBloom();
	renderTargetEnd();
	compose(rcRenderTarget);
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

void CSceneEffect::renderTargetBegin()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	m_pSystemRT = R.GetRenderTarget();


	//R.SetSamplerFilter(0, TEXF_POINT, TEXF_POINT, TEXF_POINT);
}

void CSceneEffect::renderTargetGlow()// not good
{
	return;
	CRenderSystem& R = CRenderSystem::getSingleton();
	// first: copy system render target to my render target.

	CRect<int> rect(0,0,m_nWidth,m_nHeight);
	R.StretchRect(m_pLightRT, NULL, m_pDiffuseRT, &rect, TEXF_POINT);

	return;
	if (m_bHDR)
	{
		GetSceneExposure();
	}

	//if(R.BeginFrame())
	// Get the lum texture
	{
		
		// 一定要把空白区域值为黑色
		R.SetTexture(0, m_pDiffuseRT);
	//	R.SetFVF(SceneBloomVertex::FVF);


		// Increase the contrast
		//R.SetTextureColorOP(0,TBOP_DOTPRODUCT3,TBS_TEXTURE,TBS_DIFFUSE);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));

		{
			int nInvExposure= (int)(m_fAdaptedLum * m_fAdaptedLum * 255.0f);
			Color32 clrFactor(nInvExposure,nInvExposure,nInvExposure,nInvExposure);
			//R.SetTextureFactor(clrFactor);
		}

		// To reduce the brightness
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
	R.SetShader("GammaCorrection");
	R.SetRenderTarget(0,m_pDiffuseRT);
	//R.ClearBuffer(false,true,0x0);
	R.SetTexture(0, m_pDiffuseRT);
	//R.SetTexture(0,g_samScene) ;
	R.SetFVF(QuadVertex::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_Quad, sizeof(QuadVertex));
}

void CSceneEffect::renderTargetBloom()
{
}

void CSceneEffect::renderTargetEnd()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	// Restore
	R.SetRenderTarget(0,m_pSystemRT);
	S_DEL(m_pSystemRT);
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
	if (m_nFlag==0)
	{
		R.SetShader("CombineAdd");
	}
	else
	{
		R.SetShader("Combine");
	}
	switch (m_nFlag)
	{
	case 0:
		R.SetTexture(0, m_pDiffuseRT);
		break;
	case 1:
		R.SetTexture(0, m_pPosMRT);
		break;
	case 2:
		R.SetTexture(0, m_pNormalMRT);
		break;
	case 3:
		R.SetTexture(0, m_pLightRT);
		break;
	case 4:
		R.SetTexture(0, m_pSceneRT2x);
		break;
	case 5:
		R.SetTexture(0, m_pSceneRT4x1);
		break;
	case 6:
		R.SetTexture(0, m_pShadowMapRT);
		break;
	case 7:
		R.SetTexture(0, m_pDiffuseCopyRT);
		break;
		
	}
	R.SetTexture(1, m_pSceneRT4x1);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, sizeof(QuadVertex));
}