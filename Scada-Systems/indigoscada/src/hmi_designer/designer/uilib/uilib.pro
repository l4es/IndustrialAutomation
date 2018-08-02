TEMPLATE	= lib
CONFIG		+= qt warn_on release
win32:CONFIG	+= static
win32:CONFIG	-= dll
SOURCES		= qwidgetfactory.cpp \
		  ../shared/domtool.cpp \
		  ../shared/uib.cpp

HEADERS		= ../shared/domtool.h \
		  ../shared/uib.h

sql:SOURCES += 		  ../designer/database.cpp
sql:HEADERS +=		  ../designer/database2.h

DEFINES += QT_INTERNAL_XML
!win32-borland:DEFINES += Q_TEMPLATE_EXTERN=extern
include( ../../../src/qt_professional.pri )
TARGET		= qui
INCLUDEPATH	+= ../shared
DESTDIR		= ../../../lib
VERSION		= 1.0.0
DEFINES		+= RESOURCE

unix {
	target.path=$$libs.path

	INSTALLS        += target
}

mac:QMAKE_LFLAGS_PREBIND = -prebind -seg1addr 0xB9000000
