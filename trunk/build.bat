pushd 3dgui
call build.bat
popd

setlocal
set path=%path%;
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat" || call "C:\Program Files\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
devenv Client.sln /build release
devenv Client.sln /build debug
endlocal