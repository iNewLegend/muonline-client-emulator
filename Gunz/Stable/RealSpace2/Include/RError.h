#ifndef _RERROR_H
#define _RERROR_H

/// error def

#include "RTypes.h"

enum RERROR
{
	ROK = 0,

	// d3d����
	RERROR_CANNOT_CREATE_D3D	= 1000,
	RERROR_INVALID_DEVICE		= 1001,
	
	// ��Ÿ
	RERROR_CLOTH_BONE_MISSING	= 2000,	// RCharCloth init�Ҷ� ���� ���� ��� (�Ǽ��� �޽ÿ� vertexcolor�� ���� clothmesh�� ���ֵǴ� ��찡 ����)
	RERROR_CLOTH_PHYSIQUE_MISSING = 2001, // RCharCloth init�Ҷ� ������ ���� �޽ø� ���� ���
	
};





//-------------------------------------------------------
void RSetError(int nErrCode);
int RGetLastError(void);


#endif