#include "stdafx.h"
#include "MDuelTournamentFormula.h"

int MDuelTournamentFormula::Calc_WinnerTP(int nWinnerTP, int nLoserTP, bool isFinal)
{
	float fResult = 5.0f / (1 + pow(5.0f, float(nWinnerTP-nLoserTP) / 1000.0f));
	
	if( fResult < 1 ) fResult = 1;

	if( isFinal )	return (int)(fResult * 2);	// ������� ������ 2��
	else			return (int)fResult;
}

int MDuelTournamentFormula::Calc_LoserTP(int nWinnerGainTP, bool isFinal)
{
	float fWinnerGainTP = (float)nWinnerGainTP;
	if (isFinal)
		fWinnerGainTP /= 2.f;			// ����� ��� ���ڴ� 2�� ���ʽ��� �����Ƿ� ���� �������� �ǵ��� ��

	return (int)(fWinnerGainTP / 2.f);	// ���� ������ ���ݸ�ŭ�� ���ڿ��� ����
}