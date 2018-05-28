##############################################################################
#
#  $Id$
#
#  Copyright (C) 2015-2016  Richard Hacker (lerichi at gmx dot net)
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
#
#  This is the main cmake file for building process data communications client
#
#  The following options can be specified on the command line of cmake to
#  change the installation paths (Defaults are in <>)
#
#       -DCMAKE_INSTALL_PREFIX=</usr/local>
#       -DCMAKE_INSTALL_INCLUDEDIR=<${CMAKE_INSTALL_PREFIX}/include>
#       -DCMAKE_INSTALL_BINDIR=<${CMAKE_INSTALL_PREFIX}/bin>
#       -DCMAKE_INSTALL_LIBDIR=<${CMAKE_INSTALL_PREFIX}/lib>
#
#  To enable python bindings, use
#       -DPYTHONLIB=1
#
#  For debugging, use
#
#       -DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo|MinSizeRel
#       -DDEBUG=1               # prints debug messages in terminal
#                                 when CMAKE_BUILD_TYPE=Debug
#
##############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(pdcom)

# Release Instructions
#
# - Update version numbers below
# - Update SOVERSION below
# - Update ChangeLog (hg log --branch <branch> --style=changelog > ChangeLog)
# - Update NEWS file
# - commit
# - make dist
# - add tag release-x.x.x
#
SET (MAJOR_VERSION 4)
SET (MINOR_VERSION 0)
SET (PATCH_LEVEL 1)
SET (VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL})

# library version
SET (SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

INCLUDE (GNUInstallDirs)

OPTION ( DEBUG "Debugging output when CMAKE_BUILD_TYPE=Debug" OFF )
IF (("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") AND (DEBUG))
    SET_DIRECTORY_PROPERTIES(PROPERTIES COMPILE_DEFINITIONS "PDC_DEBUG")
ENDIF()

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}       # pdcom/*.h
    ${PROJECT_BINARY_DIR}       # For config.h
    )
 
FIND_PACKAGE(EXPAT REQUIRED)
INCLUDE_DIRECTORIES(${EXPAT_INCLUDE_DIRS})

FIND_PACKAGE (Doxygen)
IF (DOXYGEN_FOUND)
    ADD_SUBDIRECTORY (doc)
ENDIF()

IF (CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    ADD_DEFINITIONS(-Wall -Wextra)
ENDIF()

FILE (GLOB LIBHEADERS "${PROJECT_SOURCE_DIR}/pdcom/*.h")
ADD_LIBRARY(${PROJECT_NAME} SHARED
    ${LIBHEADERS}
    pdcom/Variable.h                    Variable.cpp
    pdcom/Process.h                     Process.cpp
    pdcom/Subscriber.h                  Subscriber.cpp
    Debug.h                             Debug.cpp
    ProtocolHandler.h                   ProtocolHandler.cpp
    IOLayer.h                           IOLayer.cpp
    StreambufLayer.h                    StreambufLayer.cpp
    SpyLayer.h                          SpyLayer.cpp
    msrproto/SubscriptionList.h
    msrproto/DirNode.h                  msrproto/DirNode.cpp
    msrproto/Request.h                  msrproto/Request.cpp
    msrproto/Base64.h                   msrproto/Base64.cpp
    msrproto/MsrVariable.h              msrproto/MsrVariable.cpp
    msrproto/Channel.h                  msrproto/Channel.cpp
    msrproto/Parameter.h                msrproto/Parameter.cpp
    msrproto/ProtocolHandler.h          msrproto/ProtocolHandler.cpp
    msrproto/DataGroup.h                msrproto/DataGroup.cpp
    )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${EXPAT_LIBRARIES})

SET_TARGET_PROPERTIES ( ${PROJECT_NAME} PROPERTIES
    VERSION ${VERSION}
    SOVERSION "${SOVERSION}"
    CLEAN_DIRECT_OUTPUT 1)

INSTALL (TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

INSTALL (FILES "${PROJECT_BINARY_DIR}/pdcom.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
INSTALL (DIRECTORY pdcom
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h")

# Python library
SET (PYTHONLIB "${PYTHONLIB}" CACHE BOOL
    "Build pdcom library for python" FORCE)
IF (PYTHONLIB)
    ADD_SUBDIRECTORY(python)
ENDIF()

#ENABLE_TESTING()
#ADD_SUBDIRECTORY (test)

OPTION ( EXAMPLE "Build example" OFF )
IF (EXAMPLE)
    ADD_SUBDIRECTORY (example)
ENDIF()

string(LENGTH "${PROJECT_SOURCE_DIR}/src/" SRC_PATH_LENGTH)
CONFIGURE_FILE (config.h.in config.h)
CONFIGURE_FILE ( pdcom.h.in pdcom.h)
CONFIGURE_FILE (pdcom.spec.in pdcom.spec @ONLY)

SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Process data communications client library")
SET (CPACK_PACKAGE_VENDOR "Richard Hacker")
SET (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET (CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
SET (CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
SET (CPACK_PACKAGE_VERSION_PATCH ${PATCH_LEVEL})
SET (CPACK_PACKAGE_INSTALL_DIRECTORY
    "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET (CPACK_SOURCE_GENERATOR "TBZ2")
SET (CPACK_SOURCE_PACKAGE_FILE_NAME
      "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET (CPACK_GENERATOR "TGZ")
SET (CPACK_GENERATOR "DEB")
SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "Richard Hacker") #required
LIST (APPEND CPACK_SOURCE_IGNORE_FILES
    "/build"
    "/\\\\.hg"
    "\\\\.sw.$"
    "~$"
    "\\\\.orig$"
    )

INCLUDE (CPack)
ADD_CUSTOM_TARGET (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
