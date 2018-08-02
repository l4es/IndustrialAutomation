# Microsoft Developer Studio Project File - Name="mqtt_client_driver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mqtt_client_driver - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mqtt_client_driver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mqtt_client_driver.mak" CFG="mqtt_client_driver - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mqtt_client_driver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mqtt_client_driver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\trace" /I "..\..\utilities" /I "..\..\fifo" /I "..\..\configurator\sqlite" /I "..\..\middleware\ripc\inc" /I "..\..\middleware\rtps\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "MQTT_CLIENT_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /D "RIPC_DLL" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib scadad.lib utilitiesd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib uuid.lib WSOCK32.LIB snmpapi.lib qt-mt3.lib fifo.lib ripcclient.lib rtps.lib /nologo /dll /incremental:no /pdb:"Debug/mqtt_client_driver.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\..\gigabase" /I "..\..\common" /I ".." /I "..\..\database" /I "..\..\ui\widgets" /I "..\..\trace" /I "..\..\utilities" /I "..\..\fifo" /I "..\..\configurator\sqlite" /I "..\..\middleware\ripc\inc" /I "..\..\middleware\rtps\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "MQTT_CLIENT_EXPORTS" /D "USING_GARRET" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_H_CPP" /D "RIPC_DLL" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib WSOCK32.LIB scada.lib snmpapi.lib qtdll.lib /nologo /dll /pdb:"Debug/mqtt_client_driver.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 scada.lib utilities.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib uuid.lib WSOCK32.LIB snmpapi.lib qt-mt3.lib fifo.lib ripcclient.lib rtps.lib /nologo /dll /incremental:no /pdb:"Release/mqtt_client_driver.pdb" /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "mqtt_client_driver - Win32 Debug"
# Name "mqtt_client_driver - Win32 Release"
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

SOURCE=.\moc_mqtt_client.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_client_instance.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_clientCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_clientCommandData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_clientConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_clientConfigurationData.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_clientInput.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mqtt_clientInputData.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_client.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_client.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_client.h...
InputDir=.
InputPath=.\mqtt_client.h
InputName=mqtt_client

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_client.h...
InputDir=.
InputPath=.\mqtt_client.h
InputName=mqtt_client

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_client_instance.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_client_instance.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_client_instance.h...
InputDir=.
InputPath=.\mqtt_client_instance.h
InputName=mqtt_client_instance

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_client_instance.h...
InputDir=.
InputPath=.\mqtt_client_instance.h
InputName=mqtt_client_instance

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_clientCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientCommand.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientCommand.h...
InputDir=.
InputPath=.\mqtt_clientCommand.h
InputName=mqtt_clientCommand

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientCommand.h...
InputDir=.
InputPath=.\mqtt_clientCommand.h
InputName=mqtt_clientCommand

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_clientCommandData.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientCommandData.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientCommandData.h...
InputDir=.
InputPath=.\mqtt_clientCommandData.h
InputName=mqtt_clientCommandData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientCommandData.h...
InputDir=.
InputPath=.\mqtt_clientCommandData.h
InputName=mqtt_clientCommandData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_clientConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientConfiguration.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientConfiguration.h...
InputDir=.
InputPath=.\mqtt_clientConfiguration.h
InputName=mqtt_clientConfiguration

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientConfiguration.h...
InputDir=.
InputPath=.\mqtt_clientConfiguration.h
InputName=mqtt_clientConfiguration

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_clientConfigurationData.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientConfigurationData.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientConfigurationData.h...
InputDir=.
InputPath=.\mqtt_clientConfigurationData.h
InputName=mqtt_clientConfigurationData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientConfigurationData.h...
InputDir=.
InputPath=.\mqtt_clientConfigurationData.h
InputName=mqtt_clientConfigurationData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_clientdriverthread.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientdriverthread.h
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientInput.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientInput.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientInput.h...
InputDir=.
InputPath=.\mqtt_clientInput.h
InputName=mqtt_clientInput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientInput.h...
InputDir=.
InputPath=.\mqtt_clientInput.h
InputName=mqtt_clientInput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mqtt_clientInputData.cpp
# End Source File
# Begin Source File

SOURCE=.\mqtt_clientInputData.h

!IF  "$(CFG)" == "mqtt_client_driver - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientInputData.h...
InputDir=.
InputPath=.\mqtt_clientInputData.h
InputName=mqtt_clientInputData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "mqtt_client_driver - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing mqtt_clientInputData.h...
InputDir=.
InputPath=.\mqtt_clientInputData.h
InputName=mqtt_clientInputData

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
