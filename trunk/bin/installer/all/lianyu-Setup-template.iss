; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

[Setup]
AppName=黑暗帝国
AppVerName=黑暗帝国 CURRENTVER
AppPublisher=初拥数字互动科技有限公司
AppPublisherURL=http://www.LianYu2008.com
AppSupportURL=http://www.LianYu2008.com
AppUpdatesURL=http://www.LianYu2008.com
DefaultDirName={pf}\黑暗帝国
DefaultGroupName=黑暗帝国
LicenseFile=CURRENTDIR\license.txt
InfoBeforeFile=CURRENTDIR\before.txt
InfoAfterFile=CURRENTDIR\after.txt
OutputDir=CURRENTDIR\installer
OutputBaseFilename=黑暗帝国-CURRENTVER
SetupIconFile=CURRENTDIR\idr_main.ico
Compression=lzma
SolidCompression=yes
WizardImageFile = CURRENTDIR\WizModernImage2.bmp
WizardSmallImageFile = CURRENTDIR\WizModernSmallImage.bmp

[Languages]
Name: "chinese"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "GAMEDIR\GameUpdate.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "GAMEDIR\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
Name: "{group}\黑暗帝国"; Filename: "{app}\GameUpdate.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,黑暗帝国}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\黑暗帝国"; Filename: "{app}\GameUpdate.exe"; Tasks: desktopicon ; WorkingDir: "{app}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\黑暗帝国"; Filename: "{app}\GameUpdate.exe"; Tasks: quicklaunchicon ; WorkingDir: "{app}"

[Run]
Filename: "{app}\GameUpdate.exe"; Description: "{cm:LaunchProgram,黑暗帝国}"; Flags: nowait postinstall skipifsilent;WorkingDir: "{app}"

