# specify whether building SimplECAT library, or linking to it.
option(BUILDING_SIMPLECAT_LIBRARY "BUILDING_SIMPLECAT_LIBRARY" OFF)

############### FIND AND LINK LIBRARIES ######################

# EtherLab
set(ETHERLAB_DIR /opt/etherlab)
find_library( ETHERCAT_LIBRARY NAMES libethercat.so PATHS ${ETHERLAB_DIR}/lib/ )

# SimplECAT
set(SIMPLECAT_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT BUILDING_SIMPLECAT_LIBRARY)
    find_library( SIMPLECAT_LIBRARY NAMES simplecat PATHS ${SIMPLECAT_DIR}/lib/ )
endif(NOT BUILDING_SIMPLECAT_LIBRARY)

# Define library variables
set(SIMPLECAT_INCLUDE_DIRS  ${SIMPLECAT_DIR}/src
                            ${ETHERLAB_DIR}/include
    )

set(SIMPLECAT_LIBRARIES     ${SIMPLECAT_LIBRARY}
                            ${ETHERCAT_LIBRARY}
    )
    


