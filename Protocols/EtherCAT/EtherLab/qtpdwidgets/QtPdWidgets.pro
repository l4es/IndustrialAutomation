#-----------------------------------------------------------------------------
#
# vim: syntax=config
#
# Copyright (C) 2012-2018  Florian Pose <fp@igh-essen.com>
#
# This file is part of the QtPdWidgets library.
#
# The QtPdWidgets library is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# The QtPdWidgets library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the QtPdWidgets Library. If not, see
# <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------------

TEMPLATE = lib
TARGET = QtPdWidgets

# On release:
# - Change version in Doxyfile
# - Change version in .spec file
# - Add a NEWS entry
VERSION = 1.3.7
WIN32_LIB_VERSION = 1

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += debug_and_release
    QT += widgets designer
    DEFINES += PD_QT5 # needed for moc processing of src/WidgetCollection.h
}
else {
    CONFIG += designer release
}

CONFIG += plugin
QT += network xml svg

DEPENDPATH += .
MOC_DIR = .moc
OBJECTS_DIR = .obj

isEmpty(PREFIX) {
    unix:PREFIX = /usr/local
    win32:PREFIX = "c:/msys/1.0/local"
}

LIBEXT=""
unix {
    HARDWARE_PLATFORM = $$system(uname -m)
    contains(HARDWARE_PLATFORM, x86_64) {
        !exists("/etc/debian_version") {
            LIBEXT="64"
        }
    }
}

!isEmpty(EXPATPREFIX) {
    INCLUDEPATH += $${EXPATPREFIX}/include
    LIBS += -L$${EXPATPREFIX}/lib$${LIBEXT}
}

!isEmpty(PDCOMPREFIX) {
    INCLUDEPATH += $${PDCOMPREFIX}/include
    LIBS += -L$${PDCOMPREFIX}/lib$${LIBEXT}
}

LIBS += -lpdcom -lexpat

win32 {
    LIBS += -lwsock32 # for gethostname in Process.cpp
    CONFIG(debug, debug|release): TARGET = $$join(TARGET,,d)
    CONFIG += dll
}

isEmpty(PD_INSTALL_PLUGINS) {
    PD_INSTALL_PLUGINS = $$[QT_INSTALL_PLUGINS]
}

target.path = $${PD_INSTALL_PLUGINS}/designer

isEmpty(NO_DESIGNER){
    INSTALLS += target
}

unix {
    libraries.path = $${PREFIX}/lib$${LIBEXT}
    libraries.files = $${OUT_PWD}/libQtPdWidgets.so

    INSTALLS += libraries
}
win32 {
    libraries.path = "$${PREFIX}/lib"
    CONFIG(release, debug|release):libraries.files = \
        "release/libQtPdWidgets$${WIN32_LIB_VERSION}.dll.a"
    CONFIG(debug, debug|release):libraries.files = \
        "debug/libdQtPdWidgets$${WIN32_LIB_VERSION}.dll.a"

    dlls.path = "$${PREFIX}/bin"
    CONFIG(release, debug|release):dlls.files = \
        "release/QtPdWidgets$${WIN32_LIB_VERSION}.dll"
    CONFIG(debug, debug|release):dlls.files = \
        "debug/dQtPdWidgets$${WIN32_LIB_VERSION}.dll"

    INSTALLS += dlls libraries
}

inst_headers.path = $${PREFIX}/include/QtPdWidgets
inst_headers.files = \
    QtPdWidgets/Bar.h \
    QtPdWidgets/CheckBox.h \
    QtPdWidgets/ClipImage.h \
    QtPdWidgets/CursorEditWidget.h \
    QtPdWidgets/Dial.h \
    QtPdWidgets/Digital.h \
    QtPdWidgets/DoubleSpinBox.h \
    QtPdWidgets/Export.h \
    QtPdWidgets/Graph.h \
    QtPdWidgets/Image.h \
    QtPdWidgets/Led.h \
    QtPdWidgets/Message.h \
    QtPdWidgets/MessageModel.h \
    QtPdWidgets/MultiLed.h \
    QtPdWidgets/NoPdTouchEdit.h \
    QtPdWidgets/Process.h \
    QtPdWidgets/PushButton.h \
    QtPdWidgets/RadioButton.h \
    QtPdWidgets/ScalarSubscriber.h \
    QtPdWidgets/ScalarVariable.h \
    QtPdWidgets/Scale.h \
    QtPdWidgets/Settings.h \
    QtPdWidgets/SpinBox.h \
    QtPdWidgets/TableColumn.h \
    QtPdWidgets/TableModel.h \
    QtPdWidgets/TableView.h \
    QtPdWidgets/Text.h \
    QtPdWidgets/Time.h \
    QtPdWidgets/TimeScale.h \
    QtPdWidgets/TouchEdit.h \
    QtPdWidgets/TouchEditDialog.h \
    QtPdWidgets/Translator.h \
    QtPdWidgets/ValueRing.h \
    QtPdWidgets/Widget.h \
    QtPdWidgets/XYGraph.h

INSTALLS += inst_headers

HEADERS += \
    $${inst_headers.files} \
    src/BarSection.h \
    src/BarStack.h

SOURCES += \
    src/Bar.cpp \
    src/BarSection.cpp \
    src/BarStack.cpp \
    src/CheckBox.cpp \
    src/ClipImage.cpp \
    src/CursorEditWidget.cpp \
    src/Digital.cpp \
    src/Dial.cpp \
    src/DoubleSpinBox.cpp \
    src/Graph.cpp \
    src/Image.cpp \
    src/Led.cpp \
    src/Message.cpp \
    src/MessageModel.cpp \
    src/MultiLed.cpp \
    src/NoPdTouchEdit.cpp \
    src/Process.cpp \
    src/PushButton.cpp \
    src/RadioButton.cpp \
    src/ScalarSubscriber.cpp \
    src/Scale.cpp \
    src/Settings.cpp \
    src/SpinBox.cpp \
    src/TableColumn.cpp \
    src/TableModel.cpp \
    src/TableView.cpp \
    src/Text.cpp \
    src/Time.cpp \
    src/TimeScale.cpp \
    src/TouchEdit.cpp \
    src/TouchEditDialog.cpp \
    src/Translator.cpp \
    src/Widget.cpp \
    src/XYGraph.cpp

isEmpty(DISABLE_PLUGIN) {
    HEADERS += \
        src/Plugin.h \
        src/WidgetCollection.h

    SOURCES += \
        src/Plugin.cpp \
        src/WidgetCollection.cpp
}

RESOURCES = QtPdWidgets.qrc

TRANSLATIONS = \
    QtPdWidgets_de.ts \
    QtPdWidgets_nl.ts

QM_FILES = $$replace(TRANSLATIONS, "ts$", "qm")

CODECFORTR = UTF-8

ADDITIONAL_DISTFILES = \
    AUTHORS \
    COPYING \
    COPYING.LESSER \
    Doxyfile \
    NEWS \
    images/* \
    QtPdWidgets.spec \
    README \
    TODO \
    mydist.sh

DISTFILES += $${ADDITIONAL_DISTFILES}

unix {
    dox.target = dox
    dox.commands = doxygen Doxyfile
    QMAKE_EXTRA_TARGETS += dox

    tags.target = tags
    tags.commands = etags --members $${SOURCES} $${HEADERS}
    QMAKE_EXTRA_TARGETS += tags

    MYDISTFILES = \
        $${ADDITIONAL_DISTFILES} \
        $${HEADERS} \
        $${SOURCES} \
        $${TARGET}.pro \
        $${RESOURCES} \
        $${TRANSLATIONS} \
        $${QM_FILES} \
        test/MainWindow.cpp \
        test/MainWindow.h \
        test/MainWindow.ui \
        test/main.cpp \
        test/test.pro

    mydist.target = mydist
    mydist.commands = ./mydist.sh $${TARGET}-$${VERSION} $${MYDISTFILES}
    QMAKE_EXTRA_TARGETS += mydist
}

#-----------------------------------------------------------------------------
