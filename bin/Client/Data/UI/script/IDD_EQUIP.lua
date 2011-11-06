-- Create Control
m_DisplayPlayer = CDisplayModel()

m_ListIcon={}
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()
m_ListIcon[#m_ListIcon+1] = CUIIcon()

-- Register Control
IDD_EQUIP:regControl("IDC_DISPLAY_PLAYER", m_DisplayPlayer)

IDD_EQUIP:regControl("IDC_IMG_HEAD", m_ListIcon[1])
IDD_EQUIP:regControl("IDC_IMG_ARMOR", m_ListIcon[2])
IDD_EQUIP:regControl("IDC_IMG_GLOVE", m_ListIcon[3])
IDD_EQUIP:regControl("IDC_IMG_PANT", m_ListIcon[4])
IDD_EQUIP:regControl("IDC_IMG_BOOT", m_ListIcon[5])
IDD_EQUIP:regControl("IDC_IMG_BACK", m_ListIcon[6])
IDD_EQUIP:regControl("IDC_IMG_WAPEN1", m_ListIcon[7])
IDD_EQUIP:regControl("IDC_IMG_WAPEN2", m_ListIcon[8])

-- Register Event
IDD_EQUIP:regControlEvent("IDC_IMG_HEAD","IDD_EQUIP_OnBtnHead","0")
IDD_EQUIP:regControlEvent("IDC_IMG_ARMOR","IDD_EQUIP_OnBtnArmor","0")
IDD_EQUIP:regControlEvent("IDC_IMG_GLOVE","IDD_EQUIP_OnBtnGlove","0")
IDD_EQUIP:regControlEvent("IDC_IMG_PANT","IDD_EQUIP_OnBtnPant","0")
IDD_EQUIP:regControlEvent("IDC_IMG_BOOT","IDD_EQUIP_OnBtnBoot","0")
IDD_EQUIP:regControlEvent("IDC_IMG_BACK", "IDD_EQUIP_OnBtnBack","0")
IDD_EQUIP:regControlEvent("IDC_IMG_WAPEN1", "IDD_EQUIP_OnBtnRight","0")
IDD_EQUIP:regControlEvent("IDC_IMG_WAPEN2", "IDD_EQUIP_OnBtnLeft","0")

--IDD_EQUIP:regControlEvent("IDC_CLOSE", "IDD_Equip_OnBtnClose","0")

m_PlayerView = CRole()

				m_PlayerView:setClass(1)
				m_PlayerView:setSkeleton()
				m_PlayerView:setEquip(0,1)
				m_PlayerView:setEquip(1,1)
				m_PlayerView:setEquip(2,1)
				m_PlayerView:setEquip(3,1)
				m_PlayerView:setEquip(4,1)
				m_PlayerView:setEquipSkin(0,1)
				m_PlayerView:setEquipSkin(1,1)
				m_PlayerView:setEquipSkin(2,1)
				m_PlayerView:setEquipSkin(3,1)
				m_PlayerView:setEquipSkin(4,1)
				m_PlayerView:setActionState(0);
m_DisplayPlayer:setRenderNode(m_PlayerView)
m_DisplayPlayer.m_vEye=Vec3D(0.0,1.0,-3)
m_DisplayPlayer.m_vLookAt=Vec3D(0.0,1.0,0.0)

function ChangeEquip(id)
	local nIndex = math.random(0,10)
	local nLevel = math.random(0,9)
	local nSkin = 0;
	if nLevel>6 then
		nSkin = 3
	elseif nLevel>5 then
		nSkin = 2
	elseif  nLevel>2 then
		nSkin = 1
	end
	g_PlayerMe:setEquip(id,nIndex)
	g_PlayerMe:setEquipSkin(id,nSkin)
	--
	itemData = ItemData()
	itemData.cType = id+7;
	itemData.cIndex = nIndex;
	itemData.level = nLevel;
	m_ListIcon[id+1]:setItemData(itemData)
end

-- Event Func
function IDD_EQUIP_OnBtnHead()
	ChangeEquip(0)
	local nLevel = math.random(20,50)
	m_PlayerView:setAnim(nLevel)
end
function IDD_EQUIP_OnBtnArmor()
	ChangeEquip(1)
end
function IDD_EQUIP_OnBtnGlove()
	ChangeEquip(2)
end
function IDD_EQUIP_OnBtnPant()
	ChangeEquip(3)
end
function IDD_EQUIP_OnBtnBoot()
	ChangeEquip(4)
end
function IDD_EQUIP_OnBtnBack()
	ChangeEquip(5)
end
function IDD_EQUIP_OnBtnRight()
	ChangeEquip(6)
end
function IDD_EQUIP_OnBtnLeft()
	ChangeEquip(7)
end

function IDD_Equip_OnBtnClose()
	IDD_EQUIP:SetVisible(false)
end