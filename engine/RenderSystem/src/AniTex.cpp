#include "AniTex.h"
#include "RenderSystem.h"

CAniTex::CAniTex()
{
	m_nTexID=0;
	m_nXCount=1;
	m_nYCount=1;
	m_nStart=0;
	m_nCount=1;
	m_nSpeed=1;
}

CAniTex::~CAniTex()
{
}

void CAniTex::Draw(int x, int y, double fTime, Color32 color)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(m_nTexID);
	if (pTex)
	{
		RECT rcDest;
		SetRect(&rcDest, x, y, x+pTex->GetWidth(), y+pTex->GetHeight());
		Draw(rcDest, fTime, color);
	}
}

void CAniTex::Draw(const RECT& rcDest, double fTime, Color32 color)
{
	if (m_nXCount<=0&&m_nYCount<=0)
	{
		return;
	}
	int nFrame = m_nStart;
	if (m_nSpeed>0&&m_nCount>0)
	{
		nFrame+=int(fTime*1000/m_nSpeed)%m_nCount;
	}

	int nX = nFrame%m_nXCount;
	int nY = nFrame/m_nXCount;
	float fWidth = 1.0f/m_nXCount;
	float fHeight = 1.0f/m_nYCount;

	float u0 = nX*fWidth;
	float v0 = nY*fHeight;
	float u1 = u0+fWidth;
	float v1 = v0+fHeight;
	VERTEX_XYZW_DIF_TEX v[4]=
	{
		Vec4D((float) rcDest.left,	(float) rcDest.top,		0.5f, 1.0f), color, Vec2D(u0, v0),
		Vec4D((float) rcDest.right,	(float) rcDest.top,		0.5f, 1.0f), color, Vec2D(u1, v0),
		Vec4D((float) rcDest.right,	(float) rcDest.bottom,	0.5f, 1.0f), color, Vec2D(u1, v1),
		Vec4D((float) rcDest.left,	(float) rcDest.bottom,	0.5f, 1.0f), color, Vec2D(u0, v1),
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetTexture(0, m_nTexID);
	R.SetFVF(VERTEX_XYZW_DIF_TEX::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZW_DIF_TEX));
}