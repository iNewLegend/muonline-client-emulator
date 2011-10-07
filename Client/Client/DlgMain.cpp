#include "DlgMain.h"
#include "LumpFile.h"
#include "FileSystem.h"
#include "Camera.h"
#include "RegData.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMain::CDlgMain()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMain::~CDlgMain()
{	
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMain::OnControlRegister()
{
	CUIMainDialog::OnControlRegister();
	// ----
	RegisterControl("IDC_DISPLAY_WORLD", m_DisplayWorld);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CDlgMain::OnInitDialog()
{
	// ----
	return CUIMainDialog::OnInitDialog();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMain::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIMainDialog::OnFrameMove(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMain::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIMainDialog::OnFrameRender(mTransform, fTime, fElapsedTime);
	if (m_IconCursor.IsVisible())
	{
		m_IconCursor.OnFrameRender(mTransform, fTime, fElapsedTime);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CDlgMain::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if(wParam == VK_RETURN)
			{
				CUIDialog * pDialog = getChildDialog("IDD_CHAT");
				// ----
				if(pDialog != NULL)
				{
					pDialog->SetVisible( ! pDialog->IsVisible());
				}
			}
			break;
		}
	}
	// ----
	return CUIMainDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------