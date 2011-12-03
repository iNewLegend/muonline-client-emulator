#include "stdafx.h"
#include "MQuestFormula.h"
#include "MMath.h"

bool MQuestFormula::Create(void)
{

	return true;
}

int MQuestFormula::CalcQL(int nMinPlayerLevel)
{
	if ((1 <= nMinPlayerLevel) && (nMinPlayerLevel <= 4)) return 0;
	else if ((5 <= nMinPlayerLevel) && (nMinPlayerLevel <= 12)) return 1;
	else if ((13 <= nMinPlayerLevel) && (nMinPlayerLevel <= 24)) return 2;
	else if ((25 <= nMinPlayerLevel) && (nMinPlayerLevel <= 40)) return 3;
	else if ((41 <= nMinPlayerLevel) && (nMinPlayerLevel <= 64)) return 4;
	else if ((65 <= nMinPlayerLevel) && (nMinPlayerLevel <= 99)) return 5;

	return 0;
}


int MQuestFormula::CalcQLD(int nQuestLevel)
{
	return (20 + 6 * nQuestLevel);
}

int MQuestFormula::CalcLMT(int nQuestLevel)
{
	// return (8 + nQuestLevel * 3); ���������� ����
	return (9 + nQuestLevel * 2);	// 20051215�� ������ - bird
}

float MQuestFormula::CalcTC(int nQuestLevel)
{
	return ((0.3f * nQuestLevel) + 1.0f);
}

void MQuestFormula::CalcRewardRate(float& foutXPRate, float& foutBPRate,
									  int nScenarioQL, int nPlayerQL, int nDeathCount, 
									  int nUsedPageSacriItemCount, int nUsedExtraSacriItemCount)
{
	float fRate = 1.0f;

/*
	// QL�� ���� �г�Ƽ
	if ((nPlayerQL >= 2) && (nPlayerQL > nScenarioQL))
	{
		fRate = fRate - (0.1f * (nPlayerQL - nScenarioQL));
	}
*/

/*
	// ���� ȸ���� ���� �г�Ƽ
	fRate = fRate - (0.2f * nDeathCount);
*/

	// ��������� ���� ���ʽ�
	float fBonusRate = (nUsedExtraSacriItemCount + nUsedPageSacriItemCount) * 0.1f;

	foutXPRate = fRate + fBonusRate;
	foutBPRate = fRate;		// ��������ۿ� ���� �ٿ�Ƽ ���ʽ��� �������� �ʴ´�.
	

	if (foutXPRate > 1.2f) foutXPRate = 1.2f;
	else if (foutXPRate < 0.0f) foutXPRate = 0.0f;

	if (foutBPRate > 1.2f) foutBPRate = 1.2f;
	else if (foutBPRate < 0.0f) foutBPRate = 0.0f;

}

int MQuestFormula::CalcSectorXP(int nClearXP, int nSectorCount)
{
	nSectorCount = nSectorCount - 1;
	if (nSectorCount == 0) nSectorCount = 1;

	return (int)((nClearXP * 1.2f) / nSectorCount);
}