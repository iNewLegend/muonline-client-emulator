#include "DlgMain.h"
#include "LumpFile.h"
#include "FileSystem.h"
#include "Camera.h"
#include "RegData.h"
#include "MainRoot.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMain::CDlgMain()
	:m_pIconCursor(NULL)
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
	if (m_pIconCursor)
	{
		POINT cursor;
		GetCursorPos( &cursor );	
		::ScreenToClient(CMainRoot::getInstance().GetHWND(), &cursor);
		CUIControl::updateUIMatrix(mTransform, fTime, fElapsedTime);
		RECT rc;
		rc.left		= cursor.x-16;
		rc.right	= cursor.x+16;
		rc.top		= cursor.y-16;
		rc.bottom	= cursor.y+16;
		m_pIconCursor->getStyle().draw(rc,L"");
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