# Microsoft Developer Studio Project File - Name="qwt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=qwt - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qwt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qwt.mak" CFG="qwt - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qwt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qwt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib"
# PROP BASE Intermediate_Dir "src/obj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\scada\lib"
# PROP Intermediate_Dir "src/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD CPP /nologo /MD /W3 /GX /O1 /I "include" /I "$(QTDIR)\include" /I "src\moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /I ".\designer\pixmaps" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "QWT_DLL" /D "QWT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt3.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" delayimp.lib /nologo /subsystem:windows /dll /machine:IX86 /out:"c:\scada\bin/qwt.dll" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /I "$(QTDIR)\include" /I "src\moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /I ".\designer\pixmaps" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "QWT_DLL" /D "QWT_MAKEDLL" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt3.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" /nologo /subsystem:windows /dll /incremental:no /debug /machine:IX86 /out:"c:\scada\bin/qwt.dll" /implib:"c:\scada\lib\qwt.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "qwt - Win32 Release"
# Name "qwt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\double_point_led.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_elswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pdoublepointled.cpp
# End Source File
# Begin Source File

SOURCE=.\src\plcdnumber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pmeter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\psinglepointled.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ptank.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pthermometer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ptoggle.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_analog_clock.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_arrbtn.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_autoscl.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_compass.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_compass_rose.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_counter.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_curve.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_data.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_dial.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_dial_needle.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_dimap.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_double_rect.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_drange.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_dyngrid_layout.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_event_pattern.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_grid.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_knob.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_layout_metrics.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_legend.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_marker.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_math.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_paint_buffer.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_painter.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_picker.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_picker_machine.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_axis.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_canvas.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_curve.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_grid.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_item.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_layout.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_marker.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_picker.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_print.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_printfilter.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_plot_zoomer.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_push_button.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_rect.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_scale.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_scldiv.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_scldraw.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_sclif.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_sldbase.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_slider.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_spline.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_symbol.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_text.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_thermo.cpp
# End Source File
# Begin Source File

SOURCE=src\qwt_wheel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\single_point_led.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\double_point_led.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing double_point_led.h...
InputDir=.\include
InputPath=.\include\double_point_led.h
InputName=double_point_led

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing double_point_led.h...
InputDir=.\include
InputPath=.\include\double_point_led.h
InputName=double_point_led

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\elswitch.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing elswitch.h...
InputDir=.\include
InputPath=.\include\elswitch.h
InputName=elswitch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing elswitch.h...
InputDir=.\include
InputPath=.\include\elswitch.h
InputName=elswitch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\pdoublepointled.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pdoublepointled.h...
InputDir=.\include
InputPath=.\include\pdoublepointled.h
InputName=pdoublepointled

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pdoublepointled.h...
InputDir=.\include
InputPath=.\include\pdoublepointled.h
InputName=pdoublepointled

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\plcdnumber.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing plcdnumber.h...
InputDir=.\include
InputPath=.\include\plcdnumber.h
InputName=plcdnumber

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing plcdnumber.h...
InputDir=.\include
InputPath=.\include\plcdnumber.h
InputName=plcdnumber

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\pmeter.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pmeter.h...
InputDir=.\include
InputPath=.\include\pmeter.h
InputName=pmeter

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pmeter.h...
InputDir=.\include
InputPath=.\include\pmeter.h
InputName=pmeter

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\psinglepointled.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing psinglepointled.h...
InputDir=.\include
InputPath=.\include\psinglepointled.h
InputName=psinglepointled

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing psinglepointled.h...
InputDir=.\include
InputPath=.\include\psinglepointled.h
InputName=psinglepointled

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\pswitch.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pswitch.h...
InputDir=.\include
InputPath=.\include\pswitch.h
InputName=pswitch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pswitch.h...
InputDir=.\include
InputPath=.\include\pswitch.h
InputName=pswitch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\ptank.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing ptank.h...
InputDir=.\include
InputPath=.\include\ptank.h
InputName=ptank

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing ptank.h...
InputDir=.\include
InputPath=.\include\ptank.h
InputName=ptank

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\pthermometer.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pthermometer.h...
InputDir=.\include
InputPath=.\include\pthermometer.h
InputName=pthermometer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing pthermometer.h...
InputDir=.\include
InputPath=.\include\pthermometer.h
InputName=pthermometer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\ptoggle.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing ptoggle.h...
InputDir=.\include
InputPath=.\include\ptoggle.h
InputName=ptoggle

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing ptoggle.h...
InputDir=.\include
InputPath=.\include\ptoggle.h
InputName=ptoggle

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt.h
# End Source File
# Begin Source File

SOURCE=include\qwt_analog_clock.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_A="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_analog_clock.h...
InputPath=include\qwt_analog_clock.h

"src\moc\moc_qwt_analog_clock.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_analog_clock.h -o src\moc\moc_qwt_analog_clock.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_A="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_analog_clock.h...
InputPath=include\qwt_analog_clock.h

"src\moc\moc_qwt_analog_clock.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_analog_clock.h -o src\moc\moc_qwt_analog_clock.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_array.h
# End Source File
# Begin Source File

SOURCE=include\qwt_arrbtn.h
# End Source File
# Begin Source File

SOURCE=include\qwt_autoscl.h
# End Source File
# Begin Source File

SOURCE=include\qwt_compass.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_C="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_compass.h...
InputPath=include\qwt_compass.h

"src\moc\moc_qwt_compass.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_compass.h -o src\moc\moc_qwt_compass.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_C="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_compass.h...
InputPath=include\qwt_compass.h

"src\moc\moc_qwt_compass.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_compass.h -o src\moc\moc_qwt_compass.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_compass_rose.h
# End Source File
# Begin Source File

SOURCE=include\qwt_counter.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_CO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_counter.h...
InputPath=include\qwt_counter.h

"src\moc\moc_qwt_counter.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_counter.h -o src\moc\moc_qwt_counter.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_CO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_counter.h...
InputPath=include\qwt_counter.h

"src\moc\moc_qwt_counter.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_counter.h -o src\moc\moc_qwt_counter.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_curve.h
# End Source File
# Begin Source File

SOURCE=include\qwt_data.h
# End Source File
# Begin Source File

SOURCE=include\qwt_dial.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_D="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_dial.h...
InputPath=include\qwt_dial.h

"src\moc\moc_qwt_dial.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_dial.h -o src\moc\moc_qwt_dial.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_D="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_dial.h...
InputPath=include\qwt_dial.h

"src\moc\moc_qwt_dial.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_dial.h -o src\moc\moc_qwt_dial.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_dial_needle.h
# End Source File
# Begin Source File

SOURCE=include\qwt_dimap.h
# End Source File
# Begin Source File

SOURCE=include\qwt_double_rect.h
# End Source File
# Begin Source File

SOURCE=include\qwt_drange.h
# End Source File
# Begin Source File

SOURCE=include\qwt_dyngrid_layout.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_DY="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_dyngrid_layout.h...
InputPath=include\qwt_dyngrid_layout.h

"src\moc\moc_qwt_dyngrid_layout.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_dyngrid_layout.h -o src\moc\moc_qwt_dyngrid_layout.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_DY="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_dyngrid_layout.h...
InputPath=include\qwt_dyngrid_layout.h

"src\moc\moc_qwt_dyngrid_layout.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_dyngrid_layout.h -o src\moc\moc_qwt_dyngrid_layout.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_event_pattern.h
# End Source File
# Begin Source File

SOURCE=include\qwt_global.h
# End Source File
# Begin Source File

SOURCE=include\qwt_grid.h
# End Source File
# Begin Source File

SOURCE=include\qwt_knob.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_K="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_knob.h...
InputPath=include\qwt_knob.h

"src\moc\moc_qwt_knob.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_knob.h -o src\moc\moc_qwt_knob.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_K="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_knob.h...
InputPath=include\qwt_knob.h

"src\moc\moc_qwt_knob.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_knob.h -o src\moc\moc_qwt_knob.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_layout_metrics.h
# End Source File
# Begin Source File

SOURCE=include\qwt_legend.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_L="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_legend.h...
InputPath=include\qwt_legend.h

"src\moc\moc_qwt_legend.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_legend.h -o src\moc\moc_qwt_legend.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_L="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_legend.h...
InputPath=include\qwt_legend.h

"src\moc\moc_qwt_legend.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_legend.h -o src\moc\moc_qwt_legend.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_marker.h
# End Source File
# Begin Source File

SOURCE=include\qwt_math.h
# End Source File
# Begin Source File

SOURCE=include\qwt_paint_buffer.h
# End Source File
# Begin Source File

SOURCE=include\qwt_painter.h
# End Source File
# Begin Source File

SOURCE=include\qwt_picker.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_P="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_picker.h...
InputPath=include\qwt_picker.h

"src\moc\moc_qwt_picker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_picker.h -o src\moc\moc_qwt_picker.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_P="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_picker.h...
InputPath=include\qwt_picker.h

"src\moc\moc_qwt_picker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_picker.h -o src\moc\moc_qwt_picker.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_picker_machine.h
# End Source File
# Begin Source File

SOURCE=include\qwt_plot.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_PL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot.h...
InputPath=include\qwt_plot.h

"src\moc\moc_qwt_plot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot.h -o src\moc\moc_qwt_plot.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_PL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot.h...
InputPath=include\qwt_plot.h

"src\moc\moc_qwt_plot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot.h -o src\moc\moc_qwt_plot.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_plot_canvas.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_PLO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot_canvas.h...
InputPath=include\qwt_plot_canvas.h

"src\moc\moc_qwt_plot_canvas.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot_canvas.h -o src\moc\moc_qwt_plot_canvas.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_PLO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot_canvas.h...
InputPath=include\qwt_plot_canvas.h

"src\moc\moc_qwt_plot_canvas.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot_canvas.h -o src\moc\moc_qwt_plot_canvas.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_plot_classes.h
# End Source File
# Begin Source File

SOURCE=include\qwt_plot_dict.h
# End Source File
# Begin Source File

SOURCE=include\qwt_plot_item.h
# End Source File
# Begin Source File

SOURCE=include\qwt_plot_layout.h
# End Source File
# Begin Source File

SOURCE=include\qwt_plot_picker.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_PLOT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot_picker.h...
InputPath=include\qwt_plot_picker.h

"src\moc\moc_qwt_plot_picker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot_picker.h -o src\moc\moc_qwt_plot_picker.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_PLOT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot_picker.h...
InputPath=include\qwt_plot_picker.h

"src\moc\moc_qwt_plot_picker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot_picker.h -o src\moc\moc_qwt_plot_picker.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_plot_printfilter.h
# End Source File
# Begin Source File

SOURCE=include\qwt_plot_zoomer.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_PLOT_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot_zoomer.h...
InputPath=include\qwt_plot_zoomer.h

"src\moc\moc_qwt_plot_zoomer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot_zoomer.h -o src\moc\moc_qwt_plot_zoomer.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_PLOT_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_plot_zoomer.h...
InputPath=include\qwt_plot_zoomer.h

"src\moc\moc_qwt_plot_zoomer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_plot_zoomer.h -o src\moc\moc_qwt_plot_zoomer.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_push_button.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_PU="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_push_button.h...
InputPath=include\qwt_push_button.h

"src\moc\moc_qwt_push_button.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_push_button.h -o src\moc\moc_qwt_push_button.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_PU="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_push_button.h...
InputPath=include\qwt_push_button.h

"src\moc\moc_qwt_push_button.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_push_button.h -o src\moc\moc_qwt_push_button.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_rect.h
# End Source File
# Begin Source File

SOURCE=include\qwt_scale.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_S="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_scale.h...
InputPath=include\qwt_scale.h

"src\moc\moc_qwt_scale.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_scale.h -o src\moc\moc_qwt_scale.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_S="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_scale.h...
InputPath=include\qwt_scale.h

"src\moc\moc_qwt_scale.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_scale.h -o src\moc\moc_qwt_scale.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_scldiv.h
# End Source File
# Begin Source File

SOURCE=include\qwt_scldraw.h
# End Source File
# Begin Source File

SOURCE=include\qwt_sclif.h
# End Source File
# Begin Source File

SOURCE=include\qwt_sldbase.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_SL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_sldbase.h...
InputPath=include\qwt_sldbase.h

"src\moc\moc_qwt_sldbase.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_sldbase.h -o src\moc\moc_qwt_sldbase.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_SL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_sldbase.h...
InputPath=include\qwt_sldbase.h

"src\moc\moc_qwt_sldbase.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_sldbase.h -o src\moc\moc_qwt_sldbase.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_slider.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_SLI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_slider.h...
InputPath=include\qwt_slider.h

"src\moc\moc_qwt_slider.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_slider.h -o src\moc\moc_qwt_slider.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_SLI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_slider.h...
InputPath=include\qwt_slider.h

"src\moc\moc_qwt_slider.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_slider.h -o src\moc\moc_qwt_slider.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_spline.h
# End Source File
# Begin Source File

SOURCE=include\qwt_symbol.h
# End Source File
# Begin Source File

SOURCE=include\qwt_text.h
# End Source File
# Begin Source File

SOURCE=include\qwt_thermo.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_T="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_thermo.h...
InputPath=include\qwt_thermo.h

"src\moc\moc_qwt_thermo.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_thermo.h -o src\moc\moc_qwt_thermo.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_T="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_thermo.h...
InputPath=include\qwt_thermo.h

"src\moc\moc_qwt_thermo.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_thermo.h -o src\moc\moc_qwt_thermo.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=include\qwt_wheel.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_W="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_wheel.h...
InputPath=include\qwt_wheel.h

"src\moc\moc_qwt_wheel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_wheel.h -o src\moc\moc_qwt_wheel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_W="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing include\qwt_wheel.h...
InputPath=include\qwt_wheel.h

"src\moc\moc_qwt_wheel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc include\qwt_wheel.h -o src\moc\moc_qwt_wheel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\single_point_led.h

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing single_point_led.h...
InputDir=.\include
InputPath=.\include\single_point_led.h
InputName=single_point_led

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing single_point_led.h...
InputDir=.\include
InputPath=.\include\single_point_led.h
InputName=single_point_led

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\moc_double_point_led.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_pdoublepointled.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_plcdnumber.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_pmeter.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_psinglepointled.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_pswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_ptank.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_pthermometer.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_ptoggle.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_analog_clock.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_compass.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_counter.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_dial.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_dyngrid_layout.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_knob.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_legend.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_picker.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_plot.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_plot_canvas.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_plot_picker.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_plot_zoomer.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_push_button.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_scale.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_sldbase.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_slider.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_thermo.cpp
# End Source File
# Begin Source File

SOURCE=src\moc\moc_qwt_wheel.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_single_point_led.cpp
# End Source File
# End Group
# End Target
# End Project
