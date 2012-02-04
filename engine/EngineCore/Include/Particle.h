#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Material.h"
#include "RenderNode.h"

struct Particle
{
	Vec3D vPos, vSpeed, vDown, vOrigin, vDir;
	Matrix mFace;
	float fSize, fLife, fMaxLife;
	int nTile;
	Color32 color;
};

class CParticleEmitter:public CRenderNode
{
public:
	CParticleEmitter()
		:m_pData(NULL)
		,m_nTime(0)
		,m_fRem(0)
	{
	}
	virtual int		getType		() {return NODE_PARTICLE;}
	virtual bool	init		(void* pData);
	virtual void	frameMove	(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render		(const Matrix& mWorld, int nRenderType)const;
	void			update		(const Matrix& mWorld, ParticleData& particleData, float fElapsedTime);
public:
	CMaterial			m_Material;
	ParticleData*		m_pData;		// ���ӷ�����ָ��
	std::list<Particle>	m_Particles;	// ���Ӷ�
	int					m_nTime;		// ʱ��
	float				m_fRem;			// �������ٵ�λ���� ������
};
