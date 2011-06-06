#pragma once
#include <iostream>
#include <cmath>

class Vec3D
{
public:
	union
	{
		float f[3];
		struct
		{
			float x,y,z;
		};
	};

	Vec3D() : x(0.0f), y(0.0f), z(0.0f) {}

	Vec3D(float x0, float y0, float z0) : x(x0), y(y0), z(z0) {}

	Vec3D(const Vec3D& v) : x(v.x), y(v.y), z(v.z) {}

	void reset()
	{
		x = y = z = 0.0f;
	}

	void set(float fX,float fY,float fZ)
	{
		x = fX;
		y = fY;
		z = fZ;
	}

	Vec3D& operator= (const Vec3D &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	bool operator== (const Vec3D &v) const
	{
		if (x == v.x&&
			y == v.y&&
			z == v.z)
		{
			return true;
		}
		return false;
	}

	bool operator!= (const Vec3D &v) const
	{
		if (x != v.x||
			y != v.y||
			z != v.z)
		{
			return true;
		}
		return false;
	}

	Vec3D operator+ (const Vec3D &v) const
	{
		Vec3D r(x+v.x,y+v.y,z+v.z);
		return r;
	}

	Vec3D operator- (const Vec3D &v) const
	{
		Vec3D r(x-v.x,y-v.y,z-v.z);
		return r;
	}

	Vec3D operator - () const
	{
		return Vec3D(-x, -y, -z);
	}

	Vec3D operator* (const Vec3D &v) const
	{
		Vec3D r(x*v.x,y*v.y,z*v.z);
		return r;
	}

	Vec3D operator* (float d) const
	{
		Vec3D r(x*d,y*d,z*d);
		return r;
	}

	Vec3D operator/ (const Vec3D &v) const
	{
		Vec3D r(x/v.x,y/v.y,z/v.z);
		return r;
	}

	Vec3D operator/ (float d) const
	{
		Vec3D r(x/d,y/d,z/d);
		return r;
	}

	friend Vec3D operator* (float d, const Vec3D& v)
	{
		return v * d;
	}

	Vec3D operator% (const Vec3D &v) const
	{
		return Vec3D(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
	}

	Vec3D cross(const Vec3D &v) const
	{
		return Vec3D(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

	float dot(const Vec3D &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vec3D& operator+= (const Vec3D &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vec3D& operator-= (const Vec3D &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vec3D& operator*= (const Vec3D &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vec3D& operator*= (float d)
	{
		x *= d;
		y *= d;
		z *= d;
		return *this;
	}

	Vec3D& operator/= (const Vec3D &v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vec3D& operator/= (float d)
	{
		x /= d;
		y /= d;
		z /= d;
		return *this;
	}

	float lengthSquared() const
	{
		return x*x+y*y+z*z;
	}

	float length() const
	{
		return sqrtf(x*x+y*y+z*z);
	}

	Vec3D& normalize()
	{
		this->operator*= (1.0f/length());
		return *this;
	}

	Vec3D operator~ () const
	{
		Vec3D r(*this);
		r.normalize();
		return r;
	}

	friend std::istream& operator>>(std::istream& in, Vec3D& v)
	{
		in >> v.x >> v.y >> v.z;
		return in;
	}

	operator float*()
	{
		return (float*)this;
	}

	void setByString(const char* str)
	{
		sscanf_s(str, "%f,%f,%f", &x, &y, &z);
	}
};