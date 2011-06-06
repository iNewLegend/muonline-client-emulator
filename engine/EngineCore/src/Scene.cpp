#include "Scene.h"
#include "CsvFile.h"
#include "IORead.h"
#include <algorithm>
#include "RenderNodeMgr.h"

CScene::CScene():
m_bShowObject(true),
m_bShowObjectBBox(false),
m_Fog(32.0f,48.0f,0.01f,0xFF223344),
m_Light(Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec3D(-1.0f,-1.0f,-1.0f))
{
	m_bRefreshViewport = true;
	m_pTerrain = NULL;
}

CScene::~CScene()
{
	clearChildren();
}

iRenderNodeMgr* CScene::getRenderNodeMgr()
{
	return (iRenderNodeMgr*)&CRenderNodeMgr::getInstance();
}

static Vec3D vEyeForObjectSort;
bool sortObject(CRenderNode* p1, CRenderNode* p2)
{
	CMapObj* p11 = (CMapObj*)p1;
	CMapObj* p22 = (CMapObj*)p2;
	if (p11->getOrder()!=p22->getOrder())
	{
		return p11->getOrder()>p22->getOrder();
	}
	return p11>p22;
//	//float fLength = (vEyeForObjectSort-p1->getPos()).lengthSquared()-(vEyeForObjectSort-p2->getPos()).lengthSquared();
//	//if (fLength!=0)
//	//{
//	//	return fLength>0;
//	//}
//	return p1->getModelFilename()>p2->getModelFilename();
}

void CScene::GetRenderObject(const CFrustum& frustum, LIST_RENDER_NODE& ObjectList)
{
	m_ObjectTree.getObjectsByFrustum(frustum,ObjectList);
	static bool bTest = true;
	if (bTest)
	{
		vEyeForObjectSort= frustum.getEyePoint();
		//std::sort(ObjectList.begin(),ObjectList.end(), sortObject);
	}
}

bool CScene::updateMapObj(CRenderNode* pMapObj)
{
	return m_ObjectTree.updateObject(pMapObj);
}

void CScene::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
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
	m_setRenderSceneObj.clear();
	if (m_bShowObject)
	{
		GetRenderObject(frustum, m_setRenderSceneObj);
	}
}

#include "Graphics.h"
void CScene::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	CRenderSystem& R = GetRenderSystem();
	//R.setFogEnable(true);
	R.ClearBuffer(true, true, m_Fog.color);
	// 
	if (m_bShowObjectBBox)
	{
		R.SetDepthBufferFunc(true,true);
		// ----
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_setRenderSceneObj)
		{
		//	(*it)->renderDebug();
		}
	}
	//if (m_setFocusObjects.size()>0)
	{
// 		for(size_t i=0;i<m_setFocusObjects.size();++i)
// 		{
// 			m_setFocusObjects[i]->renderDebug();
// 		}
		// The octree boxs of focus objects.
// 		for(size_t i=0;i<m_setFocusObjects.size();++i)
// 		{
// 			ObjectTree* pParentObjectTree = m_ObjectTree.find(m_setFocusObjects[i]);
// 			if (pParentObjectTree)
// 			{
// 				GetGraphics().drawBBox(pParentObjectTree->getBBox(),0xFF00FF44);
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
				CONST_FOR_IN(LIST_RENDER_NODE,itLight,m_setLightObj)
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
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_setRenderSceneObj)
		{
			try {
				CMapObj* pObj = (CMapObj*)(*it);
				if(pObj)
				{
					//if(pObj->GetObjType() == MAP_3DOBJ)
					{
						C3DMapObj* p3DObj = (C3DMapObj*)pObj;
						float fHeight = getTerrainData()->GetHeight(p3DObj->getPos().x,p3DObj->getPos().z);
						p3DObj->renderShadow(Matrix::UNIT,vLightDir,fHeight);
						// ----
						CONST_FOR_IN(LIST_RENDER_NODE,itLight,m_setLightObj)
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
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_setRenderSceneObj)
		{
			try {
				CMapObj* pObj = (CMapObj*)(*it);
				if(pObj)
				{
					if(pObj->GetObjType() != MAP_ROLE &&
						pObj->GetObjType() != MAP_HERO &&
						pObj->GetObjType() != MAP_PLAYER &&
						pObj->GetObjType() != MAP_3DEFFECT &&
						pObj->GetObjType() != MAP_3DEFFECTNEW)
					{
						Vec4D vColor = m_pTerrain->GetColor((*it)->getPos().x,(*it)->getPos().z);
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
					CONST_FOR_IN(LIST_RENDER_NODE,itLight,m_setLightObj)
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
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_FocusNodel.getChildObj())
		{
			((C3DMapObj*)*it)->renderFocus();
		}
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_FocusNodel.getChildObj())
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
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_setRenderSceneObj)
		{
			(*it)->render(Matrix::UNIT,MATERIAL_ALPHA);
		}
		CONST_FOR_IN(LIST_RENDER_NODE,it,m_setRenderSceneObj)
		{
			(*it)->render(Matrix::UNIT,MATERIAL_GLOW);
		}
	}
}

void CScene::createObjectTree(const BBox& box, size_t size)
{
	m_ObjectTree.create(box,size);
}

void CScene::addChild(CRenderNode* pChild)
{
	CRenderNode::addChild(pChild);
	// ----
	if (m_ObjectTree.addObject(pChild))
	{
		m_bRefreshViewport = true;
	}
	else
	{
		delete pChild;
		//MessageBoxA(NULL,strBmdFilename.c_str(),"Error!exclude!",0);
	}
}

bool CScene::removeChild(CRenderNode* pChild)
{
	if (!CRenderNode::removeChild(pChild))
	{
		return false;
	}
	// ----
	m_ObjectTree.eraseObject(pChild);
	// ----
	removeRenderObj(pChild);
	// ----
	m_FocusNodel.removeChild(pChild);
	return true;
}

bool CScene::removeRenderObj(CRenderNode* pObj)
{
	// del from render
	LIST_RENDER_NODE::iterator it = find( m_setRenderSceneObj.begin( ), m_setRenderSceneObj.end( ), pObj );
	if(it!=m_setRenderSceneObj.end())
	{
		m_setRenderSceneObj.erase(it);
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
// 	LIST_RENDER_NODE setObject;
// 	m_ObjectTree.getObjectsByPos(vWorldPos,setObject);
// 	FOR_IN(LIST_RENDER_NODE,it,setObject)
// 	{
// 		if((*it) && ((*it)->GetObjType() == MAP_3DEFFECT || (*it)->GetObjType() == MAP_3DEFFECTNEW))
// 		{
// 			if (m_ObjectTree.delObject(*it))
// 			{
// 				C3DMapEffect* pEffect = (C3DMapEffect*)(*it);
// 				S_DEL(pEffect);
// 			}
// 		}
// 	}
}

void CScene::del3DMapEffect(C3DMapEffect* pEffect)
{
	if (m_ObjectTree.find(pEffect))
	{
		if(pEffect->GetObjType() == MAP_3DEFFECT || pEffect->GetObjType() == MAP_3DEFFECTNEW)
		{
			pEffect->Die();
		}
	}
}

void CScene::clearAllObjects()
{

}

void CScene::getAllObjects()
{

}

/*CRenderNode* CScene::add3DMapSceneObj(__int64 uID,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale)
{
 	if (m_ObjectInfo.find(uID)==m_ObjectInfo.end())
 	{
		return NULL;
	}
	// ----
	const ObjectInfo& objectInfo = m_ObjectInfo[uID];
	// ----
	BBox localBBox;
	localBBox.vMin.set(-1,-1,-1);
	localBBox.vMax.set( 1, 1, 1);
	// ----
	//C3DMapSceneObj* pObject = new C3DMapSceneObj;
	//pObject->Register(objectInfo.strFilename.c_str());
	C3DMapSceneObj* pObject = new C3DMapSceneObj;
	pObject->setPos(vPos);
	pObject->setRotate(vRotate);
	pObject->setScale(vScale);
	pObject->setLocalBBox(localBBox);
	pObject->updateWorldMatrix();
	pObject->updateWorldBBox();
	// ----
	pObject->setObjectID(uID);
	// ----
	char szNameID[255];
	sprintf(szNameID,"%d",uID);
	pObject->setName(szNameID);
	// ----
	// 		//Vec4D vColor = m_pTerrain->GetData().GetColor(Vec2D(vPos.x,vPos.z));
	// 		//vColor.w=1.0f;
	// 		//pObject->SetMaterial(vColor*0.5f,vColor+0.3f);
	// ----
	addChild(pObject);
	return pObject;
}*/

bool CScene::delChildByFocus()
{
	const LIST_RENDER_NODE& focusChild = m_FocusNodel.getChildObj();
	// ----
	CONST_FOR_IN(LIST_RENDER_NODE,it,focusChild)
	{
		delChild(*it);
	}
	return true;
}

void CScene::updateObjTreeByFocus()
{
	LIST_RENDER_NODE& focusChild = m_FocusNodel.getChildObj();
	FOR_IN(LIST_RENDER_NODE,it,focusChild)
	{
		updateMapObj(*it);
	}
}

#include "float.h"

#include "intersect.h"
CMapObj* CScene::pickObject(const Vec3D& vRayPos , const Vec3D& vRayDir)
{
	CMapObj* pObject = NULL;
	float fFocusMin = FLT_MAX;
	FOR_IN(LIST_RENDER_NODE,it,m_setRenderSceneObj)
	{
		float fMin, fMax;
		if (((CMapObj*)(*it))->intersect(vRayPos , vRayDir, fMin, fMax))
		{
			if (fFocusMin>fMin)
			{
				pObject = (CMapObj*)*it;
				fFocusMin=fMax;
			}
		}
	}
	return pObject;
}

void CScene::clearChildren()
{
	CRenderNode::clearChildren();
	m_ObjectTree.clearObjects();
	m_setRenderSceneObj.clear();
	// ----
	m_FocusNodel.removeChildren();
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