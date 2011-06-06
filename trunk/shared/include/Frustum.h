#pragma once
#include "Matrix.h"
#include "Plane.h"
#include "vector"

//Cross Return
enum CrossRet
{
	//不包含
	cross_exclude
	//包含
	,cross_include
	//交叉
	,cross_cross
};

class BBox
{
public:
	BBox();
	Vec3D vMin,vMax;
	BBox operator+ (const BBox &b) const;
	BBox& operator+= (const BBox &b);
	bool crossVertex(const Vec3D& vPos)const;
	bool interSectLine(const Vec3D &p,const Vec3D &d,float &tmin,float&tmax) const;
	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax) const;
	CrossRet checkAABBVisible(const BBox& bbox) const;
};

// 平截头体
class CFrustum
{
	Vec3D	m_vtx[8];	// 构成平截头体的8个顶点
	std::vector<Plane> m_planes; // 构成平截头体的6个平面
public:
	CFrustum();
	~CFrustum();

	bool operator==(const CFrustum & frustum)
	{
		for(int i = 0 ; i < 8 ; i ++)
		{
			if(m_vtx[i]!=frustum.m_vtx[i])
				return false;
		}
		return true;
	}
	//
	Vec3D getEyePoint()const;
	//构建视锥
	void Build(const Matrix& matProjView);
	void buildNoNear(const Matrix& matProjView);
	void Build(Vec3D vecMin,Vec3D vecMax);

	/// 点v在平截头体内，返回TRUE，不在平截头体内，返回FALSE.
	bool IsIn(const Vec3D& vector);

	/** 具有中心圆点（v）和半径（radius）的边界球体（bounding sphere）在平截头体内，
	*  返回TRUE，不在平截头体内，返回FALSE.
	*/
	bool IsInSphere(const Vec3D& vector, float radius);

	CrossRet CheckAABBVisible(const BBox& bbox) const;
};