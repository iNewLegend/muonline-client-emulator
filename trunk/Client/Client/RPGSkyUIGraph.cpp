#include "RPGSkyUIGraph.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "RPGSkyTextRender.h"
#include "Audio.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static RPGSkyUIGraph g_uiGraph;
// ----
UIGraph * UIGraph::m_pInstace = & g_uiGraph;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DWORD UI_SCREEN_VERTEX::FVF			= FVF_XYZRHW | FVF_DIFFUSE | FVF_TEX1;
DWORD UI_SCREEN_VERTEX_UNTEX::FVF	= FVF_XYZRHW | FVF_DIFFUSE;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::frameUpdate()
{
	getTextRender().OnFrameMove();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::DrawRect(const CRect<float> & rcDest, Color32 color)
{
	CRenderSystem & R = GetRenderSystem();
	// ----
	// # Since we're doing our own drawing here we need to flush the sprites
	// ----
	R.SetFVF(UI_SCREEN_VERTEX::FVF);
	// ----
	R.SetTextureColorOP(0, TBOP_SOURCE2);
	R.SetTextureAlphaOP(0, TBOP_SOURCE2);
	// ----
	GetGraphics().DrawRect(rcDest, color.c);
	// ----
	R.SetTextureColorOP(0, TBOP_MODULATE);
	R.SetTextureAlphaOP(0, TBOP_MODULATE);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::FillRect(const CRect<float> & rcDest, Color32 color)
{
	CRenderSystem & R = GetRenderSystem();
	// ----
	// # Since we're doing our own drawing here we need to flush the sprites
	// ----
	R.SetFVF(UI_SCREEN_VERTEX::FVF);
	// ----
	R.SetTextureColorOP(0, TBOP_SOURCE2);
	R.SetTextureAlphaOP(0, TBOP_SOURCE2);
	// ----
	GetGraphics().FillRect3D(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color.c);
	// ----
	R.SetTextureColorOP(0, TBOP_MODULATE);
	R.SetTextureAlphaOP(0, TBOP_MODULATE);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::DrawPolyLine(POINT * apPoints, UINT nNumPoints, Color32 color)
{
	UI_SCREEN_VERTEX * vertices = new UI_SCREEN_VERTEX[nNumPoints];
	// ----
	if(vertices != NULL)
	{
		UI_SCREEN_VERTEX * pVertex	= vertices;
		POINT * pt					= apPoints;
		// ----
		for(UINT i = 0 ; i < nNumPoints ; i++)
		{
			pVertex->x		= (float) pt->x;
			pVertex->y		= (float) pt->y;
			// ----
			pVertex->z		= 0.5f;
			pVertex->h		= 1.0f;
			pVertex->tu		= 0.0f;
			pVertex->tv		= 0.0f;
			// ----
			pVertex->color = color;
			// ----
			pVertex++;
			// ----
			pt++;
		}
		// ----
		CRenderSystem & R = GetRenderSystem();
		// ----
		R.SetFVF(UI_SCREEN_VERTEX::FVF);
		// ----
		R.SetTextureColorOP(0, TBOP_SOURCE2);
		R.SetTextureAlphaOP(0, TBOP_SOURCE2);
		// ----
		R.DrawPrimitiveUP(VROT_LINE_STRIP, (nNumPoints - 1), vertices, sizeof(UI_SCREEN_VERTEX));
		// ----
		R.SetTextureColorOP(0, TBOP_MODULATE);
		R.SetTextureAlphaOP(0, TBOP_MODULATE);
		// ----
		S_DELS(vertices);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void* RPGSkyUIGraph::createTexture(const char * szTexture)
{
	return (void*)GetRenderSystem().GetTextureMgr().RegisterTexture(szTexture);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::releaseTexture(void * pTexture)
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::DrawSprite3x3Grid(const CRect<float> & rcSrc, const CRect<float> & rcCenterSrc, const CRect<float> & rcDest, void * pTexture, Color32 color)
{
	if(color.a != 0)
	{
		GetGraphics().Draw3x3Grid3D(rcSrc, rcCenterSrc, rcDest, (int)pTexture, color);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::DrawSprite(const CRect<float> & rcSrc, const CRect<float> & rcDest, void * pTexture, Color32 color)
{
	if(color.a != 0)
	{
		GetGraphics().DrawTex3D(rcSrc, rcDest, (int)pTexture, color);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::DrawSprite(const CRect<float> & rcDest, void * pTexture, Color32 color)
{
	if(color.a != 0)
	{
		GetGraphics().DrawTex(rcDest, (int)pTexture, color);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::CalcTextRect(const wchar_t* wcsText, CRect<float> & rcDest)
{
	// ---
	// # (leo123) Why rcDest = rect and not directly to rcDest ? unless memory use.
	// ----
	// # if is not empty 
	// ----
	if(wcslen(wcsText) > 0) // not good
	{
		RECT rect = {0};
		// ----
		if(wcsText[0]==L'[')
		{
			rect	= getTextRender().CalcUBBRect(wcsText, rcDest.getRECT());
			rcDest	= rect;
		}
		else
		{
			getTextRender().drawText(wcsText, -1, rcDest.getRECT(), DTL_CALCRECT, 0, & rect);
			// ----
			rcDest	= rect;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::initDrawText()
{
	CRect<int>	rcViewport;
	Matrix		mView;
	Matrix		mProjection;
	// ---
	CRenderSystem & R		= GetRenderSystem();
	// ----
	R.SetLightingEnabled(false);
	R.SetBlendFunc(true);
	R.SetTextureColorOP(0,TBOP_MODULATE, TBS_TEXTURE,TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_MODULATE, TBS_TEXTURE,TBS_DIFFUSE);
	R.SetDepthBufferFunc(false, false);
	// ----
	R.getViewport(rcViewport);
	R.getViewMatrix(mView);
	R.getProjectionMatrix(mProjection);
	// ----
	mView.Invert();
	// ----
	{
		float fZ = 1;
		// ---
		Matrix mNewWorld= mView * 
			Matrix::newTranslation(Vec3D(- fZ / mProjection._11 , fZ / mProjection._22,fZ)) *
			Matrix::newScale(Vec3D(fZ / mProjection._11 / rcViewport.right * 2.0f,- fZ / mProjection._22 / rcViewport.bottom * 2.0f,
			- fZ / mProjection._33 / rcViewport.bottom* 2.0f));
		// ----
		R.setWorldMatrix(mNewWorld);
	}
}

void RPGSkyUIGraph::DrawText(const wchar_t* wcsText, CUIStyle & style, int nIndex,const CRect<float> & rcDest, int nCount)
{
	Color32 color			= 0xFFFFFFFF;
	// ----
	if(color.a != 0)
	{
		ULONG uFormat			= 0;
		// ----
		// No need to draw fully transparent layers
		// ----
		getTextRender().drawText(wcsText, nCount, rcDest.getRECT(), uFormat, color.c);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::drawText(const wchar_t* wcsText, int cchText, const RECT & rc, UINT format, unsigned long color, RECT * prcRet)
{
	CRenderSystem & R = GetRenderSystem();
	// ----
	R.SetTextureFactor(color);
	// ----
	R.SetTextureColorOP(1, TBOP_MODULATE, TBS_CURRENT,TBS_TFACTOR);
	R.SetTextureAlphaOP(1 ,TBOP_MODULATE, TBS_CURRENT,TBS_TFACTOR);
	// ----
	if(wcslen(wcsText) > 0) // not good
	{
		if(wcsText[0]==L'[')
		{
			getTextRender().DrawUBB(wcsText, rc);
		}
		else
		{
			getTextRender().drawText(wcsText, cchText,rc, format, color, prcRet);
		}
		// ----
		R.SetTextureColorOP(1, TBOP_DISABLE);
		R.SetTextureAlphaOP(1, TBOP_DISABLE);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::initFont(const char* szFontFilename, size_t size)
{
	getTextRender().load(szFontFilename);
	// ----
	getTextRender().setFontSize(size);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int RPGSkyUIGraph::GetFontSize()
{
	return getTextRender().GetCharHeight();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Matrix RPGSkyUIGraph::setUIMatrix(const Matrix & mTransform, const CRect<float> & rc, const Vec3D & vTranslation, const Vec3D & vRotate)
{
	CRect<int>	rcViewport;
	// ----
	Matrix		mProjection;
	Matrix		mRotate;
	Matrix		mTrans;
	Matrix		mView;
	// ----
	float fZ	= 50;
	// ----
	CRenderSystem & R = GetRenderSystem();
	// ----
	R.SetDepthBufferFunc(false, false);
	// ----
	R.getViewport(rcViewport);
	// ----
	mView.unit();
	// ----
	R.setViewMatrix(mView);
	// ----
	mProjection.MatrixPerspectiveFovLH(PI/4,(float)rcViewport.right/(float)rcViewport.bottom,0.1f,100);
	// ----
	R.setProjectionMatrix(mProjection);
	// ----
	mRotate.rotate(vRotate);
	// ----
	mTrans.translation(vTranslation);
	// ----
	Matrix mWorld = mTransform * Matrix::newTranslation(Vec3D(rc.left + 0.5f * rc.getWidth(), rc.top + 0.5f * rc.getHeight(), 0))
		* mTrans *mRotate *
		Matrix::newTranslation(Vec3D( -0.5f * rc.getWidth(), -0.5f * rc.getHeight(),0));
	{
		Matrix mNewWorld = Matrix::newTranslation(Vec3D(- fZ / mProjection._11, fZ / mProjection._22 , fZ))
			* Matrix::newScale(Vec3D(fZ / mProjection._11 / rcViewport.right * 2.0f,- fZ / mProjection._22 / rcViewport.bottom
			* 2.0f, - fZ / mProjection._33 / rcViewport.bottom * 2.0f)) * mWorld;
		// ----
		R.setWorldMatrix(mNewWorld);
	}
	// ----
	return mWorld;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool RPGSkyUIGraph::scriptStringAnalyse(CScriptStringAnalysis & analysis, const wchar_t *wcsText)
{
	return getTextRender().scriptStringAnalyse(analysis,wcsText);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RPGSkyUIGraph::playSound(const char * szFilename)
{
	GetAudio().playSound(szFilename);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CTextRender & RPGSkyUIGraph::getTextRender()
{
	static CRPGSkyTextRender s_RPGSkyTextRender;
	// ----
	return s_RPGSkyTextRender;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------