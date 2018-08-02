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

/*==============================================================================*/
/* OpcUa_TcpListener_Connection                                                     */
/*==============================================================================*/
/**
* @brief Symbolizes a single client connection for the binary protocol layer.
*/
struct _OpcUa_TcpListener_Connection
{
    /** @brief The socket to the client. */
    OpcUa_Socket        Socket;
#if OPCUA_P_SOCKETGETPEERINFO_V2
    /** @brief Information about the peer. */
    OpcUa_CharA         achPeerInfo[OPCUA_P_PEERINFO_MIN_SIZE];
#else /* OPCUA_P_SOCKETGETPEERINFO_V2 */
    /** @brief The IP address of the client. */
    OpcUa_UInt32        PeerIp;
    /** @brief The port for the socket. */
    OpcUa_UInt16        PeerPort;
#endif /* OPCUA_P_SOCKETGETPEERINFO_V2 */
    /** @brief The time when the connection was made. */
    OpcUa_DateTime      ConnectTime;
    /** @brief The time when the client disconnected. */
    OpcUa_DateTime      DisconnectTime;
    /** @brief True, as long as the connection is established. */
    OpcUa_Boolean       bConnected;
    /** @brief The size of the incoming buffer. */
    OpcUa_UInt32        ReceiveBufferSize;
    /** @brief The size of the outgoing buffer. */
    OpcUa_UInt32        SendBufferSize;
    /** @brief Backlink to the listener which hosts the connection. */
    OpcUa_Void*         pListenerHandle;
    /** @brief Holds a reference to a not fully received stream message. */
    OpcUa_InputStream*  pInputStream;
    /** @brief Mutex for granting mutually exlcusive access to the connection object */
    OpcUa_Mutex         Mutex;
    /** @brief Number of request being issued over this connection. */
    OpcUa_UInt32        uNoOfRequestsTotal;
    /** @brief The maximum message size accepted by this connection. */
    OpcUa_UInt32        MaxMessageSize;
    /** @brief The maximum number of chunks per message accepted by this connection. */
    OpcUa_UInt32        MaxChunkCount;
    /** @brief The current number of chunks in a message. If 0, the connection is waiting for the next message. */
    OpcUa_UInt32        uCurrentChunk;
    /** @brief URL supplied by the client during transport handshake. */
    OpcUa_String        sURL;
    /** @brief The version of the binary protocol used over this connection. */
    OpcUa_UInt32        uProtocolVersion;
    /** @brief The queued list of data blocks to be sent. */
    OpcUa_BufferList*   pSendQueue;
    /** @brief Should this connection close when the send completes. */
    OpcUa_Boolean       bCloseWhenDone;
    /** @brief Should this connection block the receiver until the send completes. */
    OpcUa_Boolean       bNoRcvUntilDone;
    /** @brief Tells wether data has been delayed because of bNoRcvUntilDone. */
    OpcUa_Boolean       bRcvDataPending;
};

typedef struct _OpcUa_TcpListener_Connection OpcUa_TcpListener_Connection;

/** @brief Allocate and initialize a TcpListener_Connection */
OpcUa_StatusCode        OpcUa_TcpListener_Connection_Create(              OpcUa_TcpListener_Connection**   ppConnection);

/** @brief Initialize a TcpListener_Connection */
OpcUa_StatusCode        OpcUa_TcpListener_Connection_Initialize(          OpcUa_TcpListener_Connection*    pValue);

/** @brief Clear and free a TcpListener_Connection */
OpcUa_Void              OpcUa_TcpListener_Connection_Delete(              OpcUa_TcpListener_Connection**   pValue);

/** @brief Clear a TcpListener_Connection */
OpcUa_Void              OpcUa_TcpListener_Connection_Clear(               OpcUa_TcpListener_Connection*    pValue);

/*==============================================================================*/
/* OpcUa_TcpListener_ConnectionManager                                              */
/*==============================================================================*/
/**
* @brief Being part of a specific TcpListener, it manages the resources for all clients connected to an endpoint.
*/
struct _OpcUa_TcpListener_ConnectionManager
{
    /** @brief A list with current connections of type OpcUa_TcpListener_Connection */
    OpcUa_List*     Connections;
    /** @brief Backlink to the listener to which the connection manager belongs to. */
    OpcUa_Listener* Listener;
};

typedef struct _OpcUa_TcpListener_ConnectionManager OpcUa_TcpListener_ConnectionManager;

typedef OpcUa_Void (*OpcUa_TcpListener_ConnectionDeleteCB)(
    OpcUa_Listener*                pListener,
    OpcUa_TcpListener_Connection*  pTcpConnection);

/* @brief */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_Create(
    OpcUa_TcpListener_ConnectionManager**  ppConnectionManager);

/* @brief */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_Initialize(
    OpcUa_TcpListener_ConnectionManager*   ConnectionManager);

/* @brief */
OpcUa_Void              OpcUa_TcpListener_ConnectionManager_Clear(
    OpcUa_TcpListener_ConnectionManager*   ConnectionManager);

/* @brief */
OpcUa_Void              OpcUa_TcpListener_ConnectionManager_Delete(
    OpcUa_TcpListener_ConnectionManager**  ppConnectionManager);


/* @brief Add a new connection object to the list of managed connections. */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_AddConnection(
    OpcUa_TcpListener_ConnectionManager*    ConnectionManager,
    OpcUa_TcpListener_Connection*           Connection);

/* @brief Retrieve the connection object identified by the socket. */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_GetConnectionBySocket(
    OpcUa_TcpListener_ConnectionManager*    ConnectionManager,
    OpcUa_Socket                            Socket,
    OpcUa_TcpListener_Connection**          Connection);

/* @brief Remove a connection identified by the connection object itself (if no id was assigned ie. pre validation) */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_RemoveConnection(
    OpcUa_TcpListener_ConnectionManager*    ConnectionManager,
    OpcUa_TcpListener_Connection*           pConnection);

/* @brief Remove all connections managed by the listener and call the given function for everyone. */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_RemoveConnections(
    OpcUa_TcpListener_ConnectionManager*    ConnectionManager,
    OpcUa_TcpListener_ConnectionDeleteCB    ConnectionDeleteCB);

/* @brief . */
OpcUa_StatusCode        OpcUa_TcpListener_ConnectionManager_GetConnectionCount(
    OpcUa_TcpListener_ConnectionManager*    ConnectionManager,
    OpcUa_UInt32*                           pNoOfConnections);
