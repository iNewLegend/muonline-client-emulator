-- Create Control
m_DisplayPlayer = CDisplayModel()

m_IconHead = CUIIcon()
m_IconArmor = CUIIcon()
m_IconGlove = CUIIcon()
m_IconPant = CUIIcon()
m_IconBoot = CUIIcon()

-- Register Control
IDD_EQUIP:regControl("IDC_DISPLAY_PLAYER", m_DisplayPlayer)

IDD_EQUIP:regControl("IDC_IMG_HEAD", m_IconHead)
IDD_EQUIP:regControl("IDC_IMG_ARMOR", m_IconArmor)
IDD_EQUIP:regControl("IDC_IMG_GLOVE", m_IconGlove)
IDD_EQUIP:regControl("IDC_IMG_PANT", m_IconPant)
IDD_EQUIP:regControl("IDC_IMG_BOOT", m_IconBoot)

-- Register Event
IDD_EQUIP:regControlEvent("IDC_IMG_HEAD","IDD_EQUIP_OnBtnHead",1)
IDD_EQUIP:regControlEvent("IDC_IMG_ARMOR","IDD_EQUIP_OnBtnArmor",1)
IDD_EQUIP:regControlEvent("IDC_IMG_GLOVE","IDD_EQUIP_OnBtnGlove",1)
IDD_EQUIP:regControlEvent("IDC_IMG_PANT","IDD_EQUIP_OnBtnPant",1)
IDD_EQUIP:regControlEvent("IDC_IMG_BOOT","IDD_EQUIP_OnBtnBoot",1)

m_PlayerView = CRole()

				m_PlayerView:setClass(1)
				m_PlayerView:setSkeleton()
				m_PlayerView:setEquip(7,1)
				m_PlayerView:setEquip(8,1)
				m_PlayerView:setEquip(9,1)
				m_PlayerView:setEquip(10,1)
				m_PlayerView:setEquip(11,1)
				m_PlayerView:setEquipSkin(7,1)
				m_PlayerView:setEquipSkin(8,2)
				m_PlayerView:setEquipSkin(9,3)
				m_PlayerView:setEquipSkin(10,4)
				m_PlayerView:setEquipSkin(11,1)
				m_PlayerView:setActionState(0);
m_DisplayPlayer:setRenderNode(m_PlayerView)
m_DisplayPlayer.m_vEye=Vec3D(0.0,1.0,-3)
m_DisplayPlayer.m_vLookAt=Vec3D(0.0,1.0,0.0)

-- Event Func
function IDD_EQUIP_OnBtnHead()
	local nIndex = math.random(0,10)
	local nLevel = math.random(10,30)
	g_PlayerMe:setEquip(7,nIndex)
	g_PlayerMe:setEquipSkin(7,nLevel)
	--
	itemData = ItemData()
	itemData.cType = 7;
	itemData.cIndex = nIndex;

	itemData.level = nLevel;
	m_IconHead:setItemData(itemData)

	m_PlayerView:setAnim(nLevel)
end
function IDD_EQUIP_OnBtnArmor()
	local nIndex = math.random(0,10)
	local nLevel = math.random(30,40)
	g_PlayerMe:setEquip(8,nIndex)
	g_PlayerMe:setEquipSkin(8,nLevel)
	--
	itemData = ItemData()
	itemData.cType = 8;
	itemData.cIndex = nIndex;
	itemData.level = nLevel;
	m_IconArmor:setItemData(itemData)
	m_PlayerView:setAnim(nLevel)
end
function IDD_EQUIP_OnBtnGlove()
	local nIndex = math.random(0,10)
	local nLevel = math.random(0,4)
	g_PlayerMe:setEquip(9,nIndex)
	g_PlayerMe:setEquipSkin(9,nLevel)
	--
	itemData = ItemData()
	itemData.cType = 9;
	itemData.cIndex = nIndex;
	itemData.level = nLevel;
	m_IconGlove:setItemData(itemData)
end
function IDD_EQUIP_OnBtnPant()
	local nIndex = math.random(0,10)
	local nLevel = math.random(0,4)
	g_PlayerMe:setEquip(10,nIndex)
	g_PlayerMe:setEquipSkin(10,nLevel)
	--
	itemData = ItemData()
	itemData.cType = 10;
	itemData.cIndex = nIndex;
	itemData.level = nLevel;
	m_IconPant:setItemData(itemData)
end
function IDD_EQUIP_OnBtnBoot()
	local nType = math.random(0,10)
	local nLevel = math.random(0,4)
	g_PlayerMe:setEquip(11,nType)
	g_PlayerMe:setEquipSkin(11,nLevel)
	--
	itemData = ItemData()
	itemData.cType = 11;
	itemData.cIndex = nIndex;
	itemData.level = nLevel;
	m_IconBoot:setItemData(itemData)
end