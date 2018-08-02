# Microsoft Developer Studio Project File - Name="mod_RSsim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mod_RSsim - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mod_RSsim.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mod_RSsim - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib ..\mylib\release\mylib.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "mod_RSsim - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_COMMS_DEBUGGING" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib ..\mylib\debug\mylib.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mod_RSsim - Win32 Release"
# Name "mod_RSsim - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_README.CPP
# End Source File
# Begin Source File

SOURCE=.\ABComms.cpp
# End Source File
# Begin Source File

SOURCE=.\ABCommsProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\About.cpp
# End Source File
# Begin Source File

SOURCE=.\ActiveScriptHost.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvEmulationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\CreditStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\CSVFileImportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CSVReader.cpp
# End Source File
# Begin Source File

SOURCE=.\DDKSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\DDKSrvSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\EasterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditMemoryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FaultsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\htmloutput.cpp
# End Source File
# Begin Source File

SOURCE=.\JoySCCEmulation.cpp
# End Source File
# Begin Source File

SOURCE=.\MemoryEditorList.cpp
# End Source File
# Begin Source File

SOURCE=.\MemWriteLock.cpp
# End Source File
# Begin Source File

SOURCE=.\message.cpp
# End Source File
# Begin Source File

SOURCE=.\MicroTick.cpp
# End Source File
# Begin Source File

SOURCE=.\MOD232CommsProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_RSsim.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_RSsim.odl
# End Source File
# Begin Source File

SOURCE=.\mod_RSsimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MODEthCommsProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\MyHostProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\PLCApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\RS232Noise.cpp
# End Source File
# Begin Source File

SOURCE=.\RS232Port.cpp
# End Source File
# Begin Source File

SOURCE=.\serverRS232array.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerSettings232Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerSettingsEthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerSocketArray.cpp
# End Source File
# Begin Source File

SOURCE=.\SimCmdLine.cpp
# End Source File
# Begin Source File

SOURCE=.\simport.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StarWarsCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TrainingSimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UTIL.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ABCommsProcessor.h
# End Source File
# Begin Source File

SOURCE=.\About.h
# End Source File
# Begin Source File

SOURCE=.\ActiveScriptHost.h
# End Source File
# Begin Source File

SOURCE=.\AdvEmulationDlg.h
# End Source File
# Begin Source File

SOURCE=.\AnimationSettings.h
# End Source File
# Begin Source File

SOURCE=.\CreditStatic.h
# End Source File
# Begin Source File

SOURCE=.\CSVFileImportDlg.h
# End Source File
# Begin Source File

SOURCE=.\DDKSocket.h
# End Source File
# Begin Source File

SOURCE=.\DDKSrvSocket.h
# End Source File
# Begin Source File

SOURCE=.\DiagnosticsMemFile.h
# End Source File
# Begin Source File

SOURCE=.\EasterDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditMemoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\FaultsDlg.h
# End Source File
# Begin Source File

SOURCE=.\HtmlOutput.h
# End Source File
# Begin Source File

SOURCE=.\JoySCCEmulation.h
# End Source File
# Begin Source File

SOURCE=.\MemoryEditorList.h
# End Source File
# Begin Source File

SOURCE=.\MemWriteLock.h
# End Source File
# Begin Source File

SOURCE=.\message.h
# End Source File
# Begin Source File

SOURCE=.\MicroTick.h
# End Source File
# Begin Source File

SOURCE=.\MOD232CommsProcessor.h
# End Source File
# Begin Source File

SOURCE=.\mod_RSsim.h
# End Source File
# Begin Source File

SOURCE=.\mod_RSsimDlg.h
# End Source File
# Begin Source File

SOURCE=.\MODCommsProcessor.h
# End Source File
# Begin Source File

SOURCE=.\MODEthCommsProcessor.h
# End Source File
# Begin Source File

SOURCE=.\MyHostProxy.h
# End Source File
# Begin Source File

SOURCE=.\plantsim.h
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RS232Noise.h
# End Source File
# Begin Source File

SOURCE=.\RS232Port.h
# End Source File
# Begin Source File

SOURCE=.\serverRS232array.h
# End Source File
# Begin Source File

SOURCE=.\ServerSettings232Dlg.h
# End Source File
# Begin Source File

SOURCE=.\ServerSettingsEthDlg.h
# End Source File
# Begin Source File

SOURCE=.\ServerSocketArray.h
# End Source File
# Begin Source File

SOURCE=.\SimCmdLine.h
# End Source File
# Begin Source File

SOURCE=.\simport.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StarWarsCtrl.h
# End Source File
# Begin Source File

SOURCE=.\StatLink.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TrainingSimDlg.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "Helpsystem"

# PROP Default_Filter "*,gif;*.htm"
# Begin Source File

SOURCE=.\help\images\advmbus.gif
# End Source File
# Begin Source File

SOURCE=.\help\anim.gif
# End Source File
# Begin Source File

SOURCE=.\help\images\anim.gif
# End Source File
# Begin Source File

SOURCE=.\help\index.htm
# End Source File
# Begin Source File

SOURCE=.\help\images\joyshot.gif
# End Source File
# Begin Source File

SOURCE=.\help\images\juice.gif
# End Source File
# Begin Source File

SOURCE=.\help\juice.gif
# End Source File
# Begin Source File

SOURCE=.\help\images\portA.gif
# End Source File
# Begin Source File

SOURCE=.\help\portA.gif
# End Source File
# Begin Source File

SOURCE=.\help\images\shot1.gif
# End Source File
# Begin Source File

SOURCE=.\res\shot1.gif
# End Source File
# Begin Source File

SOURCE=.\help\images\shot2.gif
# End Source File
# Begin Source File

SOURCE=.\help\shot2.gif
# End Source File
# Begin Source File

SOURCE=.\help\images\toolbar.gif
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\res\creditbm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\drag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\mod_RSsim.ico
# End Source File
# Begin Source File

SOURCE=.\mod_RSsim.rc
# End Source File
# Begin Source File

SOURCE=.\res\MOD_RSsim.rc2
# End Source File
# Begin Source File

SOURCE=.\res\mod_RSsimul.ico
# End Source File
# Begin Source File

SOURCE=.\res\multisla.bmp
# End Source File
# Begin Source File

SOURCE=.\myhandcu.cur
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# End Target
# End Project
