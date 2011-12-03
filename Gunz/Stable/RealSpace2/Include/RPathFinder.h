// PathFinder for RealSpace2
// MAI Required.

#ifndef RPATHFINDER_H
#define RPATHFINDER_H

#include <list>
using namespace std;

#include "RNameSpace.h"
#include "RTypes.h"
#include "CMPtrList.h"
#include "MPolygonMapModel.h"
#include "RPath.h"

_NAMESPACE_REALSPACE2_BEGIN

class RBspObject;
class RSBspNode;

class RVECTORLIST : public list<rvector*>{
public:
	virtual ~RVECTORLIST(void){
		Clear();
	}

	void Clear() {
		while(empty()==false){
			delete *begin();
			erase(begin());
		}
	}
};

// ��������� ���
class MRPathNode : public MNodeModel{
	RPathList*	m_pPathList;
	RPathNode*	m_pPathNode;
public:
	int			m_nShortestPathID;		/// Temporal Used

	static rvector		m_StartPos;		/// �˻��� ���� ����
	static rvector		m_EndPos;		/// �˻��� ���� ����
	static MRPathNode*	m_pStartNode;	/// �˻��� ���� ���
	static MRPathNode*	m_pEndNode;		/// �˻��� ���� ���
public:
	MRPathNode(RPathList* pPathList, RPathNode* pPathNode);
	virtual ~MRPathNode(void);

	RPathNode* GetRPathNode(void){ return m_pPathNode; }
	int GetRPathNodeIndex(void){ return m_pPathNode->m_nIndex; }

	/// �̿� Node ���
	virtual int GetSuccessorCount(void){
		return (int)m_pPathNode->m_Neighborhoods.size();
	}
	virtual MNodeModel* GetSuccessor(int i){
		_ASSERT(i>=0 && i<(int)m_pPathNode->m_Neighborhoods.size());
		return (MNodeModel*)((*m_pPathList)[m_pPathNode->m_Neighborhoods[i]->nIndex])->m_pUserData;
	}

	/// �� ��� ������ ��Ż ���ϱ�
	static bool GetSuccessorPortal(rvector v[2], MRPathNode* pParent, MRPathNode* pSuccessor);

	/// �̿� Node���� Cost ���
	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	/// ���������� �̿� Node������� Cost ���
	virtual float GetSuccessorCostFromStart(MNodeModel* pSuccessor);
	/// Heuristic Estimation�� ���� pNode������ Cost ���
	virtual float GetHeuristicCost(MNodeModel* pNode);
};

/// Path ���. ��ġ�� �ش� ���
struct RPATHRESULT{
	rvector		Pos;
	MRPathNode*	pPathNode;

	RPATHRESULT(void){}
	RPATHRESULT(rvector& Pos, MRPathNode* pPathNode){
		RPATHRESULT::Pos = Pos;
		RPATHRESULT::pPathNode = pPathNode;
	}
};

class RPathResultList : public list<RPATHRESULT*>{
public:
	virtual ~RPathResultList(void){
		Clear();
	}

	void Clear() {
		while(empty()==false){
			delete *begin();
			erase(begin());
		}
	}
};

/// PathFinder for RealSpace2
class RPathFinder{
	RBspObject*	m_pBSPObject;	// �ҽ� BSP

	RPathNode*	m_pStartNode;	// ���� Node
	RPathNode*	m_pEndNode;		// ���� Node
	rvector		m_StartPos;		// ���� ����
	rvector		m_EndPos;		// ���� ����
	MAStar		m_PolygonPathFinder;		// ������ ��带 ���� �н����δ�
	MPolygonMapModel	m_PolygonMapModel;	// VisibilityPath�� ���� �� ��
	float		m_fEnlarge;
protected:
//	bool Pick(RSBspNode** ppNode, int* pIndex, rvector* pPickPos, RPathNode** ppPathNode, int x, int y);
	bool FindVisibilityPath(RVECTORLIST* pVisibilityPathList, rvector& StartPos, rvector& EndPos, CMPtrList<MNodeModel>* pPolygonShortestPath, MPolygonMapModel* pPolygonMapModel=NULL);
	bool FindVisibilityPath(RPathResultList* pVisibilityPathList, rvector& StartPos, rvector& EndPos, CMPtrList<MNodeModel>* pPolygonShortestPath, MPolygonMapModel* pPolygonMapModel=NULL);
public:
	RPathFinder(void);
	virtual ~RPathFinder(void);

	void Create(RBspObject* pBSPObject);
	void Destroy(void);

	/// ������ ����
	/// @param	sx		ȭ�� ��ǥ�� x
	/// @param	sy		ȭ�� ��ǥ�� y
	bool SetStartPos(int sx, int sy);
	bool SetStartPos(rvector &position);

	/// ������ ���� ����
	void SetStartPos(RPathNode *pStartNode,rvector StartPos);

	/// ���� ����
	/// @param	sx		ȭ�� ��ǥ�� x
	/// @param	sy		ȭ�� ��ǥ�� y
	bool SetEndPos(int sx, int sy);
	bool SetEndPos(rvector &position);

	/// ��ã��. SetStartPos(), SetEndPos()�� �̸� �����Ǿ�� �Ѵ�.
	/// @param	pVisibilityPathList		�˻��� ���. �������� ���������� ���Եȴ�.
	/// @return							�˻� ���� ����
	bool FindPath(RVECTORLIST* pVisibilityPathList);
	bool FindPath(RPathResultList* pVisibilityPathList);

	/// ��ã��. ���������� �� �� �ִ� ��α����� ã���ش�.
	/// SetStartPos(), SetEndPos()�� �̸� �����Ǿ�� �Ѵ�.
	bool FindStraightPath(RVECTORLIST* pVisibilityPathList);

	/// �˻��� ������ ũ�⿡ ���� ��Ż�� ũ�⸦ ������ �ش�.
	void Enlarge(float fMargin);

	RPathNode* GetStartNode(void){ return m_pStartNode; }
	RPathNode* GetEndNode(void){ return m_pEndNode; }
	MAStar* GetPolygonPathFinder(void){ return &m_PolygonPathFinder; }
	MPolygonMapModel* GetPolygonMapModel(void){ return &m_PolygonMapModel; }
};


// ���� �ڵ�

// �ش� ��ġ�� ���ȿ� �����ϴ°�?
bool IsPosInNode(rvector& Pos, RPathNode* pNode);
// �ش� ��尡 ����Ǿ� �ִ°�?
int IsNodeConnection(RPathNode* pNode1, RPathNode* pNode2, RPathList* pPathList);
// �н��� ��Ż�� �������°�?
bool IsPathAcrossPortal(rvector& Pos, rvector& NextPos, RPathNode* pNode, RPathNode* pNextNode, int nNodeNeighborhoodID, RPathList* pPathList);
// �������� ������ ���ἱ�� ���۳��� �����ȿ��� ���� ���ͼ����� �߻����� �ʴ°�?
bool IsPathInNode(rvector& Pos, rvector& NextPos, RPathNode* pNode, RPathNode* pNextNode, int nNodeNeighborhoodID1, int nNodeNeighborhoodID2, RPathList* pPathList);


_NAMESPACE_REALSPACE2_END

#endif