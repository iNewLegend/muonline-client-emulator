#pragma once

template<typename T>
class Template2D
{
public:
	T x,y;

	Template2D(){}
	Template2D(const Template2D<T>& r)
	{
		memcpy(this,&r,sizeof(T)*2);
	}
	Template2D(T x0, T y0) : x(x0), y(y0) {}

	~Template2D(){}

	Template2D<T>& operator =(const Template2D<T> &r)
	{
		memcpy(this,&r,sizeof(T)*2);
		return (*this);
	}

	Template2D<T> operator+ (const Template2D<T> &v) const
	{
		Template2D<T> r(x+v.x,y+v.y);
		return r;
	}

	Template2D<T> operator- (const Template2D<T> &v) const
	{
		Template2D<T> r(x-v.x,y-v.y);
		return r;
	}

	Template2D<T> operator+ (T d) const
	{
		Template2D<T> r(x+d,y+d);
		return r;
	}

	friend Template2D<T> operator+ (T d, const Template2D<T>& v)
	{
		return v + d;
	}

	Template2D<T> operator* (T d) const
	{
		Template2D<T> r(x*d,y*d);
		return r;
	}

	friend Template2D<T> operator* (T d, const Template2D<T>& v)
	{
		return v * d;
	}

	Template2D<T> operator/ (T d) const
	{
		Template2D<T> r(x/d,y/d);
		return r;
	}

	Template2D<T>& operator+= (const Template2D<T> &v)
	{
		x	+= v.x;
		y	+= v.y;
		return *this;
	}

	Template2D<T>& operator-= (const Template2D<T> &v)
	{
		x	-= v.x;
		y	-= v.y;
		return *this;
	}

	Template2D<T>& operator*= (T d)
	{
		x	*= d;
		y	*= d;
		return *this;
	}

	T lengthSquared() const
	{
		return x*x+y*y;
	}

	T length() const
	{
		return sqrtf(x*x+y*y);
	}

	void set(T x0,T y0)
	{
		x	= x0;
		y	= y0;
	}

	void setByString(const char* str)
	{
		sscanf_s(str, "%d,%d", &x, &y);
	}
};

#include <stdio.h>

void Template2D<float>::setByString(const char* str)
{
	sscanf_s(str, "%f,%f", &x, &y);
}