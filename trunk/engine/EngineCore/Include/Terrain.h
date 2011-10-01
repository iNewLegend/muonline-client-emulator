#pragma once
#include "TerrainData.h"
#include "TerrainDecal.h"
#include "RenderSystem.h"
#include "RenderNode.h"
#include "SkinMesh.h"

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


// class CTerrain:public CRenderNode
//{
//public:
	// Ìù»¨
//	CTerrainDecal						m_LightMapDecal;
//	CTerrainDecal						m_LightDecal;
//};