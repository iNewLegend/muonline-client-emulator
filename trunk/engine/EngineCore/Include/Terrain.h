#pragma once
#include "TerrainData.h"
#include "TerrainDecal.h"
#include "RenderSystem.h"
#include "RenderNode.h"

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

class CTerrain:public CRenderNode
{
public:
	CTerrain(); 
	CTerrain(const std::string& strFilename); 
	~CTerrain();
	//
	virtual int					getType			() {return NODE_MODEL;}
	virtual void				frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime){;}
	virtual void				render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;

	CTerrainData*				getTerrainData	(){return m_pTerrainData;}
	virtual bool				init			(void* pData);
	virtual void				setTileMaterial	(int nTileID, const std::string& strMaterialName);
	virtual CMaterial&			getMaterial		(const char* szMaterialName);
	//
	GSET_VAR					(bool,	m_b,ShowBox);
	//
	virtual void				updateRender	(const CFrustum& frustum);

	virtual bool				LightMapPrepare	();
	virtual void				LightMapFinish	();
	virtual bool				prepare			()const;
	virtual void				drawCubeBoxes	(Color32 color=0xFF00FF00)const;
	virtual void				drawLayer0		()const;
	virtual void				drawLayer1		()const;
	virtual void				renderGrass		()const;

	virtual void				draw();
	void						brushLightColor	(MAP_EDIT_RECORD& mapEditRecord, float fPosX, float fPosY, Color32 colorPaint, float fRadius, float fHardness, float fStrength);
	void						drawLightDecal	(float x, float y, float fSize, Color32 color);
	virtual void				drawChunk		(TerrainChunk* pChunk);
	//
	std::map<int,std::string>&	getTiles		(){return m_Tiles;}
	CTerrainDecal&				GetLightMapDecal(){return m_LightMapDecal;}
	void						loadTilesMaterial(const char* szFilename, const char* szParentDir);
	void						clearAllTiles();
	void						setLightMapTexture(const std::string& strFilename);
protected:
	//
	virtual void				updateVB(int nBeginX, int nBeginY, int nEndX, int nEndY);
	virtual void				CreateGrassVB(size_t uGrassCount);
	virtual void				CreateGrassIB(size_t uGrassCount);
	//
	virtual void				updateIB();
protected:
	CTerrainData*						m_pTerrainData;
	CHardwareVertexBuffer*				m_pVB;
	CHardwareIndexBuffer*				m_pIB;
	// grass
	CHardwareVertexBuffer*				m_pGrassVB;
	CHardwareIndexBuffer*				m_pGrassIB;
	// Ã˘ª®
	CTerrainDecal						m_LightMapDecal;
	CTerrainDecal						m_LightDecal;
	// Œ∆¿ÌTile
	std::map<int,std::string>			m_Tiles;
	int									m_nLightMap;
	unsigned long						m_uShowTileIBCount;
	// For Render
	std::map<unsigned char,TerrainSub>	m_RenderTileSubsLayer[2];
	TerrainSub							m_GrassSub;
	//
	std::set<TerrainChunk*>				m_setRenderChunks;
	//
	bool								m_bShowBox;
};