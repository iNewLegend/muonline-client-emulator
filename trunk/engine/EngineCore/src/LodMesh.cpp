#include "LodMesh.h"
#include "RenderSystem.h"
#include "Intersect.h"

void CBoundMesh::draw()const
{
	if (indices.size())
	{
		CRenderSystem& R = GetRenderSystem();
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
CLodMesh::CLodMesh()
:m_pShareVB(NULL)
,m_pIB(NULL)
,m_pVertexDeclHardware(NULL)
,m_uVertexSize(0)
,m_uSkinVertexSize(0)
,m_uShareVertexSize(0)
,m_bSkinMesh(false)
,m_vb(NULL)
,m_vbSize(0)
{
}

CLodMesh::~CLodMesh()
{
	S_DEL(m_pShareVB);
	S_DEL(m_pIB);
	S_DEL(m_pVertexDeclHardware);
}

char* CLodMesh::createVB(size_t size)
{
	m_vb = new char[size];
	m_vbSize = size;
}

void CLodMesh::init()
{
	m_bSkinMesh = bone.size()>0;

	size_t uVertexCount=0;
	std::vector<std::vector<VertexIndex>> setVecVertexIndex;
	if (m_setSubMesh.size()!=0)
	{
		setVecVertexIndex.resize(m_setSubMesh.size());
		m_Lods.resize(1);
		IndexedSubset subset;
		std::vector<unsigned short> setIndex;
		for (size_t i=0;i<m_setSubMesh.size();++i)
		{
			CSubMesh& subMesh = m_setSubMesh[i];
			std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];

			transformRedundance(subMesh.m_setVertexIndex,setVertexIndex,setIndex);
			subset.vstart = 0;	// 因为每个sub都是独立的ib索引编号 从0开始，所以只需要设置vbase（vb地址偏移），有些显卡在误设置vstart（IB范围后）会不显示。
			subset.vbase += subset.vcount;
			subset.istart += subset.icount;
			subset.vcount = setVertexIndex.size();
			subset.icount = setIndex.size()-subset.istart;
			m_Lods[0].setSubset.push_back(subset);
			uVertexCount+=setVertexIndex.size();
		}
		transformRedundance(setIndex,m_Lods[0].IndexLookup,m_Lods[0].Indices);
	}

	if (m_bSkinMesh)
	{
		for (size_t i=0;i<setVecVertexIndex.size();++i)
		{
			std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];
			for (size_t n=0;n<setVertexIndex.size();++n)
			{
				VertexIndex& vertexIndex=setVertexIndex[n];
				SkinVertex skinVertex;
				skinVertex.p = pos[vertexIndex.p];
				skinVertex.n = normal[vertexIndex.n];
				skinVertex.w4 = weight[vertexIndex.w];
				skinVertex.b4 = bone[vertexIndex.b];
				m_setSkinVertex.push_back(skinVertex);
			}
		}
	}
	bool bPos		= pos.size()>0;
	bool bNormal	= normal.size()>0;
	bool bColor		= color.size()>0;
	bool bTexCoord	= texcoord.size()>0;
	bool bTexCoord2	= texcoord2.size()>0;

	m_pVertexDeclHardware = GetRenderSystem().CreateVertexDeclaration();

	unsigned short uOffset = 0;
	m_uSkinVertexSize = 0;
	unsigned short uStream = 0;
	// pos
	if (m_PosOffset)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT3, VES_POSITION);
		uOffset += sizeof(Vec3D);
	}
	// normal
	if (bNormal)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT3, VES_NORMAL);
		uOffset += sizeof(Vec3D);
	}
	//// 当为蒙皮时，只创建固定的公用的纹理UV缓存 // 创建刚体网格的公用的VB
	if (m_bSkinMesh)
	{
		m_uSkinVertexSize = uOffset;
		uOffset = 0; 
		uStream = 1;
	}
	// color
	if (bColor)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_COLOUR, VES_DIFFUSE);
		uOffset += sizeof(unsigned long);
	}
	// texCoord
	if (bTexCoord)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		uOffset += sizeof(Vec2D);
	}
	// texCoord2
	if (bTexCoord2)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
		uOffset += sizeof(Vec2D);
	}
	//
	m_pVertexDeclHardware->EndElement();
	m_uShareVertexSize = uOffset;

	// 	m_dwFVF |= bColor?D3DFVF_DIFFUSE:0;
	// 	m_dwFVF |= bTexCoord?D3DFVF_TEX1:0;

	// 当为蒙皮时，只创建固定的公用的纹理UV缓存
	// 创建刚体网格的公用的VB
	if (m_uShareVertexSize > 0)
	{
		m_pShareVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(uVertexCount, m_uShareVertexSize);
		if (m_pShareVB)
		{
			unsigned char* pBuffer = (unsigned char*)m_pShareVB->lock(CHardwareBuffer::HBL_NORMAL);
			if (pBuffer)
			{
				for (size_t i=0;i<setVecVertexIndex.size();++i)
				{
					CSubMesh& subMesh=m_setSubMesh[i];
					std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];
					for (size_t n=0;n<setVertexIndex.size();++n)
					{
						VertexIndex& vertexIndex=setVertexIndex[n];
							if (!m_bSkinMesh)
						{
							if (bPos)
							{
								*(Vec3D*)pBuffer = pos[vertexIndex.p];
								pBuffer += sizeof(Vec3D);
							}
							if (bNormal)
							{
								*(Vec3D*)pBuffer = normal[vertexIndex.n];
								pBuffer += sizeof(Vec3D);
							}
						}
						if (bColor)
						{
							*(Color32*)pBuffer = color[vertexIndex.c];
							pBuffer += sizeof(Color32);
						}
						if (bTexCoord)
						{
							*(Vec2D*)pBuffer = texcoord[vertexIndex.uv1];
							pBuffer += sizeof(Vec2D);
						}
						if (bTexCoord2)
						{
							*(Vec2D*)pBuffer = texcoord2[vertexIndex.uv2];
							pBuffer += sizeof(Vec2D);
						}
					}
				}
			}
			m_pShareVB->unlock();
		}
	}

	// 填入IB
	for(auto it = m_Lods.begin(); it!=m_Lods.end(); it++)
	{
		size_t indexSize = it->Indices.size();
		if (indexSize>0)
		{
			it->pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(indexSize);
			if (it->pIB)
			{
				unsigned short* indices = (unsigned short*)it->pIB->lock(CHardwareBuffer::HBL_NORMAL);
				if (indices)
				{
					for (size_t i=0;i<indexSize;++i)
					{
						indices[i] = it->IndexLookup[ it->Indices[i] ];
					}
				}
				it->pIB->unlock();
			}
		}
	}
}

bool CLodMesh::SetMeshSource(int nLodLevel, CHardwareVertexBuffer* pSkinVB)const
{
	CRenderSystem& R = GetRenderSystem();
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
	if (m_Lods[ nLodLevel ].pIB)
	{
		R.SetIndices(m_Lods[ nLodLevel ].pIB);	
	}
	else
	{
		return false;
	}
	return true;
}

void CLodMesh::drawSub(size_t uSubID, size_t uLodLevel)const
{
	if (m_Lods.size()<=uLodLevel)
	{
		return;
	}
	if (m_Lods[uLodLevel].setSubset.size()<=uSubID)
	{
		return;
	}
	const IndexedSubset &subset = m_Lods[uLodLevel].setSubset[uSubID];
	GetRenderSystem().drawIndexedSubset(subset);
}

void CLodMesh::draw(size_t uLodLevel)const
{
	if (m_Lods.size()>uLodLevel)
	{
		CRenderSystem& R = GetRenderSystem();
		const std::vector<IndexedSubset>& setSubset = m_Lods[ uLodLevel ].setSubset;
		for (auto it=setSubset.begin(); it!=setSubset.end(); it++)
		{
			R.drawIndexedSubset(*it);
		}
	}
}

void CLodMesh::skinningMesh(CHardwareVertexBuffer* pVB, std::vector<Matrix>& setBonesMatrix)const
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

void CLodMesh::Clear()
{
	m_setSubMesh.clear();
}

bool CLodMesh::intersect(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const
{
	for (size_t n=0;n<m_setSubMesh.size();++n)
	{
		const CSubMesh& subMesh = m_setSubMesh[n];
		size_t size=subMesh.m_setVertexIndex.size()/3;

		for(size_t i=0;i<size;++i)
		{
			if (IntersectTri(pos[subMesh.m_setVertexIndex[i*3].p],pos[subMesh.m_setVertexIndex[i*3+1].p],pos[subMesh.m_setVertexIndex[i*3+2].p],vRayPos,vRayDir,vOut))
			{
				nSubID = n;
				return true;
			}
		}
	}
	return false;
}

bool CLodMesh::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const
{
	Vec3D vOut;
	int nSubID;
	return intersect(vRayPos, vRayDir, vOut, nSubID);
}