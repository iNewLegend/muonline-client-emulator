#include "SkinMesh.h"
#include "RenderSystem.h"
#include "SkeletonNode.h"
#include "LodMesh.h"
#include "RenderNodeMgr.h"

CSkinMesh::CSkinMesh()
	:m_pVB(NULL)
	,m_pMesh(NULL)
	,m_uLodLevel(0)
	,m_nSkinID(0)
	,m_nRenderType(0)
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

void CSkinMesh::render(const Matrix& mWorld, int nRenderType)const
{
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_nRenderType&nRenderType && m_pMesh)
	{
		// ----
		//Matrix mNewWorld = mWorld*m_mWorldMatrix;
		// ----
		CRenderSystem::getSingleton().setWorldMatrix(mNewWorld);
		// ----
		renderMesh(nRenderType,m_uLodLevel,m_pVB);
	}
	// ----
	CRenderNode::render(mNewWorld, nRenderType);
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

	auto& R = CRenderSystem::getSingleton();

	// ----
	// # 如果是几何体动画 则进行重建VB
	// ----
	if (m_pMesh->m_bSkinMesh)
	{
		m_pVB = R.GetHardwareBufferMgr().CreateVertexBuffer(m_pMesh->getSkinVertexCount(), m_pMesh->getSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}

	if (m_vecMaterial.empty())
	{
		for (size_t i=0; i<m_pMesh->getMaterials().size();++i)
		{
			std::vector<CMaterial>& subMaterials = m_pMesh->getMaterials()[i];

			if(subMaterials.size()>0)
			{
				int nSkinID = subMaterials.size()>m_nSkinID?m_nSkinID:0;
				m_vecMaterial.push_back(subMaterials[nSkinID]);
			}
			else
			{
				CMaterial mat;
				m_vecMaterial.push_back(mat);
			}
		}
		m_nRenderType = 0;
		for (auto it=m_vecMaterial.begin(); it!=m_vecMaterial.end(); ++it)
		{
			// Texture
			for (size_t i=0;i<8;++i)
			{
				if (it->strTexture[i].length()>0)
				{
					it->uTexture[i]=R.GetTextureMgr().RegisterTexture(it->strTexture[i].c_str());
				}
				else
				{
					it->uTexture[i]=0;
				}
			}
			// Shader
			CShader* pShader = R.getShader(it->strShader.c_str());
			if (!pShader)
			{
				continue;
			}
			int renderType = pShader->getRenderType();
			m_nRenderType |= renderType;
		}
	}
	m_nOrder=0;
	for (auto it=m_vecMaterial.begin();it!=m_vecMaterial.end();it++)
	{
		//CMaterial& material = R.getMaterialMgr().getItem(it->strMaterial.c_str());
		//m_nOrder+=material.getOrder();
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
	for (int i=0; i< m_vecMaterial.size(); ++i)
	{
		std::vector<CMaterial>& subMaterials = m_pMesh->getMaterials()[i];
		if (subMaterials.size()>m_nSkinID)
		{                
			m_vecMaterial[i] = subMaterials[m_nSkinID];
		}
	}
}

void CSkinMesh::renderMesh(int eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB)const
{
	if (m_pMesh->SetMeshSource(uLodLevel,pSkinVB))
	{
		if (eModelRenderType==(MATERIAL_ALL|MATERIAL_RENDER_NO_MATERIAL))
		{
			m_pMesh->draw(uLodLevel);
		}
		else for (int i=0; i<m_vecMaterial.size(); ++i)
		{
			auto& mat = m_vecMaterial[i];
			auto& R = CRenderSystem::getSingleton();
			CShader* pShader = R.getShader(mat.strShader.c_str());
			if (!pShader)
			{
				continue;
			}
			int renderType = pShader->getRenderType();
			// No Render Type
			if ((renderType&eModelRenderType)==0)
			{
				continue;
			}
			//
			if (eModelRenderType&MATERIAL_RENDER_ALPHA_TEST)
			{
			}
			else if (eModelRenderType&MATERIAL_RENDER_NO_MATERIAL)
			{
				m_pMesh->drawSub(i,uLodLevel);
			}
			else
			{
				R.SetShader(pShader);
				for (size_t j=0;j<8;++j)
				{
					if (mat.uTexture[j]==0)
					{
						break;
					}
					// ----
					R.SetTexture(j, mat.uTexture[j]);
				}
				m_pMesh->drawSub(i,uLodLevel);
			}
		}
	}
}