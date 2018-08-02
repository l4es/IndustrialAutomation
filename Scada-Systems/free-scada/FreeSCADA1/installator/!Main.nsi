
!define PRODUCT "Free SCADA"
!define VERSION "0.08b"
!define MUI_PUBLISHER "FreeSCADA Project"
!define MUI_WEB_SITE "http://free-scada.sourceforge.net"

!include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  Name "${PRODUCT}"
  Caption "${PRODUCT} Installation"
;  BGGradient 000000 004080 FFFFFF
  XPStyle on

  OutFile "../bin/distributive/free-scada.exe"

  ;Folder selection page
  InstallDir "$PROGRAMFILES\${PRODUCT}"

  ;Remember install folder
  InstallDirRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" ""

  BrandingText "${MUI_PUBLISHER}"

  !define MUI_ICON   "${NSISDIR}\Contrib\Graphics\Icons\classic-install.ico"
  !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\classic-uninstall.ico"
  !define MUI_ABORTWARNING
;--------------------------------
;Pages
  !define MUI_COMPONENTSPAGE_NODESC

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;Install types
    InstType "Full install"
    InstType "Working station"
    InstType "Server"
;--------------------------------
;DIR Defines
;Folder names for shortcuts
!define SHORTCUT_DIR          "$SMPROGRAMS\${PRODUCT}"
!define SHORTCUT_DIR_DOC      "${SHORTCUT_DIR}\Documentation"

;Source binaries
!define DIR_BIN       "..\bin\release"
!define DIR_OPC       "..\bin\opc"
!define DIR_CRT       "..\bin\redist"
!define DIR_3RDPARTY  "..\source\3rdparty"
!define DIR_DOC_ROOT  "..\documentation"
!define DIR_DOC       "${DIR_DOC_ROOT}\pdf"

;Install folders
!define INST_BASE            "$INSTDIR"
!define INST_DOC             "${INST_BASE}\Documentation"

!define UNINSTALL_ICON "res\uninstall.ico"

!define UPDATE_OS

;Setup modules
!include "IsAdmin.nsi"
!include "GetIEVersion.nsi"
!include "VersionCheck.nsi"

!include "Common.nsi"
!include "WorkStation.nsi"
!include "Server.nsi"

Function .onInit
  Call IsUserAdmin
  Pop $R0
  strcmp $R0 "true" onInitContinue1
  MessageBox MB_OK|MB_ICONSTOP "To continue install you should have Administrator rights."
  Abort

onInitContinue1:
  Call GetIEVersion
  Pop $R0
  
  strcmp $R0 "1" WrongIEVer
  strcmp $R0 "2" WrongIEVer
  strcmp $R0 "3" WrongIEVer
  strcmp $R0 "4" WrongIEVer
  goto onInitContinue2
WrongIEVer:
  MessageBox MB_OK|MB_ICONSTOP "Internet Explorer 5.0+ is required."
  Abort
  
onInitContinue2:
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "myMutex") i .r1 ?e'
  Pop $R0

  StrCmp $R0 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "You cannot run multiple copies of installator simultaneously."
      Abort

FunctionEnd


Section -"uninstaller"
  ;Uninstall icon
  SetOutPath "${INST_BASE}"
  File "${UNINSTALL_ICON}"
  
  ;Create uninstaller
  WriteUninstaller "${INST_BASE}\Uninstall.exe"
  CreateShortCut "${SHORTCUT_DIR}\Remove ${PRODUCT}.lnk" \
                 "${INST_BASE}\Uninstall.exe"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" \
              "DisplayName" "${PRODUCT} (ver. ${VERSION})"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" \
              "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" \
              "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" \
              "DisplayIcon" "${INST_BASE}\mpribor.ico"
SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  SetShellVarContext all

  ;ссылки
  RMDir /r "${SHORTCUT_DIR}"

  ;Удаляем бинарные файлы
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\*.ico"

  ;директории
  RMDir "${INST_DOC}"
  RMDir "${INST_BASE}"

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
SectionEnd
