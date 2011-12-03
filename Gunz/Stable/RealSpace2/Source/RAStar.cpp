#include "stdafx.h"
#include "RAStar.h"
#include <crtdbg.h>

RAStarNode::RAStarNode()
{
	m_fCostFromStart = 0.0f;
	m_fCostToGoal = 0.0f;
	m_pParent = NULL;
//	m_nMapID = 0;
//	m_ppSuccessors = NULL;
//	m_bClosed = false;
//	m_nValue = 0;
//	m_nVisitID = 0;
	m_nSessionID = 0;
	m_pData = NULL;

//	m_nSuccessorCount = 0;
	m_fWeight = 1.0f;
}

RAStarNode::~RAStarNode()
{

}

/////////////////////////////////////////////////////////////////////////////////////
RAStar::RAStar() : m_nPathSession(0)
{
//	m_pNodes = NULL;
}

RAStar::~RAStar()
{

}

void RAStar::PushOnOpenList(RAStarNode* pStartNode)
{
	m_OpenList.push_back(pStartNode);
}

bool RAStar::IsOpenListEmpty()
{
	return m_OpenList.empty();
}

RAStarNode* RAStar::PopLowestCostFromOpenList()
{
	RAStarNode* pOutNode = NULL;
	list<RAStarNode*>::iterator itorErase = m_OpenList.end();

	for (list<RAStarNode*>::iterator itor = m_OpenList.begin(); itor != m_OpenList.end(); ++itor)
	{
		RAStarNode* pNode = (*itor);
		if (pOutNode==NULL)
		{
			pOutNode = pNode;
			itorErase = itor;
		}
		else
		{
			if (pOutNode->GetTotalCost() > pNode->GetTotalCost())
			{
				pOutNode = pNode;
				itorErase = itor;
			}
		}
	}

	m_OpenList.erase(itorErase);
	return pOutNode;
}

#include "RNavigationNode.h"

void RAStar::PushToShortestPath(RAStarNode* pNode)
{
	RNavigationNode* pNNode = (RNavigationNode*)pNode;
	m_ShortestPath.push_back(pNode);
}


bool RAStar::Search(RAStarNode* pStartNode, RAStarNode* pGoalNode)
{
	// clear Open and Closed
	m_OpenList.clear();
	m_nPathSession++;

	// ���� ��带 �ʱ�ȭ�Ѵ�.
	// ���������� ��� P�� �д�.
	// P�� f, g, h������ �����Ѵ�.
	pStartNode->m_fCostFromStart = 0.0f;
	pStartNode->m_fCostToGoal = pStartNode->GetHeuristicCost(pGoalNode);
	pStartNode->m_pParent = NULL;

	// push StartNode on Open
	PushOnOpenList(pStartNode);

	// ���� �Ǵ� ���п� �̸� ������ ����� ó���Ѵ�.
	while (!IsOpenListEmpty())		// ���� ����� ����ٸ� ��θ� ã�� �� ����
	{
		// pop Node from Open
		RAStarNode* pNode = PopLowestCostFromOpenList();

		// ��ǥ�� ���������� �������� ������.
		if (pNode == pGoalNode)
		{
			// construct a path backward from Node to StartLoc
			m_ShortestPath.clear();
			RAStarNode* pShortestNode = pNode;
			while (pShortestNode != pStartNode)
			{
				PushToShortestPath(pShortestNode);
				pShortestNode = pShortestNode->m_pParent;
			}

			// �������� ���� ��带 �־��ش�.
//			PushToShortestPath(pStartNode);
			return true;
		}

		// for each successor NewNode of Node
		for(int i=0; i < pNode->GetSuccessorCount(); i++)
		{
			RAStarNode* pSuccessor = pNode->GetSuccessor(i);
			if(pSuccessor==NULL) continue;	// NULL�̸� ���� ���� �����Ѵ�.
			if(pSuccessor==pStartNode) continue;					// ���� ���� �ٽ� ���ƿ� �ʿ䰡 �����Ƿ� �����Ѵ�.
			if(pNode->m_pParent==pSuccessor) continue;

			float fNewCostFromStart = pNode->GetSuccessorCostFromStart(pSuccessor);

			// �� ��尡 �����ϸ� �� ���� ����� �ƴ϶�� �����Ѵ�.
			// �� ��尡 ���� ����̳� ���� ��Ͽ� ����ִ��� �����Ѵ�.
			if(pSuccessor->GetSessionID() == m_nPathSession)
			{
				if( pSuccessor->m_fCostFromStart <= fNewCostFromStart ) continue;	// Skip
			}

			// ���ο�, �� �� ���� ������ �����Ѵ�.
			pSuccessor->m_pParent = pNode;
			pSuccessor->m_fCostFromStart = fNewCostFromStart;
			pSuccessor->m_fCostToGoal = pSuccessor->GetHeuristicCost(pGoalNode);
			pSuccessor->OnSetData(i, pNode);
			

			if(pSuccessor->GetSessionID() != m_nPathSession)
			{
				pSuccessor->m_nSessionID = m_nPathSession;
				PushOnOpenList(pSuccessor);
			}

		}

	}	// while

	return false;
}


/*
-------------------------------------------------------------------------------------
Open: priorityqueue of searchnode
Closed: list of searchnode

AStarSearch(location StartLoc, location GoalLoc, agenttype Agent) {
	clear Open and Closed

	// ���� ��带 �ʱ�ȭ�Ѵ�.
	StartNode.Loc = StartLoc
	StartNode.CostFromStart = 0
	StartNode.CostToGoal = PathCostEstimate(StartLoc, GoalLoc, Agent)
	StartNode.Parent = null
	push StartNode on Open

	// ���� �Ǵ� ���п� �̸� ������ ����� ó���Ѵ�.
	while Open is not empty {
		pop Node from Open // ��尡 ������ TotalCost�� ������ ���

		// ��ǥ�� ���������� �������� ������.
		if (Node is a goal node) {
			construct a path backward from Node to StartLoc
			return success
		} else {
			for each successor NewNode of Node {
				NewCost = Node.CostFromStart + TraverseCost(Node, NewNode, Agent)
				// �� ��尡 �����ϸ� �� ���� ����� �ƴ϶�� �����Ѵ�.
				if (NewNode is in Open or Closed) and
				(NewNode.CostFromStart <= NewCost) {
					continue
				} else {	// ���ο�, �� �� ���� ������ �����Ѵ�.
					NewNode.Parent = Node
					NewNode.CostFromStart = NewCost
					NewNode.CostToGoal = PathCostEstimate(NewNode.Loc, GoalLoc, Agent)
					NewNode.TotalCost = NewNode.CostFromStart + NewNode.CostToGoal
					if (NewNode is in Closed) {
						remove NewNode from Closed
					}
					if (NewNode is in Open) {
						adjust NewNode's location in Open
					} else {
						push NewNode onto Open
					}
				} // if (NewNode is ... <= NewNode)�� ���� else ����� ��
			} // for
		} // if (Node is a gole node)
		push Node onto Closed
	} // while Open is not empty
	return false
}
*/
