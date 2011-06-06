#pragma once

template<typename T>
class Template4D
{
public:
	union
	{
		unsigned long c;
		struct
		{
			T left;
			T top;
			T right;
			T bottom;
		};
		struct
		{
			T x,y,z,w;
		};
		struct
		{
			T b,g,r,a;
		};
	};

	Template4D(){}

	Template4D(const Template4D<T>& r)
	{
		memcpy(this,&r,sizeof(T)*4);
	}

	Template4D(T x0,T y0,T z0,T w0)
	{
		set(x0,y0,z0,w0);
	}

	~Template4D(){}

	Template4D<T>& operator =(const Template4D<T> &r)
	{
		memcpy(this,&r,sizeof(T)*4);
		return (*this);
	}

	Template4D<T> operator+ (const Template4D<T> &v) const
	{
		Template4D<T> r(x+v.x,y+v.y,z+v.z,w+v.w);
		return r;
	}

	Template4D<T> operator- (const Template4D<T> &v) const
	{
		Template4D<T> r(x-v.x,y-v.y,z-v.z,w-v.w);
		return r;
	}

	Template4D<T> operator+ (T d) const
	{
		Template4D<T> r(x+d,y+d,z+d,w+d);
		return r;
	}

	friend Template4D<T> operator+ (T d, const Template4D<T>& v)
	{
		return v + d;
	}

	Template4D<T> operator* (T d) const
	{
		Template4D<T> r(x*d,y*d,z*d,w*d);
		return r;
	}

	friend Template4D<T> operator* (T d, const Template4D<T>& v)
	{
		return v * d;
	}

	Template4D<T>& operator+= (const Template4D<T> &v)
	{
		x	+= v.x;
		y	+= v.y;
		z	+= v.z;
		w	+= v.w;
		return *this;
	}

	Template4D<T>& operator-= (const Template4D<T> &v)
	{
		x	-= v.x;
		y	-= v.y;
		z	-= v.z;
		w	-= v.w;
		return *this;
	}

	Template4D<T>& operator*= (T d)
	{
		x	*= d;
		y	*= d;
		z	*= d;
		w	*= d;
		return *this;
	}

	void set(T x0,T y0,T z0,T w0)
	{
		x	= x0;
		y	= y0;
		z	= z0;
		w	= w0;
	}

	static const Template4D<T> slerp(const float r, const Template4D<T> &v1, const Template4D<T> &v2)
	{
		Template4D<T> c;
		c.r = v1.r+(unsigned char)((v2.r-v1.r)*r);
		c.g = v1.g+(unsigned char)((v2.g-v1.g)*r);
		c.b = v1.b+(unsigned char)((v2.b-v1.b)*r);
		c.a = v1.a+(unsigned char)((v2.a-v1.a)*r);
		return c;
	}

	static const Template4D<T> lerp(const float r, const Template4D<T> &v1, const Template4D<T> &v2)
	{
		Template4D<T> c;
		c.r = v1.r+(unsigned char)((v2.r-v1.r)*r);
		c.g = v1.g+(unsigned char)((v2.g-v1.g)*r);
		c.b = v1.b+(unsigned char)((v2.b-v1.b)*r);
		c.a = v1.a+(unsigned char)((v2.a-v1.a)*r);
		return c;
	}

	/*T operator [](int index) const
	{
		return (&x)[index];
	}

	T& operator [](int index) 
	{
		return (&x)[index];
	}*/

	void setByString(const char* str)
	{
		sscanf_s(str, "%d,%d,%d,%d", &x, &y, &z, &w);
	}
};

#include <stdio.h>
void Template4D<float>::setByString(const char* str)
{
	sscanf_s(str, "%f,%f,%f,%f", &x, &y, &z, &w);
}