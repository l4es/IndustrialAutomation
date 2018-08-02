
SubSection "Servers and Configurations"

Section "!Data Access"
  SectionIn 1 3

  ;Binary files
  SetOutPath "${INST_BASE}"
  File "${DIR_BIN}\DBAccessor.exe"

  ExecWait "${INST_BASE}\DBAccessor.exe /Service"
SectionEnd

Section "Archiver"
  SectionIn 1 3

  ;Binary files
  SetOutPath "${INST_BASE}"
  File "${DIR_BIN}\ArchiverService.exe"
  
  ExecWait "${INST_BASE}\ArchiverService.exe /Service"
SectionEnd

Section "Symbolic Circuit Designer"
  SectionIn 1 3

	SetShellVarContext all
  CreateDirectory "${SHORTCUT_DIR}"

  ;Binary files
  SetOutPath "${INST_BASE}"
  SetOverwrite ifnewer
  File "${DIR_BIN}\Designer.exe"
  File "${DIR_BIN}\CrashReporter.dll"
  CreateShortCut "${SHORTCUT_DIR}\Symbolic Circuit Designer.lnk" "${INST_BASE}\Designer.exe"

  ;Help files
  SetOutPath "${INST_BASE}\Help"
;  File "${DIR_HELP}\Designer.chm"
SectionEnd

Section "Multi-configurator"
	SectionIn 1 2 3

  SetOutPath "${INST_BASE}"
  SetOverwrite ifnewer
	File "${DIR_BIN}\MultyConfigurator.exe"
	File "${DIR_BIN}\CrashReporter.dll"

  CreateDirectory "${SHORTCUT_DIR}"
  CreateShortCut "${SHORTCUT_DIR}\Multi-configurator.lnk" "${INST_BASE}\MultyConfigurator.exe"
SectionEnd

SubSectionEnd

Section "un.Server"
	IfFileExists "${INST_BASE}\DBAccessor.exe" 0 +3
		ExecWait 'net stop "FreeSCADA Database Accessor"'
  	ExecWait "${INST_BASE}\DBAccessor.exe /UnregServer"

  IfFileExists "${INST_BASE}\ArchiverService.exe" 0 +3
    ExecWait 'net stop "FreeSCADA Archiver"'
    ExecWait "${INST_BASE}\ArchiverService.exe /UnregServer"

	RMDir /r "${INST_BASE}"
	RMDir /r "${SHORTCUT_DIR}"
SectionEnd

Section "un.Multi-configurator"
  RMDir /r "${INST_BASE}"
  RMDir /r "${SHORTCUT_DIR_DOC}"
SectionEnd
