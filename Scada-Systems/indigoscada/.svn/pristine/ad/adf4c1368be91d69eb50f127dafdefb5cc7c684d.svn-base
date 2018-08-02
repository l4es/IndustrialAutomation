# Microsoft Developer Studio Project File - Name="protocol_configurator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=protocol_configurator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "protocol_configurator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "protocol_configurator.mak" CFG="protocol_configurator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "protocol_configurator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "protocol_configurator - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\sqlite" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib qt-mt3.lib qtmain.lib /nologo /subsystem:windows /pdb:"Release/protocol_configurator.pdb" /machine:I386 /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\sqlite" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib qt-mt3.lib qtmain.lib /nologo /subsystem:windows /incremental:no /pdb:"Debug/protocol_configurator.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "protocol_configurator - Win32 Release"
# Name "protocol_configurator - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aboutform.cpp
# End Source File
# Begin Source File

SOURCE=.\aboutform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing aboutform.h...
InputDir=.
InputPath=.\aboutform.h
InputName=aboutform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing aboutform.h...
InputDir=.
InputPath=.\aboutform.h
InputName=aboutform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aboutform.ui.h
# End Source File
# Begin Source File

SOURCE=.\addfieldform.cpp
# End Source File
# Begin Source File

SOURCE=.\addfieldform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing addfieldform.h...
InputDir=.
InputPath=.\addfieldform.h
InputName=addfieldform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing addfieldform.h...
InputDir=.
InputPath=.\addfieldform.h
InputName=addfieldform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\addfieldform.ui.h
# End Source File
# Begin Source File

SOURCE=.\addfieldtypeform.cpp
# End Source File
# Begin Source File

SOURCE=.\addfieldtypeform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing addfieldtypeform.h...
InputDir=.
InputPath=.\addfieldtypeform.h
InputName=addfieldtypeform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing addfieldtypeform.h...
InputDir=.
InputPath=.\addfieldtypeform.h
InputName=addfieldtypeform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\browsermain.cpp
# End Source File
# Begin Source File

SOURCE=.\choosetableform.cpp
# End Source File
# Begin Source File

SOURCE=.\choosetableform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing choosetableform.h...
InputDir=.
InputPath=.\choosetableform.h
InputName=choosetableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing choosetableform.h...
InputDir=.
InputPath=.\choosetableform.h
InputName=choosetableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\choosetableform.ui.h
# End Source File
# Begin Source File

SOURCE=.\createindexform.cpp
# End Source File
# Begin Source File

SOURCE=.\createindexform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing createindexform.h...
InputDir=.
InputPath=.\createindexform.h
InputName=createindexform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing createindexform.h...
InputDir=.
InputPath=.\createindexform.h
InputName=createindexform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\createindexform.ui.h
# End Source File
# Begin Source File

SOURCE=.\createtableform.cpp
# End Source File
# Begin Source File

SOURCE=.\createtableform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing createtableform.h...
InputDir=.
InputPath=.\createtableform.h
InputName=createtableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing createtableform.h...
InputDir=.
InputPath=.\createtableform.h
InputName=createtableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\createtableform.ui.h
# End Source File
# Begin Source File

SOURCE=.\deleteindexform.cpp
# End Source File
# Begin Source File

SOURCE=.\deleteindexform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing deleteindexform.h...
InputDir=.
InputPath=.\deleteindexform.h
InputName=deleteindexform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing deleteindexform.h...
InputDir=.
InputPath=.\deleteindexform.h
InputName=deleteindexform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\deleteindexform.ui.h
# End Source File
# Begin Source File

SOURCE=.\deletetableform.cpp
# End Source File
# Begin Source File

SOURCE=.\deletetableform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing deletetableform.h...
InputDir=.
InputPath=.\deletetableform.h
InputName=deletetableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing deletetableform.h...
InputDir=.
InputPath=.\deletetableform.h
InputName=deletetableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\deletetableform.ui.h
# End Source File
# Begin Source File

SOURCE=.\editfieldform.cpp
# End Source File
# Begin Source File

SOURCE=.\editfieldform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing editfieldform.h...
InputDir=.
InputPath=.\editfieldform.h
InputName=editfieldform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing editfieldform.h...
InputDir=.
InputPath=.\editfieldform.h
InputName=editfieldform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editfieldform.ui.h
# End Source File
# Begin Source File

SOURCE=.\editform.cpp
# End Source File
# Begin Source File

SOURCE=.\editform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing editform.h...
InputDir=.
InputPath=.\editform.h
InputName=editform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing editform.h...
InputDir=.
InputPath=.\editform.h
InputName=editform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editform.ui.h
# End Source File
# Begin Source File

SOURCE=.\edittableform.cpp
# End Source File
# Begin Source File

SOURCE=.\edittableform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing edittableform.h...
InputDir=.
InputPath=.\edittableform.h
InputName=edittableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing edittableform.h...
InputDir=.
InputPath=.\edittableform.h
InputName=edittableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\edittableform.ui.h
# End Source File
# Begin Source File

SOURCE=.\exporttablecsvform.cpp
# End Source File
# Begin Source File

SOURCE=.\exporttablecsvform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing exporttablecsvform.h...
InputDir=.
InputPath=.\exporttablecsvform.h
InputName=exporttablecsvform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing exporttablecsvform.h...
InputDir=.
InputPath=.\exporttablecsvform.h
InputName=exporttablecsvform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\exporttablecsvform.ui.h
# End Source File
# Begin Source File

SOURCE=.\findform.cpp
# End Source File
# Begin Source File

SOURCE=.\findform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing findform.h...
InputDir=.
InputPath=.\findform.h
InputName=findform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing findform.h...
InputDir=.
InputPath=.\findform.h
InputName=findform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\findform.ui.h
# End Source File
# Begin Source File

SOURCE=.\form1.cpp
# End Source File
# Begin Source File

SOURCE=.\form1.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing form1.h...
InputDir=.
InputPath=.\form1.h
InputName=form1

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing form1.h...
InputDir=.
InputPath=.\form1.h
InputName=form1

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\form1.ui.h
# End Source File
# Begin Source File

SOURCE=.\importcsvform.cpp
# End Source File
# Begin Source File

SOURCE=.\importcsvform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing importcsvform.h...
InputDir=.
InputPath=.\importcsvform.h
InputName=importcsvform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing importcsvform.h...
InputDir=.
InputPath=.\importcsvform.h
InputName=importcsvform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\importcsvform.ui.h
# End Source File
# Begin Source File

SOURCE=.\moc_aboutform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_addfieldform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_addfieldtypeform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_choosetableform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_createindexform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_createtableform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_deleteindexform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_deletetableform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_editfieldform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_editform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_edittableform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_exporttablecsvform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_findform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_form1.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_importcsvform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_renametableform.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_sqllogform.cpp
# End Source File
# Begin Source File

SOURCE=.\qmake_image_collection.cpp
# End Source File
# Begin Source File

SOURCE=.\renametableform.cpp
# End Source File
# Begin Source File

SOURCE=.\renametableform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing renametableform.h...
InputDir=.
InputPath=.\renametableform.h
InputName=renametableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing renametableform.h...
InputDir=.
InputPath=.\renametableform.h
InputName=renametableform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\renametableform.ui.h
# End Source File
# Begin Source File

SOURCE=.\sqlbrowser_util.c
# End Source File
# Begin Source File

SOURCE=.\sqlbrowser_util.h
# End Source File
# Begin Source File

SOURCE=..\sqlite\sqlite3.c
# End Source File
# Begin Source File

SOURCE=.\sqlitebrowsertypes.h
# End Source File
# Begin Source File

SOURCE=.\sqlitedb.cpp
# End Source File
# Begin Source File

SOURCE=.\sqlitedb.h
# End Source File
# Begin Source File

SOURCE=.\sqllogform.cpp
# End Source File
# Begin Source File

SOURCE=.\sqllogform.h

!IF  "$(CFG)" == "protocol_configurator - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing sqllogform.h...
InputDir=.
InputPath=.\sqllogform.h
InputName=sqllogform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "protocol_configurator - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing sqllogform.h...
InputDir=.
InputPath=.\sqllogform.h
InputName=sqllogform

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sqllogform.ui.h
# End Source File
# Begin Source File

SOURCE=.\winapp.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\iconwin.ico
# End Source File
# End Target
# End Project
