set /p version=
svn mkdir svn://192.168.0.12/LianYu/tags/releases/release%version% -m "make release %version% dir for test"
svn cp svn://192.168.0.12/LianYu/trunk/Code svn://192.168.0.12/LianYu/tags/releases/release%version%/ -m "make tags %version% for test"

