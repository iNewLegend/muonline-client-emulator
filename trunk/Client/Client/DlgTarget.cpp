#include "DlgTarget.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTarget::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	//RegisterControlEvent("IDC_CLOSE", (PEVENT)&CDlgTarget::OnBtnClose);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CDlgTarget::OnInitDialog()
{
	// ----
	return CUIDialog::OnInitDialog();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTarget::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameMove(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTarget::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameRender(mTransform, fTime, fElapsedTime);
}

void CDlgTarget::setTargetName(const wchar_t* wszName)
{
	CUIControl* pControl = getControl("IDC_TARGET_NAME");
	if (pControl)
	{
		wchar_t wszTargetName[255];
		swprintf(wszTargetName,L"[align=center]%s[/align]",wszName);
		pControl->SetText(wszTargetName);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------