#include "DlgCharCreate.h"
#include "protocol.h"
#include "common.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharCreate::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControl("IDC_DISPLAY_ROLE", m_DisplayModel);
	RegisterControl("IDC_STA_INFO", m_StaProInfo );
	RegisterControl("IDC_EDT_NAME", m_EdtName );
	// ----
	RegisterControlEvent("IDC_BTN_OK", (PEVENT)&CDlgCharCreate::OnBtnOk,"0");
	RegisterControlEvent("IDC_BTN_EXIT", (PEVENT)&CDlgCharCreate::OnBtnExit,"0");
	// ----
	RegisterControlEvent("IDC_BTN_ROLE1", (PEVENT)&CDlgCharCreate::OnBtnRole1,"0");
	RegisterControlEvent("IDC_BTN_ROLE2", (PEVENT)&CDlgCharCreate::OnBtnRole2,"0");
	RegisterControlEvent("IDC_BTN_ROLE3", (PEVENT)&CDlgCharCreate::OnBtnRole3,"0");
	// ----
	OnBtnRole1();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgCharCreate::OnBtnOk()
{
// 	if(m_EdtName.GetText().len)
// 	if string.len(m_EdtName:GetText())<4 then
// 		MessageBox(L"名字太短，大于4位！")
// 		return
// 	end
 	CSCharCreate(ws2s(m_EdtName.GetText()).c_str(),m_nIndex);
	SetVisible(false);
}

void CDlgCharCreate::OnBtnExit()
{
	SetVisible(false);
}

void CDlgCharCreate::OnBtnRole1()
{
	m_DisplayModel.LoadModel("data\\logo\\NewFace01.bmd");
	m_DisplayModel.m_vEye=Vec3D(0.05,0.08,-0.15);
	m_DisplayModel.m_vLookAt=Vec3D(0.0,0.08,0.0);
	m_StaProInfo.SetText(L"shit1");
	m_nIndex = 0;
}

void CDlgCharCreate::OnBtnRole2()
{
	m_DisplayModel.LoadModel("data\\logo\\NewFace02.bmd");
	m_DisplayModel.m_vEye=Vec3D(0.05,0.08,-0.15);
	m_DisplayModel.m_vLookAt=Vec3D(0.0,0.08,0.0);
	m_StaProInfo.SetText(L"shit2");
	m_nIndex = 1;
}

void CDlgCharCreate::OnBtnRole3()
{
	m_DisplayModel.LoadModel("data\\logo\\NewFace03.bmd");
	m_DisplayModel.m_vEye=Vec3D(0.0,0.07,-0.1);
	m_DisplayModel.m_vLookAt=Vec3D(0.0,0.07,0.0);
	m_StaProInfo.SetText(L"shit3");
	m_nIndex = 2;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------