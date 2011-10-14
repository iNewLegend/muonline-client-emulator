


local new_patch = 
[[
	Object FILE
	{		
		DES = dsc1
		SRC = game-OLDVER-to-NEWVER-patch.zip
	}
]]
new_patch= string.gsub(new_patch, "OLDVER", arg[2])
new_patch= string.gsub(new_patch, "NEWVER", arg[3])


local fd = assert(io.open(arg[1], 'r'))
local text = fd:read('*a')

text = (string.gsub(text, '(Object UPDATE.*)}', '%1' .. new_patch .. '\n}'))


--replace CurVer = 1.x.x.x with new version
old_ver = 'CurVer = ' .. arg[2]
new_ver = 'CurVer = ' .. arg[3]
text = string.gsub(text, old_ver, new_ver)

fd:close()

fd = assert(io.open(arg[1], 'w+'))
fd:write(text)
fd:close()
