TEMPLATE	= lib

CONFIG		+= qt warn_on staticlib qmake_cache
CONFIG 		-= dll
!force_static:!win32:contains(QT_PRODUCT,qt-internal) {
   CONFIG          -= staticlib
   CONFIG          += dll
}

TARGET	= designercore

DEFINES	+= DESIGNER
DEFINES += QT_INTERNAL_XML
DEFINES += QT_INTERNAL_WORKSPACE
DEFINES += QT_INTERNAL_ICONVIEW
DEFINES += QT_INTERNAL_TABLE
table:win32-msvc:DEFINES+=QM_TEMPLATE_EXTERN_TABLE=extern

unix {
	QMAKE_CFLAGS += $$QMAKE_CFLAGS_SHLIB
	QMAKE_CXXFLAGS += $$QMAKE_CXXFLAGS_SHLIB
}

include( ../../../src/qt_professional.pri )

SOURCES	+= command.cpp \
		formwindow.cpp \
		defs.cpp \
		layout.cpp \
		mainwindow.cpp \
		mainwindowactions.cpp \
		metadatabase.cpp \
		pixmapchooser.cpp \
		propertyeditor.cpp \
		resource.cpp \
		sizehandle.cpp \
		orderindicator.cpp \
		widgetfactory.cpp \
		hierarchyview.cpp \
		listboxeditorimpl.cpp \
		newformimpl.cpp \
		workspace.cpp \
		listvieweditorimpl.cpp \
		customwidgeteditorimpl.cpp \
		paletteeditorimpl.cpp \
		styledbutton.cpp \
		iconvieweditorimpl.cpp \
		multilineeditorimpl.cpp \
		formsettingsimpl.cpp \
		asciivalidator.cpp \
		designerapp.cpp \
		designerappiface.cpp \
		actioneditorimpl.cpp \
		actionlistview.cpp \
		actiondnd.cpp \
		project.cpp \
		projectsettingsimpl.cpp \
		sourceeditor.cpp \
		outputwindow.cpp \
		../shared/widgetdatabase.cpp \
		../shared/parser.cpp \
		pixmapcollection.cpp \
		previewframe.cpp \
		previewwidgetimpl.cpp \
		paletteeditoradvancedimpl.cpp \
		sourcefile.cpp \
		wizardeditorimpl.cpp \
		qcompletionedit.cpp \
		timestamp.cpp \
		formfile.cpp \
		widgetaction.cpp \
		propertyobject.cpp \
		startdialogimpl.cpp \
		syntaxhighlighter_html.cpp \
		connectionitems.cpp \
		editfunctionsimpl.cpp \
		variabledialogimpl.cpp \
		listviewdnd.cpp \
		listboxdnd.cpp \
		listdnd.cpp \
		listboxrename.cpp \
		connectiontable.cpp \
		popupmenueditor.cpp \
		menubareditor.cpp

HEADERS	+= command.h \
		defs.h \
		formwindow.h \
		layout.h \
		mainwindow.h \
		metadatabase.h \
		pixmapchooser.h \
		propertyeditor.h \
		resource.h \
		sizehandle.h \
		orderindicator.h \
		widgetfactory.h \
		hierarchyview.h \
		listboxeditorimpl.h \
		newformimpl.h \
		workspace.h \
		listvieweditorimpl.h \
		customwidgeteditorimpl.h \
		paletteeditorimpl.h \
		styledbutton.h \
		iconvieweditorimpl.h \
		multilineeditorimpl.h \
		formsettingsimpl.h \
		asciivalidator.h \
		../interfaces/widgetinterface.h \
		../interfaces/actioninterface.h \
		../interfaces/filterinterface.h \
		../interfaces/designerinterface.h \
		designerapp.h \
		designerappiface.h \
		actioneditorimpl.h \
		actionlistview.h \
		actiondnd.h \
		project.h \
		projectsettingsimpl.h \
		sourceeditor.h \
		outputwindow.h \
		../shared/widgetdatabase.h \
		../shared/parser.h \
		previewframe.h \
		previewwidgetimpl.h \
		paletteeditoradvancedimpl.h \
		pixmapcollection.h \
		sourcefile.h \
		wizardeditorimpl.h \
		qcompletionedit.h \
		timestamp.h \
		formfile.h \
		widgetaction.h \
		propertyobject.h \
		startdialogimpl.h \
		syntaxhighlighter_html.h \
		connectionitems.h \
		editfunctionsimpl.h \
		variabledialogimpl.h \
		listviewdnd.h \
		listboxdnd.h \
		listdnd.h \
		listboxrename.h \
		connectiontable.h \
		popupmenueditor.h \
		menubareditor.h

FORMS		+= listboxeditor.ui \
		editfunctions.ui \
		newform.ui \
		listvieweditor.ui \
		customwidgeteditor.ui \
		paletteeditor.ui \
		iconvieweditor.ui \
		preferences.ui \
		multilineeditor.ui \
		formsettings.ui \
		about.ui \
		pixmapfunction.ui \
		createtemplate.ui \
		actioneditor.ui \
		projectsettings.ui \
		finddialog.ui \
		replacedialog.ui \
		gotolinedialog.ui \
		pixmapcollectioneditor.ui \
		previewwidget.ui \
		paletteeditoradvanced.ui \
		wizardeditor.ui \
		listeditor.ui \
		startdialog.ui \
		richtextfontdialog.ui \
		connectiondialog.ui \
		variabledialog.ui \
		configtoolboxdialog.ui

IMAGES		+= images/designer_adjustsize.png \
		images/designer_edithlayoutsplit.png \
		images/designer_left.png \
		images/designer_sizeall.png \
		images/designer_arrow.png \
		images/designer_editlower.png \
		images/designer_line.png \
		images/designer_sizeb.png \
		images/designer_background.png \
		images/designer_editpaste.png \
		images/designer_lineedit.png \
		images/designer_sizef.png \
		images/designer_book.png \
		images/designer_editraise.png \
		images/designer_listbox.png \
		images/designer_sizeh.png \
		images/designer_buttongroup.png \
		images/designer_editslots.png \
		images/designer_listview.png \
		images/designer_sizev.png \
		images/designer_checkbox.png \
		images/designer_editvlayout.png \
		images/designer_multilineedit.png \
		images/designer_slider.png \
		images/designer_combobox.png \
		images/designer_editvlayoutsplit.png \
		images/designer_newform.png \
		images/designer_spacer.png \
		images/designer_connecttool.png \
		images/designer_filenew.png \
		images/designer_no.png \
		images/designer_spinbox.png \
		images/designer_cross.png \
		images/designer_fileopen.png \
		images/designer_ordertool.png \
		images/designer_splash.png \
		images/designer_customwidget.png \
		images/designer_filesave.png \
		images/designer_pixlabel.png \
		images/designer_table.png \
		images/designer_databrowser.png \
		images/designer_form.png \
		images/designer_pointer.png \
		images/designer_tabwidget.png \
		images/designer_datatable.png \
		images/designer_frame.png \
		images/designer_print.png \
		images/designer_textbrowser.png \
		images/designer_dataview.png \
		images/designer_groupbox.png \
		images/designer_progress.png \
		images/designer_textedit.png \
		images/designer_dateedit.png \
		images/designer_hand.png \
		images/designer_project.png \
		images/designer_textview.png \
		images/designer_datetimeedit.png \
		images/designer_help.png \
		images/designer_pushbutton.png \
		images/designer_timeedit.png \
		images/designer_dial.png \
		images/designer_home.png \
		images/designer_qtlogo.png \
		images/designer_toolbutton.png \
		images/designer_toolbox.png \
		images/designer_down.png \
		images/designer_hsplit.png \
		images/designer_radiobutton.png \
		images/designer_undo.png \
		images/designer_editbreaklayout.png \
		images/designer_ibeam.png \
		images/designer_redo.png \
		images/designer_up.png \
		images/designer_resetproperty.png \
		images/designer_editcopy.png \
		images/designer_iconview.png \
		images/designer_resetproperty.png \
		images/designer_uparrow.png \
		images/designer_editcut.png \
		images/designer_image.png \
		images/designer_richtextedit.png \
		images/designer_vsplit.png \
		images/designer_editdelete.png \
		images/designer_label.png \
		images/designer_right.png \
		images/designer_wait.png \
		images/designer_editgrid.png \
		images/designer_layout.png \
		images/designer_scrollbar.png \
		images/designer_widgetstack.png \
		images/designer_edithlayout.png \
		images/designer_lcdnumber.png \
		images/designer_searchfind.png \
		images/designer_folder.png \
		images/designer_setbuddy.png \
		images/designer_textbold.png \
		images/designer_textcenter.png \
		images/designer_texth1.png \
		images/designer_texth2.png \
		images/designer_texth3.png \
		images/designer_textitalic.png \
		images/designer_textjustify.png \
		images/designer_textlarger.png \
		images/designer_textleft.png \
		images/designer_textlinebreak.png \
		images/designer_textparagraph.png \
		images/designer_textright.png \
		images/designer_textsmaller.png \
		images/designer_textteletext.png \
		images/designer_textunderline.png \
		images/designer_wizarddata.png \
		images/designer_wizarddialog.png \
		images/designer_d_adjustsize.png \
		images/designer_d_label.png \
		images/designer_d_book.png \
		images/designer_d_layout.png \
		images/designer_d_buttongroup.png \
		images/designer_d_lcdnumber.png \
		images/designer_d_checkbox.png \
		images/designer_d_left.png \
		images/designer_d_combobox.png \
		images/designer_d_line.png \
		images/designer_d_connecttool.png \
		images/designer_d_lineedit.png \
		images/designer_d_customwidget.png \
		images/designer_d_listbox.png \
		images/designer_d_databrowser.png \
		images/designer_d_listview.png \
		images/designer_d_datatable.png \
		images/designer_d_multilineedit.png \
		images/designer_d_dataview.png \
		images/designer_d_newform.png \
		images/designer_d_dateedit.png \
		images/designer_d_ordertool.png \
		images/designer_d_datetimeedit.png \
		images/designer_d_pixlabel.png \
		images/designer_d_dial.png \
		images/designer_d_pointer.png \
		images/designer_d_down.png \
		images/designer_d_print.png \
		images/designer_d_editbreaklayout.png \
		images/designer_d_progress.png \
		images/designer_d_editcopy.png \
		images/designer_d_project.png \
		images/designer_d_editcut.png \
		images/designer_d_pushbutton.png \
		images/designer_d_editdelete.png \
		images/designer_d_radiobutton.png \
		images/designer_d_editgrid.png \
		images/designer_d_redo.png \
		images/designer_d_edithlayout.png \
		images/designer_d_richtextedit.png \
		images/designer_d_edithlayoutsplit.png \
		images/designer_d_right.png \
		images/designer_d_editlower.png \
		images/designer_d_scrollbar.png \
		images/designer_d_editpaste.png \
		images/designer_d_searchfind.png \
		images/designer_d_editraise.png \
		images/designer_d_slider.png \
		images/designer_d_editslots.png \
		images/designer_d_spacer.png \
		images/designer_d_editvlayout.png \
		images/designer_d_spinbox.png \
		images/designer_d_editvlayoutsplit.png \
		images/designer_d_table.png \
		images/designer_d_filenew.png \
		images/designer_d_folder.png \
		images/designer_d_tabwidget.png \
		images/designer_d_fileopen.png \
		images/designer_d_textbrowser.png \
		images/designer_d_filesave.png \
		images/designer_d_textedit.png \
		images/designer_d_form.png \
		images/designer_d_textview.png \
		images/designer_d_frame.png \
		images/designer_d_timeedit.png \
		images/designer_d_groupbox.png \
		images/designer_d_toolbutton.png \
		images/designer_d_toolbox.png \
		images/designer_d_help.png \
		images/designer_d_undo.png \
		images/designer_d_home.png \
		images/designer_d_up.png \
		images/designer_d_iconview.png \
		images/designer_d_widgetstack.png \
		images/designer_d_setbuddy.png \
		images/designer_d_textbold.png \
		images/designer_d_texth1.png \
		images/designer_d_texth2.png \
		images/designer_d_texth3.png \
		images/designer_d_textitalic.png \
		images/designer_d_textjustify.png \
		images/designer_d_textlarger.png \
		images/designer_d_textleft.png \
		images/designer_d_textlinebreak.png \
		images/designer_d_textparagraph.png \
		images/designer_d_textright.png \
		images/designer_d_textsmaller.png \
		images/designer_d_textteletext.png \
		images/designer_d_textunderline.png \
		images/designer_d_textcenter.png \
		images/designer_d_wizarddata.png \
		images/designer_d_wizarddialog.png \
		images/designer_s_editcut.png \
		images/designer_s_up.png \
		images/designer_s_down.png \
		images/designer_s_left.png \
		images/designer_s_right.png \
		images/designer_d_image.png \
		images/designer_d_textfont.png \
		images/designer_textfont.png \
		images/designer_object.png \
		images/designer_appicon.png \
		images/designer_wordwrap.png \
		images/designer_d_wordwrap.png


OBJECTS_DIR	= .

DEPENDPATH	+= $$QT_SOURCE_TREE/include
VERSION  	= 1.0.0
DESTDIR		= $$QT_BUILD_TREE/lib

aix-g++ {
	QMAKE_CFLAGS += -mminimal-toc
	QMAKE_CXXFLAGS += -mminimal-toc
}

sql {
	SOURCES  += database.cpp dbconnectionimpl.cpp dbconnectionsimpl.cpp
	HEADERS += database.h dbconnectionimpl.h dbconnectionsimpl.h
	FORMS += dbconnections.ui dbconnection.ui dbconnectioneditor.ui
}

table {
	HEADERS += tableeditorimpl.h
	SOURCES += tableeditorimpl.cpp
	FORMS += tableeditor.ui
}

INCLUDEPATH	+= ../shared ../uilib
LIBS            += -L$$QT_BUILD_TREE/lib -lqui

hpux-acc* {
	LIBS            += $$QT_BUILD_TREE/lib/libqassistantclient.a
}

!hpux-acc* {
	LIBS		+= -lqassistantclient
}

TRANSLATIONS	= designer_de.ts designer_fr.ts

target.path=$$libs.path
INSTALLS += target

templates.path=$$data.path/templates
templates.files = ../templates/*
INSTALLS += templates

designertranlations.files = *.qm
designertranlations.path = $$translations.path
INSTALLS += designertranlations

!macx-g++:PRECOMPILED_HEADER = designer_pch.h
