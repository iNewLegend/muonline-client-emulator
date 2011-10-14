

require "lfs"

function usage()
	print("usage: lua dirdiff.lua [-fst] [-snd] first_path second_path")
	print("			-first: show files only in first path")
	print("			-second: show files only in second path")
	print("			default: print files only in first path and")
	print("					 files exist in both path but diffrent")
end

function is_exist(f)
	return lfs.attributes(f)
end

function is_same(f1, f2)
	local attr1 = lfs.attributes(f1)
	local attr2 = lfs.attributes(f2)
	if (attr1.modification ~= attr2.modification) then
		return false
	end
	if (attr1.size ~= attr2.size) then
		return false
	end
	return true
end

function add_files_in_dir(dir, tbl)
	for f in lfs.dir(dir) do
		local attr = lfs.attributes(dir .. "/" .. f)
		if attr.mode == "directory" then
			add_files_in_dir(dir .. "\\" .. f)
		else
			table.insert(tbl, dir .. "\\" .. f)
		end
	end
end

function dirdiff(first_path, second_path, file_diff, only_in_first)
	assert(first_path and second_path and file_diff and only_in_first)

	for f_in_first_path in lfs.dir(first_path) do
		if f_in_first_path ~= "." and f_in_first_path ~= ".." then
			local f1 = first_path .. "\\" .. f_in_first_path	
			local f2 = second_path .. "\\" .. f_in_first_path
			local attr = lfs.attributes(f1)
			assert(type(attr) == "table")
			if (attr.mode ~= "directory") then
				--is this file in second directory?
				if is_exist(f2) then
					if not is_same(f1, f2) then
						table.insert(file_diff, f1)
					end
				else
					if (only_in_first) then
						table.insert(only_in_first, f1)
					end
				end
			else
				--is directory
				if not is_exist(f2) then
					add_files_in_dir(f2, only_in_first)
				else
					dirdiff(f1, f2, file_diff, only_in_first)
				end
			end
		end
	end
end

function print_tbl(t)
	for _, v in pairs(t) do print(v) end
end

if (#arg == 2) then
	path_first = arg[1]
	path_second = arg[2]
	local only_in_first = {}
	local file_diff = {}
	dirdiff(path_first, path_second, file_diff, only_in_first)
	print_tbl(only_in_first)
	print_tbl(file_diff)
elseif (#arg == 3) then
	path_first = arg[2]
	path_second = arg[3]
	if arg[1] == "-first" then
		local only_in_first = {}
		dirdiff(path_first, path_second, {}, only_in_first)
		print_tbl(only_in_first)
	elseif arg[1] == "-second" then
		local only_in_second = {}
		dirdiff(path_second, path_first, {}, only_in_second)
		print_tbl(only_in_second)
	end
else
	usage()
end


