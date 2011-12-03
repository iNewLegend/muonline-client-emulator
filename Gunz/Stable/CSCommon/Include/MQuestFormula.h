#ifndef _MQUEST_FORMULA_H
#define _MQUEST_FORMULA_H


#include "winsock2.h"
#include "MXml.h"

/// ����Ʈ���� ����ϴ� ���� ���� Ŭ����. 
/// - ��� static���� �Ǿ��ִ�.
class MQuestFormula
{
private:

public:
	/// ��꿡 �ʿ��� ������ �ʱ�ȭ�Ѵ�.
	static bool Create(void);

	/// �÷��̾� �����κ��� ����Ʈ ����(QL, Quest Level)�� ����Ѵ�.
	/// @param nMinPlayerLevel		���� �÷��̾��� ���� ����
	static int CalcQL(int nMinPlayerLevel);

	/// ����Ʈ ���� ���̵�(QLD, Quest Level Difficulty)�� ���Ѵ�.
	/// @param nQuestLevel			����Ʈ ����
	static int CalcQLD(int nQuestLevel);

	/// ���;ȿ� ������ �� �ִ� �ִ� NPC ��(LMT)�� ���Ѵ�.
	/// @param nQuestLevel			����Ʈ ����
	static int CalcLMT(int nQuestLevel);

	// NPC ���̵� ���� ���(TC, Toughness Coefficient)�� ���Ѵ�.
	/// @param nQuestLevel			����Ʈ ����
	static float CalcTC(int nQuestLevel);

	// ����ġ ���� ����
	/// @param foutXPRate				��ȯ�� XP ����
	/// @param foutBPRate				��ȯ�� BP ����
	/// @param nScenarioQL				�ó����� QL
	/// @param nDeathCount				���� ȸ��
	/// @param nUsedPageSacriItemCount	�⺻ ��� ������ ��� ����(������ �� ���� ������)
	/// @param nUsedExtraSacriItemCount	�߰� ��� ������ ��� ����(Ư�� ������)
	static void CalcRewardRate(float& foutXPRate, 
							   float& foutBPRate,
                               int nScenarioQL,
                               int nPlayerQL,
                               int nDeathCount,
                               int nUsedPageSacriItemCount,
                               int nUsedExtraSacriItemCount);

	// ���� Ŭ���� ȹ�� ����ġ
	/// @param nClearXP					����Ʈ Ŭ���� ȹ�� ����ġ
	/// @param nSectorCount				���ͼ�
	static int CalcSectorXP(int nClearXP, int nSectorCount);
};





#endif