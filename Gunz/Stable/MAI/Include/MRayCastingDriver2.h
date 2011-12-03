#ifndef MRAYCASTINGDRIVER2_H
#define MRAYCASTINGDRIVER2_H

#include "MRayCastingDriver.h"

class MRayCastingDriver2{
public:
	CMPtrList<MObstacle>		m_Obstacles;

protected:
	void CheckOverlappedObstacleTemp(MObstacle* p, int nTemp);
	void GetArea(float* pLeftArea, float* pRightArea, MObstacle* pColObstacle, rvector& Pos, rvector& Target);
	void GetArea(float* pLeftArea, float* pRightArea, MObstacle* pColObstacle, MObstacle* pAncestor, bool bAncestorLeft, rvector& Pos, rvector& Target, int nMagicNumber);
public:
	MRayCastingDriver2(void);
	virtual ~MRayCastingDriver2(void);

	void AddObstacle(MObstacle* pObstacle);			// ������ ��ֹ��� �߰�
	void AddObstacle(CMPtrList<MObstacle>* pOL);	// ������ ��ֹ��� �߰�
	void AddObstacle(CMLinkedList<MObstacle>* pOL);	// ������ ��ֹ��� �߰�
	void DeleteObstacle(void);

	bool IsCollision(rvector* pNewPos, rvector& Pos);

	bool FindPath(rvector& Pos, rvector& Dir, float fMagnitude);
};

#endif