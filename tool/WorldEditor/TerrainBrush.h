#pragma once
#include "TerrainDecal.h"
class CTerrainBrush: public CTerrainDecal
{
public:
	CTerrainBrush();
	virtual void SetPos(float fX, float fY);
	void UpdateTexTrans();
	virtual void Render();
public:
	void	SetSize(float fSize);
	float	GetSize()const{return m_fSize;}
	void	SetHardness(float fHardness){m_fHardness = fHardness;}
	float	GetHardness()const{return m_fHardness;}
	void	SetStrength(float fStrength){m_fStrength = fStrength;}
	float	GetStrength()const{return m_fStrength;}
	void	SetTileID(int nTileID);
	int		GetTileID(int nX = 0, int nY = 0)const;
	void	SetAtt(unsigned char uAtt);
	unsigned char	GetAtt()const;
	void	setHeightRang(float fMin, float fMax);
	float	getHeightMin()const;
	float	getHeightMax()const;
	Color32 getColor()const;
	void	setColor(Color32 color);

	enum BrushType
	{
		BRUSH_TYPE_TERRAIN_HEIGHT,
		BRUSH_TYPE_TERRAIN_ATT,
		BRUSH_TYPE_TERRAIN_TILE_LAYER1,
		BRUSH_TYPE_TERRAIN_TILE_LAYER2,
		BRUSH_TYPE_TERRAIN_COLOR,
		BRUSH_TYPE_SCENE_OBJECT,
		BRUSH_TYPE_MAX,
	};
	enum BrushHeightType
	{
		BHT_NORMAL,
		BHT_SMOOTH,
	};
	void	SetBrushType(BrushType eBrushType){m_eBrushType=eBrushType;}
	BrushType	GetBrushType(){return m_eBrushType;}
	void			setBrushHeightType(BrushHeightType eBrushHeightType){m_eBrushHeightType=eBrushHeightType;}
	BrushHeightType	getBrushHeightType(){return m_eBrushHeightType;}
protected:
	Matrix					m_mRandiusTexTrans;
	Matrix					m_mHardnessTexTrans;
	float					m_fX, m_fY;
	unsigned long					m_uBaseVertexIndex;
	float					m_fSize;
	float					m_fHardness;
	float					m_fStrength;
	BrushType				m_eBrushType;
	BrushHeightType			m_eBrushHeightType;
	int						m_nTileID;
	unsigned char					m_uAtt;
	float					m_fHeightMin;
	float					m_fHeightMax;
	Color32					m_color;
};