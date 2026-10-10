#define ProductName "DualBassPreamp"
#define ProductVersion "1.0.0"
#define PublisherName "Play Tempo"

[Setup]
AppId={{C1800C73-E684-4B44-953E-832CC59B2136}
AppName={#ProductName}
AppVersion={#ProductVersion}
AppVerName={#ProductName} {#ProductVersion}
AppPublisher={#PublisherName}
AppCopyright=Copyright (C) 2026 Play Tempo
DefaultDirName={autopf}\Play Tempo\DualBassPreamp
DefaultGroupName=Play Tempo
DisableProgramGroupPage=yes
OutputDir=..\release-windows
OutputBaseFilename=PlayTempo-DualBassPreamp-1.0.0-Windows-x64
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
WizardStyle=modern
LicenseFile=license.txt
SetupLogging=yes
CloseApplications=yes
RestartApplications=no
UninstallDisplayName=DualBassPreamp 1.0.0

[Files]
Source: "..\build-win\DualBassPreamp.vst3\*"; DestDir: "{commoncf64}\VST3\DualBassPreamp.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "readme-win.txt"; DestDir: "{app}"; DestName: "README.txt"; Flags: isreadme
Source: "changelog.txt"; DestDir: "{app}"; DestName: "CHANGELOG.txt"
Source: "license.txt"; DestDir: "{app}"; DestName: "LICENSE.txt"

[Icons]
Name: "{group}\DualBassPreamp README"; Filename: "{app}\README.txt"
Name: "{group}\Uninstall DualBassPreamp"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\README.txt"; Description: "Open the DualBassPreamp README"; Flags: postinstall shellexec skipifsilent
