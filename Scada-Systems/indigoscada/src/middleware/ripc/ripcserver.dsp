# Microsoft Developer Studio Project File - Name="ripcserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ripcserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ripcserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ripcserver.mak" CFG="ripcserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ripcserver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ripcserver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ripcserver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RIPCSERVER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\inc" /I ".\inc\internal" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RIPC_DLL" /D "RIPC_NO_EXCEPTIONS" /D "RIPC_IMPLEMENTATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /machine:I386 /out:"C:\scada\bin/mserver.dll" /implib:"C:\scada\lib/ripcserver.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ripcserver - Win32 Debug"

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
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RIPCSERVER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\inc" /I ".\inc\internal" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RIPC_DLL" /D "RIPC_NO_EXCEPTIONS" /D "RIPC_IMPLEMENTATION" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"C:\scada\bin/mserver.dll" /implib:"C:\scada\lib/ripcserver.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ripcserver - Win32 Release"
# Name "ripcserver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\server\RIPCBarrierImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCEventImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCLockImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCMutexImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RIPCPrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCPrimitiveImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCQueueImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RIPCRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCSemaphoreImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCServer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCServerFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCSessionImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCSharedMemoryImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\server\RIPCWaitObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\socket\RIPCWinSocket.cpp
# End Source File
# End Group
# End Target
# End Project
