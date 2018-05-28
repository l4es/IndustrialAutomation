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

FIND_PROGRAM(PYTHON "python")

IF (NOT PYTHON)
  MESSAGE(FATAL_ERROR "Unable to find python")
ENDIF()

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost REQUIRED COMPONENTS python)

IF(NOT Boost_PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "Unable to find Boost python")
ENDIF()

SET(PYLIB_MAJOR 0)
SET(PYLIB_MINOR 2)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

GET_DIRECTORY_PROPERTY (DEFS COMPILE_DEFINITIONS)
GET_DIRECTORY_PROPERTY (CFLAGS COMPILE_FLAGS)
GET_DIRECTORY_PROPERTY (OPTS COMPILE_OPTIONS)
LIST(APPEND CFLAGS ${OPTS})
GET_DIRECTORY_PROPERTY (INCLUDES INCLUDE_DIRECTORIES)
GET_DIRECTORY_PROPERTY (LIBDIR LINK_DIRECTORIES)
SET (SRC ${PROJECT_SOURCE_DIR}/python/PyProcess.cpp)
CONFIGURE_FILE (
    "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in"
    "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
    )

SET (TGT obj)

ADD_CUSTOM_COMMAND(OUTPUT ${TGT}
                   COMMAND ${PYTHON}
                   ARGS setup.py build_ext
                   DEPENDS ${LIBHEADERS})

ADD_CUSTOM_TARGET(pythonlib ALL DEPENDS ${TGT})

INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${PYTHON} setup.py install --skip-build --root=/${DESTDIR} --prefix=${CMAKE_INSTALL_PREFIX}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")
