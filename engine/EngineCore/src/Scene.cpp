#include "Scene.h"
#include <algorithm>
#include "RenderNodeMgr.h"

CScene::CScene()
	:m_Fog(32.0f,48.0f,0.01f,0xFFF23344)
	,m_pSceneData(NULL)
	,m_bRefreshViewport(NULL)
{
	m_OctreeRoot.create(BBox(-512.0f,-512.0f,-512.0f,512.0f,512.0f,512.0f),7);
}

CScene::~CScene()
{
	clearChildren();
}

bool sortNode(iRenderNode* p1, iRenderNode* p2)
{
	Matrix mView = CRenderSystem::getSingleton().getViewMatrix();
	Vec3D v1 = p1->getWorldBBox().vMin+p1->getPos();
	Vec3D v2 = p2->getWorldBBox().vMin+p2->getPos();
 	return (mView*v1).z > (mView*v2).z;
	//float fLength = (vEyeForNodeSort-p1->getPos()).lengthSquared()-(vEyeForNodeSort-p2->getPos()).lengthSquared();
	//if (fLength!=0)
	//{
	//	return fLength>0;
	//}
	//return p1->getModelFilename()>p2->getModelFilename();
}
void CScene::getRenderNodes(const CFrustum& frustum, std::set<iRenderNode*>& setNode)
{
	m_OctreeRoot.walkOctree(frustum,setNode);
	//std::sort(setNode.begin(),setNode.end(), sortNode);
}

bool CScene::updateNode(iRenderNode* pNode)
{
	if(m_OctreeRoot.eraseNode(pNode))
	{
		m_OctreeRoot.addNode(pNode->getWorldBBox(), pNode);
		return true;
	}
	return false;
}

void CScene::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	setup();
	FOR_IN(it,m_RenderNodes)
	{
		(*it)->frameMove(mWorld, fTime, fElapsedTime);
	}
}

void CScene::updateRender(const CFrustum& frustum)
{
	static CFrustum s_frustum;
	if (m_bRefreshViewport || s_frustum!=frustum)
	{
		s_frustum=frustum;
		m_bRefreshViewport = false;
		m_RenderNodes.clear();
		getRenderNodes(frustum, m_RenderNodes);
	}
}

Matrix CalcLightMatrix(const BBox& bbox, const Vec3D& vLightDir)
{
	float fLength = (bbox.vMax-bbox.vMin).length();
	Vec3D vLookAt=(bbox.vMax+bbox.vMin)*0.5f;
	Vec3D vEyePt = vLookAt - vLightDir*fLength*0.5f;

	Vec3D vUp(0,1,0); 
	Matrix mView, mProj;
	mView.MatrixLookAtLH(vEyePt,vLookAt,vUp);
	mProj.MatrixOrthoLH(fLength,fLength, 0, fLength);
	Matrix mLight = mProj*mView;
	return mLight;
}

#include "Graphics.h"
void CScene::render(int nRenderType)const
{
	// ----
	std::list<iRenderNode*> sortRenderNodes;
	// ----
	FOR_IN(it,m_RenderNodes)
	{
		sortRenderNodes.push_back(*it);
	}
	sortRenderNodes.sort(sortNode);
	// ----
	if (nRenderType&RF_GEOMETRY)
	{
		FOR_IN(it,sortRenderNodes)
		{
			if ((*it)->getFocus()>0)
			{
				continue;
			}
			(*it)->render(nRenderType);
		}
		FOR_IN(it,sortRenderNodes)
		{
			if ((*it)->getFocus()>0)
			{
				CRenderSystem& R = CRenderSystem::getSingleton();
				R.SetShader("focus");
				switch ((*it)->getFocus())
				{
				case 1:
					R.setShaderFloatArray("gColor",	&Vec3D(2,0,0), 3);
					break;
				case 2:
					R.setShaderFloatArray("gColor",	&Vec3D(0,2,0), 3);
					break;
				case 3:
					R.setShaderFloatArray("gColor",	&Vec3D(0,0,2), 3);
					break;
				}
				(*it)->render(RF_GEOMETRY|RF_ALPHA|RF_GLOW|RF_NO_SHADER);
				(*it)->render(nRenderType);
			}
		}
	}
	else
	{
		FOR_IN(it,sortRenderNodes)
		{
			(*it)->render(nRenderType);
		}
	}
}

bool CScene::setup()
{
	if (m_pData==(void*)-1)
	{
		return false;
	}
	if (m_pSceneData==m_pData)
	{
		if (m_pData==NULL)
		{
			CRenderNodeMgr::getInstance().PushMTLoading(this);
		}
		return false;
	}
	m_pSceneData = (CSceneData*)m_pData;
	//m_OctreeRoot.create(m_pSceneData->getBBox(),m_pSceneData->getOctreeDepth());
	return true;
}

void CScene::addChild(iRenderNode* pChild)
{
	// 临时放在这里
	//setup();
	// ----
	if (pChild->getLocalBBox().vMin.x == FLT_MAX)
	{
		BBox box(-2.0f,-1.0f,-2.0f, 2.0f,2.0f,2.0f);
		pChild->setLocalBBox(box);
	}
	// ----
	pChild->updateWorldBBox();
	pChild->updateWorldMatrix();
	// ----
	CRenderNode::addChild(pChild);
	// ----
	if (m_OctreeRoot.addNode(pChild->getWorldBBox(), pChild))
	{
		m_bRefreshViewport = true;
	}
	else
	{
		delete pChild;
		//MessageBoxA(NULL,strBmdFilename.c_str(),"Error!exclude!",0);
	}
}

bool CScene::removeChild(iRenderNode* pChild)
{
	if (!CRenderNode::removeChild(pChild))
	{
		return false;
	}
	// ----
	m_OctreeRoot.eraseNode(pChild);
	// ----
	removeRenderNode(pChild);
	// ----
	m_FocusNode.removeChild(pChild);
	return true;
}

bool CScene::removeRenderNode(iRenderNode* pNode)
{
	// del from render
	auto it = find( m_RenderNodes.begin( ), m_RenderNodes.end( ), pNode );
	if(it!=m_RenderNodes.end())
	{
		m_RenderNodes.erase(it);
		return true;
	}
	return false;
}

void CScene::clearNodes()
{

}

void CScene::getNodes()
{

}

bool CScene::delChildByFocus()
{
	const LIST_RENDER_NODE& focusChild = m_FocusNode.getChildObj();
	// ----
	FOR_IN(it,focusChild)
	{
		delChild(*it);
	}
	return true;
}

void CScene::updateOctreeByFocus()
{
	LIST_RENDER_NODE& focusChild = m_FocusNode.getChildObj();
	FOR_IN(it,focusChild)
	{
		updateNode(*it);
	}
}

#include "float.h"

#include "intersect.h"
CRenderNode* CScene::pickNode(const Vec3D& vRayPos, const Vec3D& vRayDir)
{
	CRenderNode* pNode = NULL;
	float fFocusMin = FLT_MAX;
	FOR_IN(it,m_RenderNodes)
	{
		float fMin, fMax;
		if ((*it)->intersect(vRayPos , vRayDir, fMin, fMax))
		{
			if (fFocusMin>fMin)
			{
				pNode = (CRenderNode*)*it;
				fFocusMin=fMax;
			}
		}
	}
	return pNode;
}

bool CScene::pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	if (m_pSceneData)
	{
		return m_pSceneData->pick(vRayPos, vRayDir, pPos);
	}
	return false;
}

float CScene::getHeight(float x, float y)const
{
	if (m_pSceneData)
	{
		return m_pSceneData->getHeight(x, y);
	}
	return 0.0f;
}

unsigned char CScene::getPath(int sx,int sy,int tx,int ty, std::deque<char>& path)
{
	if (m_pSceneData)
	{
		return m_pSceneData->getPath(sx, sy, tx, ty, path);
	}
	return 0;
}


void CScene::clearChildren()
{
	CRenderNode::clearChildren();
	m_OctreeRoot.clearNodes();
	m_RenderNodes.clear();
	// ----
	m_FocusNode.removeChildren();
	// ----
	m_bRefreshViewport = true;
}

//#include "LightMap.h"
void CScene::CalcLightMap()
{
	//CLightMap lightMap;
	//lightMap.SetScene(this);
	//lightMap.CalcLightMap();
}