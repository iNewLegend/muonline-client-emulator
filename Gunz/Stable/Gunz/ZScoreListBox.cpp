#include "stdafx.h"

#include "ZScoreListBox.h"

ZScoreListBox::ZScoreListBox(const char* szName, MWidget* pParent, MListener* pListener)
: MListBox(szName, pParent, pListener)
{
	SetFocusEnable(false);

	SetItemHeight(20);

	// �ƹ����� IDL�� �ʵ� �׸� �־�� �ҵ�..��.��
	AddField("Team", 80);
	AddField("ID", 100);
	AddField("State", 70);
	AddField("Score", 70);
	AddField("Kills", 70);
	AddField("Deaths", 70);
	AddField("Ping", 70);
}
