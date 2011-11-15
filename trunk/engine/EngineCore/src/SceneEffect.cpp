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
m_pSceneTexture(NULL),
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
	S_DEL(m_pSceneTexture);
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

	m_pSceneTexture = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);
	m_pTextureScene1 = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	m_pSceneCopyTexture = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	//m_pExposureTexture = R.GetTextureMgr().CreateRenderTarget(1,1);
	m_pBackTexture = R.GetTextureMgr().CreateRenderTarget(512, 512);//.CreateDynamicTexture(512,512);


	float fU0 = 0.0f;
	float fV0 = 0.0f;
	float fU1 = 1.0f;
	float fV1 = 1.0f;

	float fX0 = - 0.5f;
	float fY0 = - 0.5f;
	float fX1 =(float)nWidth - 0.5f;
	float fY1 =(float)nHeight - 0.5f;

	m_QuadVB[0].t = Vec2D(fU0, fV1);
	m_QuadVB[1].t = Vec2D(fU0, fV0);
	m_QuadVB[2].t = Vec2D(fU1, fV1);
	m_QuadVB[3].t = Vec2D(fU1, fV0);

	m_QuadVB[0].p = Vec4D(fX0, fY1, 0.0f, 1.0f);
	m_QuadVB[1].p = Vec4D(fX0, fY0, 0.0f, 1.0f);
	m_QuadVB[2].p = Vec4D(fX1, fY1, 0.0f, 1.0f);
	m_QuadVB[3].p = Vec4D(fX1, fY0, 0.0f, 1.0f);

	m_FloodLumVB[0].t = Vec2D(fU0, fV1);
	m_FloodLumVB[1].t = Vec2D(fU0, fV0);
	m_FloodLumVB[2].t = Vec2D(fU1, fV1);
	m_FloodLumVB[3].t = Vec2D(fU1, fV0);

	m_FloodLumVB[0].p = Vec4D(fX0, fY1, 0.0f, 1.0f);
	m_FloodLumVB[1].p = Vec4D(fX0, fY0, 0.0f, 1.0f);
	m_FloodLumVB[2].p = Vec4D(fX1, fY1, 0.0f, 1.0f);
	m_FloodLumVB[3].p = Vec4D(fX1, fY0, 0.0f, 1.0f);

	unsigned long BloomWeights[7] = 
	{
		0,
		225,
		155,
		83,
		35,
		11,
		3,
	};

	float fBloomWeights[7] = 
	{
		0.8f,
		0.08f,
		0.04f,
		0.03f,
		0.026f,
		0.018f,
		0.013f
		/*0.199471f,
		0.176033f,
		0.120985f,
		0.064759f,
		0.026995f,
		0.008764f,
		0.002216f,*/
	};

	for (size_t i=0;i<13;++i)
	{
		int nBaseID = i*6;

		m_BloomHVB[nBaseID+0].t = Vec2D(fU0, fV1);
		m_BloomHVB[nBaseID+1].t = Vec2D(fU0, fV0);
		m_BloomHVB[nBaseID+2].t = Vec2D(fU1, fV0);
		m_BloomHVB[nBaseID+3].t = Vec2D(fU0, fV1);
		m_BloomHVB[nBaseID+4].t = Vec2D(fU1, fV0);
		m_BloomHVB[nBaseID+5].t = Vec2D(fU1, fV1);

		m_BloomVVB[nBaseID+0].t = Vec2D(fU0, fV1);
		m_BloomVVB[nBaseID+1].t = Vec2D(fU0, fV0);
		m_BloomVVB[nBaseID+2].t = Vec2D(fU1, fV0);
		m_BloomVVB[nBaseID+3].t = Vec2D(fU0, fV1);
		m_BloomVVB[nBaseID+4].t = Vec2D(fU1, fV0);
		m_BloomVVB[nBaseID+5].t = Vec2D(fU1, fV1);

		m_BloomHVB[nBaseID+0].p = Vec4D(fX0,  fY1, 0.0f, 1.0f);
		m_BloomHVB[nBaseID+1].p = Vec4D(fX0,  fY0, 0.0f, 1.0f);
		m_BloomHVB[nBaseID+2].p = Vec4D(fX1,  fY0, 0.0f, 1.0f);
		m_BloomHVB[nBaseID+3].p = Vec4D(fX0,  fY1, 0.0f, 1.0f);
		m_BloomHVB[nBaseID+4].p = Vec4D(fX1,  fY0, 0.0f, 1.0f);
		m_BloomHVB[nBaseID+5].p = Vec4D(fX1,  fY1, 0.0f, 1.0f);

		m_BloomVVB[nBaseID+0].p = Vec4D(fX0,  fY1, 0.0f, 1.0f);
		m_BloomVVB[nBaseID+1].p = Vec4D(fX0,  fY0, 0.0f, 1.0f);
		m_BloomVVB[nBaseID+2].p = Vec4D(fX1,  fY0, 0.0f, 1.0f);
		m_BloomVVB[nBaseID+3].p = Vec4D(fX0,  fY1, 0.0f, 1.0f);
		m_BloomVVB[nBaseID+4].p = Vec4D(fX1,  fY0, 0.0f, 1.0f);
		m_BloomVVB[nBaseID+5].p = Vec4D(fX1,  fY1, 0.0f, 1.0f);
	}
	int nBaseID = 0;
	float fOffset = 1;
	for(int i = 0; i < 7; i++)
	{
		unsigned long Alpha =254.0f*fBloomWeights[i]+1;
		unsigned long color = (255<<24)+(Alpha<<16)+(Alpha<<8)+Alpha;
		if (i == 0)
		{
			for (int n = 0; n < 6; n++)
			{
				m_BloomHVB[nBaseID+n].c = color;
				m_BloomVVB[nBaseID+n].c = color;
			}
			nBaseID += 6;
		}
		else
		{
			for (int n = 0; n < 12; n++)
			{
				m_BloomHVB[nBaseID+n].c = color;
				m_BloomVVB[nBaseID+n].c = color;
			}
			for(size_t n=0;n<6;++n)
			{
				m_BloomHVB[nBaseID+n].p.x -= fOffset*i;
			}
			for(size_t n=6;n<12;++n)
			{
				m_BloomHVB[nBaseID+n].p.x += fOffset*i;
			}
			for(size_t n=0;n<6;++n)
			{
				m_BloomVVB[nBaseID+n].p.y -= fOffset*i;
			}
			for(size_t n=6;n<12;++n)
			{
				m_BloomVVB[nBaseID+n].p.y += fOffset*i;
			}
			nBaseID += 12;
		}
	}

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
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(QuadVertex));
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

void CSceneEffect::RenderBloom()
{
	CRenderSystem& R = CRenderSystem::getSingleton();

	
	renderTargetBegin();
	//R.ClearBuffer(true, true, 0xFFFFFFFF);
	//renderTargetGlow();
	renderTargetBloom();
	renderTargetEnd();
//	compose();
	// test
	//R.SetRenderTarget(m_pDepthRenderTarget);
	//if (R.BeginFrame())
	//{
	//	
	//}
	//R.EndFrame();
	//m_pDepthRenderTarget->SaveToFile(L"D:/Depth.bmp");

	//R.GetBackBuffer()
	//void *pContainer = NULL;
	//IDirect3DTexture9 *pTexture = NULL;
	//HRESULT hr = pSurface->GetContainer(IID_IDirect3DTexture9, &pContainer);
//	if (SUCCEEDED(hr) && pContainer)
	//{
	//	pTexture = (IDirect3DTexture9 *)pContainer;
	//}

	//int nAlpha = min(m_fBloomVal*255,255);
	//unsigned long dwFactor = nAlpha<<24;

	//R.SetShader(-1);


	//D3DXSaveSurfaceToFileW(L"D:/m_pSceneTexture.bmp", D3DXIFF_BMP, m_pSceneTexture, NULL, NULL);
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
	R.SetRenderTarget(m_pSceneTexture);
}

void CSceneEffect::renderTargetGlow()// not good
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	// first: copy system render target to my render target.

	CRect<int> rect(0,0,m_nWidth,m_nHeight);
	R.StretchRect(m_pGlowRenderTarget, NULL, m_pSceneTexture, &rect, TEXF_POINT);

	return;
	if (m_bHDR)
	{
		GetSceneExposure();
	}

	//if(R.BeginFrame())
	// Get the lum texture
	{
		
		// 一定要把空白区域值为黑色
		R.SetTexture(0, m_pSceneTexture);
		R.SetFVF(SceneBloomVertex::FVF);


		R.SetBlendFunc(false);
		// Increase the contrast
		//R.SetTextureColorOP(0,TBOP_DOTPRODUCT3,TBS_TEXTURE,TBS_DIFFUSE);
		R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_TEXTURE);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(SceneBloomVertex));

		{
			int nInvExposure= (int)(m_fAdaptedLum * m_fAdaptedLum * 255.0f);
			Color32 clrFactor(nInvExposure,nInvExposure,nInvExposure,nInvExposure);
			R.SetTextureFactor(clrFactor);
		}

		// To reduce the brightness
		R.SetTextureColorOP(0,TBOP_SUBTRACT,TBS_TEXTURE,TBS_TFACTOR);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(SceneBloomVertex));


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
	R.SetRenderTarget(m_pSceneTexture);
	//R.ClearBuffer(false,true,0x0);
	//R.GetShaderMgr().getSharedShader()->setTexture("g_texScene",m_pSceneTexture);
	R.SetTexture(0, m_pSceneTexture);

	if(R.prepareMaterial("GammaCorrection"))
	{
		CRenderSystem& R = CRenderSystem::getSingleton();
		//R.SetTexture(0,g_samScene) ;
		R.SetFVF(QuadVertex::FVF);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(QuadVertex));
		R.finishMaterial();
	}
}

void CSceneEffect::renderTargetBloom()
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	R.SetBlendFunc(true,BLENDOP_ADD,SBF_ONE,SBF_ONE);
	R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);
	R.SetFVF(SceneBloomVertex::FVF);

	R.SetCullingMode(CULL_NONE);
	R.SetLightingEnabled(false);
	R.SetDepthBufferFunc(false,false);
	// 第三步：生成光晕-纵横模糊
	for (int nBloomCount = 0; nBloomCount<1; nBloomCount++)
	{
		// 横模糊
		R.SetRenderTarget(m_pTextureScene1);
		R.ClearBuffer(false,true,0x0);
		R.SetTexture(0, m_pSceneTexture);
		R.DrawPrimitiveUP(VROT_TRIANGLE_LIST, 26, m_BloomHVB, sizeof(SceneBloomVertex));

		// 纵模糊
		R.SetRenderTarget(m_pSceneTexture);
		R.ClearBuffer(false,true,0x0);
		R.SetTexture(0, m_pTextureScene1);
		R.DrawPrimitiveUP(VROT_TRIANGLE_LIST, 26, m_BloomVVB, sizeof(SceneBloomVertex));
	}
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

	CRenderSystem& R = CRenderSystem::getSingleton();
	static bool bcan = true;
	R.SetCullingMode(CULL_NONE);
	R.SetLightingEnabled(false);
	R.SetDepthBufferFunc(false,false);
	R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_ONE,SBF_ONE);

	R.SetTextureColorOP(0,TBOP_SOURCE1);

	R.SetTexCoordIndex(0, 0);

	//if(R.BeginFrame())
	{
		R.SetTexture(0, m_pSceneTexture);
		R.SetFVF(QuadVertex::FVF);

		if (m_bHDR)
		{
			// 减低亮度
			R.SetBlendFunc(bcan,BLENDOP_REVSUBTRACT,SBF_ONE,SBF_ONE);
			//r->BlendOP(BLENDOP_REVSUBTRACT);BLENDOP_REVSUBTRACT
			float fLumScale = m_fHDRKey/m_fAdaptedLum;


			if (fLumScale > 1.0f)// 加亮屏幕操作
			{
				fLumScale -= 1.0f;
				unsigned char ucFactor = min(255,fLumScale*255);
				Color32 clrFactor(ucFactor,255,255,255);
				R.SetTextureFactor(clrFactor);
				R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_SOURCE_ALPHA);

				R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
				R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TFACTOR);

				R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, sizeof(QuadVertex));
			}
			else// 减暗屏幕操作
			{
				unsigned char ucFactor = min(255,fLumScale*255);
				Color32 clrFactor(ucFactor,ucFactor,ucFactor,ucFactor);
				R.SetTextureFactor(clrFactor);
				R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_ZERO);

				R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
				R.SetTextureAlphaOP(0,TBOP_DISABLE);
				R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, sizeof(QuadVertex));
			}
		}

		R.SetSamplerFilter(0, TEXF_POINT, TEXF_POINT, TEXF_POINT);
		// 添加光晕
		R.SetBlendFunc(false,BLENDOP_ADD,SBF_ONE,SBF_ONE);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE);
		R.SetTextureAlphaOP(0,TBOP_DISABLE);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, sizeof(QuadVertex));
		R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		//	R.EndFrame();
	}
}