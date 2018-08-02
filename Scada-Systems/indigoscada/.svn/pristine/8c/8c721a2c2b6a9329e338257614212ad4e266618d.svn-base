# Copyright (c) 1996-2017, OPC Foundation. All rights reserved.
#
#   The source code in this file is covered under a dual-license scenario:
#     - RCL: for OPC Foundation members in good-standing
#     - GPL V2: everybody else
#
#   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/
#
#   GNU General Public License as published by the Free Software Foundation;
#   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2
#
#   This source code is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
cmake_minimum_required(VERSION 2.8.11)

project(UA-AnsiC)
include(CMakeToolsHelpers OPTIONAL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(_PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "Root" FORCE)
enable_testing()

#
# output paths
#
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

    option(compile_options_C "passes a string to the command line of the C compiler" OFF)
if(NOT "${compile_options_C}" STREQUAL "OFF")
    set(CMAKE_C_FLAGS "${compile_options_C} ${CMAKE_C_FLAGS}")
endif()
    option(compile_options_CXX "passes a string to the command line of the C++ compiler" OFF)
if(NOT "${compile_options_CXX}" STREQUAL "OFF")
    set(CMAKE_CXX_FLAGS "${compile_options_CXX} ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(Stack)
add_subdirectory(AnsiCSample)
