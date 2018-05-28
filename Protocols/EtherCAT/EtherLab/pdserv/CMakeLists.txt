##############################################################################
#
#  $Id$
#
#  Copyright 2010 - 2016  Richard Hacker (lerichi at gmx dot net)
#                         Florian Pose <fp@igh-essen.com>
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
#
#  This is the main cmake file for building process data communications server
#
#  The following options can be specified on the command line of cmake to
#  change the installation paths (Defaults are in <>)
#
#       -DCMAKE_INSTALL_PREFIX=</usr/local>
#       -DCMAKE_INSTALL_INCLUDEDIR=<${CMAKE_INSTALL_PREFIX}/include>
#       -DCMAKE_INSTALL_BINDIR=<${CMAKE_INSTALL_PREFIX}/bin>
#       -DCMAKE_INSTALL_LIBDIR=<${CMAKE_INSTALL_PREFIX}/lib>
#
#  For debugging, use
#
#       -DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo|MinSizeRel
#       -DDEBUG=1               # prints debug messages in terminal
#       -DTEST=1                # Compile tests (Requires
#                               #     -DCMAKE_BUILD_TYPE=Debug)
#
#  option
#
##############################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (pdserv)
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

#
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
SET (MAJOR_VERSION 3)
SET (MINOR_VERSION 0)
SET (PATCH_LEVEL 99)
SET (VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL})

# library version
SET (SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# GNU Paths:
#       CMAKE_INSTALL_*
IF (${CMAKE_VERSION} VERSION_LESS "2.8.5")
  LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/compat/")
ENDIF ()

INCLUDE (GNUInstallDirs)
INCLUDE (CMakeDependentOption)
INCLUDE (InstallRequiredSystemLibraries)

SET (ENABLE_BUDDY "${ENABLE_BUDDY}" CACHE BOOL
    "Build etherlab_buddy" FORCE)

ENABLE_TESTING()

SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Process data communications library")
SET (CPACK_PACKAGE_VENDOR "Richard Hacker")
SET (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET (CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
SET (CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
SET (CPACK_PACKAGE_VERSION_PATCH ${PATCH_LEVEL})
SET (CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET (CPACK_SOURCE_GENERATOR "TBZ2")
SET (CPACK_SOURCE_PACKAGE_FILE_NAME
      "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET (CPACK_GENERATOR "TGZ")
SET (CPACK_GENERATOR "DEB")
SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "Richard Hacker") #required
LIST (APPEND CPACK_SOURCE_IGNORE_FILES
    "/\\\\.hg"
    "\\\\.sw.$"
    "~$"
    "\\\\.orig$"
    )

INCLUDE (CPack)
ADD_CUSTOM_TARGET (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# Search for mercurial, used to add ID information to the library
FIND_PACKAGE (Mercurial)
IF (MERCURIAL_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.hg")
MERCURIAL_HG_INFO (${PROJECT_SOURCE_DIR} Package)
ENDIF ()

# Require CommonC++
FIND_PACKAGE (PkgConfig REQUIRED)
PKG_CHECK_MODULES (LIBCCEXT2 REQUIRED libccext2)
FIND_PACKAGE (Log4cplus REQUIRED)
FIND_PACKAGE (Yaml REQUIRED)
FIND_PACKAGE (BerkeleyDB REQUIRED)
FIND_PACKAGE (CyrusSASL REQUIRED)
FIND_PACKAGE (GnuTLS)

IF (UNIX)
    ADD_CUSTOM_TARGET (tags etags -R ${CMAKE_CURRENT_SOURCE_DIR})
    ADD_CUSTOM_TARGET (etags DEPENDS tags)
ENDIF ()

ADD_DEFINITIONS (-Wall -Wextra)

ADD_SUBDIRECTORY (src)

STRING (COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Debug" ENABLE_TEST)
CMAKE_DEPENDENT_OPTION (TEST "Compile test subdirectory" ON
    "ENABLE_TEST" OFF)
IF (TEST)
  ADD_SUBDIRECTORY (test)
ENDIF ()

OPTION ( DEBUG "Debugging output" OFF )
SET  (PDS_DEBUG ${DEBUG})

if (DEBUG)
    string(LENGTH "${PROJECT_SOURCE_DIR}/src/" SRC_PATH_LENGTH)
endif (DEBUG)

CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
    )

CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/pdserv.spec.in"
    "${PROJECT_BINARY_DIR}/pdserv.spec"
    )

CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/include/pdserv.h.in"
    "${PROJECT_BINARY_DIR}/include/pdserv.h"
    )

INSTALL (FILES "${PROJECT_BINARY_DIR}/include/pdserv.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/pdserv.conf"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}"
    RENAME "pdserv.conf.orig"
    )
