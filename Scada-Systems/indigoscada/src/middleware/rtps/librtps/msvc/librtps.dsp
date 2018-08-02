# Microsoft Developer Studio Project File - Name="librtps" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=librtps - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "librtps.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "librtps.mak" CFG="librtps - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "librtps - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "librtps - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "librtps - Win32 Release"

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
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBORTE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\include\win32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBORTE_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG"
# ADD RSC /l 0x405 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 pthreadVC2.lib ws2_32.lib /nologo /dll /pdb:"Release/rtps.pdb" /machine:I386 /out:"C:\scada\bin/rtps.dll" /implib:"C:\scada\lib/rtps.lib" /libpath:"C:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "librtps - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\scada\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBORTE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\include" /I "..\..\include\win32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBORTE_EXPORTS" /FD /D /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG"
# ADD RSC /l 0x405 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 pthreadVC2.lib ws2_32.lib /nologo /dll /incremental:no /pdb:"Debug/rtps.pdb" /debug /machine:I386 /out:"C:\scada\bin/rtps.dll" /implib:"C:\scada\lib/rtps.lib" /pdbtype:sept /libpath:"C:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "librtps - Win32 Release"
# Name "librtps - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\cdr.c
# End Source File
# Begin Source File

SOURCE=..\conv.c
# End Source File
# Begin Source File

SOURCE=..\debug.c
# End Source File
# Begin Source File

SOURCE=.\dll.c
# End Source File
# Begin Source File

SOURCE=..\event.c
# End Source File
# Begin Source File

SOURCE=..\fnmatch.c
# End Source File
# Begin Source File

SOURCE=..\htimerNtp.c
# End Source File
# Begin Source File

SOURCE=.\librtps.def
# End Source File
# Begin Source File

SOURCE=..\objectEntry.c
# End Source File
# Begin Source File

SOURCE=..\objectEntryTimer.c
# End Source File
# Begin Source File

SOURCE=..\objectUtils.c
# End Source File
# Begin Source File

SOURCE=..\ORTEAppRecvThread.c
# End Source File
# Begin Source File

SOURCE=..\ORTEAppSendThread.c
# End Source File
# Begin Source File

SOURCE=..\ORTEDomain.c
# End Source File
# Begin Source File

SOURCE=..\ORTEDomainApp.c
# End Source File
# Begin Source File

SOURCE=..\ORTEDomainMgr.c
# End Source File
# Begin Source File

SOURCE=..\ORTEInit.c
# End Source File
# Begin Source File

SOURCE=..\ORTEMisc.c
# End Source File
# Begin Source File

SOURCE=..\ORTEPattern.c
# End Source File
# Begin Source File

SOURCE=..\ORTEPublication.c
# End Source File
# Begin Source File

SOURCE=..\ORTEPublicationTimer.c
# End Source File
# Begin Source File

SOURCE=..\ORTESubscription.c
# End Source File
# Begin Source File

SOURCE=..\ORTEThreadUtils.c
# End Source File
# Begin Source File

SOURCE=..\ORTETypeRegister.c
# End Source File
# Begin Source File

SOURCE=..\ORTEVerbosity.c
# End Source File
# Begin Source File

SOURCE=..\parameter.c
# End Source File
# Begin Source File

SOURCE=..\RTPSAck.c
# End Source File
# Begin Source File

SOURCE=..\RTPSCSTReader.c
# End Source File
# Begin Source File

SOURCE=..\RTPSCSTReaderProc.c
# End Source File
# Begin Source File

SOURCE=..\RTPSCSTReaderTimer.c
# End Source File
# Begin Source File

SOURCE=..\RTPSCSTWriter.c
# End Source File
# Begin Source File

SOURCE=..\RTPSCSTWriterTimer.c
# End Source File
# Begin Source File

SOURCE=..\RTPSGap.c
# End Source File
# Begin Source File

SOURCE=..\RTPSHeader.c
# End Source File
# Begin Source File

SOURCE=..\RTPSHeardBeat.c
# End Source File
# Begin Source File

SOURCE=..\RTPSInfoDST.c
# End Source File
# Begin Source File

SOURCE=..\RTPSInfoREPLY.c
# End Source File
# Begin Source File

SOURCE=..\RTPSInfoSRC.c
# End Source File
# Begin Source File

SOURCE=..\RTPSInfoTS.c
# End Source File
# Begin Source File

SOURCE=..\RTPSIssue.c
# End Source File
# Begin Source File

SOURCE=..\RTPSPad.c
# End Source File
# Begin Source File

SOURCE=..\RTPSUtils.c
# End Source File
# Begin Source File

SOURCE=..\RTPSVar.c
# End Source File
# Begin Source File

SOURCE=..\sock.c
# End Source File
# Begin Source File

SOURCE=..\ul_gavl.c
# End Source File
# Begin Source File

SOURCE=..\ul_gavlprim.c
# End Source File
# End Group
# End Target
# End Project
