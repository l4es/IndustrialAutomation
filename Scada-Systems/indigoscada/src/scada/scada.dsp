# Microsoft Developer Studio Project File - Name="scada" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=scada - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "scada.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scada.mak" CFG="scada - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scada - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "scada - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scada - Win32 Debug"

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
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCADA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\common" /I "..\database" /I "..\ui" /I "..\ui\widgets" /I ".\userdialogs" /I ".\sysdialogs" /I ".\widgets" /I ".\map" /I ".\drivers" /I ".\drivers\Simulator" /I "..\gigabase" /I "..\dispatcher\client" /I "..\ui\widgets\extra" /I "..\trace" /I "..\utilities" /I "..\fastdb\inc" /I "..\drivers" /I "..\fifo" /I "..\middleware\ripc\inc" /I "..\middleware\rtps\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "SCADA_EXPORTS" /D "USING_GARRET" /D "SECURE_SERVER" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib utilitiesd.lib qt-mt3.lib realtimedbased.lib /nologo /dll /incremental:no /pdb:"Debug/scada.pdb" /debug /machine:I386 /out:"c:\scada\bin/scadad.dll" /implib:"c:\scada\lib/scadad.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "scada___Win32_Release"
# PROP BASE Intermediate_Dir "scada___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "..\common" /I "..\database" /I "..\ui" /I "..\ui\widgets" /I "..\ui\drivers" /I ".\userdialogs" /I ".\sysdialogs" /I ".\widgets" /I ".\map" /I ".\drivers" /I ".\drivers\Simulator" /I "..\STL" /I "..\gigabase" /I "..\dispatcher\client" /I "..\ui\widgets\extra" /I "..\trace" /I "..\utilities" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "SCADA_EXPORTS" /D "USING_GARRET" /D "SECURE_SERVER" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\common" /I "..\database" /I "..\ui" /I "..\ui\widgets" /I "..\ui\drivers" /I ".\userdialogs" /I ".\sysdialogs" /I ".\widgets" /I ".\map" /I ".\drivers" /I ".\drivers\Simulator" /I "..\gigabase" /I "..\dispatcher\client" /I "..\ui\widgets\extra" /I "..\trace" /I "..\utilities" /I "..\drivers" /I "..\fifo" /I "..\middleware\ripc\inc" /I "..\middleware\rtps\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "SCADA_EXPORTS" /D "USING_GARRET" /D "SECURE_SERVER" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib qtdll.lib utilities.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /implib:"c:\scada\lib/scada.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib utilities.lib qt-mt3.lib realtimedbase.lib /nologo /dll /incremental:no /pdb:"Release/scada.pdb" /machine:I386 /implib:"c:\scada\lib/scada.lib" /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "scada - Win32 Debug"
# Name "scada - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\combo.cpp
# End Source File
# Begin Source File

SOURCE=..\common\common.cpp
# End Source File
# Begin Source File

SOURCE=..\common\common.h
# End Source File
# Begin Source File

SOURCE=..\common\dispatch.cpp
# End Source File
# Begin Source File

SOURCE=..\common\dispatch.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\dispatch.h
InputName=dispatch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\dispatch.h
InputName=dispatch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\dispatch_reader.cpp
# End Source File
# Begin Source File

SOURCE=..\common\graph.cpp
# End Source File
# Begin Source File

SOURCE=..\common\graph.h
# End Source File
# Begin Source File

SOURCE=..\common\graphdata.cpp
# End Source File
# Begin Source File

SOURCE=..\common\graphdata.h
# End Source File
# Begin Source File

SOURCE=..\common\loggen.cpp
# End Source File
# Begin Source File

SOURCE=..\common\loggen.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\loggen.h
InputName=loggen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\loggen.h
InputName=loggen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\messages.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\messages.h
InputName=messages

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\messages.h
InputName=messages

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\moc_dispatch.cpp
# End Source File
# Begin Source File

SOURCE=..\common\moc_loggen.cpp
# End Source File
# Begin Source File

SOURCE=..\common\moc_messages.cpp
# End Source File
# Begin Source File

SOURCE=..\common\moc_printrep.cpp
# End Source File
# Begin Source File

SOURCE=..\common\moc_repgen.cpp
# End Source File
# Begin Source File

SOURCE=..\common\mythread.c
# End Source File
# Begin Source File

SOURCE=..\common\printrep.cpp
# End Source File
# Begin Source File

SOURCE=..\common\printrep.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\printrep.h
InputName=printrep

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\printrep.h
InputName=printrep

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\repgen.cpp
# End Source File
# Begin Source File

SOURCE=..\common\repgen.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\repgen.h
InputName=repgen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\common
InputPath=..\common\repgen.h
InputName=repgen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\scada_dll.h
# End Source File
# Begin Source File

SOURCE=..\common\smplhist.cpp
# End Source File
# Begin Source File

SOURCE=..\common\smplhist.h
# End Source File
# Begin Source File

SOURCE=..\common\smplstat.cpp
# End Source File
# Begin Source File

SOURCE=..\common\smplstat.h
# End Source File
# End Group
# Begin Group "database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\database\dbase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\dbase.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\dbase.h
InputName=dbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\dbase.h
InputName=dbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\database\fastdbclient.cpp
# End Source File
# Begin Source File

SOURCE=..\database\fastdbclient.h
# End Source File
# Begin Source File

SOURCE=..\database\gigaclient.cpp
# End Source File
# Begin Source File

SOURCE=..\database\gigaclient.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\database\helper_functions.cpp
# End Source File
# Begin Source File

SOURCE=..\database\helper_functions.h
# End Source File
# Begin Source File

SOURCE=..\database\historicdb.cpp
# End Source File
# Begin Source File

SOURCE=..\database\historicdb.h
# End Source File
# Begin Source File

SOURCE=..\database\moc_dbase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\moc_qsfastdb.cpp
# End Source File
# Begin Source File

SOURCE=..\database\moc_qsgigabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\moc_results.cpp
# End Source File
# Begin Source File

SOURCE=..\database\qsfastdb.cpp
# End Source File
# Begin Source File

SOURCE=..\database\qsfastdb.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing qsfastdb.h...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\qsfastdb.h
InputName=qsfastdb

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing qsfastdb.h...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\qsfastdb.h
InputName=qsfastdb

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\database\qsgigabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\qsgigabase.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\qsgigabase.h
InputName=qsgigabase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\qsgigabase.h
InputName=qsgigabase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\database\realtimedb.cpp
# End Source File
# Begin Source File

SOURCE=..\database\realtimedb.h
# End Source File
# Begin Source File

SOURCE=..\database\results.cpp
# End Source File
# Begin Source File

SOURCE=..\database\results.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\results.h
InputName=results

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\database
InputPath=..\database\results.h
InputName=results

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "widgets"

# PROP Default_Filter ""
# Begin Group "default"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\UI\widgets\calendar.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\calendar.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\calendar.h
InputName=calendar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\calendar.h
InputName=calendar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\datenav.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\datenav.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\datenav.h
InputName=datenav

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\datenav.h
InputName=datenav

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\DateNavigator.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\DateNavigator.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\DateNavigator.h
InputName=DateNavigator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\DateNavigator.h
InputName=DateNavigator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\DateNavigatorData.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\DateNavigatorData.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\DateNavigatorData.h
InputName=DateNavigatorData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\DateNavigatorData.h
InputName=DateNavigatorData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\dateobj.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\dateobj.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\dateobj.h
InputName=dateobj

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\dateobj.h
InputName=dateobj

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\datepopup.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\datepopup.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\datepopup.h
InputName=datepopup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\datepopup.h
InputName=datepopup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\dclock.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\dclock.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\dclock.h
InputName=dclock

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\dclock.h
InputName=dclock

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\helpwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\helpwindow.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\helpwindow.h
InputName=helpwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\helpwindow.h
InputName=helpwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_calendar.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_datenav.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_DateNavigator.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_DateNavigatorData.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_dateobj.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_datepopup.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_dclock.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_helpwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\moc_table.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\table.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\table.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\table.h
InputName=table

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\UI\widgets
InputPath=..\UI\widgets\table.h
InputName=table

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\UI\widgets\valedit.cpp
# End Source File
# Begin Source File

SOURCE=..\UI\widgets\valedit.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "drivers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\drivers\driver.cpp
# End Source File
# Begin Source File

SOURCE=..\drivers\driver.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\drivers
InputPath=..\drivers\driver.h
InputName=driver

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\drivers
InputPath=..\drivers\driver.h
InputName=driver

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\drivers\moc_driver.cpp
# End Source File
# Begin Source File

SOURCE=..\drivers\moc_SerialConnect.cpp
# End Source File
# Begin Source File

SOURCE=..\drivers\SerialConnect.cpp
# End Source File
# Begin Source File

SOURCE=..\drivers\SerialConnect.h

!IF  "$(CFG)" == "scada - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\drivers
InputPath=..\drivers\SerialConnect.h
InputName=SerialConnect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "scada - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\indigoscada-code-279-trunk\indigoscada-code-279-trunk\src\drivers
InputPath=..\drivers\SerialConnect.h
InputName=SerialConnect

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\drivers\sptypes.h
# End Source File
# End Group
# Begin Group "cli_gigabase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gigabase\cli.h
# End Source File
# Begin Source File

SOURCE=..\gigabase\cli_gigabase.cpp
# End Source File
# Begin Source File

SOURCE=..\gigabase\cliproto.h
# End Source File
# Begin Source File

SOURCE=..\gigabase\repsock.cpp
# End Source File
# Begin Source File

SOURCE=..\gigabase\sockio.h
# End Source File
# Begin Source File

SOURCE=..\gigabase\symtab.cpp
# End Source File
# Begin Source File

SOURCE=..\gigabase\w32sock.cpp
# End Source File
# Begin Source File

SOURCE=..\gigabase\w32sock.h
# End Source File
# End Group
# Begin Group "cli_dispatcher"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dispatcher\client\cli_dispatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\dispatcher\client\cli_dispatcher.h
# End Source File
# Begin Source File

SOURCE=..\dispatcher\client\cliproto_dispatcher.h
# End Source File
# End Group
# Begin Group "cli_fastdb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\fastdb\inc\cli.h
# End Source File
# Begin Source File

SOURCE=..\fastdb\src\cli_fastdb.cpp
# End Source File
# Begin Source File

SOURCE=..\fastdb\inc\cliproto.h
# End Source File
# End Group
# End Group
# End Target
# End Project
