#include "DlgCharList.h"
#include "protocol.h"
#include "common.h"
#include "World.h"
#include "PlayerMe.h"
#include "DlgMain.h"
#include "DlgWorld.h"
#include "DlgServerList.h"
#include "DlgCharCreate.h"
#include "GameCamera.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	CDlgCharCreate::getInstance().Create("IDD_CHAR_CREATE",this);
	CDlgCharCreate::getInstance().SetVisible(false);
	// ----
	RegisterControlEvent("IDC_BTN_OK", (PEVENT)&CDlgCharList::OnBtnOk,"0");
	RegisterControlEvent("IDC_BTN_BACK", (PEVENT)&CDlgCharList::OnBtnBack,"0");
	RegisterControlEvent("IDC_BTN_CREATE_ROLE", (PEVENT)&CDlgCharList::OnBtnCharCreate,"0");
	RegisterControlEvent("IDC_BTN_DELETE_CHAR", (PEVENT)&CDlgCharList::OnBtnCharDelete,"0");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::SetVisible(bool bVisible)
{
	CUIDialog::SetVisible(bVisible);
	// ----
	if(bVisible)
	{
		CSkeletonNode* bg = new CSkeletonNode();
		bg->setFilename("data\\logo\\logo05.bmd");
		CWorld::getInstance().addChild(bg);
		CGameCamera::getInstance().setRadius(7);
		CGameCamera::getInstance().setYawAngle(0);
		CGameCamera::getInstance().setPitchAngle(0);
		CPlayerMe::getInstance().setCellPos(2,0);
		CWorld::getInstance().create(0);
		CWorld::getInstance().addRole(&CPlayerMe::getInstance());
		//--g_UIDisplayCharList.m_vEye=Vec3D(2.0,2.0,-7.0)
		//--g_UIDisplayCharList.m_vLookAt=Vec3D(2.0,1.0,-0.0)
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::OnBtnOk()
{
	extern bool g_bLocal;
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
		SetVisible(false);
		CDlgWorld::getInstance().SetVisible(true);
	}
	else
	{
		//MessageBox(L"Enter World......",-1)
		//--EnterWorld(g_UIDisplayCharList:GetSelectIndex())
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::OnBtnBack()
{
	SetVisible(false);
	CDlgServerList::getInstance().SetVisible(true);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::OnBtnCharCreate()
{
	CDlgCharCreate::getInstance().SetVisible(true);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharList::OnBtnCharDelete()
{
	//CharDelete(g_UIDisplayCharList:GetSelectIndex(),"111111")
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------