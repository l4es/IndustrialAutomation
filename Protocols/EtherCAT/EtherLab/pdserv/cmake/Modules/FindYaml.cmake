# - Try to find Yaml
# Once done this will define
#  YAML_FOUND - System has yaml
#  YAML_INCLUDE_DIRS - The yaml include directories
#  YAML_LIBRARIES - The libraries needed to use yaml
#  YAML_DEFINITIONS - Compiler switches required for using yaml

find_package(PkgConfig)

if (NOT ${CMAKE_VERSION} VERSION_LESS 2.8)
    set (QUIET QUIET)
endif ()
pkg_check_modules(PC_YAML ${QUIET} yaml-0.1)

set(YAML_DEFINITIONS ${PC_YAML_CFLAGS_OTHER})

find_path(YAML_INCLUDE_DIR yaml.h
        HINTS ${PC_YAML_INCLUDEDIR} ${PC_YAML_INCLUDE_DIRS}
        PATH_SUFFIXES yaml )

find_library(YAML_LIBRARY
        NAMES ${PC_YAML_LIBRARIES} yaml
        HINTS ${PC_YAML_LIBDIR} ${PC_YAML_LIBRARY_DIRS} )

set(YAML_LIBRARIES ${YAML_LIBRARY} )
set(YAML_INCLUDE_DIRS ${YAML_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set YAML_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(YAML  DEFAULT_MSG
        YAML_LIBRARY YAML_INCLUDE_DIR)

mark_as_advanced(YAML_INCLUDE_DIR YAML_LIBRARY )
