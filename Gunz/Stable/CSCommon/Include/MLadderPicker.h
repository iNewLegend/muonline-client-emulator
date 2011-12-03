#ifndef _MLADDERPICKER_H
#define _MLADDERPICKER_H

#pragma once

#include <list>
#include <queue>
#include <algorithm>
using namespace std;



class MLadderTicket {
private: 
	int		m_nGroupID;
	int		m_nScore;
	int		m_nRandomArg;

	// �߰�
	int		m_nCharLevel;		// ĳ���� ���� ���
	int		m_nClanPoint;		// Ŭ�� ����Ʈ
	int		m_nContPoint;		// Ŭ�� �⿩�� ���

	int		m_nTickCount;
public:
	// ���� �׼Ǹ��� ������ ����
	MLadderTicket(int nGroupID, int nScore, int nRandomArg) 
	{
		m_nGroupID = nGroupID;
		m_nScore = nScore;
		m_nRandomArg = nRandomArg;

		m_nCharLevel = 0;
		m_nClanPoint = 0;
		m_nContPoint = 0;

		m_nTickCount = 0;
	}

	MLadderTicket(int nGroupID, int nCharLevel, int nContPoint, int nClanPoint, int nTickCount, int nRandomArg)
	{
		m_nGroupID = nGroupID;
		m_nCharLevel = nCharLevel;
		m_nContPoint = nContPoint;
		m_nClanPoint = nClanPoint;
		m_nTickCount = nTickCount;

		m_nScore = 0;
		m_nRandomArg = nRandomArg;
	}
	int GetGroupID()	{ return m_nGroupID; }
	int GetEvaluation()	{ return m_nScore + m_nRandomArg; }

	int GetCharLevel()	{ return m_nCharLevel; }
	int GetClanPoint()	{ return m_nClanPoint; }
	int GetContPoint()	{ return m_nContPoint; }
	int GetTickCount()	{ return m_nTickCount; }
};

class MLadderPicker {
protected:
	list<MLadderTicket*>		m_LadderMatchList;

	bool Evaluate(MLadderTicket* pTicket, list<MLadderTicket*>::iterator* poutItorMatchedTicket);
	bool EvaluateTicket(MLadderTicket* pTicketA, MLadderTicket* pTicketB, float* poutTotalRate);
public:
	void AddTicket(MLadderGroup* pGroup, int nRandomArg);	// ���� �׼Ǹ��� ������ ����
	void AddTicket(MLadderGroup* pGroup, int nClanPoint, int nTickCount, int nRandomArg);

	bool PickMatch(int* pGroupA, int* pGroupB);
	void Shuffle();
};


#endif