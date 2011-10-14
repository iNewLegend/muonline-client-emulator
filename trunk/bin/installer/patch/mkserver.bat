@echo ÊäÈë°æ±¾ºÅ(Èç1.0.0.0):
@set /p ver=
@set base_url=svn://192.168.0.12/LianYu/tags/releases/release%ver%/Code/Release
@set path=%path%;%cd%\..\..\tools;

svn export %base_url%/svr svr
pushd svr
del /s /q dbengine.dat
popd

7z a -r -tzip svr.%ver%.zip svr 
rd /s /q svr

md5sum svr.%ver%.zip > svr.%ver%.zip.md5.txt

