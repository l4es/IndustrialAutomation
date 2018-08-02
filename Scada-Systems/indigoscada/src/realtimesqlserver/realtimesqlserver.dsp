# Microsoft Developer Studio Project File - Name="realtimesqlserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=realtimesqlserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "realtimesqlserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "realtimesqlserver.mak" CFG="realtimesqlserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "realtimesqlserver - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "realtimesqlserver - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "realtimesqlserver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\scada\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "." /I "..\fastdb\inc" /I "..\dispatcher\client" /I "..\dispatcher\server" /I "..\trace" /I "..\utilities" /D "_CONSOLE" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "QT_THREAD_SUPPORT" /D "USE_LOCALE_SETTINGS" /D "SECURE_SERVER" /D "QT_DLL" /D "QT_H_CPP" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 advapi32.lib user32.lib snmpapi.lib utilities.lib wsock32.lib realtimedbase.lib qt-mt3.lib /nologo /subsystem:console /pdb:"Release/rtsqlserver.pdb" /machine:I386 /out:"C:\scada\bin/rtsqlserver.exe" /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "realtimesqlserver - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "$(QTDIR)\mkspecs\win32-msvc" /I "." /I "..\fastdb\inc" /I "..\trace" /I "..\utilities" /I "$(QTDIR)\include" /D "_CONSOLE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "QT_THREAD_SUPPORT" /D "USE_LOCALE_SETTINGS" /D "SECURE_SERVER" /D "QT_DLL" /D "QT_H_CPP" /c
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib user32.lib snmpapi.lib utilitiesd.lib wsock32.lib realtimedbased.lib qt-mt3.lib /nologo /subsystem:console /incremental:no /pdb:"Debug/realtimesqlserver.pdb" /debug /machine:I386 /out:"c:\scada\bin/rtsqlserver.exe" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "realtimesqlserver - Win32 Release"
# Name "realtimesqlserver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\database.xpm
# End Source File
# Begin Source File

SOURCE=..\ui\logout.xpm
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\realtimesqlserver.cpp
# End Source File
# Begin Source File

SOURCE=.\realtimesqlserver.h
# End Source File
# Begin Source File

SOURCE=..\fastdb\src\server.cpp
# End Source File
# End Group
# End Target
# End Project
