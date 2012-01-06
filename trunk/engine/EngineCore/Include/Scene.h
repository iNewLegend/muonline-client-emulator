#pragma once
#include "Terrain.h"
#include "3DMapEffect.h"
#include "Octree.h"
#include "FocusNode.h"
#include "InterfacePlug.h"


class CScene: public CRenderNode
{
public:
	CScene();
	~CScene();
public:
	virtual int				getType					(){return NODE_BASE;}
	virtual void			getRenderNodes			(const CFrustum& frustum, std::set<iRenderNode*>& setNode);
	virtual void			updateRender			(const CFrustum& frustum);
	bool					updateNode				(iRenderNode* pNode);
	// ----
	virtual void			frameMove				(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render					(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	// ----
	virtual void			addChild				(iRenderNode* pChild);
	virtual bool			removeChild				(iRenderNode* pChild);
	virtual void			clearChildren			();
	// ----
	// load
	virtual bool			setup					();
	bool					removeRenderNode		(iRenderNode* pNode);
	// ----
	// # shit
	// ----
	C3DMapEffect*			add3DMapEffect			(const Vec3D& vWorldPos, char* pszIndex, bool bDelself = true);
	void					del3DMapEffect			(const Vec3D& vWorldPos);
	void					del3DMapEffect			(C3DMapEffect* pEffect);
	// ----
	// # shit
	// ----
	virtual	void			clearNodes				();
	virtual	void			getNodes				();
	// ----
	// # Focus Nodes
	// ----
	bool					delChildByFocus			();
	void					updateOctreeByFocus		();
	CFocusNode&				getFocusNodes			(){return m_FocusNode;}
	// ----
	// # Åö×²
	// ----
	CRenderNode*			pickNode				(const Vec3D& vRayPos , const Vec3D& vRayDir);
	bool					pick					(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const;
	float					getHeight				(float x, float y)const;
	unsigned char			getPath					(int sx,int sy,int tx,int ty, std::deque<char>& path);
	// ----
	// # data
	// ----
	Octree<iRenderNode*>&	getOctree				(){return m_OctreeRoot;}
	std::set<iRenderNode*>&	getRenderNodes			(){return m_RenderNodes;}
	// ----
	void					CalcLightMap			();
	// ----
	GSET_VAR				(bool,				m_b,ShowNodeBBox);
	GSET_VAR				(bool,				m_b,ShowOctreeBox);
	GSET_VAR				(bool,				m_b,RefreshViewport);
	GSET_CONST_VAR			(DirectionalLight&,	m_,Light);
	GSET_CONST_VAR			(Fog&,				m_,Fog);
	GSET_CONST_VAR			(Vec3D&,			m_v,TargetPos);
	// ----
	CSceneData*				getSceneData			(){return m_pSceneData;}
protected:
	CSceneData*				m_pSceneData;
	Octree<iRenderNode*>	m_OctreeRoot;
	std::set<iRenderNode*>	m_RenderNodes;
	LIST_RENDER_NODE		m_setLightObj;
	bool					m_bRefreshViewport;
	CFocusNode				m_FocusNode;        // The node which has focus
	bool					m_bShowAnimNode;
	bool					m_bShowNodeBBox;
	bool					m_bShowOctreeBox;

	Fog						m_Fog;
	DirectionalLight		m_Light;
	Vec3D					m_vTargetPos;
};