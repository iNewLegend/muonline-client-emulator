#include "Octree.h"

Octree::Octree()
{
	pChild=NULL;
}

Octree::~Octree()
{
	clearObjects();
	if (pChild)
	{
		delete[] pChild;
	}
}

const BBox& Octree::getBBox()const
{
	return bbox;
}

void Octree::clearObjects()
{
// 	FOR_IN(it, m_setObjet)
// 	{
// 		(*it)->release();
// 	}
	m_setObjet.clear();
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].clearObjects();
		}
	}
}

//void Octree::setupObjectBBox()
//{
//	if (pChild)
//	{
//		if (pChild[0].m_setObjet.size()>0)
//		{
//			pChild[0].setupObjectBBox();
//			bboxObject=pChild[0].bboxObject;
//			if (pChild[1].m_setObjet.size()>0)
//			{
//				pChild[1].setupObjectBBox();
//				bboxObject+=pChild[1].bboxObject;
//			}
//		}
//		else if (pChild[1].m_setObjet.size()>0)
//		{
//			pChild[1].setupObjectBBox();
//			bboxObject=pChild[1].bboxObject;
//		}
//	}
//	else
//	{
//		if (m_setObjet.size()>0)
//		{
//			bboxObject=m_setObjet[0]->GetBBox();
//		}
//		for (auto it=m_setObjet.begin();it!=m_setObjet.end();it++)
//		{
//			BBox box = (*it)->GetBBox();
//			bboxObject+=box;
//		}
//	}
//}

void Octree::getObjectsByBBox(const BBox& box,const std::vector<iRenderNode*>& setSrcObject, std::vector<iRenderNode*>& setDestObject)
{
	FOR_IN(it, setSrcObject)
	{
		if (box.crossVertex((*it)->getPos()))
		{
			setDestObject.push_back(*it);
		}
	}
}

//void Octree::getOctreesByFrustum(const CFrustum& frustum, std::vector<Octree*>& setOctree)
//{
//	if (0==m_setObjet.size())
//	{
//		return;
//	}
//	CrossRet crossRet = frustum.CheckAABBVisible(bboxObject);
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

void Octree::getObjects(LIST_RENDER_NODE& setObject)
{
	setObject.insert(setObject.end(), m_setObjet.begin(), m_setObjet.end());
	if(pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getObjects(setObject);
		}
	}
}
void Octree::getObjectsByPos(Vec3D vPos, LIST_RENDER_NODE& setObject)
{
	FOR_IN(it, m_setObjet)
	{
		if ((*it)->getPos()==vPos)
		{
			setObject.push_back(*it);
		}
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getObjectsByPos(vPos,setObject);
		}
	}
}

void Octree::getObjectsByCell(Pos2D posCell, LIST_RENDER_NODE& setObject)
{
	FOR_IN(it, m_setObjet)
	{
		if ((*it)->getPos().x ==posCell.x&&
			(*it)->getPos().z ==posCell.y)
		{
			setObject.push_back(*it);
		}
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getObjectsByCell(posCell,setObject);
		}
	}
}

void Octree::getObjectsByFrustum(const CFrustum& frustum, LIST_RENDER_NODE& setObject)
{
	CrossRet crossRet = frustum.CheckAABBVisible(bbox);
	if (cross_include == crossRet)
	{
		getObjects(setObject);
	}
	else if (cross_cross == crossRet)
	{
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].getObjectsByFrustum(frustum, setObject);
			}
		}
		else
		{
			getObjects(setObject);
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

bool Octree::addObject(const BBox& box, iRenderNode* pObject)
{
	CrossRet crossRet = bbox.checkAABBVisible(box);
	if (cross_exclude != crossRet)
	{
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].addObject(box, pObject);
			}
		}
		else
		{
			m_setObjet.push_back(pObject);
		}
		return true;
	}
	return false;
}

// bool Octree::delObject(iRenderNode* pObject)
// {
// 	if(eraseObject(pObject))
// 	{
// 		pObject->release();	// del self
// 		return true;
// 	}
// 	return false;
// }

bool Octree::eraseObject(iRenderNode* pObject)
{
	bool bRet = false;
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			if (pChild[i].eraseObject(pObject))
			{
				bRet = true;
			}
		}
	}
	else
	{
		auto it = std::find( m_setObjet.begin( ), m_setObjet.end( ), pObject );
		if(it!=m_setObjet.end())
		{
			m_setObjet.erase(it);
			bRet = true;
		}
	}
	return bRet;
}

bool Octree::updateObject(iRenderNode* pObject)
{
	if(eraseObject(pObject))
	{
		addObject(pObject);
		return true;
	}
	return false;
}

Octree* Octree::find(iRenderNode* pObject)
{
	auto it = std::find( m_setObjet.begin( ), m_setObjet.end( ), pObject );
	if(it!=m_setObjet.end())
	{
		return this;
	}
	else if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			Octree* pParentOctree = pChild[i].find(pObject);
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