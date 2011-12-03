#ifndef _ZCOMBAT_QUEST_SCREEN_H
#define _ZCOMBAT_QUEST_SCREEN_H


class MDrawContext;
class ZActor;

// ����Ʈ�� �����̹� ��� �� ����Ʈ ���� ��忡�� �������� ȭ��
class ZCombatQuestScreen
{
private:
	void DrawPlayer(MDrawContext* pDC, int index, ZCharacter* pCharacter);
	list<ZCharacter*>		m_SortedCharacterList;
public:
	ZCombatQuestScreen();
	~ZCombatQuestScreen();
	void OnDraw(MDrawContext* pDC);
};




#endif