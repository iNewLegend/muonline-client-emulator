#include "Scene.h"
#include "CsvFile.h"
#include "IORead.h"
#include <algorithm>
#include "RenderNodeMgr.h"

CScene::CScene()
	:m_bShowNode(true)
	,m_bShowNodeBBox(false)
	,m_Fog(32.0f,48.0f,0.01f,0xFF223344)
	,m_Light(Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec3D(-1.0f,-1.0f,-1.0f))
	,m_pSceneData(NULL)
	,m_pTerrain(NULL)
	,m_bRefreshViewport(NULL)
{
}

CScene::~CScene()
{
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

void CScene::getRenderNodes(const CFrustum& frustum, LIST_RENDER_NODE& NodeList)
{
	m_Octree.getNodesByFrustum(frustum,NodeList);
	static bool bTest = true;
	if (bTest)
	{
		vEyeForNodeSort= frustum.getEyePoint();
		//std::sort(NodeList.begin(),NodeList.end(), sortNode);
	}
}

bool CScene::updateNode(iRenderNode* pNode)
{
	if(m_Octree.eraseNode(pNode))
	{
		m_Octree.addNode(pNode->getWorldBBox(), pNode);
		return true;
	}
	return false;
}

void CScene::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	FOR_IN(it,m_mapChildNode)
	{
		(*it)->frameMove(Matrix::UNIT,fTime,fElapsedTime);
	}
}

void CScene::UpdateRender(const CFrustum& frustum)
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
	if (m_pTerrain)
	{
		m_pTerrain->UpdateRender(frustum);
	}
	//
	m_setRenderSceneNode.clear();
	if (m_bShowNode)
	{
		getRenderNodes(frustum, m_setRenderSceneNode);
		FOR_IN(it,m_setRenderSceneNode)
		{
			if (1)
			{
				(*it)->load((*it)->getFilename());
			}
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
		FOR_IN(it,m_setRenderSceneNode)
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
	if (m_pTerrain)
	{
		m_pTerrain->Render();
	}
	if (m_pTerrain)
	{
		if(m_pTerrain->Prepare())
		{
			if(R.prepareMaterial("LightDecal"))
			{
				R.SetBlendFunc(true,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_ONE);
				// ----
				FOR_IN(itLight,m_setLightObj)
				{
					const Vec3D& vLightPos = (*itLight)->getPos();
					m_pTerrain->drawLightDecal(vLightPos.x,vLightPos.z,3.0f,0xFFFFFFFF);
				}
			}
		}
		R.finishMaterial();
	}
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
		FOR_IN(it,m_setRenderSceneNode)
		{
			try {
				CMapObj* pObj = (CMapObj*)(*it);
				if(pObj)
				{
					//if(pObj->GetObjType() == MAP_3DOBJ)
					{
						C3DMapObj* p3DObj = (C3DMapObj*)pObj;
						float fHeight = 0.0f;
						if (getTerrainData())
						{
							fHeight = getTerrainData()->GetHeight(p3DObj->getPos().x,p3DObj->getPos().z);
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
						}
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
		FOR_IN(it,m_setRenderSceneNode)
		{
			try {
				CMapObj* pObj = (CMapObj*)(*it);
				if(pObj)
				{
					if(pObj->getObjType() != MAP_ROLE &&
						pObj->getObjType() != MAP_HERO &&
						pObj->getObjType() != MAP_PLAYER &&
						pObj->getObjType() != MAP_3DEFFECT &&
						pObj->getObjType() != MAP_3DEFFECTNEW)
					{
						Vec4D vColor(1.0f,1.0f,1.0f,1.0f);
						if (m_pTerrain)
						{
							vColor = m_pTerrain->GetColor((*it)->getPos().x,(*it)->getPos().z);
						}
						vColor.w=1.0f;

						DirectionalLight light(vColor*0.5f,vColor+0.3f,Vec4D(1.0f,1.0f,1.0f,1.0f),vLightDir);
						R.SetDirectionalLight(0,light);
					}
					else
					{
						DirectionalLight light(Vec4D(0.4f,0.4f,0.4f,0.4f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(0.6f,0.6f,0.6f,0.6f),vLightDir);
						R.SetDirectionalLight(0,light);
					}
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
			((C3DMapObj*)*it)->renderFocus();
		}
		FOR_IN(it,m_FocusNode.getChildObj())
		{
			DirectionalLight light(Vec4D(0.4f,0.4f,0.4f,0.4f),Vec4D(1.0f,1.0f,1.0f,1.0f),
				Vec4D(0.6f,0.6f,0.6f,0.6f),vLightDir);
			R.SetDirectionalLight(0,light);
			(*it)->render(Matrix::UNIT,MATERIAL_GEOMETRY);
		}
		//
		if (m_pTerrain)
		{
			m_pTerrain->renderGrass();
		}
		DirectionalLight light(Vec4D(0.3f,0.3f,0.3f,0.3f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec3D(0.0f,-1.0f,1.0f));
		R.SetDirectionalLight(0,light);

		Fog fogForGlow;
		fogForGlow = m_Fog;
		//fogForGlow.fStart = m_Fog.fStart;
		fogForGlow.fEnd = m_Fog.fEnd*2.0f;
		R.setFog(fogForGlow);
		//
		FOR_IN(it,m_setRenderSceneNode)
		{
			(*it)->render(Matrix::UNIT,MATERIAL_ALPHA);
		}
		FOR_IN(it,m_setRenderSceneNode)
		{
			(*it)->render(Matrix::UNIT,MATERIAL_GLOW);
		}
	}
}

bool CScene::init(void* pData)
{
	m_pSceneData = (SceneData*)pData;
	m_Octree.create(m_pSceneData->getBBox(),m_pSceneData->getOctreeSize());
	return true;
}

void CScene::addChild(iRenderNode* pChild)
{
	CRenderNode::addChild(pChild);
	// ----
	if (m_Octree.addNode(pChild->getWorldBBox(), pChild))
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
	m_Octree.eraseNode(pChild);
	// ----
	removeRenderNode(pChild);
	// ----
	m_FocusNode.removeChild(pChild);
	return true;
}

bool CScene::removeRenderNode(iRenderNode* pNode)
{
	// del from render
	auto it = find( m_setRenderSceneNode.begin( ), m_setRenderSceneNode.end( ), pNode );
	if(it!=m_setRenderSceneNode.end())
	{
		m_setRenderSceneNode.erase(it);
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
	if (m_Octree.find(pEffect))
	{
		if(pEffect->getObjType() == MAP_3DEFFECT || pEffect->getObjType() == MAP_3DEFFECTNEW)
		{
			pEffect->Die();
		}
	}
}

void CScene::clearNodes()
{

}

void CScene::getNodes()
{

}

/*CRenderNode* CScene::add3DMapSceneObj(__int64 uID,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale)
{
 	if (m_NodeInfo.find(uID)==m_NodeInfo.end())
 	{
		return NULL;
	}
	// ----
	const NodeInfo& NodeInfo = m_NodeInfo[uID];
	// ----
	BBox localBBox;
	localBBox.vMin.set(-1,-1,-1);
	localBBox.vMax.set( 1, 1, 1);
	// ----
	//C3DMapSceneObj* pNode = new C3DMapSceneObj;
	//pNode->Register(NodeInfo.strFilename.c_str());
	C3DMapSceneObj* pNode = new C3DMapSceneObj;
	pNode->setPos(vPos);
	pNode->setRotate(vRotate);
	pNode->setScale(vScale);
	pNode->setLocalBBox(localBBox);
	pNode->updateWorldMatrix();
	pNode->updateWorldBBox();
	// ----
	pNode->setNodeID(uID);
	// ----
	char szNameID[255];
	sprintf(szNameID,"%d",uID);
	pNode->setName(szNameID);
	// ----
	// 		//Vec4D vColor = m_pTerrain->GetData().GetColor(Vec2D(vPos.x,vPos.z));
	// 		//vColor.w=1.0f;
	// 		//pNode->SetMaterial(vColor*0.5f,vColor+0.3f);
	// ----
	addChild(pNode);
	return pNode;
}*/

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
CMapObj* CScene::pickNode(const Vec3D& vRayPos , const Vec3D& vRayDir)
{
	CMapObj* pNode = NULL;
	float fFocusMin = FLT_MAX;
	FOR_IN(it,m_setRenderSceneNode)
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

void CScene::clearChildren()
{
	CRenderNode::clearChildren();
	m_Octree.clearNodes();
	m_setRenderSceneNode.clear();
	// ----
	m_FocusNode.removeChildren();
	// ----
	m_bRefreshViewport = true;
}

#include "LightMap.h"
void CScene::CalcLightMap()
{
	CLightMap lightMap;
	lightMap.SetScene(this);
	lightMap.CalcLightMap();
}