#ifndef _ZRULE_DUELTOURNAMENT_H
#define _ZRULE_DUELTOURNAMENT_H

#include "ZRule.h"

class ZRuleDuelTournament : public ZRule
{
public:
	MTD_DuelTournamentGameInfo m_DTGameInfo;
	MTD_DuelTournamentNextMatchPlayerInfo m_prevPlayerInfo;
	MTD_DuelTournamentNextMatchPlayerInfo m_nextPlayerInfo;

	int							m_nDTRoundCount;			// ����ϱ����� ��ü���� ����
	int							m_nDTPlayCount;				// ������ ���и� ���������� ���

	MDUELTOURNAMENTROUNDSTATE	m_eDTRoundState;
	DuelTournamentPlayer		m_QuaterFinalPlayer[8];
	DuelTournamentPlayer		m_DTChampion;

	float						m_fBlinkOpacity;
	
	bool						m_bSlideToCenter;			// UI �����̵� ȿ�� ����
	float						m_fSlideElapsedTime;		// UI �����̵� ȿ�� ������ �� �帥 �ð�

	enum PRECOUNTDOWN_DETAIL {	// ����ī��Ʈ ���¸� ���������� ������
		PCD_NOT_PRECOUNTDOWN,		// ����ī��Ʈ�ٿ� ���°� �ƴ�
		PCD_WINLOSE,				// ���� ��ġ�� ����/���ڸ� ǥ��
		PCD_NEXTMATCH,				// ���� ��ġ�� ������ ������ ǥ��
	};

	bool						m_bFirstPreCountdown;
	PRECOUNTDOWN_DETAIL			m_ePreCountdownDetail;

	DWORD						m_dwTimeEnterPreCountDown;	// PRE_COUNTDOWN ���¿� ������ �ð�

	void InitCharacterList();
	void SetPreCountdownDetail(PRECOUNTDOWN_DETAIL eDetail);

public:

	ZRuleDuelTournament(ZMatch* pMatch);
	virtual ~ZRuleDuelTournament();

	int	GetQueueIdx(const MUID& uidChar);

	virtual bool OnCommand(MCommand* pCommand);
	virtual void AfterCommandProcessed( MCommand* pCommand );
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState);
	virtual void OnUpdate(float fDelta);

	int GetDuelTournamentPlayCount() { return m_nDTPlayCount; }
	MUID GetChampion() { return m_DTChampion.uidPlayer; }

	void OnDraw(MDrawContext* pDC);

	void SetMatchPlayerInfoUI(const MUID& uidPlayer1, const MUID& uidPlayer2);
	void ShowMatchPlayerInfoUI(bool bShow);
	void ShowMatchPlayerInfoUI_OnlyNextMatch(bool bShow);
	void ShowWinLoseScreenEffect(MTD_DuelTournamentMatchResultInfo& DTGameResultInfo);

	void SetRoundResultInfo(MTD_DuelTournamentRoundResultInfo& DTGameResultInfo);
	void SetMatchResultInfo(MTD_DuelTournamentMatchResultInfo& DTGameResultInfo);
	void ShowMatchOrder(MDrawContext* pDC, bool isResult, float fElapsed);
	DuelTournamentPlayer* GetPlayer(const MUID& uid);
	void DrawVictorySymbol(MDrawContext* pDC, MUID uidPlayer1, MUID uidPlayer2);

	void SetPlayerHpApForUI(const MUID& uidChar, float fmaxhp, float fmaxap, float fhp, float fap);		// UI�� ���� �׷��ֱ⸸�� �뵵
	void GetPlayerHpApForUI(const MUID& uidChar, float* fmaxhp, float* fmaxap, float* fhp, float* fap);		// UI�� ���� �׷��ֱ⸸�� �뵵
	int GetPlayerInfoIndex(const MUID& uidChar);
	int GetPingValue(MUID uiPlayer);		// �ΰ� ��������

private:
	void BeginPlayerInfoUISlideAni(bool bToCenter);
	void UpdateUISlideAni(float fElapsed);
	void DrawInverse(MDrawContext* pDC, MBitmap* pBitmap, float x, float y, float fRateX, float fRateY, int nMirror);
	void DrawHighlight(MDrawContext* pDC, const MRECT& rc);

	void RestoreStageExitButtonConfirmMessage();
	void SetVisiblePlayerInfoUI(bool bLeft, bool bShow);
};

inline ZCharacterView* GetWidgetCharViewLeft() {
	return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_CharviewerLeft");
}
inline ZCharacterView* GetWidgetCharViewRight() {
	return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_CharviewerRight");
}
inline ZCharacterView* GetWidgetCharViewResult() {
	return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("DuelTournamentResult_Charviewer");
}

#endif