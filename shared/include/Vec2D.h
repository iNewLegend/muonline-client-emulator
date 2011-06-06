#pragma once

#include <iostream>
#include <cmath>
#include "Template2D.h"

class Vec2D: public Template2D<float>
{
public:
	Vec2D(){}
	Vec2D(float x0,float y0):Template2D(x0,y0){}
	Vec2D(const Template2D<float>& v):Template2D(v){}

	Vec2D& normalize()
	{
		this->operator*= (1.0f/length());
		return *this;
	}

	Vec2D operator~ () const
	{
		Vec2D r(*this);
		r.normalize();
		return r;
	}

	friend std::istream& operator>>(std::istream& in, Vec2D& v)
	{
		in >> v.x >> v.y;
		return in;
	}
};

inline void rotate(float x0, float y0, float& x, float& y, float angle)
{
	float xa = x - x0, ya = y - y0;
	x = xa*cosf(angle) - ya*sinf(angle) + x0;
	y = xa*sinf(angle) + ya*cosf(angle) + y0;
}