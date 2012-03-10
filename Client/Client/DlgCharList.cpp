#include "DlgCharList.h"
#include "protocol.h"
#include "common.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControlEvent("IDC_BTN_OK", (PEVENT)&CDlgCharList::OnBtnOk,"0");
	RegisterControlEvent("IDC_BTN_BACK", (PEVENT)&CDlgCharList::OnBtnBack,"0");
	RegisterControlEvent("IDC_BTN_CREATE_ROLE", (PEVENT)&CDlgCharList::OnBtnCharCreate,"0");
	RegisterControlEvent("IDC_BTN_DELETE_CHAR", (PEVENT)&CDlgCharList::OnBtnCharDelete,"0");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern bool g_bLocal;
#include "World.h"
#include "PlayerMe.h"
void CDlgCharList::OnBtnOk()
{
	if (g_bLocal)
	{
		CWorld::getInstance().create(0);
		// --
		CPlayerMe::getInstance().setRoleName(L"Local");
		// --
		CPlayerMe::getInstance().setClass(1);
				CPlayerMe::getInstance().setSkeleton();
				CPlayerMe::getInstance().setEquip(0,1);
				CPlayerMe::getInstance().setEquip(1,1);
				CPlayerMe::getInstance().setEquip(2,1);
				CPlayerMe::getInstance().setEquip(3,1);
				CPlayerMe::getInstance().setEquip(4,1);
				CPlayerMe::getInstance().setEquip(6,1);
				CPlayerMe::getInstance().setEquipSkin(0,0);
				CPlayerMe::getInstance().setEquipSkin(1,1);
				CPlayerMe::getInstance().setEquipSkin(2,2);
				CPlayerMe::getInstance().setEquipSkin(3,3);
				CPlayerMe::getInstance().setEquipSkin(4,0);
				// --
				CPlayerMe::getInstance().setCellPos(167,127);
				CPlayerMe::getInstance().setDir(1);
				CPlayerMe::getInstance().setActionState(0);
				CWorld::getInstance().addRole(&CPlayerMe::getInstance());
				// --
				//OnEnterWorld();
	}
	else
	{
	//MessageBox(L"Enter World......",-1)
	//--EnterWorld(g_UIDisplayCharList:GetSelectIndex())
	}
}
void CDlgCharList::OnBtnBack()
{
	//OnServerList()
}

void CDlgCharList::OnBtnCharCreate()
{
	//dlgRoleCreate:SetVisible(true)
}

void CDlgCharList::OnBtnCharDelete()
{
	//CharDelete(g_UIDisplayCharList:GetSelectIndex(),"111111")
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------