-- Create Control
dlgMessageBox = CUIDialog()
dlgMessageBox:create("IDD_MESSAGE_BOX",IDD_MAIN)
dlgMessageBox:SetVisible(false)

dlgRoleChoose = CUIDialog()
dlgRoleChoose:create("IDD_ROLE_CHOOSE",IDD_MAIN)
dlgRoleChoose:SetVisible(false)

dlgLogin = CUIDialog()
dlgLogin:create("IDD_LOGIN",IDD_MAIN)
dlgLogin:SetVisible(false)

dlgServerList = CUIDialog()
dlgServerList:create("IDD_SERVER_LIST",IDD_MAIN)
dlgServerList:SetVisible(true)

dlgMainDialog = CUIDialog()
dlgMainDialog:create("IDD_MAIN_DIALOG",IDD_MAIN)
dlgMainDialog:SetVisible(false)

dlgChatMessage = CUIDialog()
dlgChatMessage:create("IDD_CHAT_MESSAGE",IDD_MAIN)
dlgChatMessage:SetVisible(false)

dlgChat = CUIDialog()
dlgChat:create("IDD_CHAT",IDD_MAIN)
dlgChat:SetVisible(false)

-- Equip
dlgEquip = CUIGeneralDialog()
dlgEquip:create("IDD_EQUIP",IDD_MAIN)
dlgEquip:SetVisible(false)

-- Move Map List
dlgMove = CUIGeneralDialog()
dlgMove:create("IDD_MOVE",IDD_MAIN)
dlgMove:SetVisible(false)

-- Bag
dlgBag = CUIGeneralDialog()
dlgBag:create("IDD_BAG",IDD_MAIN)
dlgBag:SetVisible(false)

-- Player Head
dlgPlayerHead = CUIDialog()
dlgPlayerHead:create("IDD_PLAYERHEAD",IDD_MAIN)
dlgPlayerHead:SetVisible(false)

-- Min Map
dlgMinmap = CUIDialog()
dlgMinmap:create("IDD_MINMAP",IDD_MAIN)
dlgMinmap:SetVisible(false)

-- Act
dlgAct = CUIDialog()
dlgAct:create("IDD_ACT",IDD_MAIN)
dlgAct:SetVisible(false)

-- Setup
dlgSetup = CUIGeneralDialog()
dlgSetup:create("IDD_SETUP",IDD_MAIN)
dlgSetup:SetVisible(false)
-- Register Control

-- Register Event
IDD_MAIN:regEvent("ROLE_CHOOSE","OnRoleChoose")
IDD_MAIN:regEvent("ENTER_WORLD","OnEnterWorld")

-- Event Func
function OnRoleChoose()
	IDD_MAIN:setControlVisible("IDC_IMAGE_BG",false)
	--MessageBox(L"正在获取人物列表……",-1)
	CloseMessageBox()
	IDD_LOGIN:SetVisible(false)
	dlgRoleChoose:SetVisible(true)
	--OnShowRoleChoose()
	g_UIDisplayCharList:LoadModel("data\\logo\\logo05.bmd")
	g_UIDisplayCharList.m_vEye=Vec3D(2.0,2.0,-7.0)
	g_UIDisplayCharList.m_vLookAt=Vec3D(2.0,1.0,-0.0)
	--g_UIDisplayCharList:SetEye(vEye)
	--g_UIDisplayCharList:SetLookAt(vLookAt)
end

function OnEnterWorld()
	CloseMessageBox()
	IDD_MAIN:setControlVisible("IDC_IMAGE_BG",false)
	IDD_SERVER_LIST:SetVisible(false)
	IDD_LOGIN:SetVisible(false)
	dlgRoleChoose:SetVisible(false)
	dlgMainDialog:SetVisible(true)
	IDD_CHAT_MESSAGE:SetVisible(true)
	dlgMinmap:SetVisible(true)
	dlgPlayerHead:SetVisible(true)
	dlgAct:SetVisible(true)
end