#pragma once
#include "RenderSystem.h"
#include "Frustum.h"
#include <Windows.h>
#include "Rect.h"

class CGraphics
{
public:
	CGraphics(void);
	~CGraphics(void);
public:
	void Clear();
	// ----
	CGraphics&	begin	(VertexRenderOperationType mode, size_t uSize);
	CGraphics&	c		(Color32 c);
	CGraphics&	t		(const Vec2D& v);
	CGraphics&	t		(float u, float v);
	CGraphics&	v		(const Vec3D& v);
	CGraphics&	v		(float x, float y, float z);
	void		end		();
	// ----
	// 2D几何绘制
	// 直线
	void DrawLine	(float x0, float y0, float x1, float y1, Color32 color);
	// 矩形
	void DrawRect	(float x0, float y0, float x1, float y1, Color32 color);
	void DrawRect	(const CRect<float>& rcDest, Color32 color);
	void FillRect	(float x0, float y0, float x1, float y1, Color32 color);
	void FillRect	(float x0, float y0, float x1, float y1, Color32 color0, Color32 color1, Color32 color2, Color32 color3);
	void FillRect	(const CRect<float>& rcDest, Color32 color);
	void FillRect	(const CRect<float>& rcDest, Color32 color, Color32 color0, Color32 color1, Color32 color2, Color32 color3);
	// 椭圆形
	void DrawCircle	(const CRect<float>& rcDest, Color32 color);
	void FillCircle	(const CRect<float>& rcDest, Color32 color);
	// Quadrilateral
	void drawQuad	(const RECT& rcDest, const RECT& rcSrc, float fWidth, float fHeight, Color32 color);
	void drawTex	(int destX, int destY, int nTexID, Color32 color=0xFFFFFFFF, const RECT* rcSrc=NULL);
	void drawTex	(const RECT& rcDest, int nTexID, Color32 color=0xFFFFFFFF, const RECT* rcSrc=NULL, const RECT* rcCenterSrc=NULL);
	//
	void Draw3x3Grid(const CRect<float>& rcSrc, const CRect<float>& rcCenterSrc, const CRect<float>& rcDest, int nTexID, Color32 color);
	// 2D线
	void Line2DBegin();
	void Line2DTo	(const Vec2D& v);
	void Line2DEnd	();
	// 3D线
	void Line3DBegin();
	void Line3DTo	(const Vec3D& v);
	void Line3DEnd	();
	//////////////////////////////////////////////////////////////////////////
	// 3D绘制
	// 3D直线
	void DrawLine3D	(const Vec3D& v0,const Vec3D& v1, Color32 color);
	void drawCross3D(const Vec3D& vPos,float fLength, Color32 color);
	// 包围盒
	void drawBBox	(const BBox& bbox, Color32 color);
protected:
	unsigned long					m_uMode;
	VERTEX_XYZ_DIF_TEX				m_Buffer[10000];
	CHardwareVertexBuffer*			m_pVB;			// 顶点缓冲
	CHardwareIndexBuffer*			m_pIB;			// 索引缓冲
	int								m_nCount;
	int								m_nVBOffset;
	int								m_nVBBatchSize;
	VERTEX_XYZ_DIF_TEX				m_TempVertex;	// 顶点缓冲 lock的时候得到的值
	VERTEX_XYZ_DIF_TEX*				m_pVertex;

	std::vector<VERTEX_XYZW_DIF>	m_set2DLine;
	std::vector<VERTEX_XYZ_DIF>		m_set3DLine;
};

CGraphics& GetGraphics();