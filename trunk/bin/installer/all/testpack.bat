@set path=%path%;%cd%\..\..\tools;
@set base_url=svn://192.168.0.12/LianYu/trunk/Code/Release
@echo export game ...
if not exist game svn export %base_url%/Game game 

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

pushd game\data\scp
for %%i in (*.scp *.csv) do scpconvert %%i
del /s /q *.scp *.csv
popd

pushd game\data
del maindata.dat
popd

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
packfinish 2 > packfile.log
pushd game\data\
encodepacket maindata.dat
popd
