#include "Octree.h"

Octree::Octree()
{
	pChild=NULL;
}

Octree::~Octree()
{
	clearNodes();
	if (pChild)
	{
		delete[] pChild;
	}
}

const BBox& Octree::getBBox()const
{
	return bbox;
}

void Octree::clearNodes()
{
// 	FOR_IN(it, m_setObjet)
// 	{
// 		(*it)->release();
// 	}
	m_setNode.clear();
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].clearNodes();
		}
	}
}

//void Octree::setupNodeBBox()
//{
//	if (pChild)
//	{
//		if (pChild[0].m_setObjet.size()>0)
//		{
//			pChild[0].setupNodeBBox();
//			bboxNode=pChild[0].bboxNode;
//			if (pChild[1].m_setObjet.size()>0)
//			{
//				pChild[1].setupNodeBBox();
//				bboxNode+=pChild[1].bboxNode;
//			}
//		}
//		else if (pChild[1].m_setObjet.size()>0)
//		{
//			pChild[1].setupNodeBBox();
//			bboxNode=pChild[1].bboxNode;
//		}
//	}
//	else
//	{
//		if (m_setObjet.size()>0)
//		{
//			bboxNode=m_setObjet[0]->GetBBox();
//		}
//		for (auto it=m_setObjet.begin();it!=m_setObjet.end();it++)
//		{
//			BBox box = (*it)->GetBBox();
//			bboxNode+=box;
//		}
//	}
//}

void Octree::getNodesByBBox(const BBox& box,const std::vector<iRenderNode*>& setSrcNode, std::vector<iRenderNode*>& setDestNode)
{
	FOR_IN(it, setSrcNode)
	{
		if (box.crossVertex((*it)->getPos()))
		{
			setDestNode.push_back(*it);
		}
	}
}

//void Octree::getOctreesByFrustum(const CFrustum& frustum, std::vector<Octree*>& setOctree)
//{
//	if (0==m_setObjet.size())
//	{
//		return;
//	}
//	CrossRet crossRet = frustum.CheckAABBVisible(bboxNode);
//	if (cross_include == crossRet)
//	{
//		setOctree.push_back(this);
//	}
//	else if (cross_cross == crossRet)
//	{
//		if (pChild)
//		{
//			for (size_t i=0;i<2;++i)
//			{
//				pChild[i].getOctreesByFrustum(frustum, setOctree);
//			}
//		}
//		else
//		{
//			setOctree.push_back(this);
//		}
//	}
//}

void Octree::getNodes(LIST_RENDER_NODE& setNode)
{
	setNode.insert(setNode.end(), m_setNode.begin(), m_setNode.end());
	if(pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getNodes(setNode);
		}
	}
}
void Octree::getNodesByPos(Vec3D vPos, LIST_RENDER_NODE& setNode)
{
	FOR_IN(it, m_setNode)
	{
		if ((*it)->getPos()==vPos)
		{
			setNode.push_back(*it);
		}
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getNodesByPos(vPos,setNode);
		}
	}
}

void Octree::getNodesByCell(Pos2D posCell, LIST_RENDER_NODE& setNode)
{
	FOR_IN(it, m_setNode)
	{
		if ((*it)->getPos().x ==posCell.x&&
			(*it)->getPos().z ==posCell.y)
		{
			setNode.push_back(*it);
		}
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getNodesByCell(posCell,setNode);
		}
	}
}

void Octree::getNodesByFrustum(const CFrustum& frustum, LIST_RENDER_NODE& setNode)
{
	CrossRet crossRet = frustum.CheckAABBVisible(bbox);
	if (cross_include == crossRet)
	{
		getNodes(setNode);
	}
	else if (cross_cross == crossRet)
	{
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].getNodesByFrustum(frustum, setNode);
			}
		}
		else
		{
			getNodes(setNode);
		}
	}
}

Octree* Octree::getNodeByAABB(const BBox& box)
{
	CrossRet crossRet = bbox.checkAABBVisible(box);
	if (cross_include == crossRet)
	{
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				Octree* pNode = pChild[i].getNodeByAABB(box);
				if (pNode)
				{
					return pNode;
				}
			}
		}
		return this;
	}
	return NULL;
}

bool Octree::addNode(const BBox& box, iRenderNode* pNode)
{
	CrossRet crossRet = bbox.checkAABBVisible(box);
	if (cross_exclude != crossRet)
	{
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].addNode(box, pNode);
			}
		}
		else
		{
			m_setNode.push_back(pNode);
		}
		return true;
	}
	return false;
}

// bool Octree::delNode(iRenderNode* pNode)
// {
// 	if(eraseNode(pNode))
// 	{
// 		pNode->release();	// del self
// 		return true;
// 	}
// 	return false;
// }

bool Octree::eraseNode(iRenderNode* pNode)
{
	bool bRet = false;
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			if (pChild[i].eraseNode(pNode))
			{
				bRet = true;
			}
		}
	}
	else
	{
		auto it = std::find( m_setNode.begin( ), m_setNode.end( ), pNode );
		if(it!=m_setNode.end())
		{
			m_setNode.erase(it);
			bRet = true;
		}
	}
	return bRet;
}

Octree* Octree::find(iRenderNode* pNode)
{
	auto it = std::find( m_setNode.begin( ), m_setNode.end( ), pNode );
	if(it!=m_setNode.end())
	{
		return this;
	}
	else if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			Octree* pParentOctree = pChild[i].find(pNode);
			if (pParentOctree)
			{
				return pParentOctree;
			}
		}
	}
	return NULL;
}

void Octree::create(const BBox& box, size_t size)
{
	bbox = box;
	if (size>0)
	{
		size--;
		if (pChild)
		{
			delete[] pChild;
		}
		pChild = new Octree[8];
		BBox childBoxs[8];
		for (size_t i=0;i<8;++i)
		{
			childBoxs[i] = bbox;
		}
		Vec3D vMiddle = (box.vMax+box.vMin)*0.5f;
		childBoxs[0].vMin.z=vMiddle.z;
		childBoxs[1].vMin.z=vMiddle.z;
		childBoxs[2].vMin.z=vMiddle.z;
		childBoxs[3].vMin.z=vMiddle.z;

		childBoxs[4].vMax.z=vMiddle.z;
		childBoxs[5].vMax.z=vMiddle.z;
		childBoxs[6].vMax.z=vMiddle.z;
		childBoxs[7].vMax.z=vMiddle.z;

		childBoxs[0].vMin.y=vMiddle.y;
		childBoxs[1].vMin.y=vMiddle.y;
		childBoxs[4].vMin.y=vMiddle.y;
		childBoxs[5].vMin.y=vMiddle.y;

		childBoxs[2].vMax.y=vMiddle.y;
		childBoxs[3].vMax.y=vMiddle.y;
		childBoxs[6].vMax.y=vMiddle.y;
		childBoxs[7].vMax.y=vMiddle.y;


		childBoxs[0].vMin.x=vMiddle.x;
		childBoxs[2].vMin.x=vMiddle.x;
		childBoxs[4].vMin.x=vMiddle.x;
		childBoxs[6].vMin.x=vMiddle.x;

		childBoxs[1].vMax.x=vMiddle.x;
		childBoxs[3].vMax.x=vMiddle.x;
		childBoxs[5].vMax.x=vMiddle.x;
		childBoxs[7].vMax.x=vMiddle.x;

		for (size_t i=0;i<8;++i)
		{
			pChild[i].create(childBoxs[i], size);
		}
	}
}