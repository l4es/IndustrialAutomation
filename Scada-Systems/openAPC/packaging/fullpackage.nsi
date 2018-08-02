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

InstallDir "$PROGRAMFILES\OpenAPC"
# InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\myCompany\myApp" ""
# DirShow show ; (make this hide to not let the user change it)
DirText "Select the directory to install the OpenAPC package into:"

InstProgressFlags smooth
#WindowIcon off

Section "" ; (default section)

SetOutPath "$INSTDIR"

File /r "vcredist2013_x86.exe"
ExecWait '"$INSTDIR\vcredist2013_x86.exe" /q /norestart'
Delete "$INSTDIR\vcredist2013_x86.exe"

File /r "vcredist2012_x86.exe"
ExecWait '"$INSTDIR\vcredist2012_x86.exe" /q /norestart'
Delete "$INSTDIR\vcredist2012_x86.exe"

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

File "DLL32\libmysql.dll"

File "..\lua\lua53.dll"
File "DLL32\inpout32.dll"
File "DLL32\portaudio_x86.dll"
File "DLL32\libiconv-2.dll"
File "DLL32\libeay32.dll"
File "DLL32\libintl-8.dll"
File "DLL32\libpq.dll"
File "DLL32\ssleay32.dll"
File "DLL32\zlib1.dll"

File "DLL32\wxbase30u_net_vc_custom.dll"
File "DLL32\wxbase30u_vc_custom.dll"
File "DLL32\wxbase30u_xml_vc_custom.dll"
File "DLL32\wxmsw30u_adv_vc_custom.dll"
File "DLL32\wxmsw30u_aui_vc_custom.dll"
File "DLL32\wxmsw30u_core_vc_custom.dll"
File "DLL32\wxmsw30u_gl_vc_custom.dll"
File "DLL32\wxmsw30u_html_vc_custom.dll"
File "DLL32\wxmsw30u_media_vc_custom.dll"
File "DLL32\wxmsw30u_xrc_vc_custom.dll"

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

