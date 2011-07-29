#pragma once
#include <algorithm>
#include <set>

template <typename T>
class Octree
{
public:
	Octree()
	:pChild(NULL)
	{
	}

	~Octree()
	{
		clearNodes();
		if (pChild)
		{
			delete[] pChild;
		}
	}

	bool addNode(const BBox& box, T node)
	{
		CrossRet crossRet = bbox.checkAABBVisible(box);
		if (cross_exclude != crossRet)
		{
			if (pChild)
			{
				for (size_t i=0;i<8;++i)
				{
					pChild[i].addNode(box, node);
				}
			}
			else
			{
				m_setNode.push_back(node);
			}
			return true;
		}
		return false;
	}

	void create(const BBox& box, size_t depth)
	{
		bbox = box;
		if (depth>0)
		{
			depth--;
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
				pChild[i].create(childBoxs[i], depth);
			}
		}
	}

	void clearNodes()
	{
		m_setNode.clear();
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].clearNodes();
			}
		}
	}

	bool eraseNode(const T& node)
	{
		bool bRet = false;
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				if (pChild[i].eraseNode(node))
				{
					bRet = true;
				}
			}
		}
		else
		{
			auto it = std::find( m_setNode.begin( ), m_setNode.end( ), node );
			if(it!=m_setNode.end())
			{
				m_setNode.erase(it);
				bRet = true;
			}
		}
		return bRet;
	}

	const BBox& getBBox()const
	{
		return bbox;
	}

	void getNodes(std::set<T>& setNode)const
	{
		setNode.insert(m_setNode.begin(), m_setNode.end());
		if(pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].getNodes(setNode);
			}
		}
	}

	void walkOctree(const CFrustum& frustum, std::set<T>& setNode)const
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
					pChild[i].walkOctree(frustum, setNode);
				}
			}
			else
			{
				getNodes(setNode);
			}
		}
	}

	//Octree*		getNodeByAABB(const BBox& box);
	//void			setupNodeBBox();
	//static void	getNodesByBBox(const BBox& box,const std::vector<T*>& setSrcNode, std::vector<T*>& setDestNode);
	//void			getOctreesByFrustum(const CFrustum& frustum, std::vector<Octree*>& setOctree);
protected:
	Octree*			pChild;
	BBox			bbox;
	std::list<T>	m_setNode;
};

