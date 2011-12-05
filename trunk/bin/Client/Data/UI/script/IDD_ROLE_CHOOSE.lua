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
IDD_ROLE_CHOOSE:regControlEvent("IDC_BTN_OK", "IDD_ROLE_CHOOSE_OnBtnOk","0");
IDD_ROLE_CHOOSE:regControlEvent("IDC_BTN_CREATE_ROLE", "IDD_ROLE_CHOOSE_OnBtnCreateRole","0");

--
dlgRoleCreate:SetVisible(false)

-- Event Func
function IDD_ROLE_CHOOSE_OnBtnBack()
	OnServerList()
end

function IDD_ROLE_CHOOSE_OnBtnOk()

if g_bLocal then
				g_World:create(0)
				-- 
				g_PlayerMe:setRoleName(L"Local")
				-- Init Equip
				g_PlayerMe:setClass(1)
				g_PlayerMe:setSkeleton()
				g_PlayerMe:setEquip(0,1)
				g_PlayerMe:setEquip(1,1)
				g_PlayerMe:setEquip(2,1)
				g_PlayerMe:setEquip(3,1)
				g_PlayerMe:setEquip(4,1)
				g_PlayerMe:setEquip(6,1)
				g_PlayerMe:setEquipSkin(0,0)
				g_PlayerMe:setEquipSkin(1,1)
				g_PlayerMe:setEquipSkin(2,2)
				g_PlayerMe:setEquipSkin(3,3)
				g_PlayerMe:setEquipSkin(4,0)
				--
				g_PlayerMe:setCellPos(167,127)
				g_PlayerMe:setDir(1)
				g_PlayerMe:setActionState(0)
				g_World:addRole(g_PlayerMe)
				----
				OnEnterWorld()
				return
end
	MessageBox(L"Enter World......",-1)
	EnterWorld(g_UIDisplayCharList:GetSelectIndex())
end

function IDD_ROLE_CHOOSE_OnBtnCreateRole()
	dlgRoleCreate:SetVisible(true)
end