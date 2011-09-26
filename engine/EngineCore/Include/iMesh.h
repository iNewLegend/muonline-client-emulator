#pragma once
#include "Vec2D.h"
#include "Frustum.h"
#include "RenderSystemCommon.h"

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

struct SkinVertex
{
	Vec3D	p;
	Vec3D	n;
	SkinVertex()
	{
		w4=0;
		b4=0;
	}
	union{
		unsigned char	w[4];
		unsigned long	w4;
	};
	union{
		unsigned char	b[4];
		unsigned long	b4;
	};
	Vec2D uv;
};

struct RigidVertex
{
	Vec3D	p;
	Vec3D	n;
	Vec2D	uv;
};

struct RigidNolightVertex
{
	Vec3D	p;
	Color32	c;
	Vec2D	uv;
};

class iMeshData
{
public:
	virtual const BBox&	getBBox()=0;
	virtual void		setBBox(const BBox& bbox)=0;

	virtual std::vector<SkinVertex>&				getSkinVertices()=0;
	virtual std::vector<RigidVertex>&				getRigidVertices()=0;
	virtual std::vector<RigidNolightVertex>&		getRigidNolightVertices()=0;
	virtual std::vector<unsigned short>&			getIndices()=0;
	virtual std::vector<IndexedSubset>&				getSubsets()=0;
	virtual std::vector<std::vector<std::string>>&	getMaterials()=0;
};