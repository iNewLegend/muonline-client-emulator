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
	void			clearNodes();
	//void			setupNodeBBox();
	static void		getNodesByBBox(const BBox& box,const std::vector<iRenderNode*>& setSrcNode, std::vector<iRenderNode*>& setDestNode);
	//void			getOctreesByFrustum(const CFrustum& frustum, std::vector<Octree*>& setOctree);
	void			getNodes(LIST_RENDER_NODE& setNode);
	void			getNodesByPos(Vec3D vPos, LIST_RENDER_NODE& setNode);
	void			getNodesByCell(Pos2D posCell, LIST_RENDER_NODE& setNode);
	void			getNodesByFrustum(const CFrustum& frustum, LIST_RENDER_NODE& setNode);
	Octree*			getNodeByAABB(const BBox& box);
	bool			addNode(const BBox& box, iRenderNode* pNode);
	//bool			delNode(iRenderNode* pNode);
	bool			eraseNode(iRenderNode* pNode);
	Octree*			find(iRenderNode* pNode);
	void			create(const BBox& box, size_t size);
protected:
	Octree*				pChild;
	BBox				bbox;
	LIST_RENDER_NODE	m_setNode;
};

