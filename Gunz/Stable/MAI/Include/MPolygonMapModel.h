/*

  3���� �������� ���ӵ� ����Ʈ�� �����Ǵ� �� ��
  ��鿡 ��������(z==0)�ؼ� 2���� ������ ����, �н��� ã�´�.

  Room�� ���� �ٰ������� �����Ǿ� �ִ�.

  All copyright (c) 1998, MAIET entertainment software, inc

*/
#ifndef MPolygonMapModel_H
#define MPolygonMapModel_H

#include "CMList.h"
#include "CMPtrList.h"
#include "MAStar.h"
#include <list>
#include <map>

using namespace std;

/*
	���׸�Ʈ�� Right-Hand�� ���⼺�� ������.

	   ->
	    /\
	   /  \
	  / in \
	 /      \ out
	+--------+
        <-
*/


/*
class MPMPoint;
typedef list<MPMPoint*>		MPNVList;
typedef MPNVList::iterator	MPNVListItor;
*/

class MPMPolygon;
class MPMPortal;
class MPolygonMapModel;

// �˻� ������ Point Node
// �������� �� �𼭸��� �˻��� ��尡 �ȴ�.
class MPMPoint : public MNodeModel{
protected:
friend class MPMPolygon;
friend class MPMPortal;
friend class MPolygonMapModel;
	MPolygonMapModel*		m_pParentMap;
	float				m_fX, m_fY, m_fZ;
	CMPtrList<MPMPoint>	m_VisibleList;		// ���̴� �� ����Ʈ
	int					m_nLastChangeCount;	// �ֱٱ��� �ݿ��� �� Map�� ChangeCount
	MPMPoint*			m_pPrevPoint;		// Polygon�� �������� ���� ����Ʈ
	MPMPoint*			m_pNextPoint;		// Polygon�� �������� ���� ����Ʈ
	CMPtrList<MPMPoint>	m_VisibledList;		// ������ �������� �ٸ� �� ����Ʈ
	void*				m_pTemp;			// �ӽ� ������ ( ������� �ʴ´�. )

	//void*				m_pRoomIndex;
	list<int>			m_RoomIndexs;
	CMPtrList<MPMPortal>	m_InPortals;
	
protected:
	bool IsParentVisiblePoint(MPMPoint* p);	// Parent Point(Path�����߿�..)���� p�� ���̴°� �׽�Ʈ
	void GenerateVisiblePoint(void);		// ���̴� �� ã��
	void ResetVisibleList(void);
	void AddVisiblePoint(MPMPoint* pPoint);

public:
	MPMPoint(float x, float y, float z, int nRoomIndex);
	MPMPoint(float x, float y, float z, list<int>& RoomIndexs);
	MPMPoint(void);

	inline float GetX(void){ return m_fX; }
	inline float GetY(void){ return m_fY; }
	inline float GetZ(void){ return m_fZ; }

	MPMPoint* GetPrevPoint(void){ return m_pPrevPoint; }
	MPMPoint* GetNextPoint(void){ return m_pNextPoint; }

	int GetVisiblePointCount(void);
	MPMPoint* GetVisiblePoint(int i);

	void ResetConnectPoint(void);
	bool IsVisible(MPMPoint* pPN);

	virtual int GetSuccessorCount(void);
	virtual MNodeModel* GetSuccessor(int i);

	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	virtual float GetHeuristicCost(MNodeModel* pNode);

	/// �� ��尡 ���� Room�߿� ���� ���� �ε���
	int GetMinRoomIndex(void);
	/// �� ��尡 ���� Room�߿� ���� ū �ε���
	int GetMaxRoomIndex(void);

	//virtual float GetTotalCostPriority(void){ return m_fCostToGoal; }	// Goal������ Cost���� ����Ѵ�.
};


enum MPMPOLYGONTYPE{
	MPMPT_NORMAL = 0,		// Ư�� ����� ���� ������
	MPMPT_RECT,				// ���簢�� ����� ������, ���̴� ������ ����ÿ� ������ ������ �ش�.
};

// �������� ����
enum MPMPOLYGONDIR{
	MPMPD_NA = 0,				// Not Available
	MPMPD_CLOCKWISE = 1,		// Clockwise
	MPMPD_COUNTERCLOCKWISE = 2,	// Counter-Clockwise
};

// ������ ��ֹ�
// Right-Handed�̹Ƿ�, �ݴ������δ� �̵� �����ϴ�.
class MPMPolygon{
public:
	CMPtrList<MPMPoint>		m_PointList;		// �� �𼭸�
	bool					m_bEnclosed;		// �������� ���� �ִ°�?
	MPMPOLYGONTYPE			m_nType;			// Ÿ��
	bool					m_bInnerStartPoint;	// ��ǥ ������ �� ������ ���ʿ� ���� �ִ� ���.
	bool					m_bInnerEndPoint;	// ��ǥ ������ �� ������ ���ʿ� ���� �ִ� ���.
	MPMPOLYGONDIR			m_nPolygonDir;		// �������� ���� ( �������� ���� �ִ� ��츸, m_bEnclosed==true )
protected:
	// Merge Support
	CMPtrList<MPMPoint>		m_TempMergedPointAddList;		// ������ ���� �ӽ� ������ ����Ʈ
	CMPtrList<MPMPoint>		m_TempMergedPointDupList;		// ������ ���� �ӽ� ������ ����Ʈ, ��ġ�� ����Ʈ
	int						m_nLatestFindIndex;
	MPMPoint*				m_pMergeTestBegin;
	MPMPoint*				m_pMergeTestEnd;
	//int						m_nMergeBeginIndex, m_nMergeEndIndex;	// �����׽�Ʈ�� ���۰� �� �ε���
protected:
	MPMPOLYGONDIR TestPolygonDir(void);			// ������ ���� �׽�Ʈ

	void Initialize(void);
public:
	MPMPolygon(void);
	// Rect Type�� Polygon ����
	MPMPolygon(MPMPoint* lt, MPMPoint* rt, MPMPoint* rb, MPMPoint* lb);

	void Add(MPMPoint* pPointNode);
	void Delete(int i);
	void Enclose(bool bEnclosed);

	// �ϳ� �̻��� ���ӵ� ������ �����ϴ� �� �������� �����ϴ� �������̽�, �����Ϸ��� �������� ����Ʈ�� �ϳ��� �߰��� ������ ���
	void Merge(MPMPoint* pPointNode);
	void MergeReset(void);	// Merge�� ���õ� �ӽ� �����͸� �����Ѵ�.

	void InsertBefore(int nIndex, MPMPoint* pPointNode);
	void AddAfter(int nIndex, MPMPoint* pPointNode);
	void InsertBefore(int nIndex, CMPtrList<MPMPoint>* pList);
	void AddAfter(int nIndex, CMPtrList<MPMPoint>* pList);

	MPMPoint* Find(float x, float y, float z, MPMPoint* pTestBegin, MPMPoint* pTestEnd);
	int FindIndex(float x, float y, float z, MPMPoint* pTestBegin, MPMPoint* pTestEnd);
	int GetIndex(MPMPoint* pTest);

	// � ���� �� ������ �ȿ� ���� �ִ°�?
	bool IsInnerPoint(float x, float y, bool bTestClockwise);
	MPMPOLYGONDIR IsInnerPoint(float* t, float x, float y);

	MPMPoint* GetStartPoint(void);
	MPMPoint* GetEndPoint(void);
	MPMPoint* Get(int i);

	void Add(MPMPolygon* pPO);

	void GetPointNormal(float* x, float* y, int i);
	void Enlarge(float fMargin);

	void RebuildConnection(void);

	bool IsIntersect(MPMPoint** p1, MPMPoint** p2, float* t, float x1, float y1, float x2, float y2);	// p1->p2?
	float GetNearestContactPoint(float* px, float* py, float x, float y, bool bInverse=false);			// ���� ����� ������ �������� ��
	//float GetContactPointByLineTest(float* px, float* py, float x, float y, int nTestCount);			// ������ nTestCount��ŭ �׾���� ���� ����� �� ã��, ���� ����� ���� ã�� �������... ���ѵ� �׽�Ʈ Ƚ���� �����Ѵ�.

	MPMPOLYGONDIR GetDir(void);		// Winding Dir
};

// Point �߰��� �������� ���� ���.
enum MPMPOLYGONADDTYPE{
	MPMPAT_CONTINUE = 0,	// ��� �������� �̾�����.
	MPMPAT_END,			// �������� ������.
	MPMPAT_ENCLOSE,		// �������� �ݰ� ������.
};

// PreparePathFinding()�� Return Value
enum MPMRESULT{
	MPMR_OK,
	MPMR_ENDPOINTCHANGED,
};

struct MPOLYGONIDPAIR{
	int a, b;
	inline friend bool operator == (const MPOLYGONIDPAIR& a, const MPOLYGONIDPAIR& b){
		if(a.a==b.a && a.b==b.b) return true;
		return false;
	}
	inline friend bool operator > (const MPOLYGONIDPAIR& a, const MPOLYGONIDPAIR& b){
		if(a.a>b.a) return true;
		if(a.a<b.a) return false;
		if(a.b>b.b) return true;
		return false;
	}
	inline friend bool operator < (const MPOLYGONIDPAIR& a, const MPOLYGONIDPAIR& b){
		if(a.a<b.a) return true;
		if(a.a>b.a) return false;
		if(a.b<b.b) return true;
		return false;
	}
};

typedef map<MPOLYGONIDPAIR, bool>	MPOLYGONIDPAIRMAP;
/*
struct MPOLYGONIDPAIR{
	int a, b;
};
struct ltstr
{
	bool operator()(const MPOLYGONIDPAIR& s1, const MPOLYGONIDPAIR& s2) const
	{
		if(s1.a<s2.b) return true;
		return false;
		//return strcmp(s1, s2) < 0;
	}
};
typedef map<MPOLYGONIDPAIR, bool, ltstr>	MPOLYGONIDPAIRMAP;
*/

class MPMPortal{
//protected:
//friend class MPolygonMapModel;
public:
	MPMPoint*	m_pPos[2];	// Portal Position
	int			m_nPortalIndex;
public:
	MPMPortal(MPMPoint* pPos1, MPMPoint* pPos2, int nPortalIndex);
	virtual ~MPMPortal(void);
	void Enlarge(float fMargin);
};


// ����ȭ���� ���� �� ��
class MPolygonMapModel{
protected:
	MPMPolygon*					m_pCurrPolygon;			// Point�� �߰��Ǵ� ���� ������
	bool						m_bEndPointInPolygon;	// �� ���� � �����￡ ���� �ִ°�?
	int							m_nChangeCount;			// ���� ����� ī��Ʈ(���� �ƴ��� �� �� �ִ�.)
	int*						m_pPolygonPathList;
	int							m_nPolygonPathCount;
	MPOLYGONIDPAIRMAP			m_PolygonSharedMap;

public:
	MPMPoint*					m_pStartPoint;			// ���� ��
	MPMPoint*					m_pEndPoint;			// �� ��
	CMLinkedList<MPMPoint>		m_PointList;
	CMLinkedList<MPMPolygon>	m_PolygonList;
	CMLinkedList<MPMPortal>		m_PortalList;

protected:
	void Change(void);									// ���� ��ȭ ����
	void GetInnerClosedPolygon(CMPtrList<MPMPolygon>* pPolygons, int nPointType, MPMPOLYGONDIR dir);	// nPointType=0 Start, nPointType=1 End Point

	bool TestEndPointInPolygon(void);					// ������ ������ �ȿ� ���� �ִ��� �׽�Ʈ, �̸� �׽�Ʈ�����ν� ��ġ Ÿ�ӿ� ���� �׻��� ���Ҽ� �ִ�.
	bool TestStartPointInPolygon(void);					// �������� ������ �ȿ� ���� �ִ��� �׽�Ʈ, �̸� �׽�Ʈ�����ν� ��ġ Ÿ�ӿ� ���� �׻��� ���Ҽ� �ִ�.

	// ������ ����Ʈ�߿� Non-Intersect�ϴ� ���� ��õ(sx,sy ������, ex,ey ����)
	bool FindNonIntersectPosition(CMPtrList<MPMPolygon>* pPolygons, float* rx, float* ry, float sx, float sy, float ex, float ey);
	// ������ ����Ʈ�߿� Non-Intersect�ϴ� ���� ��õ(x,y���� ����� ��)
	bool FindNonIntersectPosition(CMPtrList<MPMPolygon>* pPolygons, float* rx, float* ry, float x, float y);

	bool TestPointInPolygon(float* pNearestPointX, float* pNearestPointY, float x, float y);		// �����ִ��� �׽�Ʈ��, ���� ����� �ܺ� ���� ����
	bool RecommendPoint(float* pRecommendedPointX, float* pRecommendedPointY, float x, float y, float fCheckStepDistance, float fExponentialIncrease=1.0f);	// �浹���� �ʴ� �������� ��õ
	bool RecommendEndPoint(float* pRecX, float* pRecY);	// �浹���� �ʴ� �������� ��õ(return true�� ������, ����.. TestEndPointInPolygon()�� ����� �Ѵ�.

	friend class MPMPoint;
	void ClearVisibledList(void);				// MPMPoint::m_VisibledList Clear

	bool IsVisibleEdgePoint(MPMPoint* p1, MPMPoint* p2, int nBeginRoom, int nEndRoom);					// p1->p2?	( Edge�� �˻� )
	int IsBiDirectionVisiblePoint(float* pT1, float* pT2, MPMPoint* p1, MPMPoint* p2);					// 0: ����, 1: p1->p2, 2:p2->p1, 3:p1<->p2

public:
	MPolygonMapModel(void);
	virtual ~MPolygonMapModel(void);

	void Destroy(void);

	MPMPoint* AddPoint(float x, float y, float z, int nRoomIndex, bool bCheckDup=false);							// �Ϲ��� �߰� ( Point�� ��ĥ �� �ִ� ��� �ߺ� üũ�� �Ѵ�. )
	MPMPoint* AddPoint(float x, float y, float z, list<int>& RoomIndexs, bool bCheckDup=false);					// �Ϲ��� �߰� ( Point�� ��ĥ �� �ִ� ��� �ߺ� üũ�� �Ѵ�. )
	MPMPolygon* AddPolygon(MPMPoint* pNode, MPMPOLYGONADDTYPE nType=MPMPAT_CONTINUE);	// ���� �����￡ �� �߰�
	MPMPolygon* AddRect(float x1, float y1, float z1, float x2, float y2, float z2, int pRoomIndexs[4], bool bInverse=false);	// �� �簢�� ������ ������ �߰�

	MPMPortal* AddPortal(MPMPoint* p1, MPMPoint* p2);

	MPMPoint* AddStartPoint(float x, float y, float z, int nRoomIndex);				// ������ �߰�
	MPMPoint* AddEndPoint(float x, float y, float z, int nRoomIndex);				// �� �� �߰�
	MPMPoint* GetStartPoint(void);
	MPMPoint* GetEndPoint(void);

	void SetStartPoint(float x, float y, float z, int nRoomIndex);					// ������
	void SetEndPoint(float x, float y, float z, int nRoomIndex);					// �� ��

	// Low-Level Addition
	void AddPoint(MPMPoint* pPoint);												// ����Ʈ �߰�
	void AddPolygon(MPMPolygon* pPolygon);											// ������ �߰�
	void AddStartPoint(MPMPoint* pPoint);											// ������ �߰�
	void AddEndPoint(MPMPoint* pPoint);												// �� �� �߰�
	void AddPortal(MPMPortal* pPortal);												// ��Ż �߰�

	void Clear(void);																	// �߰��� Point, Polygon �ʱ�ȭ

	MPMRESULT PreparePathFinding(float* pRecX, float* pRecY);							// Path Finding Prepare ( End Point�� ������ ��ġ��Ų��.)

	void SetPolygonPathList(int* pPolygonList, int nCount);										// ���������� ����������� �н� ����Ʈ
	bool IsPolygonPathInclude(int nRoomIndexTest, int nRoomIndexBegin, int nRoomIndexEnd);	// Begin~End ���̿� ���ԵǾ� �ִ°�?
	bool IsPolygonPathInclude(list<int>* pRoomIndexTest, list<int>* pRoomIndexBegin, list<int>* pRoomIndexEnd);

	void SetPolygonSharedMap(MPOLYGONIDPAIRMAP* pSharedMap);

	bool IsEndPointInPolygon(void);														// �� ���� �����￡ ���� �ִ°�?
	void MergeBreakPolygon(void);														// ���� �ٸ� �����ִ�(MPMPAT_END) ��������� �������̳� ������ ���ļ� ������ ������ ������ ��ġ��

	MPMPolygon* IsInnerClosedPolygon(float x, float y, bool bTestClockwise=true);		// ���� ������ �ȿ� ���� �ִ°�?
	void IsInnerClosedPolygon(CMPtrList<MPMPolygon>* pPolygons, float x, float y, bool bTestClockwise=true);	// ���� ������ �ȿ� ���� �ִ°�?

	bool IsIntersect(MPMPoint** p1, MPMPoint** p2, float* t, float x1, float y1, float x2, float y2);			// p1->p2?

	MPMPolygon* GetNearestInnerPolygon(CMLinkedList<MPMPolygon>* pPolygons, float x, float y);	// Inner Polygon�� ���� �����̿� �ִ� ������( ���⿡ ��� ���� ) (1, 0, 0) �����̴�.

	bool IsEnable(CMLinkedList<MPMPolygon>* pPolygons, float x, float y);	// Enable Position�ΰ�?
	bool IsEnable(float x, float y);									// Enable Position�ΰ�?

	void Add(MPolygonMapModel* pNGMM);

	void Enlarge(float fMargin);

	int GetChangeCount(void);
};

#endif
