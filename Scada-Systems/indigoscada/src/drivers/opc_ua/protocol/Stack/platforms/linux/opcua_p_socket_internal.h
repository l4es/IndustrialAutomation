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

#ifndef _OpcUa_Socket_Internal_H_
#define _OpcUa_Socket_Internal_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * The Socket Type
 *===========================================================================*/

/** The Socket Service Table.  */
typedef const struct _OpcUa_SocketServiceTable
{
   OpcUa_StatusCode    (* SocketRead)               ( OpcUa_Socket                hSocket,
                                                      OpcUa_Byte*                 pBuffer,
                                                      OpcUa_UInt32                BufferSize,
                                                      OpcUa_UInt32*               puintBytesRead);
   OpcUa_Int32         (* SocketWrite)              ( OpcUa_Socket                hSocket,
                                                      OpcUa_Byte*                 pBuffer,
                                                      OpcUa_UInt32                BufferSize,
                                                      OpcUa_Boolean               bBlock);
   OpcUa_StatusCode    (* SocketClose)              ( OpcUa_Socket                hSocket);
   OpcUa_StatusCode    (* SocketGetPeerInfo)        ( OpcUa_Socket                hSocket,
                                                      OpcUa_CharA*                achPeerInfoBuffer,
                                                      OpcUa_UInt32                uiPeerInfoBufferSize);
   OpcUa_StatusCode    (* SocketGetLastError)       ( OpcUa_Socket                hSocket);
   OpcUa_StatusCode    (* SocketSetUserData)        ( OpcUa_Socket                hSocket,
                                                      OpcUa_Void*                 pvUserData);
} OpcUa_SocketServiceTable;

/* forward definition of the OpcUa_Socket structure */
typedef struct _OpcUa_InternalSocket OpcUa_InternalSocket;

/* forward definition of the OpcUa_SocketManager structure */
typedef struct _OpcUa_InternalSocketManager OpcUa_InternalSocketManager;

/**
* Internal representation for a logical socket (client and server). Includes
* beside the system socket additional information for handling.
*/
struct _OpcUa_InternalSocket
{
    OpcUa_SocketServiceTable*    pSocketServiceTable;/* socket service table */
    OpcUa_RawSocket              rawSocket;          /* system socket */
    OpcUa_Socket_EventCallback   pfnEventCallback;   /* function to call on event */
    OpcUa_Void*                  pvUserData;         /* data for callback */
    OpcUa_InternalSocketManager* pSocketManager;     /* the socket manager, this socket belongs to */
    OpcUa_UInt16                 usPort;             /* the socket port of this socket */
    volatile OpcUa_Boolean       bInvalidSocket;     /* is the socket usable */
    volatile OpcUa_Boolean       bSocketIsInUse;     /* true if this list member is currently connected or listening */
    struct _Flags
    {
        OpcUa_UInt               EventMask:11;       /* mask and unmask eventhandling */
        OpcUa_UInt               bClosedSocket:1;    /* is the socket closed */
        OpcUa_UInt               bOwnThread:1;       /* if this socket is handled by an own thread */
        OpcUa_UInt               bFromApplication:1; /* Application is explicitely waiting for an event on this socket. */
    } Flags;
    OpcUa_UInt32                 uintTimeout;        /* interval until connection is considered timed out */
    OpcUa_UInt32                 uintLastAccess;     /* system tick count in seconds when last action on this socket took place */
};

/**
* List of sockets for one listening socket (included).
*/
struct _OpcUa_InternalSocketManager
{
    OpcUa_InternalSocket*   pSockets;                 /* the sockets */
    OpcUa_UInt32            uintMaxSockets;           /* how many socket entries can this list hold at maximum. Mind the signal socket!  */
    OpcUa_RawSocket         pCookie;                  /* wakeup event socket */
    OpcUa_UInt32            uintLastExternalEvent;    /* the last occurred event */
#if OPCUA_MULTITHREADED
    OpcUa_InternalSocketManager** pSocketManagers;    /* the spawned socket managers go there */
    OpcUa_RawThread         pSpawnedThread;           /* the spawned accept thread */
    OpcUa_Semaphore         pStartupSemaphore;        /* wait on this semaphore to synchronize the accept thread */
    OpcUa_RawThread         pThreadToJoin;            /* the next thread to be joined */
    OpcUa_RawThread         pThread;                  /* each socket list has its own thread... */
#endif /* OPCUA_MULTITHREADED */
#if OPCUA_USE_SYNCHRONISATION
    OpcUa_Mutex             pMutex;                   /* ... and therefore its own mutex! */
#endif /* OPCUA_USE_SYNCHRONISATION */
    struct _SocketManagerFlags
    {
        OpcUa_UInt  bSpawnThreadOnAccept:1;           /* is a new thread spawned on a new connection accept? */
        OpcUa_UInt  bRejectOnThreadFail :1;           /* reject an accept when there is no free thread? */
        OpcUa_UInt  bDontCloseOnExcept  :1;           /* override default closing of a socket on except event */
    } Flags;
};

/*
* Sets a socket to invalid.
*/
#if OPCUA_USE_SYNCHRONISATION
#define OPCUA_SOCKET_INVALIDATE(a)      do {                                                    \
                                             OpcUa_P_Mutex_Lock((a)->pSocketManager->pMutex);   \
                                             (a)->bSocketIsInUse = OpcUa_False;                 \
                                             OpcUa_P_Mutex_Unlock((a)->pSocketManager->pMutex); \
                                        } while(0)
#define OPCUA_SOCKET_SETVALID(a)        do {                                                    \
                                             OpcUa_P_Mutex_Lock((a)->pSocketManager->pMutex);   \
                                             (a)->bInvalidSocket = OpcUa_False;                 \
                                             OpcUa_P_Mutex_Unlock((a)->pSocketManager->pMutex); \
                                        } while(0)
#else
#define OPCUA_SOCKET_INVALIDATE(a)      ((a)->bSocketIsInUse = OpcUa_False)
#define OPCUA_SOCKET_SETVALID(a)        ((a)->bInvalidSocket = OpcUa_False)
#endif /* OPCUA_USE_SYNCHRONISATION */


/*============================================================================
 * Initialize Socket Type
 *===========================================================================*/
OpcUa_Void          OpcUa_Socket_Initialize(    OpcUa_Socket pSocket);

/*============================================================================
 * Clear Socket Type
 *===========================================================================*/
OpcUa_Void          OpcUa_Socket_Clear(         OpcUa_Socket pSocket);


/**************************** The SocketManager Type ****************************/

/*============================================================================
 * Allocate SocketManager Type
 *===========================================================================*/
OpcUa_SocketManager OpcUa_SocketManager_Alloc(OpcUa_Void);

/*============================================================================
 * Initialize SocketManager Type
 *===========================================================================*/
OpcUa_Void          OpcUa_SocketManager_Initialize(OpcUa_SocketManager pSocketManager);



/*============================================================================
 * Create the Sockets in the given list
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_SocketManager_CreateSockets(  OpcUa_SocketManager     pSocketManager,
                                                        OpcUa_UInt32            uintMaxSockets);

/*============================================================================
 * Set the event mask for this socket.
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_Socket_SetEventMask(  OpcUa_Socket                pSocket,
                                                OpcUa_UInt32                uintEventMask);

/*============================================================================
 * Get the currently set event mask for this socket.
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_Socket_GetEventMask(  OpcUa_Socket                pSocket,
                                                OpcUa_UInt32*               puintEventMask);

/*============================================================================
 * Network Byte Order Conversion Helper Functions
 *===========================================================================*/
OpcUa_UInt32        OpcUa_Socket_NToHL(         OpcUa_UInt32 netLong);
OpcUa_UInt16        OpcUa_Socket_NToHS(         OpcUa_UInt16 netShort);

OpcUa_UInt32        OpcUa_Socket_HToNL(         OpcUa_UInt32 hstLong);
OpcUa_UInt16        OpcUa_Socket_HToNS(         OpcUa_UInt16 hstShort);


/*============================================================================
 * Find a free socket.
 *===========================================================================*/
OpcUa_Socket        OpcUa_SocketManager_FindFreeSocket( OpcUa_SocketManager pSocketManager,
                                                        OpcUa_Boolean       bIsSignalSocket);

/*============================================================================
 * Take action based on socket and event.
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_Socket_HandleEvent(   OpcUa_Socket        pSocket,
                                                OpcUa_UInt32        uintEvent);

/*!
 * @brief Fill the socket array with sockets from the given socket list and selected based on the given event.
 *
 * @param pSocketManager   [in]    The source of the sockets.
 * @param pSocketArray  [out]   The sockets in this array get set based on the socket list.
 * @param Event         [in]    Only set sockets with this event set.
 * @param max           [in]    previously used max file descriptor
 *
 * @return new max file descriptor used
 */
int OpcUa_P_Socket_FillFdSet(OpcUa_SocketManager   SocketManager,
                             OpcUa_P_Socket_Array* pSocketArray,
                             OpcUa_UInt32          Event,
                             int                   max);

/*!
 * @brief Handle all signaled events in the socket array.
 *
 * @param pSocketManager   [in]    The list with the OpcUa_Sockets which store the handler routines.
 * @param pSocketArray  [in]    The array with the system sockets to be checked.
 * @param Event         [in]    Handle all sockets waiting for this event.
 */
OpcUa_Void OpcUa_P_Socket_HandleFdSet(OpcUa_SocketManager   SocketManager,
                                      OpcUa_P_Socket_Array* SocketArray,
                                      OpcUa_UInt32          Event);

/*!
 * @brief Handle an externally triggered event.
 *
 * @param pSocketManager   [in]    The current socket list.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_Socket_HandleExternalEvent(OpcUa_SocketManager SocketManager);

/*!
 * @brief Create and initialize a listening OpcUa_Socket.
 *
 * @param Port      [in]    The port to listen on.
 * @param Status    [out]   How the operation went.
 *
 * @return The created system socket. An invalid socket in case of error.
 */
OpcUa_RawSocket OpcUa_P_Socket_CreateServer(OpcUa_StringA     IpAddress,
                                            OpcUa_Int16       Port,
                                            OpcUa_StatusCode* Status);

/*!
 * @brief Create a OpcUa_Socket and connect to specified network node.
 *
 * @param Port          [in]    Non zero to bind the socket locally.
 * @param RemotePort    [in]    The port on the server side.
 * @param RemoteAdress  [in]    The IP address of the server as string (ascii).
 * @param Status        [out]   Status how the operation finished.
 *
 * @return The created system socket. An invalid socket in case of error.
 */
OpcUa_RawSocket OpcUa_P_Socket_CreateClient(OpcUa_UInt16                    Port,
                                            OpcUa_UInt16                    RemotePort,
                                            OpcUa_StringA                   RemoteAddress,
                                            OpcUa_StatusCode*               Status);

/*!
 * @brief Check the socket list for events and handle them.
 *
 * @param pSocketManager [in]    The socket list holding the sockets for the select call.
 * @param msecTimeout    [in]    The maximum number of milliseconds, this function blocks the calling thread.
 * @param bRunOnce       [in]    Run the event loop only once.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_SocketManager_ServeLoopInternal(   OpcUa_SocketManager   SocketManager,
                                                            OpcUa_UInt32          msecTimeout,
                                                            OpcUa_Boolean         bRunOnce);

/*============================================================================
 * Network Byte Order Conversion Helper Functions
 *===========================================================================*/
OpcUa_UInt32 OpcUa_Socket_NToHL(OpcUa_UInt32 a_netLong);

OpcUa_UInt16 OpcUa_Socket_NToHS(OpcUa_UInt16 a_netShort);

OpcUa_UInt32 OpcUa_Socket_HToNL(OpcUa_UInt32 a_hstLong);

OpcUa_UInt16 OpcUa_Socket_HToNS(OpcUa_UInt16 a_hstShort);

/*============================================================================
 * Set socket to nonblocking mode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SocketManager_InternalCreateServer(  OpcUa_SocketManager         a_pSocketManager,
                                                            OpcUa_StringA               a_sIpAddress,
                                                            OpcUa_UInt16                a_uPort,
                                                            OpcUa_Socket_EventCallback  a_pfnSocketCallBack,
                                                            OpcUa_Void*                 a_pCallbackData,
                                                            OpcUa_Socket*               a_ppSocket);


OPCUA_END_EXTERN_C

#endif /* _OpcUa_Socket_Internal_H_ */

