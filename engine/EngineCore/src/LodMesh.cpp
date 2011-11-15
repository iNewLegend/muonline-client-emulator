#include "LodMesh.h"
#include "RenderSystem.h"
#include "Intersect.h"

void CBoundMesh::draw()const
{
	if (indices.size())
	{
		CRenderSystem& R = CRenderSystem::getSingleton();
		if (R.prepareMaterial("BoundMesh"))
		{
			R.SetFillMode(FILL_WIREFRAME);
			R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST,
				0, indices.size(), indices.size()/3,
				&indices[0], &pos[0], sizeof(Vec3D));
			R.SetFillMode(FILL_SOLID);
			R.finishMaterial();
		}
	}
}

template <class _T, class _T2>
void  transformRedundance(const std::vector<_T>& setIn, std::vector<_T>& setOut, std::vector<_T2>& index)
{
	std::map<_T,int> mapSort;
	size_t start = setOut.size();
	for (auto it=setIn.begin();it!=setIn.end();it++)
	{
		mapSort[*it];
	}
	int id = setOut.size();
	for (auto it=mapSort.begin();it!=mapSort.end();it++)
	{
		setOut.push_back(it->first);
		it->second=id;
		id++;
	}
	for (auto it=setIn.begin();it!=setIn.end();it++)
	{
		index.push_back(mapSort[*it]);
	}
}

//////////////////////////////////////////////////////////////////////////
CMeshData::CMeshData()
:m_pShareVB(NULL)
,m_pIB(NULL)
,m_pVertexDeclHardware(NULL)
,m_uSkinVertexSize(0)
,m_uShareVertexSize(0)
,m_bSkinMesh(false)
{
}

CMeshData::~CMeshData()
{
	S_DEL(m_pShareVB);
	S_DEL(m_pIB);
	S_DEL(m_pVertexDeclHardware);
}

void CMeshData::init()
{
	if (m_pIB!=NULL)
	{
		return;
	}
	m_bSkinMesh = m_setSkinVertex.size()>0;
	// ----
	// # Create Vertex Declaration.
	// ----
	m_pVertexDeclHardware = CRenderSystem::getSingleton().CreateVertexDeclaration();
	if (m_setSkinVertex.size()>0)
	{
		m_bSkinMesh = true;
		m_pVertexDeclHardware->AddElement(0, 0, VET_FLOAT3, VES_POSITION);
		m_pVertexDeclHardware->AddElement(0, 12, VET_FLOAT3, VES_NORMAL);
		m_pVertexDeclHardware->AddElement(1, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		m_uSkinVertexSize = 24;
		m_uShareVertexSize = sizeof(Vec2D);

		// 当为蒙皮时，只创建固定的公用的纹理UV缓存
		m_pShareVB = CRenderSystem::getSingleton().GetHardwareBufferMgr().CreateVertexBuffer(m_setSkinVertex.size(), sizeof(Vec2D));
		if (m_pShareVB)
		{
			Vec2D* pBuffer = (Vec2D*)m_pShareVB->lock(CHardwareBuffer::HBL_NORMAL);
			if (pBuffer)
			{
				for (size_t n=0;n<m_setSkinVertex.size();++n)
				{
					*pBuffer++ = m_setSkinVertex[n].uv;
				}
			}
			m_pShareVB->unlock();
		}
	}
	else if (m_setRigidVertex.size()>0)
	{
		m_pVertexDeclHardware->AddElement(0, 0, VET_FLOAT3, VES_POSITION);
		m_pVertexDeclHardware->AddElement(0, 12, VET_FLOAT3, VES_NORMAL);
		m_pVertexDeclHardware->AddElement(0, 24, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		m_uSkinVertexSize = 0;
		m_uShareVertexSize = sizeof(RigidVertex);

		m_pShareVB = CRenderSystem::getSingleton().GetHardwareBufferMgr().CreateVertexBuffer(m_setRigidVertex.size(), m_uShareVertexSize);
		if (m_pShareVB)
		{
			unsigned char* pBuffer = (unsigned char*)m_pShareVB->lock(CHardwareBuffer::HBL_NORMAL);
			if (pBuffer)
			{
				memcpy(pBuffer, &m_setRigidVertex[0], m_setRigidVertex.size()*sizeof(RigidVertex));
			}
			m_pShareVB->unlock();
		}
	}
	else if (m_setRigidNolightVertex.size()>0)
	{
		m_pVertexDeclHardware->AddElement(0, 0, VET_FLOAT3, VES_POSITION);
		m_pVertexDeclHardware->AddElement(0, 12, VET_COLOUR, VES_DIFFUSE);
		m_pVertexDeclHardware->AddElement(0, 16, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		m_uSkinVertexSize = 0;
		m_uShareVertexSize = sizeof(RigidNolightVertex);

		m_pShareVB = CRenderSystem::getSingleton().GetHardwareBufferMgr().CreateVertexBuffer(m_setRigidNolightVertex.size(), m_uShareVertexSize);
		if (m_pShareVB)
		{
			unsigned char* pBuffer = (unsigned char*)m_pShareVB->lock(CHardwareBuffer::HBL_NORMAL);
			if (pBuffer)
			{
				memcpy(pBuffer, &m_setRigidNolightVertex[0], m_setRigidNolightVertex.size()*m_uShareVertexSize);
			}
			m_pShareVB->unlock();
		}
	}
	m_pVertexDeclHardware->EndElement();


	// 填入IB
	if (m_Indices.size()>0)
	{
		m_pIB = CRenderSystem::getSingleton().GetHardwareBufferMgr().CreateIndexBuffer(m_Indices.size());
		if (m_pIB)
		{
			unsigned short* indices = (unsigned short*)m_pIB->lock(CHardwareBuffer::HBL_NORMAL);
			if (indices)
			{
				memcpy(indices, &m_Indices[0], m_Indices.size()*sizeof(unsigned short));
			}
			m_pIB->unlock();
		}
	}
}

bool CMeshData::SetMeshSource(int nLodLevel, CHardwareVertexBuffer* pSkinVB)const
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	if (m_bSkinMesh)
	{
		if (pSkinVB)
		{
			R.SetVertexDeclaration(m_pVertexDeclHardware);
			R.SetStreamSource(0, pSkinVB, 0, sizeof(SkinnedVertex));
			if (m_pShareVB)
			{
				R.SetStreamSource(1, m_pShareVB, 0, m_uShareVertexSize);
			}
			//GetRenderSystem().GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | 1ul);
			//GetRenderSystem().GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INDEXEDDATA | 1ul);
			//GetRenderSystem().GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (m_pShareVB)
		{
			R.SetVertexDeclaration(m_pVertexDeclHardware);
			R.SetStreamSource(0, m_pShareVB, 0, m_uShareVertexSize);
		}
		else
		{
			return false;
		}
	}
	if (m_pIB)
	{
		R.SetIndices(m_pIB);	
	}
	else
	{
		return false;
	}
	return true;
}

void CMeshData::drawSub(size_t uSubID, size_t uLodLevel)const
{
	if (m_setSubset.size()<=uSubID)
	{
		return;
	}
	CRenderSystem::getSingleton().drawIndexedSubset(m_setSubset[uSubID]);
}

void CMeshData::draw(size_t uLodLevel)const
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	for (auto it=m_setSubset.begin(); it!=m_setSubset.end(); it++)
	{
		R.drawIndexedSubset(*it);
	}
}

void CMeshData::skinningMesh(CHardwareVertexBuffer* pVB, std::vector<Matrix>& setBonesMatrix)const
{
	// 运算顶点
	if (pVB)
	{
		//unsigned long dwLockFlag = D3DLOCK_NOOVERWRITE;// 不等待 直接写入
		//if (1/*useEnvMapping*/)// 当一个Mesh被叠加多遍渲染的话 Lock 要等待GPU渲染完成或另开内存，否则会出现叠面
		//{
		//	dwLockFlag = 0;//D3DLOCK_DISCARD;// 另开一块内存写入
		//}
		unsigned short uCount	= m_setSkinVertex.size();
		//ModelVertex* pVIn		= &[0];
		const SkinVertex* pSkinVertex	= &m_setSkinVertex[0];


		SkinnedVertex* pVOut = (SkinnedVertex*)pVB->lock(CHardwareBuffer::HBL_NO_OVERWRITE);
		if (pVOut)
		{
			for (size_t i=0; i<uCount; ++i, pSkinVertex++, pVOut++)
			{
				Vec3D v(0,0,0);
				Vec3D n(0,0,0);
				for (unsigned char b=0; b<4; b++)
				{
					unsigned char bone = pSkinVertex->b[b];
					unsigned char weight = pSkinVertex->w[b];
					if (weight!=0)
					{
						float fWeight = weight / 255.0f;
						v += setBonesMatrix[bone]*pSkinVertex->p*fWeight;
						Matrix mRot=setBonesMatrix[bone];
						mRot._14=0;
						mRot._24=0;
						mRot._34=0;
						n += mRot*pSkinVertex->n*fWeight;
					}
				}
				pVOut->p = v;
				pVOut->n = n;
			}
		}
		pVB->unlock();
	}
}

void CMeshData::Clear()
{
}

bool CMeshData::intersect(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const
{
	size_t size=m_Indices.size()/3;

	if(m_setSkinVertex.size()>0)
	{
		for(size_t i=0;i<size;++i)
		{
			if (IntersectTri(m_setSkinVertex[m_Indices[i*3]].p,
				m_setSkinVertex[m_Indices[i*3+1]].p,
				m_setSkinVertex[m_Indices[i*3+2]].p,
				vRayPos,vRayDir,vOut))
			{
				return true;
			}
		}
	}
	else if(m_setRigidVertex.size()>0)
	{
		for(size_t i=0;i<size;++i)
		{
			if (IntersectTri(m_setRigidVertex[m_Indices[i*3]].p,
				m_setRigidVertex[m_Indices[i*3+1]].p,
				m_setRigidVertex[m_Indices[i*3+2]].p,
				vRayPos,vRayDir,vOut))
			{
				return true;
			}
		}
	}
	return false;
}

bool CMeshData::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const
{
	Vec3D vOut;
	int nSubID;
	return intersect(vRayPos, vRayDir, vOut, nSubID);
}