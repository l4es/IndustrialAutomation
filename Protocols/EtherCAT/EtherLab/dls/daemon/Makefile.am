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

REV = `$(top_srcdir)/revision.sh $(top_srcdir)`

#------------------------------------------------------------------------------

bin_PROGRAMS =

if ENABLE_DAEMON
bin_PROGRAMS += dlsd
endif

#------------------------------------------------------------------------------

dlsd_CPPFLAGS = \
	$(CPPFLAGS) \
	-DREVISION=\"$(REV)\" \
	$(XML_CPPFLAGS)

dlsd_CXXFLAGS = \
	$(CXXFLAGS) \
	-Wall \
	-I$(top_srcdir) \
	@FFTW3_CFLAGS@ \
	@ZLIB_CFLAGS@


dlsd_LDFLAGS = \
	-L$(top_builddir)/lib/.libs
	@FFTW3_LDFLAGS@ \
	@ZLIB_LDFLAGS@ \
	-pthread

dlsd_LDADD = \
	-ldls \
	@FFTW3_LIBS@ \
	@ZLIB_LIBS@ \
	-lpdcom \
	$(XML_LIBS) \
	-lm

dlsd_DEPENDENCIES = $(top_builddir)/lib/libdls.la

dlsd_SOURCES = \
	Job.cpp \
	JobPreset.cpp \
	Logger.cpp \
	Message.cpp \
	MessageList.cpp \
	ProcLogger.cpp \
	ProcMother.cpp \
	globals.cpp \
	main.cpp

if ENABLE_SERVER
dlsd_SOURCES += Connection.cpp
nodist_dlsd_SOURCES = $(top_srcdir)/proto/dls.pb.cc
dlsd_LDADD += -lprotobuf
endif

#------------------------------------------------------------------------------

noinst_HEADERS = \
	Connection.h \
	Job.h \
	JobPreset.h \
	Logger.h \
	Message.h \
	MessageList.h \
	ProcLogger.h \
	ProcMother.h \
	SaverGenT.h \
	SaverMetaT.h \
	SaverT.h \
	globals.h

#------------------------------------------------------------------------------
