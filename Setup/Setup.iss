; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{943E92A3-A70E-415D-B55D-44C8C15FE3F6}
AppName=POPTools
AppVerName=POPTools 1.0
AppPublisher=Tecnequip Tecnologia em Equipamentos Ltda
AppPublisherURL=http://www.tecnequip.com.br/
AppSupportURL=http://www.tecnequip.com.br/
AppUpdatesURL=http://www.tecnequip.com.br/
DefaultDirName={pf}\POPTools
DefaultGroupName=POPTools
OutputDir=C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup
OutputBaseFilename=POPTools_Setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
; Name: "restart"; Description: "Reiniciar o computador. A reinicializa��o do computador � necess�ria para o correto funcionamento do compilador que acompanha o programa POP7Tools. (mais informa��es em http://www.yagarto.de)"; Flags: restart

[Files]
Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Release\POPTools.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Release\src\*"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\examples\*"; DestDir: "{app}\examples"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup\yagarto-bu-2.20.1_gcc-4.5.0-c-c++_nl-1.18.0_gdb-7.1_eabi_20100501.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup\yagarto-tools-20100703-setup.exe"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\POPTools"; Filename: "{app}\POPTools.exe"
Name: "{commondesktop}\POPTools"; Filename: "{app}\POPTools.exe"; Tasks: desktopicon

[Registry]
; Root: HKLM; Subkey: "System\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "Path"; ValueData: "{reg:HKLM\System\CurrentControlSet\Control\Session Manager\Environment,Path};c:\yagarto;c:\yagarto_tools";

[Run]
Filename: "{app}\yagarto-tools-20100703-setup.exe"; Parameters: "/S /D={pf}\yagarto"; Description: "Ferramentas de Compila��o"; Flags: runascurrentuser
Filename: "{app}\yagarto-bu-2.20.1_gcc-4.5.0-c-c++_nl-1.18.0_gdb-7.1_eabi_20100501.exe"; Parameters: "/S /D={pf}\yagarto"; Description: "Compilador e Linker"; Flags: runascurrentuser
Filename: "{app}\POPTools.exe"; Description: "{cm:LaunchProgram,POPTools}"; Flags: nowait postinstall skipifsilent


