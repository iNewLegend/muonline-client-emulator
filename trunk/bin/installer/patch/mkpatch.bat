
@echo what svn revision you want patch oldsvnver(ex. 108)?
@set /p oldsvnver=
@echo what svn revision you want patch newsvnver(ex. 120)?
@set /p newsvnver=

@echo old version(ex. 1.0.5.1) is?
@set /p oldver=
@echo new version(ex. 1.0.5.2) is?
@set /p newver=

@echo off 
set path=%path%;%cd%\..\..\tools;
@echo on

rd /s /q game

@echo off 
rem 获取当前GAME_SVNURL
svn info -r%newsvnver% > version.txt 
awk "BEGIN { FS=\": \"} /URL/ { print $2 }" version.txt > tmp
sed "s/installer.*/Game/g" tmp > tmp1
for /f %%i in (tmp1) do set GAME_SVNURL=%%i
sed "s/installer.*/Server/g" tmp > tmp2
for /f %%i in (tmp2) do set SEVER_SVNURL=%%i
sed "s/installer.*/Center/g" tmp > tmp3
for /f %%i in (tmp3) do set CENTER_SVNURL=%%i
sed "s/installer.*/Login/g" tmp > tmp4
for /f %%i in (tmp4) do set LOGIN_SVNURL=%%i
sed "s/installer.*/Vestibule/g" tmp > tmp5
for /f %%i in (tmp5) do set VESTIBULE_SVNURL=%%i
sed "s/installer.*/SvrMgrServer/g" tmp > tmp6
for /f %%i in (tmp6) do set SVRMGRSERVER_SVNURL=%%i
del tmp
del tmp1
del tmp2
del tmp3
del tmp4
del tmp5
del tmp6
del version.txt
@echo on

svn diff %GAME_SVNURL% -r%oldsvnver%:%newsvnver% --summarize > game_diff.txt
lua5.1 svnexport.lua game_diff.txt %GAME_SVNURL% game %newsvnver%
del game_diff.txt
svn info %GAME_SVNURL% -r%newsvnver% > game\version.txt

if exist game (
copy skin2dds.exe game\
pushd game 
del /s /q *.lvl
del /s /q *.3sgedit
del /s /q *.max
del /s /q vssver.scc
del /s /q MapConvert.exe
del /s /q Thumbs.db
del /s /q vssver.scc
SkinConvert.exe "..\..\..\game" 
del skin2dds.exe
popd
)

rem del game/data/scp *.scp *.csv
if exist game/data/scp (
	pushd game\data\scp
	del /s /q *.scp
	del /s /q *.csv
	popd
)

rem make sure config.ini and vfs.cfg doesn't exist
del game\vfs.cfg
del game\config.ini

rem make update-%oldver%-to-%newver%-patch.dat

PackFile.exe

if exist game/data/changjing rd /s /q game\data\changjing
if exist game/data/config rd /s /q game\data\config
if exist game/data/guanka rd /s /q game\data\guanka
if exist game/data/gui rd /s /q game\data\gui
if exist game/data/npc rd /s /q game\data\npc
if exist game/data/skin rd /s /q game\data\skin
if exist game/data/texiao rd /s /q game\data\texiao
if exist game/data/zhujue rd /s /q game\data\zhujue

if exist game/data/scp (
	rem pack game.rot (carefully, must be whole scp directory)
	mkdir game\data\game\data\scp	
	svn export %GAME_SVNURL%/data/scp game/data/game/data/scp --force
	pushd game\data
	scpconvert game
	popd
	copy game\data\game.rot game\
	del game\data\game.rot
	rd /s /q game\data\scp
	rd /s /q game\data\game
)

if not exist installer mkdir installer

rem make auto update patch 
if exist game (
pushd game
7z a -r -tzip game-%oldver%-to-%newver%-patch.zip .
popd
copy .\game\game-%oldver%-to-%newver%-patch.zip .\installer
)

rem make self install patch
copy "Update PREV-CURR Setup.iss" "Update %oldver%-%newver% Setup.iss"
sed "s/OLDVER/%oldver%/g" "Update %oldver%-%newver% Setup.iss" > tmp
del "Update %oldver%-%newver% Setup.iss"
ren tmp "Update %oldver%-%newver% Setup.iss"
sed "s/NEWVER/%newver%/g" "Update %oldver%-%newver% Setup.iss" > tmp
del "Update %oldver%-%newver% Setup.iss"
ren tmp "Update %oldver%-%newver% Setup.iss"

rem copy new\GameUpdate.exe .\game\GameUpdateHelper.exe
svn cat -r%newsvnver% %GAME_SVNURL%/GameUpdate.exe > game\GameUpdateHelper.exe
mkdir update
copy game\game-%oldver%-to-%newver%-patch.zip update\
iscc "Update %oldver%-%newver% Setup.iss" || pause 
rd /s /q update
rd /s /q game
del "Update %oldver%-%newver% Setup.iss"

rem make serverver.zip
lua5.1 mkserverver.lua serverver.scp %oldver% %newver% 
scpconvert serverver.scp
7z a -tzip serverver.zip serverver.dat
copy serverver.zip .\installer
del serverver.zip
del serverver.dat

rem server patch
svn diff %SEVER_SVNURL% -r%oldsvnver%:%newsvnver% --summarize > server_diff.txt
lua5.1 svnexport.lua server_diff.txt %SEVER_SVNURL% Server %newsvnver%
del server_diff.txt

if exist Server (
pushd Server
del /s /q tradecityconfig.dlt
7z a -r -tzip Server-%oldver%-to-%newver%-patch.zip .
popd
copy .\Server\Server-%oldver%-to-%newver%-patch.zip .\installer
rd /s /q Server
)

svn diff %CENTER_SVNURL% -r%oldsvnver%:%newsvnver% --summarize > center_diff.txt
lua5.1 svnexport.lua center_diff.txt %CENTER_SVNURL% Center %newsvnver%
del center_diff.txt

if exist Center (
pushd Center
7z a -r -tzip Center-%oldver%-to-%newver%-patch.zip .
popd
copy .\Center\Center-%oldver%-to-%newver%-patch.zip .\installer
rd /s /q Center
)

svn diff %LOGIN_SVNURL% -r%oldsvnver%:%newsvnver% --summarize > login_diff.txt
lua5.1 svnexport.lua login_diff.txt %LOGIN_SVNURL% Login %newsvnver%
del login_diff.txt

rem modify LoginVerControl.ini
if not exist Login mkdir Login
if not exist Login/LoginVerControl.ini (
svn export %LOGIN_SVNURL%/LoginVerControl.ini Login/LoginVerControl.ini
)
pushd Login
sed "s/ClientVer *=[0-9 ]*\.[0-9 ]*\.[0-9 ]*\.[0-9 ]*/ClientVer=%newver%/g" LoginVerControl.ini > LoginVerControl.ini.1
del LoginVerControl.ini
ren LoginVerControl.ini.1 LoginVerControl.ini
popd

if exist Login (
pushd Login
7z a -r -tzip Login-%oldver%-to-%newver%-patch.zip .
popd
copy .\Login\Login-%oldver%-to-%newver%-patch.zip .\installer
rd /s /q Login
)

svn diff %VESTIBULE_SVNURL% -r%oldsvnver%:%newsvnver% --summarize > vestibule_diff.txt
lua5.1 svnexport.lua vestibule_diff.txt %VESTIBULE_SVNURL% Vestibule %newsvnver%
del vestibule_diff.txt

if exist Vestibule (
pushd Vestibule
7z a -r -tzip Vestibule-%oldver%-to-%newver%-patch.zip .
popd
copy .\Vestibule\Vestibule-%oldver%-to-%newver%-patch.zip .\installer
rd /s /q Vestibule
)

svn diff %SVRMGRSERVER_SVNURL% -r%oldsvnver%:%newsvnver% --summarize > SvrMgrServer_diff.txt
lua5.1 svnexport.lua SvrMgrServer_diff.txt %SVRMGRSERVER_SVNURL% SvrMgrServer %newsvnver%
del SvrMgrServer_diff.txt

if exist SvrMgrServer (
pushd SvrMgrServer
7z a -r -tzip SvrMgrServer-%oldver%-to-%newver%-patch.zip .
popd
copy .\SvrMgrServer\SvrMgrServer-%oldver%-to-%newver%-patch.zip .\installer
rd /s /q SvrMgrServer
)

copy .\"更新内容.txt" .\installer\"更新内容 %oldver%-%newver%.txt"

rem now we can cleanup old new and game directory

mkdir .svn\tmp
svn cleanup

