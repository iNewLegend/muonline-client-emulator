#pragma once

//////////////////////////////////////////////////////////////////////////
// working specification( progammer note )
// shadow�� ���� �簢 ������ �ϳ��� �����Ѵ�(global) + texture
// character�� foot position�� ���ü� �ִ� ( 2��)
// map�� ���� �� �������� ���� floor ��ǥ�� map�� �������� normal�� ���ü� �ִ�
// floor�� ��ǥ�� charactoer�� foot position�� �Ÿ��� üũ�ؼ� �׷��� �������� �����Ѵ�
// shadow�������� normal�� map�� �������� normal�� ��ġ�ϵ��� transformation �����ش�
// �׷��ش�
//////////////////////////////////////////////////////////////////////////

#define	ENABLE_SHOWDOW
#include "zapplication.h"
#include "RTypes.h"
#include "RMeshUtil.h"
#include "RBaseTexture.h"

class ZCharacter;

_USING_NAMESPACE_REALSPACE2

class ZShadow
{
public:
	ZShadow(void);
	virtual ~ZShadow(void);

private:	

	bool bLFShadow;
	bool bRFShadow;

	rmatrix mWorldMatLF;	//  ���ʹ�
	rmatrix mWorldMatRF;	// �����ʹ�

	static rvector mNormal;

	float mfDistance;	// ���ʹ߰� ������ ���� �ٴ����κ����� �Ÿ��� ������ ���

public:
	bool setMatrix( ZCharacterObject& char_, float size_ = 100.0f );	// �� �����Ӹ��� �ҷ����� ��
	bool setMatrix( RVisualMesh& vmesh, float size_ = 100.f, RBspObject* p_map  = 0 );
	void draw(bool bForced=false);

private:
	rmatrix setSize( float size_ ); // size_ should be bigger than 0
	rmatrix setDirection( rvector& dir_ );
};