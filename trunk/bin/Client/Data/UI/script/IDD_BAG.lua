-- Create Control

m_BagListIcon={}

for i=1,64 do
 m_BagListIcon[#m_BagListIcon+1] = CUIIcon()
end

-- Register Control
for i=1,64 do
  string controlName = "IDC_ICON_"+(i-1);
  IDD_BAG:regControl(controlName, m_BagListIcon[i])
end

-- Register Event
for i=1,64 do
  --IDD_BAG:regControlEvent("IDC_ICON_"+(i-1), "IDD_BAG_OnBtnHead", m_BagListIcon[i])
end

-- Event Func
function IDD_BAG_OnBtnHead()

end