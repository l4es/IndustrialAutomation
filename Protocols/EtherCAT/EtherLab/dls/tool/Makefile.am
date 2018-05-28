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

if ENABLE_TOOL
bin_PROGRAMS += dls
endif

#------------------------------------------------------------------------------

dls_CXXFLAGS = -Wall -DREVISION=\"$(REV)\" @FFTW3_CFLAGS@ @ZLIB_CFLAGS@

dls_LDFLAGS = \
	-L$(top_builddir)/lib/.libs \
	@FFTW3_LDFLAGS@ \
	@ZLIB_LDFLAGS@ \
	-pthread

dls_LDADD = \
	-ldls \
	@FFTW3_LIBS@ \
	-lm \
	@ZLIB_LIBS@ \
	$(XML_LIBS) \
	-lprotobuf

dls_DEPENDENCIES = $(top_builddir)/lib/libdls.la

dls_SOURCES = \
	Export.cpp \
	List.cpp \
	main.cpp

#------------------------------------------------------------------------------
