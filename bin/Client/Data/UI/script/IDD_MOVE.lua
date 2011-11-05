-- Create Control
IDD_MOVE_listboxMove = CUIListBox()

-- Register Control
IDD_MOVE:regControl("IDC_LISTBOX_MOVE", IDD_MOVE_listboxMove)

-- Register Event
IDD_MOVE:regControlEvent("IDC_LISTBOX_MOVE","IDD_MOVE_OnBtnOK","1793")

-- Read the MoveMapList in table 'MoveMapList'
MoveMapList = {}

for line in io.lines("Data/move.txt") do
	_, _, name, chat = string.find(line, "([^=]+)=([^=]+)")
	IDD_MOVE_listboxMove:addItem(L(name))
	table.insert(MoveMapList, chat)
end

-- Event Func
function IDD_MOVE_OnBtnOK()
	local serverIndex = IDD_MOVE_listboxMove:getSelectedIndex()
	
	-- no server
	if serverIndex < 0 then
		MessageBox(L"You did not select a map!")
		return
	end
	
	SendChat(MoveMapList[serverIndex+1])

	IDD_MOVE:SetVisible(false)
end

