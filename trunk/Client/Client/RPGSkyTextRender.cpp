#include "RPGSkyTextRender.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # Made by	:  RPGSky
// # Date		:  04.08.2010
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static WORD s_DrawTextIB[DRAW_TEXT_MAX * 6];
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRPGSkyTextRender3D::CRPGSkyTextRender3D()
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

void CRPGSkyTextRender3D::updateTextureBuffer(unsigned char* pBuffer, size_t size, size_t width, size_t height)
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

void CRPGSkyTextRender3D::drawUBB(const CUBB* pUBB)const
{
	int nVertexCount = pUBB->m_VB.size();
	if (nVertexCount)
	{
		CRenderSystem & R = GetRenderSystem();
		// ----
		R.SetTexture(0, m_pTextTexture);
		// ----
		CVertexDeclaration* VertexDeclaration = R.CreateVertexDeclaration();
		VertexDeclaration->AddElement(0, 0, VET_FLOAT3, VES_POSITION);
		VertexDeclaration->AddElement(0, 12, VET_COLOUR, VES_DIFFUSE);
		VertexDeclaration->AddElement(0, 16, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		VertexDeclaration->EndElement();
		// ----
		R.SetVertexDeclaration(VertexDeclaration);
		// ----
		if (m_nShadowBorder>0)
		{
			static unsigned long s_uShader = R.GetShaderMgr().registerItem("EngineRes\\fx\\text.fx");
			Vec4D vColor = m_uShadowColor;
			R.SetPixelShaderConstantF(0,(float*)&vColor,1);
			R.SetShader(s_uShader);
		}
		// ----
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, nVertexCount, nVertexCount / 2, & s_DrawTextIB, &pUBB->m_VB[0], sizeof(VERTEX_TEXT));
		// ----
		R.SetShader((CShader*)NULL);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------