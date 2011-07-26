#pragma once
#include "TerrainData.h"
#include "TerrainDecal.h"
#include "RenderSystem.h"
#include "Octree.h"

#define TERRAIN_VERTEX_FVF	(FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX3)

struct EditValue
{
	union 
	{
		float fHeight;
		unsigned char uAtt;
		unsigned long color;
		int nTileID;
	};
};

struct EditTarget
{
	int type;
	int x;
	int y;
	void set(int _type, int _x, int _y)
	{
		type = _type;
		x = _x;
		y = _y;
	}
	bool operator< (const EditTarget &et) const
	{
		if(type!=et.type)
		{
			return type<et.type;
		}
		if(y!=et.y)
		{
			return y<et.y;
		}
		if(x!=et.x)
		{
			return x<et.x;
		}
		return false;
	}
};

struct EditRecord
{
	std::map<EditTarget,EditValue> mapItem;
};
typedef std::map<EditTarget,EditValue>  MAP_EDIT_RECORD;

struct TerrainSub:public IndexedSubset
{
	void myVertexIndex(unsigned short uIndex)
	{
		if (icount==0)
		{
			vstart=uIndex;
		}
		vstart=min(vstart,uIndex);
		vcount=max(vcount,uIndex);
		icount+=6;
	}
};

struct TerrainChunk
{
	BBox box;
};

class CTerrain:public CTerrainData
{
public:
	CTerrain(); 
	CTerrain(const std::string& strFilename); 
	~CTerrain();
	//
	virtual void			setTileMaterial(int nTileID, const std::string& strMaterialName);
	virtual CMaterial&		getMaterial(const char* szMaterialName);
	//
	virtual void			create(size_t width, size_t height,  size_t cubeSize);
	virtual bool			resize(size_t width, size_t height,  size_t cubeSize);
	//
	void					ShowBox(bool bShowBox){m_bShowBox = bShowBox;}
	bool					IsShowBox(){return m_bShowBox;}
	//void FormMove();
	// 渲染
	//virtual void			GetRenderObject(const CFrustum& frustum, LIST_OBJECTPASS& ObjectList);

	virtual void			UpdateRender(const CFrustum& frustum);

	virtual bool			LightMapPrepare();
	virtual void			LightMapFinish();
	virtual bool			Prepare();
	virtual void			DrawCubeBoxes(Color32 color=0xFF00FF00);
	virtual void			drawLayer0();
	virtual void			drawLayer1();

	virtual void			renderGrass();

	virtual void			Render();
	virtual void			draw();
	void					brushLightColor(MAP_EDIT_RECORD& mapEditRecord, float fPosX, float fPosY, Color32 colorPaint, float fRadius, float fHardness, float fStrength);
	void					drawLightDecal(float x, float y, float fSize, Color32 color);
	virtual void			DrawChunk(int x, int y);
	//
	MAP_TILES&				GetTiles(){return m_Tiles;}
	CHardwareVertexBuffer*	GetVB(){return m_pVB;}
	CTerrainDecal&			GetLightMapDecal(){return m_LightMapDecal;}
	std::string				getTileListFilename(){return m_strTileListFilename;}
	void					loadTilesMaterial(const char* szFilename, const char* szParentDir);
	void					clearAllTiles();
	void					setLightMapTexture(const std::string& strFilename);
	virtual bool			create();// 初始化
protected:
	//
	virtual void			updateVB(int nBeginX, int nBeginY, int nEndX, int nEndY);
	virtual void			CreateVB();
	virtual void			CreateIB();
	virtual void			CreateGrassVB(size_t uGrassCount);
	virtual void			CreateGrassIB(size_t uGrassCount);
	//
	virtual void			updateIB();
	// Calculate
	//void					CalcChunkIB(Cube& cube);
	//void					UpdateCubeBBox(Cube& cube);
protected:
	CHardwareVertexBuffer*	m_pVB;
	CHardwareIndexBuffer*	m_pIB;
	// grass
	CHardwareVertexBuffer*	m_pGrassVB;
	CHardwareIndexBuffer*	m_pGrassIB;

	Octree<TerrainChunk*>	m_OctreeRoot;
	bool					m_bShowBox;
	//
	// 贴花
	CTerrainDecal			m_LightMapDecal;
	CTerrainDecal			m_LightDecal;
	// 纹理Tile
	MAP_TILES				m_Tiles;
	int						m_nLightMap;
	unsigned long			m_uShowTileIBCount;
	// For Render
	std::map<unsigned char,TerrainSub>	m_RenderTileSubsLayer[2];
	TerrainSub				m_GrassSub;
	//
	std::set<BBox>			m_RenderCubesList;

	std::string				m_strTileListFilename;
};