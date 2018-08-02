
SubSection "Work Station"
Section "Visualizer"
  SectionIn 1 2

	SetShellVarContext all
  CreateDirectory "${SHORTCUT_DIR}"

  ;Binary files
  SetOutPath "${INST_BASE}"
  File "${DIR_BIN}\Visualizator.exe"
  File "${DIR_BIN}\CrashReporter.dll"
  CreateShortCut "${SHORTCUT_DIR}\Visualizer.lnk" "${INST_BASE}\Visualizator.exe"

  ;Help files
  SetOutPath "${INST_BASE}\Help"
;  File "${DIR_HELP}\Help\Visualizator.chm"
SectionEnd

Section "Report Generator"
  SectionIn 1 2

	SetShellVarContext all
  CreateDirectory "${SHORTCUT_DIR}"

  ;Binary files
  SetOutPath "${INST_BASE}"
  File "${DIR_BIN}\ReportGenerator.exe"
  CreateShortCut "${SHORTCUT_DIR}\Report Generator.lnk" "${INST_BASE}\ReportGenerator.exe"

  ;Help files
  SetOutPath "${INST_BASE}\Help"
;  File "${DIR_HELP}\Help\Visualizator.chm"
SectionEnd
SubSectionEnd

Section "un.Work Station"
	RMDir /r "${INST_BASE}"
	RMDir /r "${SHORTCUT_DIR}"
SectionEnd