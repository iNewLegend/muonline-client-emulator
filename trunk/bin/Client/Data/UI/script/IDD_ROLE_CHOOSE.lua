-- Create Control
--m_DisplayBackground = CUIDisplayRoleChoose()
IDD_ROLE_CHOOSE_btnOK = CUIButton()

dlgRoleCreate = CUIDialog()
-- Register Control
IDD_ROLE_CHOOSE:regControl("IDC_BTN_OK", IDD_ROLE_CHOOSE_btnOK)

dlgRoleCreate:create("IDD_ROLE_CREATE",IDD_ROLE_CHOOSE)

IDD_ROLE_CHOOSE:regControl("IDC_DISPLAY_BACKGROUND", g_UIDisplayCharList)
-- Register Event
IDD_ROLE_CHOOSE:regControlEvent("IDC_BTN_BACK", "IDD_ROLE_CHOOSE_OnBtnBack","0");
IDD_ROLE_CHOOSE:regControlEvent("IDC_BTN_HELP", "IDD_ROLE_CHOOSE_OnBtnHelp","0");
IDD_ROLE_CHOOSE:regControlEvent("IDC_BTN_OK", "IDD_ROLE_CHOOSE_OnBtnOk","0");
IDD_ROLE_CHOOSE:regControlEvent("IDC_BTN_CREATE_ROLE", "IDD_ROLE_CHOOSE_OnBtnCreateRole","0");

--IDD_ROLE_CHOOSE_btnOK.SetVisible(false)
--
dlgRoleCreate:SetVisible(false)

-- Event Func
function IDD_ROLE_CHOOSE_OnBtnBack()
	IDD_SERVER_LIST:SetVisible(true)
	IDD_ROLE_CHOOSE:SetVisible(false)
end

function IDD_ROLE_CHOOSE_OnBtnHelp()
	MessageBox(L"IDD_ROLE_CHOOSE_OnBtnHelp",0)
end

function IDD_ROLE_CHOOSE_OnBtnOk()
	if g_UIDisplayCharList:GetSelectIndex()<0 then
		MessageBox(L"who?")
	else
		MessageBox(L"Enter World......",-1)
		EnterWorld(g_UIDisplayCharList:GetSelectIndex())
	end
end

function IDD_ROLE_CHOOSE_OnBtnCreateRole()
	dlgRoleCreate:SetVisible(true)
end