--g_Audio:PlayMusic("Data/Music/login_theme.mp3",true,255)

-- Create Control
editUser = CUIEditBox()
editPassword = CUIEditBox()

-- Register Control
IDD_LOGIN:regControl("IDC_EDIT_BOX_USER", editUser)
IDD_LOGIN:regControl("IDC_EDIT_BOX_PASSWORD", editPassword)

-- Register Event
IDD_LOGIN:regControlEvent("IDC_BTN_LOGIN","IDD_LOGIN_OnBtnLogin","0")
IDD_LOGIN:regControlEvent("IDC_BTN_BACK","IDD_LOGIN_OnBtnBack","0")

-- Event Func
function IDD_LOGIN_OnBtnBack()
	IDD_SERVER_LIST:SetVisible(true)
	IDD_LOGIN:SetVisible(false)
	--g_Audio:PlayMusic("Data/Music/castle.mp3",true,255)
	--GameClose()
end

function IDD_LOGIN_OnBtnLogin()
	MessageBox(L"µÇÂ¼ÖÐ...",-1)
	LoginServer(ws2s(editUser:GetText()),ws2s(editPassword:GetText()))
end