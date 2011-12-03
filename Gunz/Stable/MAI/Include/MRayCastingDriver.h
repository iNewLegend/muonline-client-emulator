#ifndef MRAYCASTINGDRIVER_H
#define MRAYCASTINGDRIVER_H

#include "rutils.h"
#include "CMList.h"
#include "CMPtrList.h"

// �������� ������ ��õ �ܺ����� �������� ���. MObstacle::GetOutPos()
// 1.0f�� ������ ���� ��Ȯ�ϱ� ������, �ܺ������� �н����ε� �ϴµ� ������ �����.
void SetOutPosConstant(float fConstant = 1.1f);

class MObstacle{
public:
	rvector					m_Pos;
	CMPtrList<MObstacle>	m_OverlappedObstacles;
	int						m_nTempUsed;
public:
	MObstacle(rvector& Pos);
	virtual ~MObstacle(void);
	virtual rvector GetOutPos(rvector& Pos) = 0;							// Pos�������� ���� ����� �ܺ��� ���ϱ�
	virtual bool IsCollision(rvector& Pos) = 0;								// ������ ��ֹ��� ���� �ִ°�?
	virtual bool IsCollision(rvector* pNewPos, rvector& Pos);				// ������ ��ֹ��� ���� �ִ°�?
	virtual bool IsIntersect(float* t, rvector& Pos, rvector& Dir, float fMagnitude) = 0;	// ���� ����
	virtual void GetEdge(rvector* pLeft, rvector* pRight, rvector& Pos) = 0;				// Pos���� �ٶ󺸴� Edge ���ϱ�
	bool IsOverlap(MObstacle* pObstacle);													// �� Obstacle�� �������°�?
	virtual bool GetDiffFromSurface(rvector* pDiff, rvector& p) = 0;						// �ܰ������� p���� �Ÿ� ���ϱ�
	virtual float GetDistance(MObstacle* pObstacle) = 0;									// Obstacle���� ǥ�鿡���� �Ÿ� ���ϱ�

	rvector GetCenter(void);				// �߽� ��ġ ( m_Pos ) ���
	virtual float GetArea(void) = 0;		// ���� ���ϱ�

	virtual void EnLarge(float fAdd) = 0;	// fAdd��ŭ ���� Ű���

	void CheckOverlappedObstacleTemp(int nTemp);

	// ������ ������Ʈ���� ������ �¿� Edge ���ϱ�
	void GetOverlappedEdge(rvector* pLeft, rvector* pRight, rvector& Pos, CMPtrList<MObstacle>* pLeftObsList=NULL, CMPtrList<MObstacle>* pRightObsList=NULL);
protected:
	MObstacle* GetOverlappedLeftEdge(rvector* pLeft, rvector& Pos, MObstacle* pAncestor=NULL, CMPtrList<MObstacle>* pObsList=NULL);
	MObstacle* GetOverlappedRightEdge(rvector* pRight, rvector& Pos, MObstacle* pAncestor=NULL, CMPtrList<MObstacle>* pObsList=NULL);
};

class MSphereObstacle : public MObstacle{
};

class MBoxObstacle : public MObstacle{
};

// 2D ��
class MCircleObstacle : public MObstacle{
protected:
	float	m_fRadius;
public:
	MCircleObstacle(rvector& Pos, float fRadius);

	virtual rvector GetOutPos(rvector& Pos);							// Pos�������� ���� ����� �ܺ��� ���ϱ�
	virtual bool IsCollision(rvector& Pos);								// ������ ��ֹ��� ���� �ִ°�?
	virtual bool IsCollision(rvector* pNewPos, rvector& Pos);							// ������ ��ֹ��� ���� �ִ°�?
	virtual bool IsIntersect(float* t, rvector& Pos, rvector& Dir, float fMagnitude);	// ���� ����
	virtual void GetEdge(rvector* pLeft, rvector* pRight, rvector& Pos);				// Pos���� �ٶ󺸴� Edge ���ϱ�
	//virtual bool IsOverlap(MObstacle* pObstacle);										// �� Obstacle�� �������°�?
	virtual bool GetDiffFromSurface(rvector* pDiff, rvector& p);						// �ܰ������� p���� �Ÿ� ���ϱ� ( p�� �ܺο� ������ true�� ���� )
	virtual float GetDistance(MObstacle* pObstacle);									// Obstacle���� ǥ�鿡���� �Ÿ� ���ϱ�

	virtual float GetArea(void);
	virtual void EnLarge(float fAdd);
};

// 2D �簢��
class MRectangleObstacle : public MObstacle{
protected:
	float	m_fWidth;		// ���� ( x )
	float	m_fHeight;		// ���� ( y )

protected:
	int GetDivision(rvector& Pos);
public:
	MRectangleObstacle(rvector& Pos, float w, float h);
	MRectangleObstacle(float x1, float y1, float x2, float y2);

	virtual rvector GetOutPos(rvector& Pos);							// Pos�������� ���� ����� �ܺ��� ���ϱ�
	virtual bool IsCollision(rvector& Pos);								// ������ ��ֹ��� ���� �ִ°�?
	virtual bool IsIntersect(float* t, rvector& Pos, rvector& Dir, float fMagnitude);	// ���� ����
	virtual void GetEdge(rvector* pLeft, rvector* pRight, rvector& Pos);				// Pos���� �ٶ󺸴� Edge ���ϱ�
	virtual bool GetDiffFromSurface(rvector* pDiff, rvector& p);						// �ܰ������� p���� �Ÿ� ���ϱ� ( p�� �ܺο� ������ true�� ���� )
	virtual float GetDistance(MObstacle* pObstacle);									// Obstacle���� ǥ�鿡���� �Ÿ� ���ϱ�

	virtual float GetArea(void);
	virtual void EnLarge(float fAdd);
};

class MReverseRectangleObstacle{
public:
	rvector	m_Pos;
	float	m_fWidth;
	float	m_fHeight;
public:
	MReverseRectangleObstacle(rvector& p, float w, float h);
};

/*
class MTerrainObstacle : public MObstacle{
};
*/

// �þ߿� ���̴� ��ֹ��� ���ذ��� �н� ���δ�
class MRayCastingDriver{
protected:
	MReverseRectangleObstacle*	m_pRRObstacle;
public:
	CMPtrList<MObstacle>		m_Obstacles;

protected:
	void CheckOverlappedObstacleTemp(MObstacle* p, int nTemp);
	void GetArea(float* pLeftArea, float* pRightArea, MObstacle* pColObstacle, rvector& Pos, rvector& Target);
	void GetArea(float* pLeftArea, float* pRightArea, MObstacle* pColObstacle, MObstacle* pAncestor, bool bAncestorLeft, rvector& Pos, rvector& Target, int nMagicNumber);
public:
	MRayCastingDriver(void);
	virtual ~MRayCastingDriver(void);

	void AddObstacle(MObstacle* pObstacle);			// ������ ��ֹ��� �߰�
	void AddObstacle(CMPtrList<MObstacle>* pOL);	// ������ ��ֹ��� �߰�
	void AddObstacle(CMLinkedList<MObstacle>* pOL);	// ������ ��ֹ��� �߰�
	void DeleteObstacle(void);

	bool IsCollision(rvector* pNewPos, rvector& Pos);
	//MObstacle* IsCollision(rvector* pNewPos, rvector& Pos);

	void SetReverseRectangleObstacle(rvector& p, float w, float h);
	void RemoveReverseRectangleObstacle(void);

	rvector GetNextDir(rvector Pos, rvector Dir, float fMagnitude, rvector CurDir);
};

#endif
