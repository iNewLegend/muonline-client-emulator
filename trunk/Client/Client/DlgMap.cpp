#include "DlgMap.h"
#include "protocol.h"
#include "common.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMap::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----
	RegisterControl("IDC_LISTBOX_MOVE", m_ListMap);
	RegisterControlEvent("IDC_LISTBOX_MOVE",(PEVENT)&CDlgMap::OnMapMove,"1793");
	// ----
	FILE* fp = fopen("Data/move.txt", "r");
	if (fp)
	{
		while(true)
		{
			char szLine[1024]	="";
			if (EOF == fscanf(fp, "%s\n", szLine))
				break;
			char* pEnd=strchr( szLine,'=');
			if( pEnd == NULL)
				continue;
			*pEnd = 0;
			m_ListMap.AddItem(s2ws(szLine).c_str());
			m_MoveCmdList.push_back(pEnd+1);
		}
		fclose(fp);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMap::OnMapMove()
{
	int serverIndex = m_ListMap.getSelectedIndex();
	if (serverIndex < 0 )
	{
		//MessageBox(L"You did not select a map!");
		return;
	}
	CSChat(m_MoveCmdList[serverIndex].c_str());
	SetVisible(false);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------