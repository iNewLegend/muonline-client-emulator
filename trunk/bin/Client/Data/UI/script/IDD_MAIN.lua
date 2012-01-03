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
IDD_MAIN:setControlVisible("IDC_IMAGE_BG",false)

	--g_World:create(77)
	--g_PlayerMe:setCellPos(229,152)
	
function OnServerList()
	CloseMessageBox()
	IDD_MAIN:setControlVisible("IDC_IMAGE_BG",true)
	IDD_SERVER_LIST:SetVisible(true)
	IDD_LOGIN:SetVisible(false)
	dlgRoleChoose:SetVisible(false)
	dlgMainDialog:SetVisible(false)
	IDD_CHAT_MESSAGE:SetVisible(false)
	dlgMinmap:SetVisible(false)
	dlgPlayerHead:SetVisible(false)
	dlgAct:SetVisible(false)
end

function OnRoleChoose()
	CloseMessageBox()
	IDD_MAIN:setControlVisible("IDC_IMAGE_BG",false)
	IDD_SERVER_LIST:SetVisible(false)
	IDD_LOGIN:SetVisible(false)
	dlgRoleChoose:SetVisible(true)
	--OnShowRoleChoose()
	bg = CSkeletonNode()
	bg:setFilename("data\\logo\\logo05.bmd")
	g_World:addChild(bg)
	g_Camera:setRadius(7)
	g_Camera:setYawAngle(0)
	g_Camera:setPitchAngle(0)
	g_PlayerMe:setCellPos(2,0)
	--g_UIDisplayCharList.m_vEye=Vec3D(2.0,2.0,-7.0)
	--g_UIDisplayCharList.m_vLookAt=Vec3D(2.0,1.0,-0.0)
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
	OnChangeMap()
end

function OnChangeMap()
	local szMapID = "MAPID="..g_PlayerMe:getCharacterData().MapNumber
	IDD_MINMAP:getControl("IDC_STA_NAME"):SetText(L(szMapID))
	IDD_MINMAP:getControl("IDC_STA_POSX"):SetText(L(g_PlayerMe:getCharacterData().MapX))
	IDD_MINMAP:getControl("IDC_STA_POSY"):SetText(L(g_PlayerMe:getCharacterData().MapY))
end