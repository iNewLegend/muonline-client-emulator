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
	RegisterControl("IDC_STATIC_FPS", m_StaticFps);
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
	static float fFps = 0.0f;
	if (fElapsedTime!=0.0f)
	{
		fFps = fFps*0.9f+0.1f/fElapsedTime;
	}
	m_StaticFps.SetFloat(fFps,0,2);
	CUIMainDialog::OnFrameMove(fTime,fElapsedTime);
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