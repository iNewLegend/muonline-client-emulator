#ifndef MUTIL_H
#define MUTIL_H

#include "rutils.h"

// �Ϻ� ������ Pos, Dir(Normalize�� �ƴ� ���� ���� ��ŭ)���� ����, �̴� ������ ���̱� �����̴�.

// 3D���� ���Ϳ� ���� �����ϴ��� �׽�Ʈ. Dir�� ������ �ǹ���
bool MIsIntersectLineAndCircle(float* t, rvector& Pos, rvector& Dir, rvector& CircleCenter, float fCircleRadius);

// Point���� Line�� ������ ������ �� ����Ʈ ���ϱ�
// pT : LineStart->LineEnd ������ �� (0~1.0)
void MGetPointFromPointToLine(rvector* pContactPoint, float* pT, rvector& Point, rvector& LineStart, rvector& LineEnd);

// 2D���� � ���� ���� ������ ���� ���ϱ�
void MGetContactPoint(rvector ContactPoint[2], rvector& Point, rvector& CircleCenter, float fCircleRadius);

// 2D���� ���ΰ� ������ ��ġ���� ����
bool MIsIntersectLineAndLine(float *pT1, float *pT2, rvector& Pos, rvector& Dir, rvector& s1, rvector& s2);

// 2D���� ���ΰ� ������ ��ġ���� ����
bool MIsIntersectLineAndHalfLine(float *pT1, float *pT2, rvector& Pos, rvector& Dir, rvector& s1, rvector& s2);

// 2D���� ���ΰ� �簢���� ��ġ���� ����
int MIsIntersectLineAndRectangle(float *pT,	// �Ű� ���� ���� ( 0.0 ~ 1.0 )
										 rvector& Pos, rvector& Dir,	// Line
										 float fLeft, float fTop, float fRight, float fBottom);	// Rect

/*
            (Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy)
        r = -----------------------------  (eqn 1)
            (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
        s = -----------------------------  (eqn 2)
            (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
*/

bool MFGetIntersectOfSegmentAndSegment(float *pT1, float *pT2, float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy);
bool MFIsPointInSegment(float sx1, float sy1, float sx2, float sy2, float x, float y);
bool MFIsVisiblePoint(float x1, float y1, float x2, float y2, float x3, float y3, float px, float py);
bool MFIsEdgePoint(float x1, float y1, float x2, float y2, float x3, float y3, float px, float py);

#endif