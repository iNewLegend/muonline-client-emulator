@echo 请先关闭杀毒软件/防火墙等, 否则会浪费太多的时间
@echo 输入制作完整安装包的版本号(如1.0.0.0):
@set /p ver=
@set base_url=https://node3d.googlecode.com/svn/trunk/bin/Client
@set path=%path%;%cd%\..\..\tools;

@echo export game ...
rd /s /q game
svn export %base_url% game

pushd game 
del /s /q *.lvl
del /s /q *.3sgedit
del /s /q *.max
del /s /q vssver.scc
del /s /q Thumbs.db
del /s /q *.pdb
del /s /q *.log
del /s /q *.lib

REM @for /r %1 %%c in (*.skin *.landscape2 *.ctc) do (skin2dds.exe "%%c")

popd

REM pushd game\data\scp
REM for %%i in (*.scp *.csv) do scpconvert %%i
REM del /s /q *.scp *.csv
REM popd

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
cscript saveas.vbs "%cd%\game\data\maindata.dat"

@echo 现在必须等压缩完文件后再关闭，然后按回车键
rem pack file is done
packfinish 2 > packfile.log

pushd game\data
encodepacket maindata.dat
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
rem curver=newver
pushd game
sed "s/ROT=0/ROT=1/g" config.ini > config.ini.1
del config.ini
ren config.ini.1 config.ini
sed "s/CurVer=.*/CurVer=%ver%/g" config.ini > config.ini.1
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

mkdir installer
makensis.exe lianyu-Setup-%ver%.nsi

pushd installer
md5sum 黑暗帝国-%ver%.exe > 黑暗帝国-%ver%.exe.md5.txt
popd

