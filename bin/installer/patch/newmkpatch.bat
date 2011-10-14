@echo 请先关闭杀毒软件/防火墙等, 否则会浪费很多时间
@set path=%path%;%cd%\..\..\tools;

@echo 输入老的版本号(ex: 1.0.0.0)
@set /p oldver=
@echo 输入新的版本号(ex: 1.0.0.1)
@set /p newver=
@set old_base_url=svn://192.168.0.12/LianYu/tags/releases/release%oldver%/Code/Release
@set new_base_url=svn://192.168.0.12/LianYu/tags/releases/release%newver%/Code/Release
rd /s /q game

svn diff %old_base_url%/Game %new_base_url%/Game --summarize > diff.txt
lua5.1 svnexport.lua diff.txt %new_base_url%/Game game %newver% %oldver% 

pushd game 
del /s /q *.lvl
del /s /q *.3sgedit
del /s /q *.max
del /s /q vssver.scc
del /s /q Thumbs.db
del /s /q *.pdb
del /s /q *.lib 
del /s /q *.log
popd

@rem 必须取一下client目录:( (for skinconvert.ext)
rd /s /q client
REM svn co %new_base_url%/Game client

REM if exist game (
REM copy skin2dds.exe game\
REM pushd game 
REM SkinConvert.exe "..\client" 
REM del skin2dds.exe
REM popd
REM )

rem del game\data\scp
rd /s /q game\data\scp

rem make sure config.ini and vfs.cfg doesn't exist
del game\vfs.cfg
del game\config.ini


rem pack data/* except music and scp
@echo now you must pack data/* execpt music and scp 
@echo compressed file must be maindata.dat in ./game/data/
start packfile.exe 
dragdrop "%cd%\game\data\changjing"
dragdrop "%cd%\game\data\config"
dragdrop "%cd%\game\data\guanka"
dragdrop "%cd%\game\data\gui"
dragdrop "%cd%\game\data\npc"
dragdrop "%cd%\game\data\skin"
dragdrop "%cd%\game\data\texiao"
dragdrop "%cd%\game\data\zhujue"
rem make update-%oldver%-to-%newver%-patch.dat
cscript saveas.vbs "%cd%\game\data\update-%oldver%-to-%newver%-patch.dat"

@echo 现在必须等压缩完文件后再关闭，然后按回车键
rem pack file is done
packfinish 2 > packfile.log

pushd game\data
encodepacket update-%oldver%-to-%newver%-patch.dat
popd

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

rem pack game.rot (carefully, must be whole scp directory)
mkdir game\data\game\data\scp
svn export %new_base_url%/Game/data/scp game/data/game/data/scp --force
pushd game\data
scpconvert game
popd
copy game\data\game.rot game\
del game\data\game.rot
rd /s /q game\data\scp
rd /s /q game\data\game

if not exist installer mkdir installer

rem make auto update patch 
pushd game
7z a -r -tzip game-%oldver%-to-%newver%-patch.zip .
popd
copy .\game\game-%oldver%-to-%newver%-patch.zip .\installer\


rem make self install patch
copy "Update PREV-CURR Setup.iss" "Update %oldver%-%newver% Setup.iss"
sed "s/OLDVER/%oldver%/g" "Update %oldver%-%newver% Setup.iss" > tmp
del "Update %oldver%-%newver% Setup.iss"
ren tmp "Update %oldver%-%newver% Setup.iss"
sed "s/NEWVER/%newver%/g" "Update %oldver%-%newver% Setup.iss" > tmp
del "Update %oldver%-%newver% Setup.iss"
ren tmp "Update %oldver%-%newver% Setup.iss"

rem copy new\GameUpdate.exe .\game\GameUpdateHelper.exe
svn cat %new_base_url%/Game/GameUpdate.exe > game\GameUpdateHelper.exe
mkdir update
copy game\game-%oldver%-to-%newver%-patch.zip update\
mkdir installer
iscc "Update %oldver%-%newver% Setup.iss" || pause 
rd /s /q update

pushd installer
md5sum 黑暗帝国-%oldver%-to-%newver%-patch.exe > 黑暗帝国-%oldver%-to-%newver%-patch.exe.md5.txt
popd
