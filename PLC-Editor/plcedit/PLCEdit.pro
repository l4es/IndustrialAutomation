CONFIG += stl \
 warn_on \
 release \
 rtti \
 qt
DESTDIR = release/
FORMS += ui/AboutDialog.ui \
 ui/BatchDialog.ui \
 ui/FBDialog.ui \
 ui/FileViewDialog.ui \
 ui/FindReplaceDialog.ui \
 ui/FncDialog.ui \
 ui/HelpWidget.ui \
 ui/PathFileNameDialog.ui \
 ui/POUInfoDialog.ui \
 ui/PrefDialog.ui \
 ui/SessionDialog.ui \
 ui/UpdateDialog.ui
HEADERS += src/aboutdialog.h \
 src/batchdialog.h \
 src/diffdialog.h \
 src/editor.h \
 src/fbcallconverter.h \
 src/fbdialog.h \
 src/fileviewdialog.h \
 src/finddialog.h \
 src/fncdialog.h \
 src/helpwidget.h \
 src/highlighter_dec.h \
 src/highlighter_ins.h \
 src/importexport.h \
 src/mainwindow.h \
 src/pagectrl.h \
 src/pagedata.h \
 src/pathfilenamedialog.h \
 src/pouinfodialog.h \
 src/prefdialog.h \
 src/printprepare.h \
 src/prototype.h \
 src/sessionmanager.h \
 src/tabwidget.h \
 src/updateinfo.h \
 src/singleapp/qtlocalpeer.h \
 src/singleapp/qtlockedfile.h \
 src/singleapp/qtsingleapplication.h \
 src/beediff/BtStringTool.h \
 src/beediff/BtToken.h \
 src/beediff/QBiConfigWidget.h \
 src/beediff/QBtBrowser.h \
 src/beediff/QBtColorDemo.h \
 src/beediff/QBtConfig.h \
 src/beediff/QBtConfigDialog.h \
 src/beediff/QBtConfigDiffProcess.h \
 src/beediff/QBtConfigOthers.h \
 src/beediff/QBtConfigTextViewer.h \
 src/beediff/QBtDiffInfo.h \
 src/beediff/QBtDiffProcess.h \
 src/beediff/QBtEvent.h \
 src/beediff/QBtEventsController.h \
 src/beediff/QBtIndicator.h \
 src/beediff/QBtLineData.h \
 src/beediff/QBtMarkerInfo.h \
 src/beediff/QBtNumeration.h \
 src/beediff/QBtOperator.h \
 src/beediff/QBtRange.h \
 src/beediff/QBtSaveQuestion.h \
 src/beediff/QBtSeparator.h \
 src/beediff/QBtSettings.h \
 src/beediff/QBtShared.h \
 src/beediff/QBtSyntax.h \
 src/beediff/QBtWorkspace.h
SOURCES += src/aboutdialog.cpp \
 src/batchdialog.cpp \
 src/diffdialog.cpp \
 src/editor.cpp \
 src/fbcallconverter.cpp \
 src/fbdialog.cpp \
 src/fileviewdialog.cpp \
 src/finddialog.cpp \
 src/fncdialog.cpp \
 src/helpwidget.cpp \
 src/highlighter_dec.cpp \
 src/highlighter_ins.cpp \
 src/importexport.cpp \
 src/main.cpp \
 src/mainwindow.cpp \
 src/pagectrl.cpp \
 src/pagedata.cpp \
 src/pathfilenamedialog.cpp \
 src/pouinfodialog.cpp \
 src/prefdialog.cpp \
 src/printprepare.cpp \
 src/prototype.cpp \
 src/sessionmanager.cpp \
 src/tabwidget.cpp \
 src/updateinfo.cpp \
 src/singleapp/qtlocalpeer.cpp \
 src/singleapp/qtlockedfile.cpp \
 src/singleapp/qtlockedfile_unix.cpp \
 src/singleapp/qtlockedfile_win.cpp \
 src/singleapp/qtsingleapplication.cpp \
 src/beediff/BtStringTool.cpp \
 src/beediff/QBtBrowser.cpp \
 src/beediff/QBtColorDemo.cpp \
 src/beediff/QBtConfig.cpp \
 src/beediff/QBtConfigDialog.cpp \
 src/beediff/QBtConfigDiffProcess.cpp \
 src/beediff/QBtConfigOthers.cpp \
 src/beediff/QBtConfigTextViewer.cpp \
 src/beediff/QBtDiffInfo.cpp \
 src/beediff/QBtDiffProcess.cpp \
 src/beediff/QBtEventsController.cpp \
 src/beediff/QBtIndicator.cpp \
 src/beediff/QBtNumeration.cpp \
 src/beediff/QBtOperator.cpp \
 src/beediff/QBtSaveQuestion.cpp \
 src/beediff/QBtSeparator.cpp \
 src/beediff/QBtSettings.cpp \
 src/beediff/QBtShared.cpp \
 src/beediff/QBtSyntax.cpp \
 src/beediff/QBtWorkspace.cpp
MOC_DIR += build/moc
RCC_DIR += build/rcc
RESOURCES += src/ressources/PLCEdit.qrc
UI_DIR += build/ui
QT += core gui network
TARGET = PLCEdit
TEMPLATE = app
macx {
 OBJECTS_DIR +=  build/obj/macx
 RC_FILE =  src/ressources/images/plce_new1_ico32_64.icns
 CONFIG +=  x86
}
win32 {
 OBJECTS_DIR +=  build/obj/win32
 RC_FILE =  src/ressources/PLCEdit.rc
 CONFIG +=  x86  windows
}
unix {
 OBJECTS_DIR +=  build/obj/unix
 CONFIG +=  x86  x11
}
TRANSLATIONS += Lang/Czech.ts \
 Lang/Deutsch.ts \
 Lang/English.ts \
 Lang/Francais.ts \
 Lang/Wildcard.ts
