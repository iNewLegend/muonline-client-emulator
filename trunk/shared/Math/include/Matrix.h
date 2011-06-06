#pragma once
#include "Vec3d.h"
#include "Quaternion.h"

#undef minor

class Matrix
{
public:
	union
	{
		float m[4][4];
		float _m[16];
		struct
		{
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
	};
	Matrix(){}
	Matrix(float f11, float f12, float f13, float f14,
		float f21, float f22, float f23, float f24,
		float f31, float f32, float f33, float f34,
		float f41, float f42, float f43, float f44);
	Matrix(const Matrix& p);
	Matrix& operator= (const Matrix& p);
	static const Matrix ZERO;
	static const Matrix UNIT;
	void Zero();
	void unit();
	void translation(const Vec3D& tr);
	static const Matrix newTranslation(const Vec3D& tr);
	void scale(const Vec3D& sc);
	static const Matrix newScale(const Vec3D& sc);
	static const Matrix newScale(float fScale);
	void quaternionRotate(const Quaternion& q);
	static const Matrix newQuatRotate(const Quaternion& qr);
	Vec3D operator* (const Vec3D& v) const;
	Vec4D operator* (const Vec4D& v) const;
	Matrix operator* (const Matrix& p) const;
	float determinant() const;
	const float minor(size_t x, size_t y) const;
	const Matrix adjoint() const;
	// 逆矩阵
	void Invert();
	void transpose();
	Matrix& operator*= (const Matrix& p);
	operator float*();

	void decompose(Vec3D& vTrans,Vec3D& vScale,Quaternion& mRot)const;
	void MatrixPerspectiveFovLH(float fFovY, float Aspect, float zn, float zf);
	void MatrixOrthoLH(float w, float h, float zn, float zf);
	void MatrixLookAtLH(const Vec3D& vEye, const Vec3D& vAt, const Vec3D& vUp);
	void rotationYawPitchRoll(float fYaw, float fPitch, float fRoll);
	void rotate(const Vec3D& vRotate);

	void rotate_axis(Vec3D v,float angle)
	{
#define DEGTORAD(degree) ((degree) * (3.14159265358979323846f/180.0f))    //度转到弧度
#define RADTODEG(radian) ((radian) * (180.0f/3.14159265358979323846f)) //弧度转到度

		unit();

		float c = cos(DEGTORAD(angle));
		float s = sin(DEGTORAD(angle));
		float oneminuscos = 1 - c;

		(*this)._11 = v.x*v.x + c*(1-v.x*v.x);
		(*this)._21 = v.x*v.y*oneminuscos - s*v.z;
		(*this)._31 = v.x*v.z*oneminuscos + s*v.y;

		(*this)._12 = v.x*v.y*oneminuscos + s*v.z;
		(*this)._22 = v.y*v.y + c*(1-v.y*v.y);
		(*this)._32 = v.y*v.z*oneminuscos - s*v.x;

		(*this)._13 = v.x*v.z*oneminuscos - s*v.y;
		(*this)._23 = v.y*v.z*oneminuscos + s*v.x;
		(*this)._33 = v.z*v.z + c*(1-v.z*v.z);

	}

};