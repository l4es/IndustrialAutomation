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

SUBDIRS = LibDLS

lib_LTLIBRARIES = libdls.la

#------------------------------------------------------------------------------

libdls_la_CXXFLAGS = -Wall @FFTW3_CFLAGS@ @ZLIB_CFLAGS@ $(XML_CPPFLAGS)

libdls_la_LDFLAGS = \
	@FFTW3_LDFLAGS@ \
	@ZLIB_LDFLAGS@ \
	-pthread \
	-version-info @LIBDLS_VERSION@

libdls_la_LIBADD = @FFTW3_LIBS@ @ZLIB_LIBS@ -lm -lprotobuf $(PCRE_LIBS)

libdls_la_SOURCES = \
	Base64.cpp \
	BaseMessage.cpp \
	BaseMessageList.cpp \
	Channel.cpp \
	ChannelPreset.cpp \
	Chunk.cpp \
	Data.cpp \
	Dir.cpp \
	Export.cpp \
	File.cpp \
	Job.cpp \
	JobPreset.cpp \
	Time.cpp \
	XmlParser.cpp \
	XmlTag.cpp \
	ZLib.cpp \
	globals.cpp \
	mdct.cpp

nodist_libdls_la_SOURCES = $(top_builddir)/proto/dls.pb.cc

#------------------------------------------------------------------------------

noinst_HEADERS = \
	Base64.h \
	BaseMessage.h \
	BaseMessageList.h \
	CompressionT.h \
	File.h \
	IndexT.h \
	MdctT.h \
	QuantT.h \
	RingBufferT.h \
	XmlParser.h \
	XmlTag.h \
	ZLib.h \
	mdct.h

nodist_noinst_HEADERS = $(top_builddir)/proto/dls.pb.h

#------------------------------------------------------------------------------
