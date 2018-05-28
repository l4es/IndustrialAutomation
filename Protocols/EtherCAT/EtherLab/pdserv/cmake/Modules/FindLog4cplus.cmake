# - Find log4cplus
# Find the native log4cplus headers and libraries.
#
#  LOG4CPLUS_INCLUDE_DIRS - where to find logger.h, etc.
#  LOG4CPLUS_LIBRARIES    - List of libraries when using log4cplus.
#  LOG4CPLUS_FOUND        - True if log4cplus found.

#=============================================================================
# Copyright 2013 Richard Hacker
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

# Look for the header file.
FIND_PATH(LOG4CPLUS_INCLUDE_DIR NAMES log4cplus/logger.h)

# Look for the library.
FIND_LIBRARY(LOG4CPLUS_LIBRARY NAMES log4cplus liblog4cplus)

# handle the QUIETLY and REQUIRED arguments and set LOG4CPLUS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LOG4CPLUS DEFAULT_MSG
    LOG4CPLUS_LIBRARY LOG4CPLUS_INCLUDE_DIR)

# Copy the results to the output variables.
IF(LOG4CPLUS_FOUND)
    SET(LOG4CPLUS_LIBRARIES ${LOG4CPLUS_LIBRARY})
    SET(LOG4CPLUS_INCLUDE_DIRS ${LOG4CPLUS_INCLUDE_DIR})
ENDIF(LOG4CPLUS_FOUND)

MARK_AS_ADVANCED(LOG4CPLUS_INCLUDE_DIR LOG4CPLUS_LIBRARY)
