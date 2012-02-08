#pragma once
#include "Frustum.h"

struct ParticleData
{
public:
	struct TexCoordSet
	{
		Vec2D tc[4];
	};
	float m_Rate;//������
	float m_Spread1, m_Spread2;// ��ɢ
	float m_Lifespan;	// ��������
	float m_fLifeMid;	// �����е�
	float m_fSpeed1;	// ���ٶ�
	float m_fSpeed2;	// ���ٶ�
	float m_Areal;		// ����L
	float m_Areaw;		// ����W
	float m_fGravity;	// ����
	float m_fDampen;	// ���ٶ�
	Color32 m_fColor1;	// ��ɫx3
	Color32 m_fColor2;	// ��ɫx3
	Color32 m_fColor3;	// ��ɫx3
	float m_fSize1;		// ��Сx3
	float m_fSize2;		// ��Сx3
	float m_fSize3;		// ��Сx3
	float m_fRotation;	// ��ת
	int m_nOrder, type;
	// ��������Tile��Ϣ
	int m_nRows, m_nCols;
	std::vector<TexCoordSet> m_Tiles;
	bool m_bBillboard;			// �����
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
	{// �б�Ҫ����������m_nOrder��
		tc[(i+4-m_nOrder) & 3] = otc[i];
	}
	}
};