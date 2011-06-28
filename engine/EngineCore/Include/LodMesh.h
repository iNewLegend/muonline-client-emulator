#pragma once
#include "common.h"
#include "iMesh.h"
#include "Material.h"

class CHardwareVertexBuffer;
class CHardwareIndexBuffer;
class CVertexDeclaration;

struct SkinnedVertex
{
	Vec3D	p;
	Vec3D	n;
};

struct SkinVertex: public SkinnedVertex
{
	SkinVertex()
	{
		w4=0;
		b4=0;
	}
	union{
		unsigned char	w[4];
		unsigned long	w4;
	};
	union{
		unsigned char	b[4];
		unsigned long	b4;
	};
};

struct ModelLod
{
	ModelLod()
	{
		pIB = NULL;
		nLevel = 0;
	}
	CHardwareIndexBuffer*		pIB;			// 索引缓冲
	std::vector<unsigned short>	IndexLookup;	// 使用索引查找表可以优化骨骼动画（通过索引表去更新必要的顶点） Vertices in this model (index into vertices[])
	std::vector<unsigned short>	Indices;		// indices
	std::vector<IndexedSubset>	setSubset;		// 部件
	long						nLevel;			// LOD等级
};

class CBoundMesh
{
public:
	std::vector<Vec3D>				pos;		// 包围盒
	std::vector<unsigned short>		indices;	// 包围盒
	void draw()const;
};

class CLodMesh:public iLodMesh
{
public:
	CLodMesh();
	~CLodMesh();
public:
	GSET_CONST_VAR	(unsigned short,		m_u,SkinVertexSize);
	virtual int			getSubCount(){return m_setSubMesh.size();}
	virtual CSubMesh&	allotSubMesh();
	virtual CSubMesh*	getSubMesh(size_t n);
	virtual const BBox& getBBox(){return m_bbox;}
	virtual void		setBBox(const BBox& bbox){m_bbox = bbox;}
	virtual void		init();

	bool				SetMeshSource(int nLodLevel=0, CHardwareVertexBuffer* pSkinVB=NULL)const;
	void				drawSub(size_t uSubID, size_t uLodLevel=0)const;
	void				draw(size_t uLodLevel=0)const;
	void				skinningMesh(CHardwareVertexBuffer* pVB, std::vector<Matrix>& setBonesMatrix)const;
	void				Clear();
	size_t				getSkinVertexCount(){return m_setSkinVertex.size();}

	bool				intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const;
	bool				intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const;
public:
	std::vector<CSubMesh>			m_setSubMesh;

	CHardwareVertexBuffer*			m_pShareBuffer;			// Share Vertex Buffer
	CVertexDeclaration*				m_pVertexDeclHardware;	// FVF
	std::vector<ModelLod>			m_Lods;					// the lods

	unsigned short					m_uSkinVertexSize;
	unsigned short					m_uShareVertexSize;

	bool							m_bSkinMesh;			// is skin mesh?
	BBox							m_bbox;					//
protected:
	std::vector<SkinVertex>			m_setSkinVertex;
};