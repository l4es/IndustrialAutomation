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

#ifndef _OpcUa_ProxyStub_H_
#define _OpcUa_ProxyStub_H_ 1

#include "opcua_platformdefs.h"
#include "opcua_crypto.h"
#include "opcua_pki.h"
#include "opcua_p_interface.h"
#include "opcua_errorhandling.h"
#include "opcua_statuscodes.h"
#include "opcua_stackstatuscodes.h"

#include "opcua_identifiers.h"
#include "opcua_builtintypes.h"
#include "opcua_encodeableobject.h"
#include "opcua_types.h"
#include "opcua_browsenames.h"
#include "opcua_attributes.h"

OPCUA_BEGIN_EXTERN_C

/* TransportProfiles */
#define OpcUa_TransportProfile_UaTcp          "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary"
#define OpcUa_TransportProfile_SoapHttpBinary "http://opcfoundation.org/UA-Profile/Transport/http-uasc-uabinary"
#define OpcUa_TransportProfile_HttpsBinary    "http://opcfoundation.org/UA-Profile/Transport/https-uabinary"

/** Holds the runtime configuration values for the proxy stub modules.
    There may be some interference with the endpoint configuration at this time.
    Negative numeric values mean to use default values. */
typedef struct _OpcUa_ProxyStubConfiguration
{
    /** Globally enable/disable trace output from the stack (exclude platformlayer) */
    OpcUa_Boolean   bProxyStub_Trace_Enabled;
    /** Configure the level of messages traced. See config.h for values. */
    OpcUa_UInt32    uProxyStub_Trace_Level;

    /** Security constraints for the serializer. Set this values carefully. */
    /** The largest size for a memory block the serializer can do when deserializing a message. */
    OpcUa_Int32     iSerializer_MaxAlloc;
    /** The largest string accepted by the serializer. */
    OpcUa_Int32     iSerializer_MaxStringLength;
    /** The largest byte string accepted by the serializer. */
    OpcUa_Int32     iSerializer_MaxByteStringLength;
    /** Maximum number of elements in an array accepted by the serializer. */
    OpcUa_Int32     iSerializer_MaxArrayLength;
    /** The maximum number of bytes per message in total. */
    OpcUa_Int32     iSerializer_MaxMessageSize;
    /** The maximum encodable object recursion depth. */
    OpcUa_Int32     iSerializer_MaxRecursionDepth;

    /** Be careful! Enabling the threadpool has severe implications on the behavior of your server! */
    /** Controls wether the secure listener uses a thread pool to dispatch received requests. */
    OpcUa_Boolean   bSecureListener_ThreadPool_Enabled;
    /** The minimum number of threads in the thread pool. */
    OpcUa_Int32     iSecureListener_ThreadPool_MinThreads;
    /** The maximum number of threads in the thread pool */
    OpcUa_Int32     iSecureListener_ThreadPool_MaxThreads;
    /** The length of the queue with jobs waiting for a free thread. */
    OpcUa_Int32     iSecureListener_ThreadPool_MaxJobs;
    /** If MaxJobs is reached the add operation can block or return an error. */
    OpcUa_Boolean   bSecureListener_ThreadPool_BlockOnAdd;
    /** If the add operation blocks on a full job queue, this value sets the max waiting time. */
    OpcUa_UInt32    uSecureListener_ThreadPool_Timeout;

    /** If true, the TcpListener request a thread per client from the underlying socketmanager. Must not work with all platform layers. */
    OpcUa_Boolean   bTcpListener_ClientThreadsEnabled;
    /** The default and maximum size for message chunks in the server. Affects network performance and memory usage. */
    OpcUa_Int32     iTcpListener_DefaultChunkSize;

    /** The default (and requested) size for message chunks. Affects network performance and memory usage. */
    OpcUa_Int32     iTcpConnection_DefaultChunkSize;

    /** The default and maximum size for messages in the server. Affects memory usage. */
    OpcUa_Int32     iTcpTransport_MaxMessageLength;
    /** The default and maximum number of message chunks per message in the server. Affects memory usage. */
    OpcUa_Int32     iTcpTransport_MaxChunkCount;

    /** The network stream should block if not all could be send in one go. Be careful and use this only with client threads. Must not work with all platform layers. */
    OpcUa_Boolean   bTcpStream_ExpectWriteToBlock;
} OpcUa_ProxyStubConfiguration;

/*============================================================================
 * OpcUa_ProxyStub_Initialize
 *===========================================================================*/
/** Initialize proxy stub library. */
OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ProxyStub_Initialize( OpcUa_Handle                    pPortLayerHandle,
                                                                        OpcUa_ProxyStubConfiguration*   pProxyStubConfiguration);

/*============================================================================
 * OpcUa_ProxyStub_Initialize
 *===========================================================================*/
/** Set a new proxy stub configuration. Not thread-safe! */
OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ProxyStub_ReInitialize( OpcUa_ProxyStubConfiguration*   pProxyStubConfiguration);

/*============================================================================
 * OpcUa_ProxyStub_Clear
 *===========================================================================*/
/** Clean up proxy stub library. */
OPCUA_EXPORT OpcUa_Void OPCUA_DLLCALL OpcUa_ProxyStub_Clear();

/*============================================================================
 * OpcUa_ProxyStub_AddTypes
 *===========================================================================*/
/** Add additional types to the known types table.
  * @param ppTypes [in] Array of pointers to vendor types with OpcUa_Null as last element.
  */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_ProxyStub_AddTypes(OpcUa_EncodeableType** ppTypes);

/*============================================================================
 * OpcUa_ProxyStub_SetNamespaceUris
 *===========================================================================*/
/** Set namespace URI table.
  * @param a_psNamespaceUris [in] Array of pointers to namespace URIs with OpcUa_Null as last element.
  */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_ProxyStub_SetNamespaceUris(OpcUa_StringA* a_psNamespaceUris);

/*============================================================================
 * OpcUa_ProxyStub_GetVersion
 *===========================================================================*/
/** Request the version string of the proxy stub.
  * @return Pointer to a static buffer containing the version information in string format. Must not be freed!
  */
OPCUA_EXPORT OpcUa_StringA OPCUA_DLLCALL OpcUa_ProxyStub_GetVersion();

/*============================================================================
 * OpcUa_ProxyStub_GetConfigString
 *===========================================================================*/
/** Request the string encoded configuration table.
  * @return Pointer to a buffer containing the configuration string. Must not be freed!
  */
OPCUA_EXPORT OpcUa_StringA OPCUA_DLLCALL OpcUa_ProxyStub_GetConfigString();

/*============================================================================
 * OpcUa_ProxyStub_GetStaticConfigString
 *===========================================================================*/
/** Request the string encoded built configuration of the stack.
  * @return Pointer to a static string containing the options set by compiler switches. Must not be freed!
  */
OPCUA_EXPORT OpcUa_StringA OPCUA_DLLCALL OpcUa_ProxyStub_GetStaticConfigString();

OPCUA_END_EXTERN_C
#endif /* _OpcUa_ProxyStub_H_ */
