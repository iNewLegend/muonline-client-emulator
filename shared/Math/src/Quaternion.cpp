#include "Quaternion.h"

Quaternion Quaternion::Multiply(const Quaternion& q) const
{
	// To multiply a quaternion you must first do the dot and cross product
	// of the 2 quaternions then add/subract them to a result.
	Quaternion r;
	r.x = w * q.x + x * q.w + y * q.z - z * q.y;
	r.y = w * q.y - x * q.z + y * q.w + z * q.x;
	r.z = w * q.z + x * q.y - y * q.x + z * q.w;
	r.w = w * q.w - x * q.x - y * q.y - z * q.z;
	return r;
}

void Quaternion::Rotate(const Vec3D& axis, const float angle)
{
	float sin = sinf(0.5f*angle);
	w = cosf(0.5f*angle);
	x = sin*axis.x;
	y = sin*axis.y;
	z = sin*axis.z;
	normalize();
}

void Quaternion::rotate(const Vec3D& vRotate)
{
	Quaternion x, y, z;
	x.Rotate(Vec3D(1.0f,0.0f,0.0f),vRotate.x);
	y.Rotate(Vec3D(0.0f,1.0f,0.0f),vRotate.y);
	z.Rotate(Vec3D(0.0f,0.0f,1.0f),vRotate.z);
	*this=x.Multiply(y).Multiply(z);
	/*float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = vRotate.z*0.5f;
	sy = sinf(angle);
	cy = cosf(angle);
	angle = vRotate.y*0.5f;
	sp = sinf(angle);
	cp = cosf(angle);
	angle = vRotate.x*0.5f;
	sr = sinf(angle);
	cr = cosf(angle);

	this->x = sr * cp * cy - cr * sp * sy;
	this->y = cr * sp * cy + sr * cp * sy;
	this->z = cr * cp * sy - sr * sp * cy;
	this->w = cr * cp * cy + sr * sp * sy;*/
}

const Quaternion Quaternion::slerp(const float t, const Quaternion &q1, const Quaternion &q2)
{
	// SLERP
	/*float dot = v1.Dot(v2);

	if (fabs(dot) > 0.9995f) {
		// fall back to LERP
		return Quaternion::lerp(r, v1, v2);
	}

	float a = acosf(dot) * r;
	Quaternion q = (v2 - v1 * dot);
	q.normalize();

	return v1 * cosf(a) + q * sinf(a);*/


	float o, co, so, scale0, scale1;
	float qi[4];


	// Do a linear interpolation between two quaternions (0 <= t <= 1).
	co = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;  // dot product
	if (co < 0)
	{
		co = -co;
		qi[0] = -q2.x;
		qi[1] = -q2.y;
		qi[2] = -q2.z;
		qi[3] = -q2.w;
	}
	else
	{
		qi[0] = q2.x;
		qi[1] = q2.y;
		qi[2] = q2.z;
		qi[3] = q2.w;
	}
	// If the quaternions are really close, do a simple linear interpolation.
	if ((1 - co) <= 0.0001f)
	{
		scale0 = 1 - t;
		scale1 = t;
	}
	else
	{
		// Otherwise SLERP.
		o      = (float) acos(co);
		so     = sinf(o);
		scale0 = sinf((1 - t) * o) / so;
		scale1 = sinf(t * o) / so;
	}
	// Calculate interpolated quaternion:
	Quaternion q;
	q.x = scale0 * q1.x + scale1 * qi[0];
	q.y = scale0 * q1.y + scale1 * qi[1];
	q.z = scale0 * q1.z + scale1 * qi[2];
	q.w = scale0 * q1.w + scale1 * qi[3];

	return q;
	/*float tol[4];
	double omega, cosom, sinom, scale0, scale1;

	cosom=v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	if(cosom<0.0)
	{
		cosom = -cosom;
		tol[0] = -v2.x;
		tol[1] = -v2.y;
		tol[2] = -v2.z;
		tol[3] = -v2.w;
	}
	else
	{
		tol[0] = -v2.x;
		tol[1] = -v2.y;
		tol[2] = -v2.z;
		tol[3] = -v2.w;
	}

	if((1.0-cosom)>0.0005f)
	{
		//标准情形(slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0-r)*omega)/sinom;
		scale1 = sin(r*omega)/sinom;
	}
	else
	{
		//from与to四元数非常接近
		//因此可以用线性插值
		scale0 = 1.0 - r;
		scale1 = r;
	}
	//计算最终值
	Quaternion q;
	q.x = scale0 * v1.x + scale1 * tol[0];
	q.y = scale0 * v1.y + scale1 * tol[1];
	q.z = scale0 * v1.z + scale1 * tol[2];
	q.w = scale0 * v1.w + scale1 * tol[3];
	return q;*/
}

const Quaternion Quaternion::lerp(const float r, const Quaternion &v1, const Quaternion &v2)
{
	return v1*(1.0f-r) + v2*r;
}