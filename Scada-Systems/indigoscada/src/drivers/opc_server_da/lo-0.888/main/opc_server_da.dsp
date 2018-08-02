# Microsoft Developer Studio Project File - Name="opc_server_da" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=opc_server_da - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opc_server_da.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opc_server_da.mak" CFG="opc_server_da - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opc_server_da - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "opc_server_da - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opc_server_da - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "c:\scada\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O2 /I "../lightopc" /I "../unilog" /I "..\opc" /I "..\..\..\..\fifo" /I "..\..\..\..\configurator\sqlite" /I "..\..\..\..\common" /I "..\..\..\..\utilities" /I "..\..\..\..\middleware\ripc\inc" /I "..\..\..\..\middleware\rtps\include" /D "_MBCS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RIPC_DLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib unilog.lib lightopc.lib fifo.lib ripcclient.lib rtps.lib /nologo /subsystem:console /pdb:"Release/opc_server_da.pdb" /machine:I386 /libpath:"c:\scada\lib" /libpath:"..\bin" /libpath:"..\unilog"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "opc_server_da - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "samp_exe___Win32_Debug"
# PROP BASE Intermediate_Dir "samp_exe___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\bin"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Zi /Od /I "../lightopc" /I "../unilog" /I "..\opc" /I "..\..\..\..\fifo" /I "..\..\..\..\configurator\sqlite" /I "..\..\..\..\common" /I "..\..\..\..\utilities" /I "..\..\..\..\middleware\rtps\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RIPC_DLL" /Fo"Debug/" /Fd"Debug/" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib unilog.lib lightopc.lib fifo.lib ripcclient.lib rtps.lib /nologo /subsystem:console /incremental:no /pdb:"Debug/opc_server_da.pdb" /debug /machine:I386 /libpath:"c:\scada\lib" /libpath:"..\bin" /libpath:"..\unilog"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "opc_server_da - Win32 Release"
# Name "opc_server_da - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\middleware\rtps\librtps\cdr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\utilities\clear_crc_eight.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\common\iec_item_type.c
# End Source File
# Begin Source File

SOURCE=.\opc_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\configurator\sqlite\sqlite3.c
# End Source File
# End Group
# End Target
# End Project
