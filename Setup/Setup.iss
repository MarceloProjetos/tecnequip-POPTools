; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{943E92A3-A70E-415D-B55D-44C8C15FE3F6}
AppName=POPTools
AppVerName=POPTools 2.0
AppPublisher=Tecnequip Tecnologia em Equipamentos Ltda
AppPublisherURL=http://www.tecnequip.com.br/
AppSupportURL=http://www.tecnequip.com.br/
AppUpdatesURL=http://www.tecnequip.com.br/
DefaultDirName={pf}\POPTools
DefaultGroupName=POPTools
OutputDir=..\Setup
OutputBaseFilename=POPTools_Setup_v2.0
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "ptbr"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"; LicenseFile: "..\Setup\EULA_PTBR.txt"
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "..\Setup\EULA_PTBR.txt"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"; LicenseFile: "..\Setup\EULA_ES.txt"

; [Types]
; Name: "full"; Description: "Instala��o Completa"
; Name: "compact"; Description: "Instala��o Compacta"
; Name: "custom"; Description: "Instala��o Personalizada"; Flags: iscustom

; [Components]
; Name:"Principal"; Description:"POPTools vers�o 1.2"; Types: full compact custom; Flags: fixed
; Name:"DriverUSB"; Description:"Driver USB para grava��o da POP-7"; Types: full custom

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "installusbdriver"; Description: "Instalar Driver USB"; GroupDescription: "Drivers:"; Languages: ptbr
Name: "installusbdriver"; Description: "Install USB Driver"; GroupDescription: "Drivers:"; Languages: en
Name: "installusbdriver"; Description: "Instalar Driver USB"; GroupDescription: "Drivers:"; Languages: es
; Name: "restart"; Description: "Reiniciar o computador. A reinicializa��o do computador � necess�ria para o correto funcionamento do compilador que acompanha o programa POP7Tools. (mais informa��es em http://www.yagarto.de)"; Flags: restart

[Files]
Source: "..\bin\POPTools.exe"; DestDir: "{app}"; Languages: ptbr; Flags: ignoreversion
Source: "..\bin\POPTools_en.exe"; DestDir: "{app}"; DestName: "POPTools.exe"; Languages: en; Flags: ignoreversion
Source: "..\bin\POPTools_es.exe"; DestDir: "{app}"; DestName: "POPTools.exe"; Languages: es; Flags: ignoreversion
Source: "..\Help\Ajuda.chm"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Setup\EULA_PTBR.TXT"; DestDir: "{app}"; DestName: "EULA.txt"; Languages: ptbr; Flags: ignoreversion
Source: "..\Setup\EULA_EN.TXT"; DestDir: "{app}"; DestName: "EULA.txt"; Languages: en; Flags: ignoreversion
Source: "..\Setup\EULA_ES.TXT"; DestDir: "{app}"; DestName: "EULA.txt"; Languages: es; Flags: ignoreversion
Source: "..\flashmagicarmcortex.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\src\*.h"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\*.a"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\*.ld"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\*.o"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\src\compile"; DestDir: "{app}\src"; Permissions: users-modify; Flags: ignoreversion
Source: "..\bin\gcc\*.h"; DestDir: "{app}\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\gcc\*.a"; DestDir: "{app}\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\gcc\*.o"; DestDir: "{app}\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\gcc\*.dll"; DestDir: "{app}\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\gcc\*.exe"; DestDir: "{app}\gcc"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "..\bin\src\lwip-1.4.0\*.h"; DestDir: "{app}\src\lwip-1.4.0"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "..\bin\src\lwip-1.4.0\*.a"; DestDir: "{app}\src\lwip-1.4.0"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "..\bin\src\lwip-1.4.0\*.o"; DestDir: "{app}\src\lwip-1.4.0"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "..\bin\src\lwip-1.4.0\*.dll"; DestDir: "{app}\src\lwip-1.4.0"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "..\bin\src\lwip-1.4.0\*.exe"; DestDir: "{app}\src\lwip-1.4.0"; Permissions: users-modify; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\examples\*"; DestDir: "{app}\examples"; Permissions: users-readexec; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\Setup\CP210x_VCP_Win_XP_S2K3_Vista_7.exe"; DestDir: "{app}"; Flags: ignoreversion
; Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup\yagarto-bu-2.20.1_gcc-4.5.0-c-c++_nl-1.18.0_gdb-7.1_eabi_20100501.exe"; DestDir: "{app}"; Flags: ignoreversion
; Source: "C:\Users\Tecnequip\Documents\Visual Studio 2010\Projects\POPTools\Setup\yagarto-tools-20100703-setup.exe"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\POPTools"; Filename: "{app}\POPTools.exe"
Name: "{group}\Manual do Usuario"; Filename: "{app}\Ajuda.chm"
Name: "{group}\Licen�a de Uso"; Filename: "{app}\EULA.TXT"
Name: "{commondesktop}\POPTools"; Filename: "{app}\POPTools.exe"; Tasks: desktopicon

[Registry]
; Root: HKLM; Subkey: "System\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "Path"; ValueData: "{reg:HKLM\System\CurrentControlSet\Control\Session Manager\Environment,Path};c:\yagarto;c:\yagarto_tools";
Root: HKCR; Subkey: ".ld"; ValueType: string; ValueName: ""; ValueData: "POPToolsFiles"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".bld"; ValueType: string; ValueName: ""; ValueData: "POPToolsFiles"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "POPToolsFiles"; ValueType: string; ValueName: ""; ValueData: "POPTools Files"; Flags: uninsdeletekey
Root: HKCR; Subkey: "POPToolsFiles\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\POPTools.exe,0"
Root: HKCR; Subkey: "POPToolsFiles\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\POPTools.exe"" ""%1"""

[Run]
; Filename: "{app}\yagarto-tools-20100703-setup.exe"; Parameters: "/S /D={pf}\yagarto"; Description: "Ferramentas de Compila��o"; Flags: runascurrentuser
; Filename: "{app}\yagarto-bu-2.20.1_gcc-4.5.0-c-c++_nl-1.18.0_gdb-7.1_eabi_20100501.exe"; Parameters: "/S /D={pf}\yagarto"; Description: "Compilador e Linker"; Flags: runascurrentuser
Filename: "{app}\CP210x_VCP_Win_XP_S2K3_Vista_7.exe"; StatusMsg: "Aguardando finalizar instala��o do Driver USB"; Tasks: installusbdriver
Filename: "{app}\POPTools.exe"; Description: "{cm:LaunchProgram,POPTools}"; Flags: nowait postinstall skipifsilent


