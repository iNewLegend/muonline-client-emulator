#include "DlgServerList.h"
#include "protocol.h"
#include "common.h"
#include "DlgLogin.h"
#include "DlgMessageBox.h"
#include "DlgCharList.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgServerList::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControl("IDC_LISTBOX_SERVER", m_ListBoxServer);
	RegisterControlEvent("IDC_BTN_OK",(PEVENT)&CDlgServerList::OnBtnOK);
	RegisterControlEvent("IDC_LISTBOX_SERVER",(PEVENT)&CDlgServerList::OnBtnOK,"DBLCLK");

	//m_ListBoxServer:selectByText(L"k")
	// Server List
	FILE* fp = fopen("server.ini", "r");
	if (fp)
	{
		while(true)
		{
			char szTemp[1024]	="";
			if (EOF == fscanf(fp, "%s\n", szTemp))
				break;
			m_ListBoxServer.AddItem(s2ws(szTemp).c_str());
			if (EOF == fscanf(fp, "%s\n", szTemp))
				break;
			if (EOF == fscanf(fp, "%s\n", szTemp))
				break;
		}
		fclose(fp);
		m_ListBoxServer.SelectItem(0);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgServerList::OnBtnOK()
{
	// g_Audio:PlayMusic("Data/Music/castle.mp3",true,255)
	int serverIndex = m_ListBoxServer.getSelectedIndex();
	int nIndex=0;
	// Connect Server
	FILE* fp = fopen("server.ini", "r");
	if (fp)
	{
		while(true)
		{
			char szName[128]	= "";
			char szIP[128]		= "";
			int nPort			= 0;
			if (EOF == fscanf(fp, "%s\n", szName))
				break;
			if (EOF == fscanf(fp, "%s\n", szIP))
				break;
			if (EOF == fscanf(fp, "%d\n", &nPort))
				break;
			if(nIndex == serverIndex)
			{
				extern bool g_bLocal;
				if(strcmp(szName,"local")==0)
				{
					g_bLocal = true;
					SetVisible(false);
					CDlgCharList::getInstance().SetVisible(true);
					//OnRoleChoose();
				}
				// Connecting
				else if(CSConnectServer(szIP,nPort))
				{
					g_bLocal = false;
					SetVisible(false);
					CDlgLogin::getInstance().SetVisible(true);
				}
				else
				{
					CDlgMessageBox::getInstance().setMessage(L"Server Connection Failed!");
				}
			}
			nIndex++;
		}
		fclose(fp);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------