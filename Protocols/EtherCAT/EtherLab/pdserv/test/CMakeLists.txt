##############################################################################
#
#  $Id$
#
#  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
#
#  This file is part of the pdserv library.
#
#  The pdserv library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  The pdserv library is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#  License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.
#
##############################################################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

INCLUDE_DIRECTORIES(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/msrproto)

LINK_DIRECTORIES(${pdserv_BINARY_DIR}/src)

ADD_EXECUTABLE(xmlwriter xmlwriter.cpp)
TARGET_LINK_LIBRARIES(xmlwriter ${PROJECT_NAME})

ADD_EXECUTABLE(config config.cpp)
TARGET_LINK_LIBRARIES(config ${PROJECT_NAME})

ADD_EXECUTABLE(test1 test1.cpp)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_NAME})

ADD_EXECUTABLE(datatype datatype.cpp)
TARGET_LINK_LIBRARIES(datatype ${PROJECT_NAME})

ADD_EXECUTABLE(parser
    parser.cpp ${PROJECT_SOURCE_DIR}/src/msrproto/XmlParser.cpp
    ${PROJECT_SOURCE_DIR}/src/Debug.cpp)
TARGET_LINK_LIBRARIES (parser ${LIBCCEXT2_LDFLAGS}
    ${LOG4CPLUS_LIBRARIES})

#ADD_TEST(test1 test1)
ADD_TEST(parser parser)
