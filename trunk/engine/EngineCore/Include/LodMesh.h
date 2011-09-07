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
	Vec3D uv;
};

struct RigidVertex
{
	Vec3D	p;
	Vec3D	n;
	Vec3D	uv;
};


class CBoundMesh
{
public:
	std::vector<Vec3D>				pos;		// °üÎ§ºÐ
	std::vector<unsigned short>		indices;	// °üÎ§ºÐ
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

	GSET_CONST_VAR		(unsigned short,		m_u,VertexSize);

	CONST_GSETVAR		(int,PosOffset);
	CONST_GSETVAR		(int,NormalOffset);
	CONST_GSETVAR		(int,ColorOffset);
	CONST_GSETVAR		(int,TexcoordOffset);
	CONST_GSETVAR		(int,Texcoord2Offset);

	virtual char*		createVB(size_t size);
	virtual char*		getVB(){return m_vb;}
	virtual size_t		getVBSize(){return m_ibSize;}

	virtual std::vector<unsigned short>&	getIndices(){return m_Indices;}
	virtual std::vector<IndexedSubset>&		getSubsets(){return m_setSubset;}
	virtual std::vector<std::vector<std::string>>&		getMaterials(){return m_setMaterial;}
	
	bool				SetMeshSource(int nLodLevel=0, CHardwareVertexBuffer* pSkinVB=NULL)const;
	void				drawSub(size_t uSubID, size_t uLodLevel=0)const;
	void				draw(size_t uLodLevel=0)const;
	void				skinningMesh(CHardwareVertexBuffer* pVB, std::vector<Matrix>& setBonesMatrix)const;
	void				Clear();
	size_t				getSkinVertexCount(){return m_setSkinVertex.size();}

	bool				intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const;
	bool				intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const;
public:
	std::vector<CSubMesh>		m_setSubMesh;

	unsigned short				m_uVertexSize;
	unsigned short				m_uSkinVertexSize;
	unsigned short				m_uShareVertexSize;

	bool						m_bSkinMesh;			// is skin mesh?
	BBox						m_bbox;					//
protected:
	CHardwareVertexBuffer*		m_pShareVB;				// Share Vertex Buffer
	CHardwareIndexBuffer*		m_pIB;					// Ë÷Òý»º³å
	CVertexDeclaration*			m_pVertexDeclHardware;	// FVF

	std::vector<SkinVertex>		m_setSkinVertex;
	std::vector<RigidVertex>		m_setRigidVertex;
	 
	std::vector<unsigned short>	m_Indices;

	std::vector<IndexedSubset>	m_setSubset;				// Sub IndexedSubset
	std::vector<std::vector<std::string>>	m_setMaterial;	// Sub Material

	char*						m_vb;
	size_t						m_vbSize;

	int							m_PosOffset;
	int							m_NormalOffset;
	int							m_ColorOffset;
	int							m_TexcoordOffset;
	int							m_Texcoord2Offset;
};