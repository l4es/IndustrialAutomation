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

AUTOMAKE_OPTIONS = no-dependencies

protobuf_sources = dls.pb.cc dls.pb.h

all-local: $(protobuf_sources)

$(protobuf_sources): dls.proto
	protoc $< --cpp_out=$(builddir)

EXTRA_DIST = dls.proto
CLEANFILES = $(protobuf_sources)

#------------------------------------------------------------------------------
