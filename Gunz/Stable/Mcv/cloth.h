#pragma once

// [10/7/2003]
//////////////////////////////////////////////////////////////////////////
// CLOTH HEADER
// VALET INTERPOLATION �˰����� �̿��� CLOTH ANIMATION
//
// MAGICBELL
//////////////////////////////////////////////////////////////////////////

// INCLUDE
#include "RMesh.h"
//#include "RHalfEdgeMesh.h"
// DEFINE

#include "vector"

using namespace std;

typedef struct constraint
{
	int refA, refB;
	float restLength;
}
sConstraint;


struct rvector;
struct RVertex;
class TBCylinder;

typedef vector<TBCylinder*> Cylinder;


class CCloth //: RealSpace2::RHalfEdgeMesh
{
public:
	CCloth();
	~CCloth();

private:

	// Rendering Primitives
	int miNumVertices;
	RVertex* mVertices;
	LPDIRECT3DVERTEXBUFFER8 mVertexBuffer;

	// number of partices
	int miNumParticles;
	// Current Position
	rvector* mX;
	// Last Position
	rvector* mOldX;
	// Force ( include Gravity )
	rvector* mForce;
	// or Use only one Force ( Gravity ) - apply same force to all particles
	rvector mgForce;
	// Constraints
	int miNumConst;
	sConstraint* mConst;
	// Time Steps
	float	mfTimeStep;
	// Number of Iteration - more iteration is more accurate and slow
	int miNumIter;

	// Hold Particles
	bool* mHolds;
	
	// Collistion Object List
	// ���Ծ��� �����Ͽ� ����.. ���� ��� ��ƼŬ�� ��� �ø��� ������Ʈ�� �浹�˻��Ѵ�
	// �ӵ� ����� ���� part���� ��� ������Ʈ�� ������ �� �ֵ��� �Ҽ� �ִ�
	//sRCObj* mDestObject;

public:
	// point to Mesh Node - to Render!!
	RealSpace2::RMeshNode *mpMesh;	
	// TODO : �׽�Ʈ �Ǹ�����...
	Cylinder mCylinders;

private:
	// �� ��� - �߷� �̿ܿ� ������ �ʹǸ� ������ ���� ����.. ���� ����Ǵ� ��Ȳ ����
	void accumulateForces();
	// Interpolation bet. current frame and last frame
	void varlet();
	// satisfy constraints - �浹 �� ���� ������ ������Ų��
	void satisfyConstraints();
	//xxx calc vertex normal
	void calcVertexNormal();

public:
	// initialize - set Gravity.. 
	void init();
	// XXX Test �� �ڵ�.. �޽� ��带 �޾� cloth�� �����Ѵ�
	// automate constraints -> �ݵ�� �������� �ٲ�� �Ѵ�
	void setColth( RealSpace2::RMeshNode* mesh_ );
	// update - call every frame
	void Update();
	// update position - vertex���� ��ü���� ������
	void updatePosition( RealSpace2::RMeshNode* mesh_, rmatrix* pMatrix_ );
	// render
	void render();

	//XXX to Test Force relative things 
	void setForces( rvector& inc_ );
};