# Microsoft Developer Studio Project File - Name="qwtplugin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=qwtplugin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qwtplugin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qwtplugin.mak" CFG="qwtplugin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qwtplugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qwtplugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qwtplugin - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\include" /I "$(QTDIR)\include" /I "moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /I ".\pixmaps" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "QT_PLUGIN" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt3.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "qwt.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" delayimp.lib /nologo /subsystem:windows /dll /machine:IX86 /out:"C:\scada\plugins\designer\qwtplugin.dll" /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "qwtplugin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "plugins/designer"
# PROP BASE Intermediate_Dir "obj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "plugins/designer"
# PROP Intermediate_Dir "obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "$(QTDIR)\include" /I "moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /I ".\pixmaps" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "QT_PLUGIN" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt3.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "qwt.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" /nologo /subsystem:windows /dll /incremental:no /debug /machine:IX86 /out:"C:\scada\plugins\designer\qwtplugin.dll" /implib:"Debug/qwtplugin.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"c:\scada\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "qwtplugin - Win32 Release"
# Name "qwtplugin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=qwtplugin.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=qwtplugin.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Images"

# PROP Default_Filter "png jpeg bmp xpm"
# Begin Source File

SOURCE=pixmaps/qwtanalogclock.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtcompass.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtcounter.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtdial.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtknob.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtplot.png

!IF  "$(CFG)" == "qwtplugin - Win32 Release"

USERDEP__QWTPL="pixmaps/qwtplot.png"	"pixmaps/qwtanalogclock.png"	"pixmaps/qwtcounter.png"	"pixmaps/qwtcompass.png"	"pixmaps/qwtdial.png"	"pixmaps/qwtknob.png"	"pixmaps/qwtpushbutton.png"	"pixmaps/qwtscale.png"	"pixmaps/qwtslider.png"	"pixmaps/qwtthermo.png"	"pixmaps/qwtwheel.png"	"pixmaps/qwtwidget.png"	
# Begin Custom Build - Creating image collection...
InputPath=pixmaps/qwtplot.png

"qmake_image_collection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic -embed qwtplugin -f images.tmp -o qmake_image_collection.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwtplugin - Win32 Debug"

USERDEP__QWTPL="pixmaps/qwtplot.png"	"pixmaps/qwtanalogclock.png"	"pixmaps/qwtcounter.png"	"pixmaps/qwtcompass.png"	"pixmaps/qwtdial.png"	"pixmaps/qwtknob.png"	"pixmaps/qwtpushbutton.png"	"pixmaps/qwtscale.png"	"pixmaps/qwtslider.png"	"pixmaps/qwtthermo.png"	"pixmaps/qwtwheel.png"	"pixmaps/qwtwidget.png"	
# Begin Custom Build - Creating image collection...
InputPath=pixmaps/qwtplot.png

"qmake_image_collection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic -embed qwtplugin -f images.tmp -o qmake_image_collection.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=pixmaps/qwtpushbutton.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtscale.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtslider.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtthermo.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtwheel.png
# End Source File
# Begin Source File

SOURCE=pixmaps/qwtwidget.png
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=qmake_image_collection.cpp
# End Source File
# End Group
# End Target
# End Project
