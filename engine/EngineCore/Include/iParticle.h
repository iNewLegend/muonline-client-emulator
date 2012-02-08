#pragma once
#include "Frustum.h"

struct ParticleData
{
public:
	struct TexCoordSet
	{
		Vec2D tc[4];
	};
	float m_Rate;//产生率
	float m_Spread1, m_Spread2;// 扩散
	float m_Lifespan;	// 生命周期
	float m_fLifeMid;	// 生命中点
	float m_fSpeed1;	// 初速度
	float m_fSpeed2;	// 初速度
	float m_Areal;		// 地区L
	float m_Areaw;		// 地区W
	float m_fGravity;	// 重量
	float m_fDampen;	// 减速度
	Color32 m_fColor1;	// 颜色x3
	Color32 m_fColor2;	// 颜色x3
	Color32 m_fColor3;	// 颜色x3
	float m_fSize1;		// 大小x3
	float m_fSize2;		// 大小x3
	float m_fSize3;		// 大小x3
	float m_fRotation;	// 旋转
	int m_nOrder, type;
	// 纹理动画的Tile信息
	int m_nRows, m_nCols;
	std::vector<TexCoordSet> m_Tiles;
	bool m_bBillboard;			// 公告板
	long flags;
	float tofs;
	CMaterial m_Material;
public:
	ParticleData(): m_fLifeMid(0)
	{
		//		m_nBlend = 0;
		m_nOrder = 0;
		type = 0;

		m_nRows = 0;
		m_nCols = 0;
		m_fRotation = 0;
		tofs = 0;

		m_Tiles.resize(1);
		m_Tiles[0].tc[0]=Vec2D(1,0);
		m_Tiles[0].tc[1]=Vec2D(1,1);
		m_Tiles[0].tc[2]=Vec2D(0,1);
		m_Tiles[0].tc[3]=Vec2D(0,0);
	}
	void InitTile(Vec2D *tc, int nID)
	{
	Vec2D otc[4];
	Vec2D a,b;
	int x = nID % m_nCols;
	int y = nID / m_nCols;
	a.x = x * (1.0f / m_nCols);
	b.x = (x+1) * (1.0f / m_nCols);
	a.y = y * (1.0f / m_nRows);
	b.y = (y+1) * (1.0f / m_nRows);

	otc[0] = a;
	otc[1].x = b.x;otc[1].y = a.y;
	otc[2] = b;
	otc[3].x = a.x;otc[3].y = b.y;

	for (int i=0; i<4; i++)
	{// 有必要这样以来吗？m_nOrder？
		tc[(i+4-m_nOrder) & 3] = otc[i];
	}
	}
};