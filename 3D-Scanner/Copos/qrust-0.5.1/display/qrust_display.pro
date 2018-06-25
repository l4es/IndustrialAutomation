TEMPLATE	= lib
CONFIG		= qt opengl staticlib release
IMAGES  = \
        images/fileopen \
        images/undo \
        images/filesave \
        images/editcut
HEADERS		= \
                  ../math/point.h \
                  ../math/octreenode.h \
                  ../math/octreeiterator.h \
                  ../math/octreeroot.h \
                  renderobject.h \
                  octreeview.h \
                  alphaview.h \
                  crustview.h \
                  viewer.h \
                  dlggenerate.h \
                  mainwindow.h
SOURCES		= \
                  renderobject.cpp \
                  octreeview.cpp \
                  alphaview.cpp \
                  crustview.cpp \
                  viewer.cpp \
                  dlggenerate.cpp \
                  mainwindow.cpp
INTERFACES	= 
#LIBS            = -lCGAL -lQGLViewer -lGLU -lGL

TARGET          = display
