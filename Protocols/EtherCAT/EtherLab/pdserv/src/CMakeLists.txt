#############################################################################
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
#############################################################################

INCLUDE_DIRECTORIES(
        ${PROJECT_BINARY_DIR}           # for config.h
        ${PROJECT_BINARY_DIR}/include   # for pdserv.h
        ${LIBCCEXT2_INCLUDE_DIRS}
        ${CYRUS_SASL_INCLUDE_DIR}
        ${GNUTLS_INCLUDE_DIR}
        )

#IF (GNUTLS_FOUND)
#    LIST (APPEND DEFS ${GNUTLS_DEFINITIONS} GNUTLS_FOUND)
#ENDIF ()

SET (msrproto_src
    msrproto/StatSignal.cpp             msrproto/StatSignal.h
    msrproto/Event.cpp                  msrproto/Event.h
    msrproto/TimeSignal.cpp             msrproto/TimeSignal.h
    msrproto/Subscription.cpp           msrproto/Subscription.h
    msrproto/SubscriptionManager.cpp    msrproto/SubscriptionManager.h
    msrproto/Session.cpp                msrproto/Session.h
    msrproto/XmlParser.cpp              msrproto/XmlParser.h
    msrproto/Attribute.cpp              msrproto/Attribute.h
    msrproto/XmlElement.cpp             msrproto/XmlElement.h
    msrproto/Variable.cpp               msrproto/Variable.h
    msrproto/Channel.cpp                msrproto/Channel.h
    msrproto/Parameter.cpp              msrproto/Parameter.h
    msrproto/DirectoryNode.cpp          msrproto/DirectoryNode.h
    msrproto/HyperDirNode.cpp           msrproto/HyperDirNode.h
    msrproto/Server.cpp                 msrproto/Server.h
    )

SET (main_src
                        SessionStatistics.h
    Config.cpp          Config.h
    Session.cpp         Session.h
    SessionTask.cpp     SessionTask.h
    Task.cpp            Task.h
    Main.cpp            Main.h
    Variable.cpp        Variable.h
    DataType.cpp        DataType.h
    Database.cpp        Database.h
    Signal.cpp          Signal.h
    Event.cpp           Event.h
    Parameter.cpp       Parameter.h
    ProcessParameter.cpp ProcessParameter.h
    Debug.cpp           Debug.h
    )

IF (GNUTLS_FOUND)
    LIST (APPEND main_src TLS.h TLS.cpp)
ENDIF ()

SET (LIBS
    ${LIBCCEXT2_LIBRARIES}
    ${YAML_LIBRARIES}
    ${LOG4CPLUS_LIBRARIES}
    ${BerkeleyDB_LIBRARIES}
    ${CYRUS_SASL_SHARED_LIB}
    ${CYRUS_SASL_LIB_DEPS}
    ${GNUTLS_LIBRARIES}
    )

# Search for files required by buddy.
FIND_PATH (RT_APP_H "rt_app.h"
    PATHS ${CMAKE_INSTALL_INCLUDEDIR} /opt/etherlab/include)

#MESSAGE("ETL_DATA=${RT_APP_H}")

IF (RT_APP_H AND ENABLE_BUDDY)

    INCLUDE_DIRECTORIES(${RT_APP_H})

    INCLUDE (CheckStructHasMember)
    SET (CMAKE_REQUIRED_INCLUDES ${RT_APP_H})

    CHECK_STRUCT_HAS_MEMBER("struct task_stats" time.tv_nsec
        app_taskstats.h HAVE_TIMESPEC)
    CHECK_STRUCT_HAS_MEMBER("struct task_stats" time.tv_usec
        app_taskstats.h HAVE_TIMEVAL)

    IF (HAVE_TIMEVAL)
        MESSAGE (STATUS "Using timeval for etherlab_buddy2")
        LIST (APPEND DEFS HAVE_TIMEVAL)
    ELSEIF (HAVE_TIMESPEC)
        MESSAGE (STATUS "Using timespec for etherlab_buddy2")
        LIST (APPEND DEFS HAVE_TIMESPEC)
    ELSE ()
        MESSAGE (FATAL_ERROR
            "Could not find out whether app_taskstats.h "
            "uses struct timespec or struct timeval. "
            "Maybe the header file is not found.")
    ENDIF()

    CHECK_STRUCT_HAS_MEMBER("struct rt_app" port rt_app.h HAVE_SIMULINK_PORT)
    IF (HAVE_SIMULINK_PORT)
        MESSAGE (STATUS "etherlab_buddy2 understands port= option")
        LIST (APPEND DEFS HAVE_SIMULINK_PORT)
    ENDIF ()

    FIND_PACKAGE (LibDaemon REQUIRED)

    ADD_EXECUTABLE (etherlab_buddy2
        buddy/SignalInfo.cpp            buddy/SignalInfo.h
        buddy/Signal.cpp                buddy/Signal.h
        buddy/Event.cpp                 buddy/Event.h
        buddy/EventQ.cpp                buddy/EventQ.h
        buddy/Task.cpp                  buddy/Task.h
        buddy/Main.cpp                  buddy/Main.h
        buddy/Parameter.cpp             buddy/Parameter.h
        buddy/SessionTaskData.cpp       buddy/SessionTaskData.h
        buddy/main.cpp

        "${ETL_DATA}" "${BUDDY_IOCTL}"

        ${msrproto_src}
        ${main_src}
        )
    SET_TARGET_PROPERTIES ( etherlab_buddy2 PROPERTIES
        COMPILE_DEFINITIONS "${DEFS}")

    INCLUDE_DIRECTORIES (${LIBDAEMON_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES (etherlab_buddy2 ${LIBS} ${LIBDAEMON_LIBRARIES})
    INSTALL (TARGETS etherlab_buddy2
        DESTINATION "${CMAKE_INSTALL_BINDIR}" RUNTIME)

    INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/buddy/buddy.conf"
        DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}"
        RENAME "buddy.conf.orig"
        )
ENDIF ()

ADD_LIBRARY( ${PROJECT_NAME} SHARED
    lib/interface.cpp
                            lib/ShmemDataStructures.h
    lib/Task.cpp            lib/Task.h
    lib/Main.cpp            lib/Main.h
    lib/Signal.cpp          lib/Signal.h
    lib/Event.cpp           lib/Event.h
    lib/Parameter.cpp       lib/Parameter.h
    lib/SessionTaskData.cpp lib/SessionTaskData.h

    ${msrproto_src}
    ${main_src}
    )

SET_TARGET_PROPERTIES ( ${PROJECT_NAME} PROPERTIES
    COMPILE_DEFINITIONS "${DEFS}"
    VERSION ${VERSION}
    SOVERSION ${SOVERSION}
    CLEAN_DIRECT_OUTPUT 1)

TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${LIBS})
INSTALL (TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_LIBDIR}" LIBRARY)

# pkgconfig files
CONFIGURE_FILE (
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpdserv.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libpdserv.pc" @ONLY
    )
INSTALL (FILES "${CMAKE_CURRENT_BINARY_DIR}/libpdserv.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
