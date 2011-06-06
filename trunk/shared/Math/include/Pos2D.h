#pragma once

#include <iostream>
#include <cmath>

#include "Template2D.h"

class Pos2D: public Template2D<int>
{
public:
	Pos2D(){}
	Pos2D(int x0,int y0):Template2D(x0,y0){}
	Pos2D(const Template2D<int>& v):Template2D(v){}

	Pos2D operator - () const
	{
		return Pos2D(-x, -y);
	}

	Pos2D operator* (const Pos2D &pos) const
	{
		return Pos2D(x*pos.x, y*pos.y);
	}

	bool operator== (const Pos2D &pos)
	{
		return x == pos.x&&y == pos.y;
	}

	bool operator!= (const Pos2D &pos)
	{
		return x != pos.x||y != pos.y;
	}

	float length() const
	{
		return sqrtf(float(x*x+y*y));
	}
};