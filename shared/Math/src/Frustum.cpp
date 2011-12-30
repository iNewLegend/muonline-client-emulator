#include "Frustum.h"
#include "float.h"
#include <windows.h>// for min() and max()

BBox::BBox()
{
	vMin.x=FLT_MAX;
	vMin.y=FLT_MAX;
	vMin.z=FLT_MAX;
	vMax.x=FLT_MIN;
	vMax.y=FLT_MIN;
	vMax.z=FLT_MIN;
};

BBox::BBox(float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ)
{
	vMin.x=fMinX;
	vMin.y=fMinY;
	vMin.z=fMinZ;
	vMax.x=fMaxX;
	vMax.y=fMaxY;
	vMax.z=fMaxZ;
}

BBox BBox::operator+ (const BBox &b) const
{
	BBox box;
	box.vMin.x=min(vMin.x,b.vMin.x);
	box.vMin.y=min(vMin.y,b.vMin.y);
	box.vMin.z=min(vMin.z,b.vMin.z);
	box.vMax.x=max(vMax.x,b.vMax.x);
	box.vMax.y=max(vMax.y,b.vMax.y);
	box.vMax.z=max(vMax.z,b.vMax.z);
	return box;
}

BBox& BBox::operator+= (const BBox &b)
{
	vMin.x=min(vMin.x,b.vMin.x);
	vMin.y=min(vMin.y,b.vMin.y);
	vMin.z=min(vMin.z,b.vMin.z);
	vMax.x=max(vMax.x,b.vMax.x);
	vMax.y=max(vMax.y,b.vMax.y);
	vMax.z=max(vMax.z,b.vMax.z);
	return *this;
}

bool BBox::crossVertex(const Vec3D& vPos)const
{
	if(vMin.x<vPos.x && vMin.y<vPos.y && vMin.z<vPos.z && vMax.x>vPos.x && vMax.y>vPos.y && vMax.z>vPos.z)
		return true;
	return false;
}

CrossRet BBox::checkAABBVisible(const BBox& bbox) const
{
	// first check exclude.
	for (size_t i=0; i<3; ++i)
	{
		if (vMin.f[i]>bbox.vMax.f[i])
		{
			return cross_exclude;
		}
		if (vMax.f[i]<bbox.vMin.f[i])
		{
			return cross_exclude;
		}
	}
	// second check cross.
	for (size_t i=0; i<3; ++i)
	{
		if (vMin.f[i]>bbox.vMin.f[i])
		{
			return cross_cross;
		}
		if (vMax.f[i]<bbox.vMax.f[i])
		{
			return cross_cross;
		}
	}
	// last include only.
	return cross_include;
}

bool BBox::interSectLine(const Vec3D &p,const Vec3D &d,float &tmin,float&tmax) const
{
	tmin = -FLT_MAX;
	tmax = FLT_MAX;
	for (int i=0; i<3; ++i)
	{
		//Test the XYZ Slab
		float fMin, fMax, fP, fD;
		fMin = vMin.f[i];
		fMax = vMax.f[i];
		fP = p.f[i];
		fD = d.f[i];
		if(fabsf(fD) < 0.00001f)
		{
			if(fP < fMin || fP > fMax)
				return false;
		}
		else
		{
			float ood = 1.0f/fD;
			float t1 = (fMin - fP) * ood;
			float t2 = (fMax - fP) * ood;
			//Swap to make t1,t2  in increase oreder
			if(t1 > t2)
			{
				float t = t1; t1 = t2; t2 = t;
			}
			if(t1 > tmin) tmin = t1;
			if(t2 < tmax) tmax = t2;
			if(tmin > tmax) return false;
		}
	}
	return true;
}

bool BBox::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax) const
{
	bool intersected = interSectLine(vRayPos , vRayDir,tmin, tmax);
	if( intersected == false)
		return false;
	else
	{
		if(tmax < 0)//If intersect Point Go Out of the Ray Fragment
			return false;
		else//Recalculate tmin
		{
			tmin = tmin > 0 ? tmin : 0;
			//assert(tmin <= tmax);
			return true;
		}
	}
}

CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

Vec3D CFrustum::getEyePoint()const
{
	return (m_vtx[0]+m_vtx[1]+m_vtx[2]+m_vtx[3])*0.25f;
}

void CFrustum::Build(Vec3D vecMin,Vec3D vecMax)
{
	m_vtx[0] = Vec3D(vecMin.x, vecMin.y,  vecMin.z); // xyz
	m_vtx[1] = Vec3D(vecMax.x, vecMin.y,  vecMin.z); // Xyz
	m_vtx[2] = Vec3D(vecMin.x, vecMax.y,  vecMin.z); // xYz
	m_vtx[3] = Vec3D(vecMax.x, vecMax.y,  vecMin.z); // XYz
	m_vtx[4] = Vec3D(vecMin.x, vecMin.y,  vecMax.z); // xyZ
	m_vtx[5] = Vec3D(vecMax.x, vecMin.y,  vecMax.z); // XyZ
	m_vtx[6] = Vec3D(vecMin.x, vecMax.y,  vecMax.z); // xYZ
	m_vtx[7] = Vec3D(vecMax.x, vecMax.y,  vecMax.z); // XYZ

	m_planes.resize(6);
	m_planes[0]=Plane(m_vtx[0], m_vtx[1], m_vtx[2]); // Near
	m_planes[1]=Plane(m_vtx[6], m_vtx[7], m_vtx[5]); // Far
	m_planes[2]=Plane(m_vtx[2], m_vtx[6], m_vtx[4]); // Left
	m_planes[3]=Plane(m_vtx[7], m_vtx[3], m_vtx[5]); // Right
	m_planes[4]=Plane(m_vtx[2], m_vtx[3], m_vtx[6]); // Top
	m_planes[5]=Plane(m_vtx[1], m_vtx[0], m_vtx[4]); // Bottom
}

void CFrustum::Build(const Matrix& matProjView)
{
	// 求得view * proj的逆矩阵.
	Matrix mat = matProjView;
	mat.Invert();
	//xyzw = {proj*View}[-1]*{-1,-1,0,1}
	//xyz = ({proj*View}[-1]*{-1,-1,0,1})/w
	//取逆
	m_vtx[0] = (mat*Vec4D(-1.0f, -1.0f,  0.0f, 1.0f)).xyzDivideByW(); // xyz
	m_vtx[1] = (mat*Vec4D( 1.0f, -1.0f,  0.0f, 1.0f)).xyzDivideByW(); // Xyz
	m_vtx[2] = (mat*Vec4D(-1.0f,  1.0f,  0.0f, 1.0f)).xyzDivideByW(); // xYz
	m_vtx[3] = (mat*Vec4D( 1.0f,  1.0f,  0.0f, 1.0f)).xyzDivideByW(); // XYz
	m_vtx[4] = (mat*Vec4D(-1.0f, -1.0f,  1.0f, 1.0f)).xyzDivideByW(); // xyZ
	m_vtx[5] = (mat*Vec4D( 1.0f, -1.0f,  1.0f, 1.0f)).xyzDivideByW(); // XyZ
	m_vtx[6] = (mat*Vec4D(-1.0f,  1.0f,  1.0f, 1.0f)).xyzDivideByW(); // xYZ
	m_vtx[7] = (mat*Vec4D( 1.0f,  1.0f,  1.0f, 1.0f)).xyzDivideByW(); // XYZ

	// 通过得到的世界坐标制作平截头体平面.
	// 向量由平截头体内部指向外部的平面.
	m_planes.resize(6);
	m_planes[0]=Plane(m_vtx[0], m_vtx[1], m_vtx[2]); // Near
	m_planes[1]=Plane(m_vtx[6], m_vtx[7], m_vtx[5]); // Far
	m_planes[2]=Plane(m_vtx[2], m_vtx[6], m_vtx[4]); // Left
	m_planes[3]=Plane(m_vtx[7], m_vtx[3], m_vtx[5]); // Right
	m_planes[4]=Plane(m_vtx[2], m_vtx[3], m_vtx[6]); // Top
	m_planes[5]=Plane(m_vtx[1], m_vtx[0], m_vtx[4]); // Bottom
}

void CFrustum::buildNoNear(const Matrix& matProjView)
{
	// 求得view * proj的逆矩阵.
	Matrix mat = matProjView;
	mat.Invert();
	//xyzw = {proj*View}[-1]*{-1,-1,0,1}
	//xyz = ({proj*View}[-1]*{-1,-1,0,1})/w
	//取逆
	m_vtx[0] = (mat*Vec4D(-1.0f, -1.0f,  0.0f, 1.0f)).xyzDivideByW(); // xyz
	m_vtx[1] = (mat*Vec4D( 1.0f, -1.0f,  0.0f, 1.0f)).xyzDivideByW(); // Xyz
	m_vtx[2] = (mat*Vec4D(-1.0f,  1.0f,  0.0f, 1.0f)).xyzDivideByW(); // xYz
	m_vtx[3] = (mat*Vec4D( 1.0f,  1.0f,  0.0f, 1.0f)).xyzDivideByW(); // XYz
	m_vtx[4] = (mat*Vec4D(-1.0f, -1.0f,  1.0f, 1.0f)).xyzDivideByW(); // xyZ
	m_vtx[5] = (mat*Vec4D( 1.0f, -1.0f,  1.0f, 1.0f)).xyzDivideByW(); // XyZ
	m_vtx[6] = (mat*Vec4D(-1.0f,  1.0f,  1.0f, 1.0f)).xyzDivideByW(); // xYZ
	m_vtx[7] = (mat*Vec4D( 1.0f,  1.0f,  1.0f, 1.0f)).xyzDivideByW(); // XYZ

	// 通过得到的世界坐标制作平截头体平面.
	// 向量由平截头体内部指向外部的平面.
	m_planes.resize(5);
	m_planes[0]=Plane(m_vtx[6], m_vtx[7], m_vtx[5]); // Far
	m_planes[1]=Plane(m_vtx[2], m_vtx[6], m_vtx[4]); // Left
	m_planes[2]=Plane(m_vtx[7], m_vtx[3], m_vtx[5]); // Right
	m_planes[3]=Plane(m_vtx[2], m_vtx[3], m_vtx[6]); // Top
	m_planes[4]=Plane(m_vtx[1], m_vtx[0], m_vtx[4]); // Bottom
}

/// 即使不包含在平截头体内，稍微增加参数，使之包含在平截头体内的值
#define PLANE_EPSILON	5.0f
/// 如果一个点 v在平截头体内，就会返回true，反之为false.
bool CFrustum::IsIn(const Vec3D& vector)
{
	float fDist;
	for(size_t i=0; i<m_planes.size(); ++i)
	{
		fDist = m_planes[i].dot(vector);
		if(fDist < -PLANE_EPSILON) return false;	// plane的 normal向量指向 far，如果为正数，表示在平截头体外部.
	}
	return true;
}

/** 如果具有中心(v)和 半径(radius)的边界球体位于平截头体内，
 *  就会返回true，反之为false.
 */
bool CFrustum::IsInSphere(const Vec3D& vector, float radius)
{
	float fDist;
	for(size_t i=0; i<m_planes.size(); ++i)
	{
		fDist = m_planes[i].dot(vector);
		if(fDist+radius < -PLANE_EPSILON)//PLANE_EPSILON
			return false;	// 平面和中心圆点的距离比半径大的话，表示不在平截头体内.
	}
	return true;
}

CrossRet CFrustum::CheckAABBVisible(const BBox& bbox) const
{
	Vec3D	vMinPt, vMaxPt;
	bool bIntersecting = false;

	const float* pMin = &bbox.vMin.x;
	const float* pMax = &bbox.vMax.x;
	float* pMinPt = &vMinPt.x;
	float* pMaxPt = &vMaxPt.x;
	for(size_t i=0; i<m_planes.size(); i++)
	{
		const float* pPlane = &m_planes[i].normal.x;
		for (int j=0; j<3; j++)
		{
			if (pPlane[j] >= 0.0f)
			{
				pMinPt[j] = pMin[j];
				pMaxPt[j] = pMax[j];
			}
			else
			{
				pMinPt[j] = pMax[j];
				pMaxPt[j] = pMin[j];
			}
		}
		float fMinDistance = m_planes[i].dot(vMaxPt);
		if (fMinDistance<0.0f)
			return cross_exclude;

		float fMaxDistance = m_planes[i].dot(vMinPt);
		if (fMaxDistance<0.0f)
			bIntersecting=true;
	}
	return bIntersecting?cross_cross:cross_include;
}