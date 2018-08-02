
Section "-Common"
	SetOutPath "${INST_BASE}"
	SetOverwrite off
	File "${DIR_BIN}\DBAccessorPS.dll"
	RegDll "${INST_BASE}\DBAccessorPS.dll"
      
	;Install and regiter OPC proxies
	SetOutPath "$SYSDIR"
	SetOverwrite ifnewer

	File "${DIR_OPC}\opccomn_ps.dll"
	File "${DIR_OPC}\opcproxy.dll"
	File "${DIR_OPC}\OpcEnum.exe"

	RegDll "$SYSDIR\opccomn_ps.dll"
	RegDll "$SYSDIR\opcproxy.dll"

  ExecWait "$SYSDIR\OpcEnum.exe /Service"
  
  ;Install Run-time libraries
  SetOutPath "${INST_BASE}"
  SetOverwrite ifnewer
	
  File "${DIR_CRT}\Microsoft.VC80.ATL\*.*"
  File /x msvcm80.* "${DIR_CRT}\Microsoft.VC80.CRT\*.*"

  ;Install support libraries
  File "${DIR_3RDPARTY}\dbghelp\bin\*.*"
  File "${DIR_3RDPARTY}\zlib\bin\*.*"
  
SectionEnd

Section "un.Common"
  UnRegDll "${INST_BASE}\DBAccessorPS.dll"
	Delete "${INST_BASE}\DBAccessorPS.dll"
SectionEnd

Section "Documentation"
	SectionIn 1 2 3
  SetOutPath "${INST_DOC}"
  File "${DIR_DOC_ROOT}\whatsnew.txt"
;  File "${DIR_DOC}\*.pdf"
  
  CreateDirectory "${SHORTCUT_DIR_DOC}"
  CreateShortCut "${SHORTCUT_DIR_DOC}\What is new.lnk" "${INST_DOC}\whatsnew.txt"
SectionEnd

Section "un.Documentation"
  RMDir /r "${INST_DOC}"
  RMDir /r "${SHORTCUT_DIR_DOC}"
SectionEnd
