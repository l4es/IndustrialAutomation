#-----------------------------------------------------------------------------
#
#  Testmanager - Graphical Automation and Visualisation Tool
#
#  Copyright (C) 2018  Florian Pose <fp@igh.de>
#
#  This file is part of Testmanager.
#
#  Testmanager is free software: you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the Free
#  Software Foundation, either version 3 of the License, or (at your option)
#  any later version.
#
#  Testmanager is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU General Public License along
#  with Testmanager. If not, see <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------------

TEMPLATE = app
TARGET = testmanager

#-----------------------------------------------------------------------------

QT += network xml testlib svg

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    CONFIG += c++11
}
else {
    QMAKE_CXXFLAGS += -std=c++0x
}

#-----------------------------------------------------------------------------

DEPENDPATH += .

OBJECTS_DIR = obj/
RCC_DIR = rcc/
MOC_DIR = moc/
UI_DIR = uic/

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

isEmpty(PYTHON_INCLUDES) {
    PYTHON_INCLUDES = $$system(python3-config --includes)
}

isEmpty(PYTHON_LIBS) {
    PYTHON_LIBS = $$system(python3-config --libs)
}

QMAKE_CXXFLAGS += $$PYTHON_INCLUDES
LIBS += $$PYTHON_LIBS -lpdcom -lQtPdWidgets

isEmpty(VERSION) {
    VERSION = $(shell $$PWD/revision.sh $$PWD)
}
QMAKE_CXXFLAGS += -DVERSION=$$VERSION

!isEmpty(GPROF) {
    QMAKE_CXXFLAGS_DEBUG *= -pg
    QMAKE_LFLAGS_DEBUG *= -pg
    CONFIG += debug
}

target.path = $$PREFIX/bin
INSTALLS += target

HEADERS += \
    AboutDialog.h \
    ColorDelegate.h \
    ConnectDialog.h \
    DataModel.h \
    DataNode.h \
    DataSlot.h \
    DataSource.h \
    HandleWidget.h \
    MainWindow.h \
    Plugin.h \
    Property.h \
    PropertyDelegate.h \
    PropertyFlag.h \
    PropertyGroup.h \
    PropertyModel.h \
    PropertyNode.h \
    PythonShell.h \
    SignalReceiver.h \
    SlotDialog.h \
    SlotModel.h \
    SlotNode.h \
    StyleDialog.h \
    TabDialog.h \
    TabPage.h \
    WidgetContainer.h \
    lib/csshighlighter_p.h \
    lib/qcssparser_p.h \
    lib/qfunctions_p.h \
    lib/shared_global_p.h \
    plugins/DialPlugin.h \
    plugins/DigitalPlugin.h \
    plugins/DoubleSpinBoxPlugin.h \
    plugins/GraphPlugin.h \
    plugins/PushButtonPlugin.h \
    plugins/QLabelPlugin.h \
    plugins/XYGraphPlugin.h

SOURCES += \
    AboutDialog.cpp \
    ColorDelegate.cpp \
    ConnectDialog.cpp \
    DataModel.cpp \
    DataNode.cpp \
    DataSlot.cpp \
    DataSource.cpp \
    HandleWidget.cpp \
    MainWindow.cpp \
    Plugin.cpp \
    Property.cpp \
    PropertyDelegate.cpp \
    PropertyFlag.cpp \
    PropertyGroup.cpp \
    PropertyModel.cpp \
    PropertyNode.cpp \
    PythonShell.cpp \
    SignalReceiver.cpp \
    SlotDialog.cpp \
    SlotModel.cpp \
    SlotNode.cpp \
    StyleDialog.cpp \
    TabDialog.cpp \
    TabPage.cpp \
    WidgetContainer.cpp \
    lib/csshighlighter.cpp \
    lib/qcssparser.cpp \
    lib/qcssscanner.cpp \
    main.cpp \
    plugins/DialPlugin.cpp \
    plugins/DigitalPlugin.cpp \
    plugins/DoubleSpinBoxPlugin.cpp \
    plugins/GraphPlugin.cpp \
    plugins/PushButtonPlugin.cpp \
    plugins/QLabelPlugin.cpp \
    plugins/XYGraphPlugin.cpp

!isEmpty(MODEL_TEST) {
    HEADERS += modeltest.h
    SOURCES += modeltest.cpp
    DEFINES += TM_MODEL_TEST
}

FORMS += \
    AboutDialog.ui \
    ConnectDialog.ui \
    MainWindow.ui \
    SlotDialog.ui \
    StyleDialog.ui \
    TabDialog.ui

RESOURCES += \
    testmanager.qrc

RC_FILE = testmanager.rc

TRANSLATIONS = testmanager_de.ts
CODECFORTR = UTF-8

include(updateqm.pri)

#-----------------------------------------------------------------------------
