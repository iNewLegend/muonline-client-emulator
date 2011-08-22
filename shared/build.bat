setlocal
set path=%path%;
call "C:\Program Files\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
devenv Shared.sln /build release
endlocal
