ACLOCAL_AMFLAGS = -I m4

noinst_LIBRARIES = libdisplay.a
noinst_HEADERS = $(wildcard *.h)

libdisplay_a_SOURCES = \
alphaview.cpp    mainwindow.h \
alphaview.h      octreeview.cpp \
crustview.cpp    octreeview.h \
crustview.h      renderobject.cpp \
dlggenerate.cpp  renderobject.h \
dlggenerate.h    viewer.cpp \
mainwindow.cpp   viewer.h

all:
	QTDIR=$(QT_DIR)
	@QMAKE@ -makefile -o Makefile.qt qrust_display.pro
	make -f Makefile.qt
clean:
	QTDIR=$(QT_DIR)
	@QMAKE@ -makefile -o Makefile.qt qrust_display.pro
	make -f Makefile.qt clean

dist-hook:
	if test -d images; then \
          mkdir $(distdir)/images; \
          for pixmap in images/*; do \
            if test -f $$pixmap; then \
                cp -p $$pixmap $(distdir)/images; \
            fi \
          done \
        fi ;\
	if test -f qrust_display.pro; then \
	  cp -p qrust_display.pro $(distdir); \
	fi

