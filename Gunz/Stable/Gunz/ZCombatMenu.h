#ifndef _ZCOMBAT_MENU_H
#define _ZCOMBAT_MENU_H

#include <string>
using namespace std;

// �����߿� ������ �޴�
class ZCombatMenu
{
public:
	enum ZCOMBAT_MENU_ITEM
	{
		ZCMI_OPTION = 0,		// �ɼ�
		ZCMI_CLOSE,				// �������� ���ư���
		ZCMI_BATTLE_EXIT,		// �������� ������
		ZCMI_STAGE_EXIT,		// ���ӿ��� ������
		ZCMI_PROG_EXIT,			// ���α׷� ����

		ZCMI_END
	};

private:
	string		m_ItemStr[ZCMI_END];
	string		m_FrameStr;
public:
	ZCombatMenu();
	~ZCombatMenu();
	bool IsEnableItem(ZCOMBAT_MENU_ITEM nItem);					// �ش� �޴��� Ȱ��ȭ�Ǿ����� ����
	void EnableItem(ZCOMBAT_MENU_ITEM nItem, bool bEnable);			// �ش� �޴� Ȱ��ȭ

	void ShowModal(bool bShow=true);
	bool IsVisible();
};








#endif