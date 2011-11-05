-- Create Control
btnOK = CUIButton()
staticINFO = CUIStatic()

-- Register Control
IDD_MESSAGE_BOX:regControl("IDC_BTN_OK", btnOK)
IDD_MESSAGE_BOX:regControl("IDC_STATIC_INFO", staticINFO)

-- Register Event
IDD_MESSAGE_BOX:regControlEvent("IDC_BTN_OK","IDD_MESSAGE_BOX_OnBtnExit","0")

-- Event Func
function IDD_MESSAGE_BOX_OnBtnExit()
	IDD_MESSAGE_BOX:SetVisible(false)
end

function CloseMessageBox()
	IDD_MESSAGE_BOX:SetVisible(false)
end

function MessageBox(text,id)
	staticINFO:SetText(text)
	IDD_MESSAGE_BOX:SetVisible(true)
	if id==-1 then
		btnOK:SetVisible(false)
	else
		btnOK:SetVisible(true)
	end
end