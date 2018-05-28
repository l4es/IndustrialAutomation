#############################################################################
#
#  $Id$
#
#  Copyright 2016 Richard Hacker (lerichi at gmx dot net)
#
#  This file is part of the pdcom library.
#
#  The pdcom library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  The pdcom library is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#  License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with the pdcom library. If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################

ADD_CUSTOM_TARGET (doc
    ${DOXYGEN_EXECUTABLE} Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

CONFIGURE_FILE (Doxyfile.in Doxyfile)
