#include "DlgWorld.h"
#include "DlgShortcuts.h"
#include "DlgChatMessage.h"
#include "DlgChat.h"
#include "DlgMap.h"
#include "DlgTarget.h"
#include "World.h"
#include "PlayerMe.h"
#include "GameCamera.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgWorld::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControl("IDC_DISPLAY_WORLD", m_DisplayWorld);

	CDlgShortcuts::getInstance().Create("IDD_SHORTCUTS",this);
	CDlgChatMessage::getInstance().Create("IDD_CHAT_MESSAGE",this);
	//CDlgChatMessage::getInstance().SetVisible(false);

	CDlgChat::getInstance().Create("IDD_CHAT",this);
	CDlgChat::getInstance().SetVisible(false);
// 
// -- Equip
// dlgEquip = CUIGeneralDialog()
// dlgEquip:create("IDD_EQUIP",IDD_MAIN)
// dlgEquip:SetVisible(false)

	// Move Map List
	CDlgMap::getInstance().Create("IDD_MOVE",this);
	CDlgMap::getInstance().SetVisible(false);
// 
// -- Bag
// dlgBag = CUIGeneralDialog()
// dlgBag:create("IDD_BAG",IDD_MAIN)
// dlgBag:SetVisible(false)
// 
// -- Player Head
// dlgPlayerHead = CUIDialog()
// dlgPlayerHead:create("IDD_PLAYERHEAD",IDD_MAIN)
// dlgPlayerHead:SetVisible(false)

// -- Min Map
// dlgMinmap = CUIDialog()
// dlgMinmap:create("IDD_MINMAP",IDD_MAIN)
// dlgMinmap:SetVisible(false)

// -- Act
// dlgAct = CUIDialog()
// dlgAct:create("IDD_ACT",IDD_MAIN)
// dlgAct:SetVisible(false)

// -- Setup
// dlgSetup = CUIGeneralDialog()
// dlgSetup:create("IDD_SETUP",IDD_MAIN)
// dlgSetup:SetVisible(false)

	CDlgTarget::getInstance().Create("IDD_TARGET",this);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CDlgWorld::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	return CUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------