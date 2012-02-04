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

class CHardwareVertexBuffer;
class CMeshData;
class CSkinMesh: public CRenderNode
{
public:
	CSkinMesh();
	~CSkinMesh();
	virtual int		getType			() {return NODE_MESH;}
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, int nRenderType)const;
	virtual bool	intersectSelf	(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const;
	virtual bool	setup			();
	bool			prepare			()const;
	void			setLOD			(unsigned long uLodID);		// 设置LodID
	void			setSkin			(int nID);
	virtual std::vector<CMaterial>&	getRenderPasses(){return m_vecMaterial;}
	virtual			GSET_CONST_VAR	(int,	m_n,Order);
	void			renderMesh(int eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB)const;
protected:
	CHardwareVertexBuffer*			m_pVB;				// 顶点缓冲
	CMeshData*						m_pMesh;
	unsigned long					m_uLodLevel;		// Current Lod Level
	std::vector<CMaterial>			m_vecMaterial;			// 渲染过程集
	int								m_nRenderType;
	int								m_nOrder;
	int								m_nSkinID;
};