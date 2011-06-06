#include "Plane.h"

Plane::Plane()
{
}

Plane::Plane(const Vec3D& vNormal, const Vec3D& vPoint)
{
	normal = vNormal;
	normal.normalize();
	d = -normal.dot(vPoint);
}

Plane::Plane(const Vec3D& vPoint0, const Vec3D& vPoint1, const Vec3D& vPoint2)
{
	Vec3D vEdge1 = vPoint1 - vPoint0;
	Vec3D vEdge2 = vPoint2 - vPoint0;
	normal = vEdge1.cross(vEdge2);
	normal.normalize();
	d = -normal.dot(vPoint0);
}

int Plane::PosToPlane(const Vec3D& p) const
{
	float t=normal.dot(p)+d;
	if(t>0) return 1;
	else if(t<0) return -1;
	else return 0;
}

float Plane::dot(const Vec3D& v) const
{
	return normal.x*v.x+normal.y*v.y+normal.z*v.z+d;
}

float Plane::dotNormal(const Vec3D& v) const
{
	return normal.x*v.x+normal.y*v.y+normal.z*v.z;
}