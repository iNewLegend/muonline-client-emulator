#include "DlgMain.h"
#include "LumpFile.h"
#include "FileSystem.h"
#include "Camera.h"
#include "RegData.h"
#include "MainRoot.h"
#include "RPGSkyUIGraph.h"
#include "DlgMessageBox.h"
#include "DlgLogin.h"
#include "DlgServerList.h"
#include "DlgCharList.h"
#include "DlgShortcuts.h"
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

	CDlgMessageBox::getInstance().Create("IDD_MESSAGE_BOX",this);
	CDlgMessageBox::getInstance().SetVisible(false);
	
	CDlgLogin::getInstance().Create("IDD_LOGIN",this);
	CDlgLogin::getInstance().SetVisible(false);
	
	CDlgServerList::getInstance().Create("IDD_SERVER_LIST",this);
	CDlgServerList::getInstance().SetVisible(true);
	
	CDlgCharList::getInstance().Create("IDD_ROLE_CHOOSE",this);
	CDlgCharList::getInstance().SetVisible(false);

	CDlgShortcuts::getInstance().Create("IDD_MAIN_DIALOG",this);
	CDlgShortcuts::getInstance().SetVisible(false);
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
	// ----
	// draw fps
	if (m_bShowFPS)
	{
		static int s_nFrame = 0;
		static float s_fElapsedTime = 1.0f;
		static float s_fFPS = 1.0f;
		static float s_fMinFPS = 1.0f;
		s_fElapsedTime+=fElapsedTime;
		++s_nFrame;
		// ----
		if (s_nFrame==100)
		{
			s_fFPS = 100.0f/s_fElapsedTime;
			s_nFrame = 0;
			s_fElapsedTime = 0.0f;
		}
		// ----
		if (1.0f/fElapsedTime<30)
		{
			s_fMinFPS = 1.0f/fElapsedTime;
		}
		// ----
		wchar_t wszFps[256];
		RECT rc 	={250, 5 , 450, 25};
		swprintf(wszFps,L"FPS=%.2f~%.2f",s_fMinFPS,s_fFPS);
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
			switch(wParam)
			{
			case VK_F5:// 刷新Shader
				{
					CRenderSystem::getSingleton().commond("resetshader");
				}
			case VK_F9:// 切换曝光效果
				{
				}
				break;

			case VK_F10:// 切换景深
				{
				}
				break;

			case VK_F12:// 切换阴影效果
				{
				}
				break;
			
			case VK_F11:// 切换RT
				{
					static int s_nFlag = 0;
					s_nFlag++;
					m_DisplayWorld.getSceneEffect().setDebug(s_nFlag%8);
				}
				break;

			case VK_RETURN:
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

			break;
		}
	}
	// ----
	return CUIMainDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------