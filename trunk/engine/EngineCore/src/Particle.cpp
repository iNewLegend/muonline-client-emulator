#include "Particle.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "Animated.h"
#include "SkeletonNode.h"
#define MAX_PARTICLES 10000

float	frand();
float	randfloat(float lower, float upper);
int		randint(int lower, int upper);

float frand()
{
	return rand()/(float)RAND_MAX;
}

float randfloat(float lower, float upper)
{
	return lower + (upper-lower)*(rand()/(float)RAND_MAX);
}

int randint(int lower, int upper)
{
	return lower + (int)((upper-lower)*frand());
}

// 基于“伸展”产生旋转的矩阵
Matrix SpreadMat;
void CalcSpreadMatrix(float Spread1,float Spread2, float w, float l)
{
	int i;
	float a[2],c[2],s[2];
	Matrix	Temp;

	SpreadMat=Matrix::UNIT;

	a[0]=randfloat(-Spread1,Spread1)/2.0f;
	a[1]=randfloat(-Spread2,Spread2)/2.0f;

	SpreadMat.m[0][0]*=l;
	SpreadMat.m[1][1]*=l;
	SpreadMat.m[2][2]*=w;

	for(i=0;i<2;i++)
	{		
		c[i]=cos(a[i]);
		s[i]=sin(a[i]);
	}
	Temp=Matrix::UNIT;
	Temp.m[1][1]=c[0];
	Temp.m[2][1]=s[0];
	Temp.m[2][2]=c[0];
	Temp.m[1][2]=-s[0];

	SpreadMat*=Temp;

	Temp=Matrix::UNIT;
	Temp.m[0][0]=c[1];
	Temp.m[1][0]=s[1];
	Temp.m[1][1]=c[1];
	Temp.m[0][1]=-s[1];

	SpreadMat*=Temp;
}

bool CParticleEmitter::init(void* pData)
{
	m_pData = (ParticleData*)pData;
	m_nBindingBoneID = m_pData->m_nBoneID;
	return true;
}

void CParticleEmitter::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_SKELETON)
	{
		CSkeletonNode* pSkeletonNode = (CSkeletonNode*)m_pParent;
		const Matrix& matBone = pSkeletonNode->getBonesMatrix()[m_pData->m_nBoneID];
		// ----
		mNewWorld = matBone*mNewWorld;
	}
	// ----
	update(mNewWorld,*m_pData,fElapsedTime);
	// ----
	CRenderNode::frameMove(mWorld,fTime,fElapsedTime);
}

void CParticleEmitter::update(const Matrix& mWorld, ParticleData& particleData, float fElapsedTime)
{
	// spawn new particles

	float fRate = particleData.m_Rate.getValue(m_nTime);
	float fToSpawn;
	{
		float fLife = 1.0f;
		//fLife = m_Lifespan.getValue(m_nTime);
		fToSpawn = (fElapsedTime * fRate / fLife) + m_fRem;
	}

	if (fToSpawn < 1.0f)// 时间不足于产生一个粒子
	{
		m_fRem = fToSpawn;
		if (m_fRem<0) 
			m_fRem = 0;
	}
	else
	{
		int nToSpawn = (int)fToSpawn;

		// 检测粒子是否超量
		if ((nToSpawn + m_Particles.size()) > MAX_PARTICLES)
		{
			nToSpawn = (int)m_Particles.size() - MAX_PARTICLES;
		}

		m_fRem = fToSpawn - (float)nToSpawn;

		bool bEnabled = particleData.m_Enabled.getValue(m_nTime)!=0;
		if (bEnabled)
		{
			float w		= particleData.m_Areal.getValue(m_nTime) * 0.5f;
			float l		= particleData.m_Areaw.getValue(m_nTime) * 0.5f;
			float spd	= particleData.m_Speed.getValue(m_nTime);
			float var	= particleData.m_Variation.getValue(m_nTime);
			float spr	= particleData.m_Spread.getValue(m_nTime);
			float spr2	= particleData.m_Lat.getValue(m_nTime);

			for (int i=0; i<nToSpawn; i++)
			{
				Particle p;

				//Spread Calculation
				Matrix mWorldRot = mWorld;
				mWorldRot._14=0;
				mWorldRot._24=0;
				mWorldRot._34=0;

				CalcSpreadMatrix(spr,spr,1.0f,1.0f);
				Matrix mRot = mWorldRot * SpreadMat;

				p.vPos = particleData.m_vPos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w));
				p.vPos = mWorld * p.vPos;

				Vec3D vDir = mRot * Vec3D(0,1,0);
				p.vDown = Vec3D(0,-1.0f,0);
				p.vSpeed = vDir.normalize() * spd * (1.0f+randfloat(-var,var));

				if(!particleData.m_bBillboard)
				{
					Vec3D look = p.vDir;
					look.x=p.vDir.y;
					look.y=p.vDir.z;
					look.z=p.vDir.x;

					Vec3D up = (look % p.vDir).normalize();
					Vec3D right = (up % p.vDir).normalize();
					up = (p.vDir % right).normalize();

					// calculate the billboard matrix
					p.mFace.m[0][1] = right.x;
					p.mFace.m[1][1] = right.y;
					p.mFace.m[2][1] = right.z;
					p.mFace.m[0][2] = up.x;
					p.mFace.m[1][2] = up.y;
					p.mFace.m[2][2] = up.z;
					p.mFace.m[0][0] = p.vDir.x;
					p.mFace.m[1][0] = p.vDir.y;
					p.mFace.m[2][0] = p.vDir.z;
				}

				p.fLife = 0;
				p.fMaxLife = particleData.m_Lifespan.getValue(m_nTime);

				p.vOrigin = p.vPos;

				p.nTile = randint(0, particleData.m_nRows*particleData.m_nCols-1);

				m_Particles.push_back(p);
			}
		}
	}
	float mspeed	= 1.0f;
	float fGrav		= particleData.m_Gravity.getValue(m_nTime);
	float fDeaccel	= particleData.m_Deacceleration.getValue(m_nTime);
	for (auto it = m_Particles.begin(); it != m_Particles.end();)
	{
		Particle &p = *it;
		// 计算出当前速度
		p.vSpeed += (p.vDown * fGrav  - p.vDir * fDeaccel) * fElapsedTime;

		if (particleData.m_fSlowdown>0)// ????????????????
		{
			mspeed = expf(-1.0f * particleData.m_fSlowdown * p.fLife);
		}
		p.vPos += p.vSpeed * mspeed * fElapsedTime;

		p.fLife += fElapsedTime;
		float rlife = p.fLife / p.fMaxLife;
		// 通过当前生命时间计算出当前粒子大小和颜色
		p.fSize = lifeRamp<float>(rlife, particleData.m_fLifeMid, particleData.m_Sizes[0], particleData.m_Sizes[1], particleData.m_Sizes[2]);
		p.color = lifeRamp<Color32>(rlife, particleData.m_fLifeMid, particleData.m_Colors[0], particleData.m_Colors[1], particleData.m_Colors[2]);

		// 杀死过期粒子
		if (rlife >= 1.0f) 
		{
			m_Particles.erase(it++);
		}
		else 
		{
			++it;
		}
	}
}

void CParticleEmitter::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if(!m_pData)
	{
		return;
	}
	CRenderSystem& R = CRenderSystem::getSingleton();
	// ----
	R.setWorldMatrix(Matrix::UNIT);
	// ----
	//CMaterial& material = R.getMaterialMgr().getItem(m_pData->m_strMaterialName.c_str());
	//if (!(m_Material.getRenderType()&eRenderType))
	{
	//	return;
	}
	// ----
	if (R.prepareMaterial(m_Material))
	{
		Vec3D bv0,bv1,bv2,bv3;
		{
			Matrix mbb=Matrix::UNIT;
			if (m_pData->m_bBillboard)
			{
				// 获取公告板矩阵
				Matrix mTrans;
				CRenderSystem::getSingleton().getViewMatrix(mTrans);
				mTrans.Invert();

				if (m_pData->flags == 569) // 圆柱形 Faith shoulders, do cylindrical billboarding
				{
					mbb._11 = 1;
					mbb._31 = 0;
					mbb._13 = 0;
					mbb._33 = 1;
				}
				else
				{
					mbb=mTrans;
					mbb._14=0;
					mbb._24=0;
					mbb._34=0;
				}
				mbb=mTrans;
				mbb._14=0;
				mbb._24=0;
				mbb._34=0;
			}

			if (m_pData->type==0 || m_pData->type==2)			// 正常的粒子
			{
				float f = 0.5;//0.707106781f; // sqrt(2)/2
				if (m_pData->m_bBillboard)
				{
					bv0 = mbb * Vec3D(+f,-f,0);
					bv1 = mbb * Vec3D(+f,+f,0);
					bv2 = mbb * Vec3D(-f,+f,0);
					bv3 = mbb * Vec3D(-f,-f,0);
				}
				else // 平板
				{
					bv0 = Vec3D(-f,0,+f);
					bv1 = Vec3D(+f,0,+f);
					bv2 = Vec3D(+f,0,-f);
					bv3 = Vec3D(-f,0,-f);
				}
				// TODO: per-particle rotation in a non-expensive way?? :|

				CGraphics& bg = GetGraphics();
				bg.begin(VROT_TRIANGLE_LIST, m_Particles.size()*4);
				for (auto it = m_Particles.begin(); it != m_Particles.end(); ++it)
				{
					bg.c(it->color);

					bg.t(m_pData->m_Tiles[it->nTile].tc[0]);
					bg.v(it->vPos + bv0 * it->fSize);

					bg.t(m_pData->m_Tiles[it->nTile].tc[1]);
					bg.v(it->vPos + bv1 * it->fSize);

					bg.t(m_pData->m_Tiles[it->nTile].tc[2]);
					bg.v(it->vPos + bv2 * it->fSize);

					bg.t(m_pData->m_Tiles[it->nTile].tc[3]);
					bg.v(it->vPos + bv3 * it->fSize);
				}
				bg.end();
			}
			else if (m_pData->type==1) // 粒子射线发射器 particles from origin to position
			{
				bv0 = mbb * Vec3D(-1.0f,0,0);
				bv1 = mbb * Vec3D(+1.0f,0,0);

				CGraphics& bg = GetGraphics();
				bg.begin(VROT_TRIANGLE_LIST, m_Particles.size()*4);
				for (auto it = m_Particles.begin(); it != m_Particles.end(); ++it)
				{
					Vec3D P,O;
					P=it->vPos;
					O=it->vOrigin;
					bg.c(it->color);

					bg.t(m_pData->m_Tiles[it->nTile].tc[0]);
					bg.v(it->vPos + bv0 * it->fSize);

					bg.t(m_pData->m_Tiles[it->nTile].tc[1]);
					bg.v(it->vPos + bv1 * it->fSize);

					bg.t(m_pData->m_Tiles[it->nTile].tc[2]);
					bg.v(it->vOrigin + bv1 * it->fSize);

					bg.t(m_pData->m_Tiles[it->nTile].tc[3]);
					bg.v(it->vOrigin + bv0 * it->fSize);
				}
				bg.end();
			}
		}
	}
}