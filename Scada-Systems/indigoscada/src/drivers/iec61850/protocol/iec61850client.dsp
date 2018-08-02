# Microsoft Developer Studio Project File - Name="iec61850client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=iec61850client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iec61850client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iec61850client.mak" CFG="iec61850client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iec61850client - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "iec61850client - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iec61850client - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\src\hal\thread" /I ".\src\hal\socket" /I ".\inc" /I ".\src\common" /I ".\src\model" /I ".\src\mms\iso_mms\server" /I ".\src\hal" /I ".\src\mms\iso_acse\asn1c" /I ".\src\mms\iso_presentation\asn1c" /I ".\src\mms\iso_mms\common" /I ".\src\mms\asn1" /I ".\src\mms\iso_acse" /I ".\src\mms\iso_cotp" /I ".\src\mms\iso_session" /I ".\src\mms\iso_presentation" /I ".\src\mms\iso_client" /I ".\src\mms\iso_mms\asn1c" /I ".\src\mms\iso_server" /I ".\src\mms\iso_mms\client" /I ".\src\mms_mapping" /I ".\src\api" /I "..\..\..\utilities" /I "..\..\..\common" /I "..\..\..\configurator\sqlite" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib iec61850lib.lib /nologo /subsystem:console /machine:I386 /libpath:".\Release"

!ELSEIF  "$(CFG)" == "iec61850client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\src\hal\thread" /I ".\src\hal\socket" /I ".\inc" /I ".\src\common" /I ".\src\model" /I ".\src\mms\iso_mms\server" /I ".\src\hal" /I ".\src\mms\iso_acse\asn1c" /I ".\src\mms\iso_presentation\asn1c" /I ".\src\mms\iso_mms\common" /I ".\src\mms\asn1" /I ".\src\mms\iso_acse" /I ".\src\mms\iso_cotp" /I ".\src\mms\iso_session" /I ".\src\mms\iso_presentation" /I ".\src\mms\iso_client" /I ".\src\mms\iso_mms\asn1c" /I ".\src\mms\iso_server" /I ".\src\mms\iso_mms\client" /I ".\src\mms_mapping" /I ".\src\api" /I "..\..\..\utilities" /I "..\..\..\common" /I "..\..\..\configurator\sqlite" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib iec61850lib.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /out:"c:\scada\bin/iec61850client.exe" /pdbtype:sept /libpath:".\Debug"

!ENDIF 

# Begin Target

# Name "iec61850client - Win32 Release"
# Name "iec61850client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\utilities\clear_crc_eight.c
# End Source File
# Begin Source File

SOURCE=.\client\client.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\utilities\getopt.c
# End Source File
# Begin Source File

SOURCE=.\client\load_database.cpp
# End Source File
# Begin Source File

SOURCE=.\client\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\configurator\sqlite\sqlite3.c
# End Source File
# End Group
# End Target
# End Project
