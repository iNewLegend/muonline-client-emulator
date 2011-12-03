/*

  ����ȭ���� ���� �ִ� �Ÿ� �˻� �� ��
  --------------------------------------

  1999/10/26, lee jang ho
  All copyright (c) 1998, MAIET entertainment software, inc

*/
#ifndef MFreeMapModel_H
#define MFreeMapModel_H

#include "CMList.h"
#include "CMPtrList.h"
#include "MAStar.h"
#include <list>

using namespace std;

/*
	���׸�Ʈ�� Right-Hand�� ���⼺�� ������.

	   ->
	+-----+
	|  in |
	|     | out
	+-----+
	  <-
*/


/*
class MFMPoint;
typedef list<MFMPoint*>		MPNVList;
typedef MPNVList::iterator	MPNVListItor;
*/

class MFMPolygon;
class MFreeMapModel;

// �˻� ������ Point Node
// �������� �� �𼭸��� �˻��� ��尡 �ȴ�.
class MFMPoint : public MNodeModel{
protected:
friend class MFMPolygon;
friend class MFreeMapModel;
	MFreeMapModel*		m_pParentMap;
	float				m_fX, m_fY;
	CMPtrList<MFMPoint>	m_VisibleList;		// ���̴� �� ����Ʈ
	int					m_nLastChangeCount;	// �ֱٱ��� �ݿ��� �� Map�� ChangeCount
	MFMPoint*			m_pPrevPoint;		// Polygon�� �������� ���� ����Ʈ
	MFMPoint*			m_pNextPoint;		// Polygon�� �������� ���� ����Ʈ
	CMPtrList<MFMPoint>	m_VisibledList;		// ������ �������� �ٸ� �� ����Ʈ
	void*				m_pTemp;			// �ӽ� ������ ( ������� �ʴ´�. )

protected:
	bool IsParentVisiblePoint(MFMPoint* p);	// Parent Point(Path�����߿�..)���� p�� ���̴°� �׽�Ʈ
	void GenerateVisiblePoint(void);		// ���̴� �� ã��
	void ResetVisibleList(void);
	void AddVisiblePoint(MFMPoint* pPoint);

public:
	MFMPoint(float x, float y);
	MFMPoint(void);

	inline float GetX(void){ return m_fX; }
	inline float GetY(void){ return m_fY; }

	MFMPoint* GetPrevPoint(void){ return m_pPrevPoint; }
	MFMPoint* GetNextPoint(void){ return m_pNextPoint; }

	int GetVisiblePointCount(void);
	MFMPoint* GetVisiblePoint(int i);

	void ResetConnectPoint(void);
	bool IsVisible(MFMPoint* pPN);

	virtual int GetSuccessorCount(void);
	virtual MNodeModel* GetSuccessor(int i);

	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	virtual float GetHeuristicCost(MNodeModel* pNode);
	

	//virtual float GetTotalCostPriority(void){ return m_fCostToGoal; }	// Goal������ Cost���� ����Ѵ�.
};


enum MFMPOLYGONTYPE{
	MFMPT_NORMAL = 0,		// Ư�� ����� ���� ������
	MFMPT_RECT,				// ���簢�� ����� ������, ���̴� ������ ����ÿ� ������ ������ �ش�.
};

// �������� ����
enum MFMPOLYGONDIR{
	MFMPD_NA = 0,				// Not Available
	MFMPD_CLOCKWISE = 1,		// Clockwise
	MFMPD_COUNTERCLOCKWISE = 2,	// Counter-Clockwise
};

// ������ ��ֹ�
// Right-Handed�̹Ƿ�, �ݴ������δ� �̵� �����ϴ�.
class MFMPolygon{
public:
	CMPtrList<MFMPoint>		m_PointList;		// �� �𼭸�
	bool					m_bEnclosed;		// �������� ���� �ִ°�?
	MFMPOLYGONTYPE			m_nType;			// Ÿ��
	bool					m_bInnerStartPoint;	// ��ǥ ������ �� ������ ���ʿ� ���� �ִ� ���.
	bool					m_bInnerEndPoint;	// ��ǥ ������ �� ������ ���ʿ� ���� �ִ� ���.
	MFMPOLYGONDIR			m_nPolygonDir;		// �������� ���� ( �������� ���� �ִ� ��츸, m_bEnclosed==true )
protected:
	MFMPOLYGONDIR TestPolygonDir(void);			// ������ ���� �׽�Ʈ
public:
	MFMPolygon(void);
	// Rect Type�� Polygon ����
	MFMPolygon(MFMPoint* lt, MFMPoint* rt, MFMPoint* rb, MFMPoint* lb);

	void Add(MFMPoint* pPointNode);
	void Delete(int i);
	void Enclose(bool bEnclosed);
	// � ���� �� ������ �ȿ� ���� �ִ°�?
	bool IsInnerPoint(float x, float y, bool bTestClockwise);
	MFMPOLYGONDIR IsInnerPoint(float* t, float x, float y);

	MFMPoint* GetStartPoint(void);
	MFMPoint* GetEndPoint(void);
	MFMPoint* Get(int i);

	void Add(MFMPolygon* pPO);

	void GetPointNormal(float* x, float* y, int i);
	void Enlarge(float fMargin);

	void RebuildConnection(void);

	bool IsIntersect(MFMPoint** p1, MFMPoint** p2, float* t, float x1, float y1, float x2, float y2);	// p1->p2?
	float GetNearestContactPoint(float* px, float* py, float x, float y, bool bInverse=false);			// ���� ����� ������ �������� ��
	//float GetContactPointByLineTest(float* px, float* py, float x, float y, int nTestCount);			// ������ nTestCount��ŭ �׾���� ���� ����� �� ã��, ���� ����� ���� ã�� �������... ���ѵ� �׽�Ʈ Ƚ���� �����Ѵ�.

	MFMPOLYGONDIR GetDir(void);		// Winding Dir
};

// Point �߰��� �������� ���� ���.
enum MFMPOLYGONADDTYPE{
	MFMPAT_CONTINUE = 0,	// ��� �������� �̾�����.
	MFMPAT_END,			// �������� ������.
	MFMPAT_ENCLOSE,		// �������� �ݰ� ������.
};

// PreparePathFinding()�� Return Value
enum MFMRESULT{
	MFMR_OK,
	MFMR_ENDPOINTCHANGED,
};

// ����ȭ���� ���� �� ��
class MFreeMapModel{
protected:
	MFMPolygon*					m_pCurrPolygon;			// Point�� �߰��Ǵ� ���� ������
	bool						m_bEndPointInPolygon;	// �� ���� � �����￡ ���� �ִ°�?
	int							m_nChangeCount;			// ���� ����� ī��Ʈ(���� �ƴ��� �� �� �ִ�.)
public:
	MFMPoint*					m_pStartPoint;			// ���� ��
	MFMPoint*					m_pEndPoint;			// �� ��
	CMLinkedList<MFMPoint>		m_PointList;
	CMLinkedList<MFMPolygon>	m_PolygonList;

protected:
	void Change(void);									// ���� ��ȭ ����
	void GetInnerClosedPolygon(CMPtrList<MFMPolygon>* pPolygons, int nPointType, MFMPOLYGONDIR dir);	// nPointType=0 Start, nPointType=1 End Point

	bool TestEndPointInPolygon(void);					// ������ ������ �ȿ� ���� �ִ��� �׽�Ʈ, �̸� �׽�Ʈ�����ν� ��ġ Ÿ�ӿ� ���� �׻��� ���Ҽ� �ִ�.
	bool TestStartPointInPolygon(void);					// �������� ������ �ȿ� ���� �ִ��� �׽�Ʈ, �̸� �׽�Ʈ�����ν� ��ġ Ÿ�ӿ� ���� �׻��� ���Ҽ� �ִ�.

	// ������ ����Ʈ�߿� Non-Intersect�ϴ� ���� ��õ(sx,sy ������, ex,ey ����)
	bool FindNonIntersectPosition(CMPtrList<MFMPolygon>* pPolygons, float* rx, float* ry, float sx, float sy, float ex, float ey);
	// ������ ����Ʈ�߿� Non-Intersect�ϴ� ���� ��õ(x,y���� ����� ��)
	bool FindNonIntersectPosition(CMPtrList<MFMPolygon>* pPolygons, float* rx, float* ry, float x, float y);

	bool TestPointInPolygon(float* pNearestPointX, float* pNearestPointY, float x, float y);		// �����ִ��� �׽�Ʈ��, ���� ����� �ܺ� ���� ����
	bool RecommendPoint(float* pRecommendedPointX, float* pRecommendedPointY, float x, float y, float fCheckStepDistance, float fExponentialIncrease=1.0f);	// �浹���� �ʴ� �������� ��õ
	bool RecommendEndPoint(float* pRecX, float* pRecY);	// �浹���� �ʴ� �������� ��õ(return true�� ������, ����.. TestEndPointInPolygon()�� ����� �Ѵ�.

	friend class MFMPoint;
	void ClearVisibledList(void);				// MFMPoint::m_VisibledList Clear

	bool IsVisibleEdgePoint(MFMPoint* p1, MFMPoint* p2);												// p1->p2?	( Edge�� �˻� )
	int IsBiDirectionVisiblePoint(float* pT1, float* pT2, MFMPoint* p1, MFMPoint* p2);					// 0: ����, 1: p1->p2, 2:p2->p1, 3:p1<->p2

public:
	MFreeMapModel(void);
	virtual ~MFreeMapModel(void);

	void Destroy(void);

	MFMPoint* AddPoint(float x, float y, bool bCheckDup=false);							// �Ϲ��� �߰� ( Point�� ��ĥ �� �ִ� ��� �ߺ� üũ�� �Ѵ�. )
	MFMPolygon* AddPolygon(MFMPoint* pNode, MFMPOLYGONADDTYPE nType=MFMPAT_CONTINUE);	// ���� �����￡ �� �߰�
	MFMPolygon* AddRect(float x1, float y1, float x2, float y2, bool bInverse=false);	// �� �簢�� ������ ������ �߰�

	MFMPoint* AddStartPoint(float x, float y);											// ������ �߰�
	MFMPoint* AddEndPoint(float x, float y);											// �� �� �߰�
	MFMPoint* GetStartPoint(void);
	MFMPoint* GetEndPoint(void);

	void SetStartPoint(float x, float y);												// ������
	void SetEndPoint(float x, float y);													// �� ��

	// Low-Level Addition
	void AddPoint(MFMPoint* pPoint);													// ����Ʈ �߰�
	void AddPolygon(MFMPolygon* pPolygon);												// ������ �߰�
	void AddStartPoint(MFMPoint* pPoint);												// ������ �߰�
	void AddEndPoint(MFMPoint* pPoint);													// �� �� �߰�

	void Clear(void);																	// �߰��� Point, Polygon �ʱ�ȭ

	MFMRESULT PreparePathFinding(float* pRecX, float* pRecY);							// Path Finding Prepare ( End Point�� ������ ��ġ��Ų��.)

	bool IsEndPointInPolygon(void);														// �� ���� �����￡ ���� �ִ°�?
	void MergeBreakPolygon(void);														// ���� �ٸ� �����ִ�(MFMPAT_END) ��������� �������̳� ������ ���ļ� ������ ������ ������ ��ġ��

	MFMPolygon* IsInnerClosedPolygon(float x, float y, bool bTestClockwise=true);		// ���� ������ �ȿ� ���� �ִ°�?
	void IsInnerClosedPolygon(CMPtrList<MFMPolygon>* pPolygons, float x, float y, bool bTestClockwise=true);	// ���� ������ �ȿ� ���� �ִ°�?

	bool IsIntersect(MFMPoint** p1, MFMPoint** p2, float* t, float x1, float y1, float x2, float y2);			// p1->p2?

	MFMPolygon* GetNearestInnerPolygon(CMLinkedList<MFMPolygon>* pPolygons, float x, float y);	// Inner Polygon�� ���� �����̿� �ִ� ������( ���⿡ ��� ���� ) (1, 0, 0) �����̴�.

	bool IsEnable(CMLinkedList<MFMPolygon>* pPolygons, float x, float y);	// Enable Position�ΰ�?
	bool IsEnable(float x, float y);									// Enable Position�ΰ�?

	void Add(MFreeMapModel* pNGMM);

	void Enlarge(float fMargin);

	int GetChangeCount(void);
};

#endif
