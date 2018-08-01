
SetCompressor /SOLID /FINAL lzma
SetDatablockOptimize off

!include MUI2.nsh

; MUI Settings
!define MUI_ICON "build\beremiz\images\brz.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" ; optional
!define MUI_ABORTWARNING

; Documentation
!insertmacro MUI_PAGE_WELCOME
!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "build/license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Name "Beremiz $BVERSION"
OutFile "Beremiz-$BVERSION.exe"
InstallDir "$PROGRAMFILES\Beremiz"
!define PYTHONW_EXE "$INSTDIR\python\pythonw.exe"
!define BEREMIZ_EXE '"$INSTDIR\beremiz\Beremiz.py" -u "http://www.beremiz.org/updateinfo/$BVERSION/" $BEXTENSIONS'

Section "Beremiz" 
  SetOutPath $INSTDIR
  File /r /x debian /x *.pyc "build/*"
SectionEnd

Section "Examples" 
  CreateDirectory "$DESKTOP\BeremizExamples"
  SetOutPath "$DESKTOP\BeremizExamples"
  File /r "examples/*"
  CreateShortCut "$DESKTOP\BeremizExamples\canopen_tests\CAN_TCP_Server.lnk" "$INSTDIR\CanFestival-3\drivers\can_tcp_win32\can_tcp_win32_server.exe" "" "" 0 SW_SHOWNORMAL "" "Simple CAN emulation over TCP (for CANopen testing)"
SectionEnd

Section "Install"
  ;Store installation folder
  WriteRegStr HKCU "Software\Beremiz" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Beremiz" "Contact" "edouard.tisserant@gmail.com"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Beremiz" "DisplayName" "Beremiz"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Beremiz" "Publisher" "Beremiz"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Beremiz" "URLInfoAbout" "http://www.beremiz.org"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Beremiz" "UninstallString" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\Beremiz"
  SetOutPath "$INSTDIR\mingw\bin"
  CreateShortCut "$SMPROGRAMS\Beremiz\PlcopenEditor.lnk" "${PYTHONW_EXE}" '"$INSTDIR\beremiz\plcopeneditor.py"' "$INSTDIR\beremiz\images\poe.ico"
  CreateShortCut "$SMPROGRAMS\Beremiz\Beremiz.lnk" "${PYTHONW_EXE}" '${BEREMIZ_EXE}' "$INSTDIR\beremiz\images\brz.ico"
  CreateShortCut "$SMPROGRAMS\Beremiz\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  SetShellVarContext current
  CreateShortCut "$DESKTOP\Beremiz.lnk" "${PYTHONW_EXE}" '${BEREMIZ_EXE}' "$INSTDIR\beremiz\images\brz.ico"
; Profiling :
;  CreateShortCut "$DESKTOP\BeremizPro.lnk" "${PYTHONW_EXE}" '-m cProfile -o %USERPROFILE%/stats.pstat ${BEREMIZ_EXE}' "$INSTDIR\beremiz\images\brz.ico"
SectionEnd

Section "Uninstall"
  SetShellVarContext all
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$SMPROGRAMS\Beremiz\PlcopenEditor.lnk"
  Delete "$SMPROGRAMS\Beremiz\Beremiz.lnk"
  RMDir /R "$SMPROGRAMS\Beremiz"
  RMDir /R "$INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\Beremiz"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Beremiz"
  SetShellVarContext current
  Delete "$DESKTOP\Beremiz.lnk"
SectionEnd
