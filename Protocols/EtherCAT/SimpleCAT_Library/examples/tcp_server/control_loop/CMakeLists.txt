CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(tcp_server_control_loop)

############## CMAKE OPTIONS #####################

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
SET(CMAKE_BUILD_TYPE Release)

############### SOURCE FILES ######################

SET(SOURCES control_loop.cpp)

add_executable( ${PROJECT_NAME} ${SOURCES} )

############### DEPENDENCIES ######################

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../SimplECAT.cmake)
include_directories(   ${SIMPLECAT_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${SIMPLECAT_LIBRARIES})

find_package(Hiredis REQUIRED)
include_directories(   ${HIREDIS_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${HIREDIS_LIBRARIES})

set(REDOX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../redox)
include_directories(   ${REDOX_DIR}/include)
target_link_libraries( ${PROJECT_NAME} ${REDOX_DIR}/build/libredox.so)
target_link_libraries( ${PROJECT_NAME} pthread ev)
