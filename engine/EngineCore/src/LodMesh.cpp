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
:m_pShareBuffer(NULL)
,m_pVertexDeclHardware(NULL)
,m_uSkinVertexSize(0)
,m_uShareVertexSize(0)
,m_bSkinMesh(false)
{
}

CLodMesh::~CLodMesh()
{
	S_DEL(m_pShareBuffer);
	S_DEL(m_pVertexDeclHardware);
}

CSubMesh& CLodMesh::allotSubMesh()
{
	size_t size=m_setSubMesh.size();
	m_setSubMesh.resize(size+1);
	return m_setSubMesh[size];
}

CSubMesh* CLodMesh::getSubMesh(size_t n)
{
	if (m_setSubMesh.size()<=n)
	{
		return NULL;
	}
	return &m_setSubMesh[n];
}

void CLodMesh::init()
{
	for(size_t i=0;i<m_setSubMesh.size();++i)
	{
		if (false==m_bSkinMesh)
		{
			if(m_setSubMesh[i].bone.size()>0)
			{
				m_bSkinMesh = true;
				break;
			}
		}
	}
	if (!m_bSkinMesh)
	{
		//weight.clear();
		//bone.clear();
	}
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
			subset.vstart = 0;	// ��Ϊÿ��sub���Ƕ�����ib������� ��0��ʼ������ֻ��Ҫ����vbase��vb��ַƫ�ƣ�����Щ�Կ���������vstart��IB��Χ�󣩻᲻��ʾ��
			subset.vbase += subset.vcount;
			subset.istart += subset.icount;
			subset.vcount = setVertexIndex.size();
			subset.icount = setIndex.size()-subset.istart;
			m_Lods[0].setSubset.push_back(subset);
			uVertexCount+=setVertexIndex.size();
		}
		transformRedundance(setIndex,m_Lods[0].IndexLookup,m_Lods[0].Indices);
	}
	else
	{
		/*for (size_t i=0; i<pos.size(); ++i)
		{
			VertexIndex vertexIndex;
			if (i<pos.size())
			{
				vertexIndex.p=i;
			}
			if (i<normal.size())
			{
				vertexIndex.n=i;
			}
			if (i<color.size())
			{
				vertexIndex.c=i;
			}
			if (i<texcoord.size())
			{
				vertexIndex.uv1=i;
			}
			if (i<texcoord2.size())
			{
				vertexIndex.uv2=i;
			}
			if (i<weight.size())
			{
				vertexIndex.w=i;
			}
			if (i<bone.size())
			{
				vertexIndex.b=i;
			}
			setVertexIndex.push_back(vertexIndex);
		}*/
	}
	if (m_bSkinMesh)
	{
		for (size_t i=0;i<setVecVertexIndex.size();++i)
		{
			CSubMesh& subMesh=m_setSubMesh[i];
			std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];
			for (size_t n=0;n<setVertexIndex.size();++n)
			{
				VertexIndex& vertexIndex=setVertexIndex[n];
				SkinVertex skinVertex;
				skinVertex.p = subMesh.pos[vertexIndex.p];
				skinVertex.n = subMesh.normal[vertexIndex.n];
				skinVertex.w4 = subMesh.weight[vertexIndex.w];
				skinVertex.b4 = subMesh.bone[vertexIndex.b];
				m_setSkinVertex.push_back(skinVertex);
			}
		}
	}
	bool bPos		= false;
	bool bNormal	= false;
	bool bColor		= false;
	bool bTexCoord	= false;
	bool bTexCoord2	= false;
	for (size_t i=0;i<m_setSubMesh.size();++i)
	{
		CSubMesh& subMesh=m_setSubMesh[i];
		if (subMesh.pos.size()>0)
		{
			bPos=true;
		}
		if (subMesh.normal.size()>0)
		{
			bNormal=true;
		}
		if (subMesh.color.size()>0)
		{
			bColor=true;
		}
		if (subMesh.texcoord.size()>0)
		{
			bTexCoord=true;
		}
		if (subMesh.texcoord2.size()>0)
		{
			bTexCoord2=true;
		}
	}

	m_pVertexDeclHardware = GetRenderSystem().CreateVertexDeclaration();

	unsigned long dwFVF = 0;
	unsigned short uOffset = 0;
	m_uSkinVertexSize = 0;
	unsigned short uStream = 0;
	// pos
	if (bPos)
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
	//// ��Ϊ��Ƥʱ��ֻ�����̶��Ĺ��õ�����UV���� // ������������Ĺ��õ�VB
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

	// ��Ϊ��Ƥʱ��ֻ�����̶��Ĺ��õ�����UV����
	// ������������Ĺ��õ�VB
	if (m_uShareVertexSize > 0)
	{
		m_pShareBuffer = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(uVertexCount, m_uShareVertexSize);
		if (m_pShareBuffer)
		{
			unsigned char* pBuffer = (unsigned char*)m_pShareBuffer->lock(CHardwareBuffer::HBL_NORMAL);
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
								*(Vec3D*)pBuffer = subMesh.pos[vertexIndex.p];
								pBuffer += sizeof(Vec3D);
							}
							if (bNormal)
							{
								*(Vec3D*)pBuffer = subMesh.normal[vertexIndex.n];
								pBuffer += sizeof(Vec3D);
							}
						}
						if (bColor)
						{
							*(Color32*)pBuffer = subMesh.color[vertexIndex.c];
							pBuffer += sizeof(Color32);
						}
						if (bTexCoord)
						{
							*(Vec2D*)pBuffer = subMesh.texcoord[vertexIndex.uv1];
							pBuffer += sizeof(Vec2D);
						}
						if (bTexCoord2)
						{
							*(Vec2D*)pBuffer = subMesh.texcoord2[vertexIndex.uv2];
							pBuffer += sizeof(Vec2D);
						}
					}
				}
			}
			m_pShareBuffer->unlock();
		}
	}

	// ����IB
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
			if (m_pShareBuffer)
			{
				R.SetStreamSource(1, m_pShareBuffer, 0, m_uShareVertexSize);
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
		if (m_pShareBuffer)
		{
			R.SetVertexDeclaration(m_pVertexDeclHardware);
			R.SetStreamSource(0, m_pShareBuffer, 0, m_uShareVertexSize);
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
	// ���㶥��
	if (pVB)
	{
		//unsigned long dwLockFlag = D3DLOCK_NOOVERWRITE;// ���ȴ� ֱ��д��
		//if (1/*useEnvMapping*/)// ��һ��Mesh�����Ӷ����Ⱦ�Ļ� Lock Ҫ�ȴ�GPU��Ⱦ��ɻ����ڴ棬�������ֵ���
		//{
		//	dwLockFlag = 0;//D3DLOCK_DISCARD;// ��һ���ڴ�д��
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

bool SubMeshIntersect(const CSubMesh& subMesh,const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut)
{
	size_t size=subMesh.m_setVertexIndex.size()/3;
	for(size_t i=0;i<size;++i)
	{
		if (IntersectTri(subMesh.pos[subMesh.m_setVertexIndex[i*3].p],subMesh.pos[subMesh.m_setVertexIndex[i*3+1].p],subMesh.pos[subMesh.m_setVertexIndex[i*3+2].p],vRayPos,vRayDir,vOut))
		{
			return true;
		}
	}
	return false;
}

bool CLodMesh::intersect(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const
{
	for (size_t i=0;i<m_setSubMesh.size();++i)
	{
		if (SubMeshIntersect(m_setSubMesh[i],vRayPos,vRayDir,vOut))
		{
			nSubID = i;
			return true;
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