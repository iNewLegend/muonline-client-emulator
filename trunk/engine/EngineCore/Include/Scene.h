#pragma once
#include "Terrain.h"
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
	virtual void			updateRender			(const CFrustum& frustum);
	bool					updateNode				(iRenderNode* pNode);
	// ----
	virtual void			frameMove				(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render					(int nRenderType)const;
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
	std::list<iRenderNode*>&getRenderNodes			(){return m_RenderNodes;}
	// ----
	void					CalcLightMap			();
	// ----
	GSET_VAR				(bool,				m_b,ShowOctreeBox);
	GSET_VAR				(bool,				m_b,RefreshViewport);
	GSET_CONST_VAR			(Fog&,				m_,Fog);
	GSET_CONST_VAR			(Vec3D&,			m_v,TargetPos);
	// ----
	CSceneData*				getSceneData			(){return m_pSceneData;}
protected:
	CSceneData*				m_pSceneData;
	Octree<iRenderNode*>	m_OctreeRoot;
	std::list<iRenderNode*>	m_RenderNodes;
	bool					m_bRefreshViewport;
	CFocusNode				m_FocusNode;        // The node which has focus
	bool					m_bShowAnimNode;
	bool					m_bShowOctreeBox;

	Fog						m_Fog;
	Vec3D					m_vTargetPos;
	CFrustum				m_OldFrustum;
};