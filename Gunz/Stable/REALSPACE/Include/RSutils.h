#ifndef __RSUTILS_H
#define __RSUTILS_H

#include "rstypes.h"

// help Functions ....
bool GetVerlistClipped(rvertex **in,int nin,rvertex ***out,int *nout);
void CheckAndAddVertex(rvertex *v1,rvertex *v2,rvertex *out,int *nout,rplane *plane);
void DrawFaceWithClip3(rface *face,rplane *planes);
void DrawFaceWithClip4(rface *face,rplane *planes);
void DrawWorldFaceWithClip3(rface *face,rplane *planes,rmatrix44 *m);


// ���ö��� ������ ��ġ�� ����. pPath �� rvector�� �迭, f �� Parameter 0 ~ #ControlPoints ������ �Ǽ�.
rvector GetCRSpline(rvector *pPath,float f);		// Catmull-Rom spline
rvector GetBSpline(rvector *pPath,float f);			// B-Spline
rvector GetCRSplineDiff(rvector *pPath,float f);		// Catmull-Rom spline �� ���Լ�

#endif