-- Create Control
m_DisplayModel = CDisplayModel()
m_StaProInfo = CUIStatic()
m_EdtName = CUIEditBox()

IDD_ROLE_CREATE_Index = 0
-- Register Control
IDD_ROLE_CREATE:regControl("IDC_DISPLAY_ROLE", m_DisplayModel)
IDD_ROLE_CREATE:regControl("IDC_STA_INFO", m_StaProInfo )
IDD_ROLE_CREATE:regControl("IDC_EDT_NAME", m_EdtName )

-- Register Event
IDD_ROLE_CREATE:regControlEvent("IDC_BTN_OK", "IDD_ROLE_CREATE_OnBtnOk",1)
IDD_ROLE_CREATE:regControlEvent("IDC_BTN_EXIT", "IDD_ROLE_CREATE_OnBtnExit",1)
--IDD_ROLE_CREATE:regControlEvent("IDC_BTN_RESET", "IDD_ROLE_CREATE_OnBtnReset",1)

IDD_ROLE_CREATE:regControlEvent("IDC_BTN_ROLE1", "IDD_ROLE_CREATE_OnBtnRole1",1)
IDD_ROLE_CREATE:regControlEvent("IDC_BTN_ROLE2", "IDD_ROLE_CREATE_OnBtnRole2",1)
IDD_ROLE_CREATE:regControlEvent("IDC_BTN_ROLE3", "IDD_ROLE_CREATE_OnBtnRole3",1)

-- Event Func
function IDD_ROLE_CREATE_OnBtnOk()
	if string.len(m_EdtName:GetText())<4 then
		MessageBox(L"名字太短，大于4位！")
		return
	end
	RoleCreate(ws2s(m_EdtName:GetText()),IDD_ROLE_CREATE_Index)
	IDD_ROLE_CREATE:SetVisible(false)
end

function IDD_ROLE_CREATE_OnBtnExit()
	IDD_ROLE_CREATE:SetVisible(false)
end

function IDD_ROLE_CREATE_OnBtnRole1()
	m_DisplayModel:LoadModel("data\\logo\\NewFace01.bmd")
	m_DisplayModel.m_vEye=Vec3D(0.05,0.08,-0.15)
	m_DisplayModel.m_vLookAt=Vec3D(0.0,0.08,0.0)
	m_StaProInfo:SetText(L"shit1")
	IDD_ROLE_CREATE_Index = 0
end

function IDD_ROLE_CREATE_OnBtnRole2()
	m_DisplayModel:LoadModel("data\\logo\\NewFace02.bmd")
	m_DisplayModel.m_vEye=Vec3D(0.05,0.08,-0.15)
	m_DisplayModel.m_vLookAt=Vec3D(0.0,0.08,0.0)
	m_StaProInfo:SetText(L"shit2")
	IDD_ROLE_CREATE_Index = 1
end

function IDD_ROLE_CREATE_OnBtnRole3()
	m_DisplayModel:LoadModel("data\\logo\\NewFace03.bmd")
	m_DisplayModel.m_vEye=Vec3D(0.0,0.07,-0.1)
	m_DisplayModel.m_vLookAt=Vec3D(0.0,0.07,0.0)
	m_StaProInfo:SetText(L"shit3")
	IDD_ROLE_CREATE_Index = 2
end