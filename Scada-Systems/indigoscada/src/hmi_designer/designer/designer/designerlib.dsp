# Microsoft Developer Studio Project File - Name="designerlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=designerlib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "designerlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "designerlib.mak" CFG="designerlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "designerlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "designerlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "designerlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "C:\scada/lib"
# PROP BASE Intermediate_Dir "."
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\scada/lib"
# PROP Intermediate_Dir "tmp/obj/release-shared-mt"
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /O1 /I "..\shared" /I "..\uilib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release-shared-mt" /I "C:\Qt\3.3.8\mkspecs\win32-msvc" /FI"designer_pch.h" /D "WIN32" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "DESIGNER" /D "QT_INTERNAL_XML" /D "QT_INTERNAL_WORKSPACE" /D "QT_INTERNAL_ICONVIEW" /D "QT_INTERNAL_TABLE" /D QM_TEMPLATE_EXTERN_TABLE=extern /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_TABLET_SUPPORT" /D "QT_NO_DEBUG" /Yu"designer_pch.h" /FD -Zm200 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 "$(IntDir)\designerlib.obj" /nologo

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "..\shared" /I "..\uilib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release-shared-mt" /I "C:\Qt\3.3.8\mkspecs\win32-msvc" /FI"designer_pch.h" /D "WIN32" /D "_DEBUG" /D "_LIB" /D "UNICODE" /D "DESIGNER" /D "QT_INTERNAL_XML" /D "QT_INTERNAL_WORKSPACE" /D "QT_INTERNAL_ICONVIEW" /D "QT_INTERNAL_TABLE" /D QM_TEMPLATE_EXTERN_TABLE=extern /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_TABLET_SUPPORT" /Yu"designer_pch.h" /FD /GZ -Zm200 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 "$(IntDir)\designerlib.obj" /nologo /out:"C:\scada\lib\designerlib.lib"

!ENDIF 

# Begin Target

# Name "designerlib - Win32 Release"
# Name "designerlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=actiondnd.cpp

!IF  "$(CFG)" == "designerlib - Win32 Release"

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=actioneditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=actionlistview.cpp
# End Source File
# Begin Source File

SOURCE=asciivalidator.cpp
# End Source File
# Begin Source File

SOURCE=command.cpp
# End Source File
# Begin Source File

SOURCE=configtoolboxdialog.ui.h
# End Source File
# Begin Source File

SOURCE=connectiondialog.ui.h
# End Source File
# Begin Source File

SOURCE=connectionitems.cpp
# End Source File
# Begin Source File

SOURCE=connectiontable.cpp
# End Source File
# Begin Source File

SOURCE=customwidgeteditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=database.cpp
# End Source File
# Begin Source File

SOURCE=dbconnectionimpl.cpp
# End Source File
# Begin Source File

SOURCE=dbconnectionsimpl.cpp
# End Source File
# Begin Source File

SOURCE=defs.cpp
# End Source File
# Begin Source File

SOURCE=designerapp.cpp
# End Source File
# Begin Source File

SOURCE=designerappiface.cpp
# End Source File
# Begin Source File

SOURCE=editfunctionsimpl.cpp
# End Source File
# Begin Source File

SOURCE=finddialog.ui.h
# End Source File
# Begin Source File

SOURCE=formfile.cpp
# End Source File
# Begin Source File

SOURCE=formsettingsimpl.cpp
# End Source File
# Begin Source File

SOURCE=formwindow.cpp
# End Source File
# Begin Source File

SOURCE=gotolinedialog.ui.h
# End Source File
# Begin Source File

SOURCE=hierarchyview.cpp
# End Source File
# Begin Source File

SOURCE=iconvieweditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=layout.cpp
# End Source File
# Begin Source File

SOURCE=listboxdnd.cpp
# End Source File
# Begin Source File

SOURCE=listboxeditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=listboxrename.cpp
# End Source File
# Begin Source File

SOURCE=listdnd.cpp
# End Source File
# Begin Source File

SOURCE=listeditor.ui.h
# End Source File
# Begin Source File

SOURCE=listviewdnd.cpp
# End Source File
# Begin Source File

SOURCE=listvieweditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=mainwindowactions.cpp
# End Source File
# Begin Source File

SOURCE=menubareditor.cpp
# End Source File
# Begin Source File

SOURCE=metadatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_actiondnd.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_actioneditor.cpp
# End Source File
# Begin Source File

SOURCE=multilineeditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=newformimpl.cpp
# End Source File
# Begin Source File

SOURCE=orderindicator.cpp
# End Source File
# Begin Source File

SOURCE=outputwindow.cpp
# End Source File
# Begin Source File

SOURCE=paletteeditoradvancedimpl.cpp
# End Source File
# Begin Source File

SOURCE=paletteeditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\parser.cpp
# End Source File
# Begin Source File

SOURCE=pixmapchooser.cpp
# End Source File
# Begin Source File

SOURCE=pixmapcollection.cpp
# End Source File
# Begin Source File

SOURCE=pixmapcollectioneditor.ui.h
# End Source File
# Begin Source File

SOURCE=popupmenueditor.cpp
# End Source File
# Begin Source File

SOURCE=previewframe.cpp
# End Source File
# Begin Source File

SOURCE=previewwidgetimpl.cpp
# End Source File
# Begin Source File

SOURCE=project.cpp
# End Source File
# Begin Source File

SOURCE=projectsettingsimpl.cpp
# End Source File
# Begin Source File

SOURCE=propertyeditor.cpp
# End Source File
# Begin Source File

SOURCE=propertyobject.cpp
# End Source File
# Begin Source File

SOURCE=qcompletionedit.cpp
# End Source File
# Begin Source File

SOURCE=.\qmake_image_collection.cpp
# End Source File
# Begin Source File

SOURCE=replacedialog.ui.h
# End Source File
# Begin Source File

SOURCE=resource.cpp
# End Source File
# Begin Source File

SOURCE=richtextfontdialog.ui.h
# End Source File
# Begin Source File

SOURCE=sizehandle.cpp
# End Source File
# Begin Source File

SOURCE=sourceeditor.cpp
# End Source File
# Begin Source File

SOURCE=sourcefile.cpp
# End Source File
# Begin Source File

SOURCE=startdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=styledbutton.cpp
# End Source File
# Begin Source File

SOURCE=syntaxhighlighter_html.cpp
# End Source File
# Begin Source File

SOURCE=tableeditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=timestamp.cpp
# End Source File
# Begin Source File

SOURCE=variabledialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=widgetaction.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\widgetdatabase.cpp
# End Source File
# Begin Source File

SOURCE=widgetfactory.cpp
# End Source File
# Begin Source File

SOURCE=wizardeditorimpl.cpp
# End Source File
# Begin Source File

SOURCE=workspace.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=actiondnd.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing actiondnd.h...
InputDir=.
InputPath=actiondnd.h
InputName=actiondnd

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing actiondnd.h...
InputDir=.
InputPath=actiondnd.h
InputName=actiondnd

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=actioneditor.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing actioneditor.h...
InputDir=.
InputPath=actioneditor.h
InputName=actioneditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing actioneditor.h...
InputDir=.
InputPath=actioneditor.h
InputName=actioneditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=actioneditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ACTIO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing actioneditorimpl.h...
InputPath=actioneditorimpl.h

"tmp\moc\release-shared-mt\moc_actioneditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc actioneditorimpl.h -o tmp\moc\release-shared-mt\moc_actioneditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ACTIO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing actioneditorimpl.h...
InputPath=actioneditorimpl.h

"tmp\moc\release-shared-mt\moc_actioneditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc actioneditorimpl.h -o tmp\moc\release-shared-mt\moc_actioneditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\interfaces\actioninterface.h
# End Source File
# Begin Source File

SOURCE=actionlistview.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ACTION=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing actionlistview.h...
InputPath=actionlistview.h

"tmp\moc\release-shared-mt\moc_actionlistview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc actionlistview.h -o tmp\moc\release-shared-mt\moc_actionlistview.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ACTION=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing actionlistview.h...
InputPath=actionlistview.h

"tmp\moc\release-shared-mt\moc_actionlistview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc actionlistview.h -o tmp\moc\release-shared-mt\moc_actionlistview.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=asciivalidator.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ASCII=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing asciivalidator.h...
InputPath=asciivalidator.h

"tmp\moc\release-shared-mt\moc_asciivalidator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc asciivalidator.h -o tmp\moc\release-shared-mt\moc_asciivalidator.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ASCII=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing asciivalidator.h...
InputPath=asciivalidator.h

"tmp\moc\release-shared-mt\moc_asciivalidator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc asciivalidator.h -o tmp\moc\release-shared-mt\moc_asciivalidator.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=command.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__COMMA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing command.h...
InputPath=command.h

"tmp\moc\release-shared-mt\moc_command.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc command.h -o tmp\moc\release-shared-mt\moc_command.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__COMMA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing command.h...
InputPath=command.h

"tmp\moc\release-shared-mt\moc_command.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc command.h -o tmp\moc\release-shared-mt\moc_command.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=connectionitems.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CONNE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing connectionitems.h...
InputPath=connectionitems.h

"tmp\moc\release-shared-mt\moc_connectionitems.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc connectionitems.h -o tmp\moc\release-shared-mt\moc_connectionitems.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CONNE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing connectionitems.h...
InputPath=connectionitems.h

"tmp\moc\release-shared-mt\moc_connectionitems.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc connectionitems.h -o tmp\moc\release-shared-mt\moc_connectionitems.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=connectiontable.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CONNEC=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing connectiontable.h...
InputPath=connectiontable.h

"tmp\moc\release-shared-mt\moc_connectiontable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc connectiontable.h -o tmp\moc\release-shared-mt\moc_connectiontable.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CONNEC=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing connectiontable.h...
InputPath=connectiontable.h

"tmp\moc\release-shared-mt\moc_connectiontable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc connectiontable.h -o tmp\moc\release-shared-mt\moc_connectiontable.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=customwidgeteditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CUSTO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing customwidgeteditorimpl.h...
InputPath=customwidgeteditorimpl.h

"tmp\moc\release-shared-mt\moc_customwidgeteditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc customwidgeteditorimpl.h -o tmp\moc\release-shared-mt\moc_customwidgeteditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CUSTO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing customwidgeteditorimpl.h...
InputPath=customwidgeteditorimpl.h

"tmp\moc\release-shared-mt\moc_customwidgeteditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc customwidgeteditorimpl.h -o tmp\moc\release-shared-mt\moc_customwidgeteditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=database.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DATAB=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing database.h...
InputPath=database.h

"tmp\moc\release-shared-mt\moc_database.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc database.h -o tmp\moc\release-shared-mt\moc_database.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DATAB=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing database.h...
InputPath=database.h

"tmp\moc\release-shared-mt\moc_database.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc database.h -o tmp\moc\release-shared-mt\moc_database.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=dbconnectionimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DBCON=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing dbconnectionimpl.h...
InputPath=dbconnectionimpl.h

"tmp\moc\release-shared-mt\moc_dbconnectionimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc dbconnectionimpl.h -o tmp\moc\release-shared-mt\moc_dbconnectionimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DBCON=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing dbconnectionimpl.h...
InputPath=dbconnectionimpl.h

"tmp\moc\release-shared-mt\moc_dbconnectionimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc dbconnectionimpl.h -o tmp\moc\release-shared-mt\moc_dbconnectionimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=dbconnectionsimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DBCONN=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing dbconnectionsimpl.h...
InputPath=dbconnectionsimpl.h

"tmp\moc\release-shared-mt\moc_dbconnectionsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc dbconnectionsimpl.h -o tmp\moc\release-shared-mt\moc_dbconnectionsimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DBCONN=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing dbconnectionsimpl.h...
InputPath=dbconnectionsimpl.h

"tmp\moc\release-shared-mt\moc_dbconnectionsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc dbconnectionsimpl.h -o tmp\moc\release-shared-mt\moc_dbconnectionsimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=defs.h
# End Source File
# Begin Source File

SOURCE=designer_pch.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DESIG="..\..\..\include\private\qucomextra_p.h"	"..\..\..\include\qaction.h"	"..\..\..\include\qapplication.h"	"..\..\..\include\qcheckbox.h"	"..\..\..\include\qcombobox.h"	"..\..\..\include\qdialog.h"	"..\..\..\include\qheader.h"	"..\..\..\include\qlabel.h"	"..\..\..\include\qlayout.h"	"..\..\..\include\qlineedit.h"	"..\..\..\include\qlistbox.h"	"..\..\..\include\qlistview.h"	"..\..\..\include\qmap.h"	"..\..\..\include\qmessagebox.h"	"..\..\..\include\qmetaobject.h"	"..\..\..\include\qobjectlist.h"	"..\..\..\include\qpainter.h"	"..\..\..\include\qpixmap.h"	"..\..\..\include\qpushbutton.h"	"..\..\..\include\qspinbox.h"	"..\..\..\include\qstring.h"	"..\..\..\include\qtooltip.h"	"..\..\..\include\qvariant.h"	"..\..\..\include\qwhatsthis.h"	"..\..\..\include\qwidget.h"	"command.h"	"formwindow.h"	"mainwindow.h"	"metadatabase.h"	"project.h"	"widgetfactory.h"	"..\..\..\include\private\qucom_p.h"	"..\..\..\include\quuid.h"	"..\..\..\include\qcstring.h"	"..\..\..\include\qwinexport.h"	"..\..\..\include\qmemarray.h"	"..\..\..\include\qgarray.h"	"..\..\..\include\qshared.h"	"..\..\..\include\qglobal.h"	"..\..\..\include\qconfig-minimal.h"	"..\..\..\include\qconfig-small.h"\
	"..\..\..\include\qconfig-medium.h"	"..\..\..\include\qconfig-large.h"	"..\..\..\include\qconfig.h"	"..\..\..\include\qmodules.h"	"..\..\..\include\qfeatures.h"	"..\..\..\include\qobject.h"	"..\..\..\include\qiconset.h"	"..\..\..\include\qkeysequence.h"	"..\..\..\include\qobjectdefs.h"	"..\..\..\include\qwindowdefs.h"	"..\..\..\include\qevent.h"	"..\..\..\include\qnamespace.h"	"..\..\..\include\qwindowdefs_win.h"	"..\..\..\include\qregion.h"	"..\..\..\include\qmime.h"	"..\..\..\include\qpair.h"	"..\..\..\include\qrect.h"	"..\..\..\include\qsize.h"	"..\..\..\include\qpoint.h"	"..\..\..\include\qptrlist.h"	"..\..\..\include\qdatastream.h"	"..\..\..\include\qvaluelist.h"	"..\..\..\include\qiodevice.h"	"..\..\..\include\qtl.h"	"..\..\..\include\qtextstream.h"	"..\..\..\include\qglist.h"	"..\..\..\include\qptrcollection.h"	"..\..\..\include\qpa"	
# Begin Custom Build - Creating PCH cpp from designer_pch.h...
IntDir=.\tmp/obj/release-shared-mt
InputPath=designer_pch.h

"$(IntDir)\designerlib.pch" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /TP /W3 /FD /c /D "WIN32" /Yc /Fp""$(IntDir)\designerlib.pch"" /Fo""$(IntDir)\designerlib.obj"" /I "..\shared" /I "..\uilib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release-shared-mt" /I "C:\Qt\3.3.8\mkspecs\win32-msvc" /D UNICODE /D DESIGNER /D QT_INTERNAL_XML /D QT_INTERNAL_WORKSPACE /D QT_INTERNAL_ICONVIEW /D QT_INTERNAL_TABLE /D QM_TEMPLATE_EXTERN_TABLE=extern /D QT_DLL /D QT_THREAD_SUPPORT /D QT_ACCESSIBILITY_SUPPORT /D QT_TABLET_SUPPORT -nologo -Zm200 -GX -GX -GR /D "NDEBUG" -O1 -MD /D "QT_NO_DEBUG" /D "_LIB" /Fd"$(IntDir)\\" designer_pch.h

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DESIG="..\..\..\include\private\qucomextra_p.h"	"..\..\..\include\qaction.h"	"..\..\..\include\qapplication.h"	"..\..\..\include\qcheckbox.h"	"..\..\..\include\qcombobox.h"	"..\..\..\include\qdialog.h"	"..\..\..\include\qheader.h"	"..\..\..\include\qlabel.h"	"..\..\..\include\qlayout.h"	"..\..\..\include\qlineedit.h"	"..\..\..\include\qlistbox.h"	"..\..\..\include\qlistview.h"	"..\..\..\include\qmap.h"	"..\..\..\include\qmessagebox.h"	"..\..\..\include\qmetaobject.h"	"..\..\..\include\qobjectlist.h"	"..\..\..\include\qpainter.h"	"..\..\..\include\qpixmap.h"	"..\..\..\include\qpushbutton.h"	"..\..\..\include\qspinbox.h"	"..\..\..\include\qstring.h"	"..\..\..\include\qtooltip.h"	"..\..\..\include\qvariant.h"	"..\..\..\include\qwhatsthis.h"	"..\..\..\include\qwidget.h"	"command.h"	"formwindow.h"	"mainwindow.h"	"metadatabase.h"	"project.h"	"widgetfactory.h"	"..\..\..\include\private\qucom_p.h"	"..\..\..\include\quuid.h"	"..\..\..\include\qcstring.h"	"..\..\..\include\qwinexport.h"	"..\..\..\include\qmemarray.h"	"..\..\..\include\qgarray.h"	"..\..\..\include\qshared.h"	"..\..\..\include\qglobal.h"	"..\..\..\include\qconfig-minimal.h"	"..\..\..\include\qconfig-small.h"\
	"..\..\..\include\qconfig-medium.h"	"..\..\..\include\qconfig-large.h"	"..\..\..\include\qconfig.h"	"..\..\..\include\qmodules.h"	"..\..\..\include\qfeatures.h"	"..\..\..\include\qobject.h"	"..\..\..\include\qiconset.h"	"..\..\..\include\qkeysequence.h"	"..\..\..\include\qobjectdefs.h"	"..\..\..\include\qwindowdefs.h"	"..\..\..\include\qevent.h"	"..\..\..\include\qnamespace.h"	"..\..\..\include\qwindowdefs_win.h"	"..\..\..\include\qregion.h"	"..\..\..\include\qmime.h"	"..\..\..\include\qpair.h"	"..\..\..\include\qrect.h"	"..\..\..\include\qsize.h"	"..\..\..\include\qpoint.h"	"..\..\..\include\qptrlist.h"	"..\..\..\include\qdatastream.h"	"..\..\..\include\qvaluelist.h"	"..\..\..\include\qiodevice.h"	"..\..\..\include\qtl.h"	"..\..\..\include\qtextstream.h"	"..\..\..\include\qglist.h"	"..\..\..\include\qptrcollection.h"	"..\..\..\include\qpa"	
# Begin Custom Build - Creating PCH cpp from designer_pch.h...
IntDir=.\Debug
InputPath=designer_pch.h

"$(IntDir)\designerlib.pch" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /TP /W3 /FD /c /D "WIN32" /Yc /Fp""$(IntDir)\designerlib.pch"" /Fo""$(IntDir)\designerlib.obj"" /I "..\shared" /I "..\uilib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release-shared-mt" /I "C:\Qt\3.3.8\mkspecs\win32-msvc" /D UNICODE /D DESIGNER /D QT_INTERNAL_XML /D QT_INTERNAL_WORKSPACE /D QT_INTERNAL_ICONVIEW /D QT_INTERNAL_TABLE /D QM_TEMPLATE_EXTERN_TABLE=extern /D QT_DLL /D QT_THREAD_SUPPORT /D QT_ACCESSIBILITY_SUPPORT /D QT_TABLET_SUPPORT -nologo -Zm200 -GX -GX -GR /D "_DEBUG" /Od -Zi -MDd /GZ /ZI /D "_LIB" /Fd"$(IntDir)\\" designer_pch.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=designerapp.h
# End Source File
# Begin Source File

SOURCE=designerappiface.h
# End Source File
# Begin Source File

SOURCE=..\interfaces\designerinterface.h
# End Source File
# Begin Source File

SOURCE=editfunctionsimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__EDITF=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing editfunctionsimpl.h...
InputPath=editfunctionsimpl.h

"tmp\moc\release-shared-mt\moc_editfunctionsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc editfunctionsimpl.h -o tmp\moc\release-shared-mt\moc_editfunctionsimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__EDITF=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing editfunctionsimpl.h...
InputPath=editfunctionsimpl.h

"tmp\moc\release-shared-mt\moc_editfunctionsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc editfunctionsimpl.h -o tmp\moc\release-shared-mt\moc_editfunctionsimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\interfaces\filterinterface.h
# End Source File
# Begin Source File

SOURCE=formfile.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__FORMF=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing formfile.h...
InputPath=formfile.h

"tmp\moc\release-shared-mt\moc_formfile.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc formfile.h -o tmp\moc\release-shared-mt\moc_formfile.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__FORMF=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing formfile.h...
InputPath=formfile.h

"tmp\moc\release-shared-mt\moc_formfile.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc formfile.h -o tmp\moc\release-shared-mt\moc_formfile.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=formsettingsimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__FORMS=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing formsettingsimpl.h...
InputPath=formsettingsimpl.h

"tmp\moc\release-shared-mt\moc_formsettingsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc formsettingsimpl.h -o tmp\moc\release-shared-mt\moc_formsettingsimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__FORMS=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing formsettingsimpl.h...
InputPath=formsettingsimpl.h

"tmp\moc\release-shared-mt\moc_formsettingsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc formsettingsimpl.h -o tmp\moc\release-shared-mt\moc_formsettingsimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=formwindow.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__FORMW=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing formwindow.h...
InputPath=formwindow.h

"tmp\moc\release-shared-mt\moc_formwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc formwindow.h -o tmp\moc\release-shared-mt\moc_formwindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__FORMW=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing formwindow.h...
InputPath=formwindow.h

"tmp\moc\release-shared-mt\moc_formwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc formwindow.h -o tmp\moc\release-shared-mt\moc_formwindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=hierarchyview.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__HIERA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing hierarchyview.h...
InputPath=hierarchyview.h

"tmp\moc\release-shared-mt\moc_hierarchyview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc hierarchyview.h -o tmp\moc\release-shared-mt\moc_hierarchyview.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__HIERA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing hierarchyview.h...
InputPath=hierarchyview.h

"tmp\moc\release-shared-mt\moc_hierarchyview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc hierarchyview.h -o tmp\moc\release-shared-mt\moc_hierarchyview.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=iconvieweditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ICONV=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing iconvieweditorimpl.h...
InputPath=iconvieweditorimpl.h

"tmp\moc\release-shared-mt\moc_iconvieweditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc iconvieweditorimpl.h -o tmp\moc\release-shared-mt\moc_iconvieweditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ICONV=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing iconvieweditorimpl.h...
InputPath=iconvieweditorimpl.h

"tmp\moc\release-shared-mt\moc_iconvieweditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc iconvieweditorimpl.h -o tmp\moc\release-shared-mt\moc_iconvieweditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=layout.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LAYOU=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing layout.h...
InputPath=layout.h

"tmp\moc\release-shared-mt\moc_layout.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc layout.h -o tmp\moc\release-shared-mt\moc_layout.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LAYOU=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing layout.h...
InputPath=layout.h

"tmp\moc\release-shared-mt\moc_layout.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc layout.h -o tmp\moc\release-shared-mt\moc_layout.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listboxdnd.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTB=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listboxdnd.h...
InputPath=listboxdnd.h

"tmp\moc\release-shared-mt\moc_listboxdnd.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listboxdnd.h -o tmp\moc\release-shared-mt\moc_listboxdnd.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTB=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listboxdnd.h...
InputPath=listboxdnd.h

"tmp\moc\release-shared-mt\moc_listboxdnd.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listboxdnd.h -o tmp\moc\release-shared-mt\moc_listboxdnd.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listboxeditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTBO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listboxeditorimpl.h...
InputPath=listboxeditorimpl.h

"tmp\moc\release-shared-mt\moc_listboxeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listboxeditorimpl.h -o tmp\moc\release-shared-mt\moc_listboxeditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTBO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listboxeditorimpl.h...
InputPath=listboxeditorimpl.h

"tmp\moc\release-shared-mt\moc_listboxeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listboxeditorimpl.h -o tmp\moc\release-shared-mt\moc_listboxeditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listboxrename.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTBOX=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listboxrename.h...
InputPath=listboxrename.h

"tmp\moc\release-shared-mt\moc_listboxrename.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listboxrename.h -o tmp\moc\release-shared-mt\moc_listboxrename.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTBOX=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listboxrename.h...
InputPath=listboxrename.h

"tmp\moc\release-shared-mt\moc_listboxrename.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listboxrename.h -o tmp\moc\release-shared-mt\moc_listboxrename.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listdnd.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTD=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listdnd.h...
InputPath=listdnd.h

"tmp\moc\release-shared-mt\moc_listdnd.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listdnd.h -o tmp\moc\release-shared-mt\moc_listdnd.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTD=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listdnd.h...
InputPath=listdnd.h

"tmp\moc\release-shared-mt\moc_listdnd.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listdnd.h -o tmp\moc\release-shared-mt\moc_listdnd.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listviewdnd.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTV=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listviewdnd.h...
InputPath=listviewdnd.h

"tmp\moc\release-shared-mt\moc_listviewdnd.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listviewdnd.h -o tmp\moc\release-shared-mt\moc_listviewdnd.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTV=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listviewdnd.h...
InputPath=listviewdnd.h

"tmp\moc\release-shared-mt\moc_listviewdnd.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listviewdnd.h -o tmp\moc\release-shared-mt\moc_listviewdnd.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listvieweditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTVI=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listvieweditorimpl.h...
InputPath=listvieweditorimpl.h

"tmp\moc\release-shared-mt\moc_listvieweditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listvieweditorimpl.h -o tmp\moc\release-shared-mt\moc_listvieweditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTVI=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing listvieweditorimpl.h...
InputPath=listvieweditorimpl.h

"tmp\moc\release-shared-mt\moc_listvieweditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc listvieweditorimpl.h -o tmp\moc\release-shared-mt\moc_listvieweditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=mainwindow.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__MAINW=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing mainwindow.h...
InputPath=mainwindow.h

"tmp\moc\release-shared-mt\moc_mainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc mainwindow.h -o tmp\moc\release-shared-mt\moc_mainwindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__MAINW=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing mainwindow.h...
InputPath=mainwindow.h

"tmp\moc\release-shared-mt\moc_mainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc mainwindow.h -o tmp\moc\release-shared-mt\moc_mainwindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=menubareditor.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__MENUB=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing menubareditor.h...
InputPath=menubareditor.h

"tmp\moc\release-shared-mt\moc_menubareditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc menubareditor.h -o tmp\moc\release-shared-mt\moc_menubareditor.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__MENUB=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing menubareditor.h...
InputPath=menubareditor.h

"tmp\moc\release-shared-mt\moc_menubareditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc menubareditor.h -o tmp\moc\release-shared-mt\moc_menubareditor.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=metadatabase.h
# End Source File
# Begin Source File

SOURCE=multilineeditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__MULTI=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing multilineeditorimpl.h...
InputPath=multilineeditorimpl.h

"tmp\moc\release-shared-mt\moc_multilineeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc multilineeditorimpl.h -o tmp\moc\release-shared-mt\moc_multilineeditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__MULTI=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing multilineeditorimpl.h...
InputPath=multilineeditorimpl.h

"tmp\moc\release-shared-mt\moc_multilineeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc multilineeditorimpl.h -o tmp\moc\release-shared-mt\moc_multilineeditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=newformimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__NEWFO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing newformimpl.h...
InputPath=newformimpl.h

"tmp\moc\release-shared-mt\moc_newformimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc newformimpl.h -o tmp\moc\release-shared-mt\moc_newformimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__NEWFO=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing newformimpl.h...
InputPath=newformimpl.h

"tmp\moc\release-shared-mt\moc_newformimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc newformimpl.h -o tmp\moc\release-shared-mt\moc_newformimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=orderindicator.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ORDER=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing orderindicator.h...
InputPath=orderindicator.h

"tmp\moc\release-shared-mt\moc_orderindicator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc orderindicator.h -o tmp\moc\release-shared-mt\moc_orderindicator.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ORDER=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing orderindicator.h...
InputPath=orderindicator.h

"tmp\moc\release-shared-mt\moc_orderindicator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc orderindicator.h -o tmp\moc\release-shared-mt\moc_orderindicator.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=outputwindow.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__OUTPU=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing outputwindow.h...
InputPath=outputwindow.h

"tmp\moc\release-shared-mt\moc_outputwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc outputwindow.h -o tmp\moc\release-shared-mt\moc_outputwindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__OUTPU=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing outputwindow.h...
InputPath=outputwindow.h

"tmp\moc\release-shared-mt\moc_outputwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc outputwindow.h -o tmp\moc\release-shared-mt\moc_outputwindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=paletteeditoradvancedimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PALET=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing paletteeditoradvancedimpl.h...
InputPath=paletteeditoradvancedimpl.h

"tmp\moc\release-shared-mt\moc_paletteeditoradvancedimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc paletteeditoradvancedimpl.h -o tmp\moc\release-shared-mt\moc_paletteeditoradvancedimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PALET=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing paletteeditoradvancedimpl.h...
InputPath=paletteeditoradvancedimpl.h

"tmp\moc\release-shared-mt\moc_paletteeditoradvancedimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc paletteeditoradvancedimpl.h -o tmp\moc\release-shared-mt\moc_paletteeditoradvancedimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=paletteeditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PALETT=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing paletteeditorimpl.h...
InputPath=paletteeditorimpl.h

"tmp\moc\release-shared-mt\moc_paletteeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc paletteeditorimpl.h -o tmp\moc\release-shared-mt\moc_paletteeditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PALETT=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing paletteeditorimpl.h...
InputPath=paletteeditorimpl.h

"tmp\moc\release-shared-mt\moc_paletteeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc paletteeditorimpl.h -o tmp\moc\release-shared-mt\moc_paletteeditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\parser.h
# End Source File
# Begin Source File

SOURCE=pixmapchooser.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PIXMA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing pixmapchooser.h...
InputPath=pixmapchooser.h

"tmp\moc\release-shared-mt\moc_pixmapchooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc pixmapchooser.h -o tmp\moc\release-shared-mt\moc_pixmapchooser.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PIXMA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing pixmapchooser.h...
InputPath=pixmapchooser.h

"tmp\moc\release-shared-mt\moc_pixmapchooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc pixmapchooser.h -o tmp\moc\release-shared-mt\moc_pixmapchooser.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=pixmapcollection.h
# End Source File
# Begin Source File

SOURCE=popupmenueditor.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__POPUP=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing popupmenueditor.h...
InputPath=popupmenueditor.h

"tmp\moc\release-shared-mt\moc_popupmenueditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc popupmenueditor.h -o tmp\moc\release-shared-mt\moc_popupmenueditor.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__POPUP=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing popupmenueditor.h...
InputPath=popupmenueditor.h

"tmp\moc\release-shared-mt\moc_popupmenueditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc popupmenueditor.h -o tmp\moc\release-shared-mt\moc_popupmenueditor.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=previewframe.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PREVI=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing previewframe.h...
InputPath=previewframe.h

"tmp\moc\release-shared-mt\moc_previewframe.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc previewframe.h -o tmp\moc\release-shared-mt\moc_previewframe.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PREVI=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing previewframe.h...
InputPath=previewframe.h

"tmp\moc\release-shared-mt\moc_previewframe.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc previewframe.h -o tmp\moc\release-shared-mt\moc_previewframe.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=previewwidgetimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PREVIE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing previewwidgetimpl.h...
InputPath=previewwidgetimpl.h

"tmp\moc\release-shared-mt\moc_previewwidgetimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc previewwidgetimpl.h -o tmp\moc\release-shared-mt\moc_previewwidgetimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PREVIE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing previewwidgetimpl.h...
InputPath=previewwidgetimpl.h

"tmp\moc\release-shared-mt\moc_previewwidgetimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc previewwidgetimpl.h -o tmp\moc\release-shared-mt\moc_previewwidgetimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=project.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PROJE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing project.h...
InputPath=project.h

"tmp\moc\release-shared-mt\moc_project.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc project.h -o tmp\moc\release-shared-mt\moc_project.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PROJE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing project.h...
InputPath=project.h

"tmp\moc\release-shared-mt\moc_project.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc project.h -o tmp\moc\release-shared-mt\moc_project.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=projectsettingsimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PROJEC=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing projectsettingsimpl.h...
InputPath=projectsettingsimpl.h

"tmp\moc\release-shared-mt\moc_projectsettingsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc projectsettingsimpl.h -o tmp\moc\release-shared-mt\moc_projectsettingsimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PROJEC=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing projectsettingsimpl.h...
InputPath=projectsettingsimpl.h

"tmp\moc\release-shared-mt\moc_projectsettingsimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc projectsettingsimpl.h -o tmp\moc\release-shared-mt\moc_projectsettingsimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=propertyeditor.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PROPE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing propertyeditor.h...
InputPath=propertyeditor.h

"tmp\moc\release-shared-mt\moc_propertyeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc propertyeditor.h -o tmp\moc\release-shared-mt\moc_propertyeditor.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PROPE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing propertyeditor.h...
InputPath=propertyeditor.h

"tmp\moc\release-shared-mt\moc_propertyeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc propertyeditor.h -o tmp\moc\release-shared-mt\moc_propertyeditor.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=propertyobject.h
# End Source File
# Begin Source File

SOURCE=qcompletionedit.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__QCOMP=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing qcompletionedit.h...
InputPath=qcompletionedit.h

"tmp\moc\release-shared-mt\moc_qcompletionedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qcompletionedit.h -o tmp\moc\release-shared-mt\moc_qcompletionedit.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__QCOMP=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing qcompletionedit.h...
InputPath=qcompletionedit.h

"tmp\moc\release-shared-mt\moc_qcompletionedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qcompletionedit.h -o tmp\moc\release-shared-mt\moc_qcompletionedit.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=resource.h
# End Source File
# Begin Source File

SOURCE=sizehandle.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__SIZEH=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing sizehandle.h...
InputPath=sizehandle.h

"tmp\moc\release-shared-mt\moc_sizehandle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc sizehandle.h -o tmp\moc\release-shared-mt\moc_sizehandle.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__SIZEH=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing sizehandle.h...
InputPath=sizehandle.h

"tmp\moc\release-shared-mt\moc_sizehandle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc sizehandle.h -o tmp\moc\release-shared-mt\moc_sizehandle.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=sourceeditor.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__SOURC=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing sourceeditor.h...
InputPath=sourceeditor.h

"tmp\moc\release-shared-mt\moc_sourceeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc sourceeditor.h -o tmp\moc\release-shared-mt\moc_sourceeditor.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__SOURC=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing sourceeditor.h...
InputPath=sourceeditor.h

"tmp\moc\release-shared-mt\moc_sourceeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc sourceeditor.h -o tmp\moc\release-shared-mt\moc_sourceeditor.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=sourcefile.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__SOURCE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing sourcefile.h...
InputPath=sourcefile.h

"tmp\moc\release-shared-mt\moc_sourcefile.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc sourcefile.h -o tmp\moc\release-shared-mt\moc_sourcefile.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__SOURCE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing sourcefile.h...
InputPath=sourcefile.h

"tmp\moc\release-shared-mt\moc_sourcefile.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc sourcefile.h -o tmp\moc\release-shared-mt\moc_sourcefile.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=startdialogimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__START=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing startdialogimpl.h...
InputPath=startdialogimpl.h

"tmp\moc\release-shared-mt\moc_startdialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc startdialogimpl.h -o tmp\moc\release-shared-mt\moc_startdialogimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__START=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing startdialogimpl.h...
InputPath=startdialogimpl.h

"tmp\moc\release-shared-mt\moc_startdialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc startdialogimpl.h -o tmp\moc\release-shared-mt\moc_startdialogimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=styledbutton.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__STYLE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing styledbutton.h...
InputPath=styledbutton.h

"tmp\moc\release-shared-mt\moc_styledbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc styledbutton.h -o tmp\moc\release-shared-mt\moc_styledbutton.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__STYLE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing styledbutton.h...
InputPath=styledbutton.h

"tmp\moc\release-shared-mt\moc_styledbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc styledbutton.h -o tmp\moc\release-shared-mt\moc_styledbutton.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=syntaxhighlighter_html.h
# End Source File
# Begin Source File

SOURCE=tableeditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__TABLE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing tableeditorimpl.h...
InputPath=tableeditorimpl.h

"tmp\moc\release-shared-mt\moc_tableeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc tableeditorimpl.h -o tmp\moc\release-shared-mt\moc_tableeditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__TABLE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing tableeditorimpl.h...
InputPath=tableeditorimpl.h

"tmp\moc\release-shared-mt\moc_tableeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc tableeditorimpl.h -o tmp\moc\release-shared-mt\moc_tableeditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=timestamp.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__TIMES=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing timestamp.h...
InputPath=timestamp.h

"tmp\moc\release-shared-mt\moc_timestamp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc timestamp.h -o tmp\moc\release-shared-mt\moc_timestamp.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__TIMES=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing timestamp.h...
InputPath=timestamp.h

"tmp\moc\release-shared-mt\moc_timestamp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc timestamp.h -o tmp\moc\release-shared-mt\moc_timestamp.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variabledialogimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__VARIA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing variabledialogimpl.h...
InputPath=variabledialogimpl.h

"tmp\moc\release-shared-mt\moc_variabledialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variabledialogimpl.h -o tmp\moc\release-shared-mt\moc_variabledialogimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__VARIA=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing variabledialogimpl.h...
InputPath=variabledialogimpl.h

"tmp\moc\release-shared-mt\moc_variabledialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variabledialogimpl.h -o tmp\moc\release-shared-mt\moc_variabledialogimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=widgetaction.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__WIDGE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing widgetaction.h...
InputPath=widgetaction.h

"tmp\moc\release-shared-mt\moc_widgetaction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc widgetaction.h -o tmp\moc\release-shared-mt\moc_widgetaction.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__WIDGE=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing widgetaction.h...
InputPath=widgetaction.h

"tmp\moc\release-shared-mt\moc_widgetaction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc widgetaction.h -o tmp\moc\release-shared-mt\moc_widgetaction.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\widgetdatabase.h
# End Source File
# Begin Source File

SOURCE=widgetfactory.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__WIDGET=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing widgetfactory.h...
InputPath=widgetfactory.h

"tmp\moc\release-shared-mt\moc_widgetfactory.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc widgetfactory.h -o tmp\moc\release-shared-mt\moc_widgetfactory.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__WIDGET=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing widgetfactory.h...
InputPath=widgetfactory.h

"tmp\moc\release-shared-mt\moc_widgetfactory.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc widgetfactory.h -o tmp\moc\release-shared-mt\moc_widgetfactory.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\interfaces\widgetinterface.h
# End Source File
# Begin Source File

SOURCE=wizardeditorimpl.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__WIZAR=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing wizardeditorimpl.h...
InputPath=wizardeditorimpl.h

"tmp\moc\release-shared-mt\moc_wizardeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc wizardeditorimpl.h -o tmp\moc\release-shared-mt\moc_wizardeditorimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__WIZAR=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing wizardeditorimpl.h...
InputPath=wizardeditorimpl.h

"tmp\moc\release-shared-mt\moc_wizardeditorimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc wizardeditorimpl.h -o tmp\moc\release-shared-mt\moc_wizardeditorimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=workspace.h

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__WORKS=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing workspace.h...
InputPath=workspace.h

"tmp\moc\release-shared-mt\moc_workspace.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc workspace.h -o tmp\moc\release-shared-mt\moc_workspace.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__WORKS=""$(QTDIR)\bin\moc.exe""	
# Begin Custom Build - Moc'ing workspace.h...
InputPath=workspace.h

"tmp\moc\release-shared-mt\moc_workspace.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc workspace.h -o tmp\moc\release-shared-mt\moc_workspace.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=about.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ABOUT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing about.ui...
InputPath=about.ui

BuildCmds= \
	$(QTDIR)\bin\uic about.ui -o about.h \
	$(QTDIR)\bin\uic about.ui -i about.h -o about.cpp \
	$(QTDIR)\bin\moc  about.h -o tmp\moc\release-shared-mt\moc_about.cpp \
	

"about.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"about.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_about.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ABOUT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing about.ui...
InputPath=about.ui

BuildCmds= \
	$(QTDIR)\bin\uic about.ui -o about.h \
	$(QTDIR)\bin\uic about.ui -i about.h -o about.cpp \
	$(QTDIR)\bin\moc  about.h -o tmp\moc\release-shared-mt\moc_about.cpp \
	

"about.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"about.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_about.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=actioneditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ACTIONE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing actioneditor.ui...
InputPath=actioneditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic actioneditor.ui -o actioneditor.h \
	$(QTDIR)\bin\uic actioneditor.ui -i actioneditor.h -o actioneditor.cpp \
	$(QTDIR)\bin\moc  actioneditor.h -o tmp\moc\release-shared-mt\moc_actioneditor.cpp \
	

"actioneditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"actioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_actioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ACTIONE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing actioneditor.ui...
InputPath=actioneditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic actioneditor.ui -o actioneditor.h \
	$(QTDIR)\bin\uic actioneditor.ui -i actioneditor.h -o actioneditor.cpp \
	$(QTDIR)\bin\moc  actioneditor.h -o tmp\moc\release-shared-mt\moc_actioneditor.cpp \
	

"actioneditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"actioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_actioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=configtoolboxdialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CONFI="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing configtoolboxdialog.ui...
InputPath=configtoolboxdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic configtoolboxdialog.ui -o configtoolboxdialog.h \
	$(QTDIR)\bin\uic configtoolboxdialog.ui -i configtoolboxdialog.h -o configtoolboxdialog.cpp \
	$(QTDIR)\bin\moc  configtoolboxdialog.h -o tmp\moc\release-shared-mt\moc_configtoolboxdialog.cpp \
	

"configtoolboxdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"configtoolboxdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_configtoolboxdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CONFI="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing configtoolboxdialog.ui...
InputPath=configtoolboxdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic configtoolboxdialog.ui -o configtoolboxdialog.h \
	$(QTDIR)\bin\uic configtoolboxdialog.ui -i configtoolboxdialog.h -o configtoolboxdialog.cpp \
	$(QTDIR)\bin\moc  configtoolboxdialog.h -o tmp\moc\release-shared-mt\moc_configtoolboxdialog.cpp \
	

"configtoolboxdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"configtoolboxdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_configtoolboxdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=connectiondialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CONNECT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing connectiondialog.ui...
InputPath=connectiondialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic connectiondialog.ui -o connectiondialog.h \
	$(QTDIR)\bin\uic connectiondialog.ui -i connectiondialog.h -o connectiondialog.cpp \
	$(QTDIR)\bin\moc  connectiondialog.h -o tmp\moc\release-shared-mt\moc_connectiondialog.cpp \
	

"connectiondialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"connectiondialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_connectiondialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CONNECT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing connectiondialog.ui...
InputPath=connectiondialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic connectiondialog.ui -o connectiondialog.h \
	$(QTDIR)\bin\uic connectiondialog.ui -i connectiondialog.h -o connectiondialog.cpp \
	$(QTDIR)\bin\moc  connectiondialog.h -o tmp\moc\release-shared-mt\moc_connectiondialog.cpp \
	

"connectiondialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"connectiondialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_connectiondialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=createtemplate.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CREAT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing createtemplate.ui...
InputPath=createtemplate.ui

BuildCmds= \
	$(QTDIR)\bin\uic createtemplate.ui -o createtemplate.h \
	$(QTDIR)\bin\uic createtemplate.ui -i createtemplate.h -o createtemplate.cpp \
	$(QTDIR)\bin\moc  createtemplate.h -o tmp\moc\release-shared-mt\moc_createtemplate.cpp \
	

"createtemplate.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"createtemplate.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_createtemplate.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CREAT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing createtemplate.ui...
InputPath=createtemplate.ui

BuildCmds= \
	$(QTDIR)\bin\uic createtemplate.ui -o createtemplate.h \
	$(QTDIR)\bin\uic createtemplate.ui -i createtemplate.h -o createtemplate.cpp \
	$(QTDIR)\bin\moc  createtemplate.h -o tmp\moc\release-shared-mt\moc_createtemplate.cpp \
	

"createtemplate.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"createtemplate.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_createtemplate.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=customwidgeteditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__CUSTOM="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing customwidgeteditor.ui...
InputPath=customwidgeteditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic customwidgeteditor.ui -o customwidgeteditor.h \
	$(QTDIR)\bin\uic customwidgeteditor.ui -i customwidgeteditor.h -o customwidgeteditor.cpp \
	$(QTDIR)\bin\moc  customwidgeteditor.h -o tmp\moc\release-shared-mt\moc_customwidgeteditor.cpp \
	

"customwidgeteditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"customwidgeteditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_customwidgeteditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__CUSTOM="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing customwidgeteditor.ui...
InputPath=customwidgeteditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic customwidgeteditor.ui -o customwidgeteditor.h \
	$(QTDIR)\bin\uic customwidgeteditor.ui -i customwidgeteditor.h -o customwidgeteditor.cpp \
	$(QTDIR)\bin\moc  customwidgeteditor.h -o tmp\moc\release-shared-mt\moc_customwidgeteditor.cpp \
	

"customwidgeteditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"customwidgeteditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_customwidgeteditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=dbconnection.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DBCONNE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing dbconnection.ui...
InputPath=dbconnection.ui

BuildCmds= \
	$(QTDIR)\bin\uic dbconnection.ui -o dbconnection.h \
	$(QTDIR)\bin\uic dbconnection.ui -i dbconnection.h -o dbconnection.cpp \
	$(QTDIR)\bin\moc  dbconnection.h -o tmp\moc\release-shared-mt\moc_dbconnection.cpp \
	

"dbconnection.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"dbconnection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_dbconnection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DBCONNE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing dbconnection.ui...
InputPath=dbconnection.ui

BuildCmds= \
	$(QTDIR)\bin\uic dbconnection.ui -o dbconnection.h \
	$(QTDIR)\bin\uic dbconnection.ui -i dbconnection.h -o dbconnection.cpp \
	$(QTDIR)\bin\moc  dbconnection.h -o tmp\moc\release-shared-mt\moc_dbconnection.cpp \
	

"dbconnection.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"dbconnection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_dbconnection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=dbconnectioneditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DBCONNEC="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing dbconnectioneditor.ui...
InputPath=dbconnectioneditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic dbconnectioneditor.ui -o dbconnectioneditor.h \
	$(QTDIR)\bin\uic dbconnectioneditor.ui -i dbconnectioneditor.h -o dbconnectioneditor.cpp \
	$(QTDIR)\bin\moc  dbconnectioneditor.h -o tmp\moc\release-shared-mt\moc_dbconnectioneditor.cpp \
	

"dbconnectioneditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"dbconnectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_dbconnectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DBCONNEC="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing dbconnectioneditor.ui...
InputPath=dbconnectioneditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic dbconnectioneditor.ui -o dbconnectioneditor.h \
	$(QTDIR)\bin\uic dbconnectioneditor.ui -i dbconnectioneditor.h -o dbconnectioneditor.cpp \
	$(QTDIR)\bin\moc  dbconnectioneditor.h -o tmp\moc\release-shared-mt\moc_dbconnectioneditor.cpp \
	

"dbconnectioneditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"dbconnectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_dbconnectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=dbconnections.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DBCONNECT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing dbconnections.ui...
InputPath=dbconnections.ui

BuildCmds= \
	$(QTDIR)\bin\uic dbconnections.ui -o dbconnections.h \
	$(QTDIR)\bin\uic dbconnections.ui -i dbconnections.h -o dbconnections.cpp \
	$(QTDIR)\bin\moc  dbconnections.h -o tmp\moc\release-shared-mt\moc_dbconnections.cpp \
	

"dbconnections.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"dbconnections.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_dbconnections.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DBCONNECT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing dbconnections.ui...
InputPath=dbconnections.ui

BuildCmds= \
	$(QTDIR)\bin\uic dbconnections.ui -o dbconnections.h \
	$(QTDIR)\bin\uic dbconnections.ui -i dbconnections.h -o dbconnections.cpp \
	$(QTDIR)\bin\moc  dbconnections.h -o tmp\moc\release-shared-mt\moc_dbconnections.cpp \
	

"dbconnections.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"dbconnections.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_dbconnections.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=editfunctions.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__EDITFU="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing editfunctions.ui...
InputPath=editfunctions.ui

BuildCmds= \
	$(QTDIR)\bin\uic editfunctions.ui -o editfunctions.h \
	$(QTDIR)\bin\uic editfunctions.ui -i editfunctions.h -o editfunctions.cpp \
	$(QTDIR)\bin\moc  editfunctions.h -o tmp\moc\release-shared-mt\moc_editfunctions.cpp \
	

"editfunctions.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"editfunctions.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_editfunctions.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__EDITFU="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing editfunctions.ui...
InputPath=editfunctions.ui

BuildCmds= \
	$(QTDIR)\bin\uic editfunctions.ui -o editfunctions.h \
	$(QTDIR)\bin\uic editfunctions.ui -i editfunctions.h -o editfunctions.cpp \
	$(QTDIR)\bin\moc  editfunctions.h -o tmp\moc\release-shared-mt\moc_editfunctions.cpp \
	

"editfunctions.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"editfunctions.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_editfunctions.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=finddialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__FINDD="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing finddialog.ui...
InputPath=finddialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic finddialog.ui -o finddialog.h \
	$(QTDIR)\bin\uic finddialog.ui -i finddialog.h -o finddialog.cpp \
	$(QTDIR)\bin\moc  finddialog.h -o tmp\moc\release-shared-mt\moc_finddialog.cpp \
	

"finddialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__FINDD="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing finddialog.ui...
InputPath=finddialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic finddialog.ui -o finddialog.h \
	$(QTDIR)\bin\uic finddialog.ui -i finddialog.h -o finddialog.cpp \
	$(QTDIR)\bin\moc  finddialog.h -o tmp\moc\release-shared-mt\moc_finddialog.cpp \
	

"finddialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=formsettings.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__FORMSE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing formsettings.ui...
InputPath=formsettings.ui

BuildCmds= \
	$(QTDIR)\bin\uic formsettings.ui -o formsettings.h \
	$(QTDIR)\bin\uic formsettings.ui -i formsettings.h -o formsettings.cpp \
	$(QTDIR)\bin\moc  formsettings.h -o tmp\moc\release-shared-mt\moc_formsettings.cpp \
	

"formsettings.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"formsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_formsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__FORMSE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing formsettings.ui...
InputPath=formsettings.ui

BuildCmds= \
	$(QTDIR)\bin\uic formsettings.ui -o formsettings.h \
	$(QTDIR)\bin\uic formsettings.ui -i formsettings.h -o formsettings.cpp \
	$(QTDIR)\bin\moc  formsettings.h -o tmp\moc\release-shared-mt\moc_formsettings.cpp \
	

"formsettings.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"formsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_formsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=gotolinedialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__GOTOL="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing gotolinedialog.ui...
InputPath=gotolinedialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic gotolinedialog.ui -o gotolinedialog.h \
	$(QTDIR)\bin\uic gotolinedialog.ui -i gotolinedialog.h -o gotolinedialog.cpp \
	$(QTDIR)\bin\moc  gotolinedialog.h -o tmp\moc\release-shared-mt\moc_gotolinedialog.cpp \
	

"gotolinedialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"gotolinedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_gotolinedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__GOTOL="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing gotolinedialog.ui...
InputPath=gotolinedialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic gotolinedialog.ui -o gotolinedialog.h \
	$(QTDIR)\bin\uic gotolinedialog.ui -i gotolinedialog.h -o gotolinedialog.cpp \
	$(QTDIR)\bin\moc  gotolinedialog.h -o tmp\moc\release-shared-mt\moc_gotolinedialog.cpp \
	

"gotolinedialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"gotolinedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_gotolinedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=iconvieweditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__ICONVI="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing iconvieweditor.ui...
InputPath=iconvieweditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic iconvieweditor.ui -o iconvieweditor.h \
	$(QTDIR)\bin\uic iconvieweditor.ui -i iconvieweditor.h -o iconvieweditor.cpp \
	$(QTDIR)\bin\moc  iconvieweditor.h -o tmp\moc\release-shared-mt\moc_iconvieweditor.cpp \
	

"iconvieweditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"iconvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_iconvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__ICONVI="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing iconvieweditor.ui...
InputPath=iconvieweditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic iconvieweditor.ui -o iconvieweditor.h \
	$(QTDIR)\bin\uic iconvieweditor.ui -i iconvieweditor.h -o iconvieweditor.cpp \
	$(QTDIR)\bin\moc  iconvieweditor.h -o tmp\moc\release-shared-mt\moc_iconvieweditor.cpp \
	

"iconvieweditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"iconvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_iconvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listboxeditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTBOXE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing listboxeditor.ui...
InputPath=listboxeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic listboxeditor.ui -o listboxeditor.h \
	$(QTDIR)\bin\uic listboxeditor.ui -i listboxeditor.h -o listboxeditor.cpp \
	$(QTDIR)\bin\moc  listboxeditor.h -o tmp\moc\release-shared-mt\moc_listboxeditor.cpp \
	

"listboxeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"listboxeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_listboxeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTBOXE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing listboxeditor.ui...
InputPath=listboxeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic listboxeditor.ui -o listboxeditor.h \
	$(QTDIR)\bin\uic listboxeditor.ui -i listboxeditor.h -o listboxeditor.cpp \
	$(QTDIR)\bin\moc  listboxeditor.h -o tmp\moc\release-shared-mt\moc_listboxeditor.cpp \
	

"listboxeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"listboxeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_listboxeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listeditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing listeditor.ui...
InputPath=listeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic listeditor.ui -o listeditor.h \
	$(QTDIR)\bin\uic listeditor.ui -i listeditor.h -o listeditor.cpp \
	$(QTDIR)\bin\moc  listeditor.h -o tmp\moc\release-shared-mt\moc_listeditor.cpp \
	

"listeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"listeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_listeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing listeditor.ui...
InputPath=listeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic listeditor.ui -o listeditor.h \
	$(QTDIR)\bin\uic listeditor.ui -i listeditor.h -o listeditor.cpp \
	$(QTDIR)\bin\moc  listeditor.h -o tmp\moc\release-shared-mt\moc_listeditor.cpp \
	

"listeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"listeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_listeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=listvieweditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__LISTVIE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing listvieweditor.ui...
InputPath=listvieweditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic listvieweditor.ui -o listvieweditor.h \
	$(QTDIR)\bin\uic listvieweditor.ui -i listvieweditor.h -o listvieweditor.cpp \
	$(QTDIR)\bin\moc  listvieweditor.h -o tmp\moc\release-shared-mt\moc_listvieweditor.cpp \
	

"listvieweditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"listvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_listvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__LISTVIE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing listvieweditor.ui...
InputPath=listvieweditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic listvieweditor.ui -o listvieweditor.h \
	$(QTDIR)\bin\uic listvieweditor.ui -i listvieweditor.h -o listvieweditor.cpp \
	$(QTDIR)\bin\moc  listvieweditor.h -o tmp\moc\release-shared-mt\moc_listvieweditor.cpp \
	

"listvieweditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"listvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_listvieweditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=multilineeditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__MULTIL="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing multilineeditor.ui...
InputPath=multilineeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic multilineeditor.ui -o multilineeditor.h \
	$(QTDIR)\bin\uic multilineeditor.ui -i multilineeditor.h -o multilineeditor.cpp \
	$(QTDIR)\bin\moc  multilineeditor.h -o tmp\moc\release-shared-mt\moc_multilineeditor.cpp \
	

"multilineeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"multilineeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_multilineeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__MULTIL="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing multilineeditor.ui...
InputPath=multilineeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic multilineeditor.ui -o multilineeditor.h \
	$(QTDIR)\bin\uic multilineeditor.ui -i multilineeditor.h -o multilineeditor.cpp \
	$(QTDIR)\bin\moc  multilineeditor.h -o tmp\moc\release-shared-mt\moc_multilineeditor.cpp \
	

"multilineeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"multilineeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_multilineeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=newform.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__NEWFOR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing newform.ui...
InputPath=newform.ui

BuildCmds= \
	$(QTDIR)\bin\uic newform.ui -o newform.h \
	$(QTDIR)\bin\uic newform.ui -i newform.h -o newform.cpp \
	$(QTDIR)\bin\moc  newform.h -o tmp\moc\release-shared-mt\moc_newform.cpp \
	

"newform.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"newform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_newform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__NEWFOR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing newform.ui...
InputPath=newform.ui

BuildCmds= \
	$(QTDIR)\bin\uic newform.ui -o newform.h \
	$(QTDIR)\bin\uic newform.ui -i newform.h -o newform.cpp \
	$(QTDIR)\bin\moc  newform.h -o tmp\moc\release-shared-mt\moc_newform.cpp \
	

"newform.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"newform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_newform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=paletteeditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PALETTE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing paletteeditor.ui...
InputPath=paletteeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic paletteeditor.ui -o paletteeditor.h \
	$(QTDIR)\bin\uic paletteeditor.ui -i paletteeditor.h -o paletteeditor.cpp \
	$(QTDIR)\bin\moc  paletteeditor.h -o tmp\moc\release-shared-mt\moc_paletteeditor.cpp \
	

"paletteeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"paletteeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_paletteeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PALETTE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing paletteeditor.ui...
InputPath=paletteeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic paletteeditor.ui -o paletteeditor.h \
	$(QTDIR)\bin\uic paletteeditor.ui -i paletteeditor.h -o paletteeditor.cpp \
	$(QTDIR)\bin\moc  paletteeditor.h -o tmp\moc\release-shared-mt\moc_paletteeditor.cpp \
	

"paletteeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"paletteeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_paletteeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=paletteeditoradvanced.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PALETTEE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing paletteeditoradvanced.ui...
InputPath=paletteeditoradvanced.ui

BuildCmds= \
	$(QTDIR)\bin\uic paletteeditoradvanced.ui -o paletteeditoradvanced.h \
	$(QTDIR)\bin\uic paletteeditoradvanced.ui -i paletteeditoradvanced.h -o paletteeditoradvanced.cpp \
	$(QTDIR)\bin\moc  paletteeditoradvanced.h -o tmp\moc\release-shared-mt\moc_paletteeditoradvanced.cpp \
	

"paletteeditoradvanced.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"paletteeditoradvanced.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_paletteeditoradvanced.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PALETTEE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing paletteeditoradvanced.ui...
InputPath=paletteeditoradvanced.ui

BuildCmds= \
	$(QTDIR)\bin\uic paletteeditoradvanced.ui -o paletteeditoradvanced.h \
	$(QTDIR)\bin\uic paletteeditoradvanced.ui -i paletteeditoradvanced.h -o paletteeditoradvanced.cpp \
	$(QTDIR)\bin\moc  paletteeditoradvanced.h -o tmp\moc\release-shared-mt\moc_paletteeditoradvanced.cpp \
	

"paletteeditoradvanced.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"paletteeditoradvanced.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_paletteeditoradvanced.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=pixmapcollectioneditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PIXMAP="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing pixmapcollectioneditor.ui...
InputPath=pixmapcollectioneditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic pixmapcollectioneditor.ui -o pixmapcollectioneditor.h \
	$(QTDIR)\bin\uic pixmapcollectioneditor.ui -i pixmapcollectioneditor.h -o pixmapcollectioneditor.cpp \
	$(QTDIR)\bin\moc  pixmapcollectioneditor.h -o tmp\moc\release-shared-mt\moc_pixmapcollectioneditor.cpp \
	

"pixmapcollectioneditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pixmapcollectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_pixmapcollectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PIXMAP="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing pixmapcollectioneditor.ui...
InputPath=pixmapcollectioneditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic pixmapcollectioneditor.ui -o pixmapcollectioneditor.h \
	$(QTDIR)\bin\uic pixmapcollectioneditor.ui -i pixmapcollectioneditor.h -o pixmapcollectioneditor.cpp \
	$(QTDIR)\bin\moc  pixmapcollectioneditor.h -o tmp\moc\release-shared-mt\moc_pixmapcollectioneditor.cpp \
	

"pixmapcollectioneditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pixmapcollectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_pixmapcollectioneditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=pixmapfunction.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PIXMAPF="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing pixmapfunction.ui...
InputPath=pixmapfunction.ui

BuildCmds= \
	$(QTDIR)\bin\uic pixmapfunction.ui -o pixmapfunction.h \
	$(QTDIR)\bin\uic pixmapfunction.ui -i pixmapfunction.h -o pixmapfunction.cpp \
	$(QTDIR)\bin\moc  pixmapfunction.h -o tmp\moc\release-shared-mt\moc_pixmapfunction.cpp \
	

"pixmapfunction.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pixmapfunction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_pixmapfunction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PIXMAPF="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing pixmapfunction.ui...
InputPath=pixmapfunction.ui

BuildCmds= \
	$(QTDIR)\bin\uic pixmapfunction.ui -o pixmapfunction.h \
	$(QTDIR)\bin\uic pixmapfunction.ui -i pixmapfunction.h -o pixmapfunction.cpp \
	$(QTDIR)\bin\moc  pixmapfunction.h -o tmp\moc\release-shared-mt\moc_pixmapfunction.cpp \
	

"pixmapfunction.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pixmapfunction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_pixmapfunction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=preferences.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PREFE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing preferences.ui...
InputPath=preferences.ui

BuildCmds= \
	$(QTDIR)\bin\uic preferences.ui -o preferences.h \
	$(QTDIR)\bin\uic preferences.ui -i preferences.h -o preferences.cpp \
	$(QTDIR)\bin\moc  preferences.h -o tmp\moc\release-shared-mt\moc_preferences.cpp \
	

"preferences.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"preferences.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_preferences.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PREFE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing preferences.ui...
InputPath=preferences.ui

BuildCmds= \
	$(QTDIR)\bin\uic preferences.ui -o preferences.h \
	$(QTDIR)\bin\uic preferences.ui -i preferences.h -o preferences.cpp \
	$(QTDIR)\bin\moc  preferences.h -o tmp\moc\release-shared-mt\moc_preferences.cpp \
	

"preferences.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"preferences.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_preferences.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=previewwidget.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PREVIEW="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing previewwidget.ui...
InputPath=previewwidget.ui

BuildCmds= \
	$(QTDIR)\bin\uic previewwidget.ui -o previewwidget.h \
	$(QTDIR)\bin\uic previewwidget.ui -i previewwidget.h -o previewwidget.cpp \
	$(QTDIR)\bin\moc  previewwidget.h -o tmp\moc\release-shared-mt\moc_previewwidget.cpp \
	

"previewwidget.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"previewwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_previewwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PREVIEW="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing previewwidget.ui...
InputPath=previewwidget.ui

BuildCmds= \
	$(QTDIR)\bin\uic previewwidget.ui -o previewwidget.h \
	$(QTDIR)\bin\uic previewwidget.ui -i previewwidget.h -o previewwidget.cpp \
	$(QTDIR)\bin\moc  previewwidget.h -o tmp\moc\release-shared-mt\moc_previewwidget.cpp \
	

"previewwidget.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"previewwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_previewwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=projectsettings.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__PROJECT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing projectsettings.ui...
InputPath=projectsettings.ui

BuildCmds= \
	$(QTDIR)\bin\uic projectsettings.ui -o projectsettings.h \
	$(QTDIR)\bin\uic projectsettings.ui -i projectsettings.h -o projectsettings.cpp \
	$(QTDIR)\bin\moc  projectsettings.h -o tmp\moc\release-shared-mt\moc_projectsettings.cpp \
	

"projectsettings.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"projectsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_projectsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__PROJECT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing projectsettings.ui...
InputPath=projectsettings.ui

BuildCmds= \
	$(QTDIR)\bin\uic projectsettings.ui -o projectsettings.h \
	$(QTDIR)\bin\uic projectsettings.ui -i projectsettings.h -o projectsettings.cpp \
	$(QTDIR)\bin\moc  projectsettings.h -o tmp\moc\release-shared-mt\moc_projectsettings.cpp \
	

"projectsettings.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"projectsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_projectsettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=replacedialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__REPLA="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing replacedialog.ui...
InputPath=replacedialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic replacedialog.ui -o replacedialog.h \
	$(QTDIR)\bin\uic replacedialog.ui -i replacedialog.h -o replacedialog.cpp \
	$(QTDIR)\bin\moc  replacedialog.h -o tmp\moc\release-shared-mt\moc_replacedialog.cpp \
	

"replacedialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"replacedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_replacedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__REPLA="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing replacedialog.ui...
InputPath=replacedialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic replacedialog.ui -o replacedialog.h \
	$(QTDIR)\bin\uic replacedialog.ui -i replacedialog.h -o replacedialog.cpp \
	$(QTDIR)\bin\moc  replacedialog.h -o tmp\moc\release-shared-mt\moc_replacedialog.cpp \
	

"replacedialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"replacedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_replacedialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=richtextfontdialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__RICHT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing richtextfontdialog.ui...
InputPath=richtextfontdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic richtextfontdialog.ui -o richtextfontdialog.h \
	$(QTDIR)\bin\uic richtextfontdialog.ui -i richtextfontdialog.h -o richtextfontdialog.cpp \
	$(QTDIR)\bin\moc  richtextfontdialog.h -o tmp\moc\release-shared-mt\moc_richtextfontdialog.cpp \
	

"richtextfontdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"richtextfontdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_richtextfontdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__RICHT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing richtextfontdialog.ui...
InputPath=richtextfontdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic richtextfontdialog.ui -o richtextfontdialog.h \
	$(QTDIR)\bin\uic richtextfontdialog.ui -i richtextfontdialog.h -o richtextfontdialog.cpp \
	$(QTDIR)\bin\moc  richtextfontdialog.h -o tmp\moc\release-shared-mt\moc_richtextfontdialog.cpp \
	

"richtextfontdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"richtextfontdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_richtextfontdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=startdialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__STARTD="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing startdialog.ui...
InputPath=startdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic startdialog.ui -o startdialog.h \
	$(QTDIR)\bin\uic startdialog.ui -i startdialog.h -o startdialog.cpp \
	$(QTDIR)\bin\moc  startdialog.h -o tmp\moc\release-shared-mt\moc_startdialog.cpp \
	

"startdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"startdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_startdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__STARTD="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing startdialog.ui...
InputPath=startdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic startdialog.ui -o startdialog.h \
	$(QTDIR)\bin\uic startdialog.ui -i startdialog.h -o startdialog.cpp \
	$(QTDIR)\bin\moc  startdialog.h -o tmp\moc\release-shared-mt\moc_startdialog.cpp \
	

"startdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"startdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_startdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=tableeditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__TABLEE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing tableeditor.ui...
InputPath=tableeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic tableeditor.ui -o tableeditor.h \
	$(QTDIR)\bin\uic tableeditor.ui -i tableeditor.h -o tableeditor.cpp \
	$(QTDIR)\bin\moc  tableeditor.h -o tmp\moc\release-shared-mt\moc_tableeditor.cpp \
	

"tableeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tableeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_tableeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__TABLEE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing tableeditor.ui...
InputPath=tableeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic tableeditor.ui -o tableeditor.h \
	$(QTDIR)\bin\uic tableeditor.ui -i tableeditor.h -o tableeditor.cpp \
	$(QTDIR)\bin\moc  tableeditor.h -o tmp\moc\release-shared-mt\moc_tableeditor.cpp \
	

"tableeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tableeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_tableeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variabledialog.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__VARIAB="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variabledialog.ui...
InputPath=variabledialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic variabledialog.ui -o variabledialog.h \
	$(QTDIR)\bin\uic variabledialog.ui -i variabledialog.h -o variabledialog.cpp \
	$(QTDIR)\bin\moc  variabledialog.h -o tmp\moc\release-shared-mt\moc_variabledialog.cpp \
	

"variabledialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variabledialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_variabledialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__VARIAB="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variabledialog.ui...
InputPath=variabledialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic variabledialog.ui -o variabledialog.h \
	$(QTDIR)\bin\uic variabledialog.ui -i variabledialog.h -o variabledialog.cpp \
	$(QTDIR)\bin\moc  variabledialog.h -o tmp\moc\release-shared-mt\moc_variabledialog.cpp \
	

"variabledialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variabledialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_variabledialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=wizardeditor.ui

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__WIZARD="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing wizardeditor.ui...
InputPath=wizardeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic wizardeditor.ui -o wizardeditor.h \
	$(QTDIR)\bin\uic wizardeditor.ui -i wizardeditor.h -o wizardeditor.cpp \
	$(QTDIR)\bin\moc  wizardeditor.h -o tmp\moc\release-shared-mt\moc_wizardeditor.cpp \
	

"wizardeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"wizardeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_wizardeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__WIZARD="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing wizardeditor.ui...
InputPath=wizardeditor.ui

BuildCmds= \
	$(QTDIR)\bin\uic wizardeditor.ui -o wizardeditor.h \
	$(QTDIR)\bin\uic wizardeditor.ui -i wizardeditor.h -o wizardeditor.cpp \
	$(QTDIR)\bin\moc  wizardeditor.h -o tmp\moc\release-shared-mt\moc_wizardeditor.cpp \
	

"wizardeditor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"wizardeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tmp\moc\release-shared-mt\moc_wizardeditor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Translations"

# PROP Default_Filter "ts"
# Begin Source File

SOURCE=designer_de.ts
# End Source File
# Begin Source File

SOURCE=designer_fr.ts
# End Source File
# End Group
# Begin Group "Images"

# PROP Default_Filter "png jpeg bmp xpm"
# Begin Source File

SOURCE=images/designer_adjustsize.png

!IF  "$(CFG)" == "designerlib - Win32 Release"

USERDEP__DESIGN="images/designer_adjustsize.png"	"images/designer_edithlayoutsplit.png"	"images/designer_left.png"	"images/designer_sizeall.png"	"images/designer_arrow.png"	"images/designer_editlower.png"	"images/designer_line.png"	"images/designer_sizeb.png"	"images/designer_background.png"	"images/designer_editpaste.png"	"images/designer_lineedit.png"	"images/designer_sizef.png"	"images/designer_book.png"	"images/designer_editraise.png"	"images/designer_listbox.png"	"images/designer_sizeh.png"	"images/designer_buttongroup.png"	"images/designer_editslots.png"	"images/designer_listview.png"	"images/designer_sizev.png"	"images/designer_checkbox.png"	"images/designer_editvlayout.png"	"images/designer_multilineedit.png"	"images/designer_slider.png"	"images/designer_combobox.png"	"images/designer_editvlayoutsplit.png"	"images/designer_newform.png"	"images/designer_spacer.png"	"images/designer_connecttool.png"	"images/designer_filenew.png"	"images/designer_no.png"	"images/designer_spinbox.png"	"images/designer_cross.png"	"images/designer_fileopen.png"	"images/designer_ordertool.png"	"images/designer_splash.png"	"images/designer_customwidget.png"	"images/designer_filesave.png"\
	"images/designer_pixlabel.png"	"images/designer_table.png"	"images/designer_databrowser.png"	"images/designer_form.png"	"images/designer_pointer.png"	"images/designer_tabwidget.png"	"images/designer_datatable.png"	"images/designer_frame.png"	"images/designer_print.png"	"images/designer_textbrowser.png"	"images/designer_dataview.png"	"images/designer_groupbox.png"	"images/designer_progress.png"	"images/designer_textedit.png"	"images/designer_dateedit.png"	"images/designer_hand.png"	"images/designer_project.png"	"images/designer_textview.png"	"images/designer_datetimeedit.png"	"images/designer_help.png"	"images/designer_pushbutton.png"	"images/designer_timeedit.png"	"images/designer_dial.png"	"images/designer_home.png"	"images/designer_qtlogo.png"	"images/designer_toolbutton.png"	"images/designer_toolbox.png"	"images/designer_down.png"	"images/designer_hsplit.png"	"images/designer_radiobutton.png"	"images/designer_undo.png"	"images/designer_editbreaklayout.png"	"images/designer_ibeam.png"	"images/designer_redo.png"	"images/designer_up.png"	"images/designer_resetproperty.png"	"images/designer_editcopy.png"	"images/designer_iconview.png"	"images/designer_resetproperty.png"\
	"images/designer_uparrow.png"	"images/designer_editcut.png"	"images/designer_image.png"	"images/designer_richtextedit.png"	"images/designer_vsplit.png"	"images/designer_editdelete.png"	"images/designer_label.png"	"images/designer_right.png"	"images/designer_wait.png"	"images/designer_editgrid.png"	"images/designer_layout.png"	"images/designer_scrollbar.png"	"images/designer_widgetstack.png"	"images/designer_edithlayout.png"	"images/designer_lcdnumber.png"	"images/designer_searchfind.png"	"images/designer_folder.png"	"images/designer_setbuddy.png"	"images/designer_textbold.png"	"images/designer_textcenter.png"	"images/designer_texth1.png"	"images/designer_texth2.png"	"images/designer_texth3.png"	"images/designer_textitalic.png"	"images/designer_textjustify.png"	"images/designer_textlarger.png"	"images/designer_textleft.png"	"images/designer_textlinebreak.png"	"images/designer_textparagraph.png"	"images/designer_textright.png"	"images/designer_textsmaller.png"	"images/designer_textteletext.png"	"images/designer_textunderline.png"	"images/designer_wizarddata.png"	"images/designer_wizarddialog.png"	"images/designer_d_adjustsize.png"	"images/designer_d_label.png"\
	"images/designer_d_book.png"	"images/designer_d_layout.png"	"images/designer_d_buttongroup.png"	"images/designer_d_lcdnumber.png"	"images/designer_d_checkbox.png"	"images/designer_d_left.png"	"images/designer_d_combobox.png"	"images/designer_d_line.png"	"images/designer_d_connecttool.png"	"images/designer_d_lineedit.png"	"images/designer_d_customwidget.png"	"images/designer_d_listbox.png"	"images/designer_d_databrowser.png"	"images/designer_d_listview.png"	"images/designer_d_datatable.png"	"images/designer_d_multilineedit.png"	"images/designer_d_dataview.png"	"images/designer_d_newform.png"	"images/designer_d_dateedit.png"	"images/designer_d_ordertool.png"	"images/designer_d_datetimeedit.png"	"images/designer_d_pixlabel.png"	"images/designer_d_dial.png"	"images/designer_d_pointer.png"	"images/designer_d_down.png"	"images/designer_d_print.png"	"images/designer_d_editbreaklayout.png"	"images/designer_d_progress.png"	"images/designer_d_editcopy.png"	"images/designer_d_project.png"	"images/designer_d_editcut.png"	"images/designer_d_pushbutton.png"	"images/designer_d_editdelete.png"	"images/designer_d_radiobutton.png"	"images/designer_d_editgrid.png"	"images/designer_d_redo.png"\
	"images/designer_d_edithlayout.png"	"images/designer_d_richtextedit.png"	"images/designer_d_edithlayoutsplit.png"	"images/designer_d_right.png"	"images/designer_d_editlower.png"	"images/designer_d_scrollbar.png"	"images/designer_d_editpaste.png"	"images/designer_d_searchfind.png"	"images/designer_d_editraise.png"	"images/designer_d_slider.png"	"images/designer_d_editslots.png"	"images/designer_d_spacer.png"	"images/designer_d_editvlayout.png"	"images/designer_d_spinbox.png"	"images/designer_d_editvlayoutsplit.png"	"images/designer_d_table.png"	"images/designer_d_filenew.png"	"images/designer_d_folder.png"	"images/designer_d_tabwidget.png"	"images/designer_d_fileopen.png"	"images/designer_d_textbrowser.png"	"images/designer_d_filesave.png"	"images/designer_d_textedit.png"	"images/designer_d_form.png"	"images/designer_d_textview.png"	"images/designer_d_frame.png"	"images/designer_d_timeedit.png"	"images/designer_d_groupbox.png"	"images/designer_d_toolbutton.png"	"images/designer_d_toolbox.png"	"images/designer_d_help.png"	"images/designer_d_undo.png"	"images/designer_d_home.png"	"images/designer_d_up.png"	"images/designer_d_iconview.png"	"images/designer_d_widgetstack.png"\
	"images/designer_d_setbuddy.png"	"images/designer_d_textbold.png"	"images/designer_d_texth1.png"	"images/designer_d_texth2.png"	"images/designer_d_texth3.png"	"images/designer_d_textitalic.png"	"images/designer_d_textjustify.png"	"images/designer_d_textlarger.png"	"images/designer_d_textleft.png"	"images/designer_d_textlinebreak.png"	"images/designer_d_textparagraph.png"	"images/designer_d_textright.png"	"images/designer_d_textsmaller.png"	"images/designer_d_textteletext.png"	"images/designer_d_textunderline.png"	"images/designer_d_textcenter.png"	"images/designer_d_wizarddata.png"	"images/designer_d_wizarddialog.png"	"images/designer_s_editcut.png"	"images/designer_s_up.png"	"images/designer_s_down.png"	"images/designer_s_left.png"	"images/designer_s_right.png"	"images/designer_d_image.png"	"images/designer_d_textfont.png"	"images/designer_textfont.png"	"images/designer_object.png"	"images/designer_appicon.png"	"images/designer_wordwrap.png"	"images/designer_d_wordwrap.png"	
# Begin Custom Build - Creating image collection...
InputPath=images/designer_adjustsize.png

"qmake_image_collection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic -embed designerlib -f images.tmp -o qmake_image_collection.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "designerlib - Win32 Debug"

USERDEP__DESIGN="images/designer_adjustsize.png"	"images/designer_edithlayoutsplit.png"	"images/designer_left.png"	"images/designer_sizeall.png"	"images/designer_arrow.png"	"images/designer_editlower.png"	"images/designer_line.png"	"images/designer_sizeb.png"	"images/designer_background.png"	"images/designer_editpaste.png"	"images/designer_lineedit.png"	"images/designer_sizef.png"	"images/designer_book.png"	"images/designer_editraise.png"	"images/designer_listbox.png"	"images/designer_sizeh.png"	"images/designer_buttongroup.png"	"images/designer_editslots.png"	"images/designer_listview.png"	"images/designer_sizev.png"	"images/designer_checkbox.png"	"images/designer_editvlayout.png"	"images/designer_multilineedit.png"	"images/designer_slider.png"	"images/designer_combobox.png"	"images/designer_editvlayoutsplit.png"	"images/designer_newform.png"	"images/designer_spacer.png"	"images/designer_connecttool.png"	"images/designer_filenew.png"	"images/designer_no.png"	"images/designer_spinbox.png"	"images/designer_cross.png"	"images/designer_fileopen.png"	"images/designer_ordertool.png"	"images/designer_splash.png"	"images/designer_customwidget.png"	"images/designer_filesave.png"\
	"images/designer_pixlabel.png"	"images/designer_table.png"	"images/designer_databrowser.png"	"images/designer_form.png"	"images/designer_pointer.png"	"images/designer_tabwidget.png"	"images/designer_datatable.png"	"images/designer_frame.png"	"images/designer_print.png"	"images/designer_textbrowser.png"	"images/designer_dataview.png"	"images/designer_groupbox.png"	"images/designer_progress.png"	"images/designer_textedit.png"	"images/designer_dateedit.png"	"images/designer_hand.png"	"images/designer_project.png"	"images/designer_textview.png"	"images/designer_datetimeedit.png"	"images/designer_help.png"	"images/designer_pushbutton.png"	"images/designer_timeedit.png"	"images/designer_dial.png"	"images/designer_home.png"	"images/designer_qtlogo.png"	"images/designer_toolbutton.png"	"images/designer_toolbox.png"	"images/designer_down.png"	"images/designer_hsplit.png"	"images/designer_radiobutton.png"	"images/designer_undo.png"	"images/designer_editbreaklayout.png"	"images/designer_ibeam.png"	"images/designer_redo.png"	"images/designer_up.png"	"images/designer_resetproperty.png"	"images/designer_editcopy.png"	"images/designer_iconview.png"	"images/designer_resetproperty.png"\
	"images/designer_uparrow.png"	"images/designer_editcut.png"	"images/designer_image.png"	"images/designer_richtextedit.png"	"images/designer_vsplit.png"	"images/designer_editdelete.png"	"images/designer_label.png"	"images/designer_right.png"	"images/designer_wait.png"	"images/designer_editgrid.png"	"images/designer_layout.png"	"images/designer_scrollbar.png"	"images/designer_widgetstack.png"	"images/designer_edithlayout.png"	"images/designer_lcdnumber.png"	"images/designer_searchfind.png"	"images/designer_folder.png"	"images/designer_setbuddy.png"	"images/designer_textbold.png"	"images/designer_textcenter.png"	"images/designer_texth1.png"	"images/designer_texth2.png"	"images/designer_texth3.png"	"images/designer_textitalic.png"	"images/designer_textjustify.png"	"images/designer_textlarger.png"	"images/designer_textleft.png"	"images/designer_textlinebreak.png"	"images/designer_textparagraph.png"	"images/designer_textright.png"	"images/designer_textsmaller.png"	"images/designer_textteletext.png"	"images/designer_textunderline.png"	"images/designer_wizarddata.png"	"images/designer_wizarddialog.png"	"images/designer_d_adjustsize.png"	"images/designer_d_label.png"\
	"images/designer_d_book.png"	"images/designer_d_layout.png"	"images/designer_d_buttongroup.png"	"images/designer_d_lcdnumber.png"	"images/designer_d_checkbox.png"	"images/designer_d_left.png"	"images/designer_d_combobox.png"	"images/designer_d_line.png"	"images/designer_d_connecttool.png"	"images/designer_d_lineedit.png"	"images/designer_d_customwidget.png"	"images/designer_d_listbox.png"	"images/designer_d_databrowser.png"	"images/designer_d_listview.png"	"images/designer_d_datatable.png"	"images/designer_d_multilineedit.png"	"images/designer_d_dataview.png"	"images/designer_d_newform.png"	"images/designer_d_dateedit.png"	"images/designer_d_ordertool.png"	"images/designer_d_datetimeedit.png"	"images/designer_d_pixlabel.png"	"images/designer_d_dial.png"	"images/designer_d_pointer.png"	"images/designer_d_down.png"	"images/designer_d_print.png"	"images/designer_d_editbreaklayout.png"	"images/designer_d_progress.png"	"images/designer_d_editcopy.png"	"images/designer_d_project.png"	"images/designer_d_editcut.png"	"images/designer_d_pushbutton.png"	"images/designer_d_editdelete.png"	"images/designer_d_radiobutton.png"	"images/designer_d_editgrid.png"	"images/designer_d_redo.png"\
	"images/designer_d_edithlayout.png"	"images/designer_d_richtextedit.png"	"images/designer_d_edithlayoutsplit.png"	"images/designer_d_right.png"	"images/designer_d_editlower.png"	"images/designer_d_scrollbar.png"	"images/designer_d_editpaste.png"	"images/designer_d_searchfind.png"	"images/designer_d_editraise.png"	"images/designer_d_slider.png"	"images/designer_d_editslots.png"	"images/designer_d_spacer.png"	"images/designer_d_editvlayout.png"	"images/designer_d_spinbox.png"	"images/designer_d_editvlayoutsplit.png"	"images/designer_d_table.png"	"images/designer_d_filenew.png"	"images/designer_d_folder.png"	"images/designer_d_tabwidget.png"	"images/designer_d_fileopen.png"	"images/designer_d_textbrowser.png"	"images/designer_d_filesave.png"	"images/designer_d_textedit.png"	"images/designer_d_form.png"	"images/designer_d_textview.png"	"images/designer_d_frame.png"	"images/designer_d_timeedit.png"	"images/designer_d_groupbox.png"	"images/designer_d_toolbutton.png"	"images/designer_d_toolbox.png"	"images/designer_d_help.png"	"images/designer_d_undo.png"	"images/designer_d_home.png"	"images/designer_d_up.png"	"images/designer_d_iconview.png"	"images/designer_d_widgetstack.png"\
	"images/designer_d_setbuddy.png"	"images/designer_d_textbold.png"	"images/designer_d_texth1.png"	"images/designer_d_texth2.png"	"images/designer_d_texth3.png"	"images/designer_d_textitalic.png"	"images/designer_d_textjustify.png"	"images/designer_d_textlarger.png"	"images/designer_d_textleft.png"	"images/designer_d_textlinebreak.png"	"images/designer_d_textparagraph.png"	"images/designer_d_textright.png"	"images/designer_d_textsmaller.png"	"images/designer_d_textteletext.png"	"images/designer_d_textunderline.png"	"images/designer_d_textcenter.png"	"images/designer_d_wizarddata.png"	"images/designer_d_wizarddialog.png"	"images/designer_s_editcut.png"	"images/designer_s_up.png"	"images/designer_s_down.png"	"images/designer_s_left.png"	"images/designer_s_right.png"	"images/designer_d_image.png"	"images/designer_d_textfont.png"	"images/designer_textfont.png"	"images/designer_object.png"	"images/designer_appicon.png"	"images/designer_wordwrap.png"	"images/designer_d_wordwrap.png"	
# Begin Custom Build - Creating image collection...
InputPath=images/designer_adjustsize.png

"qmake_image_collection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic -embed designerlib -f images.tmp -o qmake_image_collection.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=images/designer_appicon.png
# End Source File
# Begin Source File

SOURCE=images/designer_arrow.png
# End Source File
# Begin Source File

SOURCE=images/designer_background.png
# End Source File
# Begin Source File

SOURCE=images/designer_book.png
# End Source File
# Begin Source File

SOURCE=images/designer_buttongroup.png
# End Source File
# Begin Source File

SOURCE=images/designer_checkbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_combobox.png
# End Source File
# Begin Source File

SOURCE=images/designer_connecttool.png
# End Source File
# Begin Source File

SOURCE=images/designer_cross.png
# End Source File
# Begin Source File

SOURCE=images/designer_customwidget.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_adjustsize.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_book.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_buttongroup.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_checkbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_combobox.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_connecttool.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_customwidget.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_databrowser.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_datatable.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_dataview.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_dateedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_datetimeedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_dial.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_down.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editbreaklayout.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editcopy.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editcut.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editdelete.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editgrid.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_edithlayout.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_edithlayoutsplit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editlower.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editpaste.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editraise.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editslots.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editvlayout.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_editvlayoutsplit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_filenew.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_fileopen.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_filesave.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_folder.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_form.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_frame.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_groupbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_help.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_home.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_iconview.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_image.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_label.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_layout.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_lcdnumber.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_left.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_line.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_lineedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_listbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_listview.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_multilineedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_newform.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_ordertool.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_pixlabel.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_pointer.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_print.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_progress.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_project.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_pushbutton.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_radiobutton.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_redo.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_richtextedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_right.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_scrollbar.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_searchfind.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_setbuddy.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_slider.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_spacer.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_spinbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_table.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_tabwidget.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textbold.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textbrowser.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textcenter.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textfont.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_texth1.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_texth2.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_texth3.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textitalic.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textjustify.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textlarger.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textleft.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textlinebreak.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textparagraph.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textright.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textsmaller.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textteletext.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textunderline.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_textview.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_timeedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_toolbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_toolbutton.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_undo.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_up.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_widgetstack.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_wizarddata.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_wizarddialog.png
# End Source File
# Begin Source File

SOURCE=images/designer_d_wordwrap.png
# End Source File
# Begin Source File

SOURCE=images/designer_databrowser.png
# End Source File
# Begin Source File

SOURCE=images/designer_datatable.png
# End Source File
# Begin Source File

SOURCE=images/designer_dataview.png
# End Source File
# Begin Source File

SOURCE=images/designer_dateedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_datetimeedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_dial.png
# End Source File
# Begin Source File

SOURCE=images/designer_down.png
# End Source File
# Begin Source File

SOURCE=images/designer_editbreaklayout.png
# End Source File
# Begin Source File

SOURCE=images/designer_editcopy.png
# End Source File
# Begin Source File

SOURCE=images/designer_editcut.png
# End Source File
# Begin Source File

SOURCE=images/designer_editdelete.png
# End Source File
# Begin Source File

SOURCE=images/designer_editgrid.png
# End Source File
# Begin Source File

SOURCE=images/designer_edithlayout.png
# End Source File
# Begin Source File

SOURCE=images/designer_edithlayoutsplit.png
# End Source File
# Begin Source File

SOURCE=images/designer_editlower.png
# End Source File
# Begin Source File

SOURCE=images/designer_editpaste.png
# End Source File
# Begin Source File

SOURCE=images/designer_editraise.png
# End Source File
# Begin Source File

SOURCE=images/designer_editslots.png
# End Source File
# Begin Source File

SOURCE=images/designer_editvlayout.png
# End Source File
# Begin Source File

SOURCE=images/designer_editvlayoutsplit.png
# End Source File
# Begin Source File

SOURCE=images/designer_filenew.png
# End Source File
# Begin Source File

SOURCE=images/designer_fileopen.png
# End Source File
# Begin Source File

SOURCE=images/designer_filesave.png
# End Source File
# Begin Source File

SOURCE=images/designer_folder.png
# End Source File
# Begin Source File

SOURCE=images/designer_form.png
# End Source File
# Begin Source File

SOURCE=images/designer_frame.png
# End Source File
# Begin Source File

SOURCE=images/designer_groupbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_hand.png
# End Source File
# Begin Source File

SOURCE=images/designer_help.png
# End Source File
# Begin Source File

SOURCE=images/designer_home.png
# End Source File
# Begin Source File

SOURCE=images/designer_hsplit.png
# End Source File
# Begin Source File

SOURCE=images/designer_ibeam.png
# End Source File
# Begin Source File

SOURCE=images/designer_iconview.png
# End Source File
# Begin Source File

SOURCE=images/designer_image.png
# End Source File
# Begin Source File

SOURCE=images/designer_label.png
# End Source File
# Begin Source File

SOURCE=images/designer_layout.png
# End Source File
# Begin Source File

SOURCE=images/designer_lcdnumber.png
# End Source File
# Begin Source File

SOURCE=images/designer_left.png
# End Source File
# Begin Source File

SOURCE=images/designer_line.png
# End Source File
# Begin Source File

SOURCE=images/designer_lineedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_listbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_listview.png
# End Source File
# Begin Source File

SOURCE=images/designer_multilineedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_newform.png
# End Source File
# Begin Source File

SOURCE=images/designer_no.png
# End Source File
# Begin Source File

SOURCE=images/designer_object.png
# End Source File
# Begin Source File

SOURCE=images/designer_ordertool.png
# End Source File
# Begin Source File

SOURCE=images/designer_pixlabel.png
# End Source File
# Begin Source File

SOURCE=images/designer_pointer.png
# End Source File
# Begin Source File

SOURCE=images/designer_print.png
# End Source File
# Begin Source File

SOURCE=images/designer_progress.png
# End Source File
# Begin Source File

SOURCE=images/designer_project.png
# End Source File
# Begin Source File

SOURCE=images/designer_pushbutton.png
# End Source File
# Begin Source File

SOURCE=images/designer_qtlogo.png
# End Source File
# Begin Source File

SOURCE=images/designer_radiobutton.png
# End Source File
# Begin Source File

SOURCE=images/designer_redo.png
# End Source File
# Begin Source File

SOURCE=images/designer_resetproperty.png
# End Source File
# Begin Source File

SOURCE=images/designer_resetproperty.png
# End Source File
# Begin Source File

SOURCE=images/designer_richtextedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_right.png
# End Source File
# Begin Source File

SOURCE=images/designer_s_down.png
# End Source File
# Begin Source File

SOURCE=images/designer_s_editcut.png
# End Source File
# Begin Source File

SOURCE=images/designer_s_left.png
# End Source File
# Begin Source File

SOURCE=images/designer_s_right.png
# End Source File
# Begin Source File

SOURCE=images/designer_s_up.png
# End Source File
# Begin Source File

SOURCE=images/designer_scrollbar.png
# End Source File
# Begin Source File

SOURCE=images/designer_searchfind.png
# End Source File
# Begin Source File

SOURCE=images/designer_setbuddy.png
# End Source File
# Begin Source File

SOURCE=images/designer_sizeall.png
# End Source File
# Begin Source File

SOURCE=images/designer_sizeb.png
# End Source File
# Begin Source File

SOURCE=images/designer_sizef.png
# End Source File
# Begin Source File

SOURCE=images/designer_sizeh.png
# End Source File
# Begin Source File

SOURCE=images/designer_sizev.png
# End Source File
# Begin Source File

SOURCE=images/designer_slider.png
# End Source File
# Begin Source File

SOURCE=images/designer_spacer.png
# End Source File
# Begin Source File

SOURCE=images/designer_spinbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_splash.png
# End Source File
# Begin Source File

SOURCE=images/designer_table.png
# End Source File
# Begin Source File

SOURCE=images/designer_tabwidget.png
# End Source File
# Begin Source File

SOURCE=images/designer_textbold.png
# End Source File
# Begin Source File

SOURCE=images/designer_textbrowser.png
# End Source File
# Begin Source File

SOURCE=images/designer_textcenter.png
# End Source File
# Begin Source File

SOURCE=images/designer_textedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_textfont.png
# End Source File
# Begin Source File

SOURCE=images/designer_texth1.png
# End Source File
# Begin Source File

SOURCE=images/designer_texth2.png
# End Source File
# Begin Source File

SOURCE=images/designer_texth3.png
# End Source File
# Begin Source File

SOURCE=images/designer_textitalic.png
# End Source File
# Begin Source File

SOURCE=images/designer_textjustify.png
# End Source File
# Begin Source File

SOURCE=images/designer_textlarger.png
# End Source File
# Begin Source File

SOURCE=images/designer_textleft.png
# End Source File
# Begin Source File

SOURCE=images/designer_textlinebreak.png
# End Source File
# Begin Source File

SOURCE=images/designer_textparagraph.png
# End Source File
# Begin Source File

SOURCE=images/designer_textright.png
# End Source File
# Begin Source File

SOURCE=images/designer_textsmaller.png
# End Source File
# Begin Source File

SOURCE=images/designer_textteletext.png
# End Source File
# Begin Source File

SOURCE=images/designer_textunderline.png
# End Source File
# Begin Source File

SOURCE=images/designer_textview.png
# End Source File
# Begin Source File

SOURCE=images/designer_timeedit.png
# End Source File
# Begin Source File

SOURCE=images/designer_toolbox.png
# End Source File
# Begin Source File

SOURCE=images/designer_toolbutton.png
# End Source File
# Begin Source File

SOURCE=images/designer_undo.png
# End Source File
# Begin Source File

SOURCE=images/designer_up.png
# End Source File
# Begin Source File

SOURCE=images/designer_uparrow.png
# End Source File
# Begin Source File

SOURCE=images/designer_vsplit.png
# End Source File
# Begin Source File

SOURCE=images/designer_wait.png
# End Source File
# Begin Source File

SOURCE=images/designer_widgetstack.png
# End Source File
# Begin Source File

SOURCE=images/designer_wizarddata.png
# End Source File
# Begin Source File

SOURCE=images/designer_wizarddialog.png
# End Source File
# Begin Source File

SOURCE=images/designer_wordwrap.png
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=about.cpp
# End Source File
# Begin Source File

SOURCE=about.h
# End Source File
# Begin Source File

SOURCE=.\actioneditor.cpp
# End Source File
# Begin Source File

SOURCE=configtoolboxdialog.cpp
# End Source File
# Begin Source File

SOURCE=configtoolboxdialog.h
# End Source File
# Begin Source File

SOURCE=connectiondialog.cpp
# End Source File
# Begin Source File

SOURCE=connectiondialog.h
# End Source File
# Begin Source File

SOURCE=createtemplate.cpp
# End Source File
# Begin Source File

SOURCE=createtemplate.h
# End Source File
# Begin Source File

SOURCE=customwidgeteditor.cpp
# End Source File
# Begin Source File

SOURCE=customwidgeteditor.h
# End Source File
# Begin Source File

SOURCE=dbconnection.cpp
# End Source File
# Begin Source File

SOURCE=dbconnection.h
# End Source File
# Begin Source File

SOURCE=dbconnectioneditor.cpp
# End Source File
# Begin Source File

SOURCE=dbconnectioneditor.h
# End Source File
# Begin Source File

SOURCE=dbconnections.cpp
# End Source File
# Begin Source File

SOURCE=dbconnections.h
# End Source File
# Begin Source File

SOURCE=editfunctions.cpp
# End Source File
# Begin Source File

SOURCE=editfunctions.h
# End Source File
# Begin Source File

SOURCE=finddialog.cpp
# End Source File
# Begin Source File

SOURCE=finddialog.h
# End Source File
# Begin Source File

SOURCE=formsettings.cpp
# End Source File
# Begin Source File

SOURCE=formsettings.h
# End Source File
# Begin Source File

SOURCE=gotolinedialog.cpp
# End Source File
# Begin Source File

SOURCE=gotolinedialog.h
# End Source File
# Begin Source File

SOURCE=iconvieweditor.cpp
# End Source File
# Begin Source File

SOURCE=iconvieweditor.h
# End Source File
# Begin Source File

SOURCE=listboxeditor.cpp
# End Source File
# Begin Source File

SOURCE=listboxeditor.h
# End Source File
# Begin Source File

SOURCE=listeditor.cpp
# End Source File
# Begin Source File

SOURCE=listeditor.h
# End Source File
# Begin Source File

SOURCE=listvieweditor.cpp
# End Source File
# Begin Source File

SOURCE=listvieweditor.h
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_about.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_actioneditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_actionlistview.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_asciivalidator.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_command.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_configtoolboxdialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_connectiondialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_connectionitems.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_connectiontable.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_createtemplate.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_customwidgeteditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_customwidgeteditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_database.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_dbconnection.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_dbconnectioneditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_dbconnectionimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_dbconnections.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_dbconnectionsimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_editfunctions.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_editfunctionsimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_finddialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_formfile.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_formsettings.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_formsettingsimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_formwindow.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_gotolinedialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_hierarchyview.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_iconvieweditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_iconvieweditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_layout.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listboxdnd.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listboxeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listboxeditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listboxrename.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listdnd.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listviewdnd.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listvieweditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_listvieweditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_mainwindow.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_menubareditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_multilineeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_multilineeditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_newform.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_newformimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_orderindicator.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_outputwindow.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_paletteeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_paletteeditoradvanced.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_paletteeditoradvancedimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_paletteeditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_pixmapchooser.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_pixmapcollectioneditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_pixmapfunction.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_popupmenueditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_preferences.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_previewframe.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_previewwidget.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_previewwidgetimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_project.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_projectsettings.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_projectsettingsimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_propertyeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_qcompletionedit.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_replacedialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_richtextfontdialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_sizehandle.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_sourceeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_sourcefile.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_startdialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_startdialogimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_styledbutton.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_tableeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_tableeditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_timestamp.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_variabledialog.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_variabledialogimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_widgetaction.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_widgetfactory.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_wizardeditor.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_wizardeditorimpl.cpp"
# End Source File
# Begin Source File

SOURCE="tmp\moc\release-shared-mt\moc_workspace.cpp"
# End Source File
# Begin Source File

SOURCE=multilineeditor.cpp
# End Source File
# Begin Source File

SOURCE=multilineeditor.h
# End Source File
# Begin Source File

SOURCE=newform.cpp
# End Source File
# Begin Source File

SOURCE=newform.h
# End Source File
# Begin Source File

SOURCE=paletteeditor.cpp
# End Source File
# Begin Source File

SOURCE=paletteeditor.h
# End Source File
# Begin Source File

SOURCE=paletteeditoradvanced.cpp
# End Source File
# Begin Source File

SOURCE=paletteeditoradvanced.h
# End Source File
# Begin Source File

SOURCE=pixmapcollectioneditor.cpp
# End Source File
# Begin Source File

SOURCE=pixmapcollectioneditor.h
# End Source File
# Begin Source File

SOURCE=pixmapfunction.cpp
# End Source File
# Begin Source File

SOURCE=pixmapfunction.h
# End Source File
# Begin Source File

SOURCE=preferences.cpp
# End Source File
# Begin Source File

SOURCE=preferences.h
# End Source File
# Begin Source File

SOURCE=previewwidget.cpp
# End Source File
# Begin Source File

SOURCE=previewwidget.h
# End Source File
# Begin Source File

SOURCE=projectsettings.cpp
# End Source File
# Begin Source File

SOURCE=projectsettings.h
# End Source File
# Begin Source File

SOURCE=replacedialog.cpp
# End Source File
# Begin Source File

SOURCE=replacedialog.h
# End Source File
# Begin Source File

SOURCE=richtextfontdialog.cpp
# End Source File
# Begin Source File

SOURCE=richtextfontdialog.h
# End Source File
# Begin Source File

SOURCE=startdialog.cpp
# End Source File
# Begin Source File

SOURCE=startdialog.h
# End Source File
# Begin Source File

SOURCE=tableeditor.cpp
# End Source File
# Begin Source File

SOURCE=tableeditor.h
# End Source File
# Begin Source File

SOURCE=variabledialog.cpp
# End Source File
# Begin Source File

SOURCE=variabledialog.h
# End Source File
# Begin Source File

SOURCE=wizardeditor.cpp
# End Source File
# Begin Source File

SOURCE=wizardeditor.h
# End Source File
# End Group
# End Target
# End Project
