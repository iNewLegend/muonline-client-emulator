#pragma once
#include "Template4D.h"
#include "Vec3D.h"
#include "Color.h"

class Vec4D: public Template4D<float>
{
public:
	Vec4D(){}
	Vec4D(float x,float y,float z, float w):Template4D(x,y,z,w){}
	Vec4D(const Template4D<float>& v):Template4D<float>(v){}
	Vec4D(const Vec3D& v, const float w0);
	Vec4D(const Color32& Color);
	float Dot(const Vec4D &v) const;
	float lengthSquared() const;
	float length() const;
	Vec4D& normalize();
	operator float*();
	Vec3D xyz() const;
	Vec3D xyzDivideByW() const;
	Color32 getColor() const;
};