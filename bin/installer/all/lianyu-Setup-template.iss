; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

[Setup]
AppName=�ڰ��۹�
AppVerName=�ڰ��۹� CURRENTVER
AppPublisher=��ӵ���ֻ����Ƽ����޹�˾
AppPublisherURL=http://www.LianYu2008.com
AppSupportURL=http://www.LianYu2008.com
AppUpdatesURL=http://www.LianYu2008.com
DefaultDirName={pf}\�ڰ��۹�
DefaultGroupName=�ڰ��۹�
LicenseFile=CURRENTDIR\license.txt
InfoBeforeFile=CURRENTDIR\before.txt
InfoAfterFile=CURRENTDIR\after.txt
OutputDir=CURRENTDIR\installer
OutputBaseFilename=�ڰ��۹�-CURRENTVER
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
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
Name: "{group}\�ڰ��۹�"; Filename: "{app}\GameUpdate.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,�ڰ��۹�}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\�ڰ��۹�"; Filename: "{app}\GameUpdate.exe"; Tasks: desktopicon ; WorkingDir: "{app}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\�ڰ��۹�"; Filename: "{app}\GameUpdate.exe"; Tasks: quicklaunchicon ; WorkingDir: "{app}"

[Run]
Filename: "{app}\GameUpdate.exe"; Description: "{cm:LaunchProgram,�ڰ��۹�}"; Flags: nowait postinstall skipifsilent;WorkingDir: "{app}"

