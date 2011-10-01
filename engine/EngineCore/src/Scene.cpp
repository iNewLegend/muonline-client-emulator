#include "Scene.h"
#include <algorithm>
#include "RenderNodeMgr.h"

static iRenderNode* s_pThreadRenderNode = NULL;
DWORD WINAPI LoadingThreads (LPVOID pScene)
{
	while(true)
	{
		iRenderNode* pRenderNode = s_pThreadRenderNode;
		s_pThreadRenderNode=NULL;
		if (pRenderNode)
		{
			pRenderNode->load(pRenderNode->getFilename());
			pRenderNode->setLoaded(true);
		}
	}
	return 0;
}

CScene::CScene()
	:m_bShowNode(true)
	,m_bShowNodeBBox(false)
	,m_Fog(32.0f,48.0f,0.01f,0xFF223344)
	,m_Light(Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec3D(-1.0f,-1.0f,-1.0f))
	,m_pSceneData(NULL)
	,m_bRefreshViewport(NULL)
{
	//isStartImmediate = CREATE_SUSPENDED;
	m_hThread=CreateThread(NULL,0,LoadingThreads,(LPVOID)this,0,&m_dwThreadID); 
	//if (m_hThread)
	//{ 
	//	printf ("Thread launched successfully\n");                
	//}         
	//ResumeThread(m_hThread);
}

CScene::~CScene()
{
	if (m_hThread)
	{            
		CloseHandle(m_hThread);
	} 
	clearChildren();
}

static Vec3D vEyeForNodeSort;
bool sortNode(CRenderNode* p1, CRenderNode* p2)
{
	CMapObj* p11 = (CMapObj*)p1;
	CMapObj* p22 = (CMapObj*)p2;
	if (p11->getOrder()!=p22->getOrder())
	{
		return p11->getOrder()>p22->getOrder();
	}
	return p11>p22;
//	//float fLength = (vEyeForNodeSort-p1->getPos()).lengthSquared()-(vEyeForNodeSort-p2->getPos()).lengthSquared();
//	//if (fLength!=0)
//	//{
//	//	return fLength>0;
//	//}
//	return p1->getModelFilename()>p2->getModelFilename();
}

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
	setup();
	// ----
	FOR_IN(it,m_mapChildNode)
	{
		(*it)->frameMove(Matrix::UNIT,fTime,fElapsedTime);
	}
}

void CScene::loading()
{
// 	FOR_IN(it,m_setRenderSceneNode)
// 	{
// 		(*it)->load((*it)->getFilename());
// 	}
	if (m_RenderNodes.size()>0)
	{
		(*m_RenderNodes.begin())->load((*m_RenderNodes.begin())->getFilename());
	}
}

void CScene::updateRender(const CFrustum& frustum)
{
	if (!m_bRefreshViewport)
	{
		static CFrustum s_frustum;
		if (s_frustum==frustum)
		{
			return;
		}
		s_frustum=frustum;
	}
	m_bRefreshViewport = false;
	//
	m_RenderNodes.clear();
	if (m_bShowNode)
	{
		getRenderNodes(frustum, m_RenderNodes);
		FOR_IN(it,m_RenderNodes)
		{
			if (!(*it)->getLoaded())
			{
				s_pThreadRenderNode = (*it);
			}
			//(*it)->load((*it)->getFilename());
		}
	}
}

#include "Graphics.h"
void CScene::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	CRenderSystem& R = GetRenderSystem();
	//R.setFogEnable(true);
	R.ClearBuffer(true, true, m_Fog.color);
	// 
	if (m_bShowNodeBBox)
	{
		R.SetDepthBufferFunc(true,true);
		// ----
		FOR_IN(it,m_RenderNodes)
		{
		//	(*it)->renderDebug();
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
// 	if (m_pTerrain)
// 	{
// 		m_pTerrain->render(mWorld,MATERIAL_GEOMETRY);
// 	}
// 	if (m_pTerrain)
// 	{
// 		//if(m_pTerrain->prepare())
// 		{
// 			if(R.prepareMaterial("LightDecal"))
// 			{
// 				R.SetBlendFunc(true,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_ONE);
// 				// ----
// 				FOR_IN(itLight,m_setLightObj)
// 				{
// 					const Vec3D& vLightPos = (*itLight)->getPos();
// 					//m_pTerrain->drawLightDecal(vLightPos.x,vLightPos.z,3.0f,0xFFFFFFFF);
// 				}
// 			}
// 		}
// 		R.finishMaterial();
// 	}
	{
		R.SetCullingMode(CULL_NONE);
		R.SetShader((CShader*)NULL);
		R.SetLightingEnabled(false);
		R.SetBlendFunc(true,BLENDOP_ADD,SBF_ZERO,SBF_SOURCE_COLOUR);
		R.SetAlphaTestFunc(true);
		R.SetDepthBufferFunc(false,false);
		R.SetTextureFactor(0x80808080);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
		R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TEXTURE);
		R.SetTextureColorOP(1,TBOP_DISABLE);
		R.SetTextureAlphaOP(1,TBOP_DISABLE);
		R.SetStencilFunc(true,STENCILOP_INCR,CMPF_GREATER);
		// ----
		FOR_IN(it,m_RenderNodes)
		{
			try {
				CMapObj* pObj = (CMapObj*)(*it);
				if(pObj)
				{
					//if(pObj->GetObjType() == MAP_3DOBJ)
					{
					/*	C3DMapObj* p3DObj = (C3DMapObj*)pObj;
						float fHeight = 0.0f;
						if (m_pTerrain && m_pTerrain->getTerrainData())
						{
							fHeight = m_pTerrain->getTerrainData()->getHeight(p3DObj->getPos().x,p3DObj->getPos().z);
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
		FOR_IN(it,m_RenderNodes)
		{
			try {
				CMapObj* pObj = (CMapObj*)(*it);
				if(pObj)
				{
						Vec4D vColor(1.0f,1.0f,1.0f,1.0f);
						if (m_pSceneData)
						{
							vColor = m_pSceneData->getColor((*it)->getPos().x,(*it)->getPos().z);
						}
						vColor.w=1.0f;

						DirectionalLight light(vColor*0.5f,vColor+0.3f,Vec4D(1.0f,1.0f,1.0f,1.0f),vLightDir);
						R.SetDirectionalLight(0,light);
					// ----
					FOR_IN(itLight,m_setLightObj)
					{
						if (((*itLight)->getPos()-(*it)->getPos()).length()<3.0f)
						{
							PointLight light(Vec4D(0.0f,0.0f,0.0f,1.0f),Vec4D(1.0f,0.0f,0.0f,1.0f),
								Vec4D(1.0f,1.0f,1.0f,1.0f),(*itLight)->getPos(),3.0f);
							R.setPointLight(1,light);
							R.LightEnable(1,true);
						}
					}
					(*it)->render(Matrix::UNIT,MATERIAL_GEOMETRY);
					R.LightEnable(1,false);
				}
			}catch(...)
			{
				return;
			}
		}
		//
		FOR_IN(it,m_FocusNode.getChildObj())
		{
			R.SetTextureFactor(0xFF40FF40);
			if (GetRenderSystem().prepareMaterial("ObjectFocus"))
			{
				float color[4] = {1.0f,0.25f,0.0f,0.5f};
				R.SetPixelShaderConstantF(0,color,1);
				// monster (0xFFFF4040)
				// NPC (0xFF40FF40)
				// Player (0xFF00FFFF)
				((CRenderNode*)*it)->render(Matrix::UNIT, E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));
				GetRenderSystem().finishMaterial();
			}
		}
 		FOR_IN(it,m_FocusNode.getChildObj())
 		{
 			DirectionalLight light(Vec4D(0.8f,0.8f,0.8f,0.8f),Vec4D(1.0f,1.0f,1.0f,1.0f),
 				Vec4D(0.6f,0.6f,0.6f,0.6f),vLightDir);
 			R.SetDirectionalLight(0,light);
 			(*it)->render(Matrix::UNIT,MATERIAL_GEOMETRY);
 		}
		//
		//if (m_pSceneData)
		//{
		////	m_pTerrain->render(Matrix::UNIT,MATERIAL_ALPHA);
		//}
		DirectionalLight light(Vec4D(0.3f,0.3f,0.3f,0.3f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec3D(0.0f,-1.0f,1.0f));
		R.SetDirectionalLight(0,light);

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
		return false;
	}
	m_pSceneData = (CSceneData*)m_pData;
	m_OctreeRoot.create(m_pSceneData->getBBox(),m_pSceneData->getOctreeDepth());
	return true;
}

void CScene::addChild(iRenderNode* pChild)
{
	// 临时放在这里
	setup();
	//
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
	if(pEffect->getObjType() == MAP_3DEFFECT || pEffect->getObjType() == MAP_3DEFFECTNEW)
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
CMapObj* CScene::pickNode(const Vec3D& vRayPos, const Vec3D& vRayDir)
{
	CMapObj* pNode = NULL;
	float fFocusMin = FLT_MAX;
	FOR_IN(it,m_RenderNodes)
	{
		float fMin, fMax;
		if (((CMapObj*)(*it))->intersect(vRayPos , vRayDir, fMin, fMax))
		{
			if (fFocusMin>fMin)
			{
				pNode = (CMapObj*)*it;
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

unsigned char CScene::getPath(int sx,int sy,int tx,int ty, std::vector<unsigned char>& path)
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