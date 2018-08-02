/* Copyright (c) 1996-2017, OPC Foundation. All rights reserved.

   The source code in this file is covered under a dual-license scenario:
     - RCL: for OPC Foundation members in good-standing
     - GPL V2: everybody else

   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/

   GNU General Public License as published by the Free Software Foundation;
   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2

   This source code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _OPCUA_CONFIG_H_
#define _OPCUA_CONFIG_H_ 1

/*============================================================================
 * build information
 *===========================================================================*/
/** @brief The version number of the official OPC Foundation SDK this build is based on. */
#define OPCUA_BUILDINFO_VERSION                    "1.03.341.0"
#define OPCUA_BUILDINFO_VERSION_NO                  0x01030341

/*============================================================================
 * source configuration switches
 *===========================================================================*/
#define OPCUA_CONFIG_YES 1
#define OPCUA_CONFIG_NO  0

/*============================================================================
 * definition of flag indicating if openssl is available or not
 *===========================================================================*/
#define OPCUA_HAVE_OPENSSL                          OPCUA_CONFIG_NO

/*============================================================================
 * modules (removing unneeded modules reduces codesize)
 *===========================================================================*/
/** @brief Define or undefine to enable or disable client functionality */
#define OPCUA_HAVE_CLIENTAPI                        1
/** @brief Define or undefine to enable or disable server functionality */
#define OPCUA_HAVE_SERVERAPI                        1
/** @brief Define or undefine to enable or disable threadpool support. Required if secure listener shall use it. */
#define OPCUA_HAVE_THREADPOOL                       1
/** @brief define or undefine to enable or disable the memory stream module. */
#define OPCUA_HAVE_MEMORYSTREAM                     1
/** @brief define or undefine to enable or disable the soap and http support. */
/*#define OPCUA_HAVE_SOAPHTTP                         1*/
/** @brief define or undefine to enable or disable the https support. */
/*#define OPCUA_HAVE_HTTPS                            1*/


/* * @brief AUTOMATIC; activate additional modules required by soap/http */
#ifdef OPCUA_HAVE_SOAPHTTP
# define OPCUA_HAVE_HTTPAPI                         1
# define OPCUA_HAVE_XMLPARSER                       1
# define OPCUA_HAVE_BASE64                          1
# ifndef OPCUA_HAVE_MEMORYSTREAM
#  error SOAP/HTTP UA-SC transport profile requires memory stream!
# endif /* OPCUA_HAVE_MEMORYSTREAM */
#endif /* OPCUA_HAVE_SOAPHTTP */

/* * @brief AUTOMATIC; activate additional modules required by https */
#ifdef OPCUA_HAVE_HTTPS
# define OPCUA_HAVE_HTTPSAPI                        1
# define OPCUA_HAVE_BASE64                          1
#endif /* OPCUA_HAVE_HTTPS */

/*============================================================================
 * dotnet native stack wrapper requires this extension
 *===========================================================================*/
#define OPCUA_SUPPORT_PREENCODED_MESSAGES

/*============================================================================
 * general
 *===========================================================================*/
/** @brief Prefer the use of inline functions instead of function calls (see opcua_string) */
#define OPCUA_PREFERINLINE                          OPCUA_CONFIG_NO

/** @brief Enable the use of safe functions like defined with VS2005 and higher. */
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
#define OPCUA_USE_SAFE_FUNCTIONS                    OPCUA_CONFIG_YES
#else
#define OPCUA_USE_SAFE_FUNCTIONS                    OPCUA_CONFIG_NO
#endif

/** @brief Some temporary optimizations, to test their impact on performance. */
#define OPCUA_PERFORMANCE_OPTIMIZATION_TESTING      OPCUA_CONFIG_NO

/** @brief Ensure enum size of at least 32 bits (for backward compatibility). */
#define OPCUA_FORCE_INT32_ENUMS                     OPCUA_CONFIG_NO

/*============================================================================
 * threading
 *===========================================================================*/
/** @brief Run in multi thread mode. Each listen socket gets its own thread. */
#define OPCUA_MULTITHREADED                         OPCUA_CONFIG_YES

/** @brief Use access synchronization. Required for OPCUA_MULTITHREADED */
#define OPCUA_USE_SYNCHRONISATION                   OPCUA_CONFIG_YES

#if OPCUA_MULTITHREADED
#if !OPCUA_USE_SYNCHRONISATION
#error MT needs SYNCHRO!
#endif
#endif

/** @brief Using a special mutex struct with debug information. */
#define OPCUA_MUTEX_ERROR_CHECKING                  OPCUA_CONFIG_NO

/*============================================================================
 * timer
 *===========================================================================*/
/** @brief Maximum amount of milliseconds to stay inactive in the timer. */
#define OPCUA_TIMER_MAX_WAIT                        1000

/*============================================================================
 * serializer constraints
 *===========================================================================*/
/** @brief The maximum size of memory allocated by a serializer */
#define OPCUA_SERIALIZER_MAXALLOC 16777216

/** @brief Maximum String Length accepted */
#define OPCUA_ENCODER_MAXSTRINGLENGTH               ((OpcUa_UInt32)16777216)

/** @brief Maximum Array Length accepted */
#define OPCUA_ENCODER_MAXARRAYLENGTH                ((OpcUa_UInt32)65536)

/** @brief Maximum ByteString Length accepted */
#define OPCUA_ENCODER_MAXBYTESTRINGLENGTH           ((OpcUa_UInt32)16777216)

/** @brief Maximum Message Length accepted */
#define OPCUA_ENCODER_MAXMESSAGELENGTH              ((OpcUa_UInt32)16777216)

/** @brief Maximum Encodable object recursion depth */
#define OPCUA_ENCODER_MAXRECURSIONDEPTH             ((OpcUa_UInt32)100)

/*============================================================================
 * serializer checks
 *===========================================================================*/
/** @brief OpcUa_True or OpcUa_False; switches checks on or off; dont use with chunking enabled. */
#define OPCUA_SERIALIZER_CHECKLENGTHS               OpcUa_False

/*============================================================================
 * thread pool
 *===========================================================================*/
/** @brief Allow to dynamically create threads to prevent delay in queue if no static thread is free. Not recommended! */
#define OPCUA_THREADPOOL_EXPANSION                  OPCUA_CONFIG_NO

/** @brief Time in milliseconds after which a worker thread looks for further orders. Affects shutdown time. */
#define OPCUA_THREADPOOL_RELOOPTIME                 500

/*============================================================================
 * tracer
 *===========================================================================*/
/** @brief Enable output to trace device. */
#ifndef OPCUA_TRACE_ENABLE
#define OPCUA_TRACE_ENABLE                          OPCUA_CONFIG_NO
#endif

/** @brief Enable output to trace device. */
#define OPCUA_TRACE_MAXLENGTH                       200

/** @brief output the messages in errorhandling macros; requires OPCUA_ERRORHANDLING_OMIT_METHODNAME set to OPCUA_CONFIG_NO */
#define OPCUA_TRACE_ERROR_MACROS                    OPCUA_CONFIG_NO

/** @brief Omit the methodname in initialize status macro. */
#define OPCUA_ERRORHANDLING_OMIT_METHODNAME         OPCUA_CONFIG_NO

/** @brief Add __LINE__ and __FILE__ information to the trace line. */
#define OPCUA_TRACE_FILE_LINE_INFO                  OPCUA_CONFIG_NO

/*============================================================================
 * security
 *===========================================================================*/
/** @brief The maximum lifetime of a secure channel security token in milliseconds. */
#define OPCUA_SECURITYTOKEN_LIFETIME_MAX            3600000

/** @brief The minimum lifetime of a secure channel security token in milliseconds. */
#define OPCUA_SECURITYTOKEN_LIFETIME_MIN            600000

/** @brief The interval in which secure channels get checked for lifetime timeout in milliseconds. */
#define OPCUA_SECURELISTENER_WATCHDOG_INTERVAL      10000

/** @brief How many milliseconds a passive secure channel may wait for its activation. */
#define OPCUA_SECURELISTENER_CHANNELTIMEOUT         10000

/** @brief How many secure channels can be created, 0 means no explicit limit. */
#define OPCUA_SECURELISTENER_MAXCONNECTIONS         0

/** @brief How many request chunks are allowed in discovery only mode. */
#define OPCUA_SECURELISTENER_DISCOVERY_MAXCHUNKS    1

/** @brief Shall the FindServersOnNetwork request be allowed in discovery only mode. */
#define OPCUA_SECURELISTENER_DISCOVERY_ALLOW_FSON   OPCUA_CONFIG_NO

/** @brief Shall the secureconnection validate the server certificate given by the client application? */
#define OPCUA_SECURECONNECTION_VALIDATE_SERVERCERT  OPCUA_CONFIG_NO

/*============================================================================
 * networking
 *===========================================================================*/
/** @brief The standard port for the opc.tcp protocol, defined in part 6. */
#define OPCUA_TCP_DEFAULT_PORT                      4840

/** @brief The standard port for the http protocol. */
#define OPCUA_HTTP_DEFAULT_PORT                     80

/** @brief Request this buffersize for the sockets sendbuffer. */
#define OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE          OPCUA_CONFIG_YES
#define OPCUA_P_TCPRCVBUFFERSIZE                    65536
#define OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE          OPCUA_CONFIG_YES
#define OPCUA_P_TCPSNDBUFFERSIZE                    65536

/** @brief default buffer(chunk sizes) (also max value) */
#define OPCUA_TCPLISTENER_DEFAULTCHUNKSIZE          ((OpcUa_UInt32)65536)
#define OPCUA_TCPCONNECTION_DEFAULTCHUNKSIZE        ((OpcUa_UInt32)65536)

/** @brief if defined, the tcpstream expects the write call to block until all data is sent */
#define OPCUA_TCPSTREAM_BLOCKINGWRITE               OPCUA_CONFIG_NO

/** @brief The maximum number of client connections supported by a tcp listener. (maybe one reserved, see below) */
#ifndef OPCUA_TCPLISTENER_MAXCONNECTIONS
#define OPCUA_TCPLISTENER_MAXCONNECTIONS            100
#endif

/** @brief The default timeout for server sockets */
#define OPCUA_TCPLISTENER_TIMEOUT                   600000

/** @brief Reserve one of the OPCUA_TCPLISTENER_MAXCONNECTIONS for an "MaxConnectionsReached" error channel?. */
#define OPCUA_TCPLISTENER_USEEXTRAMAXCONNSOCKET     OPCUA_CONFIG_NO

/** @brief The maximum number of sockets supported by a socket manager. */
#ifndef OPCUA_P_SOCKETMANAGER_NUMBEROFSOCKETS
#define OPCUA_P_SOCKETMANAGER_NUMBEROFSOCKETS       110
#endif

/** @brief The maximum number of socket managers in multithreading config, supported by the socket module. */
#ifndef OPCUA_SOCKET_MAXMANAGERS
#define OPCUA_SOCKET_MAXMANAGERS                    110
#endif

/** @brief the time interval in msec at which the secureconnection checks for timeouts. */
#define OPCUA_SECURECONNECTION_TIMEOUTINTERVAL      1000

/** @brief Maximum number of pending messages before the server starts to block. */
#define OPCUA_SECURECONNECTION_MAXPENDINGMESSAGES   10

/*============================================================================
 * HTTPS protocol
 *===========================================================================*/
/** @brief Maximum number of receive buffers per message. */
#define OPCUA_HTTPS_MAX_RECV_BUFFER_COUNT           100

/** @brief Maximum size of a receive buffer. */
#define OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH          65536

/** @brief Calculated accepted (total) size limit of a received message. */
#define OPCUA_HTTPS_MAX_RECV_MESSAGE_LENGTH         (OPCUA_HTTPS_MAX_RECV_BUFFER_COUNT*OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH)

/** @brief Maximum length of a single HTTP header line. If a message exceeds this limit, 414 is returned. */
#define OPCUA_HTTPS_MAX_RECV_HEADER_LINE_LENGTH     1024

/** @brief Maximum accepted size of an HTTP header. If a message exceeds this limit, 414 is returned. */
#define OPCUA_HTTPS_MAX_RECV_HEADER_LENGTH          1024


/** @brief Maximum number of send buffers (result in HTTP chunks) per message. */
#define OPCUA_HTTPS_MAX_SEND_CHUNK_COUNT            50

/** @brief Maximum size of a send chunk (equals HTTP chunk size). */
#define OPCUA_HTTPS_MAX_SEND_CHUNK_LENGTH           262144

/** @brief Maximum size of outgoing HTTP header. (also includes chunk header) */
#define OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH          350

/** @brief Maximum size of outgoing HTTP footer. */
#define OPCUA_HTTPS_MAX_SEND_FOOTER_LENGTH          3

/** @brief Calculated size of a send buffer. */
#define OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH          (OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH + OPCUA_HTTPS_MAX_SEND_CHUNK_LENGTH + OPCUA_HTTPS_MAX_SEND_FOOTER_LENGTH)

/** @brief Calculated size of outgoing message. */
#define OPCUA_HTTPS_MAX_SEND_MESSAGE_LENGTH         (OPCUA_HTTPS_MAX_SEND_CHUNK_COUNT*OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH)

/** @brief the time interval in msec at which the https connection checks for timeouts. */
#define OPCUA_HTTPSCONNECTION_TIMEOUTINTERVAL       1000

/** @brief the maximum idle time on a keep-alive https connection. */
#define OPCUA_HTTPSCONNECTION_KEEP_ALIVE_TIMEOUT    60000

/** @brief Defines the number of maximum pending requests/connection to a server. */
#ifndef OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS
# define OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS  10
#endif /* OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS */

/** @brief Defines max number of sockets. (a single client needs one socket per parallel request!) */
#ifndef OPCUA_HTTPSLISTENER_MAXCONNECTIONS
# define OPCUA_HTTPSLISTENER_MAXCONNECTIONS         50
#endif /* OPCUA_HTTPSLISTENER_MAXCONNECTIONS */

/** @brief The standard port for the https protocol. */
#define OPCUA_HTTPS_DEFAULT_PORT                    443

/** @brief if defined, the httpsstream expects the write call to block until all data is sent */
#define OPCUA_HTTPSSTREAM_BLOCKINGWRITE             OPCUA_CONFIG_NO

/*============================================================================
 * type support
 *===========================================================================*/
/** @brief type exclusion configuration */
#include "opcua_exclusions.h"

#endif /* _OPCUA_CONFIG_H_ */
