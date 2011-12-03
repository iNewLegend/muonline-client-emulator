#pragma once

//  [10/7/2003]
//////////////////////////////////////////////////////////////////////////
// Temporary Class - to Test Cloth Animation
//
// magicbell
//////////////////////////////////////////////////////////////////////////

#include "rmesh.h"
#include "realspace2.h"
#include "rmeshutil.h"

#define RIGHT_LEG 0x01
#define	LEFT_LEG 0x02

class TBCylinder
{
public:
	TBCylinder(void);
	~TBCylinder(void);

private:
	int miNumVertex;
	int* vertexIndex;
	
	int miNumRenderVertices;
	RVertex* mVertices;
	LPDIRECT3DVERTEXBUFFER8 mVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 mIndexBuffer;		

public:
	rvector base_centre;
	float height;
	float radius;
	RealSpace2::RMeshNode* mpMesh;

private:

public:
	// � vertex���� �̿��ؼ� �Ǹ����� ������ �������� ����
	bool setBCylinder( RealSpace2::RMeshNode * pMeshNode_, DWORD flag_ ); 
	// update
	void update( rvector* point_list_ );
	// check collistion
	// ���� �浹 �ƴϸ� ���� false
	// �뷫 �浹�̸� ���� true �ϰ� �����Ǿ��� ��ġ pos_ ��ȯ
	bool isCollide( rvector& v_, rvector& n_, rvector& pos_ );

	void vertex_setup();
	void render(rmatrix* pWorld_);
};
	