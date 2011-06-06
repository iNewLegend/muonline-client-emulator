#pragma once
#include "Vec3D.h"

class Plane
{
public:
	Vec3D normal;
	float d;

	Plane();
	Plane(const Vec3D& vNormal, const Vec3D& vPoint);
	Plane(const Vec3D& vPoint0, const Vec3D& vPoint1, const Vec3D& vPoint2);
	int PosToPlane(const Vec3D& p) const;
	float dot(const Vec3D& v) const;
	float dotNormal(const Vec3D& v) const;
};