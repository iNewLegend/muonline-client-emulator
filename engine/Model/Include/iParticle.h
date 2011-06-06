#pragma once
#include "Frustum.h"

struct ParticleData
{
public:
	struct TexCoordSet
	{
		Vec2D tc[4];
	};
	//				���ٶȣ�		�仯��			��չ��		lat��	������		���ڣ�		�����ʣ�	����L��		����W��		���ٶ�
	Animated<float> m_Speed,	m_Variation,	m_Spread, m_Lat,	m_Gravity,	m_Lifespan,	m_Rate,	m_Areal,	m_Areaw,	m_Deacceleration;
	Animated<unsigned char> m_Enabled;
	Color32 m_Colors[3];	// ��ɫx3
	float m_Sizes[3];		// ��Сx3
	// �������е㣬���٣���ת
	float m_fLifeMid, m_fSlowdown, m_fRotation;
	Vec3D m_vPos;			// ����
	int m_nOrder, type;
	// ��������Tile��Ϣ
	int m_nRows, m_nCols;
	std::vector<TexCoordSet> m_Tiles;
	bool m_bBillboard;			// �����
	// unknown parameters omitted for now ...
	long flags;

	int	m_nBoneID;

	float tofs;

	std::string m_strMaterialName;
public:
	ParticleData(): m_nBoneID(0), m_fLifeMid(0)
	{
		//		m_nBlend = 0;
		m_nOrder = 0;
		type = 0;

		m_nRows = 0;
		m_nCols = 0;

		m_fSlowdown = 0;
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
	{// �б�Ҫ����������m_nOrder��
		tc[(i+4-m_nOrder) & 3] = otc[i];
	}
	}
};