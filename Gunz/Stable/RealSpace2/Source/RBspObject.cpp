#include "stdafx.h"
// RBspObject.cpp: 99.2.9 by dubble

#include <crtdbg.h>
#include <map>

#include "MXml.h"
#include "MZFileSystem.h"
#include "RBspObject.h"
#include "RMaterialList.h"
#include "RealSpace2.h"
#include "RBaseTexture.h"
#include "MDebug.h"
#include "RVersions.h"
#include "RMaterialList.h"
#include "RVisualMeshMgr.h"
#include "FileInfo.h"
#include "ROcclusionList.h"
#include "MProfiler.h"
#include "RShaderMgr.h"
#include "RLenzFlare.h"
#include <dxerr9.h>
#include "RNavigationNode.h"

using namespace std;

_NAMESPACE_REALSPACE2_BEGIN


#define TOLERENCE 0.001f
#define SIGN(x) ( (x)<-TOLERENCE ? -1 : (x)>TOLERENCE ? 1 : 0 )

#define MAX_LIGHTMAP_SIZE		1024
#define MAX_LEVEL_COUNT			10			// 2^MAX_LEVEL_COUNT=MAX_LIGHTMAP_SIZE �����Ѵ�..

//#define GENERATE_TEMP_FILES   // ���Ϸ� ����Ʈ���� �����Ѵ�.

#define DEFAULT_BUFFER_SIZE	1000

// �׽�Ʈ��
LPDIRECT3DTEXTURE9 g_pShademap=NULL;

// ����׿�.. 
int nsplitcount=0,nleafcount=0;
int g_nPoly,g_nCall;
int g_nPickCheckPolygon,g_nRealPickCheckPolygon;
int g_nFrameNumber=0;

// �ε����� ���� �ʿ��� �ӽ� ������..
int				g_nCreatingPosition;
BSPVERTEX		*g_pLPVertices;
WORD			*g_pLPIndices;
RSBspNode		*g_pLPNode;
RPOLYGONINFO	*g_pLPInfo;

rvector			*g_pLPColVertices;
RSolidBspNode	*g_pLPColNode;

//rvector			*g_pLPNavVertices;
//RSolidBspNode	*g_pLPNavNode;


void DrawBoundingBox(rboundingbox *bb,DWORD color)
{
	int i,j;

	int ind[8][3]= { {0,0,0},{1,0,0},{1,1,0},{0,1,0}, {0,0,1},{1,0,1},{1,1,1},{0,1,1} };
	int lines[12][2] = { {0,1},{1,5},{5,4},{4,0},{5,6},{1,2},{0,3},{4,7},{7,6},{6,2},{2,3},{3,7} };

	for(i=0;i<12;i++)
	{

		rvector a,b;
		for(j=0;j<3;j++)
		{
			a[j]=ind[lines[i][0]][j] ? bb->vmax[j] : bb->vmin[j];
			b[j]=ind[lines[i][1]][j] ? bb->vmax[j] : bb->vmin[j];
		}

		RDrawLine(a,b,color);
	}
}

static bool m_bisDrawLightMap=true;

/////////////////////////////////////////////////////////////
//	RSBspNode

RSBspNode::RSBspNode()
{
	m_pPositive=m_pNegative=NULL;
	nPolygon=0;
	pInfo=NULL;
	ppInfoSorted=NULL;

	nFrameCount=-1;
	pDrawInfo = NULL;
}

RSBspNode::~RSBspNode()
{
	SAFE_DELETE_ARRAY(ppInfoSorted);
	SAFE_DELETE_ARRAY(pDrawInfo);
}

void RSBspNode::DrawBoundingBox(DWORD color)
{
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	RealSpace2::DrawBoundingBox(&bbTree,color);

	if(m_pNegative) m_pNegative->DrawBoundingBox(color);
	if(m_pPositive) m_pPositive->DrawBoundingBox(color);
}

void RSBspNode::DrawWireFrame(int nPolygon,DWORD color)
{
	RPOLYGONINFO *info = &pInfo[nPolygon];

	for(int i=0;i<info->nVertices;i++)
	{
		RDrawLine(*info->pVertices[i].Coord(),*info->pVertices[(i+1) % info->nVertices].Coord(),color);
//		RDrawLine(*pVertices[nFace*3+i%3].Coord(),*pVertices[nFace*3+(i+1)%3].Coord(),color);
	}
}

RSBspNode* RSBspNode::GetLeafNode(rvector &pos)
{
	if(nPolygon) return this;
	if(plane.a*pos.x+plane.b*pos.y+plane.c*pos.z+plane.d>0)
		return m_pPositive->GetLeafNode(pos);
	else
		return m_pNegative->GetLeafNode(pos);
}

/////////////////////////////////////////////////////////////
//	RBspLightmapManager

RBspLightmapManager::RBspLightmapManager()
{
	m_nSize=MAX_LIGHTMAP_SIZE;
	m_pData=new DWORD[MAX_LIGHTMAP_SIZE*MAX_LIGHTMAP_SIZE];
	m_pFreeList=new RFREEBLOCKLIST[MAX_LEVEL_COUNT+1];

	POINT p={0,0};
	m_pFreeList[MAX_LEVEL_COUNT].push_back(p);
}

RBspLightmapManager::~RBspLightmapManager()
{
	Destroy();
}

void RBspLightmapManager::Destroy()
{
	SAFE_DELETE(m_pData);
	if(m_pFreeList){
		delete []m_pFreeList;
		m_pFreeList=NULL;
	}
}

float RBspLightmapManager::CalcUnused() 
{
	float fUnused=0.f;

	for(int i=0;i<=MAX_LEVEL_COUNT;i++) {
		float fThisLevelSize=pow(0.25,(MAX_LEVEL_COUNT-i));
		fUnused+=(float)m_pFreeList[i].size()*fThisLevelSize;
	}

	return fUnused;
}

bool RBspLightmapManager::GetFreeRect(int nLevel,POINT *pt)
{
	if(nLevel>MAX_LEVEL_COUNT) return false;

	if(!m_pFreeList[nLevel].size())		// �ش��ϴ� ũ���� ����� �ϳ��� ������ 
	{
		POINT point;
		if(!GetFreeRect(nLevel+1,&point))	// ���� �ϳ��� �޾ƿ´�. ���� ������ ���°����� ���� ���̹Ƿ� �ٷ� ������.
			return false;

		int nSize=1<<nLevel;

		POINT newpoint;						// �޾ƿ� ������ 4����ؼ� �������� ����, ������ �������� ��� ����Ʈ�� �־�д�.

		newpoint.x=point.x+nSize;newpoint.y=point.y;
		m_pFreeList[nLevel].push_back(newpoint);

		newpoint.x=point.x;newpoint.y=point.y+nSize;
		m_pFreeList[nLevel].push_back(newpoint);

		newpoint.x=point.x+nSize;newpoint.y=point.y+nSize;
		m_pFreeList[nLevel].push_back(newpoint);

		*pt=point;

	}else
	{
		*pt=*m_pFreeList[nLevel].begin();
		m_pFreeList[nLevel].erase(m_pFreeList[nLevel].begin());
	}

	return true;
}

bool RBspLightmapManager::Add(DWORD *data,int nSize,POINT *retpoint)
{
	int nLevel=0,nTemp=1;
	while(nSize>nTemp)
	{
		nTemp=nTemp<<1;
		nLevel++;
	}
	_ASSERT(nSize==nTemp);

	POINT pt;
	if(!GetFreeRect(nLevel,&pt))		// �� ������ ������ ����
		return false;

	for(int y=0;y<nSize;y++)
	{
		for(int x=0;x<nSize;x++)
		{
			m_pData[(y+pt.y)*GetSize()+(x+pt.x)]=data[y*nSize+x];
		}
	}
	*retpoint=pt;
	return true;
}

void RBspLightmapManager::Save(const char *filename)
{
	RSaveAsBmp(GetSize(),GetSize(),m_pData,filename);
}

RMapObjectList::~RMapObjectList()
{
	while(size())
	{
		Delete(begin());
	}
}

RMapObjectList::iterator RMapObjectList::Delete(iterator i)
{
	ROBJECTINFO *info=*i;
	delete info->pVisualMesh;
	delete info;
	return erase(i);
}

////////////////////////////
// RBspObject

RBspObject::RBspObject()
{
	m_pBspRoot=NULL;
	m_pOcRoot=NULL;
	m_ppLightmapTextures=NULL;
	m_pMaterials=NULL;
	g_pLPVertices=NULL;
	m_pVertexBuffer=NULL;
	m_pIndexBuffer=NULL;
	m_pConvexVertices=NULL;
	m_pConvexPolygons=NULL;
//	m_pSourceVertices=NULL;
	m_pBspVertices=NULL;
	m_pBspIndices=NULL;
	m_pOcVertices=NULL;
	m_pOcIndices=NULL;
	m_pBspInfo=NULL;
	m_pOcInfo=NULL;
//	m_pOcclusion=NULL;

	m_bWireframe=false;
	m_bShowLightmap=false;
	m_AmbientLight=rvector(0,0,0);

	m_MeshList.SetMtrlAutoLoad(true);
	m_MeshList.SetMapObject(true);

	m_nMaterial=0;
	m_nLightmap=0;
//	m_nOcclusion=0;

	m_pColVertices=NULL;
	m_pColRoot=NULL;

	m_bNotOcclusion = false;

//	m_pNavRoot=NULL;
//	m_pNavVertices=NULL;

	m_nPolygon = 0;
	m_nNodeCount = 0;
	m_nBspPolygon = 0;
	m_nBspNodeCount = 0;

	m_nConvexPolygon = 0;
	m_nConvexVertices = 0;
	m_pConvexNormals = NULL;

	m_DebugInfo.pLastColNode = NULL;

	m_pDynLightVertexBuffer = NULL;
}

void RBspObject::ClearLightmaps()
{
	while(m_LightmapList.size())
	{
		delete *m_LightmapList.begin();
		m_LightmapList.erase(m_LightmapList.begin());
	}

	if(m_ppLightmapTextures)
	{
		for(int i=0;i<m_nLightmap;i++)
			SAFE_RELEASE(m_ppLightmapTextures[i]);
		SAFE_DELETE(m_ppLightmapTextures);
	}

	// ����Ʈ���� �������̾��ٸ� �����￡ ������ ��ȣ�� ����
	if( m_nLightmap )
	{
		for(int i=0;i<m_nPolygon;i++)
			m_pOcInfo[i].nLightmapTexture = 0;
	}

	m_nLightmap = 0;
}

void RBspObject::LightMapOnOff(bool bDraw)
{
	if(m_bisDrawLightMap == bDraw)
		return;

	m_bisDrawLightMap = bDraw;

	if(bDraw) {	// �ٽ� �д´�
		OpenLightmap();
	} else {	// �����~
		ClearLightmaps();
	}

	CreateRenderInfo();
}

void RBspObject::SetDrawLightMap(bool b) { 
	m_bisDrawLightMap = b; 
}

RBspObject::~RBspObject()
{
	ClearLightmaps();

	OnInvalidate();

	SAFE_RELEASE(m_pDynLightVertexBuffer);

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	
	SAFE_DELETE_ARRAY(m_pConvexNormals);
	SAFE_DELETE_ARRAY(m_pConvexVertices);
	SAFE_DELETE_ARRAY(m_pConvexPolygons);
//	SAFE_DELETE(m_pSourceVertices);

	SAFE_DELETE(m_pColVertices);
	if(m_pColRoot)
	{
		delete []m_pColRoot;
		m_pColRoot=NULL;
	}

/*
	if (m_pNavVertices)
	{
		delete m_pNavVertices;
		m_pNavVertices=NULL;
	}
	if (m_pNavRoot)
	{
		delete[] m_pNavRoot;
		m_pNavRoot;
	}
*/

	SAFE_DELETE(m_pBspInfo);
	SAFE_DELETE(m_pBspVertices);
	SAFE_DELETE(m_pBspIndices);
	if(m_pBspRoot)
	{
		delete []m_pBspRoot;
		m_pBspRoot=NULL;
	}

	SAFE_DELETE(m_pOcInfo);
	SAFE_DELETE(m_pOcVertices);
	SAFE_DELETE(m_pOcIndices);
	if(m_pOcRoot)
	{
		delete []m_pOcRoot;
		m_pOcRoot=NULL;
	}
/*
	while(m_sourcelightmaplist.size())
	{
		delete *m_sourcelightmaplist.begin();
		m_sourcelightmaplist.erase(m_sourcelightmaplist.begin());
	}
*/
	if(m_nMaterial)
	{
		for(int i=0;i<m_nMaterial;i++)
		{
			RDestroyBaseTexture(m_pMaterials[i].texture);
			m_pMaterials[i].texture=NULL;
		}
		if(m_pMaterials)
		{
			delete []m_pMaterials;
			m_pMaterials=NULL;
		}
	}

	/*
	if(m_nOcclusion)
	{
		if(m_pOcclusion)
		{
			delete []m_pOcclusion;
			m_pOcclusion=NULL;
		}
	}
	*/
/*
		if(m_pBuffers)
		{
			for(int i=0;i<m_nMaterial;i++)
			{
				SAFE_DELETE(m_pBuffers[i].pBuffer);
			}
			SAFE_DELETE(m_pBuffers);
		}
*/
	for( RLightList::iterator iter = m_StaticSunLigthtList.begin(); iter != m_StaticSunLigthtList.end(); )
	{
		SAFE_DELETE( *iter );
		iter = m_StaticSunLigthtList.erase( iter );
	}


	for( list<AmbSndInfo*>::iterator iter = m_AmbSndInfoList.begin(); iter != m_AmbSndInfoList.end(); )
	{
		AmbSndInfo* p = *iter;
		SAFE_DELETE(p);
		iter = m_AmbSndInfoList.erase(iter);
	}

//	SAFE_DELETE_ARRAY(m_pSeqIndices);
}

void RBspObject::DrawNormal(int nIndex,float fSize)
{
	int j;
	RCONVEXPOLYGONINFO *pInfo=&m_pConvexPolygons[nIndex];

	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	for(j=0;j<pInfo->nVertices;j++)
	{
		// draw edge
		RDrawLine(pInfo->pVertices[j],pInfo->pVertices[(j+1)%pInfo->nVertices],0xff808080);
		
		// draw normal
		RDrawLine(pInfo->pVertices[j],pInfo->pVertices[j]+fSize*pInfo->pNormals[j],0xff00ff);
	}

	/*
	for(j=0;j<pInfo->nSourcePolygons*3;j++)
	{
		RDrawLine(*pInfo->pSourceVertices[j].Coord(),*pInfo->pSourceVertices[j].Coord()+fSize**pInfo->pSourceVertices[j].Normal(),0xff00ff);
	}
	*/
}

void RBspObject::SetDiffuseMap(int nMaterial)
{
	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();

	RBaseTexture *pTex=m_pMaterials[nMaterial].texture;
	if(pTex)
	{
		pd3dDevice->SetTexture(0,pTex->GetTexture());
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	}
	else
	{
		DWORD dwDiffuse=VECTOR2RGB24(m_pMaterials[nMaterial].Diffuse);
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , dwDiffuse);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	}
}

bool RBspObject::Draw(RSBspNode *pNode,int nMaterial)
{

	if(pNode->nPolygon)
	{
		if(pNode->nFrameCount!=g_nFrameNumber) return true;

		int nCount = pNode->pDrawInfo[nMaterial].nTriangleCount;
		if(nCount)
		{
			g_nCall++;
			g_nPoly+=nCount;
			HRESULT hr=RGetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
				pNode->m_nBaseVertexIndex,0,pNode->m_nVertices,
				pNode->pDrawInfo[nMaterial].nIndicesOffset,nCount);
			return true;

			/*
			HRESULT hr;
			hr=RGetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST,pNode->nPosition+3*pNode->pIndicesOffsets[nMaterial],nCount/3);
			_ASSERT(hr==D3D_OK);
			*/

			/*
			// ������ ���徿 ��������..
			int index=pNode->pDrawInfo[nMaterial].nIndicesOffset;
			for(int i=0;i<nCount;i++)
			{
				BSPVERTEX v[3];
				memcpy(v+0,&m_pOcVertices[m_pOcIndices[index++]],sizeof(BSPVERTEX));
				memcpy(v+1,&m_pOcVertices[m_pOcIndices[index++]],sizeof(BSPVERTEX));
				memcpy(v+2,&m_pOcVertices[m_pOcIndices[index++]],sizeof(BSPVERTEX));
				HRESULT hr;
				hr=RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,1,v,sizeof(BSPVERTEX));
				_ASSERT(hr==D3D_OK);
			}
			*/
		}
	}else
	{
		bool bOk=true;
		if(pNode->m_pNegative) {
			if(!Draw(pNode->m_pNegative,nMaterial))
				bOk=false;
		}
		if(pNode->m_pPositive) {
			if(!Draw(pNode->m_pPositive,nMaterial))
				bOk=false;
		}
		return bOk;
	}
	return true;
}

bool RBspObject::DrawTNT(RSBspNode *pNode,int nMaterial)
{
	if(pNode->nFrameCount!=g_nFrameNumber) return true;

	if(pNode->nPolygon)
	{
		int nCount = pNode->pDrawInfo[nMaterial].nTriangleCount;
		int nIndCount = nCount*3;
		if(nCount)
		{
			g_nPoly+=nCount;

			/*
			// ��� 5. �ʿ��� ���ؽ��� �������� ī���ؼ� �׸���.
			int index=pNode->pIndicesOffsets[nMaterial];
			static int nCount=0;
			static BSPVERTEX pVertexBuffer[3*200];
			for(int i=0;i<nIndCount;i++) {
			if(nCount==3*100) {
			g_nCall++;
			HRESULT hr=RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,100,pVertexBuffer,sizeof(BSPVERTEX));
			_ASSERT(hr==D3D_OK);
			nCount=0;
			}

			memcpy(pVertexBuffer+nCount,m_pOcVertices+m_pOcIndices[index+i],sizeof(BSPVERTEX));
			nCount++;
			}//*/

			// 4.5 ī�Ǿ��� �ε������۵� �Ἥ
			//			/*
			g_nCall++;
			if(nCount)
			{
				int index=pNode->pDrawInfo[nMaterial].nIndicesOffset;
				HRESULT hr=RGetDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,pNode->pDrawInfo[nMaterial].nVertice,nCount,
					m_pOcIndices+index,D3DFMT_INDEX16,pNode->pDrawInfo[nMaterial].pVertices,sizeof(BSPVERTEX));
				_ASSERT(hr==D3D_OK);
			}
			//*/

			// ��� 4. ī�Ǿ��� �׸���
			/*
			g_nCall++;
			if(nCount)
			{
			HRESULT hr=RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,nCount,pNode->ppVertices[nMaterial],sizeof(BSPVERTEX));
			_ASSERT(hr==D3D_OK);
			}
			//*/


			// ��� 3. �ʿ��� ���ؽ��� ī���ؼ� �׸���.
			/*
			int index=pNode->pIndicesOffsets[nMaterial];
			static int nCount=0;
			static BSPVERTEX pVertexBuffer[3*100];
			for(int i=0;i<nIndCount;i++) {
			if(nCount==3*100) {
			g_nCall++;
			HRESULT hr=RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,100,pVertexBuffer,sizeof(BSPVERTEX));
			_ASSERT(hr==D3D_OK);
			nCount=0;
			}

			memcpy(pVertexBuffer+nCount,m_pOcVertices+m_pOcIndices[index+i],sizeof(BSPVERTEX));
			nCount++;
			}//*/


			/*
			// ��� 2, �ѳ����� ���� material �� ���� �ѳ��� ���ؽ��� min,max ã�Ƽ� ��´�
			int index=pNode->pIndicesOffsets[nMaterial];
			int nMin=m_nVertices,nMax=0;
			for(int i=0;i<nIndCount;i++) {
			nMin = min(nMin,m_pOcIndices[index+i]);
			nMax = max(nMax,m_pOcIndices[index+i]);
			}
			HRESULT hr=RGetDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,nMin,nMax-nMin+1,nCount,
			m_pOcIndices+index,D3DFMT_INDEX16,m_pOcVertices,sizeof(BSPVERTEX));
			_ASSERT(hr==D3D_OK);
			//*/

			// ��� 1, �������﾿ ��´�
			/*
			int index=pNode->pIndicesOffsets[nMaterial];
			for(int i=0;i<nCount;i++)
			{
			BSPVERTEX v[3];
			memcpy(v+0,&m_pOcVertices[m_pOcIndices[index++]],sizeof(BSPVERTEX));
			memcpy(v+1,&m_pOcVertices[m_pOcIndices[index++]],sizeof(BSPVERTEX));
			memcpy(v+2,&m_pOcVertices[m_pOcIndices[index++]],sizeof(BSPVERTEX));
			HRESULT hr;
			hr=RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,1,v,sizeof(BSPVERTEX));
			_ASSERT(hr==D3D_OK);
			}
			*/
		}
	}else
	{
		bool bOk=true;
		if(pNode->m_pNegative) {
			if(!DrawTNT(pNode->m_pNegative,nMaterial))
				bOk=false;
		}
		if(pNode->m_pPositive) {
			if(!DrawTNT(pNode->m_pPositive,nMaterial))
				bOk=false;
		}
		return bOk;
	}
	return true;
}

int g_nChosenNodeCount;

bool RBspObject::Draw()
{
	g_nFrameNumber++;
	g_nPoly=0;
	g_nCall=0;

	if(RIsHardwareTNL() && 
		(!m_pVertexBuffer || !m_pIndexBuffer)) return false;

	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_ZERO );

	pd3dDevice->SetFVF( BSP_FVF );

	RGetDevice()->SetStreamSource(0,m_pVertexBuffer,0,sizeof(BSPVERTEX) );
	RGetDevice()->SetIndices(m_pIndexBuffer);

	if(m_bWireframe)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	}
	else
	{
		if(m_bShowLightmap)
		{
			// lightmap �� ���͸��� ���ϴ� : ����׿�
			pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

			pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0xffffffff);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE4X );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		}else
		{
			bool bTrilinear=RIsTrilinear();

			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER , bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );
			pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER , bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

			if(!m_nLightmap)
			{
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			}else
			{
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE4X );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			}
		}
	}

	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	/*
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	*/
/*
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER , D3DTEXF_NONE );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER , D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER , D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER , D3DTEXF_NONE );
//	*/

/*	DWORD dwNumPasses;
	HRESULT hr=pd3dDevice->ValidateDevice( &dwNumPasses );
	if(hr!=D3D_OK) {
		char buffer[256];
//		D3DXGetErrorString(hr,buffer,sizeof(buffer));
		mlog("%s\n",buffer);
	}
//	_ASSERT(hr==D3D_OK);
*/
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
    RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );


	// world matrix �� identity �� �ƴѰ�찡 �����Ƿ� camera �� viewfrustum �� �Űܾ��Ѵ�
	rmatrix mat;
	RGetDevice()->GetTransform(D3DTS_WORLD, &mat);

	m_OcclusionList.UpdateCamera(mat,RCameraPosition);

	// �츮�� ������ viewfrustum ����� bsp�� ���÷� ��ȯ�ؾ� �ϹǷ�, inv(mat) �� ��ȯ���־�� �Ѵ�
	// �׷��� D3DXPlaneTransform �� ������ ��ȯ����� inverse transpose ��Ʈ������ �Ѱ�����ϹǷ�
	// tr(inv(inv(mat))) �� �ǹǷ� �ᱹ tr(mat) �� �ȴ�

	rmatrix trMat;
	D3DXMatrixTranspose(&trMat, &mat);

	int i;

	for(i=0;i<6;i++)
	{
		D3DXPlaneTransform(m_localViewFrustum+i,RGetViewFrustum()+i,&trMat);
	}

	g_nChosenNodeCount=0;
	_BP("Choose Nodes..");
	ChooseNodes(m_pOcRoot);
	_EP("Choose Nodes..");
	
	RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false );
	RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	RSetWBuffer(true);
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true );

	int nCount=m_nMaterial*max(1,m_nLightmap);

	for(i=0;i<nCount;i++)
	{
		int nMaterial = i % m_nMaterial;
		int nLightmap = i / m_nMaterial;
		if((m_pMaterials[nMaterial].dwFlags & (RM_FLAG_ADDITIVE|RM_FLAG_USEOPACITY) ) == 0 )
		{
			if((m_pMaterials[nMaterial].dwFlags & RM_FLAG_TWOSIDED) == 0 )
				RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
			else
				RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );

			if(m_ppLightmapTextures)
				RGetDevice()->SetTexture(1,m_ppLightmapTextures[nLightmap]);

			SetDiffuseMap(nMaterial);
			if(RIsHardwareTNL())
				Draw(m_pOcRoot,i);
			else
				DrawTNT(m_pOcRoot,i);
		}
	}

	// opacity map �� ���°͵�

	for(int i=0;i<nCount;i++)
	{
		int nMaterial = i % m_nMaterial;
		int nLightmap = i / m_nMaterial;
		if((m_pMaterials[nMaterial].dwFlags & RM_FLAG_USEOPACITY) == RM_FLAG_USEOPACITY )
		{
			if((m_pMaterials[nMaterial].dwFlags & RM_FLAG_TWOSIDED) == 0 )
				RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
			else
				RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );

			if(m_ppLightmapTextures)
				RGetDevice()->SetTexture(1,m_ppLightmapTextures[nLightmap]);

			if((m_pMaterials[nMaterial].dwFlags & RM_FLAG_USEALPHATEST) != 0 ) {
				RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
				RGetDevice()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x80808080);
				RGetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	true);

				RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false );
				RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
				RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
				RGetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP , D3DTOP_DISABLE);
			}
			else{
				RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false );
				RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	false );
				RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true );
				RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
				RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );

				RGetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAARG1 , D3DTA_CURRENT );
				RGetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
			}


			SetDiffuseMap(nMaterial);
			if(RIsHardwareTNL())
				Draw(m_pOcRoot,i);
			else
				DrawTNT(m_pOcRoot,i);
		}
	}

	// additive �ΰ͵��� ���߿� ��´�.

	RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);

	for(int i=0;i<nCount;i++)
	{
		int nMaterial = i % m_nMaterial;
		int nLightmap = i / m_nMaterial;
		if((m_pMaterials[nMaterial].dwFlags & RM_FLAG_ADDITIVE) == RM_FLAG_ADDITIVE )
		{
			if((m_pMaterials[nMaterial].dwFlags & RM_FLAG_TWOSIDED) == 0 )
				RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
			else
				RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );

			if(m_ppLightmapTextures)
				RGetDevice()->SetTexture(1,m_ppLightmapTextures[nLightmap]);

			SetDiffuseMap(nMaterial);
			if(RIsHardwareTNL())
				Draw(m_pOcRoot,i);
			else
				DrawTNT(m_pOcRoot,i);
		}
	}

	RGetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
	RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,false);
	RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_DISABLE );
	RGetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP , D3DTOP_DISABLE );

	// state ���� �����س���..
	RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false );
	RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true );

	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 	pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
/*
	DrawObjects();
//	_RPT2(_CRT_WARN,"%d polygon , %d call \n",g_nPoly,g_nCall);

	RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	RGetDevice()->SetStreamSource( 0 , NULL,0, 0 );
	//*/

	return true;
}

// �ʿ� ��ϵ� ������Ʈ �̿��� ������Ʈ ó�� ( ĳ���� ����ȭ��� )

void RBspObject::SetObjectLight(rvector vPos)
{
	float fIntensityFirst = FLT_MIN;

	float fdistance = 0.f;
	float fIntensity = 0.f;

	RLIGHT *first = NULL;
	RLIGHT *plight = NULL;

	RLightList *pllist = GetObjectLightList();

	D3DLIGHT9 light;

	ZeroMemory( &light, sizeof(D3DLIGHT9) );

	light.Type       = D3DLIGHT_POINT;

	light.Attenuation0 = 0.f;
	light.Attenuation1 = 0.0010f;
	light.Attenuation2 = 0.f;

	for(RLightList::iterator i=pllist->begin();i!=pllist->end();i++) {

		plight = *i;

		fdistance = Magnitude(plight->Position - vPos);
		fIntensity = (fdistance - plight->fAttnStart) / (plight->fAttnEnd - plight->fAttnStart);

		fIntensity = min(max(1.0f-fIntensity,0),1);
		fIntensity *= plight->fIntensity;

		fIntensity = min(max(fIntensity,0),1);

		if(fIntensityFirst < fIntensity) {
			fIntensityFirst=fIntensity;
			first=plight;
		}
	}

	if(first) {

		light.Position = first->Position;

//		light.Diffuse.r  = first->Color.x*lightmapcolor.x;
//		light.Diffuse.g  = first->Color.y*lightmapcolor.y;
//		light.Diffuse.b  = first->Color.z*lightmapcolor.z;

//		light.Diffuse.r  = first->Color.x;
//		light.Diffuse.g  = first->Color.y;
//		light.Diffuse.b  = first->Color.z;

		light.Ambient.r = min(first->Color.x*first->fIntensity * 0.25,1.f);
		light.Ambient.g = min(first->Color.y*first->fIntensity * 0.25,1.f);
		light.Ambient.b = min(first->Color.z*first->fIntensity * 0.25,1.f);

		light.Diffuse.r  = min(first->Color.x*first->fIntensity * 0.25,1.f);
		light.Diffuse.g  = min(first->Color.y*first->fIntensity * 0.25,1.f);
		light.Diffuse.b  = min(first->Color.z*first->fIntensity * 0.25,1.f);

		light.Specular.r = 1.f;
		light.Specular.g = 1.f;
		light.Specular.b = 1.f;

		light.Range       = first->fAttnEnd*1.0f;

		RGetDevice()->SetLight( 0, &light );
		RGetDevice()->LightEnable( 0, TRUE );

		RShaderMgr::getShaderMgr()->setLight( 0, &light );
	}
}

void RBspObject::SetCharactorLight(rvector pos)
{

}

bool e_mapobject_sort_float(ROBJECTINFO* _a,ROBJECTINFO* _b) {
	if( _a->fDist > _b->fDist )
		return true;
	return false;
}

void RBspObject::DrawObjects()
{
	m_DebugInfo.nMapObjectFrustumCulled = 0;
	m_DebugInfo.nMapObjectOcclusionCulled = 0;

	RGetDevice()->SetRenderState(D3DRS_CULLMODE ,D3DCULL_NONE);
	RGetDevice()->SetRenderState(D3DRS_LIGHTING ,TRUE);

//	m_MeshList.Frame();

	D3DXMATRIX world;
	RGetDevice()->GetTransform(D3DTS_WORLD, &world);

	rvector v_add = rvector(world._41,world._42,world._43);

//	mlog("begin \n");

	rvector camera_pos = RealSpace2::RCameraPosition;
	rvector t_vec;

	for(list<ROBJECTINFO*>::iterator i=m_ObjectList.begin();i!=m_ObjectList.end();i++) {

		ROBJECTINFO *pInfo = *i;
		if(!pInfo->pVisualMesh) continue;

		if(pInfo) {
			t_vec = rvector(pInfo->pVisualMesh->m_WorldMat._41,
							pInfo->pVisualMesh->m_WorldMat._42,
							pInfo->pVisualMesh->m_WorldMat._43 );
			t_vec = camera_pos - t_vec;
			pInfo->fDist = Magnitude(t_vec);
		}
	}

	m_ObjectList.sort(e_mapobject_sort_float);

	for(list<ROBJECTINFO*>::iterator i=m_ObjectList.begin();i!=m_ObjectList.end();i++)
	{
		ROBJECTINFO *pInfo=*i;
		if(!pInfo->pVisualMesh) continue;

		// occlusion test
		rboundingbox bb;
		bb.vmax=pInfo->pVisualMesh->m_vBMax;
		bb.vmin=pInfo->pVisualMesh->m_vBMin;

		if( !m_bNotOcclusion ) {
			if(!IsVisible(bb)) {
				m_DebugInfo.nMapObjectOcclusionCulled++;
				continue;
			}
		}else {
			pInfo->pVisualMesh->m_bCheckViewFrustum = false;
		}

		bool bLight = true;

		if(pInfo->pVisualMesh && pInfo->pVisualMesh->m_pMesh)
			bLight = !pInfo->pVisualMesh->m_pMesh->m_LitVertexModel;

		if( pInfo->pLight && bLight )
		{
			D3DLIGHT9 light;
			ZeroMemory( &light, sizeof(D3DLIGHT9) );

			light.Type       = D3DLIGHT_POINT;

			light.Attenuation0 = 0.f;
//			light.Attenuation1 = 0.0001f;
			light.Attenuation1 = 0.0001f;
			light.Attenuation2 = 0.f;

			light.Position = pInfo->pLight->Position + v_add;

			rvector lightmapcolor(1,1,1);

//			if(Pick(m_CurrentPos+rvector(0,0,100),m_CurrentPos,&pNode,&nIndex,&PickPos)) {
//				color=g_pGame->m_bsp.GetLightmap(PickPos,pNode,nIndex);
//				lightmapcolor=DWORD2VECTOR(color);
//			}

//			light.Diffuse.r  = 1.f;
//			light.Diffuse.g  = 0.f;
//			light.Diffuse.b  = 0.f;
//			light.Diffuse.r  = pInfo->pLight->Color.x*lightmapcolor.x;
//			light.Diffuse.g  = pInfo->pLight->Color.y*lightmapcolor.y;
//			light.Diffuse.b  = pInfo->pLight->Color.z*lightmapcolor.z;

//			light.Ambient.r = pInfo->pLight->Color.x*pInfo->pLight->fIntensity*lightmapcolor.x;
//			light.Ambient.g = pInfo->pLight->Color.y*pInfo->pLight->fIntensity*lightmapcolor.y;
//			light.Ambient.b = pInfo->pLight->Color.z*pInfo->pLight->fIntensity*lightmapcolor.z;

			light.Diffuse.r  = pInfo->pLight->Color.x*pInfo->pLight->fIntensity;
			light.Diffuse.g  = pInfo->pLight->Color.y*pInfo->pLight->fIntensity;
			light.Diffuse.b  = pInfo->pLight->Color.z*pInfo->pLight->fIntensity;

//			light.Range       = pInfo->pLight->fAttnEnd * 1.5f;
			light.Range       = pInfo->pLight->fAttnEnd;

			RGetDevice()->SetLight( 0, &light );
			RGetDevice()->LightEnable( 0, TRUE );
			RGetDevice()->LightEnable( 1, FALSE );
		}

		pInfo->pVisualMesh->SetWorldMatrix(world);
		pInfo->pVisualMesh->Frame();
//		pInfo->pVisualMesh->Render();
		pInfo->pVisualMesh->Render(&m_OcclusionList);

		if(!pInfo->pVisualMesh->m_bIsRender) m_DebugInfo.nMapObjectFrustumCulled++;

//		mlog("%s,%s \n",pInfo->pVisualMesh->m_pMesh->m_name,pInfo->pLight->Name.c_str());
	}
//	mlog("end \n");
}

void RBspObject::DrawBoundingBox()
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pOcRoot->DrawBoundingBox(0xffffff);
}

void RBspObject::DrawOcclusions()
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetRenderState(D3DRS_ZENABLE, false );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	for(ROcclusionList::iterator i=m_OcclusionList.begin();i!=m_OcclusionList.end();i++)
	{
		ROcclusion *poc=*i;
		
		for(int j=0;j<poc->nCount;j++)
		{
			RDrawLine(poc->pVertices[j],poc->pVertices[(j+1)%poc->nCount],0xffff00ff);
		}
	}

	RSetWBuffer(true);
}

#ifndef _PUBLISH
void RBspObject::DrawColNodePolygon(rvector &pos)
{ 
	m_pColRoot->DrawPos(pos); 
}

void RBspObject::DrawCollision_Polygon()
{
	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();

	pd3dDevice->SetFVF( D3DFVF_XYZ );
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x406fa867);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_pColRoot->DrawPolygon();

	RSetWBuffer(true);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ffffff);
	m_pColRoot->DrawPolygonWireframe();

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ff00ff);
	m_pColRoot->DrawPolygonNormal();

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

}

void RBspObject::DrawCollision_Solid()
{
	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();

	pd3dDevice->SetFVF( D3DFVF_XYZ );
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40808080);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_pColRoot->DrawSolidPolygon();

	RSetWBuffer(true);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ffffff);
	m_pColRoot->DrawSolidPolygonWireframe();

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ff00ff);
	m_pColRoot->DrawSolidPolygonNormal();

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

}

void RBspObject::DrawNavi_Polygon()	
{
	m_NavigationMesh.Render();
}

void RBspObject::DrawNavi_Links()
{
	m_NavigationMesh.RenderLinks();
}

#else
void RBspObject::DrawColNodePolygon(rvector &pos)	{}
void RBspObject::DrawCollision_Polygon()			{}
void RBspObject::DrawCollision_Solid()				{}
void RBspObject::DrawNavi_Polygon()					{}
void RBspObject::DrawNavi_Links()					{}

#endif

bool isInViewFrustumWithFarZ(rboundingbox *bb,rplane *plane)
{
	if(isInPlane(bb,plane)&&isInPlane(bb,plane+1)&&
		isInPlane(bb,plane+2)&&isInPlane(bb,plane+3)&&
		isInPlane(bb,plane+5)) return true;
	return false;
}

void RBspObject::ChooseNodes(RSBspNode *bspNode)
{
	if(isInViewFrustumWithFarZ(&bspNode->bbTree,m_localViewFrustum))
	{
		if(bspNode->nPolygon)
		{
			if(!IsVisible(bspNode->bbTree)) return;
		}

		g_nChosenNodeCount++;
		bspNode->nFrameCount=g_nFrameNumber;
		if(bspNode->m_pNegative)
			ChooseNodes(bspNode->m_pNegative);
		if(bspNode->m_pPositive)
			ChooseNodes(bspNode->m_pPositive);
	}
}

int RBspObject::ChooseNodes(RSBspNode *bspNode,rvector &center,float fRadius)
{
	if (bspNode == NULL) return 0;

	if(IsInSphere(bspNode->bbTree,center,fRadius))
		bspNode->nFrameCount=g_nFrameNumber;

	if (!bspNode->nPolygon)
	{
		int nm_pNegative=ChooseNodes(bspNode->m_pNegative,center,fRadius);
		int nm_pPositive=ChooseNodes(bspNode->m_pPositive,center,fRadius);

		return 1+nm_pNegative+nm_pPositive;
	}
	return 0;
}

// bsp �� ����ϴ� ����ε�, �ӵ� ���������� ���� !
/*
void RBspObject::TraverseTreeAndRender(RSBspNode *bspNode)
{
	if (bspNode == NULL) return;
	if (bspNode->nPolygon) DrawNodeFaces(bspNode);
	else
	{
		if(isInViewFrustum(&bspNode->bbTree,RGetViewFrustum()))
		{
			if (bspNode->plane.a*RCameraPosition.x+
				bspNode->plane.b*RCameraPosition.y+
				bspNode->plane.c*RCameraPosition.z+
				bspNode->plane.d>0) // if position is m_pPositive side
			{
				TraverseTreeAndRender(bspNode->m_pNegative);
				TraverseTreeAndRender(bspNode->m_pPositive);
			}
			else {
				TraverseTreeAndRender(bspNode->m_pPositive);
				TraverseTreeAndRender(bspNode->m_pNegative);
			}
		}
	}
} // BSPtraverseTreeAndRender()

void RBspObject::Flush(int nMaterial)
{
	RGetDevice()->SetTexture(0, (nMaterial==0) ? NULL : m_ppBaseTextures[nMaterial]->GetTexture());

	RPolygonBuffer *pBuffer=m_pBuffers+nMaterial;
	for(int i=0;i<pBuffer->nCount;i++)
	{
		HRESULT hr=RGetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST,pBuffer->pBuffer[i],1);
	}
	g_nPoly+=pBuffer->nCount;
	pBuffer->nCount=0;
}

void RBspObject::DrawNodeFaces(RSBspNode *bspNode)
{
	for(int i=0;i<bspNode->nPolygon;i++)
	{
		int nMaterial=bspNode->pInfo[i].nTexture;
		if(m_pBuffers[nMaterial].nCount>=DEFAULT_BUFFER_SIZE)
			Flush(nMaterial);
		m_pBuffers[nMaterial].Add(bspNode->nPosition+i*3);
	}
}
*/

bool RBspObject::ReadString(MZFile *pfile,char *buffer,int nBufferSize)
{
	int nCount=0;
	do{
		pfile->Read(buffer,1);
		nCount++;
		buffer++;
		if(nCount>=nBufferSize)
			return false;
	}while((*(buffer-1))!=0);
	return true;
}

bool bPathOnlyLoad = false;

void DeleteVoidNodes(RSBspNode *pNode)
{
	if(pNode->m_pPositive)
		DeleteVoidNodes(pNode->m_pPositive);
	if(pNode->m_pNegative)
		DeleteVoidNodes(pNode->m_pNegative);

	if(pNode->m_pPositive && !pNode->m_pPositive->nPolygon && !pNode->m_pPositive->m_pPositive && !pNode->m_pPositive->m_pNegative)
	{
		SAFE_DELETE(pNode->m_pPositive);
	}
	if(pNode->m_pNegative && !pNode->m_pNegative->nPolygon && !pNode->m_pNegative->m_pPositive && !pNode->m_pNegative->m_pNegative)
	{
		SAFE_DELETE(pNode->m_pNegative);
	}
}

void RecalcBoundingBox(RSBspNode *pNode)
{
	if(pNode->nPolygon)
	{
		rboundingbox *bb=&pNode->bbTree;
		bb->vmin.x=bb->vmin.y=bb->vmin.z=FLT_MAX;
		bb->vmax.x=bb->vmax.y=bb->vmax.z=-FLT_MAX;
		for(int i=0;i<pNode->nPolygon;i++)
		{
			RPOLYGONINFO *pInfo = &pNode->pInfo[i];
			
			for(int j=0;j<pInfo->nVertices;j++)
			{
				for(int k=0;k<3;k++)
				{
					bb->vmin[k]=min(bb->vmin[k],(*pInfo->pVertices[j].Coord())[k]);
					bb->vmax[k]=max(bb->vmax[k],(*pInfo->pVertices[j].Coord())[k]);
				}
			}
		}
	}
	else
	{
		if(pNode->m_pPositive)
		{
			RecalcBoundingBox(pNode->m_pPositive);
			memcpy(&pNode->bbTree,&pNode->m_pPositive->bbTree,sizeof(rboundingbox));
		}
		if(pNode->m_pNegative)
		{
			RecalcBoundingBox(pNode->m_pNegative);
			memcpy(&pNode->bbTree,&pNode->m_pNegative->bbTree,sizeof(rboundingbox));
		}
		if(pNode->m_pPositive) MergeBoundingBox(&pNode->bbTree,&pNode->m_pPositive->bbTree);
		if(pNode->m_pNegative) MergeBoundingBox(&pNode->bbTree,&pNode->m_pNegative->bbTree);
	}
}

bool RBspObject::Open(const char *filename, const char* descExtension, ROpenFlag nOpenFlag,RFPROGRESSCALLBACK pfnProgressCallback, void *CallbackParam)
{
	mlog("BspObject open : begin %s \n",filename);

	m_OpenMode = nOpenFlag;
	m_filename=filename;

	char xmlname[_MAX_PATH];
	sprintf(xmlname,"%s.%s",filename,descExtension);

	if(!OpenDescription(xmlname))
	{
		MLog("Error while loading %s\n",xmlname);
		return false;
	}
	if(pfnProgressCallback) pfnProgressCallback(CallbackParam,.3f);

	mlog("RBspObject::Open : OpenDescription\n");

	if(!OpenRs(filename))
	{
		MLog("Error while loading %s\n",filename);
		return false;
	}

	if(pfnProgressCallback) pfnProgressCallback(CallbackParam,.6f);
	mlog("RBspObject::Open : OpenRs \n");

/*	// ����
	char pathfilename[_MAX_PATH];
	sprintf(pathfilename,"%s.pat",filename);
	if((nOpenFlag==ROF_ALL || nOpenFlag==ROF_BSPANDPATH) && !OpenPathNode(pathfilename))
		MLog("Error while loading %s\n",pathfilename);
*/


	char bspname[_MAX_PATH];
	sprintf(bspname,"%s.bsp",filename);
	if(!OpenBsp(bspname))
	{
		MLog("Error while loading %s\n",bspname);
		return false;
	}

	if(pfnProgressCallback) pfnProgressCallback(CallbackParam,.8f);
	mlog("RBspObject::Open : OpenBsp \n");

	char colfilename[_MAX_PATH];
	sprintf(colfilename,"%s.col",filename);
	if(!OpenCol(colfilename))
	{
		MLog("Error while loading %s\n",colfilename);
		return false;
	}

	// ����Ʈ�ʸ� �׺���̼� ���� �д´�.
	char navfilename[_MAX_PATH];
	sprintf(navfilename,"%s.nav",filename);
	if(!OpenNav(navfilename))
	{
		//MLog("Error while loading %s\n",navfilename);
		//return false;
	}


	if(RIsHardwareTNL())
	{
		if(!CreateVertexBuffer())
			mlog("Error while Creating VB\n");
	}

	// ����Ʈ���� �д´�
	if(m_bisDrawLightMap)
		OpenLightmap();

	CreateRenderInfo();

	mlog("RBspObject::Open : done\n");

	if(pfnProgressCallback) pfnProgressCallback(CallbackParam,1.f);

	return true;
}

void RBspObject::CreateRenderInfo()
{
	Sort_Nodes(m_pOcRoot);

	if(RIsHardwareTNL())
		UpdateVertexBuffer();

	CreatePolygonTableAndIndexBuffer();

	if(RIsHardwareTNL())
	{
		if(!CreateIndexBuffer())
			mlog("Error while Creating IB\n");

		UpdateIndexBuffer();
	}
}

void RBspObject::OptimizeBoundingBox()
{
	DeleteVoidNodes(m_pOcRoot);
	RecalcBoundingBox(m_pOcRoot);
}

bool RBspObject::CreateIndexBuffer()
{
	SAFE_RELEASE(m_pIndexBuffer);

	HRESULT hr=RGetDevice()->CreateIndexBuffer(sizeof(WORD)*m_nIndices,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pIndexBuffer,NULL);
	if(D3D_OK!=hr) {
		mlog("CreateIndexBuffer failed\n");
		return false;
	}

	return true;
}

bool RBspObject::UpdateIndexBuffer()
{
	if(!m_pIndexBuffer) return false;

	WORD *pIndices = NULL;
	m_pIndexBuffer->Lock(0,0,(VOID**)&pIndices,0);
	memcpy(pIndices,m_pOcIndices,sizeof(WORD)*m_nIndices);
	m_pIndexBuffer->Unlock();
	return true;
}
 

void RBspObject::OnInvalidate()
{
//	SAFE_RELEASE(m_pIndexBuffer);

	//if(m_ppLightmapTextures)
	//{
	//	for(int i=0;i<m_nLightmap;i++)
	//	{
	//		SAFE_RELEASE(m_ppLightmapTextures[i]);
	//	}
	//	SAFE_DELETE(m_ppLightmapTextures);
	//	m_nLightmap=0;
	//}

	InvalidateDynamicLightVertexBuffer();
}

void RBspObject::OnRestore()
{
//	CreateIndexBuffer();

	if(m_bisDrawLightMap) {
//		OpenLightmap();
	}else
		Sort_Nodes(m_pOcRoot);
}

bool RBspObject::Open_MaterialList(MXmlElement *pElement)
{
	RMaterialList ml;
	ml.Open(pElement);
	///// material ó���Ѵ�.

	// 0�� ����Ʈ ���͸����� �ϳ� �����..  NULL texture
	// �� ������ ���� �ʴ� (dwFlags�� HIDE �� ����������) material �� -1��.
	m_nMaterial=ml.size();
	m_nMaterial=m_nMaterial+1;

	m_pMaterials=new RBSPMATERIAL[m_nMaterial];

	m_pMaterials[0].texture=NULL;	//new RBaseTexture;
	m_pMaterials[0].Diffuse=rvector(1,1,1);
	m_pMaterials[0].dwFlags=0;

	string strBase = m_filename;
	string::size_type nPos=strBase.find_last_of("\\"),nothing=-1;
	if(nPos==nothing)
		nPos=strBase.find_last_of("/");

	if(nPos==nothing)
		strBase="";
	else
		strBase=strBase.substr(0,nPos)+"/";



	RMaterialList::iterator itor;
	itor=ml.begin();

	for(int i=1;i<m_nMaterial;i++)
	{
		RMATERIAL *mat=*itor;

		m_pMaterials[i].dwFlags=mat->dwFlags;
		m_pMaterials[i].Diffuse=mat->Diffuse;
		m_pMaterials[i].Specular=mat->Specular;
		m_pMaterials[i].Ambient=mat->Ambient;
		m_pMaterials[i].Name=mat->Name;
		m_pMaterials[i].DiffuseMap=mat->DiffuseMap;

		string DiffuseMapName=strBase + mat->DiffuseMap;
		char szMapName[256];
		GetRefineFilename(szMapName,DiffuseMapName.c_str());

		if(strlen(szMapName))
		{
			m_pMaterials[i].texture= RCreateBaseTexture(szMapName,RTextureType_Map,true);
		}

		itor++;
	}

	mlog("RBspObject::Open : Open_MaterialList\n");
	return true;
}


bool RBspObject::Open_LightList(MXmlElement *pElement)
{
	RLightList llist;
	llist.Open(pElement);

	for(RLightList::iterator i=llist.begin();i!=llist.end();i++)
	{
		RLIGHT *plight=*i;
		if(strnicmp(plight->Name.c_str(),RTOK_MAX_OBJLIGHT,strlen(RTOK_MAX_OBJLIGHT))==0)
			m_StaticObjectLightList.push_back(plight);
		else {
			// sun light �� map light �� ���Խ�Ų��.
			m_StaticMapLightList.push_back(plight);

			if( strnicmp(plight->Name.c_str(), "sun_omni", 8 ) == 0 )
			{
				RLIGHT *psunlight=new RLIGHT;
				psunlight->Name=plight->Name;
				psunlight->dwFlags=plight->dwFlags;
				psunlight->fAttnEnd=plight->fAttnEnd;
				psunlight->fAttnStart=plight->fAttnStart;
				psunlight->fIntensity=plight->fIntensity;
				psunlight->Position=plight->Position;
				psunlight->Color	= plight->Color;
				m_StaticSunLigthtList.push_back(psunlight);
			}
		}
	}
	llist.erase(llist.begin(),llist.end());

	mlog("RBspObject::Open : Open_LightList\n");
	return true;
}

bool RBspObject::Open_OcclusionList(MXmlElement *pElement)
{
	m_OcclusionList.Open(pElement);

	for(ROcclusionList::iterator i=m_OcclusionList.begin();i!=m_OcclusionList.end();i++)
	{
		ROcclusion *poc=*i;
		poc->CalcPlane();
		poc->pPlanes=new rplane[poc->nCount+1];
	}

	return true;
}

bool RBspObject::Open_ObjectList(MXmlElement *pElement)
{
	int i;

	mlog("BspObject open object list : begin\n");

	MXmlElement	aObjectNode,aChild;
	int nCount = pElement->GetChildNodeCount();

//	RMesh* pBaseMtrlMesh=NULL;//�ӽ�

//	bool bcheck = true;

	char szTagName[256],szContents[256];
	for (i = 0; i < nCount; i++)
	{
		aObjectNode=pElement->GetChildNode(i);
		aObjectNode.GetTagName(szTagName);

		if(stricmp(szTagName,RTOK_OBJECT)==0)
		{
			ROBJECTINFO *pInfo=new ROBJECTINFO;
			aObjectNode.GetAttribute(szContents,RTOK_NAME);
			pInfo->name=szContents;

			char fname[_MAX_PATH];
            GetPurePath(fname,m_descfilename.c_str());
			strcat(fname,szContents);

			/*
//			if(bcheck)
			{
			//	m_MeshList.SetMtrlAutoLoad(true);
				pInfo->nMeshID = m_MeshList.Add(fname);
			//	m_MeshList.SetMtrlAutoLoad(false);

				RMesh *pmesh=m_MeshList.GetFast(pInfo->nMeshID);

				//mtrl �ѹ����..
				if(pmesh) {
					pBaseMtrlMesh =	pmesh;
					bcheck = false;
				}
			}
			else {
				pInfo->nMeshID=m_MeshList.Add(fname);

				RMesh *pmesh=m_MeshList.GetFast(pInfo->nMeshID);

				if(pmesh)
					pmesh->SetBaseMtrlMesh(pBaseMtrlMesh);
			}
			*/

			m_MeshList.SetMtrlAutoLoad(true);
			m_MeshList.SetMapObject(true);

			pInfo->nMeshID = m_MeshList.Add(fname);
			RMesh *pmesh=m_MeshList.GetFast(pInfo->nMeshID);

			if(pmesh)
			{
				char* pName = pmesh->GetFileName();
				while( pName[0] != '\0' )
				{ 
					if( pName[0] == 'o' && pName[1] == 'b' ) break;
					++pName;
				}

				if( strncmp(pName,"obj_water",9) != 0 && strncmp(pName,"obj_flag",8) != 0 ) 
				{
					strcat(fname,".ani");

					m_AniList.Add(fname,fname,i);
					RAnimation* pAni=m_AniList.GetAnimation(fname);

					pmesh->SetAnimation(pAni);

					pInfo->pVisualMesh=new RVisualMesh;
					pInfo->pVisualMesh->Create(pmesh);
					pInfo->pVisualMesh->SetAnimation(pAni);
				}
				else
				{
					pInfo->pVisualMesh = 0;
				}

				m_ObjectList.push_back(pInfo);
				pInfo->pLight=NULL;
			}
			else
				delete pInfo;
		}
	}

	///// objectlist �� ó���Ѵ�.

	mlog("RBspObject::Open_ObjectList : size %d \n",m_ObjectList.size());


	for(list<ROBJECTINFO*>::iterator it=m_ObjectList.begin();it!=m_ObjectList.end();it++) {

		ROBJECTINFO *pInfo=*it;

//		mlog("RBspObject::Open_ObjectList %d : %s \n",__cnt,pInfo->name.c_str());

		// ��ġ�� ������ ū ������ �ϳ��� ������ ����.
		float fIntensityFirst=FLT_MIN;

		if(pInfo == NULL) {

			mlog("RBspObject::Open_ObjectList : pInfo == NULL pVisualMesh->CalcBox ����\n");
			continue;
		}
		else {
			if(pInfo->pVisualMesh == NULL) {
				mlog("RBspObject::Open_ObjectList : pInfo->pVisualMesh == NULL \n");
				continue;
			}
		}

//		mlog("RBspObject::Open_ObjectList : pInfo->pVisualMesh->CalcBox... \n");
		pInfo->pVisualMesh->CalcBox();

		rvector center = (pInfo->pVisualMesh->m_vBMax+pInfo->pVisualMesh->m_vBMin)*.5f;

//		RLightList *pllist=GetMapLightList();
		RLightList *pllist=GetObjectLightList();

		for( RLightList::iterator rlit=pllist->begin();rlit != pllist->end(); rlit++ ) {

			RLIGHT *plight = *rlit;
			
			if(plight) {
			
				float fdistance=Magnitude(plight->Position-center);

				float fDist = plight->fAttnEnd - plight->fAttnStart;
				float fIntensity;
				if( fDist == 0 ) fIntensity = 0;
				else fIntensity=(fdistance-plight->fAttnStart)/(plight->fAttnEnd-plight->fAttnStart);

				fIntensity=min(max(1.0f-fIntensity,0),1);
				fIntensity*=plight->fIntensity;
				fIntensity=min(max(fIntensity,0),1);

				if(fIntensityFirst<fIntensity) {
					fIntensityFirst=fIntensity;
					pInfo->pLight=plight;
				}
			}
		}
	}

	mlog("RBspObject::Open_ObjectList : end\n");

	return true;
}

bool RBspObject::Make_LenzFalreList()
{
	for (RDummyList::iterator itor = m_DummyList.begin(); itor != m_DummyList.end(); ++itor)
	{
		RDummy* pDummy = *itor;

		if( stricmp( pDummy->szName.c_str(), "sun_dummy" ) == 0 )
		{
			if( !RGetLenzFlare()->SetLight( pDummy->Position ))
			{
				mlog( "Fail to Set LenzFlare Position...\n" );
			}

			return true;
		}
	}

	return true;
}

bool RBspObject::Open_DummyList(MXmlElement *pElement)
{
	m_DummyList.Open(pElement);

	Make_LenzFalreList();
	return true;
}

bool RBspObject::Set_Fog(MXmlElement *pElement)
{
	MXmlElement childElem;
	DWORD dwColor = 0;
	int color;
	char name[8];

	m_FogInfo.bFogEnable = true;
	pElement->GetAttribute(&m_FogInfo.fNear,"min");
	pElement->GetAttribute(&m_FogInfo.fFar,"max");
	int nChild = pElement->GetChildNodeCount();
	for(int i = 0 ; i < nChild; ++i)
	{
		childElem = pElement->GetChildNode(i);
		childElem.GetContents(&color);
		childElem.GetNodeName(name);
		if(name[0] == 'R')	dwColor |= (color<<16);
		else if(name[0] == 'G') dwColor |= (color<<8);
		else if(name[0] == 'B') dwColor |= (color);
	}
	m_FogInfo.dwFogColor = dwColor;
	return true;
}

bool RBspObject::Set_AmbSound(MXmlElement *pElement)
{
	MXmlElement childElem, contentElem;
	char szBuffer[128];
	AmbSndInfo* asinfo = NULL;

	int nChild = pElement->GetChildNodeCount();
	for( int i = 0 ; i < nChild; ++i )
	{
		childElem = pElement->GetChildNode(i);
		childElem.GetTagName(szBuffer);
		if(stricmp(szBuffer,"AMBIENTSOUND")==0)
		{
			asinfo = new AmbSndInfo;
			asinfo->itype = 0;


			childElem.GetAttribute(szBuffer, "type");
			if(szBuffer[0]=='a')
				asinfo->itype |= AS_2D;
			else if(szBuffer[0]=='b')
				asinfo->itype |= AS_3D;

			if(szBuffer[1]=='0')
				asinfo->itype |= AS_AABB;
			else if(szBuffer[1]=='1')
				asinfo->itype |= AS_SPHERE;

			childElem.GetAttribute(asinfo->szSoundName,"filename");

			int nContents = childElem.GetChildNodeCount();
			for( int j = 0 ; j < nContents; ++j )
			{			
				char* token = NULL;
				contentElem = childElem.GetChildNode(j);
				contentElem.GetNodeName(szBuffer);
				if(stricmp("MIN_POSITION", szBuffer)==0)
				{				
					contentElem.GetContents(szBuffer);
					token = strtok(szBuffer, " ");
					if(token!= NULL) asinfo->min.x = atof(token);
					token = strtok(NULL, " ");
					if(token!= NULL) asinfo->min.y = atof(token);
					token = strtok(NULL, " ");
					if(token!= NULL) asinfo->min.z = atof(token);
				}	
				else if(stricmp("MAX_POSITION", szBuffer)==0)
				{
					contentElem.GetContents(szBuffer);
					token = strtok(szBuffer, " ");
					if(token!= NULL) asinfo->max.x = atof(token);
					token = strtok(NULL, " ");
					if(token!= NULL) asinfo->max.y = atof(token);
					token = strtok(NULL, " ");
					if(token!= NULL) asinfo->max.z = atof(token);
				}
				else if(stricmp("RADIUS", szBuffer)==0)
				{
					contentElem.GetContents(szBuffer);
					asinfo->radius = atof(szBuffer);
				}
				else if(stricmp("CENTER", szBuffer)==0)
				{
					contentElem.GetContents(szBuffer);
					token = strtok(szBuffer, " ");
					if(token!= NULL) asinfo->center.x = atof(token);
					token = strtok(NULL, " ");
					if(token!= NULL) asinfo->center.y = atof(token);
					token = strtok(NULL, " ");
					if(token!= NULL) asinfo->center.z = atof(token);
				}
			}
			m_AmbSndInfoList.push_back(asinfo);
		}
	}

	return true;
}

bool RBspObject::OpenDescription(const char *filename)
{
	MZFile mzf;
	if(!mzf.Open(filename, g_pFileSystem))
		return false;

	m_descfilename = filename;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;
	
	MXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer))
	{
		delete buffer;
		return false;
	}

	int iCount, i;
	MXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,RTOK_MATERIALLIST)==0)
			Open_MaterialList(&aChild); else
		if(stricmp(szTagName,RTOK_LIGHTLIST)==0)
			Open_LightList(&aChild); else
		if(stricmp(szTagName,RTOK_OBJECTLIST)==0)
			Open_ObjectList(&aChild); else
		if(stricmp(szTagName,RTOK_OCCLUSIONLIST)==0)
			Open_OcclusionList(&aChild);
		if(stricmp(szTagName,RTOK_DUMMYLIST)==0)
		{
			Open_DummyList(&aChild);
		}
		if(stricmp(szTagName,RTOK_FOG)==0)
			Set_Fog(&aChild);
		if(stricmp(szTagName,"AMBIENTSOUNDLIST")==0)
			Set_AmbSound(&aChild);
	}

	delete buffer;
	mzf.Close();

	return true;
}

/*
bool RBspObject::OpenPathNode(const char *pathfilename)
{
	if(m_PathNodes.Open(pathfilename,m_nConvexPolygon,g_pFileSystem))
	{
		mlog("%d pathnodes ",m_PathNodes.size());
		GeneratePathNodeTable();
		return true;
	}
	return false;
}
*/

bool RBspObject::OpenRs(const char *filename)
{
	MZFile file;
	if(!file.Open(filename,g_pFileSystem)) 
		return false;

	mlog("RBspObject::OpenRs : file.Open \n");

	RHEADER header;
	file.Read(&header,sizeof(RHEADER));
	if(header.dwID!=RS_ID || header.dwVersion!=RS_VERSION)
	{
		mlog("%s : %d , %d version required.\n",filename,header.dwVersion,RS_VERSION);
		file.Close();
		return false;
	}

	mlog("RBspObject::OpenRs : file.Read(&header) \n");

	// read material indices
	int nMaterial;
	file.Read(&nMaterial,sizeof(int));

	mlog("RBspObject::OpenRs : file.Read(&nMaterial) \n");

	if(m_nMaterial-1!=nMaterial)
		return false;

	for(int i=1;i<m_nMaterial;i++)
	{
		char buf[256];
		if(!ReadString(&file,buf,sizeof(buf)))
			return false;
	}

	Open_ConvexPolygons(&file);

	file.Read(&m_nBspNodeCount,sizeof(int));
	file.Read(&m_nBspPolygon,sizeof(int));
	file.Read(&m_nBspVertices,sizeof(int));
	file.Read(&m_nBspIndices,sizeof(int));

	file.Read(&m_nNodeCount,sizeof(int));
	file.Read(&m_nPolygon,sizeof(int));
	file.Read(&m_nVertices,sizeof(int));
	file.Read(&m_nIndices,sizeof(int));

	m_pOcRoot = new RSBspNode[m_nNodeCount];
	m_pOcInfo = new RPOLYGONINFO[m_nPolygon];
	m_pOcVertices = new BSPVERTEX[m_nVertices];
	m_pOcIndices = new WORD[m_nIndices];

	g_pLPNode=m_pOcRoot;
	g_pLPInfo=m_pOcInfo;
	g_nCreatingPosition=0;
	g_pLPVertices=m_pOcVertices;

	mlog("RBspObject::OpenRs : Open_Nodes begin \n");

	Open_Nodes(m_pOcRoot,&file);

	mlog("RBspObject::OpenRs : Open_Nodes end \n");
	return true;
}

bool RBspObject::OpenBsp(const char *filename)
{
	MZFile file;
	if(!file.Open(filename,g_pFileSystem)) 
		return false;

	RHEADER header;
	file.Read(&header,sizeof(RHEADER));
	if(header.dwID!=RBSP_ID || header.dwVersion!=RBSP_VERSION)
	{
		file.Close();
		return false;
	}

	int nBspNodeCount,nBspPolygon,nBspVertices,nBspIndices;
	// read tree information
	file.Read(&nBspNodeCount,sizeof(int));
	file.Read(&nBspPolygon,sizeof(int));
	file.Read(&nBspVertices,sizeof(int));
	file.Read(&nBspIndices,sizeof(int));

	if(m_nBspNodeCount!=nBspNodeCount || m_nBspPolygon!=nBspPolygon ||
		m_nBspVertices!=nBspVertices || m_nBspIndices!=nBspIndices ) 
	{
		file.Close();
		return false;
	}

	m_pBspRoot=new RSBspNode[m_nBspNodeCount];
	m_pBspInfo=new RPOLYGONINFO[m_nBspPolygon];
	m_pBspVertices=new BSPVERTEX[m_nBspVertices];
//	m_pBspIndices = new WORD[m_nBspIndices];

	g_pLPNode=m_pBspRoot;
	g_pLPInfo=m_pBspInfo;
	g_nCreatingPosition=0;
	g_pLPVertices=m_pBspVertices;
//	g_pLPIndices=m_pBspIndices;

	Open_Nodes(m_pBspRoot,&file);
	_ASSERT(m_pBspRoot+m_nBspNodeCount>g_pLPNode);
	
	file.Close();
	return true;
}

bool RBspObject::Open_ColNodes(RSolidBspNode *pNode,MZFile *pfile)
{
	pfile->Read(&pNode->m_Plane,sizeof(rplane));

	pfile->Read(&pNode->m_bSolid,sizeof(bool));

	bool flag;
	pfile->Read(&flag,sizeof(bool));
	if(flag)
	{
		g_pLPColNode++;
		pNode->m_pPositive=g_pLPColNode;
		Open_ColNodes(pNode->m_pPositive,pfile);
	}
	pfile->Read(&flag,sizeof(bool));
	if(flag)
	{
		g_pLPColNode++;
		pNode->m_pNegative=g_pLPColNode;
		Open_ColNodes(pNode->m_pNegative,pfile);
	}

	int nPolygon;
	pfile->Read(&nPolygon,sizeof(int));

#ifndef _PUBLISH
	pNode->nPolygon=nPolygon;
	if(pNode->nPolygon)
	{
		pNode->pVertices=g_pLPColVertices;
		pNode->pNormals=new rvector[pNode->nPolygon];
		g_pLPColVertices+=pNode->nPolygon*3;
		for(int i=0;i<pNode->nPolygon;i++)
		{
			pfile->Read(pNode->pVertices+i*3,sizeof(rvector)*3);
			pfile->Read(pNode->pNormals+i,sizeof(rvector));
		}
	}
#else
	
	pfile->Seek(nPolygon*4*sizeof(rvector),MZFile::current);

	/*
	// �о ������
	rvector temp;
	for(int i=0;i<nPolygon*4;i++)
		pfile->Read(&temp,sizeof(rvector));
	*/

#endif

	return true;

}

bool RBspObject::OpenCol(const char *filename)
{
	MZFile file;
	if(!file.Open(filename,g_pFileSystem)) 
		return false;

	RHEADER header;
	file.Read(&header,sizeof(RHEADER));
	if(header.dwID!=R_COL_ID || header.dwVersion!=R_COL_VERSION)
	{
		file.Close();
		return false;
	}

	int nBspNodeCount,nBspPolygon;
	// read tree information
	file.Read(&nBspNodeCount,sizeof(int));
	file.Read(&nBspPolygon,sizeof(int));

	m_pColRoot=new RSolidBspNode[nBspNodeCount];
	m_pColVertices=new rvector[nBspPolygon*3];

	g_pLPColNode=m_pColRoot;
	g_pLPColVertices=m_pColVertices;
	g_nCreatingPosition=0;

	Open_ColNodes(m_pColRoot,&file);
	_ASSERT(m_pColRoot+nBspNodeCount>g_pLPColNode);

	file.Close();
#ifndef _PUBLISH	
	m_pColRoot->ConstructBoundingBox();
#endif
	return true;

}


bool RBspObject::OpenNav(const char *filename)
{
	return m_NavigationMesh.Open(filename, g_pFileSystem);
}

bool SaveMemoryBmp(int x,int y,void *data,void **retmemory,int *nsize);

bool RBspObject::OpenLightmap()
{
//	if(m_ppLightmapTextures) return true;	// �̹� �ε�Ǿ��ִ�

	char lightmapinfofilename[_MAX_PATH];
	sprintf(lightmapinfofilename,"%s.lm",m_filename.c_str());

	int i;
//	int *pLightmapInfo=new int[m_nConvexPolygon];		// lightmap �� �ߺ��� ���ŵ� �ε���.

	MZFile file;
	if(!file.Open(lightmapinfofilename,g_pFileSystem)) return false;

	RHEADER header;
	file.Read(&header,sizeof(RHEADER));
	if(header.dwID!=R_LM_ID || header.dwVersion!=R_LM_VERSION)
	{
		file.Close();
		return false;
	}

	mlog("BspObject load lightmap : file.Read(&header)\n");

	int nSourcePolygon,nNodeCount;
	
	file.Read(&nSourcePolygon,sizeof(int));
	file.Read(&nNodeCount,sizeof(int));

//	mlog("RBspObject::OpenLightmap : nSourcePolygon = %d ,nNodeCount = %d\n",nSourcePolygon,nNodeCount);

	// ���� �ʿ��� ������ ����Ʈ������ Ȯ���� �����صа��� Ȯ���Ѵ�.
	if(nSourcePolygon!=m_nConvexPolygon || m_nNodeCount!=nNodeCount)
	{
		file.Close();
		return false;
	}

	file.Read(&m_nLightmap,sizeof(int));
	m_ppLightmapTextures=new LPDIRECT3DTEXTURE9[m_nLightmap];

	mlog("BspObject load lightmap nCount = %d\n",m_nLightmap);
	for(i=0;i<m_nLightmap;i++)
	{
		mlog("BspObject load lightmap %d\n",i);

		int nBmpSize;
		file.Read(&nBmpSize,sizeof(int));

		void *bmpmemory=new BYTE[nBmpSize];
		file.Read(bmpmemory,nBmpSize);

		m_ppLightmapTextures[i]=NULL;

		HRESULT hr=D3DXCreateTextureFromFileInMemoryEx(
			RGetDevice(),bmpmemory,nBmpSize,
			D3DX_DEFAULT, D3DX_DEFAULT, 
			D3DX_DEFAULT,
			0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
			D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
			D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
			0, NULL, NULL, &m_ppLightmapTextures[i]);

		if(hr!=D3D_OK) mlog("lightmap texture ���� ���� %s \n",DXGetErrorString9(hr));
		delete bmpmemory;
//		delete memory;
	}

	mlog("BspObject load lightmap : file.Read(&m_nLightmap) done\n");

	// ����ɶ��� ������ ������ �д´�
	int *pOrder=new int[m_nPolygon];
	file.Read(pOrder,sizeof(int)*m_nPolygon);

	// ����ɶ��� ������ ������ �����Ͽ� ������� ����Ʈ�� ��ȣ�� �����Ѵ�
	for(i=0;i<m_nPolygon;i++)
		file.Read(&(m_pOcInfo+pOrder[i])->nLightmapTexture,sizeof(int));
	delete pOrder;

//	mlog("RBspObject::OpenLightmap : file.Read(&(m_pOcInfo) \n");

	// ����Ʈ�� uv ��ǥ�� �д´�.
	for(i=0;i<m_nVertices;i++)
		file.Read(&(m_pOcVertices+i)->tu2,sizeof(float)*2);

//	mlog("RBspObject::OpenLightmap : file.Read(&(m_pOcVertices) \n");

	file.Close();

	mlog("BspObject load lightmap : end \n");

	return true;
}

bool RBspObject::Open_ConvexPolygons(MZFile *pfile)
{
	int nConvexVertices;//,nSourcePolygon;

	pfile->Read(&m_nConvexPolygon,sizeof(int));
	pfile->Read(&nConvexVertices,sizeof(int));

	// �� �������� ����Ʈ���� �����Ҷ� �ʿ��ϹǷ� ��Ÿ�ӿ����� �����ʿ䰡 ����

	if(m_OpenMode==ROF_RUNTIME) {
		for(int i=0;i<m_nConvexPolygon;i++)
		{
			pfile->Seek(sizeof(int)+sizeof(DWORD)+sizeof(rplane)+sizeof(float),MZFile::current);
			
			int nVertices;
			pfile->Read(&nVertices,sizeof(int));

			pfile->Seek(nVertices*2*sizeof(rvector),MZFile::current);
		}
		return true;
	}

	m_pConvexPolygons=new RCONVEXPOLYGONINFO[m_nConvexPolygon];
	m_pConvexVertices=new rvector[nConvexVertices];
	m_pConvexNormals=new  rvector[nConvexVertices];

	rvector *pLoadingVertex=m_pConvexVertices;
	rvector *pLoadingNormal=m_pConvexNormals;

	for(int i=0;i<m_nConvexPolygon;i++)
	{
		pfile->Read(&m_pConvexPolygons[i].nMaterial,sizeof(int));
		// 0���� ������ ����.. 1�� default material��.. (���Ͽ��� -1�� ����Ǿ��ִ�)
		m_pConvexPolygons[i].nMaterial+=2;
		pfile->Read(&m_pConvexPolygons[i].dwFlags,sizeof(DWORD));
		pfile->Read(&m_pConvexPolygons[i].plane,sizeof(rplane));
		pfile->Read(&m_pConvexPolygons[i].fArea,sizeof(float));
		pfile->Read(&m_pConvexPolygons[i].nVertices,sizeof(int));

		m_pConvexPolygons[i].pVertices=pLoadingVertex;
		for(int j=0;j<m_pConvexPolygons[i].nVertices;j++)
		{
			pfile->Read(pLoadingVertex,sizeof(rvector));
			pLoadingVertex++;
		}
		m_pConvexPolygons[i].pNormals=pLoadingNormal;
		for(int j=0;j<m_pConvexPolygons[i].nVertices;j++)
		{
			pfile->Read(pLoadingNormal,sizeof(rvector));
			pLoadingNormal++;
		}
	}
	return true;
}

void RBspObject::CreatePolygonTableAndIndexBuffer()
{
	g_pLPIndices=m_pOcIndices;
	CreatePolygonTableAndIndexBuffer(m_pOcRoot);
}

/// ���� material ���� ���ִ°��� ���̺�� ����� �������� ���� �ε��� ���۸� �����Ѵ�
void RBspObject::CreatePolygonTableAndIndexBuffer(RSBspNode *pNode)
{
	if(pNode->m_pPositive)
		CreatePolygonTableAndIndexBuffer(pNode->m_pPositive);

	if(pNode->m_pNegative)
		CreatePolygonTableAndIndexBuffer(pNode->m_pNegative);

	if(pNode->nPolygon)
	{
		for(int i=0;i<pNode->nPolygon;i++)
		{
			WORD *pInd = g_pLPIndices;
			RPOLYGONINFO *pInfo = pNode->ppInfoSorted[i];

			pInfo->nIndicesPos = g_pLPIndices - m_pOcIndices;
			if(RIsHardwareTNL())
			{
//				WORD base = pInfo->pVertices - m_pOcVertices;
				WORD base = pInfo->pVertices - ( m_pOcVertices + pNode->m_nBaseVertexIndex );
				for(int j=0;j<pInfo->nVertices-2;j++)
				{
					*pInd++ = base + 0; 
					*pInd++ = base + j+1; 
					*pInd++ = base + j+2; 
				}
			}
			g_pLPIndices+=(pInfo->nVertices-2)*3;
		}

		int nCount=m_nMaterial*max(1,m_nLightmap);

		SAFE_DELETE_ARRAY(pNode->pDrawInfo);
		pNode->pDrawInfo = new RDrawInfo[nCount];

		int lastmatind=pNode->ppInfoSorted[0]->nIndicesPos;
		int lastmat=pNode->ppInfoSorted[0]->nMaterial+pNode->ppInfoSorted[0]->nLightmapTexture*m_nMaterial;
		int nTriangle=pNode->ppInfoSorted[0]->nVertices-2;
		int lastj=0;

		for(int j=1;j<pNode->nPolygon+1;j++)
		{
			int nMatIndex = (j==pNode->nPolygon) ? -999 :	// �������̸� ��������
				pNode->ppInfoSorted[j]->nMaterial+pNode->ppInfoSorted[j]->nLightmapTexture*m_nMaterial;
			
			if(nMatIndex!=lastmat)
			{
				RDrawInfo *pdi = &pNode->pDrawInfo[lastmat];

				_ASSERT(lastmat!=-1 && lastmat>=0 && lastmat<nCount);

				pdi->nIndicesOffset=lastmatind;
				pdi->nTriangleCount=nTriangle;
				pdi->pPlanes = new rplane[nTriangle];
				pdi->pUAxis = new rvector[nTriangle];
				pdi->pVAxis = new rvector[nTriangle];

				int nPlaneIndex = 0;
				for(int k=lastj;k<j;k++)
				{
					for(int l=0;l<pNode->ppInfoSorted[k]->nVertices-2;l++)
					{
						rplane *pPlane = &pNode->ppInfoSorted[k]->plane;
						pdi->pPlanes[nPlaneIndex] = *pPlane;
						rvector normal = rvector(pPlane->a,pPlane->b,pPlane->c);

						rvector up;
						if(fabs(DotProduct(normal,rvector(1,0,0)))<0.01) {
							up = rvector(1,0,0);
						}else
							up = rvector(0,1,0);

						rvector au;
						CrossProduct(&au,up,normal);
						Normalize(au);
						rvector av;
						CrossProduct(&av,au,normal);
						Normalize(av);

						pdi->pUAxis[nPlaneIndex] = au;
						pdi->pVAxis[nPlaneIndex] = av;

						nPlaneIndex++;
					}
				}
				_ASSERT(nPlaneIndex==pdi->nTriangleCount);

				if(!RIsHardwareTNL())
				{
//					/*
					for(int k=lastj;k<j;k++)
					{
						pdi->nVertice+=pNode->ppInfoSorted[k]->nVertices;
					}

					pdi->pVertices=new BSPVERTEX[pdi->nVertice];

					WORD base = 0;
					for(int k=lastj;k<j;k++)
					{
						memcpy(pdi->pVertices+base,pNode->ppInfoSorted[k]->pVertices,sizeof(BSPVERTEX)*pNode->ppInfoSorted[k]->nVertices);

						WORD *pInd = m_pOcIndices + pNode->ppInfoSorted[k]->nIndicesPos;
						for(int l=0;l<pNode->ppInfoSorted[k]->nVertices-2;l++)
						{
							*pInd++ = base + 0; 
							*pInd++ = base + l+1; 
							*pInd++ = base + l+2;
						}
						base+=pNode->ppInfoSorted[k]->nVertices;
					}
				}

				if(j==pNode->nPolygon) break;

				lastmat=nMatIndex;
				lastmatind=pNode->ppInfoSorted[j]->nIndicesPos;
				nTriangle=0;
				lastj=j;
			}
			nTriangle+=pNode->ppInfoSorted[j]->nVertices-2;
		}
	}
}

void RBspObject::Sort_Nodes(RSBspNode *pNode)
{
	if(pNode->m_pPositive)
		Sort_Nodes(pNode->m_pPositive);
	
	if(pNode->m_pNegative)
		Sort_Nodes(pNode->m_pNegative);

	if(pNode->nPolygon)
	{
		// ����Ʈ��&material ���� sort �� �Ѵ�..
		
		for(int j=0;j<pNode->nPolygon-1;j++)
		{
			for(int k=j+1;k<pNode->nPolygon;k++)
			{
				RPOLYGONINFO *pj=pNode->ppInfoSorted[j],*pk=pNode->ppInfoSorted[k];

				if(pj->nLightmapTexture*m_nMaterial + pj->nMaterial > pk->nLightmapTexture*m_nMaterial + pk->nMaterial )
				{
					pNode->ppInfoSorted[j] = pk;
					pNode->ppInfoSorted[k] = pj;
				}
			}
		}
	}
}

bool RBspObject::Open_Nodes(RSBspNode *pNode,MZFile *pfile)
{
	pfile->Read(&pNode->bbTree,sizeof(rboundingbox));
	pfile->Read(&pNode->plane,sizeof(rplane));

	bool flag;
	pfile->Read(&flag,sizeof(bool));
	if(flag)
	{
		g_pLPNode++;
		pNode->m_pPositive=g_pLPNode;
		Open_Nodes(pNode->m_pPositive,pfile);
	}
	pfile->Read(&flag,sizeof(bool));
	if(flag)
	{
		g_pLPNode++;
		pNode->m_pNegative=g_pLPNode;
		Open_Nodes(pNode->m_pNegative,pfile);
	}

	pfile->Read(&pNode->nPolygon,sizeof(int));

	// 16bit index �� ���� ������ ���� ������ �� ������ �ɾ����ϴ�.
	// ���� �̰� �߻��Ѵٸ� �ͽ���Ʈ�� �� ������ ���� ���� �ʵ��� ������ ��带 �ɰ��� �մϴ�.
	if( pNode->nPolygon >= (65535/3) ) {
		_ASSERT(FALSE);
		return false;
	}

	if(pNode->nPolygon)
	{
		pNode->ppInfoSorted = new RPOLYGONINFO *[pNode->nPolygon];
		pNode->pInfo=g_pLPInfo;g_pLPInfo+=pNode->nPolygon;
		pNode->m_nBaseVertexIndex = g_pLPVertices - m_pOcVertices;

		RPOLYGONINFO *pInfo=pNode->pInfo;

		int i;
		for(i=0;i<pNode->nPolygon;i++)
		{
			pNode->ppInfoSorted[i] = pInfo;
			int mat;

			rvector c1,c2,c3,nor;

			pfile->Read(&mat,sizeof(int));
			pfile->Read(&pInfo->nConvexPolygon,sizeof(int));
			pfile->Read(&pInfo->dwFlags,sizeof(DWORD));
			pfile->Read(&pInfo->nVertices,sizeof(int));


			BSPVERTEX *pVertex = pInfo->pVertices = g_pLPVertices;

			// normal �� �о ������
			for(int j=0;j<pInfo->nVertices;j++)
			{
				rvector normal;
				pfile->Read(&pVertex->x,sizeof(float)*3);
				pfile->Read(&normal,sizeof(float)*3);
				pfile->Read(&pVertex->tu1,sizeof(float)*4);
				pVertex++;
			}
			// */

			// normal �� �д´�			
			//pfile->Read(pVertex,pInfo->nVertices*sizeof(BSPVERTEX));


			g_pLPVertices+=pInfo->nVertices;


			pfile->Read(&nor,sizeof(rvector));
//			Normalize(nor);
			pInfo->plane.a=nor.x;
			pInfo->plane.b=nor.y;
			pInfo->plane.c=nor.z;
			pInfo->plane.d=-DotProduct(nor,*pInfo->pVertices[0].Coord());

			if((pInfo->dwFlags & RM_FLAG_HIDE)!=0)
				pInfo->nMaterial=-1;
			else
			{
				int nMaterial=mat+1;		// -1 �� 0���� ...

				// ���� �̷����� ������ ���� ��������Ʈ������ �߰� -_-;
				if(nMaterial<0 || nMaterial>=m_nMaterial) nMaterial=0;	

				pInfo->nMaterial=nMaterial;
				pInfo->dwFlags|=m_pMaterials[nMaterial].dwFlags;
			}
			_ASSERT(pInfo->nMaterial<m_nMaterial);
			pInfo->nPolygonID=g_nCreatingPosition;
			pInfo->nLightmapTexture=0;
			
			pInfo++;
			g_nCreatingPosition++;
		}

		pNode->m_nVertices = (g_pLPVertices - m_pOcVertices) - pNode->m_nBaseVertexIndex;
	}

	return true;
}

bool RBspObject::CreateVertexBuffer()
{
	if(/*m_nPolygon*3 > 65530 || */m_nPolygon==0 ) return false;

	g_nCreatingPosition=0;

	SAFE_RELEASE(m_pVertexBuffer);

	HRESULT hr=RGetDevice()->CreateVertexBuffer( sizeof(BSPVERTEX)*m_nVertices, D3DUSAGE_WRITEONLY , BSP_FVF, D3DPOOL_MANAGED, &m_pVertexBuffer ,NULL);
	_ASSERT(hr==D3D_OK);
	if(hr!=D3D_OK) return false;

	return true;
}

bool RBspObject::UpdateVertexBuffer()
{
	if(!m_pVertexBuffer) return false;

	LPBYTE pVer=NULL;
	HRESULT hr=m_pVertexBuffer->Lock(0,0,(VOID**)&pVer,0);
	_ASSERT(hr==D3D_OK);
	memcpy(pVer,m_pOcVertices,sizeof(BSPVERTEX)*m_nVertices);
	hr=m_pVertexBuffer->Unlock();
	
	return true;
}


/*
void RBspObject::DrawPathNode()
{
    RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RPathList::iterator i=m_PathNodes.begin();
	while(i!=m_PathNodes.end()){
		RPathNode* pNode=*i;

		(*i)->DrawWireFrame(0x808080);
		i++;
	}
}
*/

/*
bool RBspObject::PickPathNode(int x,int y,RPathNode **pOut,rvector *ColPos)
{
	if(!m_pBspRoot) return false;

	rvector scrpoint=rvector((float)x,(float)y,0.1f);

	rmatrix inv;	
	float det;
	D3DXMatrixInverse(&inv,&det,&RViewProjectionViewport);

	rvector worldpoint;
	D3DXVec3TransformCoord(&worldpoint,&scrpoint,&inv);

	g_PickOrigin=RCameraPosition;
	g_PickDir=worldpoint-RCameraPosition;
	Normalize(g_PickDir);

	g_bPickFound=false;
	g_pPickOutPathNode=pOut;
	g_pColPos=ColPos;
	D3DXPlaneFromPointNormal(&g_PickPlane,&g_PickOrigin,&g_PickDir);
	return PickPathNode(m_pBspRoot);
}

bool RBspObject::PickPathNode(rvector &from,rvector &to,RPathNode **pOut,rvector *ColPos)
{
	if(!m_pBspRoot) return false;

	g_PickOrigin=from;
	g_PickDir=to-from;
	Normalize(g_PickDir);

	g_bPickFound=false;
	g_pPickOutPathNode=pOut;
	g_pColPos=ColPos;
	D3DXPlaneFromPointNormal(&g_PickPlane,&g_PickOrigin,&g_PickDir);
	return PickPathNode(m_pBspRoot);
}

// ������ ���� ������쿡�� ���� ����� node �� ã�ƾ� �Ѵ�....
bool RBspObject::PickPathNode(RSBspNode *pNode)
{
	if(pNode->nPolygon){
		for(int i=0;i<pNode->nPolygon;i++)
		{
			float fDist;
			BSPVERTEX *pVer=pNode->pVertices+i*3;
			RPathNode *pPathNode=GetPathNode(pNode,i);
			if( (pNode->pInfo[i].dwFlags & RM_FLAG_ADDITIVE) == 0 && pPathNode &&
				IsIntersect(g_PickOrigin,g_PickDir,*pVer->Coord(),*(pVer+2)->Coord(),*(pVer+1)->Coord(),&fDist))
			{
				rvector pickto=g_PickOrigin+g_PickDir;

				rvector pos;
				D3DXPlaneIntersectLine(&pos,&pNode->pInfo[i].plane,&g_PickOrigin,&pickto);

				if(D3DXPlaneDotCoord(&g_PickPlane,&pos)<0)
					return false;

				*g_pColPos=pos;
				*g_pPickOutPathNode=pPathNode;

				return true;  // �ٷθ����ϸ�ȵǰ�.. ���� �� ã�ƺ��� �Ѵ� .. �ϴ� �����ڵ尡 �ƴϹǷ� ���� -_-;
			}
		}
	}
	else
		if(isLineIntersectBoundingBox(g_PickOrigin,g_PickDir,pNode->bbTree))// && isInPlane(&bbTree,&g_PickPlane))
		{
			if(D3DXPlaneDotCoord(&pNode->plane,&g_PickOrigin)>0)
				if(PickPathNode(pNode->m_pPositive)) return true;
				else
					return PickPathNode(pNode->m_pNegative);
			else
				if(PickPathNode(pNode->m_pNegative)) return true;
				else
					return PickPathNode(pNode->m_pPositive);
		}

		return false;
}

bool RBspObject::GeneratePathData(const char *filename,float fAngle,float fToler)
{
	int i,j;

	m_PathNodes.DeleteAll();
	for(i=0;i<m_nConvexPolygon;i++)
	{
		if(sin(fAngle)<fabs(m_pConvexPolygons[i].plane.c) && (m_pConvexPolygons[i].dwFlags & RM_FLAG_NOTWALKABLE)==0 )	// normal �� z ��
		{
			RPathNode *ppathnode=new RPathNode;
			ppathnode->m_nSourceID=i;
			ppathnode->m_nGroupID=-1;
			ppathnode->plane=m_pConvexPolygons[i].plane;
			for(j=0;j<m_pConvexPolygons[i].nVertices;j++)
			{
				ppathnode->vertices.push_back(new rvector(m_pConvexPolygons[i].pVertices[j]));
			}
			m_PathNodes.push_back(ppathnode);
		}
	}

	m_PathNodes.ConstructNeighborhood();
	m_PathNodes.ConstructGroupIDs();
	
	GeneratePathNodeTable();
	return m_PathNodes.Save(filename,m_nConvexPolygon);
}

void RBspObject::GeneratePathNodeTable()
{
	for(int i=0;i<m_nConvexPolygon;i++)
		m_pConvexPolygons[i].nPathNode=-1;

	{
		for(size_t i=0;i<m_PathNodes.size();i++)
		{
			m_pConvexPolygons[m_PathNodes[i]->m_nSourceID].nPathNode=i;
		}
	}
}

RPathNode *RBspObject::GetPathNode(RSBspNode *pNode,int index)
{	
	int nConvexPolygon=pNode->pInfo[index].nConvexPolygon;
	if(nConvexPolygon<0) return NULL;
	int nNode=m_pConvexPolygons[nConvexPolygon].nPathNode;
	if(nNode<0) return NULL;
	return m_PathNodes[nNode];
}
*/

void RBspObject::GetNormal(RCONVEXPOLYGONINFO *poly,rvector &position,rvector *normal,int au,int av)
{

	int nSelPolygon=-1,nSelEdge=-1;
	float fMinDist=FLT_MAX;

	if(poly->nVertices==3)
		nSelPolygon=0;
	else
	{
		// ���� ������ȿ� ������ ������, ���� ����� edge �� ���� �������� �������� ����Ѵ�.
		rvector pnormal(poly->plane.a,poly->plane.b,poly->plane.c);

		for(int i=0;i<poly->nVertices-2;i++)
		{
			float t;
			rvector *a=&poly->pVertices[0];
			rvector *b=&poly->pVertices[i+1];
			rvector *c=&poly->pVertices[i+2];


			if(IsIntersect(position+pnormal,-pnormal,*a,*b,*c,&t))
			{
				nSelPolygon=i;
				nSelEdge=-1;
				break;
			}else
			{
				float dist=GetDistance(position,*a,*b);
				if(dist<fMinDist) {fMinDist=dist;nSelPolygon=i;nSelEdge=0;}
				dist=GetDistance(position,*b,*c);
				if(dist<fMinDist) {fMinDist=dist;nSelPolygon=i;nSelEdge=1;}
				dist=GetDistance(position,*c,*a);
				if(dist<fMinDist) {fMinDist=dist;nSelPolygon=i;nSelEdge=2;}
			}
		}
	}

	rvector *v0=&poly->pVertices[0];
	rvector *v1=&poly->pVertices[nSelPolygon+1];
	rvector *v2=&poly->pVertices[nSelPolygon+2];

	rvector *n0=&poly->pNormals[0];
	rvector *n1=&poly->pNormals[nSelPolygon+1];
	rvector *n2=&poly->pNormals[nSelPolygon+2];

	rvector pos;
	if(nSelEdge!=-1)
	{
		// �������� ��� ���� ���ؼ��� ���� ����� edge���� ���� �����Ѵ�

		rvector *e0 = nSelEdge==0 ? v0 : nSelEdge==1 ? v1 : v2;
		rvector *e1 = nSelEdge==0 ? v1 : nSelEdge==1 ? v2 : v0;

		rvector dir=*e1-*e0;
		Normalize(dir);

		pos= *e0 + DotProduct(dir,position-*e0)*dir;
	}
	else
		pos=position;

	rvector a,b,x,tem;

	a=*v1-*v0;
	b=*v2-*v1;
	x=pos-*v0;

	float f=b[au]*x[av]-b[av]*x[au];
//	_ASSERT(!IS_ZERO(f));
	if(IS_ZERO(f))
	{
		*normal=*n0;
		return;
	}
	float t=(a[av]*x[au]-a[au]*x[av])/f;

	tem=InterpolatedVector(*n1,*n2,t);


	rvector inter=a+t*b;

	int axisfors;
	if(fabs(inter.x)>fabs(inter.y) && fabs(inter.x)>fabs(inter.z)) axisfors=0;
	else
		if(fabs(inter.y)>fabs(inter.z)) axisfors=1;
		else axisfors=2;

	float s=x[axisfors]/inter[axisfors];
//*/
	*normal=InterpolatedVector(*n0,tem,s);
}


bool RBspObject::GenerateLightmap(const char *filename,int nMaxlightmapsize,int nMinLightmapSize,int nSuperSample,float fToler,RGENERATELIGHTMAPCALLBACK pProgressFn)
{
	bool bReturnValue=true;

	ClearLightmaps();

	int i,j,k,l;

	float fMaximumArea=0;

	// �ִ� ������ ���� �������� ã�´�.
	for(i=0;i<m_nConvexPolygon;i++)
	{
		fMaximumArea=max(fMaximumArea,m_pConvexPolygons[i].fArea);
	}

	int nConstCount=0;
	int nLight;
	RLIGHT **pplight;
	pplight=new RLIGHT*[m_StaticMapLightList.size()];
	rvector *lightmap=new rvector[nMaxlightmapsize*nMaxlightmapsize];
	DWORD	*lightmapdata=new DWORD[nMaxlightmapsize*nMaxlightmapsize];
	bool *isshadow=new bool[(nMaxlightmapsize+1)*(nMaxlightmapsize+1)];
	int	*pSourceLightmap=new int[m_nConvexPolygon];
	map<DWORD,int> ConstmapTable;

	vector<RLIGHTMAPTEXTURE*> sourcelightmaplist;

	RHEADER header(R_LM_ID,R_LM_VERSION);

	for(i=0;i<m_nConvexPolygon;i++)
	{
		RCONVEXPOLYGONINFO *poly=m_pConvexPolygons+i;

		// progress bar �����Ѵ�. cancel �Ǿ����� ������.
		if(pProgressFn)
		{
			bool bContinue=pProgressFn((float)i/(float)m_nConvexPolygon);
			if(!bContinue)
			{
				bReturnValue=false;
				goto clearandexit;
			}
		}

		rboundingbox bbox;	// �ٿ���ڽ��� ã�´�..

		bbox.vmin=bbox.vmax=poly->pVertices[0];
		for(j=1;j<poly->nVertices;j++)
		{
			for(k=0;k<3;k++)
			{
				bbox.vmin[k]=min(bbox.vmin[k],poly->pVertices[j][k]);
				bbox.vmax[k]=max(bbox.vmax[k],poly->pVertices[j][k]);
			}
		}

		int lightmapsize; //����Ʈ���� size

		/*
		if(m_pMaterials[m_pSourceMaterial[i]]->dwFlags!=0)		// add ����̸� ������ white 
		{
			lightmapsize=2;
			lightmapdata[0]=lightmapdata[1]=lightmapdata[2]=lightmapdata[3]=0xffffffff;
		}
		else

		*/
		{
			// ������ ����Ʈ���� size �� �����Ѵ�.
			lightmapsize=nMaxlightmapsize;

			float targetarea=fMaximumArea/4.f;
			while(poly->fArea<targetarea && lightmapsize>nMinLightmapSize)
			{
				targetarea/=4.f;
				lightmapsize/=2;
			}
			
			rvector diff=float(lightmapsize)/float(lightmapsize-1)*(bbox.vmax-bbox.vmin);

			// 1 texel �� ������ �����.
			for(k=0;k<3;k++)
			{
				bbox.vmin[k]-=.5f/float(lightmapsize)*diff[k];
				bbox.vmax[k]+=.5f/float(lightmapsize)*diff[k];
			}

			rvector pnormal=rvector(poly->plane.a,poly->plane.b,poly->plane.c);
	
			RBSPMATERIAL *pMaterial = &m_pMaterials[m_pConvexPolygons[i].nMaterial];

			rvector ambient=pMaterial->Ambient;

			// �� �����￡ ������ �ٸ��� �������� ��������..
			nLight=0;

			
			for(RLightList::iterator light=m_StaticMapLightList.begin();light!=m_StaticMapLightList.end();light++){
				
				// ��ȿ�Ÿ��� ����� ������ ����
				if(GetDistance((*light)->Position,poly->plane)>(*light)->fAttnEnd) continue;

				for(int iv=0;iv<poly->nVertices;iv++)
				{
					// �ϳ��� ���� �޴� normal �� �ִٸ�
					if(DotProduct((*light)->Position-poly->pVertices[iv],poly->pNormals[iv])>0) {
						pplight[nLight++]=*light;
						break;
					}

				}
			}
			

			int au,av,ax; // ���� index    ���� �ؽ��Ŀ����� u��, v��, �׸��� ������ ����..

			if(fabs(poly->plane.a)>fabs(poly->plane.b) && fabs(poly->plane.a)>fabs(poly->plane.c) )
				ax=0;   // yz ������� projection	u �� y �࿡ v �� z �࿡ ����, ���������� ax�� x ��
			else if(fabs(poly->plane.b)>fabs(poly->plane.c))
				ax=1;	// xz ������� ...
			else
				ax=2;	// xy ������� ...

			au=(ax+1)%3;
			av=(ax+2)%3;

			for(j=0;j<lightmapsize;j++)			// v 
			{
				for(k=0;k<lightmapsize;k++)		// u
				{
//					lightmap[j*lightmapsize+k]=rvector(0,0,0);
					lightmap[j*lightmapsize+k]=m_AmbientLight;
						//ambient;
				}
			}

			for(l=0;l<nLight;l++)
			{
				RLIGHT *plight=pplight[l];

				// �׸��ڰ� �Ǵ� ������� �̸� ���̺�(isshadow)�� ����س��´�..
				for(j=0;j<lightmapsize+1;j++)			// v 
				{
					for(k=0;k<lightmapsize+1;k++)		// u
					{
						isshadow[k*(lightmapsize+1)+j]=false;
						if((plight->dwFlags & RM_FLAG_CASTSHADOW)==0 ||
							(poly->dwFlags & RM_FLAG_RECEIVESHADOW)==0) continue;
						_ASSERT(plight->dwFlags ==16);

						rvector position;
						position[au]=bbox.vmin[au]+((float)k/(float)lightmapsize)*diff[au];
						position[av]=bbox.vmin[av]+((float)j/(float)lightmapsize)*diff[av];
						// ����� �����Ŀ� ���� ������ �� ��ǥ�� �����ȴ�.
						position[ax]=(-poly->plane.d-pnormal[au]*position[au]-pnormal[av]*position[av])/pnormal[ax];

						float fDistanceToPolygon=Magnitude(position-plight->Position);

						RBSPPICKINFO bpi;
						if(PickShadow(plight->Position,position,&bpi)) 
						{
							float fDistanceToPickPos=Magnitude(bpi.PickPos-plight->Position);

							if(fDistanceToPolygon>fDistanceToPickPos+fToler) 
								isshadow[k*(lightmapsize+1)+j]=true;
						}

						// mesh �� �׸��������� �˻��ϴ� �κ�
						/*
						{
							for(RMapObjectList::iterator i=m_ObjectList.begin();i!=m_ObjectList.end();i++)
							{
								ROBJECTINFO *poi=*i;

								if(!poi->pVisualMesh) continue;
								float t;

								// �ٿ���ڽ��� ����ǥ��� �Ǿ������Ƿ� ���� ������ untransform �Ѵ�.
								rmatrix inv;	
								float det;
								D3DXMatrixInverse(&inv,&det,&poi->pVisualMesh->m_WorldMat);
//								rvector origin=plight->Position*inv;
//								rvector target=position*inv;

								rvector origin;
								D3DXVec3TransformCoord(&origin,&plight->Position,&inv);
								rvector target;
								D3DXVec3TransformCoord(&target,&position,&inv);

								rvector dir=target-origin;
								rvector dirorigin=position-plight->Position;

								rvector vOut;

								BOOL bBBTest=D3DXBoxBoundProbe(&poi->pVisualMesh->m_vBMin,&poi->pVisualMesh->m_vBMax,&origin,&dir);
								if( // �ٿ���ڽ��׽�Ʈ��
									bBBTest &&
									// pick �Ǿ�����..
									poi->pVisualMesh->Pick(plight->Position,dirorigin,&vOut,&t))
								{
									rvector PickPos=plight->Position+vOut*t;
									// ���� ��ġ�� �����ϴ� ���� �� �̻��ؼ� �ּ�ó���ص�.
									//float fDistanceToPickPos=Magnitude(PickPos-plight->Position);
									//if(fDistanceToPolygon>fDistanceToPickPos+fToler)
										isshadow[k*(lightmapsize+1)+j]=true;
								}
							}
						}
						// mesh �׸��� �˻� ��
						//*/
					}
				}


				for(j=0;j<lightmapsize;j++)			// v 
				{
					for(k=0;k<lightmapsize;k++)		// u
					{
						rvector color=rvector(0,0,0);

						// ���� ���� ������ ���̿� ������ �ִ°��� �׸������°��.. 
						// �׳� ó���ϸ�, ����Ʈ�ʿ� ��������� ����Ƿ� �̰�쿣 supersample �� �ʿ��ϴ�..
						// �� �����̰� �׸��ڰ� �ƴѰ��� �׸����� ��찡 ���� ������
						// �ٷ� �̰��� supersample �� �ʿ��ϴ�..

						int nShadowCount=0;

						for(int m=0;m<4;m++)
						{
							if(isshadow[(k+m%2)*(lightmapsize+1)+j+m/2])
								nShadowCount++;
						}


						if(nShadowCount<4)
						{
							if(nShadowCount>0)		// super sample
//							if(1)
							{
								int m,n;
								rvector tempcolor=rvector(0,0,0);

								//*
								
								for(m=0;m<nSuperSample;m++)
								{
									for(n=0;n<nSuperSample;n++)
									{
										rvector position;
										position[au]=bbox.vmin[au]+(((float)k+((float)n+.5f)/(float)nSuperSample)/(float)lightmapsize)*diff[au];
										position[av]=bbox.vmin[av]+(((float)j+((float)m+.5f)/(float)nSuperSample)/(float)lightmapsize)*diff[av];
										// ����� �����Ŀ� ���� ������ �� ��ǥ�� �����ȴ�.
										position[ax]=(-poly->plane.d-pnormal[au]*position[au]-pnormal[av]*position[av])/pnormal[ax];

										bool bShadow = false;

										float fDistanceToPolygon=Magnitude(position-plight->Position);

										RBSPPICKINFO bpi;
										if(PickShadow(plight->Position,position,&bpi))
										{
											float fDistanceToPickPos=Magnitude(bpi.PickPos-plight->Position);
											if(fDistanceToPolygon>fDistanceToPickPos+fToler) 
												bShadow = true;
										}

										if(!bShadow)
										{
											rvector dpos=plight->Position-position;
											float fdistance=Magnitude(dpos);
											float fIntensity=(fdistance-plight->fAttnStart)/(plight->fAttnEnd-plight->fAttnStart);
											fIntensity=min(max(1.0f-fIntensity,0),1);
											Normalize(dpos);

											rvector normal;
											GetNormal(poly,position,&normal,au,av);

											float fDot;
											fDot=DotProduct(dpos,normal);
											fDot=max(0,fDot);

											tempcolor+=fIntensity*plight->fIntensity*fDot*plight->Color;
										}
									}
								}
								tempcolor*=1.f/(nSuperSample*nSuperSample);
								//*/
								color+=tempcolor;
							}
							else					// �׸��ڰ� ���°��� �ؼ��� �߰� ��ǥ�� �ѹ��� ����Ѵ�..
							{
								rvector position;
								position[au]=bbox.vmin[au]+(((float)k+.5f)/(float)lightmapsize)*diff[au];
								position[av]=bbox.vmin[av]+(((float)j+.5f)/(float)lightmapsize)*diff[av];
								// ����� �����Ŀ� ���� ������ �� ��ǥ�� �����ȴ�.
								position[ax]=(-poly->plane.d-pnormal[au]*position[au]-pnormal[av]*position[av])/pnormal[ax];

								rvector dpos=plight->Position-position;
								float fdistance=Magnitude(dpos);
								float fIntensity=(fdistance-plight->fAttnStart)/(plight->fAttnEnd-plight->fAttnStart);
								fIntensity=min(max(1.0f-fIntensity,0),1);
								Normalize(dpos);
								
								rvector normal;
								GetNormal(poly,position,&normal,au,av);
								
								float fDot;
								fDot=DotProduct(dpos,normal);
								fDot=max(0,fDot);

								color+=fIntensity*plight->fIntensity*fDot*plight->Color;
							}
						}

						lightmap[j*lightmapsize+k]+=color;
					}
				}
			}

			// 1,1,1 �� �Ѿ�� Ŀ�����ش�..

			for(j=0;j<lightmapsize*lightmapsize;j++)
			{
				rvector color=lightmap[j];

				color*=0.25f;
				color.x=min(color.x,1);
				color.y=min(color.y,1);
				color.z=min(color.z,1);
				lightmap[j]=color;
				lightmapdata[j]=((DWORD)(color.x*255))<<16 | ((DWORD)(color.y*255))<<8 | ((DWORD)(color.z*255));
			}

			/*
			// supersample �ε� �ذ���� �ʴ� ���� �ִ�.. �׷���
			// ���� ����ش�.. 1�ȼ��� ������ �����Ƿ�.. ��� 
			
			//      1 2 1
			//      2 4 2
			//      1 2 1

			for(j=1;j<lightmapsize-1;j++)
			{
				for(k=1;k<lightmapsize-1;k++)
				{
					rvector color=lightmap[j*lightmapsize+k]*4;
					color+=(lightmap[(j-1)*lightmapsize+k]+lightmap[j*lightmapsize+k-1]+
							lightmap[(j+1)*lightmapsize+k]+lightmap[j*lightmapsize+k+1])*2;
					color+=	lightmap[(j-1)*lightmapsize+(k-1)]+lightmap[(j-1)*lightmapsize+k+1]+
							lightmap[(j+1)*lightmapsize+(k-1)]+lightmap[(j+1)*lightmapsize+k+1];
					color/=16.f;

					lightmapdata[j*lightmapsize+k]=((DWORD)(color.x*255))<<16 | ((DWORD)(color.y*255))<<8 | ((DWORD)(color.z*255));
				}
			}
			*/
		}

		bool bConstmap=true;
		for(j=0;j<lightmapsize*lightmapsize;j++)
		{
			if(lightmapdata[j]!=lightmapdata[0])
			{
				bConstmap=false;
				nConstCount++;
				break;
			}
		}

		bool bNeedInsert=true;
		if(bConstmap)
		{
			// ����̸� ũ�⸦ 2x2�� �ٲ۴�.. ��? �޾Ƽ� -_-;; �׳� �ٲ㵵 �������.
			lightmapsize=2;

			map<DWORD,int>::iterator it=ConstmapTable.find(lightmapdata[0]);
			if(it!= ConstmapTable.end())
			{
				pSourceLightmap[i]=(*it).second;
				bNeedInsert=false;
			}
		}

		if(bNeedInsert)
		{
			int nLightmap=sourcelightmaplist.size();

			pSourceLightmap[i]=nLightmap;
			if(bConstmap)
				ConstmapTable.insert(map<DWORD,int>::value_type(lightmapdata[0],nLightmap));

#ifdef GENERATE_TEMP_FILES   // ���Ϸ� ����Ʈ���� �����Ѵ�.
			
			if(i<100)	// 100�������� -_-;
			{
				char lightmapfilename[256];
				sprintf(lightmapfilename,"%s%d.bmp",m_filename.c_str(),nLightmap);
				RSaveAsBmp(lightmapsize,lightmapsize,lightmapdata,lightmapfilename);
			}
#endif			//*/

			RLIGHTMAPTEXTURE *pnew=new RLIGHTMAPTEXTURE;
			pnew->bLoaded=false;
			pnew->nSize=lightmapsize;
			pnew->data=new DWORD[lightmapsize*lightmapsize];
			memcpy(pnew->data,lightmapdata,lightmapsize*lightmapsize*sizeof(DWORD));
			sourcelightmaplist.push_back(pnew);
		}
	}

	// ���� ũ���� ����Ʈ���� Ŀ�ٶ� �ؽ��ķ� ��ġ�鼭 ���ο� uv��ǥ�� ����Ѵ�.
	CalcLightmapUV(m_pBspRoot,pSourceLightmap,&sourcelightmaplist);
	CalcLightmapUV(m_pOcRoot,pSourceLightmap,&sourcelightmaplist);


	// ������ ����Ʈ���� �����Ѵ�.

	FILE *file=fopen(filename,"wb+");
	if(!file) {
		bReturnValue=false;
		goto clearandexit;
	}

	fwrite(&header,sizeof(RHEADER),1,file);

	// ���� �ʿ��� ������ ����Ʈ������ Ȯ���� �����صд�.
	fwrite(&m_nConvexPolygon,sizeof(int),1,file);
	fwrite(&m_nNodeCount,sizeof(int),1,file);

	// ������ ũ�⿡ ������ �־��� ����Ʈ���� �����Ѵ�.
	m_nLightmap=m_LightmapList.size();
	fwrite(&m_nLightmap,sizeof(int),1,file);
	for(size_t i=0;i<m_LightmapList.size();i++)
	{
		/*//	����Ʈ�� ����
		char filename[256];
		sprintf(filename,"temp%d.bmp",i);
		m_sourcelightmaplist[i]->Save(filename);
		//*/

//		/*
		// ������ ū ����Ʈ�� ����
		char lightfilename[256];
		sprintf(lightfilename,"%s.light%d.bmp",filename,i);
		RSaveAsBmp(m_LightmapList[i]->GetSize(),m_LightmapList[i]->GetSize(),m_LightmapList[i]->GetData(),lightfilename);
		//*/

		void *memory;
		int nSize;
		bool bSaved=SaveMemoryBmp(m_LightmapList[i]->GetSize(),m_LightmapList[i]->GetSize(),m_LightmapList[i]->GetData(),&memory,&nSize);
		_ASSERT(bSaved);
		fwrite(&nSize,sizeof(int),1,file);
		fwrite(memory,nSize,1,file);
		delete memory;

		/*
		RBspLightmapManager *plm=m_LightmapList[i];		

		int nSize=plm->GetSize();
		fwrite(&nSize,sizeof(int),1,file);
		float fUnused=plm->CalcUnused();
		fwrite(&fUnused,sizeof(float),1,file);
		fwrite(plm->GetData(),plm->GetSize()*plm->GetSize(),sizeof(DWORD),file);
		*/
	}


	Sort_Nodes(m_pOcRoot);

	// �������� ������ �����Ѵ�.
	for(int i=0;i<m_nPolygon;i++)
		fwrite(&(m_pOcInfo+i)->nPolygonID,sizeof(int),1,file);

	// ����Ʈ�� ��ȣ�� �����Ѵ�.
	/*
	for(int i=0;i<m_nBspPolygon;i++)
		fwrite(&(m_pBspInfo+i)->nLightmapTexture,sizeof(int),1,file);
	*/
	for(int i=0;i<m_nPolygon;i++)
		fwrite(&(m_pOcInfo+i)->nLightmapTexture,sizeof(int),1,file);

	// uv ��ǥ�鵵 �����Ѵ�.
	
	/*
	for(int i=0;i<m_nBspPolygon*3;i++)
		fwrite(&(m_pBspVertices+i)->tu2,sizeof(float),2,file);
	*/
	for(int i=0;i<m_nVertices;i++)
		fwrite(&(m_pOcVertices+i)->tu2,sizeof(float),2,file);

	fclose(file);

clearandexit:

	delete []pplight;
	delete []lightmap;
	delete []lightmapdata;
	delete []isshadow;

	delete pSourceLightmap;
	while(sourcelightmaplist.size())
	{
		delete (*sourcelightmaplist.begin())->data;
		delete *sourcelightmaplist.begin();
		sourcelightmaplist.erase(sourcelightmaplist.begin());
	}

	return bReturnValue;
}

void RBspObject::CalcLightmapUV(RSBspNode *pNode,int *pSourceLightmap,vector<RLIGHTMAPTEXTURE*> *pSourceLightmaps)
{
	if(pNode->nPolygon)
	{
		int i,j,k;
		for(i=0;i<pNode->nPolygon;i++)
		{
			int is=pNode->pInfo[i].nConvexPolygon;
			int nSI=pSourceLightmap[is];	// nSI = �������� ������ ����Ʈ���� ���� �ε���

			RCONVEXPOLYGONINFO *poly=m_pConvexPolygons+is;

			rboundingbox bbox;	// �ٿ���ڽ��� ã�´�..

			bbox.vmin=bbox.vmax=poly->pVertices[0];
			for(j=1;j<poly->nVertices;j++)
			{
				for(k=0;k<3;k++)
				{
					bbox.vmin[k]=min(bbox.vmin[k],poly->pVertices[j][k]);
					bbox.vmax[k]=max(bbox.vmax[k],poly->pVertices[j][k]);
				}
			}

			RLIGHTMAPTEXTURE* pDestLightmap=pSourceLightmaps->at(nSI);

			int lightmapsize=pDestLightmap->nSize;

			rvector diff=float(lightmapsize)/float(lightmapsize-1)*(bbox.vmax-bbox.vmin);

			// 1 texel �� ������ �����.
			for(k=0;k<3;k++)
			{
				bbox.vmin[k]-=.5f/float(lightmapsize)*diff[k];
				bbox.vmax[k]+=.5f/float(lightmapsize)*diff[k];
			}

			int au,av,ax; // ���� index    ���� �ؽ��Ŀ����� u��, v��, �׸��� ������ ����..

			if(fabs(poly->plane.a)>fabs(poly->plane.b) && fabs(poly->plane.a)>fabs(poly->plane.c) )
				ax=0;   // yz ������� projection	u �� y �࿡ v �� z �࿡ ����, ���������� ax�� x ��
			else if(fabs(poly->plane.b)>fabs(poly->plane.c))
				ax=1;	// xz ������� ...
			else
				ax=2;	// xy ������� ...

			au=(ax+1)%3;
			av=(ax+2)%3;

			RPOLYGONINFO *pInfo = &pNode->pInfo[i];
			// u2,v2 lightmap �� uv��ǥ�� �����Ѵ�.
			for(j=0;j<pInfo->nVertices;j++)
			{
				pInfo->pVertices[j].tu2=((*pInfo->pVertices[j].Coord())[au]-bbox.vmin[au])/diff[au];
				pInfo->pVertices[j].tv2=((*pInfo->pVertices[j].Coord())[av]-bbox.vmin[av])/diff[av];
			}

			RBspLightmapManager *pCurrentLightmap=m_LightmapList.size() ? m_LightmapList[m_LightmapList.size()-1] : NULL;

			if(!pDestLightmap->bLoaded)
			{
				POINT pt;

				while(!pCurrentLightmap || 
					!pCurrentLightmap->Add(pDestLightmap->data,pDestLightmap->nSize,&pt))
				{
					pCurrentLightmap=new RBspLightmapManager;
					m_LightmapList.push_back(pCurrentLightmap);
				}
				pDestLightmap->bLoaded=true;
				pDestLightmap->position=pt;
				pDestLightmap->nLightmapIndex=m_LightmapList.size()-1;
			}

			pNode->pInfo[i].nLightmapTexture=pDestLightmap->nLightmapIndex;

			// ���� �����޸𸮿� ������ �ؽ�ó���� ��ǥ�� ��ȯ�Ѵ�.
			float fScaleFactor=(float)pDestLightmap->nSize/(float)pCurrentLightmap->GetSize();
			for(j=0;j<pInfo->nVertices;j++)
			{
				pInfo->pVertices[j].tu2 = 
					pInfo->pVertices[j].tu2*fScaleFactor+(float)pDestLightmap->position.x/(float)pCurrentLightmap->GetSize();
				pInfo->pVertices[j].tv2 = 
					pInfo->pVertices[j].tv2*fScaleFactor+(float)pDestLightmap->position.y/(float)pCurrentLightmap->GetSize();
			}
		}
	}

	if(pNode->m_pPositive) CalcLightmapUV(pNode->m_pPositive,pSourceLightmap,pSourceLightmaps);
	if(pNode->m_pNegative) CalcLightmapUV(pNode->m_pNegative,pSourceLightmap,pSourceLightmaps);
}

void RBspObject::GetUV(rvector &Pos,RSBspNode *pNode,int nIndex,float *uv)
{
	/*
	rplane plane=m_pConvexPolygons[pNode->pInfo[nIndex].nConvexPolygon].plane;

	int au,av,ax; // ���� index    ���� �ؽ��Ŀ����� u��, v��, �׸��� ������ ����..

	if(fabs(plane.a)>fabs(plane.b) && fabs(plane.a)>fabs(plane.c) )
		ax=0;   // yz ������� projection	u �� y �࿡ v �� z �࿡ ����, ���������� ax�� x ��
	else if(fabs(plane.b)>fabs(plane.c))
		ax=1;	// xz ������� ...
	else
		ax=2;	// xy ������� ...

	au=(ax+1)%3;
	av=(ax+2)%3;

	rvector *v0=pNode->pVertices[nIndex*3+0].Coord();
	rvector *v1=pNode->pVertices[nIndex*3+1].Coord();
	rvector *v2=pNode->pVertices[nIndex*3+2].Coord();

	float *uv0=&pNode->pVertices[nIndex*3+0].tu2;
	float *uv1=&pNode->pVertices[nIndex*3+1].tu2;
	float *uv2=&pNode->pVertices[nIndex*3+2].tu2;

	rvector a,b,x;
	float tem[2];

	a=*v1-*v0;
	b=*v2-*v1;
	x=Pos-*v0;

	float f=b[au]*x[av]-b[av]*x[au];
	//	_ASSERT(!IS_ZERO(f));
	if(IS_ZERO(f))
	{
		uv[0]=uv0[0];
		uv[1]=uv0[1];
		return;
	}
	float t=(a[av]*x[au]-a[au]*x[av])/f;

	tem[0]=(1-t)*uv1[0]+t*uv2[0];
	tem[1]=(1-t)*uv1[1]+t*uv2[1];

	rvector inter=a+t*b;

	int axisfors;
	if(fabs(inter.x)>fabs(inter.y) && fabs(inter.x)>fabs(inter.z)) axisfors=0;
	else
		if(fabs(inter.y)>fabs(inter.z)) axisfors=1;
		else axisfors=2;

	float s=x[axisfors]/inter[axisfors];

	uv[0]=(1-s)*uv0[0]+s*tem[0];
	uv[1]=(1-s)*uv0[1]+s*tem[1];

//	*normal=InterpolatedVector(*n0,tem,s);
	*/
}

DWORD RBspObject::GetLightmap(rvector &Pos,RSBspNode *pNode,int nIndex)
{
	return 0xffffffff;

	// ���� ���̰� ���� �ʴ�. ����!
	/*
//	_ASSERT(m_pCurrentLightmap);
	if(!m_LightmapList.size())
		return 0xffffffff;

	float uv[2];
	GetUV(Pos,pNode,nIndex,uv);
	
	RBspLightmapManager *pCurrentLightmap=m_LightmapList[pNode->pInfo[nIndex].nLightmapTexture];

	int nSize=pCurrentLightmap->GetSize();

	DWORD *pdata=pCurrentLightmap->GetData();

	uv[0]*=(float)nSize;
	uv[1]*=(float)nSize;

	//// nearest point ���
	//int iuv[2];
	//iuv[0]=int(uv[0]+0.5f);
	//iuv[1]=int(uv[1]+0.5f);

	//return pdata[iuv[1]*nSize+iuv[0]];

	// bilinear interpolation ���..
	int iuv[2];
	iuv[0]=int(uv[0]);
	iuv[1]=int(uv[1]);

	float s=uv[0]-(float)iuv[0];
	float t=uv[1]-(float)iuv[1];

	_ASSERT(s>=0 && s<1.f);

	rvector color0,color1,tempcolor,tempcolor2;
	color0=DWORD2VECTOR(pdata[iuv[1]*nSize+iuv[0]]);
	color1=DWORD2VECTOR(pdata[(iuv[1]+1)*nSize+iuv[0]]);
	tempcolor=(1-t)*color0+t*color1;

	color0=DWORD2VECTOR(pdata[iuv[1]*nSize+iuv[0]+1]);
	color1=DWORD2VECTOR(pdata[(iuv[1]+1)*nSize+iuv[0]+1]);
	tempcolor2=(1-t)*color0+t*color1;

	rvector color=(1-s)*tempcolor+s*tempcolor2;

	return VECTOR2RGB24(color);
	*/
}

rvector RBspObject::GetDimension()
{
	if(!m_pOcRoot)
		return rvector(0,0,0);

	return m_pOcRoot->bbTree.vmax-m_pOcRoot->bbTree.vmin;
}

void RBspObject::DrawSolid()					// ��� solid ������ �׸���
{
#ifndef _PUBLISH

	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();

	pd3dDevice->SetFVF( D3DFVF_XYZ );
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x406fa867);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_pColRoot->DrawSolidPolygon();

	RSetWBuffer(true);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ffffff);
	m_pColRoot->DrawSolidPolygonWireframe();

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ff00ff);
	m_pColRoot->DrawSolidPolygonNormal();

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ff0000);
#endif
}

void RBspObject::DrawSolidNode()
{
	if(!m_DebugInfo.pLastColNode) return;
#ifndef _PUBLISH
	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();
	
	pd3dDevice->SetFVF( D3DFVF_XYZ );
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40808080);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_DebugInfo.pLastColNode->DrawSolidPolygon();

	RSetWBuffer(true);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ffffff);
	m_DebugInfo.pLastColNode->DrawSolidPolygonWireframe();

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ff00ff);
	m_DebugInfo.pLastColNode->DrawSolidPolygonNormal();

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ff0000);

	/*
	{
		// �߰��� ���ؽ� ����� normal�� �׷�����. 

		RSolidBspNode *pNode=m_pColRoot;
		for(int i=0;i<g_nPathDepth;i++)
		{
			rplane plane=pNode->plane;
			if(g_impactpath[i]==0)
				pNode=pNode->m_pNegative;
			else
				pNode=pNode->m_pPositive;

			// ��� ���� �� �ϳ��� ���� �����ٸ�.. ������ ���ؽ��� �߰��� ����̴�
			bool bExist=false;
			rvector pos;
			for(int j=0;j<m_DebugInfo.pLastColNode->nPolygon*3;j++)
			{
				if(fabs(D3DXPlaneDotCoord(&plane,&m_DebugInfo.pLastColNode->pVertices[j]))<0.01f) {
					if(!bExist) {
						pos=m_DebugInfo.pLastColNode->pVertices[j];
						bExist=true;
					}
					else
					{
						if(!IS_EQ3(pos,m_DebugInfo.pLastColNode->pVertices[j])) {
							bExist=false;
							break;
						}
					}
				}
			}

			if(bExist)
			{
				rvector v[2];
				v[0]=pos;
				v[1]=pos+rvector(plane.a,plane.b,plane.c)*50.f;
				RGetDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP,1,&v,sizeof(rvector));
			}
		}
	}
	*/
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , 0x40ffffff);

#endif
}

bool RBspObject::CheckWall(rvector &origin,rvector &targetpos,float fRadius,float fHeight,RCOLLISIONMETHOD method,int nDepth,rplane *pimpactplane)
{
	return RSolidBspNode::CheckWall(m_pColRoot,origin,targetpos,fRadius,fHeight,method,nDepth,pimpactplane);
}

bool RBspObject::CheckSolid(rvector &pos,float fRadius,float fHeight,RCOLLISIONMETHOD method)
{
	RImpactPlanes impactPlanes;
	if(method==RCW_SPHERE)
		return m_pColRoot->GetColPlanes_Sphere(&impactPlanes,pos,pos,fRadius);
	else
		return m_pColRoot->GetColPlanes_Cylinder(&impactPlanes,pos,pos,fRadius,fHeight);
}

rvector RBspObject::GetFloor(rvector &origin,float fRadius,float fHeight,rplane *pimpactplane)
{
	rvector targetpos=origin+rvector(0,0,-10000);

	RImpactPlanes impactPlanes;
	bool bIntersect=m_pColRoot->GetColPlanes_Cylinder(&impactPlanes,origin,targetpos,fRadius,fHeight);
//	_ASSERT(bIntersect);
	if(!bIntersect)
		return targetpos;

	rvector floor=m_pColRoot->GetLastColPos();
	floor.z-=fHeight;
	if(pimpactplane)
		*pimpactplane=m_pColRoot->GetLastColPlane();

	return floor;
}

RBSPMATERIAL *RBspObject::GetMaterial(int nIndex)
{
	_ASSERT(nIndex<m_nMaterial);
	return &m_pMaterials[nIndex];
}

// world matrix �� ���ȵǾ����� �ʴ�. ������ world==identity�� ����
bool RBspObject::IsVisible(rboundingbox &bb)		// occlusion �� ���� ������������ false �� ����.
{
	return m_OcclusionList.IsVisible(bb);
}

bool RBspObject::GetShadowPosition( rvector& pos_, rvector& dir_, rvector* outNormal_, rvector* outPos_ )
{
	RBSPPICKINFO pick_info;
	if(!Pick( pos_, dir_, &pick_info ))
		return false;
	*outPos_ = pick_info.PickPos;
	*outNormal_ = rvector(pick_info.pInfo[pick_info.nIndex].plane.a,
		pick_info.pInfo[pick_info.nIndex].plane.b, pick_info.pInfo[pick_info.nIndex].plane.c);
	//*outNormal_= rvector(pick_info.pNode->plane.a,pick_info.pNode->plane.b, pick_info.pNode->plane.c);
	return true;
}
//*/

/*
// center�� �ظ��� �߽� pole�� ������, radius�� �ظ��� ���������� �ϴ� �ܿ� ������ ���� ����� ���� ����. (ī�޶� ���)
bool RBspObject::CheckWall_Corn(rvector *pOut,rvector &center,rvector &pole,float fRadius)
{
	float fMinLength=0.f;

	rvector dir=pole-center;
	float fLength=Magnitude(dir);
	Normalize(dir);

	impactcount=0;
	impactPlanes.erase(impactPlanes.begin(),impactPlanes.end());

	bool bIntersectThis=CheckWall_Sphere(m_pColRoot,center,pole,fRadius);

	for(RImpactPlanes::iterator i=impactPlanes.begin();i!=impactPlanes.end();i++)
	{
		rplane plane=i->second;

		if(D3DXPlaneDotNormal(&plane,&dir)<0.01){
			rplane shiftplane=plane;
			shiftplane.d+=fRadius;
			
			// ���� �浹 ��ġ
			rvector impactpos;
			D3DXPlaneIntersectLine(&impactpos,&shiftplane,&center,&pole);
			impactpos-=fRadius*rvector(plane.a,plane.b,plane.c);

			// �浹��ġ�� ������ corn�� �࿡ ������ ���
			rplane ortplane;
			D3DXPlaneFromPointNormal(&ortplane,&impactpos,&dir);
			
			// �� ���� ����� ����
			rvector ortcenter;
			D3DXPlaneIntersectLine(&ortcenter,&ortplane,&center,&pole);

			// �� ��鿡 ���� �ܸ��� ���� �ݰ�
			float fortlength=max(DotProduct(pole-ortcenter,dir),0);
			float fortradius=fRadius*fortlength/fLength;

			float fDistToCorn=fortradius-Magnitude(impactpos-ortcenter);

			// 0���� ũ�� �浹���� ��������
			if(fDistToCorn<fMinLength)
			{
				fMinLength=fDistToCorn;
			}
		}
	}

	*pOut=pole+dir*fMinLength;

	return true;
}
*/

// pick �� ���� ���� ����..
bool			g_bPickFound;
rvector			g_PickOrigin;
rvector			g_PickTo;
rvector			g_PickDir;
RBSPPICKINFO	*g_pPickOut;
//RPathNode		**g_pPickOutPathNode;
rvector			*g_pColPos;
rplane			g_PickPlane;
float			g_fPickDist;
DWORD			g_dwPassFlag;
//*/

#ifndef _PUBLISH

#define __BP(i,n)	MBeginProfile(i,n);
#define __EP(i)		MEndProfile(i);

#else

#define __BP(i,n) ;
#define __EP(i) ;

#endif

bool RBspObject::Pick( const rvector &pos, const rvector &dir,RBSPPICKINFO *pOut,DWORD dwPassFlag)
{
	if(!m_pBspRoot) return false;

	__BP(195,"RBspObject::Pick");

	g_PickOrigin=pos;
	g_PickTo=pos+10000.f*dir;
	g_PickDir=dir;
	Normalize(g_PickDir);

	g_bPickFound=false;
	g_pPickOut=pOut;
	D3DXPlaneFromPointNormal(&g_PickPlane,&g_PickOrigin,&g_PickDir);
	g_fPickDist=FLT_MAX;

	g_dwPassFlag=dwPassFlag;

	g_bPickFound=Pick(m_pBspRoot,pos,g_PickTo);

	__EP(195);

	if(g_bPickFound)
		return true;

	return false;
}

bool RBspObject::PickTo( const rvector &pos, const rvector &to,RBSPPICKINFO *pOut,DWORD dwPassFlag )
{
	if(!m_pBspRoot) return false;

	__BP(195,"RBspObject::Pick");

	g_PickOrigin=pos;
	g_PickTo=to;
	g_PickDir=to-pos;
	Normalize(g_PickDir);

	g_bPickFound=false;
	g_pPickOut=pOut;
	D3DXPlaneFromPointNormal(&g_PickPlane,&g_PickOrigin,&g_PickDir);
	g_fPickDist=FLT_MAX;

	g_dwPassFlag=dwPassFlag;

	g_bPickFound=Pick(m_pBspRoot,pos,g_PickTo);

	__EP(195);

	if(g_bPickFound)
		return true;

	return false;
}

bool RBspObject::PickOcTree(rvector &pos,rvector &dir,RBSPPICKINFO *pOut,DWORD dwPassFlag)
{
	if(!m_pOcRoot) return false;

	g_PickOrigin=pos;
	g_PickTo=pos+10000.f*dir;
	g_PickDir=dir;
	Normalize(g_PickDir);

	g_bPickFound=false;
	g_pPickOut=pOut;
	D3DXPlaneFromPointNormal(&g_PickPlane,&g_PickOrigin,&g_PickDir);
	g_fPickDist=FLT_MAX;

	g_dwPassFlag=dwPassFlag;

	g_bPickFound=Pick(m_pOcRoot,pos,g_PickTo);

	if(g_bPickFound)
		return true;

	return false;
}

bool RBspObject::PickShadow(rvector &pos,rvector &to,RBSPPICKINFO *pOut)
{
	if(!m_pBspRoot) return false;

	g_PickOrigin=pos;
	g_PickTo=to;
	g_PickDir=to-pos;
	Normalize(g_PickDir);

	g_bPickFound=false;
	g_pPickOut=pOut;
	D3DXPlaneFromPointNormal(&g_PickPlane,&g_PickOrigin,&g_PickDir);
	g_fPickDist=FLT_MAX;

	g_bPickFound=PickShadow(m_pBspRoot,pos,to);

	if(g_bPickFound)
		return true;

	return false;
}

#define PICK_TOLERENCE 0.01f
#define PICK_SIGN(x) ( (x)<-PICK_TOLERENCE ? -1 : (x)>PICK_TOLERENCE ? 1 : 0 )

// side �ʰ� v0-v1 ������ �����ϴ� �κ��� ������ true �� �����ϸ鼭 �����κ��� w0-w1�� ����
bool pick_checkplane(int side,rplane &plane, const rvector &v0, const rvector &v1,rvector *w0,rvector *w1)
{
	float dotv0=D3DXPlaneDotCoord(&plane,&v0);
	float dotv1=D3DXPlaneDotCoord(&plane,&v1);

	int signv0=PICK_SIGN(dotv0),signv1=PICK_SIGN(dotv1);

	if(signv0!=-side) {
		*w0=v0;

		if(signv1!=-side)
			*w1=v1;
		else
		{
			rvector intersect;
			if(D3DXPlaneIntersectLine(&intersect,&plane,&v0,&v1))
				*w1=intersect;
			else
				*w1=v1;
		}
		return true;
	}

	if(signv1!=-side) {
		*w1=v1;

		if(signv0!=-side)
			*w0=v0;
		else
		{
			rvector intersect;
			if(D3DXPlaneIntersectLine(&intersect,&plane,&v0,&v1))
				*w0=intersect;
			else
				*w0=v0;
		}
		return true;
	}

	return false;
}


bool RBspObject::Pick(RSBspNode *pNode, const rvector &v0,const rvector &v1)
{
	if(!pNode) return false;

	// leaf node �̸�
	if(pNode->nPolygon){
		bool bPicked=false;

		for(int i=0;i<pNode->nPolygon;i++)
		{
			RPOLYGONINFO *pInfo = &pNode->pInfo[i];
			if( (pInfo->dwFlags & g_dwPassFlag) != 0 ) continue;
			if( D3DXPlaneDotCoord(&pInfo->plane,&g_PickOrigin)<0 ) continue;

			for(int j=0;j<pInfo->nVertices-2;j++)
			{
				float fDist;
				if( IsIntersect(g_PickOrigin,g_PickDir,
						*pInfo->pVertices[0].Coord(),
						*pInfo->pVertices[j+1].Coord(),
						*pInfo->pVertices[j+2].Coord(),&fDist))
				{
					rvector pos;
					D3DXPlaneIntersectLine(&pos,&pNode->pInfo[i].plane,&g_PickOrigin,&g_PickTo);

					if(D3DXPlaneDotCoord(&g_PickPlane,&pos)>=0)
					{
						float fDist=Magnitude(pos-g_PickOrigin);
						if(fDist<g_fPickDist)
						{
							bPicked=true;
							g_fPickDist=fDist;
							g_pPickOut->PickPos=pos;
							g_pPickOut->pNode=pNode;
							g_pPickOut->nIndex=i;
							g_pPickOut->pInfo=&pNode->pInfo[i];
						}
					}
				}
			}
		}

		return bPicked;
	}

	rvector w0,w1;
	bool bHit=false;
	if(D3DXPlaneDotNormal(&pNode->plane,&g_PickDir)>0) {
		// ��������� ������ ���������̸� m_pNegative -> postive ������ �˻�

		if(pick_checkplane(-1,pNode->plane,v0,v1,&w0,&w1)) 
		{
			bHit=Pick(pNode->m_pNegative,w0,w1);
			if(bHit) return true;
		}

		if(pick_checkplane(1,pNode->plane,v0,v1,&w0,&w1)) 
		{
			bHit |= Pick(pNode->m_pPositive,w0,w1);
		}

		return bHit;
	}else
	{
		if(pick_checkplane(1,pNode->plane,v0,v1,&w0,&w1))
		{
			bHit=Pick(pNode->m_pPositive,w0,w1);
			if(bHit) return true;
		}

		if(pick_checkplane(-1,pNode->plane,v0,v1,&w0,&w1))
		{
			bHit |= Pick(pNode->m_pNegative,w0,w1);
		}

		return bHit;
	}

	return false;
}

// lightmap �����Ҷ��� ���̴� ���
// ���Ŀ� opacity ���� ������ �ؽ��ĸ� ã�ƺ��� ����. ������ pick �� ������ �ȹ�ħ.
bool RBspObject::PickShadow(RSBspNode *pNode,rvector &v0,rvector &v1)
{
	if(!pNode) return false;

	// leaf node �̸�
	if(pNode->nPolygon){
		bool bPicked=false;

		for(int i=0;i<pNode->nPolygon;i++)
		{
			RPOLYGONINFO *pInfo = &pNode->pInfo[i];
			if( (pInfo->dwFlags & (RM_FLAG_ADDITIVE | RM_FLAG_USEOPACITY | RM_FLAG_HIDE)) != 0 ||
				(pInfo->dwFlags & RM_FLAG_CASTSHADOW) == 0 ||
				(D3DXPlaneDotCoord(&pInfo->plane,&g_PickOrigin)<0) ) continue;

			for(int j=0;j<pInfo->nVertices-2;j++)
			{
				float fDist;
				if( 
					/*
					(pInfo->dwFlags & (RM_FLAG_ADDITIVE | RM_FLAG_USEOPACITY | RM_FLAG_HIDE)) == 0 &&
					(pInfo->dwFlags & RM_FLAG_CASTSHADOW) != 0 &&
					(D3DXPlaneDotCoord(&pInfo->plane,&g_PickOrigin)>=0) &&
					*/
					IsIntersect(g_PickOrigin,g_PickDir,
						*pInfo->pVertices[0].Coord(),
						*pInfo->pVertices[j+1].Coord(),
						*pInfo->pVertices[j+2].Coord(),&fDist))
				{
					rvector pos;
					D3DXPlaneIntersectLine(&pos,&pInfo->plane,&g_PickOrigin,&g_PickTo);

					if(D3DXPlaneDotCoord(&g_PickPlane,&pos)>=0)
					{
						float fDist=Magnitude(pos-g_PickOrigin);
						if(fDist<g_fPickDist)
						{
							bPicked=true;
							g_fPickDist=fDist;
							g_pPickOut->PickPos=pos;
							g_pPickOut->pNode=pNode;
							g_pPickOut->nIndex=i;
							g_pPickOut->pInfo=pInfo;
						}
					}
				}
			}
		}

		return bPicked;
	}

	rvector w0,w1;
	bool bHit=false;
	if(D3DXPlaneDotNormal(&pNode->plane,&g_PickDir)>0) {
		// ��������� ������ ���������̸� m_pNegative -> postive ������ �˻�

		if(pick_checkplane(-1,pNode->plane,v0,v1,&w0,&w1)) 
		{
			bHit=PickShadow(pNode->m_pNegative,w0,w1);
			if(bHit) {
				return true;
			//	v1=g_pPickOut->PickPos;
			//	return true;
			}
		}

		if(pick_checkplane(1,pNode->plane,v0,v1,&w0,&w1)) 
		{
			bHit |= PickShadow(pNode->m_pPositive,w0,w1);
		}

		return bHit;
	}else
	{
		if(pick_checkplane(1,pNode->plane,v0,v1,&w0,&w1))
		{
			bHit=PickShadow(pNode->m_pPositive,w0,w1);
			if(bHit) return true;

			/*
			if(bHit) {
				return true;
				rvector dir=g_pPickOut->PickPos-v0;
				Normalize(dir);
				_ASSERT(DotProduct(dir,g_PickDir)>0.99);
				v1=g_pPickOut->PickPos;
			}
			*/
		}

		if(pick_checkplane(-1,pNode->plane,v0,v1,&w0,&w1))
		{
			bHit |= PickShadow(pNode->m_pNegative,w0,w1);
		}

		return bHit;
	}

	return false;
}

void RBspObject::GetNormal(int nConvexPolygon,rvector &position,rvector *normal)
{
	RCONVEXPOLYGONINFO *poly = m_pConvexPolygons+nConvexPolygon;
	int au,av,ax; // ���� index    ���� �ؽ��Ŀ����� u��, v��, �׸��� ������ ����..

	if(fabs(poly->plane.a)>fabs(poly->plane.b) && fabs(poly->plane.a)>fabs(poly->plane.c) )
		ax=0;   // yz ������� projection	u �� y �࿡ v �� z �࿡ ����, ���������� ax�� x ��
	else if(fabs(poly->plane.b)>fabs(poly->plane.c))
		ax=1;	// xz ������� ...
	else
		ax=2;	// xy ������� ...

	au=(ax+1)%3;
	av=(ax+2)%3;

	GetNormal(poly,position,normal,au,av);
}


// ���������� �׸������� ��ǵ�.. ���� �׽�Ʈ��

RBaseTexture *RBspObject::m_pShadeMap;

bool RBspObject::CreateShadeMap(const char *szShadeMap)
{
	if(m_pShadeMap) 
		DestroyShadeMap();
	m_pShadeMap = RCreateBaseTexture(szShadeMap,RTextureType_Etc,false);
	return true;
}

void RBspObject::DestroyShadeMap()
{
	RDestroyBaseTexture(m_pShadeMap);
	m_pShadeMap = NULL;
}

struct LIGHTBSPVERTEX {
	rvector coord;
	DWORD dwColor;
	float tu1, tv1;
	float tu2, tv2;
};

#define LIGHTVERTEXBUFFER_SIZE	1024

DWORD m_dwLightVBBase = 0;
LIGHTBSPVERTEX *m_pLightVertex;

bool RBspObject::CreateDynamicLightVertexBuffer()
{
	InvalidateDynamicLightVertexBuffer();
	HRESULT hr=RGetDevice()->CreateVertexBuffer( sizeof(LIGHTBSPVERTEX)*LIGHTVERTEXBUFFER_SIZE * 3, 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LIGHT_BSP_FVF, D3DPOOL_DEFAULT, &m_pDynLightVertexBuffer ,NULL);

	return true;
}

void RBspObject::InvalidateDynamicLightVertexBuffer()
{
	SAFE_RELEASE(m_pDynLightVertexBuffer);
}


bool RBspObject::FlushLightVB()
{
	m_pDynLightVertexBuffer->Unlock();
	if(m_dwLightVBBase==0) return true;

	g_nCall++;
	HRESULT hr = RGetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_dwLightVBBase);
	_ASSERT(hr == D3D_OK);
	return true;
}

bool RBspObject::LockLightVB()
{
	HRESULT hr;
	if( FAILED( hr = m_pDynLightVertexBuffer->Lock( 0, 
		LIGHTVERTEXBUFFER_SIZE * sizeof(LIGHTBSPVERTEX) ,
		(LPVOID*)&m_pLightVertex, D3DLOCK_DISCARD ) ) )
	{
		return false;
	}
	m_dwLightVBBase=0;

	return true;
}

D3DLIGHT9 *g_pTargetLight;
DWORD		g_dwTargetLightColor;

bool RBspObject::DrawLight(RSBspNode *pNode,int nMaterial)
{
	if(pNode->nFrameCount!=g_nFrameNumber) return true;

	if(pNode->nPolygon)
	{
		int nCount = pNode->pDrawInfo[nMaterial].nTriangleCount;
		int nIndCount = nCount*3;
		if(nCount)
		{
			g_nPoly+=nCount;

			RDrawInfo *pdi = &pNode->pDrawInfo[nMaterial];
			int index=pdi->nIndicesOffset;
			
			int i;
			for(i=0;i<nCount;i++)
			{

				rplane *pPlane = &pdi->pPlanes[i];

				WORD indices[3] = { m_pOcIndices[index++],m_pOcIndices[index++],m_pOcIndices[index++] };

				float fPlaneDotCoord = pPlane->a * g_pTargetLight->Position.x + pPlane->b * g_pTargetLight->Position.y +
					pPlane->c * g_pTargetLight->Position.z +pPlane->d;

				if(fPlaneDotCoord>g_pTargetLight->Range) continue;

				// ���� �޸鿡 �ִ°͵��� (���ڱ� ������� �̻��ϹǷ�) ���� ������� �Ѵ�
#define BACK_FACE_DISTANCE 200.f
				if(fPlaneDotCoord<-BACK_FACE_DISTANCE) continue;
				if(fPlaneDotCoord<0) fPlaneDotCoord=-fPlaneDotCoord/BACK_FACE_DISTANCE * g_pTargetLight->Range;


				LIGHTBSPVERTEX *v = m_pLightVertex+m_dwLightVBBase*3;

				for(int j=0;j<3;j++) {
					BSPVERTEX *pv = &m_pOcVertices[indices[j]];
					v[j].coord = *pv->Coord();
					v[j].tu2 = pv->tu1;
					v[j].tv2 = pv->tv1;
				}

				rvector t;
				t = v[0].coord - g_pTargetLight->Position;

				for(int j=0;j<3;j++)
				{
					rvector l = v[j].coord - g_pTargetLight->Position;
					l *= 1.f/g_pTargetLight->Range;

					v[j].tu1 = -DotProduct(pdi->pUAxis[i],l) * .5 + .5;
					v[j].tv1 = -DotProduct(pdi->pVAxis[i],l) * .5 + .5;
					
					float fIntensity = min(1.f,max(0,1.f - fPlaneDotCoord / g_pTargetLight->Range));

					v[j].dwColor = DWORD(fIntensity*255) << 24 | g_dwTargetLightColor;
					//*/

				}

// �ϳ��� ����
//				RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,1,v,sizeof(LIGHTBSPVERTEX));

				m_dwLightVBBase++;
				if(m_dwLightVBBase==LIGHTVERTEXBUFFER_SIZE)
				{
					FlushLightVB();
					LockLightVB();
				}
			}
		}
	}else
	{
		bool bOk=true;
		if(pNode->m_pNegative) {
			if(!DrawLight(pNode->m_pNegative,nMaterial))
				bOk=false;
		}
		if(pNode->m_pPositive) {
			if(!DrawLight(pNode->m_pPositive,nMaterial))
				bOk=false;
		}
		return bOk;
	}
	return true;
}

void RBspObject::DrawLight(D3DLIGHT9 *pLight)
{
	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();
	if(!m_pVertexBuffer)
		return;

	if(!m_pDynLightVertexBuffer)
		CreateDynamicLightVertexBuffer();

	pd3dDevice->SetTexture(0,m_pShadeMap->GetTexture());

	RGetDevice()->SetStreamSource(0,m_pDynLightVertexBuffer,0,sizeof(LIGHTBSPVERTEX) );

	g_pTargetLight = pLight;
	g_dwTargetLightColor = FLOAT2RGB24(
		min(1.f,max(0.f,g_pTargetLight->Diffuse.r)),
		min(1.f,max(0.f,g_pTargetLight->Diffuse.g)),
		min(1.f,max(0.f,g_pTargetLight->Diffuse.b)));

	g_nPoly=0;
	g_nCall=0;
	g_nFrameNumber++;

	int nChosen=ChooseNodes(m_pOcRoot,rvector(pLight->Position),pLight->Range);

//	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );

	for(int i=0;i<m_nMaterial;i++)
	{
		/*
		pd3dDevice->SetTexture(0,m_pMaterials[i].texture->GetTexture());
		if((m_pMaterials[i].dwFlags & RM_FLAG_ADDITIVE) == 0 )
			Draw(m_pOcRoot,i);
		*/

		if((m_pMaterials[i % m_nMaterial].dwFlags & RM_FLAG_TWOSIDED) == 0 )
			RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
		else
			RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );

		if((m_pMaterials[i % m_nMaterial].dwFlags & RM_FLAG_ADDITIVE ) == 0 )
		{
			int nMaterial = i % m_nMaterial;
			RBaseTexture *pTex=m_pMaterials[nMaterial].texture;
			if(pTex)
			{
				pd3dDevice->SetTexture(1,pTex->GetTexture());
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			}
			else
			{
				DWORD dwDiffuse=VECTOR2RGB24(m_pMaterials[nMaterial].Diffuse);
				pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR , dwDiffuse);
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TFACTOR );
			}

			LockLightVB();
			DrawLight(m_pOcRoot,i);
			FlushLightVB();
		}
	}

	pd3dDevice->SetStreamSource( 0, NULL,0, 0 );
}


_NAMESPACE_REALSPACE2_END