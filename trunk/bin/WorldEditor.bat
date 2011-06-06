@echo off 
rd WorldEditor /s /q

md "WorldEditor"
copy client\WorldEditor.exe WorldEditor\WorldEditor.exe
copy client\WorldEditor.cfg WorldEditor\WorldEditor.cfg
copy client\d3dx9_30.dll WorldEditor\d3dx9_30.dll
copy client\lua51.dll WorldEditor\lua51.dll
copy client\lua5.1.dll WorldEditor\lua5.1.dll
copy client\fmod.dll WorldEditor\fmod.dll
copy client\WorldEditorUI.cfg WorldEditor\WorldEditorUI.cfg
copy client\Font.cfg WorldEditor\Font.cfg
copy client\ReadMe.txt WorldEditor\ReadMe.txt

md "WorldEditor\Plugins"
xcopy client\Plugins\*.dll WorldEditor\Plugins\

md "WorldEditor\Plugins\data"
md "WorldEditor\Plugins\data\Default"
md "WorldEditor\Plugins\data\Object1"
md "WorldEditor\Plugins\data\World1"
xcopy client\Plugins\data\Default\*.* WorldEditor\Plugins\data\Default
xcopy client\Plugins\data\Object1\*.* WorldEditor\Plugins\data\Object1
xcopy client\Plugins\data\World1\*.* WorldEditor\Plugins\data\World1

md "WorldEditor\WorldEditorRes"
xcopy client\WorldEditorRes\*.* WorldEditor\WorldEditorRes\
md "WorldEditor\WorldEditorRes\Textures"
xcopy client\WorldEditorRes\Textures\*.* WorldEditor\WorldEditorRes\Textures\

md "WorldEditor\EngineRes"
xcopy client\EngineRes\*.* WorldEditor\EngineRes\
md "WorldEditor\EngineRes\Textures"
xcopy client\EngineRes\Textures\*.* WorldEditor\EngineRes\Textures\

md "WorldEditor\Data"
md "WorldEditor\Data\fx"
xcopy client\Data\fx\*.* WorldEditor\Data\fx

md "WorldEditor\Data\Themes\New"
xcopy client\Data\Themes\New\*.* WorldEditor\Data\Themes\New

md "WorldEditor\Data\Fonts"
copy client\Data\Fonts\fzl2jw.ttf WorldEditor\Data\Fonts\fzl2jw.ttf
