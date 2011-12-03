#ifndef _RCYLINDER_H
#define _RCYLINDER_H

//#pragma once

//  [10/13/2003]
//////////////////////////////////////////////////////////////////////////
// RCylinder 
// Bounding Cylinder
//
// Magicbell
//////////////////////////////////////////////////////////////////////////

#include "RBoundary.h"
#include "RTypes.h"

//Declare

//class RCylinder 
class RCylinder : public RBoundary
{
public:
	rvector mTopCentre;
	rvector mBottomCentre;
	float mHeight;
	float mRadius;

	//////////////////////////////////////////////////////////////////////////
	//
	//	<<<	Rendering Primitives >>>
	//	
	LPD3DXMESH mCylinder;
	rmatrix mWorld;
	//
	//////////////////////////////////////////////////////////////////////////

public:
	bool isCollide(	CDInfo* data_, CDInfoType cdType_ );
	void draw();
	inline void setTopCentre( const rvector& in_ );
	inline void setBottomCentre( const rvector& in_ );
	inline void setHeight( const float& in_ );
	inline void setRadius( const float& in_ );
	inline void setTransform( rmatrix& world_ );

public:
	RCylinder(void);
	virtual ~RCylinder(void);
};

void RCylinder::setTopCentre( const rvector& in_ ) { mTopCentre = in_; mHeight = D3DXVec3Length(&(mTopCentre - mBottomCentre)); }
void RCylinder::setBottomCentre( const rvector& in_ ) { mBottomCentre = in_; mHeight = D3DXVec3Length(&(mTopCentre - mBottomCentre)); }
void RCylinder::setHeight( const float& in_ ) { mHeight = in_; }
void RCylinder::setRadius( const float& in_ ) { mRadius = in_; }
void RCylinder::setTransform( rmatrix& world_ ) { mWorld = world_; }


//////////////////////////////////////////////////////////////////////////
// Helper Function
// Shortest Distance Between a Line and a Point
// return : true - shortest point is exist in a line , false - does not
// intersection : ���� line �� ���� ����� �������� ���� - ���� �������
// direction : line�� ���� - ���� �������
// distacne : ���ΰ� ������ �Ÿ�
// TODO (OPTI)  : ������� input�� ���ְ� ������ ���δ� 
//////////////////////////////////////////////////////////////////////////
bool getDistanceBetLineSegmentAndPoint( const rvector& lineStart_, 
									   const rvector& lineEnd_, 
									   rvector* point_, 
									   rvector* intersection_, 
									   rvector* direction_, 
									   float& distance_ );

#endif