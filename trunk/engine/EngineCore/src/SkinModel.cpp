#include "SkinModel.h"
#include "RenderSystem.h"
#include "SkeletonNode.h"
#include "LodMesh.h"

CSkinModel::CSkinModel()
	:m_pVB(NULL)
	,m_pMesh(NULL)
	,m_uLightMapTex(0)
	,m_bLightmap(false)
	,m_uLodLevel(0)
{
}

CSkinModel::~CSkinModel()
{
	S_DEL(m_pVB);
}

void CSkinModel::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	if(!m_pMesh)
	{
		return;
	}
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_SKELETON)
	{
		CSkeletonNode* pSkeletonNode = (CSkeletonNode*)m_pParent;
		// ----
		if (m_pMesh->m_bSkinMesh)
		{
			m_pMesh->skinningMesh(m_pVB, pSkeletonNode->getBonesMatrix());
		}
	}
	// ----
	CRenderNode::frameMove(mWorld,fTime,fElapsedTime);
	// ----
	m_LocalBBox += m_pMesh->getBBox();
	updateWorldBBox();
}

void CSkinModel::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if(!m_pMesh)
	{
		return;
	}
	if (eRenderType==MATERIAL_NONE)
	{
		return;
	}
	// ----
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	GetRenderSystem().setWorldMatrix(mNewWorld);
	// ----
	renderMesh(eRenderType,m_uLodLevel,m_pVB,1.0f/*m_fTrans*/,0/*m_nAnimTime*/);
	// ----
	CRenderNode::render(mNewWorld, eRenderType);
}

bool CSkinModel::intersectSelf(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const
{
	if (!m_pMesh)
	{
		return false;
	}
	if (m_pMesh->intersect(vRayPos , vRayDir))
	{
		return true;
	}
	return false;
}

bool CSkinModel::init(void* pData)
{
	m_pMesh =(CLodMesh*) pData;
	// ----
	if(!m_pMesh)
	{
		return false;
	}
	// ----
	// # 设置默认LOD
	// ----
	SetLOD(0);
	// ----
	// # 如果是几何体动画 则进行重建VB
	// ----
	if (m_pMesh->m_bSkinMesh)
	{
		m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pMesh->getSkinVertexCount(), m_pMesh->getSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}

	if (m_vecPasses.empty())
	{
		if (m_pMesh->m_Lods.size()>0)
		{
			for (size_t i=0; i<m_pMesh->m_Lods[0].setSubset.size();++i)
			{
				ModelRenderPass pass;
				pass.nSubID = i;
				CSubMesh* pSubMesh = m_pMesh->getSubMesh(i);
				if(pSubMesh)
				{
					pass.strMaterial = pSubMesh->strMaterial;
				}
				m_vecPasses.push_back(pass);
			}
		}
	}
	m_nOrder=0;
	for (auto it=m_vecPasses.begin();it!=m_vecPasses.end();it++)
	{
		CMaterial& material = GetRenderSystem().getMaterialMgr().getItem(it->strMaterial.c_str());
		m_nOrder+=material.getOrder();
	}
	return true;
}

bool CSkinModel::Prepare()const
{
	if(!m_pMesh)
	{
		return false;
	}
	return m_pMesh->SetMeshSource(m_uLodLevel,m_pVB);
}

void CSkinModel::SetLOD(unsigned long uLodID)
{
	if (!m_pMesh)
	{
		return;
	}
	if (m_pMesh->m_Lods.size()>uLodID)
	{
		m_uLodLevel = uLodID;
	}
}

void CSkinModel::SetLightMap(const char* szFilename)
{
	m_uLightMapTex = GetRenderSystem().GetTextureMgr().RegisterTexture(szFilename);
	GetRenderSystem().GetTextureMgr().releaseBuffer(m_uLightMapTex);
	m_bLightmap = true;
}

void CSkinModel::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const
{
	if (m_pMesh->SetMeshSource(uLodLevel,pSkinVB))
	{
		if (eModelRenderType==(MATERIAL_ALL|MATERIAL_RENDER_NO_MATERIAL))
		{
			m_pMesh->draw();
		}
		else for (auto it = m_vecPasses.begin(); it != m_vecPasses.end(); ++it)
		{
			E_MATERIAL_RENDER_TYPE RenderType = GetRenderSystem().getMaterialMgr().getItem(it->strMaterial.c_str()).getRenderType();
			if (RenderType&eModelRenderType)
			{
				if (eModelRenderType&MATERIAL_RENDER_ALPHA_TEST)
				{
					GetRenderSystem().SetTexture(0,GetRenderSystem().getMaterialMgr().getItem(it->strMaterial.c_str()).uTexture[0]);
					m_pMesh->drawSub(it->nSubID,uLodLevel);
				}
				else if (eModelRenderType&MATERIAL_RENDER_NO_MATERIAL)
				{
					m_pMesh->drawSub(it->nSubID,uLodLevel);
				}
				else
				{
					if (GetRenderSystem().prepareMaterial(it->strMaterial.c_str(),fOpacity))
					{
						if (it->nSubID<0)
						{
							m_pMesh->draw(uLodLevel);
						}
						else
						{
							m_pMesh->drawSub(it->nSubID,uLodLevel);
						}
					}
					GetRenderSystem().finishMaterial();
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(0,1);
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(1,1);
				}
			}
		}
	}
}