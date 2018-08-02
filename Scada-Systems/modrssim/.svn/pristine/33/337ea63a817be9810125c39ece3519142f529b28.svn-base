# Microsoft Developer Studio Generated NMAKE File, Based on MYLIB.dsp
!IF "$(CFG)" == ""
CFG=MYLIB - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MYLIB - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MYLIB - Win32 Release" && "$(CFG)" != "MYLIB - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MYLIB.mak" CFG="MYLIB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MYLIB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MYLIB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MYLIB - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\MYLIB.lib"


CLEAN :
	-@erase "$(INTDIR)\ado.obj"
	-@erase "$(INTDIR)\ColorPickerCB.obj"
	-@erase "$(INTDIR)\ColorStatic.obj"
	-@erase "$(INTDIR)\CommsLamp.obj"
	-@erase "$(INTDIR)\DragSizerBmp.obj"
	-@erase "$(INTDIR)\dregkey.obj"
	-@erase "$(INTDIR)\MicroTick.obj"
	-@erase "$(INTDIR)\mylib.obj"
	-@erase "$(INTDIR)\MYLIB.pch"
	-@erase "$(INTDIR)\mylib.res"
	-@erase "$(INTDIR)\Resizer.obj"
	-@erase "$(INTDIR)\StatLink.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MYLIB.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MYLIB.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mylib.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MYLIB.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\MYLIB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ado.obj" \
	"$(INTDIR)\ColorStatic.obj" \
	"$(INTDIR)\DragSizerBmp.obj" \
	"$(INTDIR)\dregkey.obj" \
	"$(INTDIR)\mylib.obj" \
	"$(INTDIR)\Resizer.obj" \
	"$(INTDIR)\StatLink.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mylib.res" \
	"$(INTDIR)\CommsLamp.obj" \
	"$(INTDIR)\MicroTick.obj" \
	"$(INTDIR)\ColorPickerCB.obj"

"$(OUTDIR)\MYLIB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\MYLIB.lib" "$(OUTDIR)\MYLIB.bsc"


CLEAN :
	-@erase "$(INTDIR)\ado.obj"
	-@erase "$(INTDIR)\ado.sbr"
	-@erase "$(INTDIR)\ColorPickerCB.obj"
	-@erase "$(INTDIR)\ColorPickerCB.sbr"
	-@erase "$(INTDIR)\ColorStatic.obj"
	-@erase "$(INTDIR)\ColorStatic.sbr"
	-@erase "$(INTDIR)\CommsLamp.obj"
	-@erase "$(INTDIR)\CommsLamp.sbr"
	-@erase "$(INTDIR)\DragSizerBmp.obj"
	-@erase "$(INTDIR)\DragSizerBmp.sbr"
	-@erase "$(INTDIR)\dregkey.obj"
	-@erase "$(INTDIR)\dregkey.sbr"
	-@erase "$(INTDIR)\MicroTick.obj"
	-@erase "$(INTDIR)\MicroTick.sbr"
	-@erase "$(INTDIR)\mylib.obj"
	-@erase "$(INTDIR)\MYLIB.pch"
	-@erase "$(INTDIR)\mylib.res"
	-@erase "$(INTDIR)\mylib.sbr"
	-@erase "$(INTDIR)\Resizer.obj"
	-@erase "$(INTDIR)\Resizer.sbr"
	-@erase "$(INTDIR)\StatLink.obj"
	-@erase "$(INTDIR)\StatLink.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MYLIB.bsc"
	-@erase "$(OUTDIR)\MYLIB.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\MYLIB.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mylib.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MYLIB.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ado.sbr" \
	"$(INTDIR)\ColorStatic.sbr" \
	"$(INTDIR)\DragSizerBmp.sbr" \
	"$(INTDIR)\dregkey.sbr" \
	"$(INTDIR)\mylib.sbr" \
	"$(INTDIR)\Resizer.sbr" \
	"$(INTDIR)\StatLink.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\CommsLamp.sbr" \
	"$(INTDIR)\MicroTick.sbr" \
	"$(INTDIR)\ColorPickerCB.sbr"

"$(OUTDIR)\MYLIB.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\MYLIB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ado.obj" \
	"$(INTDIR)\ColorStatic.obj" \
	"$(INTDIR)\DragSizerBmp.obj" \
	"$(INTDIR)\dregkey.obj" \
	"$(INTDIR)\mylib.obj" \
	"$(INTDIR)\Resizer.obj" \
	"$(INTDIR)\StatLink.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mylib.res" \
	"$(INTDIR)\CommsLamp.obj" \
	"$(INTDIR)\MicroTick.obj" \
	"$(INTDIR)\ColorPickerCB.obj"

"$(OUTDIR)\MYLIB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("MYLIB.dep")
!INCLUDE "MYLIB.dep"
!ELSE 
!MESSAGE Warning: cannot find "MYLIB.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MYLIB - Win32 Release" || "$(CFG)" == "MYLIB - Win32 Debug"
SOURCE=.\ado.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\ado.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\ado.obj"	"$(INTDIR)\ado.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\ColorPickerCB.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\ColorPickerCB.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\ColorPickerCB.obj"	"$(INTDIR)\ColorPickerCB.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\ColorStatic.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\ColorStatic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\ColorStatic.obj"	"$(INTDIR)\ColorStatic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\CommsLamp.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\CommsLamp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\CommsLamp.obj"	"$(INTDIR)\CommsLamp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\DragSizerBmp.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\DragSizerBmp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\DragSizerBmp.obj"	"$(INTDIR)\DragSizerBmp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\dregkey.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\dregkey.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\dregkey.obj"	"$(INTDIR)\dregkey.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=..\simulators\mod_RSsim\MicroTick.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\MicroTick.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\MicroTick.obj"	"$(INTDIR)\MicroTick.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\mylib.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\mylib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\mylib.obj"	"$(INTDIR)\mylib.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\mylib.rc

"$(INTDIR)\mylib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Resizer.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\Resizer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\Resizer.obj"	"$(INTDIR)\Resizer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\StatLink.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"


"$(INTDIR)\StatLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"


"$(INTDIR)\StatLink.obj"	"$(INTDIR)\StatLink.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\MYLIB.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "MYLIB - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\MYLIB.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\MYLIB.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MYLIB - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\MYLIB.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\MYLIB.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

