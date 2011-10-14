
function usage()
	local s = [[
	uasge: lua svnexport.lua diff_file URL export_dir newver oldver
	]]
	print(s)
end

if #arg ~= 5 then 
	usage()
	return
end

local diff_file = arg[1]
local URL = arg[2]
local export_dir = arg[3]
local newver = arg[4]
local oldver = arg[5]

local gmatch = string.gmatch
local gsub = string.gsub

for l in io.lines(diff_file) do		
	l = string.gsub(l, oldver, newver)
	local Dindex= string.find(l, "D+%s+(.+)")
	if Dindex==nil then --排除D开头的，删除的文件
	 	for file_url in gmatch(l, "%a+%s+(.+)") do 
			--替换路径中的 - (减号)					
			local newURL = gsub(URL,"%-","%%-")
			local newURL2 = gsub(newURL,"%.","%%.")
			
			local s = (gsub(file_url, newURL2, export_dir))
			local export_url = s
			
			local pointindex = string.find(s, "(.+)%.(.+)")
			
			if pointindex == nil then
				--dir
				s = gsub(s, "/", "\\")		
				os.execute('if not exist ' .. s .. ' mkdir ' .. s)
			else
				--file
				s = gsub(s, "(.*)/.*", "%1")				
				s = gsub(s, "/", "\\")										
				os.execute('if not exist ' .. s .. ' mkdir ' .. s)
				
				local cmd = 'svn cat "' .. file_url .. '" > "' .. export_url..'"'
				
				print(cmd)
				local code = os.execute(cmd)
				if (code ~= 0) then print(cmd) end
			end		
		end
	end		
end
