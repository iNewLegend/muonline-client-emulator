#pragma once
#include "Terrain.h"
#include "MapObj.h"
#include "3DMapSceneObj.h"
#include "3DMapEffect.h"
#include "ObjectTree.h"
#include "FocusNode.h"

class SceneData: public iSceneData
{
public:
	virtual GSET_CONST_VAR	(BBox&,				m_,BBox);
	virtual GSET_CONST_VAR	(size_t,			m_,ObjectTreeSize);
	virtual GSET_CONST_VAR	(Fog&,				m_,Fog);
	virtual GSET_CONST_VAR	(DirectionalLight&,	m_,Light);
private:
	BBox				m_BBox;
	size_t				m_ObjectTreeSize;
	Fog					m_Fog;
	DirectionalLight	m_Light;
};

class CScene: public CRenderNode
{
public:
	CScene();
	~CScene();
public:
	virtual int			getType					(){return NODE_BASE;}
	virtual void		GetRenderObject			(const CFrustum& frustum, LIST_RENDER_NODE& ObjectList);
	void				UpdateRender			(const CFrustum& frustum);
	bool				updateMapObj			(iRenderNode* pMapObj);
	// ----
	virtual void		frameMove				(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void		render					(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	// ----
	virtual void		addChild				(iRenderNode* pChild);
	virtual bool		removeChild				(iRenderNode* pChild);
	virtual void		clearChildren			();
	// ----
	// load
	virtual bool		init					(void* pData);
	bool				removeRenderObj			(iRenderNode* pObj);
	// ----
	// # shit
	// ----
	C3DMapEffect*		add3DMapEffect			(const Vec3D& vWorldPos, char* pszIndex, bool bDelself = true);
	void				del3DMapEffect			(const Vec3D& vWorldPos);
	void				del3DMapEffect			(C3DMapEffect* pEffect);
	// ----
	// # shit
	// ----
	virtual	void		clearAllObjects			();
	virtual	void		getAllObjects			();
	// ----
	// # Focus Objects
	// ----
	bool				delChildByFocus			();
	void				updateObjTreeByFocus	();
	CFocusNode&			getFocusObjects			(){return m_FocusNodel;}
	// ----
	CMapObj*			pickObject				(const Vec3D& vRayPos , const Vec3D& vRayDir);
	// ----
	// # data
	// ----
	ObjectTree&			GetObject				()						{return m_ObjectTree;}
	// ----
	iTerrainData*		getTerrainData			()						{return m_pTerrain;}
	const iTerrainData*	getTerrainData			()const					{return m_pTerrain;}
	void				CalcLightMap			();
	// ----
	GSET_VAR		(bool,				m_b,ShowObject);
	GSET_VAR		(bool,				m_b,ShowObjectBBox);
	GSET_VAR		(bool,				m_b,ShowObjectTreeBox);
	GSET_VAR		(bool,				m_b,RefreshViewport);
	GSET_VAR		(CTerrain*,			m_p,Terrain );
	GSET_CONST_VAR	(DirectionalLight&,	m_,Light);
	GSET_CONST_VAR	(Fog&,				m_,Fog);
	GSET_CONST_VAR	(Vec3D&,			m_v,TargetPos);
	// ----
	SceneData*				getSceneData(){return m_pSceneData;}
protected:
	SceneData*				m_pSceneData;
	CTerrain*				m_pTerrain;
	ObjectTree				m_ObjectTree;
	LIST_RENDER_NODE		m_setRenderSceneObj;
	LIST_RENDER_NODE		m_setLightObj;
	bool					m_bRefreshViewport;
protected:
	CFocusNode				m_FocusNodel;        // The object which has focus
	bool					m_bShowObject;
	bool					m_bShowAnimObject;
	bool					m_bShowObjectBBox;
	bool					m_bShowObjectTreeBox;

	Fog						m_Fog;
	DirectionalLight		m_Light;
	Vec3D					m_vTargetPos;
};