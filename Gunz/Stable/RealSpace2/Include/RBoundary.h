#ifndef _RBOUNDARY_H
#define _RBOUNDARY_H

//#pragma once

//  [10/13/2003]
//////////////////////////////////////////////////////////////////////////
// RBoundary Header
// Base Class of Bounding...blabla Classes to CD
//////////////////////////////////////////////////////////////////////////

// Include
#include "RTypes.h"
#include "vector"

// Struct

// Collision Detection���� �Ѱ���� �ϴ� ���̳�
// ���;� �ϴ� ���� �پ��� �� �ִ�
// union�� Ȱ���Ͽ� �ʿ��� ���� �����Ͽ� �Ѱ��ټ� �ִ�
enum CDInfoType
{
	CDINFO_CLOTH
};

// per vertex CD test for Cloth Animation
typedef struct
{
	rvector* v; // test vertex
	rvector* n; // vertex's normal
	rvector* pos; // return new vertex
} sClothCDInfo, psClothCDInfo;

union CDInfo
{
	sClothCDInfo clothCD;
};

class RBoundary; 

typedef std::vector<RBoundary*> COList;
typedef std::vector<RBoundary*>::iterator iterCOList;

// class RBoundary - Interface
class RBoundary
{
public:
	// Root CO
	//bool isRoot;
	// Hierarchy
	//COList mChildren;
	//RBoundary* mpParent;

public:
	// CD function 
	// if there are any collision return true otherwise return false
	virtual bool isCollide( CDInfo* data_, CDInfoType cdType_ ) {return false;};
	// create
	virtual bool create() {return true;};
	// draw if needed
	virtual void draw() {};

	virtual void setSphere( rvector centre_, float radius_ ){};
	virtual void setTopCentre( const rvector& in_ ){};
	virtual void setBottomCentre( const rvector& in_ ){};

public:
	RBoundary(void);
	~RBoundary(void);
};


#endif