#ifndef _MVOTEMGR_H
#define _MVOTEMGR_H

//#pragma once

#include "MUID.h"
using namespace std;
#include <list>
#include <map>


enum MVOTE {
	MVOTE_GIVEUP,
	MVOTE_YES,
	MVOTE_NO
};


class MVoter {
protected:
	MUID	m_uidVoter;
public:
	MVoter(MUID uid)	{ m_uidVoter = uid; }
	MUID GetID()		{ return m_uidVoter; }
};
class MVoterMap : public map<MUID, MVoter*> {};


class MVoteDiscuss {
protected:
	MUID			m_uidStage;			// ��������UID
	string			m_strDiscuss;		// �Ȱ�
	unsigned long	m_nBeginTime;		// ���ǵ� �ð�
	list<MUID>		m_YesVoterList;		// ������
	list<MUID>		m_NoVoterList;		// �ݴ���

public:
	MVoteDiscuss(const MUID& uidStage);
	virtual ~MVoteDiscuss();

	MUID GetStageUID()	{ return m_uidStage; }
	const char* GetDiscussName()	{ return m_strDiscuss.c_str(); }
	unsigned long GetBeginTime()	{ return m_nBeginTime; }
	size_t GetYesVoterCount()	{ return m_YesVoterList.size(); }
	size_t GetNoVoterCount()	{ return m_NoVoterList.size(); }

	bool CheckVoter(const MUID& uid);	// ��ǥ������ �˻�
	void Vote(const MUID& uid, MVOTE nVote);
public:
	virtual bool OnJudge(bool bJudge) = 0;
	virtual string GetImplTarget() = 0;
};


class MVoteMgr {
public:
	enum VOTEMGR_ERROR {
		VOTEMGR_ERROR_OK,
		VOTEMGR_ERROR_UNKNOWN,
		VOTEMGR_ERROR_VOTE_NODISCUSS,
		VOTEMGR_ERROR_VOTE_INPROGRESS,
		VOTEMGR_ERROR_VOTE_ALREADY_VOTED
	};

protected:
	MVoterMap			m_VoterMap;
	MVoteDiscuss*		m_pDiscuss;
	VOTEMGR_ERROR		m_nLastDiscussError;

protected:
	bool CheckDiscuss();
	void FinishDiscuss(bool bJudge);

public:
	MVoteMgr();
	virtual ~MVoteMgr();

	MVoter* FindVoter(const MUID& uid);
	void AddVoter(const MUID& uid);
	void RemoveVoter(const MUID& uid);	

	MVoteDiscuss* GetDiscuss()		{ return m_pDiscuss; }
	VOTEMGR_ERROR GetLastError()	{ return m_nLastDiscussError; }
	void SetLastError(VOTEMGR_ERROR nError)	{ m_nLastDiscussError = nError; }

	bool CallVote(MVoteDiscuss* pDiscuss);
	bool Vote(const MUID& uid, MVOTE nVote);
	void Tick(unsigned long nClock);

	bool IsGoingOnVote() { return (0 != m_pDiscuss); }

	void StopVote( const MUID& uidUser );
};

/*
void Client()
{
	MVoteMgr	m_VoteMgr;

	m_VoteMgr.AddVoter( MUID(0,1) );
	m_VoteMgr.AddVoter( MUID(0,2) );
	m_VoteMgr.AddVoter( MUID(0,3) );

	m_VoteMgr.CallVote(new MVoteDiscussKick());

	m_VoteMgr.Vote(MUID(0,1), MVOTE_YES);
	m_VoteMgr.Vote(MUID(0,2), MVOTE_YES);
	m_VoteMgr.Vote(MUID(0,3), MVOTE_NO);
}
*/

#endif