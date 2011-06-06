-- Create Control

-- Register Control

-- Register Event
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_CHAR","IDD_MAIN_DIALOG_OnBtnCharInfo",1)
IDD_MAIN_DIALOG:regControlEvent("IDC_MAIN_MOVE","IDD_MAIN_DIALOG_OnBtnMove",1)

-- Event Func
function IDD_MAIN_DIALOG_OnBtnCharInfo()
	local bVisible = IDD_CHAR_INFO:IsVisible();
	if bVisible then
		bVisible=false
	else
		bVisible=true
	end
	IDD_CHAR_INFO:SetVisible(bVisible)
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