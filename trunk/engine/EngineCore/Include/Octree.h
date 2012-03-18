#pragma once
#include <algorithm>
#include <set>
#include <list>

template <typename T>
class Octree
{
public:
	bool addNode(const BBox& box, T node)
	{
		if (m_bbox.checkAABBVisible(box) == cross_exclude)
		{
			return false;
		}
		BBox bbox = m_bbox;
		int nLength = 64;
		int index = 0;
		while (1)
		{
			int nSize = nLength*nLength*nLength;
			CrossRet crossRet = bbox.checkAABBVisible(box);
			if (cross_exclude != crossRet)
			{
				if (nLength>1)
				{
					nLength/=2;
					bbox.vMax-=(bbox.vMax-bbox.vMin)*0.5f;
					continue;
				}
				else
				{
					m_setNode[index].push_back(node);
				}
			}
			// Inc Index
			index+=nSize;
			// Check The Size
			if (index>=64*64*64)
			{
				return true;
			}
			// Get Length
			nLength = 1;
			for (int i=index; i!=0; i>>=3)
			{
				if ((i&0x7)!=0)
				{
					break;
				}
				nLength*=2;
			}
			// Get New Box
			bbox.vMin = m_BoxMin[index];
			bbox.vMax = bbox.vMin+m_vLength*nLength;
		}
		return true;
	}

	void create(const BBox& box, size_t depth)
	{
		m_bbox = box;
		m_vLength = (m_bbox.vMax-m_bbox.vMin)/64.0f;
		Vec3D vTemp;
		for (size_t i=0;i<64*64*64;++i)
		{
			m_BoxMin[i] = m_bbox.vMin;
			vTemp = m_vLength;
			for (int n=i; n!=0; n>>=3)
			{
				if(n&0x1) m_BoxMin[i].x += vTemp.x;
				if(n&0x2) m_BoxMin[i].y += vTemp.y;
				if(n&0x4) m_BoxMin[i].z += vTemp.z;
				vTemp*=2.0f;
			}
		}
	}

	void clearNodes()
	{
		for (size_t i=0;i<64*64*64;++i)
		{
			m_setNode[i].clear();
		}
	}

	void eraseNode(const T& node)
	{
		for (size_t i=0;i<64*64*64;++i)
		{
			auto it = std::find( m_setNode[i].begin(), m_setNode[i].end(), node);
			if(it!=m_setNode[i].end())
			{
				m_setNode[i].erase(it);
			}
		}
	}

	const BBox& getBBox()const
	{
		return m_bbox;
	}

	void walkOctree(const CFrustum& frustum, std::set<T>& setNode)const
	{
		BBox bbox = m_bbox;
		int nLength = 64;
		int index = 0;
		while (1)
		{
			int nSize = nLength*nLength*nLength;
			CrossRet crossRet = frustum.CheckAABBVisible(bbox);
			if (cross_include == crossRet)
			{
				for (int i=index; i<index+nSize; ++i)
				{
					setNode.insert(m_setNode[i].begin(), m_setNode[i].end());
				}
			}
			else if (cross_cross == crossRet)
			{
				if (nLength>1)
				{
					nLength/=2;
					bbox.vMax-=(bbox.vMax-bbox.vMin)*0.5f;
					continue;
				}
				else
				{
					setNode.insert(m_setNode[index].begin(), m_setNode[index].end());
				}
			}
			// Inc Index
			index+=nSize;
			// Check The Size
			if (index>=64*64*64)
			{
				return;
			}
			// Get Length
			nLength = 1;
			for (int i=index; i!=0; i>>=3)
			{
				if ((i&0x7)!=0)
				{
					break;
				}
				nLength*=2;
			}
			// Get New Box
			bbox.vMin = m_BoxMin[index];
			bbox.vMax = bbox.vMin+m_vLength*nLength;
		}
	}
protected:
	BBox			m_bbox;
	Vec3D			m_vLength;
	Vec3D			m_BoxMin[64*64*64];
	std::list<T>	m_setNode[64*64*64];
};

