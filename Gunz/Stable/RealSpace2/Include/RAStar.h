#ifndef _RASTAR_H
#define _RASTAR_H

#include <vector>
#include <list>
using namespace std;

class RAStar;

class RAStarNode
{
	friend			RAStar;
private:
	int				m_nSessionID;
protected:
	float			m_fCostFromStart;
	float			m_fCostToGoal;
	RAStarNode*		m_pParent;
	float			m_fWeight;
	void*			m_pData;
public:
	RAStarNode();
	virtual ~RAStarNode();
	float GetTotalCost(void)							{ return m_fCostFromStart + m_fCostToGoal; }
	// �̿� Node���� Cost ���
	float GetSuccessorCost(int i)						{ return GetSuccessorCost(GetSuccessor(i)); }

	// ���������� �̿� Node������� Cost ���
	virtual float GetSuccessorCostFromStart(RAStarNode* pSuccessor)
	{
		return m_fCostFromStart + GetSuccessorCost(pSuccessor);
	}

	// ��ӹ޾Ƽ� ����� ��
	virtual float GetSuccessorCost(RAStarNode* pSuccessor) = 0;
	virtual int GetSuccessorCount() = 0;
	virtual RAStarNode* GetSuccessor(int i) = 0;
	virtual float GetHeuristicCost(RAStarNode* pGoal) = 0;
	virtual void OnSetData(int nSuccessorIndex,RAStarNode* pParent) { }

	// --- Accessor ------------------------
	int GetSessionID()				{ return m_nSessionID; }
	void SetWeight(float fWeight)	{ m_fWeight = fWeight; }
	float GetWeight()				{ return m_fWeight; }
};


////////////////////////////////////////////////////////////////////////////
class RAStar
{
protected:
	int					m_nPathSession;
	void PushToShortestPath(RAStarNode* pNode);
public:
	list<RAStarNode*>	m_ShortestPath;
	list<RAStarNode*>	m_OpenList;

	void PushOnOpenList(RAStarNode* pStartNode);
	RAStarNode* PopLowestCostFromOpenList();
	bool IsOpenListEmpty();

	bool Search(RAStarNode* pStartNode, RAStarNode* pGoalNode);

	RAStar();
	virtual ~RAStar();
};


#endif