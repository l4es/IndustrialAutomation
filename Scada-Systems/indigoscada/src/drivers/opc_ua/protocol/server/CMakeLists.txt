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

if(WIN32)
    if(MSVC)
        if(not ${link_dynamic_msvcrt})
            foreach(flag_var
                    CMAKE_CXX_FLAGS 
                    CMAKE_C_FLAGS
                    CMAKE_CXX_FLAGS_DEBUG 
                    CMAKE_C_FLAGS_DEBUG
                    CMAKE_CXX_FLAGS_RELEASE 
                    CMAKE_C_FLAGS_RELEASE
                    CMAKE_CXX_FLAGS_MINSIZEREL 
                    CMAKE_C_FLAGS_MINSIZEREL
                    CMAKE_CXX_FLAGS_RELWITHDEBINFO 
                    CMAKE_C_FLAGS_RELWITHDEBINFO
                )
                if(${flag_var} MATCHES "/MD")
                    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
                endif(${flag_var} MATCHES "/MD")
            endforeach(flag_var)
            set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" 
                CACHE STRING "MSVC C Debug MT flags " FORCE)    
            set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" 
                CACHE STRING "MSVC CXX Debug MT flags " FORCE)
            set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" 
                CACHE STRING "MSVC C Release MT flags " FORCE)
            set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" 
                CACHE STRING "MSVC CXX Release MT flags " FORCE)
            set (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}" 
                CACHE STRING "MSVC C Debug MT flags " FORCE)    
            set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}" 
                CACHE STRING "MSVC C Release MT flags " FORCE)
            set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" 
                CACHE STRING "MSVC CXX Debug MT flags " FORCE)    
            set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" 
                CACHE STRING "MSVC CXX Release MT flags " FORCE)
        endif()
    endif()
endif()

    add_executable(AnsiCServer 
        ansicservermain.c
        browsenext.c
        browseservice.c
        init_variables_of_addressspace.c
        readservice.c
    )
    set_target_properties(AnsiCServer PROPERTIES FOLDER "AnsiCSample")
    target_link_libraries(AnsiCServer PUBLIC uastack)
