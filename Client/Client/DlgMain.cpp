#include "DlgMain.h"
#include "LumpFile.h"
#include "FileSystem.h"
#include "Camera.h"
#include "RegData.h"
#include "MainRoot.h"
#include "RPGSkyUIGraph.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMain::CDlgMain()
	:m_pIconCursor(NULL)
	,m_bShowFPS(true)
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
	// draw fps
	// ----
	if (m_bShowFPS)
	{
		static float s_fElapsedTime = 1.0f;
		static float s_fFPS = 1.0f;
		s_fElapsedTime+=fElapsedTime;
		if (/*s_fElapsedTime>1.0f||*/1.0f/fElapsedTime<30)
		{
			s_fElapsedTime = 0.0f;
			s_fFPS = 1.0f/fElapsedTime;
		}
		// ----
		wchar_t wszFps[256];
		RECT rc 	={250, 5 , 350, 25};
		swprintf(wszFps,L"FPS=%.2f",s_fFPS);
		CUIControl::updateUIMatrix(mTransform, fTime, fElapsedTime);
		Node3DUIGraph::getInstance().drawText(wszFps, lstrlenW(wszFps), rc, ALIGN_TYPE_CENTER);

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