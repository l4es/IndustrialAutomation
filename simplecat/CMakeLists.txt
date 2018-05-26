cmake_minimum_required(VERSION 2.8)

project(simplecat)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_BUILD_TYPE Release)

# ===== USER CONFIGURATION ======
set(LIB_TYPE SHARED)

# ===== SOURCE =====
option(BUILDING_SIMPLECAT_LIBRARY "BUILDING_SIMPLECAT_LIBRARY" ON)
include(${CMAKE_CURRENT_SOURCE_DIR}/SimplECAT.cmake)

SET(SIMPLECAT_SOURCE ${SIMPLECAT_DIR}/src/simplecat/Master.cpp)

# Create a library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SIMPLECAT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SIMPLECAT_DIR}/lib)
add_library(${PROJECT_NAME} ${LIB_TYPE} ${SIMPLECAT_SOURCE})

# Make sure the compiler can find include files for the SIMPLECAT library
# when other libraries or executables link to SIMPLECAT
target_include_directories(${PROJECT_NAME} PUBLIC ${SIMPLECAT_DIR}/src)

# ===== DEPENDENCIES =====

#SimplECAT
include_directories(   ${SIMPLECAT_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${SIMPLECAT_LIBRARIES} )
