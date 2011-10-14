; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

[Setup]
AppName=黑暗帝国-OLDVER-to-NEWVER-补丁 
AppVerName=黑暗帝国 OLDVER-to-NEWVER
AppPublisher=初拥网络科技有限公司
AppPublisherURL=http://www.96mmo.com
AppSupportURL=http://www.96mmo.com
AppUpdatesURL=http://www.96mmo.com
DefaultDirName={pf}\黑暗帝国
DefaultGroupName=黑暗帝国
DisableProgramGroupPage=yes
InfoBeforeFile=.\更新内容.txt
OutputDir=.\installer
OutputBaseFilename=黑暗帝国-OLDVER-to-NEWVER-patch 
SetupIconFile=idr_main.ico
Compression=lzma
SolidCompression=yes
DirExistsWarning = no
AppendDefaultDirName = no
Uninstallable = no
disablefinishedpage = yes
WizardImageFile = .\WizModernImage2.bmp
WizardSmallImageFile = .\WizModernSmallImage.bmp
[Languages]
Name: "chinese"; MessagesFile: "compiler:Default.isl"

[Files]
Source: ".\game\GameUpdateHelper.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\update\*"; DestDir: "{app}\update"; Flags: ignoreversion recursesubdirs createallsubdirs
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Run]
Filename: "{app}\GameUpdateHelper.exe"; Description: "{cm:LaunchProgram,黑暗帝国}"; Flags: postinstall skipifsilent ; Parameters:"-helper";WorkingDir: "{app}"; StatusMsg:"正在启动黑暗帝国..."

[Code]
function NextButtonClick(CurPageID: Integer): Boolean;
var
  MyProgChecked: Boolean;
  ClientVersion: String;
  MinVersion:String;
  MaxVersion:String;
begin
  case CurPageID of
    wpSelectDir:
      begin
        MinVersion := 'OLDVER';
        MaxVersion := 'NEWVER';
        
        MyProgChecked := FileExists(WizardDirValue+'\GameUpdate.exe');
        if not MyProgChecked then
        begin
           MsgBox(''''+WizardDirValue + '''不是黑暗帝国游戏安装目录，请重新选择正确的位置.', mbInformation, MB_OK);
           Result := False;
        end
        else
        begin
          ClientVersion := GetIniString('OPTION','CurVer','0',WizardDirValue+'\config.ini');
          if ClientVersion > MaxVersion then
          begin
            MsgBox('您当前版本太高('+ClientVersion+'),不能安装本补丁.', mbInformation, MB_OK);
            Result := False;
          end
          else if ClientVersion < MinVersion then
          begin
            MsgBox('您当前版本太低('+ClientVersion+'),不能安装本补丁.', mbInformation, MB_OK);
            Result := False;
          end
          else
          begin
            Result := True;
          end
        end
      end;
  else
      Result := True;
  end;
end;

