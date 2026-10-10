#define ProductName "Modern Metal Bass"
#define ProductVersion "0.4.1"
#define PublisherName "Play Tempo"

[Setup]
AppId={{BF86450E-F021-4388-B9C9-469AE30446AE}
AppName={#ProductName}
AppVersion=0.4.1
AppVerName={#ProductName} {#ProductVersion}
AppPublisher={#PublisherName}
AppCopyright=Copyright (C) 2026 Play Tempo
DefaultDirName={autopf}\Play Tempo\Modern Metal Bass
DefaultGroupName=Play Tempo
DisableProgramGroupPage=yes
OutputDir=..\release-windows
OutputBaseFilename=PlayTempo-ModernMetalBass-0.4.1-Windows-x64
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
UninstallDisplayName=Modern Metal Bass 0.4.1

[Files]
Source: "..\build-win\ModernMetalBass.vst3\*"; DestDir: "{commoncf64}\VST3\ModernMetalBass.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\build-win\installer-docs\readme-win.txt"; DestDir: "{app}"; DestName: "readme.txt"; Flags: isreadme
Source: "changelog.txt"; DestDir: "{app}"; DestName: "CHANGELOG.txt"
Source: "license.txt"; DestDir: "{app}"; DestName: "LICENSE.txt"
Source: "..\THIRD_PARTY-iPlug2-LICENSE.txt"; DestDir: "{app}\licenses"
Source: "..\resources\fonts\LICENSE-Roboto.txt"; DestDir: "{app}\licenses"
Source: "..\build-win\installer-docs\VST3-SDK-MIT.txt"; DestDir: "{app}\licenses"; Flags: skipifsourcedoesntexist

[Icons]
Name: "{group}\Modern Metal Bass README"; Filename: "{app}\README.txt"
Name: "{group}\Uninstall Modern Metal Bass"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\README.txt"; Description: "Open the Modern Metal Bass README"; Flags: postinstall shellexec skipifsilent
