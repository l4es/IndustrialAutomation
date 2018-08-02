# Microsoft Developer Studio Project File - Name="Calculated" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Calculated - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Calculated.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Calculated.mak" CFG="Calculated - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Calculated - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Calculated - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\Drivers"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMULATOR_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\trace" /I "..\..\utilities" /I "..\..\fifo" /I "..\..\middleware\ripc\inc" /I "..\..\middleware\rtps\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "CALCULATED_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 scadad.lib utilitiesd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB qt-mt3.lib fifo.lib eic.lib /nologo /dll /incremental:no /pdb:"Debug/Calculated.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib" /libpath:"..\..\softlogic\EiC-4.4.2\src" /libpath:"..\..\fifo\Debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Simulator___Win32_Release"
# PROP BASE Intermediate_Dir "Simulator___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\Drivers"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\STL" /I "..\..\trace" /I "..\..\utilities" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "SIMULATOR_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\trace" /I "..\..\utilities" /I "..\..\fifo" /I "..\..\middleware\ripc\inc" /I "..\..\middleware\rtps\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "CALCULATED_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB scada.lib qtdll.lib /nologo /dll /pdb:"Debug/Calculated.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 scada.lib utilities.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB qt-mt3.lib eic.lib /nologo /dll /incremental:no /pdb:"Release/Calculated.pdb" /machine:I386 /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib" /libpath:"..\..\softlogic\EiC-4.4.2\src" /libpath:"..\..\fifo\Release"
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "Calculated - Win32 Debug"
# Name "Calculated - Win32 Release"
# Begin Group "Calculated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CalcEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CalcEntryDlg.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalcEntryDlg.h...
InputDir=.
InputPath=.\CalcEntryDlg.h
InputName=CalcEntryDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalcEntryDlg.h...
InputDir=.
InputPath=.\CalcEntryDlg.h
InputName=CalcEntryDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CalcEntryDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\CalcEntryDlgData.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalcEntryDlgData.h...
InputDir=.
InputPath=.\CalcEntryDlgData.h
InputName=CalcEntryDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalcEntryDlgData.h...
InputDir=.
InputPath=.\CalcEntryDlgData.h
InputName=CalcEntryDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Calculated.cpp
# End Source File
# Begin Source File

SOURCE=.\Calculated.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing Calculated.h...
InputDir=.
InputPath=.\Calculated.h
InputName=Calculated

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing Calculated.h...
InputDir=.
InputPath=.\Calculated.h
InputName=Calculated

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CalculatedConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\CalculatedConfigure.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedConfigure.h...
InputDir=.
InputPath=.\CalculatedConfigure.h
InputName=CalculatedConfigure

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedConfigure.h...
InputDir=.
InputPath=.\CalculatedConfigure.h
InputName=CalculatedConfigure

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CalculatedConfigureData.cpp
# End Source File
# Begin Source File

SOURCE=.\CalculatedConfigureData.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedConfigureData.h...
InputDir=.
InputPath=.\CalculatedConfigureData.h
InputName=CalculatedConfigureData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedConfigureData.h...
InputDir=.
InputPath=.\CalculatedConfigureData.h
InputName=CalculatedConfigureData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CalculatedInput.cpp
# End Source File
# Begin Source File

SOURCE=.\CalculatedInput.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedInput.h...
InputDir=.
InputPath=.\CalculatedInput.h
InputName=CalculatedInput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedInput.h...
InputDir=.
InputPath=.\CalculatedInput.h
InputName=CalculatedInput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CalculatedInputData.cpp
# End Source File
# Begin Source File

SOURCE=.\CalculatedInputData.h

!IF  "$(CFG)" == "Calculated - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedInputData.h...
InputDir=.
InputPath=.\CalculatedInputData.h
InputName=CalculatedInputData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Calculated - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing CalculatedInputData.h...
InputDir=.
InputPath=.\CalculatedInputData.h
InputName=CalculatedInputData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\utilities\clear_crc_eight.c
# End Source File
# Begin Source File

SOURCE=.\embedEiC.c
# End Source File
# Begin Source File

SOURCE=.\moc_CalcEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_CalcEntryDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_Calculated.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_CalculatedConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_CalculatedConfigureData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_CalculatedInput.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_CalculatedInputData.cpp
# End Source File
# End Group
# End Target
# End Project
