#------------------------------------------------------------------------------
#
# $Id$
#
# This file is part of the Data Logging Service (DLS).
#
# DLS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# DLS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along
# with DLS. If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------

SUBDIRS = images

REV = `$(top_srcdir)/revision.sh $(top_srcdir)`

appdir=$(datarootdir)/applications
dist_app_DATA =

#------------------------------------------------------------------------------

bin_PROGRAMS =

if ENABLE_FLTK
bin_PROGRAMS += dls_ctl dls_view
endif

#------------------------------------------------------------------------------

dls_ctl_CXXFLAGS = -Wall -fPIE -DREVISION=\"$(REV)\" @FLTK_CPPFLAGS@
dls_ctl_LDFLAGS = -pie -L$(builddir)/../lib/.libs -pthread
dls_ctl_LDADD = @FLTK_LDFLAGS@ -ldls $(XML_LIBS) -lprotobuf
dls_ctl_DEPENDENCIES = $(builddir)/../lib/libdls.la

if ENABLE_FLTK
install-exec-hook:
	chmod +s $(DESTDIR)$(bindir)/dls_ctl
endif

dls_ctl_SOURCES = \
	CtlDialogChannel.cpp \
	CtlDialogChannels.cpp \
	CtlDialogJob.cpp \
	CtlDialogJobEdit.cpp \
	CtlDialogMain.cpp \
	CtlDialogMsg.cpp \
	CtlJobPreset.cpp \
	CtlMain.cpp \
	fl_grid.cpp

dist_app_DATA += \
	dls_ctl.desktop

#------------------------------------------------------------------------------

dls_view_CXXFLAGS = -Wall -DREVISION=\"$(REV)\" @FLTK_CPPFLAGS@ \
	@FFTW3_CFLAGS@ @ZLIB_CFLAGS@
dls_view_LDFLAGS = -L$(builddir)/../lib/.libs \
	@FFTW3_LDFLAGS@ @ZLIB_LDFLAGS@ -pthread
dls_view_LDADD = @FLTK_LDFLAGS@ -ldls @FFTW3_LIBS@ -lm @ZLIB_LIBS@ \
	$(XML_LIBS) -lprotobuf
dls_view_DEPENDENCIES = $(builddir)/../lib/libdls.la

dls_view_SOURCES = \
	ViewChannel.cpp \
	ViewDialogExport.cpp \
	ViewDialogMain.cpp \
	ViewMain.cpp \
	ViewViewData.cpp \
	ViewViewMsg.cpp \
	fl_grid.cpp \
	fl_track_bar.cpp

dist_app_DATA += \
	dls_view.desktop

#------------------------------------------------------------------------------

noinst_HEADERS = \
	CtlDialogChannel.h \
	CtlDialogChannels.h \
	CtlDialogJob.h \
	CtlDialogJobEdit.h \
	CtlDialogMain.h \
	CtlDialogMsg.h \
	CtlGlobals.h \
	CtlJobPreset.h \
	ViewChannel.h \
	ViewDialogExport.h \
	ViewDialogMain.h \
	ViewGlobals.h \
	ViewViewData.h \
	ViewViewMsg.h \
	fl_grid.h \
	fl_grid.h \
	fl_track_bar.h

#------------------------------------------------------------------------------
