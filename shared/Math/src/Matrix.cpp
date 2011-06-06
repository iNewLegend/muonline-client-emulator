#include "Matrix.h"

const Matrix Matrix::ZERO(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 );

const Matrix Matrix::UNIT(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 );


Matrix::Matrix(float f11, float f12, float f13, float f14,
			   float f21, float f22, float f23, float f24,
			   float f31, float f32, float f33, float f34,
			   float f41, float f42, float f43, float f44)
{
	_11 = f11; _12 = f12; _13 = f13; _14 = f14;
	_21 = f21; _22 = f22; _23 = f23; _24 = f24;
	_31 = f31; _32 = f32; _33 = f33; _34 = f34;
	_41 = f41; _42 = f42; _43 = f43; _44 = f44;
}

Matrix::Matrix(const Matrix& p)
{
	for (size_t j=0; j<4; j++) {
		for (size_t i=0; i<4; i++) {
			m[j][i] = p.m[j][i];
		}
	}
}

Matrix& Matrix::operator= (const Matrix& p)
{
	for (size_t j=0; j<4; j++) {
		for (size_t i=0; i<4; i++) {
			m[j][i] = p.m[j][i];
		}
	}
	return *this;
}


void Matrix::Zero()
{
	for (size_t j=0; j<4; j++) {
		for (size_t i=0; i<4; i++) {
			m[j][i] = 0;
		}
	}
}

void Matrix::unit()
{
	Zero();
	_11 = _22 = _33 = _44 = 1.0f;
}

void Matrix::translation(const Vec3D& tr)
{
	/*
	100#
	010#
	001#
	0001
	*/
	unit();
 	_14=tr.x;
 	_24=tr.y;
 	_34=tr.z;
}

const Matrix Matrix::newTranslation(const Vec3D& tr)
{
	Matrix t;
	t.translation(tr);
	return t;
}

void Matrix::scale(const Vec3D& sc)
{
	/*
	#000
	0#00
	00#0
	0001
	*/
	Zero();
	_11=sc.x;
	_22=sc.y;
	_33=sc.z;
	_44=1.0f;
}

const Matrix Matrix::newScale(const Vec3D& sc)
{
	Matrix t;
	t.scale(sc);
	return t;
}

const Matrix Matrix::newScale(float fScale)
{
	Matrix t;
	t.scale(Vec3D(fScale,fScale,fScale));
	return t;
}

void Matrix::quaternionRotate(const Quaternion& q)
{
	float fTx  = q.x*2.0f;
	float fTy  = q.y*2.0f;
	float fTz  = q.z*2.0f;
	float fTwx = fTx*q.w;
	float fTwy = fTy*q.w;
	float fTwz = fTz*q.w;
	float fTxx = fTx*q.x;
	float fTxy = fTy*q.x;
	float fTxz = fTz*q.x;
	float fTyy = fTy*q.y;
	float fTyz = fTz*q.y;
	float fTzz = fTz*q.z;

	_11 = 1.0f-(fTyy+fTzz);
	_12 = fTxy+fTwz;
	_13 = fTxz-fTwy;
	_21 = fTxy-fTwz;
	_22 = 1.0f-(fTxx+fTzz);
	_23 = fTyz+fTwx;
	_31 = fTxz+fTwy;
	_32 = fTyz-fTwx;
	_33 = 1.0f-(fTxx+fTyy);
	_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
	_44 = 1.0f;
}

const Matrix Matrix::newQuatRotate(const Quaternion& qr)
{
	Matrix t;
	t.quaternionRotate(qr);
	return t;
}

//  	float* operator [] (unsigned char uRow)
//  	{
//  		//assert(uRow < 4);
//  		return m[uRow];
//  	}
// 
// 	const float *const operator [] (unsigned char uRow) const
//  	{
//  		//assert(uRow < 4);
//  		return m[uRow];
//  	}

Vec3D Matrix::operator* (const Vec3D& v) const
{
	Vec3D o;
	o.x = _11*v.x + _12*v.y + _13*v.z + _14;
	o.y = _21*v.x + _22*v.y + _23*v.z + _24;
	o.z = _31*v.x + _32*v.y + _33*v.z + _34;
	return o;
}

Vec4D Matrix::operator* (const Vec4D& v) const
{
	Vec4D o;
	o.x = _11*v.x + _12*v.y + _13*v.z + _14*v.w;
	o.y = _21*v.x + _22*v.y + _23*v.z + _24*v.w;
	o.z = _31*v.x + _32*v.y + _33*v.z + _34*v.w;
	o.w = _41*v.x + _42*v.y + _43*v.z + _44*v.w;
	return o;
}

Matrix Matrix::operator* (const Matrix& p) const
{
	Matrix o;
	o._11 = _11*p._11 + _12*p._21 + _13*p._31 + _14*p._41;
	o._12 = _11*p._12 + _12*p._22 + _13*p._32 + _14*p._42;
	o._13 = _11*p._13 + _12*p._23 + _13*p._33 + _14*p._43;
	o._14 = _11*p._14 + _12*p._24 + _13*p._34 + _14*p._44;

	o._21 = _21*p._11 + _22*p._21 + _23*p._31 + _24*p._41;
	o._22 = _21*p._12 + _22*p._22 + _23*p._32 + _24*p._42;
	o._23 = _21*p._13 + _22*p._23 + _23*p._33 + _24*p._43;
	o._24 = _21*p._14 + _22*p._24 + _23*p._34 + _24*p._44;

	o._31 = _31*p._11 + _32*p._21 + _33*p._31 + _34*p._41;
	o._32 = _31*p._12 + _32*p._22 + _33*p._32 + _34*p._42;
	o._33 = _31*p._13 + _32*p._23 + _33*p._33 + _34*p._43;
	o._34 = _31*p._14 + _32*p._24 + _33*p._34 + _34*p._44;

	o._41 = _41*p._11 + _42*p._21 + _43*p._31 + _44*p._41;
	o._42 = _41*p._12 + _42*p._22 + _43*p._32 + _44*p._42;
	o._43 = _41*p._13 + _42*p._23 + _43*p._33 + _44*p._43;
	o._44 = _41*p._14 + _42*p._24 + _43*p._34 + _44*p._44;
	return o;
}

float Matrix::determinant() const
{
#define SUB(a,b) (m[2][a]*m[3][b] - m[3][a]*m[2][b])
	return
		_11 * (_22*SUB(2,3) - _23*SUB(1,3) + _24*SUB(1,2))
		-_12 * (_21*SUB(2,3) - _23*SUB(0,3) + _24*SUB(0,2))
		+_13 * (_21*SUB(1,3) - _22*SUB(0,3) + _24*SUB(0,1))
		-_14 * (_21*SUB(1,2) - _22*SUB(0,2) + _23*SUB(0,1));
#undef SUB
}

const float Matrix::minor(size_t x, size_t y) const
{
	float s[3][3];
	for (size_t j=0, v=0; j<4; j++) {
		if (j==y) continue;
		for (size_t i=0, u=0; i<4; i++) {
			if (i!=x) {
				s[v][u++] = m[j][i];
			}
		}
		v++;
	}
#define SUB(a,b) (s[1][a]*s[2][b] - s[2][a]*s[1][b])
	return s[0][0] * SUB(1,2) - s[0][1] * SUB(0,2) + s[0][2] * SUB(0,1);
#undef SUB
}

const Matrix Matrix::adjoint() const
{
	Matrix a;
	for (size_t j=0; j<4; j++) {
		for (size_t i=0; i<4; i++) {
			a.m[i][j] = (((i+j)&1)?-1.0f:1.0f) * minor(i,j);
		}
	}
	return a;
}

// Äæ¾ØÕó
void Matrix::Invert()
{
	Matrix adj = this->adjoint();
	float invdet = 1.0f / this->determinant();
	for (size_t j=0; j<4; j++) {
		for (size_t i=0; i<4; i++) {
			m[j][i] = adj.m[j][i] * invdet;
		}
	}
}

void Matrix::transpose()
{
	for (size_t j=1; j<4; j++) {
		for (size_t i=0; i<j; i++) {
			float f = m[j][i];
			m[j][i] = m[i][j];
			m[i][j] = f;
		}
	}
}

Matrix& Matrix::operator*= (const Matrix& p)
{
	return *this = this->operator*(p);
}

Matrix::operator float*()
{
	return (float*)this;
}

void Matrix::decompose(Vec3D& vTrans,Vec3D& vScale,Quaternion& mRot)const
{
	// Retrieving the translation is as easy as getting the x,y,z values from the thrid row:
	vTrans.x = _14;
	vTrans.y = _24;
	vTrans.z = _34;
	// We create a temporary Vec3D array to store the 3x3 matrix that contains the scaling and rotation. We will then take this information and seperate it into its scale and rotation components.

	Vec3D vCols[3] = {
		Vec3D(_11,_21,_31),
		Vec3D(_12,_22,_32),
		Vec3D(_13,_23,_33)
	};


	//Retrieving the scale is done by gathering the legnth of the vectors for each column of the 3x3 matrix.
	vScale.x = vCols[0].length();
	vScale.y = vCols[1].length();
	vScale.z = vCols[2].length();

	//The 3x3 rotation matrix can be obtained by dividing each column of the 3x3 rotation/scale matrix by the retrieved scalar component:
	if(vScale.x != 0)
	{
		vCols[0].x /= vScale.x;
		vCols[0].y /= vScale.x;
		vCols[0].z /= vScale.x;
	}
	if(vScale.y != 0)
	{
		vCols[1].x /= vScale.y;
		vCols[1].y /= vScale.y;
		vCols[1].z /= vScale.y;
	}
	if(vScale.z != 0)
	{
		vCols[2].x /= vScale.z;
		vCols[2].y /= vScale.z;
		vCols[2].z /= vScale.z;
	}

	//unroll the loop to increase the speed
	/*for(int x=0;x<3;x++)
	{
	mRot.m_val[0][x] = vCols[x].x;
	mRot.m_val[1][x] = vCols[x].y;
	mRot.m_val[2][x] = vCols[x].z;
	mRot.m_val[x][3] = 0;
	mRot.m_val[3][x] = 0;
	}*/
/*	mRot.m_val[0][0] = vCols[0].x;
	mRot.m_val[1][0] = vCols[0].y;
	mRot.m_val[2][0] = vCols[0].z;
	mRot.m_val[0][3] = 0;
	mRot._14 = 0;
	mRot.m_val[0][1] = vCols[1].x;
	mRot.m_val[1][1] = vCols[1].y;
	mRot.m_val[2][1] = vCols[1].z;
	mRot.m_val[1][3] = 0;
	mRot._24 = 0;
	mRot.m_val[0][2] = vCols[2].x;
	mRot.m_val[1][2] = vCols[2].y;
	mRot.m_val[2][2] = vCols[2].z;
	mRot.m_val[2][3] = 0;
	mRot._34 = 0;  
	mRot.m_val[3][3] = 1;*/
}

void Matrix::MatrixPerspectiveFovLH(float fFovY, float Aspect, float fNear, float fFar)
{
	float fYScale = 1.0f/tan(fFovY*0.5f);
	float fXScale = fYScale/Aspect;

	float n = 1.0f / (fFar - fNear);

	// [ xScale	0		0				0  ]
	// [ 0		yScale	0				0  ]
	// [ 0		0		zf/(zf-zn)		-zn*zf/(zf-zn)  ]
	// [ 0		0		1				0  ]
	this->Zero();
	_11 = fXScale;
	_22 = fYScale;
	_33 = fFar*n;
	_34 = -fNear*fFar*n;
	_43 = 1.0f;
}

void Matrix::MatrixLookAtLH(const Vec3D& vEye, const Vec3D& vAt, const Vec3D& vUp)
{
	Vec3D vZAxis = (vAt - vEye);
	vZAxis.normalize();
	Vec3D vXAxis = vUp.cross(vZAxis);
	vXAxis.normalize();
	Vec3D vYAxis = vZAxis.cross(vXAxis);

	//xaxis.x           xaxis.y           xaxis.z			-dot(xaxis, eye)
	//yaxis.x           yaxis.y           yaxis.z			-dot(yaxis, eye)
	//zaxis.x           zaxis.y           zaxis.z			-dot(zaxis, eye)
	//0					0					0				1

	_11 = vXAxis.x;
	_12 = vXAxis.y;
	_13 = vXAxis.z;
	_14 = -vXAxis.dot(vEye);
	_21 = vYAxis.x;
	_22 = vYAxis.y;
	_23 = vYAxis.z;
	_24 = -vYAxis.dot(vEye);
	_31 = vZAxis.x;
	_32 = vZAxis.y;
	_33 = vZAxis.z;
	_34 = -vZAxis.dot(vEye);
	_41 = 0;
	_42 = 0;
	_43 = 0;
	_44 = 1;
}

void Matrix::MatrixOrthoLH(float w, float h, float zn, float zf)
{
	//2/w  0    0           0
	//0    2/h  0           0
	//0    0    1/(zf-zn)	zn/(zn-zf)
	//0    0    0			1
	this->Zero();
	_11 = 2/w;
	_22 = 2/h;
	_33 = 1/(zf-zn);
	_34 = zn/(zn-zf);
	_44 = 1;
}

void Matrix::rotationYawPitchRoll(float fYaw, float fPitch, float fRoll)
{
	rotate(Vec3D(fPitch,fYaw,fRoll));
}

void Matrix::rotate(const Vec3D& vRotate)
{
	Quaternion rot;
	rot.rotate(vRotate);
	this->quaternionRotate(rot);
}