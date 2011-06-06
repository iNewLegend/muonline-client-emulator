#pragma once
#include "Vec4D.h"

#ifndef PI
#define PI	3.141592654f
#endif

#ifndef PI_X2
#define PI_X2	6.283185308f
#endif

class Quaternion: public Vec4D
{
public:
	Quaternion():Vec4D(){}
	Quaternion(const Template4D<float>& v):Vec4D(v){}
	Quaternion(float x,float y,float z, float w):Vec4D(x,y,z,w){}
	//Quaternion(const Vec3D& v, const float w0)			:Vec4D(v, w0){}
	//Quaternion(const Color32& Color)					:Vec4D(Color){}
	Quaternion Multiply(const Quaternion& q) const;
	void Rotate(const Vec3D& axis, const float angle);
	void rotate(const Vec3D& vRotate);
	static const Quaternion slerp(const float r, const Quaternion &v1, const Quaternion &v2);
	static const Quaternion lerp(const float r, const Quaternion &v1, const Quaternion &v2);
};