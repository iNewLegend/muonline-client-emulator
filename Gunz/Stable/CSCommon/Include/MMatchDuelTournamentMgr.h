#ifndef _MDUELTOURNAMENT_MANAGER_H
#define _MDUELTOURNAMENT_MANAGER_H

#include "MDuelTournamentMatchMaker.h"
#include "MDuelTournamentTimeChecker.h"

#define MAX_DT_GROUP_RANKING_COUNT 100

class MDuelTournamentMatchLauncher;

//////////////////////////////////////////////////////////////////////
// Duel Tournament�� ���� Ŭ�����̴�.
// ���� 1. ��� ��ʸ�Ʈ Match-Up�� �����ִ� MatchMaker ���
// ���� 2. ��� ��ʸ�Ʈ�� Group Ranking�� �������ִ� ���(������ �ð����� DB���� �޾ƿ��� ���)
//////////////////////////////////////////////////////////////////////

class MMatchDuelTournamentMgr
{
	MMatchObjectContainer m_matchObjectContainer;
	MDuelTournamentTimeChecker m_TimeChecker;

	DWORD m_lastMatchedTick;		// ���������� ��ġ ó���� �ð�
	DWORD m_nLastTimeCheckedTick;	// ���������� TimeChecker�� ������ �ð�
	bool m_bIsServiceTime;			// ���� �ð��� ���� �α׸� ����� ����

protected:	
	DTRankingInfo m_GroupRankingBlob[MAX_DT_GROUP_RANKING_COUNT];
	MDuelTournamentMatchMaker m_DTMatchMakers[MDUELTOURNAMENTTYPE_MAX];
	MDuelTournamentMatchLauncher* m_pDTMatchLauncher;

	void ClearGroupRanking();
	void LaunchMatch(MDUELTOURNAMENTTYPE nType, MDuelTournamentPickedGroup& vecUidPlayer);
public:
	MMatchDuelTournamentMgr();
	~MMatchDuelTournamentMgr();

	void Init();
	void Destory();

	bool AddPlayer(MDUELTOURNAMENTTYPE nType, MUID &uidPlayer);
	bool RemovePlayer(MDUELTOURNAMENTTYPE nType, MUID &uidPlayer);
	
	void Tick(unsigned long nTick);

	void AddGroupRanking(list<DTRankingInfo*>* pRankingList);

	void  SetTimeStamp(const char* szTimeStamp)		{ m_TimeChecker.SetTimeStamp(szTimeStamp); }
	char* GetTimeStamp()							{ return m_TimeChecker.GetTimeStamp(); }
	bool  IsSameTimeStamp(const char* szTimeStamp)  { return m_TimeChecker.IsSameTimeStamp(szTimeStamp); }

	bool GetTimeStampChanged()				{ return m_TimeChecker.GetTimeStampChanged(); }
	void SetTimeStampChanged(bool bValue)	{ m_TimeChecker.SetTimeStampChanged(bValue); }
};


// MMatchDuelTournamentMgr�� �� Ŭ������ �̿��ؼ� MatchMaker�� �̾Ƴ� �׷��� ������ ����Ų��
class MDuelTournamentMatchLauncher
{
	DWORD m_dwLimitUserWaitTime;
	DWORD m_dwAcceptableTpGap;

public:
	MDuelTournamentMatchLauncher() : m_dwLimitUserWaitTime(10000), m_dwAcceptableTpGap(10) {}

	void SetLimitUserWaitTime(DWORD n) { m_dwLimitUserWaitTime = n; }
	void SetAcceptableTpGap(DWORD n) { m_dwAcceptableTpGap = n; }

	void LaunchAvailableMatch(MDUELTOURNAMENTTYPE nType, MDuelTournamentMatchMaker& matchMaker, DWORD nCurTick);

protected:
	void LaunchMatchGroups(MDUELTOURNAMENTTYPE nType, MDuelTournamentPickedGroup& vecUidPlayer, MDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);

	// ���� MMatchServer���� ��� �����Ű�� �κ��� �����ϴ� �Լ� (�����׽�Ʈ ����)
	virtual void LaunchMatch(MDUELTOURNAMENTTYPE nType, MDuelTournamentPickedGroup* pPickedGroup, MDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);
};

#endif