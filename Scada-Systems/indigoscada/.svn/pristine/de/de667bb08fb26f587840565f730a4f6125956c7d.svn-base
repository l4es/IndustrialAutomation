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

#
# Hint at old root folder (see build_win32/build_win64.bat)
#
    set (OPENSSL_ROOT_DIR ${_PROJECT_ROOT}/openssl)
    include(FindOpenSSL)
if(NOT OPENSSL_FOUND)
    message(FATAL_ERROR, "No openssl found - please install")
endif()
    set(_uastack_src
        core/opcua_buffer.c
        core/opcua_core.c
        core/opcua_datetime.c
        core/opcua_guid.c
        core/opcua_list.c
        core/opcua_memory.c
        core/opcua_memorystream.c
        core/opcua_proxystub.c
        core/opcua_string.c
        core/opcua_thread.c
        core/opcua_threadpool.c
        core/opcua_timer.c
        core/opcua_trace.c
        core/opcua_utilities.c
        proxystub/clientproxy/opcua_asynccallstate.c
        proxystub/clientproxy/opcua_channel.c
        proxystub/clientproxy/opcua_clientapi.c
        proxystub/serverstub/opcua_endpoint.c
        proxystub/serverstub/opcua_serverapi.c
        proxystub/serverstub/opcua_servicetable.c
        securechannel/opcua_secureconnection.c
        securechannel/opcua_securelistener.c
        securechannel/opcua_securelistener_channelmanager.c
        securechannel/opcua_securelistener_policymanager.c
        securechannel/opcua_securestream.c
        securechannel/opcua_soapsecurechannel.c
        securechannel/opcua_tcpsecurechannel.c
        stackcore/opcua_binarydecoder.c
        stackcore/opcua_binaryencoder.c
        stackcore/opcua_builtintypes.c
        stackcore/opcua_connection.c
        stackcore/opcua_crypto.c
        stackcore/opcua_decoder.c
        stackcore/opcua_encodeableobject.c
        stackcore/opcua_encoder.c
        stackcore/opcua_enumeratedtype.c
        stackcore/opcua_extensionobject.c
        stackcore/opcua_listener.c
        stackcore/opcua_messagecontext.c
        stackcore/opcua_pki.c
        stackcore/opcua_securechannel.c
        stackcore/opcua_stream.c
        stackcore/opcua_stringtable.c
        stackcore/opcua_types.c
        transport/https/opcua_httpsconnection.c
        transport/https/opcua_httpslistener.c
        transport/https/opcua_httpslistener_connectionmanager.c
        transport/https/opcua_httpsstream.c
        transport/https/opcua_https_internal.c
        transport/tcp/opcua_tcpconnection.c
        transport/tcp/opcua_tcplistener.c
        transport/tcp/opcua_tcplistener_connectionmanager.c
        transport/tcp/opcua_tcpstream.c
    )

if(WIN32)
    set(_uastack_src ${_uastack_src}
        platforms/win32/opcua_p_binary.c
        platforms/win32/opcua_p_cryptofactory.c
        platforms/win32/opcua_p_datetime.c
        platforms/win32/opcua_p_guid.c
        platforms/win32/opcua_p_interface.c
        platforms/win32/opcua_p_internal.c
        platforms/win32/opcua_p_memory.c
        platforms/win32/opcua_p_mutex.c
        platforms/win32/opcua_p_openssl.c
        platforms/win32/opcua_p_openssl_aes.c
        platforms/win32/opcua_p_openssl_hmac_sha.c
        platforms/win32/opcua_p_openssl_nosecurity.c
        platforms/win32/opcua_p_openssl_pki.c
        platforms/win32/opcua_p_openssl_pki_nosecurity.c
        platforms/win32/opcua_p_openssl_random.c
        platforms/win32/opcua_p_openssl_rsa.c
        platforms/win32/opcua_p_openssl_sha.c
        platforms/win32/opcua_p_openssl_x509.c
        platforms/win32/opcua_p_pkifactory.c
        platforms/win32/opcua_p_semaphore.c
        platforms/win32/opcua_p_socket.c
        platforms/win32/opcua_p_socket_interface.c
        platforms/win32/opcua_p_socket_internal.c
        platforms/win32/opcua_p_socket_ssl.c
        platforms/win32/opcua_p_string.c
        platforms/win32/opcua_p_thread.c
        platforms/win32/opcua_p_timer.c
        platforms/win32/opcua_p_trace.c
        platforms/win32/opcua_p_utilities.c
        platforms/win32/opcua_p_win32_pki.c
    )
else()
    include(CheckIncludeFile)
    include(FindPkgConfig)
 
    set(_uastack_src ${_uastack_src}
        platforms/linux/opcua_p_binary.c
        platforms/linux/opcua_p_cryptofactory.c
        platforms/linux/opcua_p_datetime.c
        platforms/linux/opcua_p_guid.c
        platforms/linux/opcua_p_interface.c
        platforms/linux/opcua_p_internal.c
        platforms/linux/opcua_p_memory.c
        platforms/linux/opcua_p_mutex.c
        platforms/linux/opcua_p_openssl.c
        platforms/linux/opcua_p_openssl_aes.c
        platforms/linux/opcua_p_openssl_hmac_sha.c
        platforms/linux/opcua_p_openssl_nosecurity.c
        platforms/linux/opcua_p_openssl_pki.c
        platforms/linux/opcua_p_openssl_pki_nosecurity.c
        platforms/linux/opcua_p_openssl_random.c
        platforms/linux/opcua_p_openssl_rsa.c
        platforms/linux/opcua_p_openssl_sha.c
        platforms/linux/opcua_p_openssl_x509.c
        platforms/linux/opcua_p_pkifactory.c
        platforms/linux/opcua_p_semaphore.c
        platforms/linux/opcua_p_socket.c
        platforms/linux/opcua_p_socket_interface.c
        platforms/linux/opcua_p_socket_internal.c
        platforms/linux/opcua_p_socket_ssl.c
        platforms/linux/opcua_p_string.c
        platforms/linux/opcua_p_thread.c
        platforms/linux/opcua_p_timer.c
        platforms/linux/opcua_p_trace.c
        platforms/linux/opcua_p_utilities.c
    )

    set(CMAKE_THREAD_PREFER_PTHREAD 1)
    set(THREADS_PREFER_PTHREAD_FLAG 1)
    include(FindThreads)
endif()
    add_library(uastack STATIC ${_uastack_src})
    target_include_directories(uastack
        PUBLIC core
        PUBLIC proxystub/clientproxy
        PUBLIC proxystub/serverstub
        PUBLIC securechannel
        PUBLIC stackcore
        PUBLIC transport/https
        PUBLIC transport/tcp
        PUBLIC ${OPENSSL_INCLUDE_DIR}
    )
    option(trace_enable "set to OFF to disable stack tracing." ON)
if (trace_enable)
    target_compile_definitions(uastack PUBLIC OPCUA_TRACE_ENABLE)
endif()
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(uastack PUBLIC _DEBUG)
endif()
if(WIN32)
    target_include_directories(uastack PUBLIC platforms/win32)
    if(MSVC)
        target_compile_definitions(uastack
            PUBLIC _CRT_SECURE_NO_WARNINGS
            PUBLIC STRICT
        )
        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            target_compile_definitions(uastack PUBLIC _CRTDBG_MAP_ALLOC)
        else()
            target_compile_definitions(uastack PUBLIC NDEBUG)
        endif()
    endif()
    target_link_libraries(uastack
        PUBLIC ws2_32 
        PUBLIC crypt32 
        PUBLIC rpcrt4
    )
else()
    target_include_directories(uastack PUBLIC platforms/linux)
    target_compile_definitions(uastack PUBLIC _GNU_SOURCE)
    target_link_libraries(uastack PUBLIC pthread)
endif()
    target_link_libraries(uastack PUBLIC ${OPENSSL_LIBRARIES})

if(WIN32)
  # Place here the install rule for win32
elseif(LINUX)
    install (TARGETS uastack LIBRARY DESTINATION lib)
endif()
