#pragma once
#include "Vec2D.h"
#include "Frustum.h"

struct VertexIndex
{
	unsigned short p;
	unsigned short n;
	unsigned short c;
	unsigned short uv1;
	unsigned short uv2;
	unsigned short w;
	unsigned short b;

	VertexIndex()
	{
		memset(this,0,sizeof(*this));
	}

	bool operator< (const VertexIndex& v) const
	{
		const unsigned char* p1=(const unsigned char*)this;
		const unsigned char* p2=(const unsigned char*)&v;
		for (size_t i=0;i<sizeof(VertexIndex);++i)
		{
			if (*p1!=*p2)
			{
				return *p1<*p2;
			}
			p1++;p2++;
		}
		return false;
	}
	bool operator== (const VertexIndex& v) const
	{
		return p==v.p&&n==v.n&&c==v.c&&uv1==v.uv1&&uv2==v.uv2&&w==v.w&&b==v.b;
	}
};

class CSubMesh
{
public:
	void addMaterial(const char* szMaterial)
	{material.push_back(szMaterial);}
	const char* getMaterial(size_t n)
	{
		if (material.size()>n)
		{
			return material[n].c_str();
		}
		return NULL;
	}
	void getVertexIndex(size_t n, VertexIndex& vertexIndex)
	{
		if (m_setVertexIndex.size()>n)
		{
			vertexIndex=m_setVertexIndex[n];
		}
	}

	std::vector<std::string>	material;


	std::vector<VertexIndex> m_setVertexIndex;
};

class iLodMesh
{
public:
	virtual CSubMesh&	allotSubMesh()=0;
	virtual int			getSubCount()=0;
	virtual CSubMesh*	getSubMesh(size_t n)=0;
	virtual const BBox&	getBBox()=0;
	virtual void		setBBox(const BBox& bbox)=0;
	virtual void		init()=0;

	virtual void					setVertexSize(const unsigned short vertexSize)=0;
	virtual const unsigned short	getVertexSize()const=0;

	virtual char*		createVB(size_t size)=0;
	virtual char*		getVB()=0;
	virtual size_t		getVBSize()=0;
	virtual std::vector<unsigned short>&	getIB()=0;

	template <class _T>
	void  setVectorValue(std::vector<_T>& vec, size_t pos, const _T& val)
	{
		if (vec.size()<=pos)
		{
			vec.resize(pos+1);
		}
		vec.push_back(val);
	}


	template <class _T>
	void  getVectorValue(const std::vector<_T>& vec, size_t pos, _T& val)
	{
		if (vec.size()>pos)
		{
			val=vec[pos];
		}
	}

};