#pragma once
#include "SceneData.h"
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

// void CTerrain::updateIB()
// {
// 	if (m_pTerrainData==NULL)
// 	{
// 		return;
// 	}
// 	m_RenderTileSubsLayer[0].clear();
// 	m_RenderTileSubsLayer[1].clear();
// 	// 写IB
// 	if (m_pIB)
// 	{
// 		for (auto it=m_setRenderChunks.begin(); it!=m_setRenderChunks.end(); ++it)
// 		{
// 			for (size_t y = (*it)->box.vMin.z; y<(*it)->box.vMax.z; ++y)
// 			{
// 				for (size_t x = (*it)->box.vMin.x; x<(*it)->box.vMax.x; ++x)
// 				{
// 					TerrainCell& cell = *m_pTerrainData->getCell(x,y);
// 					if ((cell.uAttribute&0x8)==0)// 透明
// 					{
// 						unsigned long uIndex = y*(m_pTerrainData->getWidth()+1)+x;
// 						const unsigned char uTileID0 = cell.uTileID[0];
// 						if (uTileID0!=0xFF)// 透明
// 						{
// 							m_RenderTileSubsLayer[0][uTileID0].myVertexIndex(uIndex);
// 						}
// 						const unsigned char uTileID1 = cell.uTileID[1];
// 						if (uTileID1!=0xFF)// 透明
// 						{
// 							m_RenderTileSubsLayer[1][uTileID1].myVertexIndex(uIndex);
// 						}
// 					}
// 				}
// 			}
// 		}
// 
// 		m_uShowTileIBCount = 0;
// 		for (int nLayer=0; nLayer<2; nLayer++)
// 		{
// 			for (auto it = m_RenderTileSubsLayer[nLayer].begin(); it!=m_RenderTileSubsLayer[nLayer].end(); it++)
// 			{
// 				it->second.istart = m_uShowTileIBCount;
// 				m_uShowTileIBCount += it->second.icount;
// 				it->second.icount = 0;
// 				it->second.vcount=it->second.vcount-it->second.vstart+1+(m_pTerrainData->getWidth()+1)+1;
// 			}
// 		}
// 
// 		unsigned long uTempVertexXCount = m_pTerrainData->getWidth()+1;
// 		unsigned short* index = (unsigned short*)m_pIB->lock(0, m_uShowTileIBCount*sizeof(unsigned short),CHardwareBuffer::HBL_NO_OVERWRITE);
// 		for (auto it=m_setRenderChunks.begin(); it!=m_setRenderChunks.end(); it++)
// 		{
// 			for (size_t y = (*it)->box.vMin.z; y<(*it)->box.vMax.z; ++y)
// 			{
// 				for (size_t x = (*it)->box.vMin.x; x<(*it)->box.vMax.x; ++x)
// 				{
// 					TerrainCell& cell = *m_pTerrainData->getCell(x,y);
// 					if ((cell.uAttribute&0x8)==0)// 透明
// 					{
// 						unsigned long uIndex = m_pTerrainData->getVertexIndex(x,y);
// 						for (int nLayer=0; nLayer<2; nLayer++)
// 						{
// 							const unsigned char uTileID = cell.uTileID[nLayer];
// 							TerrainSub& sub = m_RenderTileSubsLayer[nLayer][uTileID];
// 							if (uTileID!=0xFF)
// 							{
// 								// 2	 3
// 								//	*---*
// 								//	| / |
// 								//	*---*
// 								// 0	 1
// 								unsigned short* p = index+sub.istart+sub.icount;
// 								sub.icount+=6;
// 								*p = uIndex;
// 								p++;
// 								*p = uIndex+uTempVertexXCount;
// 								p++;
// 								*p = uIndex+uTempVertexXCount+1;
// 								p++;
// 
// 								*p = uIndex;
// 								p++;
// 								*p = uIndex+uTempVertexXCount+1;
// 								p++;
// 								*p = uIndex+1;
// 								p++;
// 							}
// 						}
// 					}
// 				}
// 			}
// 		}
// 		m_pIB->unlock();
// 	}
// }

// void CTerrain::updateVB(int nBeginX, int nBeginY, int nEndX, int nEndY)
// {
// 	if (m_pVB==NULL || m_pTerrainData==NULL)
// 	{
// 		return;
// 	}
// 	unsigned long uOffset	= sizeof(TerrainVertex)*m_pTerrainData->getVertexIndex(nBeginX,nBeginY);
// 	unsigned long uSize	= sizeof(TerrainVertex)*(m_pTerrainData->getVertexIndex(nEndX,nEndY)+1)-uOffset;
// 	if (uSize>0)
// 	{
// 		TerrainVertex*	pV = (TerrainVertex*)m_pVB->lock(uOffset, uSize, CHardwareBuffer::HBL_NO_OVERWRITE/*CHardwareBuffer::HBL_NORMAL*/);
// 		for (int y = nBeginY; y <= nEndY; ++y)
// 		{
// 			for (int x = nBeginX; x <= nEndX; ++x)
// 			{
// 				m_pTerrainData->getVertexByCell(x,y,*pV);
// 				pV++;
// 			}
// 		}
// 		m_pVB->unlock();
// 	}
// }

//bool CTerrain::init(void* pData)
//{

// 	S_DEL(m_pVB);
// 	m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pTerrainData->getVertexCount(),sizeof(TerrainVertex));
// 	updateVB(0,0,m_pTerrainData->getWidth(),m_pTerrainData->getHeight());/*CHardwareBuffer::HBL_NORMAL*/

	//m_LightMapDecal.createBySize((m_pTerrainData->getWidth()+1), m_pTerrainData->getChunkSize(), m_pTerrainData->getChunkSize());
	//m_LightDecal.createBySize((m_pTerrainData->getWidth()+1),8,8);
//	return true;
//}

/*void CTerrain::brushLightColor(MAP_EDIT_RECORD& mapEditRecord, float fPosX, float fPosY, Color32 colorPaint, float fRadius, float fHardness, float fStrength)
{
	if (isPointIn(fPosX, fPosY))
	{
		EditTarget editTarget;
		//editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_COLOR;
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				editTarget.x = x;
				editTarget.y = y;
				if (isPointIn(x,y))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-fHardness),1.0f);
					if (fOffset>0)
					{
						float fRate=fOffset*fStrength;
						Color32& colorDest = (Color32)mapEditRecord[editTarget].color;
						if(colorDest.c==0)
						{
							colorDest=getVertexColor(x,y);
						}
						colorDest += colorPaint*fRate;
					}
				}
			}
		}
	}
}

void CTerrain::drawLightDecal(float x, float y, float fSize, Color32 color)
*/