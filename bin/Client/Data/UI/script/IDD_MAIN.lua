-- Create Control



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
	OnChangeMap()
end

function OnChangeMap()
	local szMapID = "MAPID="..g_PlayerMe:getCharacterData().MapNumber
	IDD_MINMAP:getControl("IDC_STA_NAME"):SetText(L(szMapID))
	IDD_MINMAP:getControl("IDC_STA_POSX"):SetText(L(g_PlayerMe:getCharacterData().MapX))
	IDD_MINMAP:getControl("IDC_STA_POSY"):SetText(L(g_PlayerMe:getCharacterData().MapY))
end