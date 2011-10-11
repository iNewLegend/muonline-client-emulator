#include "Graphics.h"
#include "RenderSystem.h"
#include "Vec4D.h"
#include "Vec3D.h"

#define MAX_VBSIZE 10000

CGraphics& GetGraphics()
{
	static CGraphics g_Graphics;
	return g_Graphics;
}

CGraphics::CGraphics(void)
{
	m_pVB = NULL;
	m_pIB = NULL;

	m_nVBOffset = 0;
	m_nVBBatchSize = 0;
	m_nCount = 0;

	/*m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(MAX_VBSIZE, sizeof(VERTEX_XYZ_DIF_TEX),
		CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(6000);
	WORD* pIndex = (WORD*)m_pIB->lock(0, 6000*sizeof(WORD), CHardwareBuffer::HBL_NORMAL);
	for(int i = 0 ; i < 4000 ; i+=4)
	{
		*pIndex= i;
		pIndex++;
		*pIndex= i+1;
		pIndex++;
		*pIndex= i+2;
		pIndex++;
		*pIndex= i;
		pIndex++;
		*pIndex= i+2;
		pIndex++;
		*pIndex= i+3;
		pIndex++;
	}
	m_pIB->unlock();*/
}

CGraphics::~CGraphics(void)
{
	S_DEL(m_pVB);
	S_DEL(m_pIB);
}

void CGraphics::Clear()
{
	m_nVBOffset = 0;
}

CGraphics& CGraphics::begin(VertexRenderOperationType mode, size_t uSize)
{
	m_nCount = 0;
	m_uMode = mode;
	m_nVBBatchSize = uSize;
	if (m_nVBOffset + uSize > MAX_VBSIZE)
	{
		m_nVBOffset = 0;
	}
	m_pVertex = NULL;
	if (m_pVB)
	{
		m_pVertex = (VERTEX_XYZ_DIF_TEX*)m_pVB->lock(m_nVBOffset * sizeof(VERTEX_XYZ_DIF_TEX), uSize * sizeof(VERTEX_XYZ_DIF_TEX),
			m_nVBOffset?CHardwareBuffer::HBL_NO_OVERWRITE:CHardwareBuffer::HBL_DISCARD);
	}
	//assert(m_pVB);
	//assert(m_pVertex);
	return *this;
}

CGraphics& CGraphics::c(Color32 c)
{
	m_TempVertex.c = c;
	return *this;
}

CGraphics& CGraphics::t(const Vec2D& v)
{
	m_TempVertex.t = v;
	return *this;
}

CGraphics& CGraphics::t(float u, float v)
{
	m_TempVertex.t.x = u;
	m_TempVertex.t.y = v;
	return *this;
}

CGraphics& CGraphics::v(const Vec3D& v)
{
	m_TempVertex.p = v;
	m_pVertex[m_nCount] = m_TempVertex;
	m_nCount++;
	if (m_nCount>=10000)
	{
		m_nCount=0;
	}
	return *this;
}

CGraphics& CGraphics::v(float x, float y, float z)
{
	m_TempVertex.p.x = x;
	m_TempVertex.p.y = y;
	m_TempVertex.p.z = z;
	m_pVertex[m_nCount] = m_TempVertex;
	m_nCount++;
	return *this;
}

void CGraphics::end()
{
	if (!m_pVB || m_nCount==0) return;
	m_pVB->unlock();
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);
	R.SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_XYZ_DIF_TEX));

	switch(m_uMode)
	{
		case VROT_POINT_LIST:
			R.DrawPrimitive(VROT_POINT_LIST, m_nVBOffset, m_nCount);
			break;
		case VROT_LINE_LIST:
			R.DrawPrimitive(VROT_LINE_LIST, m_nVBOffset, m_nCount/2);
			break;
		case VROT_TRIANGLE_LIST:
			R.SetIndices(m_pIB);
			R.DrawIndexedPrimitive(VROT_TRIANGLE_LIST, m_nVBOffset, 0, m_nCount, 0, m_nCount/2);
			break;
		case VROT_TRIANGLE_STRIP:
			R.DrawPrimitive(VROT_TRIANGLE_STRIP, m_nVBOffset, m_nCount-2);
			break;
		case VROT_TRIANGLE_FAN:
			R.DrawPrimitive(VROT_TRIANGLE_FAN, m_nVBOffset, m_nCount-2);
			break;
		default:
			break;
	}

///	assert(m_nCount>0);

///	assert(m_nVBBatchSize>=m_nCount);
	m_nVBOffset += m_nCount;
	m_nCount = 0;
}


//void CGraphics::Vertex2fv(const float *v)
//{
//	m_TempVertex.p = *(Vec3D*)v;
//	m_Buffer[m_nCount] = m_TempVertex;
//	m_nCount++;
//}
//
//void CGraphics::Vertex3fv(const float *v)
//{
//	m_TempVertex.p = *(Vec3D*)v;
//	m_Buffer[m_nCount] = m_TempVertex;
//	m_nCount++;
//}

//void CGraphics::End()
//{
//	CRenderSystem& R = GetRenderSystem();
//	R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);

//		switch(m_dwMode) {
//		case BGMODE_POIN:
//			break;
//		case BGMODE_LINE:
//			R.DrawPrimitiveUP(VROT_LINE_LIST, m_nCount/2,m_Buffer + m_nOffset, sizeof(VERTEX_XYZ_DIF_TEX));
//			break;
//		case BGMODE_LINESTRIP:
//			break;
//		case BGMODE_QUADS:
//			R.DrawPrimitiveUP(VROT_TRIANGLE_LIST, m_nCount/3,m_Buffer + m_nOffset, sizeof(VERTEX_XYZ_DIF_TEX));
//			break;
//		case BGMODE_QUADSTRIP:
//			R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP,m_nCount-2,m_Buffer + m_nOffset, sizeof(VERTEX_XYZ_DIF_TEX));
//			break;
//		default:
//			break;
//		}
//	
//	m_nOffset += m_nCount;
//	m_nCount = 0;
//}

void CGraphics::Line2DBegin()
{
	m_set2DLine.clear();
}

void CGraphics::Line2DTo(const Vec2D& v)
{
	VERTEX_XYZW_DIF	temp;
	temp.p = Vec4D(v.x, v.y, 0, 1);
	temp.c = m_TempVertex.c;
	m_set2DLine.push_back(temp);
}

void CGraphics::Line2DEnd()
{
	if (m_set2DLine.size()>1)
	{
		CRenderSystem& R = GetRenderSystem();
		R.SetFVF(VERTEX_XYZW_DIF::FVF);

		R.DrawPrimitiveUP(VROT_LINE_STRIP, m_set2DLine.size()-1, &m_set2DLine[0], sizeof(VERTEX_XYZW_DIF));
	}
}

void CGraphics::Line3DBegin()
{
	m_set3DLine.clear();
}

void CGraphics::Line3DTo(const Vec3D& v)
{
	VERTEX_XYZ_DIF	temp;
	temp.p = v;
	temp.c = m_TempVertex.c;
	m_set3DLine.push_back(temp);
}

void CGraphics::Line3DEnd()
{
	if (m_set3DLine.size()>1)
	{
		CRenderSystem& R = GetRenderSystem();
		R.SetFVF(VERTEX_XYZ_DIF::FVF);

		R.DrawPrimitiveUP(VROT_LINE_STRIP, m_set3DLine.size()-1, &m_set3DLine[0], sizeof(VERTEX_XYZ_DIF));
	}
}

//////////////////////////////////////////////////////////////////////////
//2D Graph
//////////////////////////////////////////////////////////////////////////
void CGraphics::DrawLine(float x0, float y0, float x1, float y1, Color32 color)
{
	VERTEX_XYZ_DIF v[2]=
	{
		Vec3D(x0, y0, 0.0f), color,
		Vec3D(x1, y1, 0.0f), color,
	};
	GetRenderSystem().SetFVF(VERTEX_XYZ_DIF::FVF);
	GetRenderSystem().DrawPrimitiveUP(VROT_LINE_STRIP, 1, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::DrawRect(float x0, float y0, float x1, float y1, Color32 color)
{
	// border adjustment
	if(x0<x1)
		++x1;
	else if(x0>x1)
		++x0;
	if(y0<y1)
		++y1;
	else if(y0>y1)
		++y0;
	VERTEX_XYZ_DIF v[5]=
	{
		Vec3D(x0, y0, 0.0f), color,
		Vec3D(x1, y0, 0.0f), color,
		Vec3D(x1, y1, 0.0f), color,
		Vec3D(x0, y1, 0.0f), color,
		Vec3D(x0, y0, 0.0f), color,
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_LINE_STRIP, 4, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::DrawRect(const CRect<float>& rcDest, Color32 color)
{
	DrawRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color);
}

void CGraphics::FillRect(float x0, float y0, float x1, float y1, Color32 color)
{
	// border adjustment
	/*if(x0<x1)
		++x1;
	else if(x0>x1)
		++x0;
	if(y0<y1)
		++y1;
	else if(y0>y1)
		++y0;*/
	VERTEX_XYZ_DIF v[4]=
	{
		Vec3D(x0, y0, 0.0f), color,
		Vec3D(x1, y0, 0.0f), color,
		Vec3D(x1, y1, 0.0f), color,
		Vec3D(x0, y1, 0.0f), color,
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::FillRect(float x0, float y0, float x1, float y1, Color32 color0, Color32 color1, Color32 color2, Color32 color3)
{
	// border adjustment
	/*if(x0<x1)
		++x1;
	else if(x0>x1)
		++x0;
	if(y0<y1)
		++y1;
	else if(y0>y1)
		++y0;*/
	VERTEX_XYZ_DIF v[4]=
	{
		Vec3D(x0, y0, 0.0f), color0,
		Vec3D(x1, y0, 0.0f), color1,
		Vec3D(x1, y1, 0.0f), color2,
		Vec3D(x0, y1, 0.0f), color3,
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::FillRect(const CRect<float>& rcDest, Color32 color)
{
	FillRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color);
}

void CGraphics::FillRect(const CRect<float>& rcDest, Color32 color, Color32 color0, Color32 color1, Color32 color2, Color32 color3)
{
	FillRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color0, color1, color2, color3);
}

//////////////////////////////////////////////////////////////////////////

void CGraphics::drawQuad(const RECT& rcDest, const RECT& rcSrc, int nWidth, int nHeight, Color32 color)
{
	CRenderSystem& R = GetRenderSystem();
	float u0 = ((float)rcSrc.left+0.5f)	/ (float)nWidth;
	float v0 = ((float)rcSrc.top+0.5f)	/ (float)nHeight;
	float u1 = ((float)rcSrc.right)		/ (float)nWidth;
	float v1 = ((float)rcSrc.bottom)	/ (float)nHeight;
	VERTEX_XYZ_DIF_TEX v[4]=
	{
		Vec3D( (float)rcDest.left,	(float)rcDest.top,		0.0f), color, Vec2D(u0, v0),
		Vec3D( (float)rcDest.right,	(float)rcDest.top,		0.0f), color, Vec2D(u1, v0),
		Vec3D( (float)rcDest.right,	(float)rcDest.bottom,	0.0f), color, Vec2D(u1, v1),
		Vec3D( (float)rcDest.left,	(float)rcDest.bottom,	0.0f), color, Vec2D(u0, v1),
	};
	R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZ_DIF_TEX));
}

void CGraphics::drawTex(int destX, int destY, int nTexID, Color32 color, const RECT* prcSrc)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	// ----
	if (!pTex)
	{
		return;
	}
	// ----
	GetRenderSystem().SetTexture(0, nTexID);
	// ----
	int nTexWidth = pTex->GetWidth();
	int nTexHeight = pTex->GetHeight();
	// ----
	if (!prcSrc)
	{
		RECT rcSrc={0,0,nTexWidth,nTexHeight};
		RECT rcDest={destX,destY,nTexWidth+destX,nTexHeight+destY};
		drawQuad(rcDest, rcSrc, nTexWidth, nTexHeight, color);
	}
	else
	{
		RECT rcDest={destX,destY,destX+prcSrc->right-prcSrc->left,destY+prcSrc->bottom-prcSrc->top};
		drawQuad(rcDest, *prcSrc, nTexWidth, nTexHeight, color);
	}
}

void CGraphics::drawTex(const RECT& rcDest, int nTexID, Color32 color, const RECT* prcSrc, const RECT* prcCenterSrc)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	// ----
	if (!pTex)
	{
		return;
	}
	// ----
	GetRenderSystem().SetTexture(0, nTexID);
	// ----
	int nTexWidth = pTex->GetWidth();
	int nTexHeight = pTex->GetHeight();
	// ----
	if (!prcSrc)
	{
		RECT rcSrc={0,0,nTexWidth,nTexHeight};
		drawQuad(rcDest, rcSrc, nTexWidth, nTexHeight, color);
	}
	else if (!prcCenterSrc)
	{
		drawQuad(rcDest, *prcSrc, nTexWidth, nTexHeight, color);
	}
	else
	{
		CRect<float> rcCenterDest(
			rcDest.left + (prcCenterSrc->left - prcSrc->left),
			rcDest.top + (prcCenterSrc->top - prcSrc->top),
			rcDest.right + (prcCenterSrc->right - prcSrc->right),
			rcDest.bottom + (prcCenterSrc->bottom - prcSrc->bottom));

		if(rcCenterDest.left>rcCenterDest.right)
		{
			rcCenterDest.left=(rcDest.left+rcDest.right)*0.5f;
			rcCenterDest.right=(rcDest.left+rcDest.right)*0.5f;
		}
		if(rcCenterDest.top>rcCenterDest.bottom)
		{
			rcCenterDest.top=(rcDest.top+rcDest.bottom)*0.5f;
			rcCenterDest.bottom=(rcDest.top+rcDest.bottom)*0.5f;
		}
		VERTEX_XYZ_DIF_TEX vertex[4*4];
		for (int i = 0; i<16; i++)
		{
			vertex[i].p.z = 0.0f;
			vertex[i].c = color;
		}
		//////////////////////////////////////////////////////////////////////////
		float X[4] =
		{
			rcDest.left,
			rcCenterDest.left,
			rcCenterDest.right,
			rcDest.right,
		};
		float U[4] =
		{
			(prcSrc->left+0.5f)/nTexWidth,
			(prcCenterSrc->left+0.5f)/nTexWidth,
			(prcCenterSrc->right+0.5f)/nTexWidth,
			(prcSrc->right+0.5f)/nTexWidth,
		};
		for (int i = 0; i<4; i++)
		{
			for (int n = 0; n < 4; n ++)
			{
				int index = i*4+n;
				vertex[index].p.x = X[n];
				vertex[index].t.x = U[n];
			}
		}
		//////////////////////////////////////////////////////////////////////////
		float Y[4] =
		{
			rcDest.top,
			rcCenterDest.top,
			rcCenterDest.bottom,
			rcDest.bottom,
		};
		float V[4] =
		{
			(prcSrc->top+0.5f)/nTexHeight,
			(prcCenterSrc->top+0.5f)/nTexHeight,
			(prcCenterSrc->bottom+0.5f)/nTexHeight,
			(prcSrc->bottom+0.5f)/nTexHeight,
		};
		for (int i = 0; i<4; i++)
		{
			for (int n = 0; n < 4; n ++)
			{
				int index = i+n*4;
				vertex[index].p.y = Y[n];
				vertex[index].t.y = V[n];
			}
		}
		//////////////////////////////////////////////////////////////////////////
		const static unsigned short index[3*3*6] = { 0,0+1,0+5,		0,0+5,0+4,
			1,1+1,1+5,		1,1+5,1+4,
			2,2+1,2+5,		2,2+5,2+4,
			4,4+1,4+5,		4,4+5,4+4,
			5,5+1,5+5,		5,5+5,5+4,
			6,6+1,6+5,		6,6+5,6+4,
			8,8+1,8+5,		8,8+5,8+4,
			9,9+1,9+5,		9,9+5,9+4,
			10,10+1,10+5,	10,10+5,10+4};

		CRenderSystem& R = GetRenderSystem();
		R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, 4*4, 3*3*2, index, vertex, sizeof(VERTEX_XYZ_DIF_TEX));
	}
	// ----
}

//////////////////////////////////////////////////////////////////////////
// 3D Graph
//////////////////////////////////////////////////////////////////////////
void CGraphics::DrawLine3D(const Vec3D& v0,const Vec3D& v1, Color32 color)
{
	VERTEX_XYZ_DIF v[2];

	v[0].p = v0;
	v[1].p = v1;
	v[0].c = color;
	v[1].c = color;

	CRenderSystem& R = GetRenderSystem();

	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_LINE_STRIP, 1, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::drawCross3D(const Vec3D& vPos,float fLength, Color32 color)
{
	DrawLine3D(vPos+Vec3D(-fLength,0.0f,0.0f),vPos+Vec3D(fLength,0.0f,0.0f),color);
	DrawLine3D(vPos+Vec3D(0.0f,-fLength,0.0f),vPos+Vec3D(0.0f,fLength,0.0f),color);
	DrawLine3D(vPos+Vec3D(0.0f,0.0f,-fLength),vPos+Vec3D(0.0f,0.0f,fLength),color);
}

void CGraphics::drawBBox(const BBox& bbox, Color32 color)
{
	VERTEX_XYZ_DIF vtx[8] =
	{
		Vec3D(bbox.vMin.x, bbox.vMax.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMax.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMax.y, bbox.vMin.z), color,
		Vec3D(bbox.vMin.x, bbox.vMax.y, bbox.vMin.z), color,
		Vec3D(bbox.vMin.x, bbox.vMin.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMin.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMin.y, bbox.vMin.z), color,
		Vec3D(bbox.vMin.x, bbox.vMin.y, bbox.vMin.z), color,
	};

	//Index3w		idx[12] = 
	//{ 
	//	//{ 0, 1, 2 }, { 0, 2, 3 },	/// 上方
	//	//{ 4, 6, 5 }, { 4, 7, 6 },	/// 下方
	//	//{ 0, 3, 7 }, { 0, 7, 4 },	/// 左方
	//	//{ 1, 5, 6 }, { 1, 6, 2 },	/// 右方
	//	//{ 3, 2, 6 }, { 3, 6, 7 },	/// 前方
	//	//{ 0, 4, 5 }, { 0, 5, 1 }	/// 后方
	//}
	unsigned short		idx[24] = 
	{ 
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7

	};
	CRenderSystem& R = GetRenderSystem();
	R.SetTextureColorOP(0,TBOP_SOURCE2, TBS_TEXTURE, TBS_DIFFUSE);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawIndexedPrimitiveUP(VROT_LINE_LIST, 0, 8, 12, idx, vtx, sizeof(VERTEX_XYZ_DIF));
}