set /p version=

svn info svn://192.168.0.12/LianYu/trunk/Code > info.txt

for /f "eol=; tokens=1,2* delims=: " %%i in ('findstr "°æ±¾" info.txt') do set rev=%%j

for /f %%i in (lastver.txt) do set lastrev=%%i

set /a lastrev=%lastrev%+1

svn log  svn://192.168.0.12/LianYu/trunk/Code -r%lastrev%:%rev% > versionlog.txt

echo %rev% > lastver.txt

svn ci -m "make version tags %version%"

svn mkdir svn://192.168.0.12/LianYu/tags/releases/release%version% -F versionlog.txt --force-log
svn cp svn://192.168.0.12/LianYu/trunk/Code svn://192.168.0.12/LianYu/tags/releases/release%version%/ -F versionlog.txt --force-log
