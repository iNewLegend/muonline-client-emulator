#include "stdafx.h"
#include "MMatchServer.h"
#include "MMatchRule.h"
#include "MMatchStage.h"
#include "MMatchObject.h"
#include "MSharedCommandTable.h"
#include "MBlobArray.h"
#include "MMatchConfig.h"
#include "MMatchEventFactory.h"



void MMatchRule::DebugTest()
{
#ifdef _DEBUG
	SetRoundState(MMATCH_ROUNDSTATE_FINISH);
#endif
}

//// RULE //////////////////////////////////////////////////////////////////////
MMatchRule::MMatchRule(MMatchStage* pStage)	: m_pStage(pStage), m_nRoundCount(0), m_nRoundArg(0),
				/*m_pGameTypeInfo(NULL),*/ m_nRoundState(MMATCH_ROUNDSTATE_FREE)
{ 
	SetRoundStateTimer(MMatchServer::GetInstance()->GetGlobalClockCount());
}

void MMatchRule::SetRoundState(MMATCH_ROUNDSTATE nState)
{ 
	if (m_nRoundState == nState) return;

/*#ifdef _DEBUG

	{
		char szTemp[256]="";
		switch (nState)
		{
		case MMATCH_ROUNDSTATE_PREPARE:		sprintf(szTemp, "SetRoundState: PREPARE\n");	break;
		case MMATCH_ROUNDSTATE_COUNTDOWN:	sprintf(szTemp, "SetRoundState: COUNTDOWN\n");	break;
		case MMATCH_ROUNDSTATE_PLAY:		sprintf(szTemp, "SetRoundState: PLAY(%d)\n", m_nRoundCount); break;
		case MMATCH_ROUNDSTATE_FINISH:		sprintf(szTemp, "SetRoundState: FINISH\n");		break;
		case MMATCH_ROUNDSTATE_EXIT:		sprintf(szTemp, "SetRoundState: EXIT\n");		break;
		case MMATCH_ROUNDSTATE_FREE:		sprintf(szTemp, "SetRoundState: FREE\n");		break;
		case MMATCH_ROUNDSTATE_FAILED:		sprintf(szTemp, "SetRoundState: FAILED\n");		break;
		}
		MMatchServer::GetInstance()->LOG(MMatchServer::LOG_PROG, "%s", szTemp);
	}
#endif*/

	if (nState == MMATCH_ROUNDSTATE_FINISH)	OnRoundEnd();
	else if (nState == MMATCH_ROUNDSTATE_PLAY) OnRoundBegin();

	m_nRoundState = nState;
	SetRoundStateTimer(MMatchServer::GetInstance()->GetGlobalClockCount());

	// ���尡 ���� ���۵Ǹ� ���� �ʱ�ȭ
	if (nState == MMATCH_ROUNDSTATE_COUNTDOWN)
	{
		InitRound();
	}

	// �÷��̾�鿡�� ���� ���� �ٲ���ٰ� �˷���
	MMatchServer::GetInstance()->ResponseRoundState(GetStage()->GetUID());

}

void MMatchRule::InitRound()
{
	if (m_pStage == NULL) 
	{
		_ASSERT(0);
		return;
	}

	// ���� ���ʽ� �ʱ�ȭ
	m_pStage->OnInitRound();

	// ��������� �ʱ�ȭ
	m_pStage->m_WorldItemManager.OnRoundBegin();
	m_pStage->m_ActiveTrapManager.Clear();

	// �ð����� �ܷ� ���� �ʱ�ȭ
	SetLastTimeLimitAnnounce(INT_MAX);
}

void MMatchRule::OnBegin()
{
}
void MMatchRule::OnEnd()
{
}
void MMatchRule::OnRoundBegin()
{
//	OutputDebugString("OnRoundBegin \n");
}
void MMatchRule::OnRoundEnd()
{
//	OutputDebugString("OnRoundEnd \n");
}

void MMatchRule::OnRoundTimeOut()
{
//	OutputDebugString("OnRoundTimeOut \n");
}

bool MMatchRule::OnCheckBattleTimeOut(unsigned int tmTimeSpend)	// true �����ϸ� Ÿ�Ӿƿ�
{
	int nLimitTime = GetStage()->GetStageSetting()->GetLimitTime() * 60 * 1000;
	if (nLimitTime <= STAGESETTING_LIMITTIME_UNLIMITED) return false;

//	int nLimitTime = 1 * 60 * 1000;
	int nTimeRemain = nLimitTime - tmTimeSpend;

	if ((unsigned int)nLimitTime < tmTimeSpend)
		return true;
	if ((nLimitTime == 0) && (tmTimeSpend > 24*60*60*1000))
		return true;

	char szMsg[256];

	if ((GetLastTimeLimitAnnounce() > 10) && (nTimeRemain < 10*1000))  {
		MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
		pCmd->AddParameter(new MCmdParamUInt(0));
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_TIME_10REMAINING);
		pCmd->AddParameter(new MCmdParamStr( szMsg));

		MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
		SetLastTimeLimitAnnounce(10);
		return false;
	} else if ((GetLastTimeLimitAnnounce() > 30) && (nTimeRemain < 30*1000)) {
		MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
		pCmd->AddParameter(new MCmdParamUInt(0));
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_TIME_30REMAINING);
		pCmd->AddParameter(new MCmdParamStr( szMsg));

		MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
		SetLastTimeLimitAnnounce(30);
		return false;
	} else if ((GetLastTimeLimitAnnounce() > 60) && (nTimeRemain < 60*1000)) {
		MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
		pCmd->AddParameter(new MCmdParamUInt(0));
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_TIME_60REMAINING);
		pCmd->AddParameter(new MCmdParamStr( szMsg));

		MMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
		SetLastTimeLimitAnnounce(60);
		return false;
	}

	return false;
}

bool MMatchRule::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
	case MMATCH_ROUNDSTATE_PREPARE:
		{
			if (GetStage()->CheckBattleEntry() == true) 
			{
				if (OnCheckEnableBattleCondition())
				{
					// ���� ����
					SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
				}
				else
				{
					SetRoundState(MMATCH_ROUNDSTATE_FREE);
				}
				return true;
			} 
			else
			{
				// Drop Timedout Player
				if (nClock - GetRoundStateTimer() > 30*1000)
				{
					// ���� ������ �ʾҴ�.
				} 

				
				return true;
			}
		}
		break;
	case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (nClock - GetRoundStateTimer() > 2*1000) {
				SetRoundState(MMATCH_ROUNDSTATE_PLAY);
				return true;
			} else {
				// Countdown proceeding now, Do Nothing..
				return true;
			}

		}
		break;
	case MMATCH_ROUNDSTATE_PLAY:
		{
			// �������� ���� ��Ȳ�̸� Free���·� ��ȯ
			if (!OnCheckEnableBattleCondition())
			{
				SetRoundState(MMATCH_ROUNDSTATE_FREE);
				
				// �� SetRoundState() �ȿ��� RoundStateTimer�� ���ŵȴ�
				// ���� nClock�� ��������� �ϸ�, �׷��� ������ �Ʒ��� nClock - GetRoundStateTimer()���� ������ ���� �� �ְ� �Ǿ
				// unsigned ĳ�������� ��û ū ���� �ǰ�, OnCheckBattleTimeOut()�� �׻� true�̰� �ȴ�.
				nClock = MMatchServer::GetInstance()->GetGlobalClockCount();
			}

			if (OnCheckRoundFinish())
			{
				// �������� ����Ʈ ����.
				// OnRoundTimeOut();
				SetRoundState( MMATCH_ROUNDSTATE_FINISH );
			} 
			else if (OnCheckBattleTimeOut(nClock - GetRoundStateTimer())) 
			{
				// Make Draw Game...

				OnRoundTimeOut();
				SetRoundState(MMATCH_ROUNDSTATE_FINISH);
			} 
			else 
			{
				// They still playing the game..
			}

			CheckOnGameEvent();
			
			return true;
		}
		break;
	case MMATCH_ROUNDSTATE_FINISH:
		{
			if (nClock - GetRoundStateTimer() > 3*1000) {	// 3��
				if (RoundCount() == true)
				{
					SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
				}
				else
					SetRoundState(MMATCH_ROUNDSTATE_EXIT);

				return true;
			} else {
				// Spend time, Do nothing
				return true;
			}
		}
		break;
	case MMATCH_ROUNDSTATE_EXIT:
		{
			// End game
			return false;
		}
		break;
	case MMATCH_ROUNDSTATE_FREE:
		{
			if (OnCheckEnableBattleCondition())
			{
				// ���� ����
				SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
			}

			return true;
		}
		break;
	default:
		{
			_ASSERT(0);
		}
	}

	return false;	// false return make RoundExit
}

bool MMatchRule::Run()
{
	const bool bResult = OnRun();
	// CheckOnGameEvent();
	RunOnGameEvent();
	return bResult;
}

void MMatchRule::Begin()
{
	m_nRoundCount = 0;
	SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
	SetRoundStateTimer( MMatchServer::GetInstance()->GetGlobalClockCount() );
    
	InitOnBeginEventManager();
	InitOnGameEventManager();
	InitOnEndEventManager();
	
	OnBegin();

	CheckOnBeginEvent();
	RunOnBeginEvent();
}

void MMatchRule::End()
{
	OnEnd();

	CheckOnEndEvent();
	RunOnEndEvent();
}

void MMatchRule::CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	*poutAttackerExp = nSrcExp;
	*poutTeamExp = 0;
}


void MMatchRule::InitOnBeginEventManager()
{
	if( GetGameType() != GetOnBeginEventManager().GetLastSetGameType() )
	{
		EventPtrVec EvnPtrVec;
		if( !MMatchEventFactoryManager::GetInstance().GetEventList(GetGameType(), ET_BEGIN, EvnPtrVec) )
		{
			ASSERT( 0 && "�̺�Ʈ ����Ʈ ���� ����.\n" );
			mlog( "MMatchRule::InitOnBeginEventManager - ����Ʈ ���� ����.\n" );
			MMatchEventManager::ClearEventPtrVec( EvnPtrVec );
			return;
		}
		GetOnBeginEventManager().ChangeEventList( EvnPtrVec );
	}
}


void MMatchRule::InitOnGameEventManager()
{
	if( GetGameType() != GetOnGameEventManager().GetLastSetGameType() )
	{
		EventPtrVec EvnPtrVec;
		if( !MMatchEventFactoryManager::GetInstance().GetEventList(GetGameType(), ET_ONGAME, EvnPtrVec) )
		{
			ASSERT( 0 && "�̺�Ʈ ����Ʈ ���� ����.\n" );
			mlog( "MMatchRule::InitOnGameEventManager - ����Ʈ ���� ����.\n" );
			MMatchEventManager::ClearEventPtrVec( EvnPtrVec );
			return;
		}
		GetOnGameEventManager().ChangeEventList( EvnPtrVec );
	}
}


void MMatchRule::InitOnEndEventManager()
{
	if( GetGameType() != GetOnGameEventManager().GetLastSetGameType() )
	{
		EventPtrVec EvnPtrVec;
		if( !MMatchEventFactoryManager::GetInstance().GetEventList(GetGameType(), ET_END, EvnPtrVec) )
		{
			ASSERT( 0 && "�̺�Ʈ ����Ʈ ���� ����.\n" );
			mlog( "MMatchRule::InitOnEndEventManager - ����Ʈ ���� ����.\n" );
			MMatchEventManager::ClearEventPtrVec( EvnPtrVec );
			return;
		}
		GetOnEndEventManager().ChangeEventList( EvnPtrVec );
	}
	
}


void MMatchRule::CheckOnBeginEvent()
{
	if( !GetOnBeginEventManager().Empty() )
	{
		MMatchObject* pObj;
		const DWORD dwClock = MMatchServer::GetInstance()->GetGlobalClockCount();
		GetOnBeginEventManager().StartNewEvent();
		for (MUIDRefCache::iterator i=GetStage()->GetObjBegin(); i!=GetStage()->GetObjEnd(); i++) 
		{
			pObj = (MMatchObject*)(*i).second;
			if( MMP_STAGE == pObj->GetPlace() )
				GetOnBeginEventManager().CheckEventObj( pObj, dwClock );
		}

		GetOnBeginEventManager().SetLastCheckTime( dwClock );
	}
}


void MMatchRule::CheckOnGameEvent()
{
	if( MMATCH_ROUNDSTATE_PLAY == GetRoundState() ) {
		// ���ӻ� �ƹ��� ������ exit
		int nInGamePlayer = 0;

		MMatchObject*	pObj;
		const DWORD		dwClock = MMatchServer::GetInstance()->GetGlobalClockCount();

		for (MUIDRefCache::iterator i=GetStage()->GetObjBegin(); i!=GetStage()->GetObjEnd(); i++) 
		{
			pObj = (MMatchObject*)(*i).second;
			if( (pObj->GetEnterBattle() == true) || pObj->IsLaunchedGame() )
			{
				GetOnGameEventManager().CheckEventObj( pObj, dwClock );
				++nInGamePlayer;
			}
		}

		GetOnGameEventManager().SetLastCheckTime( dwClock );

		if (nInGamePlayer == 0) SetRoundState( MMATCH_ROUNDSTATE_EXIT );
	}	
}


void MMatchRule::CheckOnEndEvent()
{
	if( !GetOnEndEventManager().Empty() )
	{
		MMatchObject*	pObj;
		const DWORD		dwClock = MMatchServer::GetInstance()->GetGlobalClockCount();

		GetOnEndEventManager().StartNewEvent();
		for (MUIDRefCache::iterator i=GetStage()->GetObjBegin(); i!=GetStage()->GetObjEnd(); i++) 
		{
			pObj = (MMatchObject*)(*i).second;
			if( MMP_BATTLE == pObj->GetPlace() )
				GetOnEndEventManager().CheckEventObj( pObj, dwClock );
		}

		GetOnEndEventManager().SetLastCheckTime( dwClock );
	}
}


void MMatchRule::RunOnBeginEvent()
{
	GetOnBeginEventManager().Run();	
}


void MMatchRule::RunOnGameEvent()
{
	GetOnGameEventManager().Run();
	GetOnGameEventManager().StartNewEvent();
}


void MMatchRule::RunOnEndEvent()
{
	GetOnEndEventManager().Run();
}
//////////////////////////////////////////////////////////////////////////////////