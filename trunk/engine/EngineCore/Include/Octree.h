#pragma once
#include "MapObj.h"
#include "RenderNode.h"
#include <algorithm>

class Octree
{
public:
	Octree();
	~Octree();
	const BBox&		getBBox()const;
	void			clearObjects();
	//void setupObjectBBox();
	static void		getObjectsByBBox(const BBox& box,const std::vector<iRenderNode*>& setSrcObject, std::vector<iRenderNode*>& setDestObject);
	//void getOctreesByFrustum(const CFrustum& frustum, std::vector<Octree*>& setOctree);
	void			getObjects(LIST_RENDER_NODE& setObject);
	void			getObjectsByPos(Vec3D vPos, LIST_RENDER_NODE& setObject);
	void			getObjectsByCell(Pos2D posCell, LIST_RENDER_NODE& setObject);
	void			getObjectsByFrustum(const CFrustum& frustum, LIST_RENDER_NODE& setObject);
	Octree*			getNodeByAABB(const BBox& box);
	bool			addObject(iRenderNode* pObject);
	//bool			delObject(iRenderNode* pObject);
	bool			eraseObject(iRenderNode* pObject);
	bool			updateObject(iRenderNode* pObject);
	Octree*			find(iRenderNode* pObject);
	void			create(const BBox& box, size_t size);
protected:
	Octree*				pChild;
	BBox				bbox;
	LIST_RENDER_NODE	m_setObjet;
};

