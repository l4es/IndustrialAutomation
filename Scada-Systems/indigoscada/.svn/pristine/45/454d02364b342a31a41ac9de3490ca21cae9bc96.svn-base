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

#ifndef _OpcUa_HttpsListener_ConnectionManager_H_
#define _OpcUa_HttpsListener_ConnectionManager_H_ 1

OPCUA_BEGIN_EXTERN_C

/*==============================================================================*/
/* OpcUa_HttpsListener_Connection                                                     */
/*==============================================================================*/
/**
* @brief Symbolizes a single client connection for the binary protocol layer.
*/
struct _OpcUa_HttpsListener_Connection
{
    /** @brief The socket to the client. */
    OpcUa_Socket                Socket;
    /** @brief Information about the peer. */
    OpcUa_CharA                 achPeerInfo[OPCUA_P_PEERINFO_MIN_SIZE];
    /** @brief The time when the connection was made. */
    OpcUa_UInt32                uConnectTime;
    /** @brief The time when the client disconnected. */
    OpcUa_UInt32                uDisconnectTime;
    /** @brief The last time data was received. */
    OpcUa_UInt32                uLastReceiveTime;
    /** @brief True, as long as the connection is established. */
    OpcUa_Boolean               bConnected;
    /** @brief True, if the client requested the connection to be kept alive. */
    OpcUa_Boolean               bKeepAlive;
    /** @brief Backlink to the listener which hosts the connection. */
    OpcUa_Void*                 pListenerHandle;
    /** @brief Holds a reference to a not fully received stream message. */
    OpcUa_InputStream*          pInputStream;
    /** @brief Holds a reference to a not fully sent stream message. */
    OpcUa_OutputStream*         pOutputStream;
    /** @brief Mutex for granting mutually exlcusive access to the connection object */
    OpcUa_Mutex                 Mutex;
    /** @brief Number of request being issued over this connection. */
    OpcUa_UInt32                uNoOfRequestsTotal;
    /** @brief Security Policy provided by the client. */
    OpcUa_String                sSecurityPolicy;
    /** @brief ValidationCallback has been received. */
    OpcUa_Boolean               bCallbackPending;
    /** @brief hResult from ValidationCallback. */
    OpcUa_StatusCode            hValidationResult;
    /** @brief ClientCertificate from ValidationCallback. */
    OpcUa_ByteString            bsClientCertificate;
    /** @brief Counts number of variables pointing to this object. */
    OpcUa_Int32                 iReferenceCount;
    /** @brief The queued list of data blocks to be sent. */
    OpcUa_BufferList*           pSendQueue;
};

typedef struct _OpcUa_HttpsListener_Connection OpcUa_HttpsListener_Connection;

/** @brief Allocate and initialize an HttpListener_Connection */
OpcUa_StatusCode        OpcUa_HttpsListener_Connection_Create(              OpcUa_HttpsListener_Connection**   ppConnection);

/** @brief Clear and free an HttpListener_Connection */
OpcUa_Void              OpcUa_HttpsListener_Connection_Delete(              OpcUa_HttpsListener_Connection**   pValue);

/*==============================================================================*/
/* OpcUa_HttpsListener_ConnectionManager                                              */
/*==============================================================================*/

typedef struct _OpcUa_HttpsListener_ConnectionManager OpcUa_HttpsListener_ConnectionManager;

/**
* @brief Being part of a specific HttpListener, it manages the resources for all clients connected to an endpoint.
*/
struct _OpcUa_HttpsListener_ConnectionManager
{
    /** @brief A list with current connections of type OpcUa_HttpsListener_Connection */
    OpcUa_List*                             Connections;
    /** @brief Backlink to the listener to which the connection manager belongs to. */
    OpcUa_Listener*                         Listener;
};

/* @brief Create a new connection manager object. */
OpcUa_StatusCode    OpcUa_HttpsListener_ConnectionManager_Create(
    OpcUa_HttpsListener_ConnectionManager** ppConnectionManager);

/* @brief Initialize a new connection manager object. */
OpcUa_StatusCode    OpcUa_HttpsListener_ConnectionManager_Initialize(
    OpcUa_HttpsListener_ConnectionManager*  ppConnectionManager);

/* @brief Clear the connection manager object. */
OpcUa_Void          OpcUa_HttpsListener_ConnectionManager_Clear(
    OpcUa_HttpsListener_ConnectionManager*  ConnectionManager);

/* @brief Delete the connection manager object. */
OpcUa_Void          OpcUa_HttpsListener_ConnectionManager_Delete(
    OpcUa_HttpsListener_ConnectionManager** ppConnectionManager);


/* @brief Add a new connection object to the list of managed connections. */
OpcUa_StatusCode    OpcUa_HttpsListener_ConnectionManager_AddConnection(
    OpcUa_HttpsListener_ConnectionManager*  ConnectionManager,
    OpcUa_HttpsListener_Connection*         Connection);

/* @brief Retrieve the connection object identified by the socket. */
OpcUa_StatusCode    OpcUa_HttpsListener_ConnectionManager_GetConnectionBySocket(
    OpcUa_HttpsListener_ConnectionManager*  ConnectionManager,
    OpcUa_Socket                            Socket,
    OpcUa_HttpsListener_Connection**        Connection);

/* @brief Release reference to given connection. Pointer gets nulled on return. */
OpcUa_StatusCode OpcUa_HttpsListener_ConnectionManager_ReleaseConnection(
    OpcUa_HttpsListener_ConnectionManager*    a_pConnectionManager,
    OpcUa_HttpsListener_Connection**          a_ppConnection);

/* @brief Get the current number of managed connections. */
OpcUa_StatusCode    OpcUa_HttpsListener_ConnectionManager_GetConnectionCount(
    OpcUa_HttpsListener_ConnectionManager*  ConnectionManager,
    OpcUa_UInt32*                           pNoOfConnections);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_HttpsListener_ConnectionManager_H_ */

