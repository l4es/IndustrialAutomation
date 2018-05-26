cmake_minimum_required(VERSION 2.8)

project(technosoft_ipos3604)

############## CMAKE OPTIONS #####################

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_BUILD_TYPE Release)

############### SOURCE FILES ######################

set(SOURCES main.cpp)

add_executable( ${PROJECT_NAME} ${SOURCES} )

############### DEPENDENCIES ######################

include(${CMAKE_CURRENT_SOURCE_DIR}/../../SimplECAT.cmake)
include_directories(   ${SIMPLECAT_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${SIMPLECAT_LIBRARIES} )
