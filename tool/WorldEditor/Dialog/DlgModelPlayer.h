#pragma once
#include "UIDialog.h"

enum EQUIP_TYPE
{
	ET_HELM,
	ET_ARMOR,
	ET_GLOVE,
	ET_PANTS,
	ET_BOOT,
	ET_MAX,
};

static const char* g_EquipNames[ET_MAX]=
{
	"HELM",
	"ARMOR",
	"GLOVE",
	"PANT",
	"BOOT",
};

class CDlgModelPlayer :public CUIDialog
{
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	void OnUpdatePlayer();
	void OnSelectSet();
protected:
private:
	CUIComboBox m_ComboBoxSkeleton;

	CUIComboBox m_ComboBoxSet;

	CUIComboBox m_ComboBoxHead;

	CUIComboBox m_ComboBoxEquips[ET_MAX];

	CUIComboBox m_ComboBoxLeftHand;
	CUIComboBox m_ComboBoxRightHand;
	CUIComboBox m_ComboBoxBack;
};