#pragma once
#include "RenderNode.h"
#include "Animated.h"

struct ModelCamera
{
	bool ok;

	Vec3D pos, target;
	float nearclip, farclip, fov;
	Animated<Vec3D> tPos, tTarget;
	Animated<float> rot;

	void setup(int time=0);

	ModelCamera():ok(false) {}
};

struct LightAnim
{
	int type, parent;
	Vec3D pos, tpos, dir, tdir;
	Animated<Vec3D> diffColor, ambColor;
	Animated<float> diffIntensity, ambIntensity;
	void setup(int time, int l);
};


//////////////////////////////////////////////////////////////////////////
struct ModelRenderPass
{
	ModelRenderPass()
		:nOrder(0)
		,p(0)
	{
	}
	int nOrder;
	float p;
	// Geoset ID
	int nSubID;
	std::string strMaterial;
	bool operator< (const ModelRenderPass &m) const
	{
		// sort order method
		if (nOrder!=m.nOrder)
			return nOrder<m.nOrder;
		else
			return p<m.p;
	}
};

class CHardwareVertexBuffer;
class CMeshData;
class CSkinMesh: public CRenderNode
{
public:
	CSkinMesh();
	~CSkinMesh();
	virtual int		getType			() {return NODE_MESH;}
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	virtual bool	intersectSelf	(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const;
	virtual bool	setup			();
	bool			prepare			()const;
	void			setLOD			(unsigned long uLodID);		// 设置LodID
	void			setSubSkin		(int nSubID, int nID);
	void			setLightMap		(const char* szFilename);	// SetLightMap
	virtual std::vector<ModelRenderPass>&	getRenderPasses(){return m_vecPasses;}
	virtual			GSET_CONST_VAR	(int,	m_n,Order);
	void			renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const;
protected:
	CHardwareVertexBuffer*			m_pVB;				// 顶点缓冲
	CMeshData*						m_pMesh;
	unsigned long					m_uLodLevel;		// Current Lod Level
	unsigned long					m_uLightMapTex;		//
	bool							m_bLightmap;
	std::vector<ModelRenderPass>	m_vecPasses;			// 渲染过程集
	int								m_nOrder;
};