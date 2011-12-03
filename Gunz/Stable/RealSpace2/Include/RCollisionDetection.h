//
//	Collision Detection
//	
//  bird - [10/08/2004]
//////////////////////////////////////////////////////////////////////////
#ifndef _RCOLLISIONDETECTION_H
#define _RCOLLISIONDETECTION_H

#include "RNameSpace.h"
#include "RTypes.h"
_NAMESPACE_REALSPACE2_BEGIN


#define AABB		rboundingbox

struct rcapsule {
	rcapsule() {}
	rcapsule(const rvector& va, const rvector& vb, float radius) {
		rcapsule::pivot = va;
		rcapsule::lerp = vb - va;
		rcapsule::length = Magnitude(lerp);
		rcapsule::radius = radius;
	}
	rvector pivot;
	rvector lerp;
	float length;
	float radius;
};

// RSphere�� ������ �����ؼ� �����ϰ� ���������� �� �� �ִ°ɷ� ���� ����. =_=
struct rsphere {
	rsphere() {}
	rsphere(const rvector& pos, float r) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		radius = r;
	}

	float x, y, z;
	float radius;

   const rvector& Pos() const { return *((rvector*)&x); }
};


float SweepTest(const rsphere& sphere, const rvector& vec, const rplane &pln, rplane* out = 0);
float SweepTest(const rsphere& sphere, const rvector& vec, const rsphere& body, rplane* out = 0);
float SweepTest(const rsphere& sphere, const rvector& vec, const rcapsule &capsule, rplane* out = 0);

_NAMESPACE_REALSPACE2_END

#endif