# Microsoft Developer Studio Project File - Name="ui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ui - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak" CFG="ui - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\gigabase" /I "..\common" /I "..\database" /I ".\userdialogs" /I ".\sysdialogs" /I ".\widgets" /I ".\map" /I "..\drivers" /I "..\drivers\Simulator" /I ".\widgets\extra" /I ".\widgets\gantt" /I "..\trace" /I "..\utilities" /I ".\qwt-4.2.0\include" /I ".\libqt4lab-0.1.1\include" /I "..\fifo" /I "..\middleware\ripc\inc" /I "..\middleware\rtps\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB snmpapi.lib scadad.lib utilitiesd.lib qt-mt3.lib qtmain.lib qui.lib qwt.lib /nologo /subsystem:windows /incremental:no /pdb:"Debug/ui.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ui___Win32_Release"
# PROP BASE Intermediate_Dir "ui___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "$(QTDIR)\include" /I "..\gigabase" /I "..\common" /I "..\database" /I ".\userdialogs" /I ".\sysdialogs" /I ".\widgets" /I ".\map" /I "..\drivers" /I "..\drivers\Simulator" /I "..\STL" /I "..\drivers\en_base" /I ".\widgets\extra" /I ".\widgets\gantt" /I "..\trace" /I "..\utilities" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /FD /GZ /c
# SUBTRACT BASE CPP /u /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\gigabase" /I "..\common" /I "..\database" /I ".\userdialogs" /I ".\sysdialogs" /I ".\widgets" /I ".\map" /I "..\drivers" /I "..\drivers\Simulator" /I ".\widgets\extra" /I ".\widgets\gantt" /I "..\trace" /I "..\utilities" /I ".\qwt-4.2.0\include" /I "..\hmi_designer\designer\shared" /I ".\libqt4lab-0.1.1\include" /I "..\fifo" /I "..\middleware\ripc\inc" /I "..\middleware\rtps\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB scada.lib snmpapi.lib qtdll.lib utilities.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"c:\scada\bin/ist.exe" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib snmpapi.lib wsock32.lib scada.lib utilities.lib qt-mt3.lib qtmain.lib qwt.lib qui.lib /nologo /subsystem:windows /incremental:no /pdb:"Release/ui.pdb" /machine:I386 /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ui - Win32 Debug"
# Name "ui - Win32 Release"
# Begin Group "ui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\alarmdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\alarmdisplay.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\alarmdisplay.h
InputName=alarmdisplay

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\alarmdisplay.h
InputName=alarmdisplay

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\dbtable.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\dbtable.h
InputName=dbtable

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\dbtable.h
InputName=dbtable

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\display.cpp
# End Source File
# Begin Source File

SOURCE=.\display.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\graphpane.cpp
# End Source File
# Begin Source File

SOURCE=.\graphpane.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\graphpane.h
InputName=graphpane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\graphpane.h
InputName=graphpane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\graphpanedata.cpp
# End Source File
# Begin Source File

SOURCE=.\graphpanedata.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\graphpanedata.h
InputName=graphpanedata

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\graphpanedata.h
InputName=graphpanedata

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\historic_browsedb.cpp
# End Source File
# Begin Source File

SOURCE=.\historic_browsedb.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing historic_browsedb.h...
InputDir=.
InputPath=.\historic_browsedb.h
InputName=historic_browsedb

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing historic_browsedb.h...
InputDir=.
InputPath=.\historic_browsedb.h
InputName=historic_browsedb

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\historic_inspect.cpp
# End Source File
# Begin Source File

SOURCE=.\historic_inspect.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing historic_inspect.h...
InputDir=.
InputPath=.\historic_inspect.h
InputName=historic_inspect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing historic_inspect.h...
InputDir=.
InputPath=.\historic_inspect.h
InputName=historic_inspect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hmi_mng.cpp
# End Source File
# Begin Source File

SOURCE=.\hmi_mng.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing hmi_mng.h...
InputDir=.
InputPath=.\hmi_mng.h
InputName=hmi_mng

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing hmi_mng.h...
InputDir=.
InputPath=.\hmi_mng.h
InputName=hmi_mng

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inspect.cpp
# End Source File
# Begin Source File

SOURCE=.\inspect.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\inspect.h
InputName=inspect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\inspect.h
InputName=inspect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\multitrace.cpp
# End Source File
# Begin Source File

SOURCE=.\multitrace.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\multitrace.h
InputName=multitrace

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\multitrace.h
InputName=multitrace

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\realtime_browsedb.cpp
# End Source File
# Begin Source File

SOURCE=.\realtime_browsedb.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing realtime_browsedb.h...
InputDir=.
InputPath=.\realtime_browsedb.h
InputName=realtime_browsedb

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing realtime_browsedb.h...
InputDir=.
InputPath=.\realtime_browsedb.h
InputName=realtime_browsedb

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rintro.cpp
# End Source File
# Begin Source File

SOURCE=.\rintro.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\rintro.h
InputName=rintro

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\rintro.h
InputName=rintro

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\statusdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\statusdisplay.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\statusdisplay.h
InputName=statusdisplay

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\statusdisplay.h
InputName=statusdisplay

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\statuspane.cpp
# End Source File
# Begin Source File

SOURCE=.\statuspane.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\statuspane.h
InputName=statuspane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\statuspane.h
InputName=statuspane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\user.cpp
# End Source File
# Begin Source File

SOURCE=.\user.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\user.h
InputName=user

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\user.h
InputName=user

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "sysdialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sysdialogs\ActionConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ActionConfigure.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ActionConfigure.h
InputName=ActionConfigure

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ActionConfigure.h
InputName=ActionConfigure

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ActionConfigureData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ActionConfigureData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ActionConfigureData.h
InputName=ActionConfigureData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ActionConfigureData.h
InputName=ActionConfigureData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\AlarmGroupCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\AlarmGroupCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\AlarmGroupCfg.h
InputName=AlarmGroupCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\AlarmGroupCfg.h
InputName=AlarmGroupCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\AlarmGroupCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\AlarmGroupCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\AlarmGroupCfgData.h
InputName=AlarmGroupCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\AlarmGroupCfgData.h
InputName=AlarmGroupCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\BatchCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\BatchCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\BatchCfg.h
InputName=BatchCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\BatchCfg.h
InputName=BatchCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\BatchCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\BatchCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\BatchCfgData.h
InputName=BatchCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\BatchCfgData.h
InputName=BatchCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\CheckTagEntries.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\CheckTagEntries.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\CheckTagEntries.h
InputName=CheckTagEntries

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\CheckTagEntries.h
InputName=CheckTagEntries

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\CheckTagEntriesData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\CheckTagEntriesData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\CheckTagEntriesData.h
InputName=CheckTagEntriesData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\CheckTagEntriesData.h
InputName=CheckTagEntriesData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\NewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\NewDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\NewDlg.h
InputName=NewDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\NewDlg.h
InputName=NewDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\NewDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\NewDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\NewDlgData.h
InputName=NewDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\NewDlgData.h
InputName=NewDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReceipeCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReceipeCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReceipeCfg.h
InputName=ReceipeCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReceipeCfg.h
InputName=ReceipeCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReceipeCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReceipeCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReceipeCfgData.h
InputName=ReceipeCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReceipeCfgData.h
InputName=ReceipeCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReportCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReportCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReportCfg.h
InputName=ReportCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReportCfg.h
InputName=ReportCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReportCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ReportCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReportCfgData.h
InputName=ReportCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ReportCfgData.h
InputName=ReportCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SampleCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SampleCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SampleCfg.h
InputName=SampleCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SampleCfg.h
InputName=SampleCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SampleCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SampleCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SampleCfgData.h
InputName=SampleCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SampleCfgData.h
InputName=SampleCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ScheduleCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ScheduleCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ScheduleCfg.h
InputName=ScheduleCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ScheduleCfg.h
InputName=ScheduleCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ScheduleCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\ScheduleCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ScheduleCfgData.h
InputName=ScheduleCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\ScheduleCfgData.h
InputName=ScheduleCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SerialCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SerialCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SerialCfg.h
InputName=SerialCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SerialCfg.h
InputName=SerialCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SerialCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SerialCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SerialCfgData.h
InputName=SerialCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SerialCfgData.h
InputName=SerialCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SystemCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SystemCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SystemCfg.h
InputName=SystemCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SystemCfg.h
InputName=SystemCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SystemCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\SystemCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SystemCfgData.h
InputName=SystemCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\SystemCfgData.h
InputName=SystemCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfg.h
InputName=TagCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfg.h
InputName=TagCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfgData.h
InputName=TagCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfgData.h
InputName=TagCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfgDigital.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfgDigital.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing TagCfgDigital.h...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfgDigital.h
InputName=TagCfgDigital

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing TagCfgDigital.h...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfgDigital.h
InputName=TagCfgDigital

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfgDigitalData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\TagCfgDigitalData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing TagCfgDigitalData.h...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfgDigitalData.h
InputName=TagCfgDigitalData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing TagCfgDigitalData.h...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\TagCfgDigitalData.h
InputName=TagCfgDigitalData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UnitCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UnitCfg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UnitCfg.h
InputName=UnitCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UnitCfg.h
InputName=UnitCfg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UnitCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UnitCfgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UnitCfgData.h
InputName=UnitCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UnitCfgData.h
InputName=UnitCfgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UserCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UserCfgDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UserCfgDlg.h
InputName=UserCfgDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UserCfgDlg.h
InputName=UserCfgDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UserCfgDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\UserCfgDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UserCfgDlgData.h
InputName=UserCfgDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\sysdialogs
InputPath=.\sysdialogs\UserCfgDlgData.h
InputName=UserCfgDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "userdialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userdialogs\AckAlarmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\AckAlarmDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\AckAlarmDlg.h
InputName=AckAlarmDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\AckAlarmDlg.h
InputName=AckAlarmDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\AckAlarmDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\AckAlarmDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\AckAlarmDlgData.h
InputName=AckAlarmDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\AckAlarmDlgData.h
InputName=AckAlarmDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\InspectFrameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\InspectFrameDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\InspectFrameDlg.h
InputName=InspectFrameDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\InspectFrameDlg.h
InputName=InspectFrameDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\InspectFrameDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\InspectFrameDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\InspectFrameDlgData.h
InputName=InspectFrameDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\InspectFrameDlgData.h
InputName=InspectFrameDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\LoginDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\LoginDlg.h
InputName=LoginDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\LoginDlg.h
InputName=LoginDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\LoginDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\LoginDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\LoginDlgData.h
InputName=LoginDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\LoginDlgData.h
InputName=LoginDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\MultiTraceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\MultiTraceDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\MultiTraceDlg.h
InputName=MultiTraceDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\MultiTraceDlg.h
InputName=MultiTraceDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\MultiTraceDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\MultiTraceDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\MultiTraceDlgData.h
InputName=MultiTraceDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\MultiTraceDlgData.h
InputName=MultiTraceDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\NewPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\NewPassword.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\NewPassword.h
InputName=NewPassword

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\NewPassword.h
InputName=NewPassword

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\NewPasswordData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\NewPasswordData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\NewPasswordData.h
InputName=NewPasswordData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\NewPasswordData.h
InputName=NewPasswordData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReceipeSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReceipeSelect.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReceipeSelect.h
InputName=ReceipeSelect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReceipeSelect.h
InputName=ReceipeSelect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReceipeSelectData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReceipeSelectData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReceipeSelectData.h
InputName=ReceipeSelectData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReceipeSelectData.h
InputName=ReceipeSelectData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportDir.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportDir.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportDir.h
InputName=ReportDir

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportDir.h
InputName=ReportDir

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportDirData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportDirData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportDirData.h
InputName=ReportDirData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportDirData.h
InputName=ReportDirData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportSelect.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportSelect.h
InputName=ReportSelect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportSelect.h
InputName=ReportSelect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportSelectData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\ReportSelectData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportSelectData.h
InputName=ReportSelectData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\ReportSelectData.h
InputName=ReportSelectData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\SelectPrinterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\SelectPrinterDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\SelectPrinterDlg.h
InputName=SelectPrinterDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\SelectPrinterDlg.h
InputName=SelectPrinterDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\SelectPrinterDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\SelectPrinterDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\SelectPrinterDlgData.h
InputName=SelectPrinterDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\SelectPrinterDlgData.h
InputName=SelectPrinterDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\TimeFrameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\TimeFrameDlg.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\TimeFrameDlg.h
InputName=TimeFrameDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\TimeFrameDlg.h
InputName=TimeFrameDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\userdialogs\TimeFrameDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\TimeFrameDlgData.h

!IF  "$(CFG)" == "ui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\TimeFrameDlgData.h
InputName=TimeFrameDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\userdialogs
InputPath=.\userdialogs\TimeFrameDlgData.h
InputName=TimeFrameDlgData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "mocs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userdialogs\moc_AckAlarmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_AckAlarmDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ActionConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ActionConfigureData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_alarmdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_AlarmGroupCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_AlarmGroupCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_BatchCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_BatchCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_CheckTagEntries.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_CheckTagEntriesData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_graphpane.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_graphpanedata.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_historic_browsedb.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_historic_inspect.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_hmi_mng.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_inspect.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_InspectFrameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_InspectFrameDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_LoginDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_multitrace.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_MultiTraceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_MultiTraceDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_NewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_NewDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_NewPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_NewPasswordData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_realtime_browsedb.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ReceipeCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ReceipeCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_ReceipeSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_ReceipeSelectData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ReportCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ReportCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_ReportDir.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_ReportDirData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_ReportSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_ReportSelectData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_rintro.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_SampleCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_SampleCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ScheduleCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_ScheduleCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_SelectPrinterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_SelectPrinterDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_SerialCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_SerialCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_statusdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_statuspane.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_SystemCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_SystemCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_TagCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_TagCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_TagCfgDigital.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_TagCfgDigitalData.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_TimeFrameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\userdialogs\moc_TimeFrameDlgData.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_UnitCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_UnitCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_user.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_UserCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sysdialogs\moc_UserCfgDlgData.cpp
# End Source File
# End Group
# Begin Group "int"

# PROP Default_Filter ""
# Begin Group "italian"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Setup\International\Italiano.po
# End Source File
# End Group
# End Group
# End Target
# End Project
