#include "DlgShortcuts.h"
#include "DlgChatMessage.h"
#include "DlgChat.h"
#include "DlgMap.h"
#include "DlgTarget.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgShortcuts::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControlEvent("IDC_MAIN_CHAR",(PEVENT)&CDlgShortcuts::OnBtnCharInfo,"0");
	RegisterControlEvent("IDC_MAIN_BAG",(PEVENT)&CDlgShortcuts::OnBtnBag,"0");
	RegisterControlEvent("IDC_MAIN_MOVE",(PEVENT)&CDlgShortcuts::OnBtnMove,"0");
	RegisterControlEvent("IDC_MAIN_SETUP",(PEVENT)&CDlgShortcuts::OnBtnSetup,"0");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgShortcuts::OnBtnCharInfo()
{
	//IDD_EQUIP:SetVisible(! IDD_EQUIP:IsVisible())
}

void CDlgShortcuts::OnBtnMove()
{
	CDlgMap::getInstance().SetVisible(!CDlgMap::getInstance().IsVisible());
}

void CDlgShortcuts::OnBtnBag()
{
	//IDD_BAG:SetVisible(not IDD_BAG:IsVisible())
}

#include "World.h"
#include "PlayerMe.h"
#include "Monster.h"
void CDlgShortcuts::OnBtnSetup()
{
	CMonster* monster = new CMonster();
	monster->setRoleID((rand()%1000)+1);
	monster->setType((rand()%100)+1);
	int x = CPlayerMe::getInstance().getCellPosX()+((rand()%11)-5);
	int y = CPlayerMe::getInstance().getCellPosY()+((rand()%11)-5);
	monster->setCellPos(x, y);
	monster->setDir(rand()%8);
	CWorld::getInstance().addRole(monster);
	//IDD_SETUP.SetVisible(not IDD_SETUP.IsVisible());
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------