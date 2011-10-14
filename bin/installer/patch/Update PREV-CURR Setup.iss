; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

[Setup]
AppName=�ڰ��۹�-OLDVER-to-NEWVER-���� 
AppVerName=�ڰ��۹� OLDVER-to-NEWVER
AppPublisher=��ӵ����Ƽ����޹�˾
AppPublisherURL=http://www.96mmo.com
AppSupportURL=http://www.96mmo.com
AppUpdatesURL=http://www.96mmo.com
DefaultDirName={pf}\�ڰ��۹�
DefaultGroupName=�ڰ��۹�
DisableProgramGroupPage=yes
InfoBeforeFile=.\��������.txt
OutputDir=.\installer
OutputBaseFilename=�ڰ��۹�-OLDVER-to-NEWVER-patch 
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
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Run]
Filename: "{app}\GameUpdateHelper.exe"; Description: "{cm:LaunchProgram,�ڰ��۹�}"; Flags: postinstall skipifsilent ; Parameters:"-helper";WorkingDir: "{app}"; StatusMsg:"���������ڰ��۹�..."

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
           MsgBox(''''+WizardDirValue + '''���Ǻڰ��۹���Ϸ��װĿ¼��������ѡ����ȷ��λ��.', mbInformation, MB_OK);
           Result := False;
        end
        else
        begin
          ClientVersion := GetIniString('OPTION','CurVer','0',WizardDirValue+'\config.ini');
          if ClientVersion > MaxVersion then
          begin
            MsgBox('����ǰ�汾̫��('+ClientVersion+'),���ܰ�װ������.', mbInformation, MB_OK);
            Result := False;
          end
          else if ClientVersion < MinVersion then
          begin
            MsgBox('����ǰ�汾̫��('+ClientVersion+'),���ܰ�װ������.', mbInformation, MB_OK);
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

