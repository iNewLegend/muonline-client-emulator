#include "UIGeneralDialog.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIGeneralDialog::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControlEvent("IDC_CLOSE", (PEVENT)&CUIGeneralDialog::OnBtnClose);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CUIGeneralDialog::OnInitDialog()
{
	// ----
	return CUIDialog::OnInitDialog();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIGeneralDialog::OnBtnClose()
{
	SetVisible(false);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIGeneralDialog::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameMove(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIGeneralDialog::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameRender(mTransform, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CUIGeneralDialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if(wParam == VK_ESCAPE)
			{
				SetVisible(false);
			}
			break;
		}
	}
	// ----
	return CUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------