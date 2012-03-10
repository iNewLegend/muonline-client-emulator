#include "DlgServerList.h"
#include "protocol.h"
#include "common.h"
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
			char szLine[1024]	="";
			if (EOF == fscanf(fp, "%s\n", szLine))
				break;
			char* pEnd=strchr( szLine,' ');
			if( pEnd == NULL)
				continue;
			*pEnd = 0;
			m_ListBoxServer.AddItem(s2ws(szLine).c_str());
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
			if (EOF == fscanf(fp, "%s %s %d\n", szName, szIP, nPort))
				break;
			if(nIndex == serverIndex)
			{
				if(strcmp(szName,"local")==0)
				{
					//SetLocal(true);
					//OnRoleChoose();
				}
				// Connecting
				else if(CSConnectServer(szIP,nPort))
				{
					//SetLocal(false);
					SetVisible(false);
					//IDD_LOGIN:SetVisible(true)
				}
				else
				{
					//MessageBox(L"Server Connection Failed!");
				}
			}
			nIndex++;
		}
		fclose(fp);
		m_ListBoxServer.SelectItem(0);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------