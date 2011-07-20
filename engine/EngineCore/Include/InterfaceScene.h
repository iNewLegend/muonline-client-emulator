#pragma once
#include "InterfaceDataPlugsBase.h"
#include "Vec4D.h"
#include "Vec2D.h"
#include "Color.h"
#include "Pos2D.h"
#include "Material.h"
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
#include "Frustum.h"

struct Cube
{
	typedef std::vector<const Cube*>		LIST_CUBES;
	Cube()
	{
		pChildCube=NULL;
	}
	~Cube()
	{
		if (pChildCube)
		{
			delete[] pChildCube;
		}
	}
	Cube* pChildCube;
	BBox bbox;

	void createChildCube(size_t size)
	{
		int nWidth = int((bbox.vMax.x-bbox.vMin.x)/size);
		int nHeight = int((bbox.vMax.z-bbox.vMin.z)/size);
		if (1<nWidth||1<nHeight)
		{
			pChildCube = new Cube[2];
			pChildCube[0].bbox = bbox;
			pChildCube[1].bbox = bbox;
			if (nWidth>=nHeight)
			{
				float fX = bbox.vMin.x+(int(nWidth/2))*size;
				pChildCube[0].bbox.vMax.x = fX;
				pChildCube[1].bbox.vMin.x = fX;
			}
			else
			{
				float fZ = bbox.vMin.z+(int(nHeight/2))*size;
				pChildCube[0].bbox.vMax.z = fZ;
				pChildCube[1].bbox.vMin.z = fZ;
			}
			pChildCube[0].createChildCube(size);
			pChildCube[1].createChildCube(size);
		}
	}

	void getChildCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const
	{
		CrossRet crossRet = frustum.CheckAABBVisible(bbox);

		if (cross_include == crossRet)
		{
			CubeList.push_back(this);
		}
		else if (cross_cross == crossRet)
		{
			if (pChildCube)
			{
				for (int i=0; i<2; ++i)
				{
					pChildCube[i].getChildCubesByFrustum(frustum, CubeList);
				}
			}
			else
			{
				CubeList.push_back(this);
			}
		}
	}

	void getChildCrunodeCubes(LIST_CUBES& CubeList)const
	{
		if (pChildCube)
		{
			for (int i=0; i<2; ++i)
			{
				pChildCube[i].getChildCrunodeCubes(CubeList);
			}
		}
		else
		{
			CubeList.push_back(this);
		}
	}

	bool isPosIn(const Pos2D& posCell)
	{
		return posCell.x>=bbox.vMin.x&&
			posCell.x<bbox.vMax.x&&
			posCell.y>=bbox.vMin.y&&
			posCell.y<bbox.vMax.y;
	}

	Cube* getCrunodeCubeByPos(const Pos2D& posCell)
	{
		if (pChildCube==NULL)
		{
			return this;
		}
		for (int i = 0; i<2; i++)
		{
			if (pChildCube[i].isPosIn(posCell))
			{
				return getCrunodeCubeByPos(posCell);
			}
		}
		return NULL;
	}
};

class CTerrainDecal;

class iTerrainData
{
public:
	typedef std::map<int,std::string>		MAP_TILES;
	typedef std::vector<const Cube*>		LIST_CUBES;

	virtual void setTileMaterial(int nTileID, const std::string& strMaterialName)=0;
	virtual CMaterial& getMaterial(const char* szMaterialName)=0;

	virtual CTerrainDecal& GetLightMapDecal()=0;

	virtual void getCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const=0;
	virtual void getCrunodeCubes(LIST_CUBES& CubeList)const=0;

	virtual bool Prepare()=0;
	virtual void DrawChunk(const Cube& cube)=0;

	virtual void loadTilesMaterial(const char* szFilename, const char* szParentDir)=0;
	virtual void clearAllTiles()=0;

	virtual void setLightMapTexture(const std::string& strFilename)=0;

	virtual bool create()=0;
public:
	iTerrainData(){};
	virtual ~iTerrainData(){};
	//
	virtual void clear()=0;
	virtual void create(size_t width, size_t height, size_t cubeSize)=0;
	virtual bool resize(size_t width, size_t height, size_t cubeSize)=0;
	//
	virtual int	GetVertexXCount()const=0;
	virtual int	GetVertexYCount()const=0;
	virtual int	GetVertexCount()const=0;
	//
	virtual int GetWidth()const=0;
	virtual int GetHeight()const=0;
	virtual int GetCubeSize()const=0;
	virtual int GetCellCount()const=0;
	//
	virtual bool			isCellIn(int nCellX, int nCellY)const=0;
	virtual bool			isPointIn(int nCellX, int nCellY)const=0;
	//
	virtual unsigned char	GetCellTileID(int nCellX, int nCellY, size_t layer = 0)const=0;
	virtual void			SetCellTileID(int nCellX, int nCellY, unsigned char uTileID, size_t layer = 0)=0;
	//
	virtual unsigned long	getVertexIndex(int nCellX, int nCellY)const=0;
	virtual int				getCellXByVertexIndex(unsigned long uVertexIndex)const=0;
	virtual int				getCellYByVertexIndex(unsigned long uVertexIndex)const=0;
	virtual Pos2D			getCellPosByVertexIndex(unsigned long uVertexIndex)const=0;
	//
	virtual float			getVertexHeight(int nCellX, int nCellY)const=0;
	virtual void			setVertexHeight(int nCellX, int nCellY, float fHeight)=0;
	//
	virtual Vec3D			getVertexNormal(int nCellX, int nCellY)const=0;
	//
	virtual unsigned char	getCellAttribute(int nCellX, int nCellY)const=0;
	virtual void			setCellAttribute(int nCellX, int nCellY, unsigned char uAtt)=0;
	//
	virtual Color32			getVertexColor(int nCellX, int nCellY)const=0;
	virtual void			setVertexColor(int nCellX, int nCellY, Color32 color)=0;
	//
	virtual float			GetHeight(float fX, float fY)const=0;
	virtual Vec4D			GetColor(float fX, float fY)const=0;
	//
	virtual bool			PickCell(int nCellX, int nCellY, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const=0;
	virtual bool			Pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const=0;
	//
	virtual const			std::string& getFilename()const=0;
	//
	virtual void			getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const=0;
	virtual void			getGrassVertexByCell(int nCellX, int nCellY, TerrainVertex*& vertex)const=0;

	virtual bool			hasGrass(int nCellX, int nCellY)const=0;

	virtual std::vector<TerrainCell>& getCells()=0;
};

//////////////////////////////////////////////////////////////////////////
// Scene
//////////////////////////////////////////////////////////////////////////
#include "MapObj.h"
#include "RenderSystemCommon.h"
#include "RenderNode.h"
#include "InterfaceModel.h"

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