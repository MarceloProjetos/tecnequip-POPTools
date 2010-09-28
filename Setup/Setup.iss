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
OutputDir=..\Setup
OutputBaseFilename=POPTools_Setup
Compression=lzma
SolidCompression=yes
LicenseFile=..\Setup\EULA.TXT
ChangesAssociations=yes

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
; Name: "restart"; Description: "Reiniciar o computador. A reinicializa��o do computador � necess�ria para o correto funcionamento do compilador que acompanha o programa POP7Tools. (mais informa��es em http://www.yagarto.de)"; Flags: restart

[Files]
Source: "..\bin\POPTools.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\flashmagicarmcortex.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\src\*.h"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\*.a"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\*.ld"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\*.o"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\compile"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\gcc\*.h"; DestDir: "{app}\src\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\gcc\*.a"; DestDir: "{app}\src\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\gcc\*.o"; DestDir: "{app}\src\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\gcc\*.dll"; DestDir: "{app}\src\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\gcc\*.exe"; DestDir: "{app}\src\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\lwip\*.h"; DestDir: "{app}\src\lwip"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\lwip\*.a"; DestDir: "{app}\src\lwip"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\lwip\*.o"; DestDir: "{app}\src\lwip"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\lwip\*.dll"; DestDir: "{app}\src\lwip"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\src\lwip\*.exe"; DestDir: "{app}\src\lwip"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\examples\*"; DestDir: "{app}\examples"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\Setup\CP210x_VCP_Win_XP_S2K3_Vista_7.exe"; DestDir: "{app}\usb_driver"; Flags: ignoreversion
; Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup\yagarto-bu-2.20.1_gcc-4.5.0-c-c++_nl-1.18.0_gdb-7.1_eabi_20100501.exe"; DestDir: "{app}"; Flags: ignoreversion
; Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup\yagarto-tools-20100703-setup.exe"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\POPTools"; Filename: "{app}\POPTools.exe"
Name: "{commondesktop}\POPTools"; Filename: "{app}\POPTools.exe"; Tasks: desktopicon

[Registry]
; Root: HKLM; Subkey: "System\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "Path"; ValueData: "{reg:HKLM\System\CurrentControlSet\Control\Session Manager\Environment,Path};c:\yagarto;c:\yagarto_tools";
Root: HKCR; Subkey: ".ld"; ValueType: string; ValueName: ""; ValueData: "POPToolsFiles"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "POPToolsFiles"; ValueType: string; ValueName: ""; ValueData: "POPTools Files"; Flags: uninsdeletekey
Root: HKCR; Subkey: "POPToolsFiles\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\POPTools.exe,0"
Root: HKCR; Subkey: "POPToolsFiles\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\POPTools.exe"" ""%1"""

[Run]
; Filename: "{app}\yagarto-tools-20100703-setup.exe"; Parameters: "/S /D={pf}\yagarto"; Description: "Ferramentas de Compila��o"; Flags: runascurrentuser
; Filename: "{app}\yagarto-bu-2.20.1_gcc-4.5.0-c-c++_nl-1.18.0_gdb-7.1_eabi_20100501.exe"; Parameters: "/S /D={pf}\yagarto"; Description: "Compilador e Linker"; Flags: runascurrentuser
Filename: "{app}\POPTools.exe"; Description: "{cm:LaunchProgram,POPTools}"; Flags: nowait postinstall skipifsilent


