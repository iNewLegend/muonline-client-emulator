#include "DlgLogin.h"
#include "protocol.h"
#include "common.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgLogin::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControl("IDC_EDIT_BOX_USER", m_EditUser);
	RegisterControl("IDC_EDIT_BOX_PASSWORD", m_EditPassword);
	// ----
	RegisterControlEvent("IDC_BTN_LOGIN",(PEVENT)&CDlgLogin::OnBtnLogin);
	RegisterControlEvent("IDC_BTN_BACK",(PEVENT)&CDlgLogin::OnBtnBack);
	//--g_Audio:PlayMusic("Data/Music/login_theme.mp3",true,255)
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgLogin::OnBtnLogin()
{
	//MessageBox(L"µÇÂ¼ÖÐ...",-1);
	CSLoginServer(ws2s(m_EditUser.GetText()).c_str(),ws2s(m_EditPassword.GetText()).c_str());
}

void CDlgLogin::OnBtnBack()
{
	//IDD_SERVER_LIST:SetVisible(true)
	SetVisible(false);
	//--g_Audio:PlayMusic("Data/Music/castle.mp3",true,255)
	//--GameClose()
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------