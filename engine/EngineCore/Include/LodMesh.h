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
	virtual const BBox& getBBox(){return m_bbox;}
	virtual void		setBBox(const BBox& bbox){m_bbox = bbox;}
	virtual void		init();

	virtual std::vector<SkinVertex>&				getSkinVertices(){return m_setSkinVertex;}
	virtual std::vector<RigidVertex>&				getRigidVertices(){return m_setRigidVertex;}
	virtual std::vector<RigidNolightVertex>&		getRigidNolightVertices(){return m_setRigidNolightVertex;}
	virtual std::vector<unsigned short>&			getIndices(){return m_Indices;}
	virtual std::vector<IndexedSubset>&				getSubsets(){return m_setSubset;}
	virtual std::vector<std::vector<std::string>>&	getMaterials(){return m_setMaterial;}
	
	bool				SetMeshSource(int nLodLevel=0, CHardwareVertexBuffer* pSkinVB=NULL)const;
	void				drawSub(size_t uSubID, size_t uLodLevel=0)const;
	void				draw(size_t uLodLevel=0)const;
	void				skinningMesh(CHardwareVertexBuffer* pVB, std::vector<Matrix>& setBonesMatrix)const;
	void				Clear();
	size_t				getSkinVertexCount(){return m_setSkinVertex.size();}

	bool				intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const;
	bool				intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const;
public:
	unsigned short				m_uSkinVertexSize;
	unsigned short				m_uShareVertexSize;

	bool						m_bSkinMesh;			// is skin mesh?
	BBox						m_bbox;					//
protected:
	CHardwareVertexBuffer*		m_pShareVB;				// Share Vertex Buffer
	CHardwareIndexBuffer*		m_pIB;					// Ë÷Òý»º³å
	CVertexDeclaration*			m_pVertexDeclHardware;	// FVF

	std::vector<SkinVertex>					m_setSkinVertex;
	std::vector<RigidVertex>				m_setRigidVertex;
	std::vector<RigidNolightVertex>			m_setRigidNolightVertex;
	 
	std::vector<unsigned short>				m_Indices;

	std::vector<IndexedSubset>				m_setSubset;				// Sub IndexedSubset
	std::vector<std::vector<std::string>>	m_setMaterial;	// Sub Material
};