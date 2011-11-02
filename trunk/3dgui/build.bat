setlocal
set path=%path%;
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat" || call "C:\Program Files\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
devenv 3DGUI.sln /build release
devenv 3DGUI.sln /build debug
endlocal
