

rem export CURRENT version
@echo off 
set path=%path%;%cd%\..\..\tools;
@echo on

@echo what svn revision you want ?
@set /p SVNVER=

@echo new version(ex. 1.0.5.1) is?
@set /p ver=

rd /s /q game
rd /s /q Server

rem 获取当前GAME_SVNURL
@echo off
svn info -r%SVNVER% > version.txt 
awk "BEGIN { FS=\": \"} /URL/ { print $2 }" version.txt > tmp
sed "s/installer.*/Game/g" tmp > tmp1
for /f %%i in (tmp1) do set GAME_SVNURL=%%i
sed "s/installer.*/Center/g" tmp > tmp2
for /f %%i in (tmp2) do set GAME_CENTER=%%i
sed "s/installer.*/Login/g" tmp > tmp2
for /f %%i in (tmp2) do set GAME_LOGIN=%%i
sed "s/installer.*/Server/g" tmp > tmp3
for /f %%i in (tmp3) do set GAME_SERVER=%%i
sed "s/installer.*/SvrMgrServer/g" tmp > tmp4
for /f %%i in (tmp4) do set GAME_SVRMGRSERVER=%%i
sed "s/installer.*/Vestibule/g" tmp > tmp5
for /f %%i in (tmp5) do set GAME_VESTIBULE=%%i
sed "s/installer.*/sql/g" tmp > tmp6
for /f %%i in (tmp6) do set GAME_SQL=%%i
del tmp
del tmp1
del tmp2
del tmp3
del tmp4
del tmp5
del tmp6
del version.txt
@echo on

svn export %GAME_CENTER% -r%SVNVER% Server/Center
svn info %GAME_CENTER% -r%SVNVER% > Server/Center/Centerversion.txt
svn export %GAME_LOGIN% Server/Login
svn info %GAME_LOGIN% -r%SVNVER% > Server/Login/Loginversion.txt
svn export %GAME_SERVER% Server/Server
svn info %GAME_SERVER% -r%SVNVER% > Server/Server/Serverversion.txt
svn export %GAME_SVRMGRSERVER% Server/SvrMgrServer
svn info %GAME_SVRMGRSERVER% -r%SVNVER% > Server/SvrMgrServer/SvrMgrServerversion.txt
svn export %GAME_VESTIBULE% Server/Vestibule
svn info %GAME_VESTIBULE% -r%SVNVER% > Server/Vestibule/Vestibuleversion.txt
svn export %GAME_SQL% -r%SVNVER% Server/sql
svn info %GAME_SQL% -r%SVNVER% > Server/sql/sqlversion.txt

rem modify LoginVerControl.ini
pushd Server\Login
sed "s/ClientVer *=[0-9 ]*\.[0-9 ]*\.[0-9 ]*\.[0-9 ]*/ClientVer=%ver%/g" LoginVerControl.ini > LoginVerControl.ini.1
del LoginVerControl.ini
ren LoginVerControl.ini.1 LoginVerControl.ini
popd

pushd Server
del /s /q DBEngine.dat
del /s /q tradecityconfig.dlt
7z a -r -tzip GameServer_%ver%.zip .
popd
if not exist installer mkdir installer
copy .\Server\GameServer_%ver%.zip .\installer

svn export %GAME_SVNURL% -r%SVNVER% game
svn info %GAME_SVNURL% -r%SVNVER% > game\version.txt 

pushd game
del /s /q *.lvl
del /s /q *.3sgedit
del /s /q *.max
del /s /q vssver.scc
del /s /q MapConvert.exe
del /s /q Thumbs.db
del /s /q vssver.scc

@for /r %1 %%c in (*.skin *.landscape2 *.ctc) do @( 
 @skin2dds.exe "%%c" )
popd

pushd game\data\scp
del /s /q *.scp
del /s /q *.csv
popd

rem the ./game directory is a clean exported game directory

rem pack data/* except music and scp
@echo now you must pack data/* execpt music ,avi and scp 
@echo compressed file must be MainData.dat in ./game/data/
packfile.exe 

rem cleanup maindata.dat
pushd game\data
rd /s /q changjing
rd /s /q config
rd /s /q guanka
rd /s /q gui
rd /s /q npc
rd /s /q skin
rd /s /q texiao
rd /s /q zhujue
popd

rem pack data/scp -> game.rot
mkdir game\data\game\data\scp
xcopy /s game\data\scp game\data\game\data\scp
pushd game\data
scpconvert game
popd
copy game\data\game.rot game\
del game\data\game.rot
rd /s /q game\data\scp
rd /s /q game\data\game


rem modify vfs.cfg  (/gui/datafiles/fonts -> /data/gui/datafiles/fonts
pushd game
sed "s/gui/data\/gui/g" vfs.cfg > vfs.cfg.1
del vfs.cfg
ren vfs.cfg.1 vfs.cfg
popd

rem modify config.ini (ROT=0 -> ROT=1)
pushd game
sed "s/ROT=0/ROT=1/g" config.ini > config.ini.1
del config.ini
ren config.ini.1 config.ini

sed "s/CurVer *=[0-9 ]*\.[0-9 ]*\.[0-9 ]*\.[0-9 ]*/CurVer=%ver%/g" config.ini > config.ini.1
del config.ini
ren config.ini.1 config.ini
popd


rem prepare setup script
copy lianyu-Setup-template.nsi lianyu-Setup-%ver%.nsi

rem replace CURRENTDIR and GAMEDIR and CUR_RENTVER
sed "s/CURRENTDIR/\./g" lianyu-Setup-%ver%.nsi > tmp
del lianyu-Setup-%ver%.nsi
ren tmp lianyu-Setup-%ver%.nsi
sed "s/GAMEDIR/.\\game/g" lianyu-Setup-%ver%.nsi > tmp
del lianyu-Setup-%ver%.nsi
ren tmp lianyu-Setup-%ver%.nsi
sed "s/CUR_RENTVER/%ver%/g" lianyu-Setup-%ver%.nsi > tmp
del lianyu-Setup-%ver%.nsi
ren tmp lianyu-Setup-%ver%.nsi

makensis.exe lianyu-Setup-%ver%.nsi

rem del lianyu-Setup-%ver%.nsi

