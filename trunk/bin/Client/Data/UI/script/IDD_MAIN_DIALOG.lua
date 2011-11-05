-- Create Control

-- Register Control

-- Register Event
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_CHAR","IDD_MAIN_DIALOG_OnBtnCharInfo","0")
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_BAG","IDD_MAIN_DIALOG_OnBtnBag","0")
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_MOVE","IDD_MAIN_DIALOG_OnBtnMove","0")
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_SETUP","IDD_MAIN_DIALOG_OnBtnSetup","0")

-- Event Func
function IDD_MAIN_DIALOG_OnBtnCharInfo()
	IDD_EQUIP:SetVisible(not IDD_EQUIP:IsVisible())
end

function IDD_MAIN_DIALOG_OnBtnMove()
	IDD_MOVE:SetVisible(not IDD_MOVE:IsVisible())
end

function IDD_MAIN_DIALOG_OnBtnBag()
	IDD_BAG:SetVisible(not IDD_BAG:IsVisible())
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

	IDD_SETUP:SetVisible(not IDD_SETUP:IsVisible());
end
