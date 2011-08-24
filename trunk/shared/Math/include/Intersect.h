#pragma once
#include "Vec3D.h"
#include "Plane.h"
#include "Matrix.h"
#include "assert.h"
#include "wtypes.h"

inline void GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir, int x, int y, const Matrix mView, const Matrix mProj, const RECT& rc)
{
	Vec3D v;
	v.x =  (((2.0f * (x-rc.left)) / (rc.right-rc.left)) - 1) / mProj._11;
	v.y = -(((2.0f * (y-rc.top)) / (rc.bottom-rc.top)) - 1) / mProj._22;
	v.z =  1.0f;

	Matrix mViewInv = mView;
	mViewInv.Invert();
	vRayPos = mViewInv*Vec3D(0.0f,0.0f,0.0f);
	vRayDir = mViewInv*v-vRayPos;
	vRayDir.normalize();
}

inline bool IntersectTri(const Vec3D& v0, const Vec3D& v1, const Vec3D& v2, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D& vOut)
{
	float a = (v0-vRayPos).cross(v1-vRayPos).dot(vRayDir);
	float b = (v1-vRayPos).cross(v2-vRayPos).dot(vRayDir);
	float c = (v2-vRayPos).cross(v0-vRayPos).dot(vRayDir);
	if ((a<=0&&b<=0&&c<=0)||(a>=0&&b>=0&&c>=0))
	{
		vOut = (a*v2+b*v0+c*v1)/(a+b+c);
		return true;
	}
	return false;
}

inline int LineSphereIntersector(const Vec3D &p, const Vec3D &d,const Vec3D &Center, float radius, float & tmin, float & tmax)
{
	Vec3D m = p - Center;
	float a = d.dot(d);
	float b = m.dot(d);
	float c = m.dot(m) - radius * radius;
	assert(a != 0);
	float discr = b * b - a * c;
	//No intersect 
	if(discr < 0)
		return 0;
	tmin = (-b - sqrtf(discr)) / a;
	tmax = (-b + sqrtf(discr)) / a;
	if(discr == 0)// Root Num =1;
		return 1;
	else// Root Num =2
		return 2;
}

inline int LineCapsuleIntersector(const Vec3D &sa, const Vec3D &n, const Vec3D &p, const Vec3D &q , float r, float & tmin, float & tmax)
{
	//r = r + r * 0.0001f;// Expand R A little to prevent Error At Cylinder-SemiSphere intersect Line
	Plane Capp(p - q, p );
	Plane Capq(q - p, q );
	Vec3D m = sa - p;
	Vec3D d = q - p;
	float md = m.dot(d);
	float nd = n.dot(d);
	float dd = d.dot(d);
	//
	float nn = n.dot(n);
	float mn = m.dot(n);
	float a = dd * nn - nd * nd;
	float k = m.dot(m) - r * r;
	float c = dd * k - md * md;
	//if the Line is parallel to the cylinder's axis
	assert(dd != 0&& nn !=0);
	float sqrtcostheta = (nd * nd) / (dd * nn);
	if(sqrtcostheta > 0.99999f)//nearly parallel
	{
		if(c > 0) return 0;//a lies outside the cylinder and thus Line Lies out
		//Calcuate the intersector with 2 EndCap
		float smin , smax;
		//Cacluate the min Intersect Point
		int intersectnum = LineSphereIntersector(sa, n , p , r , smin , smax);
		//No intersect with the first Sphere this must be caused by the error,so use the Cap Plane
		if( intersectnum == 0) 
			tmin = -mn / nn;
		else if( Capp.PosToPlane(sa + n * smin) >= 0 )// if smin is in positive sphere
			tmin = smin;
		else//else we take smax
			tmin = smax;

		intersectnum = LineSphereIntersector(sa, n , q , r , smin , smax);
		//No intersect with the first Sphere this must be caused by the error,so use the Cap Plane
		if( intersectnum == 0) 
			tmax = (nd - mn) / nn;
		if( Capq.PosToPlane(sa + n * smin) >= 0 )
			tmax = smin;
		else
			tmax = smax;

		if(tmin > tmax)
		{
			float t = tmin; tmin = tmax; tmax = t;
		}
		return 2;
	}
	//Line Not Paralle to Axis
	float b = dd * mn- nd * md;
	float discr = b * b - a * c;
	if(discr < 0 ) return 0;//No intersection Point

	tmin = (-b - sqrtf(discr)) / a;
	tmax = (-b + sqrtf(discr)) / a;
	if(tmin > tmax)
	{ float t = tmin; tmin = tmax; tmax = t;}


	if((md + tmin * nd )< 0)
	{
		//Intersection out sidee cylinder's one End Cap;
		float smin , smax;
		int intersectnum = LineSphereIntersector(sa, n , p , r , smin , smax);
		if( intersectnum == 0)
			return 0;
		else if( intersectnum == 1)
		{   //In the Positive Sphere space's Cylinder  
			//if there is a intersection it should always intersect in the Positive Speher
			//So We Do not need to test whether the Point is in the Positive Sphere or Not
			tmin = tmax = smin;
			return 1;
		}
		else if( intersectnum == 2)
		{
			//In this Case At Least tmin intersect Point is in the Positive Sphere
			tmin = smin;
			//If the Max InterSect Point is also in the Positive Spehre
			if(Capp.PosToPlane(sa + n * smax) >= 0)
			{
				tmax = smax;
				return 2;
			}
			//Else do nothing because the tmax should be Refound in other Caps Sphere or in side
			//the Cylinder
		}
		else
			assert(0);
	}
	else if(md + tmin * nd > dd)
	{
		//Intersection out sidee cylinder's one End Cap;
		float smin , smax;
		int intersectnum = LineSphereIntersector(sa, n , q , r , smin , smax);
		if( intersectnum == 0)
			return false;
		else if( intersectnum == 1)
		{   //In the Positive Sphere space's Cylinder  
			//if there is a intersection it should always intersect in the Positive Speher
			//So We Do not need to test whether the Point is in the Positive Sphere or Not
			tmin = tmax = smin;
			return 1;
		}
		else if( intersectnum == 2)
		{
			//In this Case At Least tmin intersect Point is in the Positive Sphere
			tmin = smin;
			//If the Max InterSect Point is also in the Positive Spehre
			if(Capq.PosToPlane(sa + n * smax) >= 0)
			{
				tmax = smax;
				return 2;
			}
			//Else do nothing because the tmax should be Refound in other Caps Sphere or in side
			//the Cylinder
		}
		else
			assert(0);
	}
	//Now Adjust Max Point
	if(nd < 0)//out side cylinder's one End Cap;
	{//Adjust tmax 
		float smin , smax;
		int intersectnum = LineSphereIntersector(sa, n , p , r , smin , smax);
		if( intersectnum > 0)
		{
			if( Capp.PosToPlane(sa + n * smax) >= 0)
			{
				tmax = smax;
			}
			//else Reserve tmax
		}
		//else Reserve tmax
	}
	else if(nd > 0)
	{//Adjust tmax 
		float smin , smax;
		int intersectnum = LineSphereIntersector(sa, n , q , r , smin , smax);
		if( intersectnum > 0)
		{
			if( Capq.PosToPlane(sa + n * smax) >= 0)
			{
				tmax = smax;
			}
			//else Reserve tmax
		}
		//else Reserve tmax
	}
	else//Do not need Adjust tmax 
	{ ;}
	//One Root Or 2
	if(discr == 0)
		return 1;
	else// > 0
		return 2;
}

inline void transformRay(Vec3D& vRayPos, Vec3D& vRayDir,const Matrix& mat)
{
	Matrix m = mat;
	m.Invert();
	vRayPos = m*vRayPos;
	m._14=0.0f;m._24=0.0f;m._34=0.0f;
	vRayDir = m*vRayDir;
	vRayDir.normalize();
}
