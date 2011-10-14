@for /r %1 %%c in (*.skin *.landscape2 *.ctc) do @(
 @echo %%c
 @skin2dds.exe "%%c" )

    