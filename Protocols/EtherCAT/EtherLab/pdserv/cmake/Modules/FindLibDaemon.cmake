# - Try to find LibDaemon
# Once done this will define
#  LIBDAEMON_FOUND - System has libdaemon
#  LIBDAEMON_INCLUDE_DIRS - The libdaemon include directories
#  LIBDAEMON_LIBRARIES - The libraries needed to use libdaemon
#  LIBDAEMON_DEFINITIONS - Compiler switches required for using libdaemon

find_package(PkgConfig)
pkg_check_modules(PC_LIBDAEMON libdaemon)
set(LIBDAEMON_DEFINITIONS ${PC_LIBDAEMON_CFLAGS_OTHER})

find_path(LIBDAEMON_INCLUDE_DIR dfork.h
        HINTS ${PC_LIBDAEMON_INCLUDEDIR} ${PC_LIBDAEMON_INCLUDE_DIRS}
        PATH_SUFFIXES libdaemon )

find_library(LIBDAEMON_LIBRARY
        NAMES ${PC_LIBDAEMON_LIBRARIES} daemon libdaemon 
        HINTS ${PC_LIBDAEMON_LIBDIR} ${PC_LIBDAEMON_LIBRARY_DIRS} )

set(LIBDAEMON_LIBRARIES ${LIBDAEMON_LIBRARY} )
set(LIBDAEMON_INCLUDE_DIRS ${LIBDAEMON_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBDAEMON_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibDaemon  DEFAULT_MSG
        LIBDAEMON_LIBRARY LIBDAEMON_INCLUDE_DIR)

mark_as_advanced(LIBDAEMON_INCLUDE_DIR LIBDAEMON_LIBRARY )
