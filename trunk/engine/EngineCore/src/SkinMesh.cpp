#include "SkinMesh.h"
#include "RenderSystem.h"
#include "SkeletonNode.h"
#include "LodMesh.h"
#include "RenderNodeMgr.h"

CSkinMesh::CSkinMesh()
	:m_pVB(NULL)
	,m_pMesh(NULL)
	,m_uLightMapTex(0)
	,m_bLightmap(false)
	,m_uLodLevel(0)
{
}

CSkinMesh::~CSkinMesh()
{
	S_DEL(m_pVB);
}

void CSkinMesh::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	setup();
	// ----
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

void CSkinMesh::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_pMesh)
	{
		if (eRenderType==MATERIAL_NONE)
		{
			return;
		}
		// ----
		//Matrix mNewWorld = mWorld*m_mWorldMatrix;
		// ----
		GetRenderSystem().setWorldMatrix(mNewWorld);
		// ----
		renderMesh(eRenderType,m_uLodLevel,m_pVB,1.0f/*m_fTrans*/,0/*m_nAnimTime*/);
	}
	// ----
	CRenderNode::render(mNewWorld, eRenderType);
}

bool CSkinMesh::intersectSelf(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const
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

bool CSkinMesh::setup()
{
	if (m_pMesh==m_pData)
	{
		if (m_pData==NULL)
		{
			CRenderNodeMgr::getInstance().PushMTLoading(this);
		}
		return false;
	}
	// ----
	m_pMesh = (CMeshData*)m_pData;
	// ----
	if(!m_pMesh)
	{
		return false;
	}
	m_pMesh->init();
	// ----
	// # 设置默认LOD
	// ----
	setLOD(0);
	// ----
	// # 如果是几何体动画 则进行重建VB
	// ----
	if (m_pMesh->m_bSkinMesh)
	{
		m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pMesh->getSkinVertexCount(), m_pMesh->getSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}

	if (m_vecPasses.empty())
	{
			for (size_t i=0; i<m_pMesh->getMaterials().size();++i)
			{
				ModelRenderPass pass;
				pass.nSubID = i;
				if(m_pMesh->getMaterials()[i].size()>0)
				{
					pass.strMaterial = m_pMesh->getMaterials()[i][0];
				}
				m_vecPasses.push_back(pass);
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

bool CSkinMesh::prepare()const
{
	if(!m_pMesh)
	{
		return false;
	}
	return m_pMesh->SetMeshSource(m_uLodLevel,m_pVB);
}

void CSkinMesh::setLOD(unsigned long uLodID)
{
	if (!m_pMesh)
	{
		return;
	}
	//if (m_pMesh->m_Lods.size()>uLodID)
	{
		m_uLodLevel = uLodID;
	}
}

void CSkinMesh::setSubSkin(int nSubID, int nID)
{
	if (!m_pMesh)
	{
		return;
	}
	// ----
	if(m_pMesh->getMaterials().size()<=nSubID)
	{
		return;
	}
	// ----
	if(m_pMesh->getMaterials()[nSubID].size()<=nID)
	{
		return;
	}
	// ----
	for (auto it=m_vecPasses.begin();it!=m_vecPasses.end();it++)
	{
		if (it->nSubID==nSubID)
		{
			it->strMaterial = m_pMesh->getMaterials()[nSubID][nID];
		}
	}
}

void CSkinMesh::setLightMap(const char* szFilename)
{
	m_uLightMapTex = GetRenderSystem().GetTextureMgr().RegisterTexture(szFilename);
	GetRenderSystem().GetTextureMgr().releaseBuffer(m_uLightMapTex);
	m_bLightmap = true;
}

void CSkinMesh::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const
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