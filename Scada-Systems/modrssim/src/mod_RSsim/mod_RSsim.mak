# Microsoft Developer Studio Generated NMAKE File, Based on mod_RSsim.dsp
!IF "$(CFG)" == ""
CFG=mod_RSsim - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mod_RSsim - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mod_RSsim - Win32 Release" && "$(CFG)" != "mod_RSsim - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mod_RSsim.mak" CFG="mod_RSsim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mod_RSsim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mod_RSsim - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mod_RSsim - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mod_RSsim.exe" "$(OUTDIR)\mod_RSsim.tlb" "$(OUTDIR)\mod_RSsim.bsc"

!ELSE 

ALL : "MYLIB - Win32 Release" "$(OUTDIR)\mod_RSsim.exe" "$(OUTDIR)\mod_RSsim.tlb" "$(OUTDIR)\mod_RSsim.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"MYLIB - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\_README.OBJ"
	-@erase "$(INTDIR)\_README.SBR"
	-@erase "$(INTDIR)\ABComms.obj"
	-@erase "$(INTDIR)\ABComms.sbr"
	-@erase "$(INTDIR)\ABCommsProcessor.obj"
	-@erase "$(INTDIR)\ABCommsProcessor.sbr"
	-@erase "$(INTDIR)\About.obj"
	-@erase "$(INTDIR)\About.sbr"
	-@erase "$(INTDIR)\ActiveScriptHost.obj"
	-@erase "$(INTDIR)\ActiveScriptHost.sbr"
	-@erase "$(INTDIR)\AdvEmulationDlg.obj"
	-@erase "$(INTDIR)\AdvEmulationDlg.sbr"
	-@erase "$(INTDIR)\AnimationSettings.obj"
	-@erase "$(INTDIR)\AnimationSettings.sbr"
	-@erase "$(INTDIR)\CreditStatic.obj"
	-@erase "$(INTDIR)\CreditStatic.sbr"
	-@erase "$(INTDIR)\DDKSocket.obj"
	-@erase "$(INTDIR)\DDKSocket.sbr"
	-@erase "$(INTDIR)\DDKSrvSocket.obj"
	-@erase "$(INTDIR)\DDKSrvSocket.sbr"
	-@erase "$(INTDIR)\EasterDlg.obj"
	-@erase "$(INTDIR)\EasterDlg.sbr"
	-@erase "$(INTDIR)\EditMemoryDlg.obj"
	-@erase "$(INTDIR)\EditMemoryDlg.sbr"
	-@erase "$(INTDIR)\FaultsDlg.obj"
	-@erase "$(INTDIR)\FaultsDlg.sbr"
	-@erase "$(INTDIR)\JoySCCEmulation.obj"
	-@erase "$(INTDIR)\JoySCCEmulation.sbr"
	-@erase "$(INTDIR)\MemoryEditorList.obj"
	-@erase "$(INTDIR)\MemoryEditorList.sbr"
	-@erase "$(INTDIR)\MemWriteLock.obj"
	-@erase "$(INTDIR)\MemWriteLock.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\MOD232CommsProcessor.obj"
	-@erase "$(INTDIR)\MOD232CommsProcessor.sbr"
	-@erase "$(INTDIR)\mod_RSsim.obj"
	-@erase "$(INTDIR)\mod_RSsim.pch"
	-@erase "$(INTDIR)\mod_RSsim.res"
	-@erase "$(INTDIR)\mod_RSsim.sbr"
	-@erase "$(INTDIR)\mod_RSsim.tlb"
	-@erase "$(INTDIR)\mod_RSsimDlg.obj"
	-@erase "$(INTDIR)\mod_RSsimDlg.sbr"
	-@erase "$(INTDIR)\MODEthCommsProcessor.obj"
	-@erase "$(INTDIR)\MODEthCommsProcessor.sbr"
	-@erase "$(INTDIR)\MyHostProxy.obj"
	-@erase "$(INTDIR)\MyHostProxy.sbr"
	-@erase "$(INTDIR)\PLCApplication.obj"
	-@erase "$(INTDIR)\PLCApplication.sbr"
	-@erase "$(INTDIR)\RS232Noise.obj"
	-@erase "$(INTDIR)\RS232Noise.sbr"
	-@erase "$(INTDIR)\RS232Port.obj"
	-@erase "$(INTDIR)\RS232Port.sbr"
	-@erase "$(INTDIR)\serverRS232array.obj"
	-@erase "$(INTDIR)\serverRS232array.sbr"
	-@erase "$(INTDIR)\ServerSettings232Dlg.obj"
	-@erase "$(INTDIR)\ServerSettings232Dlg.sbr"
	-@erase "$(INTDIR)\ServerSettingsEthDlg.obj"
	-@erase "$(INTDIR)\ServerSettingsEthDlg.sbr"
	-@erase "$(INTDIR)\ServerSocketArray.obj"
	-@erase "$(INTDIR)\ServerSocketArray.sbr"
	-@erase "$(INTDIR)\simport.obj"
	-@erase "$(INTDIR)\simport.sbr"
	-@erase "$(INTDIR)\Splash.obj"
	-@erase "$(INTDIR)\Splash.sbr"
	-@erase "$(INTDIR)\StarWarsCtrl.obj"
	-@erase "$(INTDIR)\StarWarsCtrl.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TrainingSimDlg.obj"
	-@erase "$(INTDIR)\TrainingSimDlg.sbr"
	-@erase "$(INTDIR)\UTIL.OBJ"
	-@erase "$(INTDIR)\UTIL.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mod_RSsim.bsc"
	-@erase "$(OUTDIR)\mod_RSsim.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mod_RSsim.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mod_RSsim.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_RSsim.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\_README.SBR" \
	"$(INTDIR)\ABComms.sbr" \
	"$(INTDIR)\ABCommsProcessor.sbr" \
	"$(INTDIR)\About.sbr" \
	"$(INTDIR)\ActiveScriptHost.sbr" \
	"$(INTDIR)\AdvEmulationDlg.sbr" \
	"$(INTDIR)\AnimationSettings.sbr" \
	"$(INTDIR)\CreditStatic.sbr" \
	"$(INTDIR)\DDKSocket.sbr" \
	"$(INTDIR)\DDKSrvSocket.sbr" \
	"$(INTDIR)\EasterDlg.sbr" \
	"$(INTDIR)\EditMemoryDlg.sbr" \
	"$(INTDIR)\FaultsDlg.sbr" \
	"$(INTDIR)\JoySCCEmulation.sbr" \
	"$(INTDIR)\MemoryEditorList.sbr" \
	"$(INTDIR)\MemWriteLock.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\MOD232CommsProcessor.sbr" \
	"$(INTDIR)\mod_RSsim.sbr" \
	"$(INTDIR)\mod_RSsimDlg.sbr" \
	"$(INTDIR)\MODEthCommsProcessor.sbr" \
	"$(INTDIR)\MyHostProxy.sbr" \
	"$(INTDIR)\PLCApplication.sbr" \
	"$(INTDIR)\RS232Noise.sbr" \
	"$(INTDIR)\RS232Port.sbr" \
	"$(INTDIR)\serverRS232array.sbr" \
	"$(INTDIR)\ServerSettings232Dlg.sbr" \
	"$(INTDIR)\ServerSettingsEthDlg.sbr" \
	"$(INTDIR)\ServerSocketArray.sbr" \
	"$(INTDIR)\simport.sbr" \
	"$(INTDIR)\Splash.sbr" \
	"$(INTDIR)\StarWarsCtrl.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TrainingSimDlg.sbr" \
	"$(INTDIR)\UTIL.SBR"

"$(OUTDIR)\mod_RSsim.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wsock32.lib ..\..\mylib\release\mylib.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\mod_RSsim.pdb" /machine:I386 /out:"$(OUTDIR)\mod_RSsim.exe" 
LINK32_OBJS= \
	"$(INTDIR)\_README.OBJ" \
	"$(INTDIR)\ABComms.obj" \
	"$(INTDIR)\ABCommsProcessor.obj" \
	"$(INTDIR)\About.obj" \
	"$(INTDIR)\ActiveScriptHost.obj" \
	"$(INTDIR)\AdvEmulationDlg.obj" \
	"$(INTDIR)\AnimationSettings.obj" \
	"$(INTDIR)\CreditStatic.obj" \
	"$(INTDIR)\DDKSocket.obj" \
	"$(INTDIR)\DDKSrvSocket.obj" \
	"$(INTDIR)\EasterDlg.obj" \
	"$(INTDIR)\EditMemoryDlg.obj" \
	"$(INTDIR)\FaultsDlg.obj" \
	"$(INTDIR)\JoySCCEmulation.obj" \
	"$(INTDIR)\MemoryEditorList.obj" \
	"$(INTDIR)\MemWriteLock.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\MOD232CommsProcessor.obj" \
	"$(INTDIR)\mod_RSsim.obj" \
	"$(INTDIR)\mod_RSsimDlg.obj" \
	"$(INTDIR)\MODEthCommsProcessor.obj" \
	"$(INTDIR)\MyHostProxy.obj" \
	"$(INTDIR)\PLCApplication.obj" \
	"$(INTDIR)\RS232Noise.obj" \
	"$(INTDIR)\RS232Port.obj" \
	"$(INTDIR)\serverRS232array.obj" \
	"$(INTDIR)\ServerSettings232Dlg.obj" \
	"$(INTDIR)\ServerSettingsEthDlg.obj" \
	"$(INTDIR)\ServerSocketArray.obj" \
	"$(INTDIR)\simport.obj" \
	"$(INTDIR)\Splash.obj" \
	"$(INTDIR)\StarWarsCtrl.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TrainingSimDlg.obj" \
	"$(INTDIR)\UTIL.OBJ" \
	"$(INTDIR)\mod_RSsim.res" \
	"..\..\MYLIB\Release\MYLIB.lib"

"$(OUTDIR)\mod_RSsim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mod_RSsim - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mod_RSsim.exe" "$(OUTDIR)\mod_RSsim.tlb" "$(OUTDIR)\mod_RSsim.bsc"

!ELSE 

ALL : "MYLIB - Win32 Debug" "$(OUTDIR)\mod_RSsim.exe" "$(OUTDIR)\mod_RSsim.tlb" "$(OUTDIR)\mod_RSsim.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"MYLIB - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\_README.OBJ"
	-@erase "$(INTDIR)\_README.SBR"
	-@erase "$(INTDIR)\ABComms.obj"
	-@erase "$(INTDIR)\ABComms.sbr"
	-@erase "$(INTDIR)\ABCommsProcessor.obj"
	-@erase "$(INTDIR)\ABCommsProcessor.sbr"
	-@erase "$(INTDIR)\About.obj"
	-@erase "$(INTDIR)\About.sbr"
	-@erase "$(INTDIR)\ActiveScriptHost.obj"
	-@erase "$(INTDIR)\ActiveScriptHost.sbr"
	-@erase "$(INTDIR)\AdvEmulationDlg.obj"
	-@erase "$(INTDIR)\AdvEmulationDlg.sbr"
	-@erase "$(INTDIR)\AnimationSettings.obj"
	-@erase "$(INTDIR)\AnimationSettings.sbr"
	-@erase "$(INTDIR)\CreditStatic.obj"
	-@erase "$(INTDIR)\CreditStatic.sbr"
	-@erase "$(INTDIR)\DDKSocket.obj"
	-@erase "$(INTDIR)\DDKSocket.sbr"
	-@erase "$(INTDIR)\DDKSrvSocket.obj"
	-@erase "$(INTDIR)\DDKSrvSocket.sbr"
	-@erase "$(INTDIR)\EasterDlg.obj"
	-@erase "$(INTDIR)\EasterDlg.sbr"
	-@erase "$(INTDIR)\EditMemoryDlg.obj"
	-@erase "$(INTDIR)\EditMemoryDlg.sbr"
	-@erase "$(INTDIR)\FaultsDlg.obj"
	-@erase "$(INTDIR)\FaultsDlg.sbr"
	-@erase "$(INTDIR)\JoySCCEmulation.obj"
	-@erase "$(INTDIR)\JoySCCEmulation.sbr"
	-@erase "$(INTDIR)\MemoryEditorList.obj"
	-@erase "$(INTDIR)\MemoryEditorList.sbr"
	-@erase "$(INTDIR)\MemWriteLock.obj"
	-@erase "$(INTDIR)\MemWriteLock.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\MOD232CommsProcessor.obj"
	-@erase "$(INTDIR)\MOD232CommsProcessor.sbr"
	-@erase "$(INTDIR)\mod_RSsim.obj"
	-@erase "$(INTDIR)\mod_RSsim.pch"
	-@erase "$(INTDIR)\mod_RSsim.res"
	-@erase "$(INTDIR)\mod_RSsim.sbr"
	-@erase "$(INTDIR)\mod_RSsim.tlb"
	-@erase "$(INTDIR)\mod_RSsimDlg.obj"
	-@erase "$(INTDIR)\mod_RSsimDlg.sbr"
	-@erase "$(INTDIR)\MODEthCommsProcessor.obj"
	-@erase "$(INTDIR)\MODEthCommsProcessor.sbr"
	-@erase "$(INTDIR)\MyHostProxy.obj"
	-@erase "$(INTDIR)\MyHostProxy.sbr"
	-@erase "$(INTDIR)\PLCApplication.obj"
	-@erase "$(INTDIR)\PLCApplication.sbr"
	-@erase "$(INTDIR)\RS232Noise.obj"
	-@erase "$(INTDIR)\RS232Noise.sbr"
	-@erase "$(INTDIR)\RS232Port.obj"
	-@erase "$(INTDIR)\RS232Port.sbr"
	-@erase "$(INTDIR)\serverRS232array.obj"
	-@erase "$(INTDIR)\serverRS232array.sbr"
	-@erase "$(INTDIR)\ServerSettings232Dlg.obj"
	-@erase "$(INTDIR)\ServerSettings232Dlg.sbr"
	-@erase "$(INTDIR)\ServerSettingsEthDlg.obj"
	-@erase "$(INTDIR)\ServerSettingsEthDlg.sbr"
	-@erase "$(INTDIR)\ServerSocketArray.obj"
	-@erase "$(INTDIR)\ServerSocketArray.sbr"
	-@erase "$(INTDIR)\simport.obj"
	-@erase "$(INTDIR)\simport.sbr"
	-@erase "$(INTDIR)\Splash.obj"
	-@erase "$(INTDIR)\Splash.sbr"
	-@erase "$(INTDIR)\StarWarsCtrl.obj"
	-@erase "$(INTDIR)\StarWarsCtrl.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TrainingSimDlg.obj"
	-@erase "$(INTDIR)\TrainingSimDlg.sbr"
	-@erase "$(INTDIR)\UTIL.OBJ"
	-@erase "$(INTDIR)\UTIL.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mod_RSsim.bsc"
	-@erase "$(OUTDIR)\mod_RSsim.exe"
	-@erase "$(OUTDIR)\mod_RSsim.ilk"
	-@erase "$(OUTDIR)\mod_RSsim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_COMMS_DEBUGGING" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mod_RSsim.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mod_RSsim.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_RSsim.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\_README.SBR" \
	"$(INTDIR)\ABComms.sbr" \
	"$(INTDIR)\ABCommsProcessor.sbr" \
	"$(INTDIR)\About.sbr" \
	"$(INTDIR)\ActiveScriptHost.sbr" \
	"$(INTDIR)\AdvEmulationDlg.sbr" \
	"$(INTDIR)\AnimationSettings.sbr" \
	"$(INTDIR)\CreditStatic.sbr" \
	"$(INTDIR)\DDKSocket.sbr" \
	"$(INTDIR)\DDKSrvSocket.sbr" \
	"$(INTDIR)\EasterDlg.sbr" \
	"$(INTDIR)\EditMemoryDlg.sbr" \
	"$(INTDIR)\FaultsDlg.sbr" \
	"$(INTDIR)\JoySCCEmulation.sbr" \
	"$(INTDIR)\MemoryEditorList.sbr" \
	"$(INTDIR)\MemWriteLock.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\MOD232CommsProcessor.sbr" \
	"$(INTDIR)\mod_RSsim.sbr" \
	"$(INTDIR)\mod_RSsimDlg.sbr" \
	"$(INTDIR)\MODEthCommsProcessor.sbr" \
	"$(INTDIR)\MyHostProxy.sbr" \
	"$(INTDIR)\PLCApplication.sbr" \
	"$(INTDIR)\RS232Noise.sbr" \
	"$(INTDIR)\RS232Port.sbr" \
	"$(INTDIR)\serverRS232array.sbr" \
	"$(INTDIR)\ServerSettings232Dlg.sbr" \
	"$(INTDIR)\ServerSettingsEthDlg.sbr" \
	"$(INTDIR)\ServerSocketArray.sbr" \
	"$(INTDIR)\simport.sbr" \
	"$(INTDIR)\Splash.sbr" \
	"$(INTDIR)\StarWarsCtrl.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TrainingSimDlg.sbr" \
	"$(INTDIR)\UTIL.SBR"

"$(OUTDIR)\mod_RSsim.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wsock32.lib ..\..\mylib\debug\mylib.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\mod_RSsim.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mod_RSsim.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\_README.OBJ" \
	"$(INTDIR)\ABComms.obj" \
	"$(INTDIR)\ABCommsProcessor.obj" \
	"$(INTDIR)\About.obj" \
	"$(INTDIR)\ActiveScriptHost.obj" \
	"$(INTDIR)\AdvEmulationDlg.obj" \
	"$(INTDIR)\AnimationSettings.obj" \
	"$(INTDIR)\CreditStatic.obj" \
	"$(INTDIR)\DDKSocket.obj" \
	"$(INTDIR)\DDKSrvSocket.obj" \
	"$(INTDIR)\EasterDlg.obj" \
	"$(INTDIR)\EditMemoryDlg.obj" \
	"$(INTDIR)\FaultsDlg.obj" \
	"$(INTDIR)\JoySCCEmulation.obj" \
	"$(INTDIR)\MemoryEditorList.obj" \
	"$(INTDIR)\MemWriteLock.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\MOD232CommsProcessor.obj" \
	"$(INTDIR)\mod_RSsim.obj" \
	"$(INTDIR)\mod_RSsimDlg.obj" \
	"$(INTDIR)\MODEthCommsProcessor.obj" \
	"$(INTDIR)\MyHostProxy.obj" \
	"$(INTDIR)\PLCApplication.obj" \
	"$(INTDIR)\RS232Noise.obj" \
	"$(INTDIR)\RS232Port.obj" \
	"$(INTDIR)\serverRS232array.obj" \
	"$(INTDIR)\ServerSettings232Dlg.obj" \
	"$(INTDIR)\ServerSettingsEthDlg.obj" \
	"$(INTDIR)\ServerSocketArray.obj" \
	"$(INTDIR)\simport.obj" \
	"$(INTDIR)\Splash.obj" \
	"$(INTDIR)\StarWarsCtrl.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TrainingSimDlg.obj" \
	"$(INTDIR)\UTIL.OBJ" \
	"$(INTDIR)\mod_RSsim.res" \
	"..\..\MYLIB\Debug\MYLIB.lib"

"$(OUTDIR)\mod_RSsim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mod_RSsim.dep")
!INCLUDE "mod_RSsim.dep"
!ELSE 
!MESSAGE Warning: cannot find "mod_RSsim.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mod_RSsim - Win32 Release" || "$(CFG)" == "mod_RSsim - Win32 Debug"
SOURCE=.\_README.CPP

"$(INTDIR)\_README.OBJ"	"$(INTDIR)\_README.SBR" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\ABComms.cpp

"$(INTDIR)\ABComms.obj"	"$(INTDIR)\ABComms.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\ABCommsProcessor.cpp

"$(INTDIR)\ABCommsProcessor.obj"	"$(INTDIR)\ABCommsProcessor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\About.cpp

"$(INTDIR)\About.obj"	"$(INTDIR)\About.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\ActiveScriptHost.cpp

"$(INTDIR)\ActiveScriptHost.obj"	"$(INTDIR)\ActiveScriptHost.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\AdvEmulationDlg.cpp

"$(INTDIR)\AdvEmulationDlg.obj"	"$(INTDIR)\AdvEmulationDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\AnimationSettings.cpp

"$(INTDIR)\AnimationSettings.obj"	"$(INTDIR)\AnimationSettings.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\CreditStatic.cpp

"$(INTDIR)\CreditStatic.obj"	"$(INTDIR)\CreditStatic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\DDKSocket.cpp

"$(INTDIR)\DDKSocket.obj"	"$(INTDIR)\DDKSocket.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\DDKSrvSocket.cpp

"$(INTDIR)\DDKSrvSocket.obj"	"$(INTDIR)\DDKSrvSocket.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\EasterDlg.cpp

"$(INTDIR)\EasterDlg.obj"	"$(INTDIR)\EasterDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\EditMemoryDlg.cpp

"$(INTDIR)\EditMemoryDlg.obj"	"$(INTDIR)\EditMemoryDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\FaultsDlg.cpp

"$(INTDIR)\FaultsDlg.obj"	"$(INTDIR)\FaultsDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\JoySCCEmulation.cpp

"$(INTDIR)\JoySCCEmulation.obj"	"$(INTDIR)\JoySCCEmulation.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\MemoryEditorList.cpp

"$(INTDIR)\MemoryEditorList.obj"	"$(INTDIR)\MemoryEditorList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\MemWriteLock.cpp

"$(INTDIR)\MemWriteLock.obj"	"$(INTDIR)\MemWriteLock.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\message.cpp

"$(INTDIR)\message.obj"	"$(INTDIR)\message.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\MOD232CommsProcessor.cpp

"$(INTDIR)\MOD232CommsProcessor.obj"	"$(INTDIR)\MOD232CommsProcessor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\mod_RSsim.cpp

"$(INTDIR)\mod_RSsim.obj"	"$(INTDIR)\mod_RSsim.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\mod_RSsim.odl

!IF  "$(CFG)" == "mod_RSsim - Win32 Release"

MTL_SWITCHES=/nologo /D "NDEBUG" /tlb "$(OUTDIR)\mod_RSsim.tlb" /mktyplib203 /win32 

"$(OUTDIR)\mod_RSsim.tlb" : $(SOURCE) "$(OUTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mod_RSsim - Win32 Debug"

MTL_SWITCHES=/nologo /D "_DEBUG" /tlb "$(OUTDIR)\mod_RSsim.tlb" /mktyplib203 /win32 

"$(OUTDIR)\mod_RSsim.tlb" : $(SOURCE) "$(OUTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mod_RSsimDlg.cpp

"$(INTDIR)\mod_RSsimDlg.obj"	"$(INTDIR)\mod_RSsimDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\MODEthCommsProcessor.cpp

"$(INTDIR)\MODEthCommsProcessor.obj"	"$(INTDIR)\MODEthCommsProcessor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\MyHostProxy.cpp

"$(INTDIR)\MyHostProxy.obj"	"$(INTDIR)\MyHostProxy.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\PLCApplication.cpp

"$(INTDIR)\PLCApplication.obj"	"$(INTDIR)\PLCApplication.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\RS232Noise.cpp

"$(INTDIR)\RS232Noise.obj"	"$(INTDIR)\RS232Noise.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\RS232Port.cpp

"$(INTDIR)\RS232Port.obj"	"$(INTDIR)\RS232Port.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\serverRS232array.cpp

"$(INTDIR)\serverRS232array.obj"	"$(INTDIR)\serverRS232array.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\ServerSettings232Dlg.cpp

"$(INTDIR)\ServerSettings232Dlg.obj"	"$(INTDIR)\ServerSettings232Dlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\ServerSettingsEthDlg.cpp

"$(INTDIR)\ServerSettingsEthDlg.obj"	"$(INTDIR)\ServerSettingsEthDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\ServerSocketArray.cpp

"$(INTDIR)\ServerSocketArray.obj"	"$(INTDIR)\ServerSocketArray.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\simport.cpp

"$(INTDIR)\simport.obj"	"$(INTDIR)\simport.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\Splash.cpp

"$(INTDIR)\Splash.obj"	"$(INTDIR)\Splash.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\StarWarsCtrl.cpp

"$(INTDIR)\StarWarsCtrl.obj"	"$(INTDIR)\StarWarsCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "mod_RSsim - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mod_RSsim.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\mod_RSsim.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mod_RSsim - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_COMMS_DEBUGGING" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mod_RSsim.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\mod_RSsim.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TrainingSimDlg.cpp

"$(INTDIR)\TrainingSimDlg.obj"	"$(INTDIR)\TrainingSimDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\UTIL.CPP

"$(INTDIR)\UTIL.OBJ"	"$(INTDIR)\UTIL.SBR" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mod_RSsim.pch"


SOURCE=.\mod_RSsim.rc

!IF  "$(CFG)" == "mod_RSsim - Win32 Release"


"$(INTDIR)\mod_RSsim.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\mod_RSsim.res" /i "Release" /d "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "mod_RSsim - Win32 Debug"


"$(INTDIR)\mod_RSsim.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\mod_RSsim.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "mod_RSsim - Win32 Release"

"MYLIB - Win32 Release" : 
   cd "\source\protdrv\UTILS\MYLIB"
   $(MAKE) /$(MAKEFLAGS) /F .\MYLIB.mak CFG="MYLIB - Win32 Release" 
   cd "..\simulators\mod_RSsim"

"MYLIB - Win32 ReleaseCLEAN" : 
   cd "\source\protdrv\UTILS\MYLIB"
   $(MAKE) /$(MAKEFLAGS) /F .\MYLIB.mak CFG="MYLIB - Win32 Release" RECURSE=1 CLEAN 
   cd "..\simulators\mod_RSsim"

!ELSEIF  "$(CFG)" == "mod_RSsim - Win32 Debug"

"MYLIB - Win32 Debug" : 
   cd "\source\protdrv\UTILS\MYLIB"
   $(MAKE) /$(MAKEFLAGS) /F .\MYLIB.mak CFG="MYLIB - Win32 Debug" 
   cd "..\simulators\mod_RSsim"

"MYLIB - Win32 DebugCLEAN" : 
   cd "\source\protdrv\UTILS\MYLIB"
   $(MAKE) /$(MAKEFLAGS) /F .\MYLIB.mak CFG="MYLIB - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\simulators\mod_RSsim"

!ENDIF 


!ENDIF 

