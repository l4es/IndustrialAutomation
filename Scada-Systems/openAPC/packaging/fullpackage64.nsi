!include Library.nsh

RequestExecutionLevel admin

Name "OpenAPC ControlRoom HMI/Process Control Software"
OutFile "ControlRoom.exe"

SetCompress auto
SetDatablockOptimize on
CRCCheck on
; AutoCloseWindow false ; (can be true for the window go away automatically at end)
ShowInstDetails nevershow ; (can be show to have them shown, or nevershow to disable)
; SetDateSave off ; (can be on to have files restored to their orginal date)

LicenseText "Please read the license agreement before installing and using the software:"
LicenseData "LICENSE.txt"

InstallDir "$PROGRAMFILES64\OpenAPC"
# InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\myCompany\myApp" ""
# DirShow show ; (make this hide to not let the user change it)
DirText "Select the directory to install the OpenAPC package into:"

InstProgressFlags smooth
#WindowIcon off

Section "" ; (default section)

SetOutPath "$INSTDIR"

# VS2012
File /r "vcredist110_x64.exe"
# VS2013
File /r "vcredist120_x64.exe"
ExecWait '"$INSTDIR\vcredist110_x64.exe" /q /norestart'
ExecWait '"$INSTDIR\vcredist120_x64.exe" /q /norestart'
Delete "$INSTDIR\vcredist110_x64.exe"
Delete "$INSTDIR\vcredist120_x64.exe"

SetOutPath "$INSTDIR\img\"
File /r "..\img\*.png"

SetOutPath "$INSTDIR\icons\"
File /r "..\icons\*.png"

SetOutPath "$INSTDIR\flowplugins\"
File /r "..\flowplugins_release\*.dll"
SetOutPath "$INSTDIR\flowplugins\espeak-data"
File /r "..\flowplugins_release\espeak-data\"

SetOutPath "$INSTDIR\hmiplugins\"
File /r "..\hmiplugins_release\*.dll"

SetOutPath "$INSTDIR\macros\"
File /r "..\macros\*.apcg"

SetOutPath "$INSTDIR\translations\"
File /r "..\translations\common_*.property"
File /r "..\translations\construct_*.property"
File /r "..\translations\openapc_*.property"
File /r "..\translations\openplayer_*.property"

SetOutPath "$INSTDIR"
File "..\ControlRoom\Release\OpenEditor.exe"
File "..\ControlRoom\Release\OpenDebugger.exe"
File "..\ControlRoom\Release\OpenPlayer.exe"
File "..\ControlRoom\Release\OpenHPlayer.exe"
File "..\ControlRoom\Release\OpenIServer.exe"
File "..\ControlRoom\Release\OpenPlugger.exe"
File "..\ControlRoom\icon.ico"
File "..\ControlRoom\Release\ilPLC.exe"
File "..\ControlRoom\Release\luaPLC.exe"

File "..\Release\liboapc.dll"
File "..\Release\liboapcwx.dll"

File "DLL64\libmysql.dll"

File "..\lua64\lua53.dll"
File "DLL64\inpoutx64.dll"
File "DLL64\portaudio_x64.dll"
File "DLL64\libiconv-2.dll"
File "DLL64\libeay32.dll"
File "DLL64\libintl-8.dll"
File "DLL64\libpq.dll"
File "DLL64\ssleay32.dll"
File "DLL64\zlib1.dll"

File "DLL64\wxbase31u_net_vc_x64_custom.dll"
File "DLL64\wxbase31u_vc_x64_custom.dll"
File "DLL64\wxbase31u_xml_vc_x64_custom.dll"
File "DLL64\wxmsw31u_adv_vc_x64_custom.dll"
File "DLL64\wxmsw31u_aui_vc_x64_custom.dll"
File "DLL64\wxmsw31u_core_vc_x64_custom.dll"
File "DLL64\wxmsw31u_gl_vc_x64_custom.dll"
File "DLL64\wxmsw31u_html_vc_x64_custom.dll"
File "DLL64\wxmsw31u_media_vc_x64_custom.dll"
File "DLL64\wxmsw31u_xrc_vc_x64_custom.dll"

; write out uninstaller
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ControlRoom" "DisplayName" "ControlRoom Package"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ControlRoom" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ControlRoom" "Publisher" "OpenAPC Project Group"

SectionEnd ; end of default section


Section -startmenu

SetShellVarContext all
CreateDirectory "$SMPROGRAMS\OpenAPC"
CreateShortCut "$SMPROGRAMS\OpenAPC\ControlRoom Editor.lnk" "$INSTDIR\OpenEditor.exe" "" "$INSTDIR\icon.ico" 0

Sectionend


; begin uninstall settings/section
UninstallText "This will uninstall the ControlRoom Package from your system"

Section Uninstall

DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ControlRoom"

Delete "$INSTDIR\*.*"
RMDir /r $INSTDIR

SetShellVarContext all
Delete "$SMPROGRAMS\OpenAPC\*.*"
RMDir /r "$SMPROGRAMS\OpenAPC"
RMDir "$SMPROGRAMS\OpenAPC"

SectionEnd ; end of uninstall section

