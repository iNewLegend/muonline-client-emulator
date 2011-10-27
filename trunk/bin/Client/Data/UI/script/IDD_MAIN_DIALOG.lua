-- Create Control

-- Register Control

-- Register Event
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_CHAR","IDD_MAIN_DIALOG_OnBtnCharInfo",1)
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_BAG","IDD_MAIN_DIALOG_OnBtnBag",1)
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_MOVE","IDD_MAIN_DIALOG_OnBtnMove",1)
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_SETUP","IDD_MAIN_DIALOG_OnBtnSetup",1)

-- Event Func
function IDD_MAIN_DIALOG_OnBtnCharInfo()
	local bVisible = IDD_EQUIP:IsVisible();
	if bVisible then
		bVisible=false
	else
		bVisible=true
	end
	IDD_EQUIP:SetVisible(bVisible)
end

function IDD_MAIN_DIALOG_OnBtnMove()
	local bVisible = IDD_MOVE:IsVisible();
	if bVisible then
		bVisible=false
	else
		bVisible=true
	end
	IDD_MOVE:SetVisible(bVisible)
end

function IDD_MAIN_DIALOG_OnBtnBag()
	local bVisible = IDD_BAG:IsVisible();
	if bVisible then
		bVisible=false
	else
		bVisible=true
	end
	IDD_BAG:SetVisible(bVisible)
end

function IDD_MAIN_DIALOG_OnBtnSetup()
	monster = CMonster()
	monster:setID(math.random(0,100));
	monster:setType(math.random(0,100));
	local x = g_PlayerMe:getCellPosX()+math.random(-5,5);
	local y = g_PlayerMe:getCellPosY()+math.random(-5,5);
	monster:setCellPos(x, y);
	monster:setDir(math.random(0,7));
	g_World:addRole(monster)
end
