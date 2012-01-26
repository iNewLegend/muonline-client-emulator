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
	,m_nSkinID(0)
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
		if (m_pMesh->m_bSkinMesh&&pSkeletonNode->getSkeletonData())
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
		CRenderSystem::getSingleton().setWorldMatrix(mNewWorld);
		// ----
		renderMesh(eRenderType,m_uLodLevel,m_pVB);
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
	if (m_pData==(void*)-1)
	{
		return false;
	}
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
		m_pVB = CRenderSystem::getSingleton().GetHardwareBufferMgr().CreateVertexBuffer(m_pMesh->getSkinVertexCount(), m_pMesh->getSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}

	if (m_vecPasses.empty())
	{
		for (size_t i=0; i<m_pMesh->getMaterials().size();++i)
		{
			ModelRenderPass pass;
			pass.nSubID = i;

			std::vector<std::string>& subMaterials = m_pMesh->getMaterials()[i];

			if(subMaterials.size()>0)
			{
				int nSkinID = subMaterials.size()>m_nSkinID?m_nSkinID:0;
				pass.strMaterial = subMaterials[nSkinID];
			}
			m_vecPasses.push_back(pass);
		}
	}
	m_nOrder=0;
	for (auto it=m_vecPasses.begin();it!=m_vecPasses.end();it++)
	{
		CMaterial& material = CRenderSystem::getSingleton().getMaterialMgr().getItem(it->strMaterial.c_str());
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

void CSkinMesh::setSkin(int nID)
{
	m_nSkinID = nID;
	if (!m_pMesh)
	{
		return;
	}
	// ----
	for (auto it=m_vecPasses.begin();it!=m_vecPasses.end();it++)
	{
		if(m_pMesh->getMaterials().size()>it->nSubID)
		{
			std::vector<std::string>& subMaterials = m_pMesh->getMaterials()[it->nSubID];
			if (subMaterials.size()>m_nSkinID)
			{
				it->strMaterial = subMaterials[m_nSkinID];
			}
		}
	}
}

void CSkinMesh::setLightMap(const char* szFilename)
{
	m_uLightMapTex = CRenderSystem::getSingleton().GetTextureMgr().RegisterTexture(szFilename);
	CRenderSystem::getSingleton().GetTextureMgr().releaseBuffer(m_uLightMapTex);
	m_bLightmap = true;
}

void CSkinMesh::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB)const
{
	if (m_pMesh->SetMeshSource(uLodLevel,pSkinVB))
	{
		if (eModelRenderType==(MATERIAL_ALL|MATERIAL_RENDER_NO_MATERIAL))
		{
			m_pMesh->draw();
		}
		else for (auto it = m_vecPasses.begin(); it != m_vecPasses.end(); ++it)
		{
			E_MATERIAL_RENDER_TYPE RenderType = CRenderSystem::getSingleton().getMaterialMgr().getItem(it->strMaterial.c_str()).getRenderType();
			if (RenderType&eModelRenderType)
			{
				if (eModelRenderType&MATERIAL_RENDER_ALPHA_TEST)
				{
					CRenderSystem::getSingleton().SetTexture(0,CRenderSystem::getSingleton().getMaterialMgr().getItem(it->strMaterial.c_str()).uTexture[0]);
					m_pMesh->drawSub(it->nSubID,uLodLevel);
				}
				else if (eModelRenderType&MATERIAL_RENDER_NO_MATERIAL)
				{
					m_pMesh->drawSub(it->nSubID,uLodLevel);
				}
				else
				{
					if (CRenderSystem::getSingleton().prepareMaterial(it->strMaterial.c_str()))
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
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(0,1);
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(1,1);
				}
			}
		}
	}
}