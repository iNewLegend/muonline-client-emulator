#pragma once
#include "MapObj.h"
#include "RenderNode.h"
#include <algorithm>

class ObjectTree
{
protected:
	ObjectTree* pChild;
	BBox bbox;
	LIST_RENDER_NODE m_setObjet;
public:
	ObjectTree();
	~ObjectTree();
	const BBox& getBBox()const;
	void clearObjects();
	//void setupObjectBBox();
	static void getObjectsByBBox(const BBox& box,const std::vector<CRenderNode*>& setSrcObject, std::vector<CRenderNode*>& setDestObject);
	//void getObjectTreesByFrustum(const CFrustum& frustum, std::vector<ObjectTree*>& setObjectTree);
	void getObjects(LIST_RENDER_NODE& setObject);
	void getObjectsByPos(Vec3D vPos, LIST_RENDER_NODE& setObject);
	void getObjectsByCell(Pos2D posCell, LIST_RENDER_NODE& setObject);
	void getObjectsByFrustum(const CFrustum& frustum, LIST_RENDER_NODE& setObject);
	ObjectTree* getNodeByAABB(const BBox& box);
	bool addObject(CRenderNode* pObject);
	//bool delObject(CRenderNode* pObject);
	bool eraseObject(CRenderNode* pObject);
	bool updateObject(CRenderNode* pObject);
	ObjectTree* find(CRenderNode* pObject);
	void create(const BBox& box, size_t size);
	void process();
};

