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
	void addPos(const Vec3D& vPos)
	{pos.push_back(vPos);}
	void addBone(unsigned long uBone)
	{bone.push_back(uBone);}
	void addWeight(unsigned long uWeight)
	{weight.push_back(uWeight);}
	void addNormal(const Vec3D& vNormal)
	{normal.push_back(vNormal);}
	void addColor(const Color32& clr)
	{color.push_back(clr);}
	void addTexcoord(const Vec2D& vUV)
	{texcoord.push_back(vUV);}
	void addMaterial(const char* szMaterial)
	{material.push_back(szMaterial);}

	template <class _T>
	void  setVectorValue(std::vector<_T>& vec, size_t pos, const _T& val)
	{
		if (vec.size()<=pos)
		{
			vec.resize(pos+1);
		}
		vec.push_back(val);
	}

	void setPos(size_t n, const Vec3D& vPos)
	{
		setVectorValue(pos,n,vPos);
	}
	void setBone(size_t n, unsigned long uBone)
	{
		setVectorValue(bone,n,uBone);
	}
	void setWeight(size_t n, unsigned long uWeight)
	{
		setVectorValue(weight,n,uWeight);
	}
	void setNormal(size_t n, const Vec3D& vNormal)
	{
		setVectorValue(normal,n,vNormal);
	}
	void setTexcoord(size_t n, const Vec2D& vUV)
	{
		setVectorValue(texcoord,n,vUV);
	}

	template <class _T>
	void  getVectorValue(const std::vector<_T>& vec, size_t pos, _T& val)
	{
		if (vec.size()>pos)
		{
			val=vec[pos];
		}
	}

	void getVertexIndex(size_t n, VertexIndex& vertexIndex)
	{
		getVectorValue(m_setVertexIndex,n,vertexIndex);
	}

	void getPos(size_t n, Vec3D& vPos)
	{
		getVectorValue(pos,n,vPos);
	}
	void getBone(size_t n, unsigned long& uBone)
	{
		getVectorValue(bone,n,uBone);
	}
	void getWeight(size_t n, unsigned long& uWeight)
	{
		getVectorValue(weight,n,uWeight);
	}
	void getNormal(size_t n, Vec3D& vNormal)
	{
		getVectorValue(normal,n,vNormal);
	}
	void getTexcoord(size_t n, Vec2D& vUV)
	{
		getVectorValue(texcoord,n,vUV);
	}
	const char* getMaterial(size_t n)
	{
		if (material.size()>n)
		{
			return material[n].c_str();
		}
		return NULL;
	}

	std::vector<std::string>	material;
	std::vector<Vec3D>			pos;
	std::vector<unsigned long>	weight;
	std::vector<unsigned long>	bone;
	std::vector<Vec3D>			normal;
	std::vector<Color32>		color;
	std::vector<Vec2D>			texcoord;
	std::vector<Vec2D>			texcoord2;

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
};