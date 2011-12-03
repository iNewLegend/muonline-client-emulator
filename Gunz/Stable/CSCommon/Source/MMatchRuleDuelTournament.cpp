#include "stdafx.h"
#include "MMatchRuleDuelTournament.h"

#include "MBlobArray.h"
#include "MMatchServer.h"
#include "MMatchTransDataType.h"

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////
// 1. ��ġ�� ���� ����
//   - ��� ���� �����̸�, ��ʸ�Ʈ ������ ��ġ�� �����Ѵ�.
//   - �ذ��, ����� ���� ��ġ��� �ϸ�, 3�� 2������ ���� ��/�д� ����� �Ѵ�.
// 2. OnRun ������ ���� ����
//   - OnRun�� SetRoundState�� SetDTRoundState�� �ٽ� �����ߴ�. ������ ������ 
//    ������ ���� ������, MMatchRule�� ����� ���� ������ ����� �����̴�.
//     * ���� ���۽�, ����ǥ�� �����ִ� �ð� Ȯ��
//     * ��ġ �����, �÷��̾��� HP, AP ������ �ʿ��ѵ�, �� ������ �޴� ���� �ð� Ȯ��
//     * ���� �ڻ�
////////////////////////////////////////////////////////////////////////////////////////

MMatchRuleDuelTournament::MMatchRuleDuelTournament(MMatchStage* pStage) : MMatchRule(pStage)
{
	m_CurrentMatchInfo.uidPlayer1 = MUID(0, 0);
	m_CurrentMatchInfo.uidPlayer2 = MUID(0, 0);
	m_CurrentMatchInfo.nMatchNumber = 0;
	m_CurrentMatchInfo.nNextMatchNumber = 0;
	m_CurrentMatchInfo.nRoundState = MDUELTOURNAMENTROUNDSTATE_MAX;

	InitMatchRecord();
	InitRoundRecord();

	SetRoundFinish(true);
	SetCorrectFinish(true);
}

void MMatchRuleDuelTournament::OnBegin()
{
	MMatchStage* pStage = GetStage();

	if (pStage != NULL) {
		////////////////////////////////////////////////////////////////////////////////////////
		// �ʱ� ������ �⺻ ������ �������ش�.
		// ������ Ÿ��, ���� ��ȣ(DB���� LogID), ���� Ÿ�ӽ�����(DB���� TimeStamp)
		m_GameInfo.nType = pStage->GetDuelTournamentType();
		m_GameInfo.nGameNumber = pStage->GetDuelTournamentMatchNumber();
		strcpy(m_GameInfo.szTimeStamp, pStage->GetDuelTournamentMatchTimeStamp());

		m_DuelTournamentMatchMap = pStage->GetDuelTournamentMatchMap();
		////////////////////////////////////////////////////////////////////////////////////////
		// Stage�� ������ ��� ����� �ϴ� ������ ��ܿ� ����ִ´�.
		m_DTPlayerMap.clear();
		for(MUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {			
			MMatchObject *pObj = MMatchServer::GetInstance()->GetObject((*itor).first);
			if( pObj != NULL ) {
				MDuelTournamentPlayerInfo *pInfo = new MDuelTournamentPlayerInfo;

				memset(pInfo, 0, sizeof(MDuelTournamentPlayerInfo));
				pInfo->nCID = pObj->GetCharInfo()->m_nCID;
				pInfo->bInMatch = false;

				m_DTPlayerMap.InsertPlayerInfo((*itor).first, pInfo);				
			}
		}

		DTLog(DTLOG_DEBUG, "�١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١�");
		DTLog(DTLOG_DEBUG, "::OnBegin() - GameNumber : %d, TimeStamp : %s", m_GameInfo.nGameNumber, m_GameInfo.szTimeStamp);
		DTLog(DTLOG_DEBUG, "::OnBegin() - DuelTournament Game Begin");
	} else {
		DTLog(DTLOG_RELEASE, "::OnBegin() - There is no stage");
	}	

	LogInfo();
}

void MMatchRuleDuelTournament::OnEnd()
{
	////////////////////////////////////////////////////////////////////////////////////////
	// ���������� ���� ���۽�, Insert�ߴ� DB ���ڵ带 è�ǿ°� �Բ� ������Ʈ�Ѵ�(DB Schema ����)
	MMatchServer::GetInstance()->OnAsyncRequest_UpdateDuelTournamentGameLog(m_GameInfo.szTimeStamp, m_GameInfo.nGameNumber, m_RoundRecord.uidWinnerPlayer);
	
	DTLog(DTLOG_DEBUG, "::OnEnd() - DuelTournament Game Finish");
	DTLog(DTLOG_DEBUG, "::OnEnd() - Champion = (%d%d)", m_MatchRecord.uidWinner.High, m_MatchRecord.uidWinner.Low);
	DTLog(DTLOG_DEBUG, "�١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١ڡ١�");
}

void MMatchRuleDuelTournament::OnPreCountDown()
{
	if( m_MatchRecord.bMatchFinish ){
		InitMatchRecord();

		SpawnPlayers();
		PrepareDuelTournamentMatchRecord();
	}

	SendDuelTournamentNextGamePlayerInfo();

	DTLog(DTLOG_DEBUG, "::OnPreCountDown() - Match(%d), Round(%dth) Begin", m_CurrentMatchInfo.nMatchNumber, m_MatchRecord.nRoundCount);
}

void MMatchRuleDuelTournament::OnRoundBegin()
{		
	if( m_MatchRecord.bMatchFinish ){
		m_MatchRecord.nStartTime = MMatchServer::GetInstance()->GetGlobalClockCount();
	} 

	InitRoundRecord();
	SetRoundFinish(false);
	SetRoundTimeOut(false);

	SendDuelTournamentGameInfo(true);

	DTLog(DTLOG_DEBUG, "::OnRoundBegin() - Match(%d), Round(%dth) Begin", m_CurrentMatchInfo.nMatchNumber, m_MatchRecord.nRoundCount);

	m_MatchRecord.nRoundCount++;	
}

void MMatchRuleDuelTournament::OnMatchFinish(bool bIsPlayer1Win)
{
	////////////////////////////////////////////////////////////////////////////////////////
	// ��ġ�� ����� ���
	// - ���� �� ��ġ�� ����� ���� �� ����
	// - ���� ��ġ �غ�
	m_MatchRecord.bIsPlayer1Win = bIsPlayer1Win;
	m_MatchRecord.bIsPlayer2Win = !bIsPlayer1Win;

	if( bIsPlayer1Win ){
		m_MatchRecord.uidWinner = m_CurrentMatchInfo.uidPlayer1;
		m_MatchRecord.uidLoser  = m_CurrentMatchInfo.uidPlayer2;
	} else {
		m_MatchRecord.uidWinner = m_CurrentMatchInfo.uidPlayer2;
		m_MatchRecord.uidLoser  = m_CurrentMatchInfo.uidPlayer1;
	}

	m_MatchRecord.bMatchFinish = true;
	m_MatchRecord.nFinishTime = MMatchServer::GetInstance()->GetGlobalClockCount();

	RecordGameResult();
	MakeNextMatch();

	SendDuelTournamentRoundResultInfo(true, false);
	SendDuelTournamentMatchResultInfo();
	SetCorrectFinish(true);
}

void MMatchRuleDuelTournament::OnMatchNotFinish()
{
	////////////////////////////////////////////////////////////////////////////////////////
	// ��ġ�� ������� ���� ���
	// - ������ ����� ����
	m_MatchRecord.bMatchFinish = false;

	if( m_RoundRecord.uidWinnerPlayer == m_RoundRecord.uidLoserPlayer ) { SendDuelTournamentRoundResultInfo(false, true);} 
	else																{ SendDuelTournamentRoundResultInfo(false, false);}

	SetCorrectFinish(false);
}

bool MMatchRuleDuelTournament::OnRoundEnd_PlayerOut()
{	
	if( m_CurrentMatchInfo.uidPlayer1 == MUID(0, 0) && m_CurrentMatchInfo.uidPlayer2 == MUID(0, 0)) {
		m_RoundRecord.bPlayer2Win = false;
		m_RoundRecord.bPlayer1Win = false;

		// MUID�� (0, 0)�� ���̴�.
		m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer1;
		m_RoundRecord.uidLoserPlayer  = m_CurrentMatchInfo.uidPlayer2;

		m_MatchRecord.bIsPlayer2Win = false;
		m_MatchRecord.bIsPlayer1Win = false;
		m_MatchRecord.uidWinner = m_CurrentMatchInfo.uidPlayer2;
		m_MatchRecord.uidLoser  = m_CurrentMatchInfo.uidPlayer1;

		m_MatchRecord.bMatchFinish  = true;
		m_MatchRecord.nFinishTime = MMatchServer::GetInstance()->GetGlobalClockCount();

		MakeNextMatch();

		SendDuelTournamentRoundResultInfo(true, true);
		SendDuelTournamentMatchResultInfo();
		SetCorrectFinish(true);

		DTLog(DTLOG_DEBUG, "::OnRoundEnd() - Player1, Player2 Out! Go Next Match!");
	} 
	else if( m_CurrentMatchInfo.uidPlayer1 == MUID(0, 0)) { OnMatchFinish(false);} 
	else if( m_CurrentMatchInfo.uidPlayer2 == MUID(0, 0)) { OnMatchFinish(true);} 
	else												  { return false; }

	return true;
}

void MMatchRuleDuelTournament::OnRoundEnd_TimeOut()
{
	bool bPlayer1Win = true;

	if( m_RoundRecord.fPlayer1DamagedPoint == m_RoundRecord.fPlayer2DamagedPoint ) {
		if( m_RoundRecord.fPlayer1HealthPoint == m_RoundRecord.fPlayer2HealthPoint ) {
			if( m_RoundRecord.fPlayer1AmorPoint > m_RoundRecord.fPlayer2AmorPoint ) { bPlayer1Win = true;} 
			else																	{ bPlayer1Win = false;}
		} 
		else if( m_RoundRecord.fPlayer1HealthPoint > m_RoundRecord.fPlayer2HealthPoint )	{ bPlayer1Win = true;} 
		else																				{ bPlayer1Win = false;}
	} 
	else if( m_RoundRecord.fPlayer1DamagedPoint < m_RoundRecord.fPlayer2DamagedPoint )	{ bPlayer1Win = true; } 
	else																				{ bPlayer1Win = false;} 

	if( bPlayer1Win ){
		m_RoundRecord.bPlayer1Win = true;
		m_RoundRecord.bPlayer2Win = false;
		m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer1;
		m_RoundRecord.uidLoserPlayer = m_CurrentMatchInfo.uidPlayer2;
	} else {
		m_RoundRecord.bPlayer1Win = false;
		m_RoundRecord.bPlayer2Win = true;
		m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer2;
		m_RoundRecord.uidLoserPlayer = m_CurrentMatchInfo.uidPlayer1;
	}

	DTLog(DTLOG_DEBUG, "MMatchRuleDuelTournament::OnRoundEnd - Player1UID(%d%d) - DamagedPoint(%d), HealthPoint(%d), AmorPoint(%d)",
		m_CurrentMatchInfo.uidPlayer1.High, m_CurrentMatchInfo.uidPlayer1.Low, m_RoundRecord.fPlayer1DamagedPoint, m_RoundRecord.fPlayer1HealthPoint, m_RoundRecord.fPlayer1AmorPoint);

	DTLog(DTLOG_DEBUG, "MMatchRuleDuelTournament::OnRoundEnd - Player2UID(%d%d) - DamagedPoint(%d), HealthPoint(%d), AmorPoint(%d)",
		m_CurrentMatchInfo.uidPlayer2.High, m_CurrentMatchInfo.uidPlayer2.Low, m_RoundRecord.fPlayer2DamagedPoint, m_RoundRecord.fPlayer2HealthPoint, m_RoundRecord.fPlayer2AmorPoint);
}

void MMatchRuleDuelTournament::OnRoundEnd()
{
	// �÷��̾� ��Ż�� ���� ó��	
	if( OnRoundEnd_PlayerOut() == true ) {
		DTLog(DTLOG_DEBUG, "::OnRoundEnd() - Match(%d), Round(%d) were Finished - PlayerOut", m_CurrentMatchInfo.nMatchNumber, m_MatchRecord.nRoundCount);
		return;
	}

	// �ð� ���ѿ� �ɷ��� ���, ��ó�� �۾�
	if( IsRoundTimeOut() == true ) {OnRoundEnd_TimeOut();}

	////////////////////////////////////////////////////////////////////////////////////////
	// ������ ���� ����� ����, �¸��� ó���� �Ѵ�.
	// - ���� �ڻ��� ���, �Ѵ� �¸� ī��Ʈ�� �ϳ��� ������Ų��.
	// - ���� �ڽ��� �ƴ� ���, �������� �¸��� ó���� �Ѵ�.
	if( m_RoundRecord.bPlayer1Win && m_RoundRecord.bPlayer2Win ){ //< ���� �ڻ��� ���
		m_MatchRecord.nPlayer1WinCount++;
		m_MatchRecord.nPlayer2WinCount++;
	} else {
		if( m_RoundRecord.bPlayer1Win ){
			m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer1;
			m_RoundRecord.uidLoserPlayer  = m_CurrentMatchInfo.uidPlayer2;

			m_MatchRecord.nPlayer1WinCount++;
		}
		else if( m_RoundRecord.bPlayer2Win ) {
			m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer2;
			m_RoundRecord.uidLoserPlayer  = m_CurrentMatchInfo.uidPlayer1;

			m_MatchRecord.nPlayer2WinCount++;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// �¸��� ó�� ��, ��ġ�� ����Ǿ����� �Ǵ��غ���.
	// - �ذ��, ����� ��� 2�������̰�, �������� 1�������̴�.
	// - ��ġ�� ����Ǿ����� OnMatchFinish��, ���� ���尡 �� ����Ǿ� �ϸ� OnMatchNotFinish�� ȣ��
	if( m_CurrentMatchInfo.nRoundState == MDUELTOURNAMENTROUNDSTATE_SEMIFINAL || m_CurrentMatchInfo.nRoundState == MDUELTOURNAMENTROUNDSTATE_FINAL ) {
		if( m_MatchRecord.nPlayer1WinCount > m_MatchRecord.nPlayer2WinCount && m_MatchRecord.nPlayer1WinCount > 1)		{ OnMatchFinish(true);} 
		else if( m_MatchRecord.nPlayer1WinCount < m_MatchRecord.nPlayer2WinCount && m_MatchRecord.nPlayer2WinCount > 1) { OnMatchFinish(false);} 
		else																											{ OnMatchNotFinish();}
	} else {
		if( m_MatchRecord.nPlayer1WinCount > m_MatchRecord.nPlayer2WinCount )		{ OnMatchFinish(true);} 
		else if( m_MatchRecord.nPlayer1WinCount < m_MatchRecord.nPlayer2WinCount )	{ OnMatchFinish(false);} 
		else																		{ OnMatchNotFinish();}
	}

	if( m_MatchRecord.bMatchFinish )	{DTLog(DTLOG_DEBUG, "::OnRoundEnd() - Match(%d), Round(%d) were Finished", m_CurrentMatchInfo.nMatchNumber, m_MatchRecord.nRoundCount);}
	else								{DTLog(DTLOG_DEBUG, "::OnRoundEnd() - Round(%d) were Finished, but Match(%d) is not finish", m_MatchRecord.nRoundCount, m_CurrentMatchInfo.nMatchNumber);}
}

void MMatchRuleDuelTournament::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{    
	if( m_CurrentMatchInfo.uidPlayer1 == uidAttacker && m_CurrentMatchInfo.uidPlayer2 == uidVictim) {
		m_RoundRecord.bPlayer1Win = true;
		m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer1;
	} else if (m_CurrentMatchInfo.uidPlayer1 == uidVictim && m_CurrentMatchInfo.uidPlayer2 == uidAttacker) {
		m_RoundRecord.bPlayer2Win = true;
		m_RoundRecord.uidWinnerPlayer = m_CurrentMatchInfo.uidPlayer2;
	} else if( uidVictim == uidAttacker ) {		///< �ڻ��� ���...	�ҽ� ����� ���� ����!	
		if( m_CurrentMatchInfo.uidPlayer2 == uidAttacker ){					// Player2�� �ڻ�
			m_RoundRecord.bPlayer1Win = true;
			m_RoundRecord.uidLoserPlayer = m_CurrentMatchInfo.uidPlayer2;
		} else if( m_CurrentMatchInfo.uidPlayer1 == uidAttacker ){			// Player1�� �ڻ�
			m_RoundRecord.bPlayer2Win = true;
			m_RoundRecord.uidLoserPlayer = m_CurrentMatchInfo.uidPlayer1;
		}
	} else{
		//DTLog(DTLOG_RELEASE, "::OnGameKill - Victim(%d%d), Attacker(%d%d)", uidVictim.High, uidVictim.Low, uidAttacker.High, uidAttacker.Low);
		return;
	}

	SetRoundFinish(true);
}

void MMatchRuleDuelTournament::OnLeaveBattle(MUID& uidChar)
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// ���� ����, �÷��̾ ��Ż���� ���..
	// - ���� ���� �÷��̾�� ���带 �����Ŵ
	// - ���� ���� �÷��̾ �ƴϸ� �׳� ��Ż��Ŵ
	// - ��Ż��, �̹� ��ʸ�Ʈ���� ���� ���� �������̸� �׳� ��Ż
	//           ��ʸ�Ʈ�� ��� �����ؾߵǴ� �÷��̾�� ����Ʈ ���� �� ��Ż
	if (uidChar == m_CurrentMatchInfo.uidPlayer1) {
		m_CurrentMatchInfo.uidPlayer1 = MUID(0, 0);
		SetRoundFinish(true);

		m_DTPlayerMap.SetLoser(m_CurrentMatchInfo.uidPlayer1);		// �ϴ� �й��� ���!
	} else if (uidChar == m_CurrentMatchInfo.uidPlayer2) {
		m_CurrentMatchInfo.uidPlayer2 = MUID(0, 0);
		SetRoundFinish(true);

		m_DTPlayerMap.SetLoser(m_CurrentMatchInfo.uidPlayer2);		// �ϴ� �й��� ���!
	} else {
		m_DTPlayerMap.SetOutStage(uidChar);
		SendDuelTournamentGameInfo(false);
	}

	// �̹� �� �÷��̾�����, ��� �����ؾߵǴ� �÷��̾����� ����
	// ��� �����ؾߵǴ� �÷��̾�(�й��� ����� �ȵ� �÷��̾�)�� ���, ���� �� ��Ż
	if( m_DTPlayerMap.IsLoser(uidChar) == false ) {
		m_DTPlayerMap.SetLoser(uidChar);
		UpdateDuelTournamentPlayerInfo(uidChar, MDUELTOURNAMENTROUNDSTATE_MAX, false, DUELTOURNAMENT_PLAYER_LEAVE_PENALTYPOINT, true);
	}
}

void MMatchRuleDuelTournament::InitMatchRecord()
{
	m_MatchRecord.uidWinner = MUID(0, 0);	m_MatchRecord.nTP1 = -1;	m_MatchRecord.nCID1 = -1;	
	m_MatchRecord.uidLoser = MUID(0 ,0);	m_MatchRecord.nTP2 = -1;	m_MatchRecord.nCID2 = -1;	
	
	m_MatchRecord.nGainTP = 0;	m_MatchRecord.nLoseTP = 0;

	m_MatchRecord.nPlayer1WinCount = 0;		m_MatchRecord.bIsPlayer2Win = false;	
	m_MatchRecord.nPlayer2WinCount = 0;		m_MatchRecord.bIsPlayer2Win = false;

	m_MatchRecord.bMatchFinish = true;
	m_MatchRecord.nRoundCount = 1;
	m_MatchRecord.nStartTime = 0;
	m_MatchRecord.nFinishTime = 0;
}

void MMatchRuleDuelTournament::InitRoundRecord()
{
	m_RoundRecord.uidWinnerPlayer = MUID(0, 0);
	m_RoundRecord.uidLoserPlayer = MUID(0, 0);

	m_RoundRecord.fPlayer1DamagedPoint  = 1000;	
	m_RoundRecord.fPlayer1HealthPoint	= 0;	
	m_RoundRecord.fPlayer1AmorPoint		= 0;

	m_RoundRecord.fPlayer2DamagedPoint  = 1000;	
	m_RoundRecord.fPlayer2HealthPoint	= 0;	
	m_RoundRecord.fPlayer2AmorPoint		= 0;

	m_RoundRecord.bPlayer1Win = false;
	m_RoundRecord.bPlayer2Win = false;

	m_RoundRecord.bIsTimeOut = false;

	m_RoundRecord.nFinishTime = 0;
}

void MMatchRuleDuelTournament::MakeNextMatch()
{
	if( m_CurrentMatchInfo.nNextMatchNumber > 0 ){
		map<int, MMatchDuelTournamentMatch*>::iterator iter;
		iter = m_DuelTournamentMatchMap->find(m_CurrentMatchInfo.nNextMatchNumber);

		if( iter == m_DuelTournamentMatchMap->end() ){
			DTLog(DTLOG_RELEASE, "::MakeNextMatch Error - Wrong Order");
			return;
		}

		MMatchDuelTournamentMatch* pMatch = iter->second;

		if( m_CurrentMatchInfo.nMatchNumber % 2 == 0 )	pMatch->uidPlayer1 = m_MatchRecord.uidWinner;
		else											pMatch->uidPlayer2 = m_MatchRecord.uidWinner;

		if( m_MatchRecord.uidWinner	!= MUID(0, 0))	m_DTPlayerMap.LeaveMatch(m_MatchRecord.uidWinner);
		if( m_MatchRecord.uidLoser	!= MUID(0, 0))	m_DTPlayerMap.LeaveMatch(m_MatchRecord.uidLoser);
	}
}

void MMatchRuleDuelTournament::SpawnPlayers()
{
	if( m_DuelTournamentMatchMap->empty() == false ) {
		map<int, MMatchDuelTournamentMatch*>::iterator iter1 = m_DuelTournamentMatchMap->find(m_CurrentMatchInfo.nMatchNumber + 1);
		if( iter1 == m_DuelTournamentMatchMap->end() ) {
			DTLog(DTLOG_RELEASE, "::SpawnPlayers() - Can't Find MatchInfo(%d)", m_CurrentMatchInfo.nMatchNumber + 1);
			return;
		}

		memcpy(&m_CurrentMatchInfo, (MMatchDuelTournamentMatch*)iter1->second, sizeof(MMatchDuelTournamentMatch));

		if( m_DTPlayerMap.EnterMatch(m_CurrentMatchInfo.uidPlayer1) == false ) {
			m_CurrentMatchInfo.uidPlayer1 = MUID(0, 0);
			DTLog(DTLOG_DEBUG, "::SpawnPlayers() - uidPlayer1 is not exist");
		}

		if( m_DTPlayerMap.EnterMatch(m_CurrentMatchInfo.uidPlayer2) == false ) {
			m_CurrentMatchInfo.uidPlayer2 = MUID(0, 0);
			DTLog(DTLOG_DEBUG, "::SpawnPlayers() - uidPlayer2 is not exist");
		}

		// ��װ� �����ٴ°�... ��Ե� �غ��ô�..-_��
		if( m_DTPlayerMap.IsOutUser(m_CurrentMatchInfo.uidPlayer1) == true ) {			
			DTLog(DTLOG_DEBUG, "::SpawnPlayers() - uidPlayer1(%d%d) is out of game"
				, m_CurrentMatchInfo.uidPlayer1.High, m_CurrentMatchInfo.uidPlayer1.Low);
			m_CurrentMatchInfo.uidPlayer1 = MUID(0, 0);
		} 

		if( m_DTPlayerMap.IsOutUser(m_CurrentMatchInfo.uidPlayer2) == true ) {			
			DTLog(DTLOG_DEBUG, "::SpawnPlayers() - uidPlayer2(%d%d) is out of game"
				, m_CurrentMatchInfo.uidPlayer2.High, m_CurrentMatchInfo.uidPlayer2.Low);
			m_CurrentMatchInfo.uidPlayer2 = MUID(0, 0);
		} 

		if( m_CurrentMatchInfo.nRoundState == MDUELTOURNAMENTROUNDSTATE_FINAL ) {
			MMatchStage* pStage = GetStage();
			if (pStage != NULL) {
				MMatchStageSetting* pSetting = pStage->GetStageSetting();
				pSetting->SetLimitTime(STAGESETTING_LIMITTIME_UNLIMITED);

				MMatchServer::GetInstance()->RouteCmdDuelTournamentStageSetting(pStage->GetUID());
			}
		}

		LogInfo();
	} else {
		DTLog(DTLOG_RELEASE, "::SpawnPlayers() - DuelTournamentMatchmap is Empty");
	}
}

void MMatchRuleDuelTournament::PrepareDuelTournamentMatchRecord()
{
	MMatchObject* pObj1 = m_pStage->GetObj(m_CurrentMatchInfo.uidPlayer1);
	if( pObj1 != NULL ) {
		int nCID;
		MMatchObjectDuelTournamentCharInfo *pDTCharInfo;
		
		nCID = pObj1->GetCharInfo()->m_nCID;
		pDTCharInfo = pObj1->GetDuelTournamentCharInfo();

		m_MatchRecord.nCID1 = nCID;
		m_MatchRecord.nTP1 = pDTCharInfo->GetTP();
	} else{
		m_MatchRecord.nCID1 = -1;
		m_MatchRecord.nTP1 = -1;
	}

	MMatchObject* pObj2 = m_pStage->GetObj(m_CurrentMatchInfo.uidPlayer2);
	if( pObj2 != NULL ) {
		int nCID;
		MMatchObjectDuelTournamentCharInfo *pDTCharInfo;

		nCID = pObj2->GetCharInfo()->m_nCID;
		pDTCharInfo = pObj2->GetDuelTournamentCharInfo();

		m_MatchRecord.nCID2 = nCID;
		m_MatchRecord.nTP2 = pDTCharInfo->GetTP();
	} else{
		m_MatchRecord.nCID2 = -1;
		m_MatchRecord.nTP2 = -1;
	}
}

void MMatchRuleDuelTournament::RecordGameResult()
{	
	bool bIsLeave = false;
	int nWinnerCID, nLoserCID;
	int nWinnerCurTP, nLoserCurTP;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	if( m_MatchRecord.uidWinner == m_CurrentMatchInfo.uidPlayer1){			///< ����� uidPlayer1�� �̰��� ��!
		nWinnerCID	= m_MatchRecord.nCID1;
		nLoserCID	= m_MatchRecord.nCID2;

		nWinnerCurTP = m_MatchRecord.nTP1;
		nLoserCurTP  = m_MatchRecord.nTP2;

		if( m_CurrentMatchInfo.uidPlayer2 != MUID(0, 0)) { m_DTPlayerMap.SetLoser(m_CurrentMatchInfo.uidPlayer2);} 
		else											 { bIsLeave = true; }
	} else if( m_MatchRecord.uidWinner == m_CurrentMatchInfo.uidPlayer2) {	 ///< ����� uidPlayer2�� �̰��� ��!
		nWinnerCID	= m_MatchRecord.nCID2;
		nLoserCID	= m_MatchRecord.nCID1;

		nWinnerCurTP = m_MatchRecord.nTP2;
		nLoserCurTP  = m_MatchRecord.nTP1;

		if( m_CurrentMatchInfo.uidPlayer1 != MUID(0, 0)) { m_DTPlayerMap.SetLoser(m_CurrentMatchInfo.uidPlayer1);} 
		else											 { bIsLeave = true; }
	} else {
		DTLog(DTLOG_DEBUG, "Player1(%d%d), Player2(%d%d) - Loser(Both)", m_CurrentMatchInfo.uidPlayer1, m_CurrentMatchInfo.uidPlayer2);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	int nGainTP = DUELTOURNAMENT_PLAYER_UNEARNEDPOINT;
	int nLoseTP = DUELTOURNAMENT_PLAYER_LEAVE_PENALTYPOINT;
	bool bFinal = (m_CurrentMatchInfo.nRoundState == MDUELTOURNAMENTROUNDSTATE_FINAL);

	if( bIsLeave == false ){
		nGainTP = m_DuelTournamentCalculator.Calc_WinnerTP(nWinnerCurTP, nLoserCurTP, bFinal);
		nLoseTP = m_DuelTournamentCalculator.Calc_LoserTP(nGainTP, bFinal);
	}

	if( m_CurrentMatchInfo.uidPlayer1 == m_MatchRecord.uidWinner ){
		DTLog(DTLOG_DEBUG, "Winner(%d%d) - Gain %d Point, Loser(%d%d) - Lose %d Point", m_CurrentMatchInfo.uidPlayer1, nGainTP, m_CurrentMatchInfo.uidPlayer2, nLoseTP);
	} else {
		DTLog(DTLOG_DEBUG, "Winner(%d%d) - Gain %d Point, Loser(%d%d) - Lose %d Point", m_CurrentMatchInfo.uidPlayer2, nGainTP, m_CurrentMatchInfo.uidPlayer1, nLoseTP);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// �������� ���� �ִ� �����Ϳ��� ������Ʈ ���ֱ�
	InsertDuelTournamentGameLogDeatil(m_CurrentMatchInfo.nRoundState, nWinnerCID, nLoserCID, nGainTP, nLoseTP, (m_MatchRecord.nFinishTime - m_MatchRecord.nStartTime) / 1000);

	UpdateDuelTournamentPlayerInfo(m_MatchRecord.uidWinner, m_CurrentMatchInfo.nRoundState, true,  nGainTP, false);
	UpdateDuelTournamentPlayerInfo(m_MatchRecord.uidLoser,  m_CurrentMatchInfo.nRoundState, false, nLoseTP, bIsLeave);
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

void MMatchRuleDuelTournament::SetDTRoundState(MMATCH_ROUNDSTATE nState)
{ 	
	m_nRoundState = nState;
	SetRoundStateTimer(MMatchServer::GetInstance()->GetGlobalClockCount());

	// ���尡 ���� ���۵Ǹ� ���� �ʱ�ȭ
	if (nState == MMATCH_ROUNDSTATE_COUNTDOWN) {
		InitRound();
	} else if( nState == MMATCH_ROUNDSTATE_PRE_COUNTDOWN ) {
		OnPreCountDown();
	} else if( nState == MMATCH_ROUNDSTATE_PLAY ) {
		OnRoundBegin();
	}

	// �÷��̾�鿡�� ���� ���� �ٲ���ٰ� �˷���
	MMatchServer::GetInstance()->ResponseRoundState(GetStage()->GetUID());
}

bool MMatchRuleDuelTournament::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
	case MMATCH_ROUNDSTATE_PREPARE:	{
			if (GetStage()->CheckBattleEntry() == true) {
				if (OnCheckEnableBattleCondition()) {
					if( m_MatchRecord.bMatchFinish )	SetDTRoundState(MMATCH_ROUNDSTATE_PRE_COUNTDOWN);
					else								SetDTRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
				}
				else									SetDTRoundState(MMATCH_ROUNDSTATE_FREE);
			}
			return true;

		}

	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN : {
			DWORD dwPeriod = DUELTOURNAMENT_PRECOUNTDOWN_NEXTMATCH_SHOWTIME;
			if (m_CurrentMatchInfo.nMatchNumber != 1)
				dwPeriod += DUELTOURNAMENT_PRECOUNTDOWN_WINLOSE_SHOWTIME;

			if (nClock - GetRoundStateTimer() > dwPeriod) {SetDTRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);}
			return true;
		}

	case MMATCH_ROUNDSTATE_COUNTDOWN: {
			if (nClock - GetRoundStateTimer() > 2*1000) {SetDTRoundState(MMATCH_ROUNDSTATE_PLAY);}
			return true;
		}

	case MMATCH_ROUNDSTATE_PLAY: {
			// �������� ���� ��Ȳ�̸� Free���·� ��ȯ
			if (!OnCheckEnableBattleCondition()) {SetDTRoundState(MMATCH_ROUNDSTATE_FREE);}

			if (OnCheckRoundFinish()) {
				if( m_RoundRecord.nFinishTime == 0 ) {m_RoundRecord.nFinishTime = nClock;} 
				else if( nClock - m_RoundRecord.nFinishTime > 1500) {
					m_RoundRecord.nFinishTime = 0;
					SetDTRoundState( MMATCH_ROUNDSTATE_FINISH );					
				}				
			} else if (OnCheckBattleTimeOut(nClock - GetRoundStateTimer()) 
				&& m_CurrentMatchInfo.nRoundState != MDUELTOURNAMENTROUNDSTATE_FINAL) 
			{
				m_RoundRecord.bIsTimeOut = true;

				SetRoundTimeOut(true);
				SetRoundFinish(true);
			}
			CheckOnGameEvent();
			return true;
		}

	case MMATCH_ROUNDSTATE_FINISH: {
			if (nClock - GetRoundStateTimer() > 1*1000) {				
				OnRoundEnd();

				if (RoundCount() == true)	{ SetDTRoundState(MMATCH_ROUNDSTATE_PREPARE);} 
				else						{ SetDTRoundState(MMATCH_ROUNDSTATE_EXIT);}
			} 
			return true;
		}

	case MMATCH_ROUNDSTATE_FREE: {
			if (OnCheckEnableBattleCondition()) {SetDTRoundState(MMATCH_ROUNDSTATE_PREPARE);}
			return true;
		}

	case MMATCH_ROUNDSTATE_EXIT: {
			return false;
		}

	default: {
			_ASSERT(0);
			return false;
		}		
	}

	return false;
}

void MMatchRuleDuelTournament::OnCommand(MCommand* pCommand)
{
	MMatchRule::OnCommand(pCommand);

	switch(pCommand->GetID()){
		case MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS :
			{
				MUID uidPlayer;
				float fDamagedPoint, fHealthPoint, fAmorPoint;

				pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&fDamagedPoint, 1, MPT_FLOAT);
				pCommand->GetParameter(&fHealthPoint, 2, MPT_FLOAT);
				pCommand->GetParameter(&fAmorPoint, 3, MPT_FLOAT);

				if( uidPlayer == m_CurrentMatchInfo.uidPlayer1 ) {
					m_RoundRecord.fPlayer1DamagedPoint	= (int)fDamagedPoint;
					m_RoundRecord.fPlayer1HealthPoint	= (int)fHealthPoint;
					m_RoundRecord.fPlayer1AmorPoint		= (int)fAmorPoint;
				} else if( uidPlayer == m_CurrentMatchInfo.uidPlayer2 ) {
					m_RoundRecord.fPlayer2DamagedPoint	= (int)fDamagedPoint;
					m_RoundRecord.fPlayer2HealthPoint	= (int)fHealthPoint;
					m_RoundRecord.fPlayer2AmorPoint		= (int)fAmorPoint;
				} else {
					//DTLog(DTLOG_RELEASE, "MMatchRuleDuelTournament::OnCommand - PlayerUID(%d%d, HP(%d), AP(%d)) is not Exists",	uidPlayer.High, uidPlayer.Low, nHP, nAP);
				}
			}
			break;
	}
}


void MMatchRuleDuelTournament::SendDuelTournamentNextGamePlayerInfo()
{
	MTD_DuelTournamentNextMatchPlayerInfo DTPlayerInfo;

	DTPlayerInfo.uidPlayer1	= m_CurrentMatchInfo.uidPlayer1;
	DTPlayerInfo.uidPlayer2	= m_CurrentMatchInfo.uidPlayer2;

	MMatchServer::GetInstance()->RouteCmdDuelTournamentMTDNextGamePlayerInfo(m_pStage->GetUID(), DTPlayerInfo);	
}

void MMatchRuleDuelTournament::SendDuelTournamentGameInfo(bool bIsRoundEnd)
{
	MTD_DuelTournamentGameInfo DTGameInfo;

	DTGameInfo.uidPlayer1	= m_CurrentMatchInfo.uidPlayer1;
	DTGameInfo.uidPlayer2	= m_CurrentMatchInfo.uidPlayer2;
	DTGameInfo.bIsRoundEnd	= bIsRoundEnd;
	DTGameInfo.nMatchNumber = m_CurrentMatchInfo.nMatchNumber;
	DTGameInfo.nMatchType	= m_CurrentMatchInfo.nRoundState;
	DTGameInfo.nRoundCount	= m_MatchRecord.nRoundCount;

	int i = 0;
	for (MDuelTournamentPlayerMap::iterator iter = m_DTPlayerMap.begin(); iter != m_DTPlayerMap.end(); ++iter){
		MDuelTournamentPlayerInfo* pInfo = (*iter).second;
		if( pInfo->bOutGame == false && pInfo->bInMatch == false ) 
			DTGameInfo.WaitPlayerList[i++] = (*iter).first;
	}

	DTGameInfo.nWaitPlayerListLength = static_cast<char>(i);

	MMatchServer::GetInstance()->RouteCmdDuelTournamentMTDGameInfo(m_pStage->GetUID(), DTGameInfo);	
}

void MMatchRuleDuelTournament::SendDuelTournamentRoundResultInfo(bool bIsMatchFinish, bool bIsRoundDraw)
{
	MTD_DuelTournamentRoundResultInfo DTRoundResultInfo;

	DTRoundResultInfo.uidWinnerPlayer	= m_RoundRecord.uidWinnerPlayer;
	DTRoundResultInfo.uidLoserPlayer	= m_RoundRecord.uidLoserPlayer;
	DTRoundResultInfo.bDraw				= bIsRoundDraw;
	DTRoundResultInfo.bIsTimeOut		= m_RoundRecord.bIsTimeOut;
	DTRoundResultInfo.bIsMatchFinish	= bIsMatchFinish;

	MMatchServer::GetInstance()->RouteCmdDuelTournamentMTDRoundResultInfo(m_pStage->GetUID(), &DTRoundResultInfo);

	DTLog(DTLOG_DEBUG, "::SendDuelTournamentRoundResultInfo - Round Winner(%d%d), Round Loser(%d%d)", m_RoundRecord.uidWinnerPlayer.High, 
		m_RoundRecord.uidWinnerPlayer.Low, m_RoundRecord.uidLoserPlayer.High, m_RoundRecord.uidLoserPlayer.Low);
}

void MMatchRuleDuelTournament::SendDuelTournamentMatchResultInfo()
{
	MTD_DuelTournamentMatchResultInfo DTMatchResultInfo;

	DTMatchResultInfo.nMatchNumber		= m_CurrentMatchInfo.nMatchNumber;
	DTMatchResultInfo.nMatchType		= m_CurrentMatchInfo.nRoundState;
	DTMatchResultInfo.uidWinnerPlayer	= m_MatchRecord.uidWinner;
	DTMatchResultInfo.nGainTP			= m_MatchRecord.nGainTP;
	DTMatchResultInfo.uidLoserPlayer	= m_MatchRecord.uidLoser;
	DTMatchResultInfo.nLoseTP			= m_MatchRecord.nLoseTP;
	
	MMatchServer::GetInstance()->RouteCmdDuelTournamentMTDMatchResultInfo(m_pStage->GetUID(), &DTMatchResultInfo);

	DTLog(DTLOG_DEBUG, "::SendDuelTournamentMatchResultInfo - Match(%d), Match Winner(%d%d), Match Loser(%d%d)", m_CurrentMatchInfo.nMatchNumber, 
		m_MatchRecord.uidWinner.High, m_MatchRecord.uidWinner.Low, m_MatchRecord.uidLoser.High, m_MatchRecord.uidLoser.Low);
}

void MMatchRuleDuelTournament::UpdateDuelTournamentPlayerInfo(MUID uidPlayer, MDUELTOURNAMENTROUNDSTATE nState, bool bIsWinner, int nChangeTP, bool bIsLeaveUser)
{
	MMatchObject* pObj1 = m_pStage->GetObj(uidPlayer);
	if( pObj1 != NULL ){
		MMatchObjectDuelTournamentCharInfo *pDTCharInfo = pObj1->GetDuelTournamentCharInfo();

		if( bIsWinner )	{ 
			pDTCharInfo->SetTP(pDTCharInfo->GetTP() + nChangeTP); 
			if( pDTCharInfo->GetTP()  <= 0 ) pDTCharInfo->SetTP(0); 

			pDTCharInfo->IncreaseWins();
		}
		else { 
			pDTCharInfo->SetTP(pDTCharInfo->GetTP() - nChangeTP); 
			if( pDTCharInfo->GetTP()  <= 0 ) pDTCharInfo->SetTP(0); 

			pDTCharInfo->IncreaseLoses();
		}

		switch(nState){
			case MDUELTOURNAMENTROUNDSTATE_QUATERFINAL :	
				pDTCharInfo->IncreaseQuaterFinalCount();
				if(bIsWinner) pDTCharInfo->IncreaseQuaterFinalWins(); 
				break;
			case MDUELTOURNAMENTROUNDSTATE_SEMIFINAL :		
				pDTCharInfo->IncreaseSemiFinalCount();	
				if(bIsWinner) pDTCharInfo->IncreaseSemiFinalWins();	
				break;
			case MDUELTOURNAMENTROUNDSTATE_FINAL :			
				pDTCharInfo->IncreaseFinalCount();		
				if(bIsWinner) pDTCharInfo->IncreaseFinalWins();		
				break;
		}

		if( bIsLeaveUser ) {pDTCharInfo->IncreaseLeaveCount();}

		// Loser�� ���, DB�� ������Ʈ�� ���ش�.
		// Winner������, ��������� �̰��� ���, DB�� ������Ʈ�� ���ش�.
		if( !bIsWinner || (bIsWinner && nState == MDUELTOURNAMENTROUNDSTATE_FINAL)){
			MMatchServer::GetInstance()->OnAsyncRequest_UpdateDuelTournamentCharacterInfo(uidPlayer, m_GameInfo.szTimeStamp);
		} 
	}
}

void MMatchRuleDuelTournament::InsertDuelTournamentGameLogDeatil(MDUELTOURNAMENTROUNDSTATE nDTRoundState, int nWinnerCID, int nLoserCID, int nGainTP, int nLoseTP, int nPlayTime)
{
	MMatchServer::GetInstance()->OnAsyncRequest_InsertDuelTournamentGameLogDetail(
		m_GameInfo.nGameNumber, m_GameInfo.szTimeStamp, m_CurrentMatchInfo.nRoundState, nWinnerCID, nLoserCID, nGainTP, nLoseTP, nPlayTime);
}