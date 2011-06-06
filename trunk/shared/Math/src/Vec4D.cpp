#include "Vec4D.h"

Vec4D::Vec4D(const Vec3D& v, const float w0)
{
	x=v.x;
	y=v.y;
	z=v.z;
	w=w0;
}

Vec4D::Vec4D(const Color32& c)
{
	x=c.r/255.0f;
	y=c.g/255.0f;
	z=c.b/255.0f;
	w=c.a/255.0f;
}

float Vec4D::Dot(const Vec4D &v) const
{
	return x*v.x + y*v.y + z*v.z + w*v.w;
}

float Vec4D::lengthSquared() const
{
	return x*x+y*y+z*z+w*w;
}

float Vec4D::length() const
{
	return sqrtf(x*x+y*y+z*z+w*w);
}

Vec4D& Vec4D::normalize()
{
	this->operator*= (1.0f/length());
	return *this;
}

Vec4D::operator float*()
{
	return (float*)this;
}

Vec3D Vec4D::xyz() const
{
	return Vec3D(x,y,z);
}

Vec3D Vec4D::xyzDivideByW() const
{
	return Vec3D(x/w,y/w,z/w);
}

Color32 Vec4D::getColor() const
{
	return Color32(unsigned char(w*255),unsigned char(x*255),unsigned char(y*255),unsigned char(z*255));
}