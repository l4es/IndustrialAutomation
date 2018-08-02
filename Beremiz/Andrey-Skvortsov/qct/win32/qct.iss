; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "qct"
#define MyAppVerName "Qct-1.7"
#define MyAppPublisher "Steve Borho"
#define MyAppURL "http://qct.sourceforge.net/"
#define MyAppExeName "qct.exe"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppVerName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName=Qct
AllowNoIcons=yes
LicenseFile=..\COPYING
OutputDir=release
OutputBaseFilename={#MyAppVerName}-standalone-win32
Compression=lzma
SolidCompression=yes

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: ..\dist\qct.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\dist\library.zip; DestDir: {app}; Flags: ignoreversion
Source: ..\dist\*.dll; DestDir: {app}; Flags: ignoreversion
Source: ..\dist\*.pyd; DestDir: {app}; Flags: ignoreversion
Source: ..\dist\w9xpopen.exe; DestDir: {app}; Flags: ignoreversion
;Source: ..\dist\mercurial\*; DestDir: {app}\mercurial; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\hgext\qct.py; DestDir: {app}\mercurial; Flags: ignoreversion
;Source: ..\dist\qct\*; DestDir: {app}\docs; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {#MyAppURL}
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}