/*

  �ٵ��� ������ ���� ��Ƽ�Ŵ��� ��
  ----------------------------------

  2000/03/02, lee jang ho
  All copyright (c) 1998, MAIET entertainment software, inc

*/
#ifndef MGridPartitionModel_H
#define MGridPartitionModel_H

#include "MAStar.h"


struct MGPPOINT{
	int x, y;
};
struct MGPRECT{
	int x, y;
	int w, h;
};
struct MGPLINE{
	MGPPOINT p[2];
};


class MGridPartition;


// MGridPartition������ �̿��� �ΰ� ������ ����
class MGridPartitionLink{
public:
	MGridPartition*		m_pGP;
	MGPLINE				m_Portal;		// m_pGP�� ���� ��Ż
	int					m_nDivision;	// m_pGP�� ���� ��Ż�� Division ( 0 ~ 4 )
};

// ������ ������ Partition
class MGridPartition : public MNodeModel{
	CMLinkedList<MGridPartitionLink>	m_Successors;
public:
	MGPRECT	m_Rect;

protected:
	void GetCenter(float* x, float* y);

public:
	MGridPartition(int x, int y, int w, int h);
	virtual ~MGridPartition(void);

	bool HasSuccessor(MGridPartition* pGP);
	void AddSuccessor(MGridPartition* pGP);
	// �̿� Node ���
	virtual int GetSuccessorCount(void);
	virtual MNodeModel* GetSuccessor(int i);
	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	virtual float GetHeuristicCost(MNodeModel* pNode);
	MGridPartitionLink* GetLink(MGridPartition* pGP);

	bool GetOutLines(CMLinkedList<MGPLINE>* pLines, CMPtrList<MGridPartition>* pGPs);
};


struct MGPMCELL{
	bool				bEnable;	// �� �� �ִ� Cell�ΰ�?
	MGridPartition*		pPart;		// �� Cell�� ���� �ִ� Partition
};


class MGridPartitionModel{
	MGPMCELL*	m_pCells;						// �� ����(�ٵ��� ���) ��
	int			m_nWidth, m_nHeight;
public:
	CMLinkedList<MGridPartition>	m_Parts;	// ��Ƽ�� ( ���簢�� ����� ������ �ִ� )

protected:
	// ���� ��Ƽ���� �ȵ� ������ �ִ� ���� ���ϱ�
	bool IsEmptyRangeWidth(int x, int y, int w);
	bool IsEmptyRangeHeight(int x, int y, int h);
	void GetEmptyRangeWidthFirst(int* w, int* h, int x, int y);		// Width�� ���� ���
	void GetEmptyRangeHeightFirst(int* w, int* h, int x, int y);	// Height�� ���� ���
	int GetEmptySquareSideLength(int x, int y);						// ������ �� ������ ã�´�.

	void FillPartition(int x, int y, int w, int h, MGridPartition* pGP);
public:
	MGridPartitionModel(void);
	virtual ~MGridPartitionModel(void);

	bool Create(int x, int y);
	void Destroy(void);

	MGPMCELL* Get(int x, int y);

	void Enable(int x, int y, bool bEnable=true);

	void Partition(void);
};

// �н����δ� ���(MGridPartition)�� �ƿ� ���� ����
void GenerateOutLine(CMLinkedList<MGPLINE>* pLines, MAStar* pPF, int nLimitPath=-1);


#endif