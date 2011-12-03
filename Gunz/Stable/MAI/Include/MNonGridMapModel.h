/*

  ����ȭ���� ���� �ִ� �Ÿ� �˻� �� ��
  --------------------------------------

  1999/10/26, lee jang ho
  All copyright (c) 1998, MAIET entertainment software, inc

*/
#ifndef MNONGRIDMAPMODEL_H
#define MNONGRIDMAPMODEL_H

#include "CMList.h"
#include "CMPtrList.h"
#include "MAStar.h"

/*
	���׸�Ʈ�� Right-Hand�� ���⼺�� ������.

	   ->
	+-----+
	|  in |
	|     | out
	+-----+
	  <-
*/


// �˻� ������ Point Node
// �������� �� �𼭸��� �˻��� ��尡 �ȴ�.
class MPointNode : public MNodeModel{
public:
	float		m_fX, m_fY;
	CMPtrList<MPointNode>	m_VisibleList;	// ���̴� �� ����Ʈ
	MPointNode*		m_pPreviousPoint;		// Polygon�� �������� ���� ����Ʈ
	MPointNode*		m_pNextPoint;			// Polygon�� �������� ���� ����Ʈ
	void*			m_pTemp;				// �ӽ� ������ ( ������� �ʴ´�. )
	bool			m_bPreCalcVisiblePoint;
public:
	MPointNode(float x, float y);
	MPointNode(void);
	void ResetVisibleList(void);
	void ResetConnectPoint(void);
	bool IsVisible(MPointNode* pPN);
	virtual int GetSuccessorCount(void);
	virtual MNodeModel* GetSuccessor(int i);
	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	virtual float GetHeuristicCost(MNodeModel* pNode);
};


enum MPOLYGONTYPE{
	MPT_NORMAL = 0,		// Ư�� ����� ���� ������
	MPT_RECT,			// ���簢�� ����� ������, ���̴� ������ ����ÿ� ������ ������ �ش�.
};


// ������ ��ֹ�
// Right-Handed�̹Ƿ�, �ݴ������δ� �̵� �����ϴ�.
class MPolygonObstacle{
public:
	CMPtrList<MPointNode>	m_PointList;		// �� �𼭸�
	bool					m_bEnclosed;		// �������� ���� �ִ°�?
	MPOLYGONTYPE			m_nType;			// Ÿ��
	bool					m_bInnerGoal;		// ��ǥ ������ �� ������ ���ʿ� ���� �ִ� ���.
public:
	MPolygonObstacle(void);
	// Rect Type�� Polygon ����
	MPolygonObstacle(MPointNode* lt, MPointNode* rt, MPointNode* rb, MPointNode* lb);
	void Add(MPointNode* pPointNode);
	void Delete(int i);
	void Enclose(bool bEnclosed);
	// � ���� �� ������ �ȿ� ���� �ִ°�?
	bool IsInnerPoint(float x, float y);

	MPointNode* GetStartPoint(void);
	MPointNode* GetEndPoint(void);
	MPointNode* Get(int i);

	void Add(MPolygonObstacle* pPO);

	void GetPointNormal(float* x, float* y, int i);
	void Shrink(float fMargin);
};

// Point �߰��� �������� ���� ���.
enum MPolygonObstacleADDTYPE{
	MPAT_CONTINUE = 0,	// ��� �������� �̾�����.
	MPAT_END,			// �������� ������.
	MPAT_ENCLOSE,		// �������� �ݰ� ������.
};


// ����ȭ���� ���� �� ��
// GenerateVisiblePoint()�� �� �Ŀ� �ִ� �Ÿ� �˻��� �ؾ� �Ѵ�.
class MNonGridMapModel{
protected:
	MPolygonObstacle*	m_pCurrPolygon;			// Point�� �߰��Ǵ� ���� ������
	MPointNode*			m_pStartPoint;			// ���� ��
	MPointNode*			m_pEndPoint;			// �� ��
	bool				m_bEndPointInPolygon;	// �� ���� � �����￡ ���� �ִ°�?
public:
	CMLinkedList<MPointNode>		m_PointList;
	CMLinkedList<MPolygonObstacle>	m_PolygonList;

protected:
	//bool IsVisiblePoint(MPointNode* p1, MPointNode* p2);
	int IsBiDirectionVisiblePoint(MPointNode* p1, MPointNode* p2);	// 0: ����, 1: p1->p2, 2:p2->p1, 3:p1<->p2
public:
	MNonGridMapModel(void);
	void Destroy(void);

	MPointNode* AddPoint(float x, float y, bool bCheckDup=false);			// �Ϲ��� �߰� ( Point�� ��ĥ �� �ִ� ��� �ߺ� üũ�� �Ѵ�. )
	MPointNode* AddStartPoint(float x, float y);	// ������ �߰�
	MPointNode* AddEndPoint(float x, float y);		// �� �� �߰�
	MPolygonObstacle* AddPolygon(MPointNode* pNode, MPolygonObstacleADDTYPE nType=MPAT_CONTINUE);	// ���� �����￡ �� �߰�
	MPolygonObstacle* AddRect(float x1, float y1, float x2, float y2, bool bInverse=false);	// �� �簢�� ������ ������ �߰�
	void GenerateVisiblePoint(void);			// �� ������ ���̴� �� ã��
	void Reset(void);							// �ʱ�ȭ
	bool IsEndPointInPolygon(void);				// �� ���� �����￡ ���� �ִ°�?
	void MergeBreakPolygon(void);				// ���� �ٸ� �����ִ�(MPAT_END) ��������� �������̳� ������ ���ļ� ������ ������ ������ ��ġ��

	void Add(MNonGridMapModel* pNGMM, bool bVisibleTest=false);

	void Shrink(float fMargin);
};

#endif
