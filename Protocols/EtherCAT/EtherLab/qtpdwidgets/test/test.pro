#-----------------------------------------------------------------------------
#
# $Id$
#
# Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
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
# vim: syntax=config
#
#-----------------------------------------------------------------------------

TEMPLATE = app
TARGET = pdwidgetstest
QT += gui widgets network svg
CONFIG += debug

DEPENDPATH += ..
INCLUDEPATH += ..

!isEmpty(PDCOMPREFIX) {
    INCLUDEPATH += $${PDCOMPREFIX}/include
    LIBS += -L$${PDCOMPREFIX}/lib$${LIBEXT}
}

QMAKE_LFLAGS += -L$$OUT_PWD/..
QMAKE_LFLAGS_APP += -Wl,--rpath -Wl,..

LIBS += -lQtPdWidgets -lpdcom

HEADERS += MainWindow.h
SOURCES += main.cpp MainWindow.cpp
FORMS += MainWindow.ui

#-----------------------------------------------------------------------------
