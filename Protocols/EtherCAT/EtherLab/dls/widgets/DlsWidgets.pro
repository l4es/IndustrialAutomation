#-----------------------------------------------------------------------------
#
# Copyright (C) 2012-2017  Florian Pose <fp@igh-essen.com>
#
# This file is part of the DLS widget library.
#
# The DLS widget library is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# The DLS widget library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the DLS widget library. If not, see
# <http://www.gnu.org/licenses/>.
#
# vim: syntax=config
#
#-----------------------------------------------------------------------------

TEMPLATE = lib
TARGET = DlsWidgets

# On release:
# - Change version in Doxyfile
# - Change version in .spec file
# - Add a NEWS entry
VERSION = 0.9.0

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += dll
    QT += widgets svg printsupport xml
}
else {
    CONFIG += dll
    QT += svg
}

MOC_DIR = .moc
OBJECTS_DIR = .obj

include(updateqm.pri)

isEmpty(PREFIX) {
    unix:PREFIX = /vol/opt/etherlab
    win32:PREFIX = "c:/msys/1.0/local"
}

# Graph class debug defines
equals(DEBUG_MT_ON_SCREEN, 1) {
    DEFINES += DEBUG_MT_ON_SCREEN
}
equals(DEBUG_MT_IN_FILE, 1) {
    DEFINES += DEBUG_MT_IN_FILE
}

isEmpty(DLS_DESIGNER) {
    DLS_DESIGNER=1
}
equals(DLS_DESIGNER, 1) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        CONFIG += plugin
        QT += designer
        DEFINES += DLS_QT5 # needed for moc processing of WidgetCollection.h
    }
    else {
        CONFIG += designer plugin
    }
}
else {
    DEFINES += DLS_NO_DESIGNER
}

LIBEXT=""
unix {
    HARDWARE_PLATFORM = $$system(uname -i)
    contains(HARDWARE_PLATFORM, x86_64) {
        LIBEXT="64"
    }
}

INCLUDEPATH += $$PWD $$PWD/../lib
DEPENDPATH += $$PWD $$PWD/../lib

win32 {
    LIBS += $$OUT_PWD/../lib/.libs/libdls.a -lprotobuf -luriparser -lws2_32 -lpcre
    QMAKE_LFLAGS += -shared
}
unix {
    QMAKE_LFLAGS += -L../lib/.libs
    LIBS += -ldls
}

LIBS += -lfftw3 -lz -lxml2

equals(DLS_DESIGNER, 1) {
    target.path = $$[QT_INSTALL_PLUGINS]/designer
    INSTALLS += target
}

unix {
    libraries.path = $${PREFIX}/lib$${LIBEXT}
    libraries.files = libDlsWidgets.so
}
win32 {
    libraries.path = $${PREFIX}/bin
    libraries.files = release/DlsWidgets0.dll
}
INSTALLS += libraries

unix:inst_headers.path = $${PREFIX}/include/DlsWidgets
win32:inst_headers.path = "$${PREFIX}/include/DlsWidgets"
inst_headers.files = \
    DlsWidgets/Graph.h \
    DlsWidgets/Layer.h \
    DlsWidgets/Model.h \
    DlsWidgets/Scale.h \
    DlsWidgets/Section.h \
    DlsWidgets/Translator.h \
    DlsWidgets/ValueScale.h

INSTALLS += inst_headers

HEADERS += \
    $${inst_headers.files} \
    Channel.h \
    ColorDelegate.h \
    DatePickerDialog.h \
    Dir.h \
    ExportDialog.h \
    FilterDialog.h \
    Job.h \
    Node.h \
    SectionDialog.h \
    SectionModel.h

SOURCES += \
    Channel.cpp \
    ColorDelegate.cpp \
    DatePickerDialog.cpp \
    Dir.cpp \
    ExportDialog.cpp \
    FilterDialog.cpp \
    Graph.cpp \
    Job.cpp \
    Layer.cpp \
    Model.cpp \
    Node.cpp \
    Scale.cpp \
    Section.cpp \
    SectionDialog.cpp \
    SectionModel.cpp \
    Translator.cpp \
    ValueScale.cpp

equals(DLS_DESIGNER, 1) {
    SOURCES += \
        Plugin.cpp \
        WidgetCollection.cpp
    HEADERS += \
        Plugin.h \
        WidgetCollection.h
}

FORMS = \
    DatePickerDialog.ui \
    ExportDialog.ui \
    FilterDialog.ui \
    SectionDialog.ui

RESOURCES = DlsWidgets.qrc

TRANSLATIONS = DlsWidgets_de.ts

ADDITIONAL_DISTFILES =

DISTFILES += $${ADDITIONAL_DISTFILES}

unix {
    tags.target = tags
    tags.commands = etags --members $${SOURCES} $${HEADERS}
    QMAKE_EXTRA_TARGETS += tags
}

#-----------------------------------------------------------------------------
