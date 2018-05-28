#############################################################################
#
#  $Id$
#
#  Copyright 2012-2013  Richard Hacker (lerichi at gmx dot net)
#                 2013  Florian Pose <fp@igh.de>
#
#  This is the main cmake file for EtherLab target for Real-Time Workshop(TM)
#  target of Mathworks.
#
#  The following options can be specified on the command line of cmake to
#  change the installation paths (Defaults are in <>)
#       -DPREFIX=</opt/etherlab>
#
#  For debugging, use the
#       -DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo|MinSizeRel
#  option
#
#  This file is part of the EtherLab package.
#
#  EtherLab is free software: you can redistribute it and/or modify it under
#  the terms of the GNU General Public License as published by the Free
#  Software Foundation, either version 3 of the License, or (at your option)
#  any later version.
#
#  EtherLab is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
#  details.
#
#  You should have received a copy of the GNU General Public License along
#  with EtherLab. See COPYING. If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (etherlab)
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

#
# Release Instructions
#
# - Update version numbers below
# - Update ChangeLog (hg log --branch <branch> --style=changelog > ChangeLog)
# - Update NEWS file
# - commit
# - make dist
# - add tag release-x.x.x
#
SET (MAJOR_VERSION 2)
SET (MINOR_VERSION 1)
SET (PATCH_LEVEL 0)
SET (VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL})

# Project path options
IF (NOT PREFIX)         # Project root
    SET (PREFIX "/opt/etherlab")
ENDIF ()

SET (CMAKE_INSTALL_PREFIX "${PREFIX}")
LIST (APPEND CMAKE_PREFIX_PATH "${PREFIX}")

INCLUDE (GNUInstallDirs)

# Make sure that the project options are remembered
SET (PREFIX "${PREFIX}" CACHE PATH
    "Root directory for the project" FORCE)

# Search for mercurial, used to add ID information to the library
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
FIND_PACKAGE (Mercurial)
IF (MERCURIAL_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.hg")
MERCURIAL_HG_INFO (${PROJECT_SOURCE_DIR} Package)
ENDIF ()

ADD_SUBDIRECTORY (rtw)

SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Simulink Coder target using pdserv library")
SET (CPACK_PACKAGE_VENDOR "Richard Hacker")
SET (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
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
    "~$"
    "/build.*"
    "\\\\.sw.$"
    "\\\\.orig$"
    "/\\\\.hg"
    )

INCLUDE (CPack)
ADD_CUSTOM_TARGET (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

