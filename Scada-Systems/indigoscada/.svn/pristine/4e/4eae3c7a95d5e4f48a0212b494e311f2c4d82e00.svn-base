# Microsoft Developer Studio Project File - Name="opc_client_da_driver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=opc_client_da_driver - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opc_client_da_driver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opc_client_da_driver.mak" CFG="opc_client_da_driver - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opc_client_da_driver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "opc_client_da_driver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPC_CLIENT_DA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\trace" /I "..\..\utilities" /I "..\..\fifo" /I "..\..\configurator\sqlite" /I "..\..\middleware\ripc\inc" /I "..\..\middleware\rtps\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "OPC_CLIENT_DA_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /D "_WIN32_DCOM" /D "RIPC_DLL" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib scadad.lib utilitiesd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib uuid.lib WSOCK32.LIB snmpapi.lib qt-mt3.lib fifo.lib ripcclient.lib rtps.lib /nologo /dll /incremental:no /pdb:"Debug/opc_client_da_driver.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\scada\Drivers"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "$(QTDIR)\include" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\STL" /I "..\..\trace" /I "..\..\utilities" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "OPC_CLIENT_DA_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\trace" /I "..\..\utilities" /I "..\..\fifo" /I "..\..\configurator\sqlite" /I "..\..\middleware\ripc\inc" /I "..\..\middleware\rtps\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "OPC_CLIENT_DA_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /D "_WIN32_DCOM" /D "RIPC_DLL" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB scada.lib snmpapi.lib qtdll.lib /nologo /dll /pdb:"Debug/opc_client_da_driver.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 scada.lib utilities.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib uuid.lib WSOCK32.LIB snmpapi.lib qt-mt3.lib fifo.lib ripcclient.lib rtps.lib /nologo /dll /incremental:no /pdb:"Release/opc_client_da_driver.pdb" /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "opc_client_da_driver - Win32 Debug"
# Name "opc_client_da_driver - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\middleware\rtps\librtps\cdr.c
# End Source File
# Begin Source File

SOURCE=..\..\utilities\clear_crc_eight.c
# End Source File
# Begin Source File

SOURCE=..\..\common\iec_item_type.c
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_da.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_da_instance.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_daCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_daCommandData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_daConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_daConfigurationData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_daInput.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_opc_client_daInputData.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_da.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_da.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_da.h...
InputDir=.
InputPath=.\opc_client_da.h
InputName=opc_client_da

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_da.h...
InputDir=.
InputPath=.\opc_client_da.h
InputName=opc_client_da

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_da_instance.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_da_instance.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_da_instance.h...
InputDir=.
InputPath=.\opc_client_da_instance.h
InputName=opc_client_da_instance

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_da_instance.h...
InputDir=.
InputPath=.\opc_client_da_instance.h
InputName=opc_client_da_instance

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_daCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_daCommand.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daCommand.h...
InputDir=.
InputPath=.\opc_client_daCommand.h
InputName=opc_client_daCommand

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daCommand.h...
InputDir=.
InputPath=.\opc_client_daCommand.h
InputName=opc_client_daCommand

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_daCommandData.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_daCommandData.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daCommandData.h...
InputDir=.
InputPath=.\opc_client_daCommandData.h
InputName=opc_client_daCommandData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daCommandData.h...
InputDir=.
InputPath=.\opc_client_daCommandData.h
InputName=opc_client_daCommandData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_daConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_daConfiguration.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daConfiguration.h...
InputDir=.
InputPath=.\opc_client_daConfiguration.h
InputName=opc_client_daConfiguration

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daConfiguration.h...
InputDir=.
InputPath=.\opc_client_daConfiguration.h
InputName=opc_client_daConfiguration

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_daConfigurationData.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_daConfigurationData.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daConfigurationData.h...
InputDir=.
InputPath=.\opc_client_daConfigurationData.h
InputName=opc_client_daConfigurationData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daConfigurationData.h...
InputDir=.
InputPath=.\opc_client_daConfigurationData.h
InputName=opc_client_daConfigurationData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_dadriverthread.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_dadriverthread.h
# End Source File
# Begin Source File

SOURCE=.\opc_client_daInput.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_daInput.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daInput.h...
InputDir=.
InputPath=.\opc_client_daInput.h
InputName=opc_client_daInput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daInput.h...
InputDir=.
InputPath=.\opc_client_daInput.h
InputName=opc_client_daInput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opc_client_daInputData.cpp
# End Source File
# Begin Source File

SOURCE=.\opc_client_daInputData.h

!IF  "$(CFG)" == "opc_client_da_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daInputData.h...
InputDir=.
InputPath=.\opc_client_daInputData.h
InputName=opc_client_daInputData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "opc_client_da_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing opc_client_daInputData.h...
InputDir=.
InputPath=.\opc_client_daInputData.h
InputName=opc_client_daInputData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\configurator\sqlite\sqlite3.c
# End Source File
# End Group
# End Target
# End Project
