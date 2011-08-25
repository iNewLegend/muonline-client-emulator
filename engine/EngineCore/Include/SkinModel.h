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
struct ModelRenderPass // 这部分的结构需要赚到材质层去 使材质更强大
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
class CLodMesh;
class CSkinModel: public CRenderNode
{
public:
	CSkinModel();
	~CSkinModel();
	virtual int		getType			() {return NODE_MODEL;}
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	virtual bool	intersectSelf	(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const;
	virtual bool	init			(void* pData);
	bool			Prepare			()const;
	void			SetLOD			(unsigned long uLodID);		// 设置LodID
	void			setSubSkin		(int nSubID, int nID);
	void			SetLightMap		(const char* szFilename);	// SetLightMap

protected:
	CHardwareVertexBuffer*			m_pVB;				// 顶点缓冲
	CLodMesh*						m_pMesh;
	unsigned long					m_uLodLevel;		// Current Lod Level
	unsigned long					m_uLightMapTex;		//
	bool							m_bLightmap;

	public:
	virtual GSET_CONST_VAR	(int,				m_n,Order);
	virtual std::vector<ModelRenderPass>&	getRenderPasses(){return m_vecPasses;}
public:
	void				renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const;
private:
	std::vector<ModelRenderPass>			m_vecPasses;			// 渲染过程集
	int										m_nOrder;
};