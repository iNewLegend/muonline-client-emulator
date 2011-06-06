#pragma once
#include "Quaternion.h"

// interpolation functions
template<class T>
inline T interpolate(const float r, const T &v1, const T &v2)
{
	return static_cast<T>(v1*(1.0f - r) + v2*r);
}

template<class T>
inline T interpolateHermite(const float r, const T &v1, const T &v2, const T &in, const T &out)
{
	// basis functions
	float h1 = 2.0f*r*r*r - 3.0f*r*r + 1.0f;
	float h2 = -2.0f*r*r*r + 3.0f*r*r;
	float h3 = r*r*r - 2.0f*r*r + r;
	float h4 = r*r*r - r*r;

	// interpolation
	return static_cast<T>(v1*h1 + v2*h2 + in*h3 + out*h4);
}

// "linear" interpolation for quaternions should be slerp by default
template<>
inline Quaternion interpolate<Quaternion>(const float r, const Quaternion &v1, const Quaternion &v2)
{
	return Quaternion::slerp(r, v1, v2);
}

// "linear" interpolation for quaternions should be slerp by default
template<>
inline Color32 interpolate<Color32>(const float r, const Color32 &v1, const Color32 &v2)
{
	return Color32::slerp(r, v1, v2);
}

template<class T>
inline T bilinearInterpolation(const T& a, const T& b, const T& c, const T& d, float u, float v)
{
	// a	b
	//	*---*
	//	| \ |
	//	*---*
	// c	d
	if (u>v)
	{
		return b+(a-b)*(1.0f-u)+(d-b)*v;
	}
	else
	{
		return c+(d-c)*u+(a-c)*(1.0f-v);
	}
}

template<class T>
T lifeRamp(float life, float mid, const T &a, const T &b, const T &c)
{
	if (life<=mid) return interpolate<T>(life / mid,a,b);
	else return interpolate<T>((life-mid) / (1.0f-mid),b,c);
}