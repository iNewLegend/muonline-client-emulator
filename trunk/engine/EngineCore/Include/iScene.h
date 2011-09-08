#pragma once
#include "Vec4D.h"
#include "Vec2D.h"
#include "Color.h"
#include "Pos2D.h"
#include "Material.h"
#include "vector"

//////////////////////////////////////////////////////////////////////////
// TerrainData
//////////////////////////////////////////////////////////////////////////
struct TerrainVertex
{
	//enum { FVF = (FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX3) };
	Vec3D	p;
	Vec3D	n;
	Color32	c;
	Vec2D	t0;
	Vec2D	t1;
};

struct TerrainCell
{
	unsigned char	uTileID[2];
	Color32			color;
	Color32			lightColor;
	float			fHeight;
	Vec3D			vNormals;
	unsigned char	uAttribute;
};

//////////////////////////////////////////////////////////////////////////
// Terrain
//////////////////////////////////////////////////////////////////////////
class iTerrainData
{
public:
	iTerrainData(){};
	virtual ~iTerrainData(){};
	//
	virtual void			clear()=0;
	virtual void			create(size_t width, size_t height)=0;
	virtual bool			resize(size_t width, size_t height)=0;
	//
	virtual int				getWidth()const=0;
	virtual int				getHeight()const=0;
	virtual int				getVertexCount()const=0;
	//
	virtual bool			isCellIn(int x, int y)const=0;
	virtual bool			isPointIn(int x, int y)const=0;
	//
	virtual unsigned char	getCellTileID(int x, int y, size_t layer = 0)const=0;
	virtual void			setCellTileID(int x, int y, unsigned char uTileID, size_t layer = 0)=0;
	//
	virtual unsigned long	getVertexIndex(int nCellX, int nCellY)const=0;
	virtual int				getCellXByVertexIndex(unsigned long uVertexIndex)const=0;
	virtual int				getCellYByVertexIndex(unsigned long uVertexIndex)const=0;
	//
	virtual float			getVertexHeight(int x, int y)const=0;
	virtual void			setVertexHeight(int x, int y, float fHeight)=0;
	//
	virtual Vec3D			getVertexNormal(int x, int y)const=0;
	//
	virtual unsigned char	getCellAttribute(int x, int y)const=0;
	virtual void			setCellAttribute(int x, int y, unsigned char uAtt)=0;
	//
	virtual Color32			getVertexColor(int x, int y)const=0;
	virtual void			setVertexColor(int x, int y, Color32 color)=0;
	//
	virtual float			getHeight(float fX, float fY)const=0;
	virtual Vec4D			getColor(float fX, float fY)const=0;
	//
	virtual bool			pickCell(int nCellX, int nCellY, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const=0;
	virtual bool			pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const=0;
	//
	virtual const			std::string& getFilename()const=0;
	//
	virtual void			getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const=0;

	virtual bool			hasGrass(int nCellX, int nCellY)const=0;

	virtual std::vector<TerrainCell>& getCells()=0;
};

//////////////////////////////////////////////////////////////////////////
// Scene
//////////////////////////////////////////////////////////////////////////
#include "RenderSystemCommon.h"
#include "Frustum.h"

class iSceneData
{
public:
	virtual void					setBBox					(const BBox& box)=0;
	virtual void					setOctreeDepth			(const size_t size)=0;
	virtual void					setFog					(const Fog& fog)=0;
	virtual void					setLight				(const DirectionalLight& light)=0;
	virtual const BBox&				getBBox					()const=0;
	virtual const size_t			getOctreeDepth			()const=0;
	virtual const Fog&				getFog					()const=0;
	virtual const DirectionalLight&	getLight				()const=0;
};