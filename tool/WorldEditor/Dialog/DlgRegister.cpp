#include "DlgRegister.h"
#include "LumpFile.h"
#include "FileSystem.h"
/*#include "World.h"*/
#include "Camera.h"
#include "TextRender.h"

CDlgRegister::CDlgRegister()
{
}

CDlgRegister::~CDlgRegister()
{	
}

bool CDlgRegister::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return CUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

#include "diskid32.h"

inline std::string UUBCode(const std::string& str)  
{
	std::string strDecode;
	strDecode.resize(str.size());
	static const char tab[10] = {
		'U', '1', '4', 'z','7',
		'0', 'q', 'o', '8','S'
	};
	char key = 0x5E;
	for (size_t i=0; i<str.size(); ++i)
	{
		strDecode[i] = tab[str[i]-'0'];
	}
	return strDecode;
}

void CDlgRegister::OnControlRegister()
{
	CUIDialog::OnControlRegister();

	RegisterControl("IDC_EDIT_BOX_EMAIL",		m_EditBoxEmail);
	RegisterControl("IDC_EDIT_BOX_HARDWARE_ID",	m_EditBoxHardwareID);
	RegisterControl("IDC_EDIT_BOX_KEY",		m_EditBoxKey);

	RegisterControlEvent("IDC_BTN_OK",		(PEVENT)&CDlgRegister::OnBtnOK);
	RegisterControlEvent("IDC_BTN_CANCEL", (PEVENT)&CDlgRegister::OnBtnCancel);

	m_EditBoxHardwareID.setEditEnabled(false);

	//m_EditBoxHardwareID.SetText(s2ws(UUBCode(Format("%u%u",getHardDriveComputerID(),GetMACaddress()))));
}

bool CDlgRegister::OnInitDialog()
{
#if defined(_MU_KEY)
	std::string strInfo;
	SetVisible(!CMUScene::checkKeyByFile(strInfo));
	m_wstrCaption = s2ws(strInfo);
#else
	SetVisible(false);
#endif
	return true;
}

void CDlgRegister::OnBtnOK()
{
	std::string strInfo;
	//CMUScene::regKey(ws2s(m_EditBoxEmail.GetText()), ws2s(m_EditBoxKey.GetText()) ,strInfo);
	//m_wstrCaption = s2ws(strInfo);
}

void CDlgRegister::OnBtnCancel()
{
	SetVisible(false);
}