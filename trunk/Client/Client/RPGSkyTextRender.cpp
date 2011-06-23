#include "RPGSkyTextRender.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # Made by	:  RPGSky
// # Date		:  04.08.2010
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static WORD s_DrawTextIB[DRAW_TEXT_MAX * 6];
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRPGSkyTextRender::CRPGSkyTextRender()
{
	for(int i = 0 ; i < DRAW_TEXT_MAX ; i++)
	{
		s_DrawTextIB[i * 6]		= i*4;
		s_DrawTextIB[i * 6 + 1]	= i*4+1;
		s_DrawTextIB[i * 6 + 2]	= i*4+2;
		s_DrawTextIB[i * 6 + 3]	= i*4;
		s_DrawTextIB[i * 6 + 4]	= i*4+2;
		s_DrawTextIB[i * 6 + 5]	= i*4+3;
	}
	// ----
	m_pTextTexture = NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRPGSkyTextRender::updateTextureBuffer(unsigned char* pBuffer, size_t size, size_t width, size_t height)
{
	if(m_pTextTexture == NULL)
	{
		m_pTextTexture = GetRenderSystem().GetTextureMgr().CreateTexture(width, height,1);
		// ----
		//GetRenderSystem().GetTextureMgr().CreateDynamicTexture(width, height);
	}
	if(m_pTextTexture)
	{
		m_pTextTexture->FillLevel(pBuffer, size, width, height, 0);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRPGSkyTextRender::drawTextVertexBuffer(int nVertexCount, void* pBuffer)
{
	CRenderSystem & R = GetRenderSystem();
	// ----
	R.SetTexture(0, m_pTextTexture);
	// ----
	#ifdef _3D_TEXT
		R.SetFVF(FVF_XYZ | FVF_DIFFUSE | FVF_TEX1);
		// ----
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, nVertexCount, nVertexCount / 2, & s_DrawTextIB, pBuffer, sizeof(VERTEX_3D_TEXT));
	#else
		R.SetFVF(FVF_XYZW | FVF_DIFFUSE | FVF_TEX1);
		// ----
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, nVertexCount, nVertexCount / 2, & s_DrawTextIB, pBuffer, sizeof(VERTEX_TEXT));
	#endif
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------