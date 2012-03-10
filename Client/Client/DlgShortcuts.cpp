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

	CDlgChatMessage::getInstance().Create("IDD_CHAT_MESSAGE",this);
	//CDlgChatMessage::getInstance().SetVisible(false);

	CDlgChat::getInstance().Create("IDD_CHAT",this);
	CDlgChat::getInstance().SetVisible(false);
// 
// -- Equip
// dlgEquip = CUIGeneralDialog()
// dlgEquip:create("IDD_EQUIP",IDD_MAIN)
// dlgEquip:SetVisible(false)

	// Move Map List
	CDlgMap::getInstance().Create("IDD_MOVE",this);
	CDlgMap::getInstance().SetVisible(false);
// 
// -- Bag
// dlgBag = CUIGeneralDialog()
// dlgBag:create("IDD_BAG",IDD_MAIN)
// dlgBag:SetVisible(false)
// 
// -- Player Head
// dlgPlayerHead = CUIDialog()
// dlgPlayerHead:create("IDD_PLAYERHEAD",IDD_MAIN)
// dlgPlayerHead:SetVisible(false)

// -- Min Map
// dlgMinmap = CUIDialog()
// dlgMinmap:create("IDD_MINMAP",IDD_MAIN)
// dlgMinmap:SetVisible(false)

// -- Act
// dlgAct = CUIDialog()
// dlgAct:create("IDD_ACT",IDD_MAIN)
// dlgAct:SetVisible(false)

// -- Setup
// dlgSetup = CUIGeneralDialog()
// dlgSetup:create("IDD_SETUP",IDD_MAIN)
// dlgSetup:SetVisible(false)

	CDlgTarget::getInstance().Create("IDD_TARGET",this);
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