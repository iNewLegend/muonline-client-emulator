#include "Scene.h"
#include <algorithm>
#include "RenderNodeMgr.h"

CScene::CScene()
	:m_bShowNodeBBox(false)
	,m_Fog(32.0f,48.0f,0.01f,0xFFF23344)
	,m_Light(Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec3D(-1.0f,-1.0f,-1.0f))
	,m_pSceneData(NULL)
	,m_bRefreshViewport(NULL)
{
	m_OctreeRoot.create(BBox(-512.0f,-512.0f,-512.0f,512.0f,512.0f,512.0f),7);
}

CScene::~CScene()
{
	clearChildren();
}

static Vec3D vEyeForNodeSort;
//bool sortNode(CRenderNode* p1, CRenderNode* p2)
//{
// 	CMapObj* p11 = (CMapObj*)p1;
// 	CMapObj* p22 = (CMapObj*)p2;
// 	if (p11->getOrder()!=p22->getOrder())
// 	{
// 		return p11->getOrder()>p22->getOrder();
// 	}
// 	return p11>p22;
//	//float fLength = (vEyeForNodeSort-p1->getPos()).lengthSquared()-(vEyeForNodeSort-p2->getPos()).lengthSquared();
//	//if (fLength!=0)
//	//{
//	//	return fLength>0;
//	//}
//	return p1->getModelFilename()>p2->getModelFilename();
//}

void CScene::getRenderNodes(const CFrustum& frustum, std::set<iRenderNode*>& setNode)
{
	m_OctreeRoot.walkOctree(frustum,setNode);
	static bool bTest = true;
	if (bTest)
	{
		vEyeForNodeSort= frustum.getEyePoint();
		//std::sort(NodeList.begin(),NodeList.end(), sortNode);
	}
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

#include "Graphics.h"
void CScene::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	CRenderSystem& R = CRenderSystem::getSingleton();
	//R.setFogEnable(true);
	R.ClearBuffer(true, true, m_Fog.color);
	// 
	if (m_bShowNodeBBox)
	{
		R.SetDepthBufferFunc(true,true);
		// ----
		FOR_IN(it,m_RenderNodes)
		{
		//	GetGraphics().drawBBox((*it)->getLocalBBox(),0xFFFF4400);
		}
	}
	//if (m_setFocusNodes.size()>0)
	{
// 		for(size_t i=0;i<m_setFocusNodes.size();++i)
// 		{
// 			m_setFocusNodes[i]->renderDebug();
// 		}
		// The octree boxs of focus Nodes.
// 		for(size_t i=0;i<m_setFocusNodes.size();++i)
// 		{
// 			Octree* pParentOctree = m_Octree.find(m_setFocusNodes[i]);
// 			if (pParentOctree)
// 			{
// 				GetGraphics().drawBBox(pParentOctree->getBBox(),0xFF00FF44);
// 			}
// 		}
	}
	//
	R.setFog(m_Fog);
	R.setFogEnable(m_Fog.fEnd>0.0f);
	R.SetDirectionalLight(0, m_Light);
	Vec3D vLightDir = Vec3D(-0.8f,-1.0f,0.0f).normalize();
 	{
		// ----
		FOR_IN(it,m_RenderNodes)
		{
			try {
				CRenderNode* pObj = (CRenderNode*)(*it);
				if(pObj)
				{
					//if(pObj->GetObjType() == MAP_3DOBJ)
					{
					/*	C3DMapObj* p3DObj = (C3DMapObj*)pObj;
						float fHeight = 0.0f;
						if (m_pSceneData)
						{
							fHeight = m_pScene->getHeight(p3DObj->getPos().x,p3DObj->getPos().z);
						}
						p3DObj->renderShadow(Matrix::UNIT,vLightDir,fHeight);
						// ----
						FOR_IN(itLight,m_setLightObj)
						{
							Vec3D vDir = (*it)->getPos()-(*itLight)->getPos();
							if (vDir.length()<3.0f)
							{
								vDir.normalize();
								vDir.y=-1;
								vDir.normalize();
								p3DObj->renderShadow(Matrix::UNIT,vDir,fHeight);
							}
						}*/
					}
				}
			}catch(...)
			{ 
				return;
			}
		}
		// ----
		R.SetStencilFunc(false);
		// ----
		iRenderNode* pFocusNode = NULL;
		if (m_FocusNode.getChildObj().size()>0)
		{
			pFocusNode = *m_FocusNode.getChildObj().begin();
		}
		FOR_IN(it,m_RenderNodes)
		{
			if (*it==pFocusNode)
			{
				CRenderSystem::getSingleton().SetShader("ObjectFocus");
				float color[4] = {1.0f,0.25f,0.0f,0.5f};
				R.SetPixelShaderConstantF(0,color,1);
				// monster (0xFFFF4040)
				// NPC (0xFF40FF40)
				// Player (0xFF00FFFF)
				((CRenderNode*)*it)->render(Matrix::UNIT, E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));

			}
			(*it)->render(Matrix::UNIT,MATERIAL_GEOMETRY);
		}
		Fog fogForGlow;
		fogForGlow = m_Fog;
		//fogForGlow.fStart = m_Fog.fStart;
		fogForGlow.fEnd = m_Fog.fEnd*2.0f;
		R.setFog(fogForGlow);
		//
		FOR_IN(it,m_RenderNodes)
		{
		//	(*it)->render(Matrix::UNIT,MATERIAL_ALPHA);
		}
		FOR_IN(it,m_RenderNodes)
		{
		//	(*it)->render(Matrix::UNIT,MATERIAL_GLOW);
		}
	}
}

bool CScene::setup()
{
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
	setup();
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

C3DMapEffect* CScene::add3DMapEffect(const Vec3D& vWorldPos, char* pszIndex, bool bDelSelf)
{
	if (!pszIndex)
		return NULL;
	// ----
	C3DMapEffect* pEffect = C3DMapEffect::CreateNew( vWorldPos, pszIndex, bDelSelf);
	// ----
	if(pEffect)
	{
		addChild(pEffect);
	}
	return pEffect;
}

void CScene::del3DMapEffect(const Vec3D& vWorldPos)
{
// 	LIST_RENDER_NODE setNode;
// 	m_Octree.getNodesByPos(vWorldPos,setNode);
// 	FOR_IN(it,setNode)
// 	{
// 		if((*it) && ((*it)->GetObjType() == MAP_3DEFFECT || (*it)->GetObjType() == MAP_3DEFFECTNEW))
// 		{
// 			if (m_Octree.delNode(*it))
// 			{
// 				C3DMapEffect* pEffect = (C3DMapEffect*)(*it);
// 				S_DEL(pEffect);
// 			}
// 		}
// 	}
}

void CScene::del3DMapEffect(C3DMapEffect* pEffect)
{
	m_OctreeRoot.eraseNode(pEffect);
	//if(pEffect->getObjType() == MAP_3DEFFECT || pEffect->getObjType() == MAP_3DEFFECTNEW)
	{
		pEffect->Die();
	}
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
		if (((CRenderNode*)(*it))->intersect(vRayPos , vRayDir, fMin, fMax))
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