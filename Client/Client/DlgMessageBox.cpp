#include "DlgMessageBox.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMessageBox::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControlEvent("IDC_BTN_OK",(PEVENT)&CDlgMessageBox::OnBtnOK);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMessageBox::setMessage(const wchar_t* wszMessage, int nType)
{
	//SetControlText("IDC_STATIC_INFO",wszMessage);
	SetVisible(true);
	if (nType==-1)
	{
		setControlVisible("IDC_BTN_OK",false);
	}
	else
	{
		setControlVisible("IDC_BTN_OK",true);
	}
}

void CDlgMessageBox::OnBtnOK()
{
	SetVisible(false);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------