##############################################################################
#
#  $Id$
#
#  Copyright (C) 2015-2016  Richard Hacker (lerichi at gmx dot net)
#                           Florian Pose <fp@igh.de>
#
#  This file is part of the PdCom library.
#
#  The PdCom library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  The PdCom library is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#  License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
#
##############################################################################

# Search for SASL. This package is optional
FIND_PACKAGE (CyrusSASL)
IF (CYRUS_SASL_FOUND)
    ADD_DEFINITIONS(-DSASL)
    LIST(APPEND SASL_SRCS SASLClient.cpp SASLClient.h)
ENDIF ()

INCLUDE_DIRECTORIES(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
    ${CYRUS_SASL_INCLUDE_DIR})

LINK_DIRECTORIES(${pdcom_BINARY_DIR}/src)

ADD_EXECUTABLE(example example.cpp)
TARGET_LINK_LIBRARIES(example ${PROJECT_NAME}
    ${CYRUS_SASL_SHARED_LIB} ${CYRUS_SASL_LIB_DEPS})
