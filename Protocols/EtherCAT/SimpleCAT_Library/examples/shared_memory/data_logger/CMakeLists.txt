CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(shared_memory_data_logger)

############## CMAKE OPTIONS #####################

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
SET(CMAKE_BUILD_TYPE Release)

############### SOURCE FILES ######################

SET(SOURCES data_logger.cpp)

add_executable( ${PROJECT_NAME} ${SOURCES} )

############### DEPENDENCIES ######################

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../SimplECAT.cmake)
include_directories(   ${SIMPLECAT_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${SIMPLECAT_LIBRARIES})

find_package( Boost 1.40 REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
target_link_libraries( ${PROJECT_NAME} ${Boost_LIBRARIES} pthread rt)
