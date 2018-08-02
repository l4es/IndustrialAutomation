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

/* System Headers */
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

/* additional UA dependencies */
#include <opcua_datetime.h>

/* platform layer includes */
#include <opcua_p_thread.h>
#include <opcua_p_mutex.h>
#include <opcua_p_semaphore.h>
#include <opcua_p_utilities.h>

/* own headers */
#include <opcua_p_socket.h>
#include <opcua_p_memory.h>
#include <opcua_p_socket_internal.h>
#include <opcua_p_socket_interface.h>

/* platform layer includes */
#include <opcua_p_timer.h> /* for timered select */


/*============================================================================
 * Read Socket.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_Read( OpcUa_Socket    a_pSocket,
                                                    OpcUa_Byte*     a_pBuffer,
                                                    OpcUa_UInt32    a_nBufferSize,
                                                    OpcUa_UInt32*   a_pBytesRead)
{
    OpcUa_Int32 result = 0;
    OpcUa_InternalSocket*   pInternalSocket     = (OpcUa_InternalSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "Read");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(a_pBuffer);
    OpcUa_GotoErrorIfArgumentNull(a_pBytesRead);

    if(!(pInternalSocket->Flags.EventMask & OPCUA_SOCKET_READ_EVENT))
    {
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Lock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
        pInternalSocket->Flags.EventMask |= OPCUA_SOCKET_READ_EVENT;
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
    }

    /* returns the number of bytes received or 0 or a negative value in case of disconnect or error. */
    result = OpcUa_P_RawSocket_Read(    pInternalSocket->rawSocket,
                                        a_pBuffer,
                                        a_nBufferSize);

    /* decide if the result is an error or the number of bytes read. */
    if(result > 0)
    {
        *a_pBytesRead = (OpcUa_UInt32)result;
    }
    else /* result <=0 (error or disconnect) */
    {
        *a_pBytesRead = 0;

        if(result == 0)
        {
            uStatus = OpcUa_BadDisconnect;
        }
        else
        {
            OpcUa_Int32 uLastError = OpcUa_P_RawSocket_GetLastError(pInternalSocket->rawSocket);

            switch(uLastError)
            {
            case EWOULDBLOCK:
                {
                    /* this needs to be set specific because some */
                    /* callers might want to ignore this error    */
                    uStatus = OpcUa_BadWouldBlock;
                    break;
                }
            case ECONNABORTED:
            case ECONNRESET:
                {
                    uStatus = OpcUa_BadDisconnect;
                    break;
                }
            default:
                {
                    uStatus = OpcUa_BadCommunicationError;
                    break;
                }
            }
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Write Socket.
 *===========================================================================*/
/* returns number of bytes written to the socket */
static OpcUa_Int32 OpcUa_P_SocketService_Write( OpcUa_Socket    a_pSocket,
                                                OpcUa_Byte*     a_pBuffer,
                                                OpcUa_UInt32    a_uBufferSize,
                                                OpcUa_Boolean   a_bBlock)
{
    OpcUa_Int32             result;
    OpcUa_Int32             intError;
    OpcUa_Byte*             pBuffer             = a_pBuffer;
    OpcUa_UInt32            RemainingBufferSize = a_uBufferSize;
    OpcUa_InternalSocket*   pInternalSocket     = (OpcUa_InternalSocket*)a_pSocket;
    OpcUa_RawSocket         hRawSocket;

    /* check for errors */
    OpcUa_ReturnErrorIfNull(a_pSocket, OPCUA_SOCKET_ERROR);
    OpcUa_ReturnErrorIfNull(a_pBuffer, OPCUA_SOCKET_ERROR);

    if(a_bBlock != OpcUa_False)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_P_Socket_Write: Blocking write not supported.\n");
        return OPCUA_SOCKET_ERROR;
    }

    if(a_uBufferSize == 0)
    {
        return OPCUA_SOCKET_ERROR;
    }

    if(     pInternalSocket->bSocketIsInUse == OpcUa_False
        ||  pInternalSocket->bInvalidSocket != OpcUa_False)
    {
        return OPCUA_SOCKET_ERROR;
    }

    hRawSocket = pInternalSocket->rawSocket;

    /* send loop */
    do /* while no error and not all send */
    {
        /* send data */
        result = OpcUa_P_RawSocket_Write(   hRawSocket,
                                            pBuffer,
                                            RemainingBufferSize);

        /* special treatment for wouldblock */
        if(result == OPCUA_P_SOCKET_SOCKETERROR)
        {
            intError = OpcUa_P_RawSocket_GetLastError(hRawSocket);

            if(intError != EWOULDBLOCK)
            {
                /* error, but no wouldblock */
                return OPCUA_SOCKET_ERROR;
            }
        }
        else if (result == 0)
        {
            return OPCUA_SOCKET_ERROR; /* closed socket? */
        }
        else /* no error */
        {
            /* update data size */
            RemainingBufferSize = RemainingBufferSize - result;

            /* move data pointer */
            pBuffer = pBuffer + result;
        }
    }
    while((RemainingBufferSize > 0) && (result > 0)); /* loop until all data sent or error occurred */

    /* update size before returning */
    result = a_uBufferSize - RemainingBufferSize;

    /* give the application a callback as soon as more tcp bytes can be sent */
    if(     (RemainingBufferSize > 0)
        && !(pInternalSocket->Flags.EventMask & OPCUA_SOCKET_WRITE_EVENT))
    {
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Lock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
        pInternalSocket->Flags.EventMask |= OPCUA_SOCKET_WRITE_EVENT;
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
#if OPCUA_MULTITHREADED
        if(pInternalSocket->Flags.bFromApplication == OpcUa_False)
        {
            OpcUa_P_SocketManager_InterruptLoop(    pInternalSocket->pSocketManager,
                                                    OPCUA_SOCKET_RENEWLOOP_EVENT,
                                                    OpcUa_False);
        }
#endif /* OPCUA_MULTITHREADED */
    }

    return result;
}

/*============================================================================
 * Close Socket.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_Close(OpcUa_Socket a_pSocket)
{
    OpcUa_InternalSocket* pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "Close");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    if(     pInternalSocket->bSocketIsInUse == OpcUa_False
        ||  pInternalSocket->bInvalidSocket != OpcUa_False)
    {
        return OpcUa_Bad;
    }

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(pInternalSocket->Flags.bClosedSocket != OpcUa_False)
    {
        /* caller tried to close an invalid socket, what may happen intentionally... */
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
        return OpcUa_Bad;
    }

    pInternalSocket->Flags.bClosedSocket = OpcUa_True;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_P_Socket_Close: Shutting down socket %p!\n", a_pSocket);
    uStatus = OpcUa_P_RawSocket_Shutdown(pInternalSocket->rawSocket, OPCUA_P_SOCKET_SD_BOTH);

#if OPCUA_MULTITHREADED
    /* the if this is a client connection in a own thread, the loop should be notified to shut down */
    if(pInternalSocket->Flags.bOwnThread != 0)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_P_Socket_Close: Notifying SocketManager to shut down ... \n");
        OpcUa_P_SocketManager_InterruptLoop(    pInternalSocket->pSocketManager,
                                                OPCUA_SOCKET_SHUTDOWN_EVENT,
                                                OpcUa_False);
    }
#endif /* OPCUA_MULTITHREADED */

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get IP Address and Port Number of the Peer
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_GetPeerInfo(  OpcUa_Socket  a_pSocket,
                                                            OpcUa_CharA*  a_achPeerInfoBuffer,
                                                            OpcUa_UInt32  a_uiPeerInfoBufferSize)
{
    OpcUa_InternalSocket* pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "GetPeerInfo");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(a_achPeerInfoBuffer);

    a_achPeerInfoBuffer[0] = 0;
    uStatus = OpcUa_P_RawSocket_GetPeerInfo((OpcUa_RawSocket)pInternalSocket->rawSocket, a_achPeerInfoBuffer, a_uiPeerInfoBufferSize);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get last socket error
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_GetLastError(OpcUa_Socket a_pSocket)
{
    OpcUa_InternalSocket*   pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;
    OpcUa_Int32             iLastError;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "GetLastError");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfTrue(pInternalSocket->rawSocket == OPCUA_P_SOCKET_INVALID,
                          OpcUa_BadCommunicationError);

    iLastError = OpcUa_P_RawSocket_GetLastError(pInternalSocket->rawSocket);

    if(iLastError != 0)
    {
        /* TODO: Map errorcodes. */
        switch(iLastError)
        {
        case EWOULDBLOCK:
        case EINPROGRESS:
            {
                uStatus = OpcUa_BadWouldBlock;
                break;
            }
        case ECONNABORTED:
        case ECONNRESET:
            {
                uStatus = OpcUa_BadDisconnect;
                break;
            }
        default:
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_P_Socket_GetLastError: Error 0x%08X\n", iLastError);
                uStatus = OpcUa_BadCommunicationError;
            }
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Set socket user data
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_SetUserData(OpcUa_Socket a_pSocket,
                                                          OpcUa_Void*  a_pvUserData)
{
    OpcUa_InternalSocket*   pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SetUserData");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    pInternalSocket->pvUserData = a_pvUserData;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Initialize Socket Service Table
 *===========================================================================*/
static OpcUa_SocketServiceTable OpcUa_RawSocketServiceTable =
{
  OpcUa_P_SocketService_Read,
  OpcUa_P_SocketService_Write,
  OpcUa_P_SocketService_Close,
  OpcUa_P_SocketService_GetPeerInfo,
  OpcUa_P_SocketService_GetLastError,
  OpcUa_P_SocketService_SetUserData
};

/*============================================================================
 * Initialize Socket Type
 *===========================================================================*/
OpcUa_Void OpcUa_Socket_Initialize(OpcUa_Socket a_pSocket)
{
    OpcUa_InternalSocket* pInternalSocket = OpcUa_Null;

    if(a_pSocket == OpcUa_Null)
    {
        return;
    }

    pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

    pInternalSocket->pSocketServiceTable = &OpcUa_RawSocketServiceTable;

    pInternalSocket->bSocketIsInUse = OpcUa_False;

    pInternalSocket->rawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
}

/*============================================================================
 * Clear Socket Type
 *===========================================================================*/
OpcUa_Void OpcUa_Socket_Clear(OpcUa_Socket a_pSocket)
{
    OpcUa_InternalSocket* pInternalSocket = OpcUa_Null;

    if(a_pSocket == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Socket_Clear: Invalid handle!\n");
        return;
    }

    pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

    pInternalSocket->bSocketIsInUse = OpcUa_False;

    pInternalSocket->rawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
}

/*============================================================================
 * Allocate SocketManager Type
 *===========================================================================*/
OpcUa_SocketManager OpcUa_SocketManager_Alloc(OpcUa_Void)
{
    OpcUa_InternalSocketManager*    pInternalSocketManager  = OpcUa_Null;

    pInternalSocketManager = (OpcUa_InternalSocketManager*)OpcUa_P_Memory_Alloc(sizeof(OpcUa_InternalSocketManager));

    if(pInternalSocketManager == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    return (OpcUa_SocketManager)pInternalSocketManager;
}

/*============================================================================
 * Initialize SocketManager Type
 *===========================================================================*/
OpcUa_Void OpcUa_SocketManager_Initialize(OpcUa_SocketManager a_pSocketManager)
{
    OpcUa_InternalSocketManager* pInternalSocketManager = OpcUa_Null;

    if(a_pSocketManager == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemSet(a_pSocketManager, 0, sizeof(OpcUa_InternalSocketManager));

    pInternalSocketManager = (OpcUa_InternalSocketManager*)a_pSocketManager;

    pInternalSocketManager->pSockets                = OpcUa_Null;
    pInternalSocketManager->uintMaxSockets          = 0;
    pInternalSocketManager->pCookie                 = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    pInternalSocketManager->uintLastExternalEvent   = OPCUA_SOCKET_NO_EVENT;
}

/*============================================================================
 * Create the Sockets in the List
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SocketManager_CreateSockets(
    OpcUa_SocketManager a_pSocketManager,
    OpcUa_UInt32        a_uMaxSockets)
{
    OpcUa_UInt32                 ntemp                  = 0;
    OpcUa_InternalSocketManager* pInternalSocketManager = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "CreateSockets");

    OpcUa_GotoErrorIfArgumentNull(a_pSocketManager);

    pInternalSocketManager = (OpcUa_InternalSocketManager*)a_pSocketManager;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    pInternalSocketManager->pSockets = (OpcUa_InternalSocket *)OpcUa_P_Memory_Alloc(sizeof(OpcUa_InternalSocket) * a_uMaxSockets);
    OpcUa_GotoErrorIfAllocFailed(pInternalSocketManager->pSockets);

    /* initialize the whole socket list with zero */
    OpcUa_MemSet(pInternalSocketManager->pSockets, 0, sizeof(OpcUa_InternalSocket) * a_uMaxSockets);

    for(ntemp = 0; ntemp < a_uMaxSockets; ntemp++)
    {
        OpcUa_Socket_Initialize(&(pInternalSocketManager->pSockets[ntemp]));
    }

    pInternalSocketManager->uintMaxSockets = a_uMaxSockets;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/************************* Internal Helper Functions *************************/

/*============================================================================
 * Internal helper function to create a server socket
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SocketManager_InternalCreateServer(
    OpcUa_SocketManager         a_pSocketManager,
    OpcUa_StringA               a_sIpAddress,
    OpcUa_UInt16                a_uPort,
    OpcUa_Socket_EventCallback  a_pfnSocketCallBack,
    OpcUa_Void*                 a_pCallbackData,
    OpcUa_Socket*               a_ppSocket)
{
    OpcUa_StatusCode        uStatus         = OpcUa_Good;
    OpcUa_InternalSocket*   pInternalSocket = OpcUa_Null;

    *a_ppSocket = OpcUa_Null;

    /* create the main server socket and raise error if no socket is found */
    pInternalSocket = (OpcUa_InternalSocket*)OpcUa_SocketManager_FindFreeSocket(a_pSocketManager,
                                                                                OpcUa_False);
    /* no free sockets, out of resources.. */
    OpcUa_ReturnErrorIfNull(pInternalSocket, OpcUa_BadMaxConnectionsReached);

    pInternalSocket->rawSocket = OpcUa_P_Socket_CreateServer(a_sIpAddress, a_uPort, &uStatus);

    if(OpcUa_IsBad(uStatus))
    {
        OPCUA_SOCKET_INVALIDATE(pInternalSocket);
        return OpcUa_BadCommunicationError;
    }

    pInternalSocket->pfnEventCallback       = a_pfnSocketCallBack;
    pInternalSocket->pvUserData             = a_pCallbackData;
    pInternalSocket->Flags.bOwnThread       = OpcUa_False;
    pInternalSocket->Flags.EventMask        = OPCUA_SOCKET_READ_EVENT | OPCUA_SOCKET_EXCEPT_EVENT | OPCUA_SOCKET_ACCEPT_EVENT | OPCUA_SOCKET_CLOSE_EVENT | OPCUA_SOCKET_TIMEOUT_EVENT;
    pInternalSocket->usPort                 = a_uPort;

    OPCUA_SOCKET_SETVALID(pInternalSocket);

    *a_ppSocket = pInternalSocket;

    return uStatus;
}

/*============================================================================
*
*===========================================================================*/
OpcUa_StatusCode OpcUa_Socket_HandleAcceptEvent(    OpcUa_Socket a_pListenSocket,
                                                    OpcUa_Socket a_pAcceptedSocket) /* this is allowed to be null */
{
    OpcUa_RawSocket         AcceptedRawSocket       = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    OpcUa_InternalSocket*   pListenInternalSocket   = (OpcUa_InternalSocket*)a_pListenSocket;
    OpcUa_InternalSocket*   pAcceptInternalSocket   = (OpcUa_InternalSocket*)a_pAcceptedSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pListenSocket);

    AcceptedRawSocket = OpcUa_P_RawSocket_Accept(   pListenInternalSocket->rawSocket,
                                                    OpcUa_True,
                                                    OpcUa_False);

    /* accept but close if caller provided a null argument */
    if(a_pAcceptedSocket == OpcUa_Null)
    {
        if(AcceptedRawSocket != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
        {
            OpcUa_P_RawSocket_Close(AcceptedRawSocket);
        }

        return OpcUa_BadMaxConnectionsReached;
    }
    else
    {
        pAcceptInternalSocket->rawSocket = AcceptedRawSocket;
    }

    if( pAcceptInternalSocket->rawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        OPCUA_SOCKET_INVALIDATE(pAcceptInternalSocket);
        return OpcUa_BadCommunicationError;
    }

    OpcUa_P_RawSocket_SetBlockMode ( pAcceptInternalSocket->rawSocket,
                                     OpcUa_False);

    /* inherit from parent (listen) socket */
    pAcceptInternalSocket->pfnEventCallback       = pListenInternalSocket->pfnEventCallback;
    pAcceptInternalSocket->pvUserData             = pListenInternalSocket->pvUserData;
    pAcceptInternalSocket->Flags.bOwnThread       = OpcUa_False;
    pAcceptInternalSocket->Flags.EventMask        =   OPCUA_SOCKET_READ_EVENT
                                                    | OPCUA_SOCKET_EXCEPT_EVENT
                                                    | OPCUA_SOCKET_CLOSE_EVENT
                                                    | OPCUA_SOCKET_TIMEOUT_EVENT;
    pAcceptInternalSocket->usPort                 = pListenInternalSocket->usPort;
    pAcceptInternalSocket->uintTimeout            = OPCUA_TCPLISTENER_TIMEOUT;
    pAcceptInternalSocket->uintLastAccess         = OpcUa_P_GetTickCount();

    OPCUA_SOCKET_SETVALID(pAcceptInternalSocket);

    return OpcUa_Good;
}

#if OPCUA_MULTITHREADED
/*============================================================================
*
*===========================================================================*/
static OpcUa_Int32 OpcUa_SocketManager_GetSocketManagerSlot(OpcUa_InternalSocketManager* a_pSocketManager,
                                                            OpcUa_InternalSocketManager* a_pSpawnedManager)
{
    OpcUa_Int32 iSocketManagerSlot;

    /* we are syncronized via the pStartupSemaphore... */

    for(iSocketManagerSlot = 0; iSocketManagerSlot < OPCUA_SOCKET_MAXMANAGERS; iSocketManagerSlot++)
    {
        if(a_pSocketManager->pSocketManagers[iSocketManagerSlot] == OpcUa_Null)
        {
            a_pSocketManager->pSocketManagers[iSocketManagerSlot] = a_pSpawnedManager;

            return iSocketManagerSlot;
        }
    }

    if (a_pSpawnedManager->pThread != OpcUa_Null && a_pSpawnedManager->pThreadToJoin == OpcUa_Null)
    {
        a_pSpawnedManager->pThreadToJoin = a_pSocketManager->pThreadToJoin;
        a_pSocketManager->pThreadToJoin  = a_pSpawnedManager->pThread;
        a_pSpawnedManager->pThread       = OpcUa_Null;
    }

    return -1;
}

/*============================================================================
*
*===========================================================================*/
static OpcUa_Void OpcUa_SocketManager_ReleaseSocketManagerSlot(OpcUa_InternalSocketManager* a_pSocketManager, OpcUa_Int32 a_uSlot)
{
    OpcUa_InternalSocketManager* pSpawnedManager;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(a_pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    pSpawnedManager = a_pSocketManager->pSocketManagers[a_uSlot];
    a_pSocketManager->pSocketManagers[a_uSlot] = OpcUa_Null;

    if (pSpawnedManager != OpcUa_Null && pSpawnedManager->pThread != OpcUa_Null && pSpawnedManager->pThreadToJoin == OpcUa_Null)
    {
        pSpawnedManager->pThreadToJoin  = a_pSocketManager->pThreadToJoin;
        a_pSocketManager->pThreadToJoin = pSpawnedManager->pThread;
        pSpawnedManager->pThread        = OpcUa_Null;
    }

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(a_pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    return;
}

/*============================================================================
* Takes appropriate action based on an event on a certain socket.
*===========================================================================*/
OpcUa_Void OpcUa_SocketManager_AcceptHandlerThread(OpcUa_Void* a_pArgument)
{

    OpcUa_InternalSocket*       pInternalSocket     = (OpcUa_InternalSocket*)a_pArgument;
    OpcUa_InternalSocketManager* pSocketManager     = pInternalSocket->pSocketManager;
    OpcUa_StatusCode            uStatus             = OpcUa_Good;
    OpcUa_Int32                 iSocketManagerSlot  = -1;
    OpcUa_InternalSocket*       pClientSocket       = OpcUa_Null;
    OpcUa_InternalSocket        ClientSocket[2]; /* one for the client, one for _signals_ */
    OpcUa_InternalSocketManager SpawnedSocketManager;


    OpcUa_MemSet(&ClientSocket, 0, sizeof(OpcUa_InternalSocket) * 2);
    OpcUa_MemSet(&SpawnedSocketManager, 0, sizeof(OpcUa_InternalSocketManager));

    SpawnedSocketManager.pThread                    = pSocketManager->pSpawnedThread;
    SpawnedSocketManager.uintMaxSockets             = 2;
    SpawnedSocketManager.pSockets                   = ClientSocket;
    SpawnedSocketManager.pCookie                    = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    SpawnedSocketManager.uintLastExternalEvent      = OPCUA_SOCKET_NO_EVENT;

    SpawnedSocketManager.Flags.bSpawnThreadOnAccept = 0;
    SpawnedSocketManager.Flags.bRejectOnThreadFail  = 0;

    ClientSocket[1].pSocketServiceTable             = &OpcUa_RawSocketServiceTable;

    /* obtain slot in global socket list array */
    uStatus = OpcUa_P_Mutex_Create(&SpawnedSocketManager.pMutex);

    if(OpcUa_IsGood(uStatus))
    {
        uStatus = OpcUa_P_SocketManager_NewSignalSocket(&SpawnedSocketManager);
    }

    if(OpcUa_IsGood(uStatus))
    {
        iSocketManagerSlot = OpcUa_SocketManager_GetSocketManagerSlot(pSocketManager, &SpawnedSocketManager);
        if(iSocketManagerSlot == -1)
        {
            uStatus = OpcUa_BadOutOfMemory;
        }
    }

    if(OpcUa_IsGood(uStatus))
    {
        pClientSocket = (OpcUa_InternalSocket*)OpcUa_SocketManager_FindFreeSocket(&SpawnedSocketManager, OpcUa_False);
    }

    /* handle event */
    uStatus = OpcUa_Socket_HandleAcceptEvent(  pInternalSocket,    /* listen socket */
                                               pClientSocket);     /* accepted socket */

    /* release spawn semaphore */
    pSocketManager->pSpawnedThread = OpcUa_Null;
    OpcUa_P_Semaphore_Post( pSocketManager->pStartupSemaphore,
                            1);

    if(OpcUa_IsGood(uStatus))
    {
        /* fire accept event */
        pClientSocket->Flags.bOwnThread = OpcUa_True;
        pClientSocket->Flags.bFromApplication = OpcUa_True;
        pClientSocket->pfnEventCallback(    (OpcUa_Socket)pClientSocket,
                                            OPCUA_SOCKET_ACCEPT_EVENT,
                                            pClientSocket->pvUserData,
                                            pClientSocket->usPort,
                                            OpcUa_False);
        pClientSocket->Flags.bFromApplication = OpcUa_False;

        do
        {
            uStatus = OpcUa_P_SocketManager_ServeLoopInternal(  &SpawnedSocketManager,
                                                                OPCUA_INFINITE,
                                                                OpcUa_False);

            if(OpcUa_IsEqual(OpcUa_GoodShutdownEvent))
            {
                /* leave this loop if a shutdown was signalled */
                break;
            }
        } while(OpcUa_IsGood(uStatus));

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SocketManager_AcceptHandlerThread: Client Handler shutting down! (0x%08X)\n", uStatus);

        OpcUa_SocketManager_ReleaseSocketManagerSlot(pSocketManager, iSocketManagerSlot);

        OpcUa_P_Mutex_Lock(SpawnedSocketManager.pMutex);

        if((ClientSocket[0].bSocketIsInUse != OpcUa_False) &&
           (ClientSocket[0].bInvalidSocket == OpcUa_False))
        {
            OpcUa_Socket_HandleEvent(&ClientSocket[0], OPCUA_SOCKET_CLOSE_EVENT);
            OpcUa_P_RawSocket_Close(ClientSocket[0].rawSocket);
        }
        if((ClientSocket[1].bSocketIsInUse != OpcUa_False) &&
           (ClientSocket[1].bInvalidSocket == OpcUa_False))
        {
            OpcUa_Socket_HandleEvent(&ClientSocket[1], OPCUA_SOCKET_CLOSE_EVENT);
            OpcUa_P_RawSocket_Close(ClientSocket[1].rawSocket);
        }
        OpcUa_P_RawSocket_Close(SpawnedSocketManager.pCookie);

        OpcUa_P_Mutex_Unlock(SpawnedSocketManager.pMutex);
        OpcUa_P_Mutex_Delete(&SpawnedSocketManager.pMutex);

        /* loop ended */
    }
    else
    {
        /* error, configuration maximum reached */
        if(iSocketManagerSlot != -1)
        {
            OpcUa_SocketManager_ReleaseSocketManagerSlot(pSocketManager, iSocketManagerSlot);
        }
        if(SpawnedSocketManager.pCookie != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
        {
            OpcUa_P_RawSocket_Close(SpawnedSocketManager.pCookie);
            OpcUa_P_RawSocket_Close(ClientSocket[0].rawSocket);
        }
        if(SpawnedSocketManager.pMutex != OpcUa_Null)
        {
            OpcUa_P_Mutex_Delete(&SpawnedSocketManager.pMutex);
        }
    }

    if(SpawnedSocketManager.pThreadToJoin != OpcUa_Null)
    {
        OpcUa_P_Thread_Delete(&SpawnedSocketManager.pThreadToJoin);
    }

    return;
}
#endif /* OPCUA_MULTITHREADED */

/*============================================================================
* Takes appropriate action based on an event on a certain socket.
*===========================================================================*/
OpcUa_StatusCode OpcUa_Socket_HandleEvent(  OpcUa_Socket a_pSocket,
                                            OpcUa_UInt32 a_uEvent)
{
    OpcUa_Socket            pAcceptedSocket = OpcUa_Null;
    OpcUa_InternalSocket*   pInternalSocket = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "HandleEvent");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

    /* update last access variable */
    pInternalSocket->uintLastAccess = OpcUa_P_GetTickCount();

    switch(a_uEvent)
    {
    case OPCUA_SOCKET_READ_EVENT:
        {
            /* OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_READ_EVENT\n"); */
            pInternalSocket->Flags.EventMask &= (~OPCUA_SOCKET_READ_EVENT);
            break;
        }
    case OPCUA_SOCKET_WRITE_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_WRITE_EVENT\n");
            pInternalSocket->Flags.EventMask &= (~OPCUA_SOCKET_WRITE_EVENT);
            break;
        }
    case OPCUA_SOCKET_CONNECT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_CONNECT_EVENT\n");
            OpcUa_P_RawSocket_GetLocalInfo(pInternalSocket->rawSocket, &(pInternalSocket->usPort));
            pInternalSocket->Flags.EventMask &= (~OPCUA_SOCKET_CONNECT_EVENT);
            break;
        }
    case OPCUA_SOCKET_CLOSE_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_CLOSE_EVENT\n");
            break;
        }
    case OPCUA_SOCKET_TIMEOUT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_TIMEOUT_EVENT\n");
            break;
        }
    case OPCUA_SOCKET_EXCEPT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_EXCEPT_EVENT\n");
            break;
        }
    case OPCUA_SOCKET_ACCEPT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: OPCUA_SOCKET_ACCEPT_EVENT\n");

#if OPCUA_MULTITHREADED
            if(pInternalSocket->pSocketManager->Flags.bSpawnThreadOnAccept != 0)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: Spawning Client Connection thread.\n");

                uStatus = OpcUa_P_Thread_Create(&pInternalSocket->pSocketManager->pSpawnedThread);
                if(OpcUa_IsGood(uStatus))
                {
                    uStatus = OpcUa_P_Thread_Start( pInternalSocket->pSocketManager->pSpawnedThread, /* handle */
                                                    OpcUa_SocketManager_AcceptHandlerThread,         /* handler */
                                                    (OpcUa_Void*)pInternalSocket);                   /* argument */
                    if(OpcUa_IsGood(uStatus))
                    {
                        /* we must wait until the spawned thread handled the accept event */
                        OpcUa_P_Semaphore_TimedWait(pInternalSocket->pSocketManager->pStartupSemaphore,
                                                    OPCUA_INFINITE);

                        OpcUa_ReturnStatusCode;
                    }
                    else
                    {
                        OpcUa_P_Thread_Delete(&pInternalSocket->pSocketManager->pSpawnedThread);
                    }
                }

                if(pInternalSocket->pSocketManager->Flags.bRejectOnThreadFail != 0)
                {
                    OpcUa_Socket_HandleAcceptEvent(a_pSocket, pAcceptedSocket);
                    OpcUa_ReturnStatusCode;
                }
            }
#endif /* OPCUA_MULTITHREADED */

            pAcceptedSocket = OpcUa_SocketManager_FindFreeSocket(pInternalSocket->pSocketManager, OpcUa_False);
            OpcUa_Socket_HandleAcceptEvent(a_pSocket, pAcceptedSocket);
            OpcUa_GotoErrorIfNull(pAcceptedSocket, OpcUa_BadMaxConnectionsReached);
            a_pSocket = pAcceptedSocket;
            break;
        }
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Socket_HandleEvent: Unknown event!\n");
            break;
        }

    }; /* end of event dispatcher */

    /* begin dispatching of remaining events */
    if(pInternalSocket->pfnEventCallback != OpcUa_Null)
    {
        pInternalSocket->Flags.bFromApplication = OpcUa_True;
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
        pInternalSocket->pfnEventCallback(a_pSocket, a_uEvent, pInternalSocket->pvUserData, pInternalSocket->usPort, OpcUa_False);
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Lock(pInternalSocket->pSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
        pInternalSocket->Flags.bFromApplication = OpcUa_False;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Socket_HandleEvent: pfnEventCallback is OpcUa_Null\n");
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Set the event mask for this socket.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Socket_SetEventMask( OpcUa_Socket a_pSocket,
                                            OpcUa_UInt32 a_uEventMask)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "SetEventMask");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfTrue(((OpcUa_InternalSocket*)a_pSocket)->rawSocket == OPCUA_P_SOCKET_INVALID,
                          OpcUa_BadCommunicationError);

    ((OpcUa_InternalSocket*)a_pSocket)->Flags.EventMask = (OpcUa_Int)a_uEventMask;

    OpcUa_P_SocketManager_SignalEvent(  ((OpcUa_InternalSocket*)a_pSocket)->pSocketManager,
                                        OPCUA_SOCKET_RENEWLOOP_EVENT,
                                        OpcUa_False);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get the currently set event mask for this socket.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Socket_GetEventMask(
    OpcUa_Socket a_pSocket,
    OpcUa_UInt32* a_pEventMask)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "GetEventMask");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfTrue(((OpcUa_InternalSocket*)a_pSocket)->rawSocket == OPCUA_P_SOCKET_INVALID,
                          OpcUa_BadCommunicationError);

    *a_pEventMask = (OpcUa_UInt32)((OpcUa_InternalSocket*)a_pSocket)->Flags.EventMask;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Find a free socket in the given list.
 *===========================================================================*/
OpcUa_Socket OpcUa_SocketManager_FindFreeSocket(    OpcUa_SocketManager     a_pSocketManager,
                                                    OpcUa_Boolean           a_bIsSignalSocket)
{
    OpcUa_UInt32                 uIndex       = 0;
    OpcUa_Boolean                bFound       = OpcUa_False;
    OpcUa_InternalSocketManager* pInternalSocketManager  = (OpcUa_InternalSocketManager*)a_pSocketManager;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    for(uIndex = 0; uIndex < pInternalSocketManager->uintMaxSockets; uIndex++)
    {
        if(uIndex == 0 && !a_bIsSignalSocket)
        {
            continue;
        }

        if(pInternalSocketManager->pSockets[uIndex].bSocketIsInUse == OpcUa_False)
        {
            pInternalSocketManager->pSockets[uIndex].bInvalidSocket         = OpcUa_True;
            pInternalSocketManager->pSockets[uIndex].Flags.bClosedSocket    = OpcUa_False;
            pInternalSocketManager->pSockets[uIndex].Flags.bOwnThread       = OpcUa_False;
            pInternalSocketManager->pSockets[uIndex].Flags.bFromApplication = OpcUa_False;
            pInternalSocketManager->pSockets[uIndex].Flags.EventMask        = 0;
            pInternalSocketManager->pSockets[uIndex].uintTimeout            = 0;
            pInternalSocketManager->pSockets[uIndex].uintLastAccess         = OpcUa_P_GetTickCount();
            pInternalSocketManager->pSockets[uIndex].pvUserData             = OpcUa_Null;
            pInternalSocketManager->pSockets[uIndex].pfnEventCallback       = OpcUa_Null;
            pInternalSocketManager->pSockets[uIndex].pSocketManager         = (OpcUa_InternalSocketManager *)a_pSocketManager;
            pInternalSocketManager->pSockets[uIndex].rawSocket              = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
            pInternalSocketManager->pSockets[uIndex].bSocketIsInUse         = OpcUa_True;

            bFound = OpcUa_True;

            break; /* for loop */
        }
    }

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(bFound)
    {
        return &(pInternalSocketManager->pSockets[uIndex]);
    }
    else
    {
        return OpcUa_Null;
    }
}




/*============================================================================
* Main socket based server loop.
*===========================================================================*/
OpcUa_StatusCode OpcUa_P_SocketManager_ServeLoopInternal(   OpcUa_SocketManager   a_pSocketManager,
                                                            OpcUa_UInt32          a_msecTimeout,
                                                            OpcUa_Boolean         bRunOnce)
{
    OpcUa_StatusCode                selectStatus            = OpcUa_Good;

    OpcUa_P_Socket_Array            readFdSet;
    OpcUa_P_Socket_Array            writeFdSet;
    OpcUa_P_Socket_Array            exceptFdSet;

    int                             max;
    OpcUa_InternalSocketManager*    pInternalSocketManager  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_ServeLoop");

    /* cap */
    if(a_msecTimeout > OPCUA_SOCKET_MAXLOOPTIME)
    {
        a_msecTimeout = OPCUA_SOCKET_MAXLOOPTIME;
    }

    if(a_pSocketManager == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    pInternalSocketManager = (OpcUa_InternalSocketManager*)a_pSocketManager;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* the serving loop */
    do
    {
        /* fill fdsets with the sockets from the SocketManager */
        max = OpcUa_P_Socket_FillFdSet(pInternalSocketManager, &readFdSet, OPCUA_SOCKET_READ_EVENT, 0);
        max = OpcUa_P_Socket_FillFdSet(pInternalSocketManager, &writeFdSet, (OPCUA_SOCKET_WRITE_EVENT | OPCUA_SOCKET_CONNECT_EVENT), max);
        max = OpcUa_P_Socket_FillFdSet(pInternalSocketManager, &exceptFdSet, OPCUA_SOCKET_EXCEPT_EVENT, max);

#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        /****************************************************************/
        /* This is the only point in the whole engine, where blocking   */
        /* of the current thread is allowed. Else, processing of        */
        /* network events is slowed down!                               */
#if OPCUA_MULTITHREADED
        selectStatus = OpcUa_P_RawSocket_Select(    (OpcUa_RawSocket)max, /* ignore on win */
                                                    &readFdSet,
                                                    &writeFdSet,
                                                    &exceptFdSet,
                                                    a_msecTimeout);
#else
        /* if we're here, the processing socketmanager should better be the global one...! */
        /* maybe test this state here */
        /* The provided ST config implements lowres timers via the global socketmanager's select timeout ... yes, it's lame ... */
        /* Thanks to Andy Griffith for the TimeredSelect and the Timer implementation in general. */
        selectStatus = OpcUa_P_Socket_TimeredSelect((OpcUa_RawSocket)max, /* ignore on win */
                                                    &readFdSet,
                                                    &writeFdSet,
                                                    &exceptFdSet,
                                                    a_msecTimeout);
#endif
        /*                                                              */
        /****************************************************************/


#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        /* handle errors in select, excluding timeout */
        /* "good" errors result from timeout and closeing the signal socket; the rest is bad... */
        if(OpcUa_IsBad(selectStatus))
        {
            uStatus = OpcUa_BadCommunicationError;
            goto Error;
        }

        /* check for external events */
        /* right after possible select delay */
        if (OPCUA_P_SOCKET_ARRAY_ISSET(pInternalSocketManager->pSockets[0].rawSocket, &readFdSet))
        {
            uStatus = OpcUa_P_Socket_HandleExternalEvent(pInternalSocketManager);
            OpcUa_GotoErrorIfBad(uStatus);

            /* continue if a renew event was signalled */
            if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
            {
                continue;
            }

            /* leave if a shutdown event was signalled */
            if(OpcUa_IsEqual(OpcUa_GoodShutdownEvent))
            {
                break;
            }
        }

        /* Handle Events by calling the registered callbacks (all sockets except the waiting socket) */
        OpcUa_P_Socket_HandleFdSet(pInternalSocketManager, &exceptFdSet,  OPCUA_SOCKET_EXCEPT_EVENT);
        OpcUa_P_Socket_HandleFdSet(pInternalSocketManager, &writeFdSet,  (OPCUA_SOCKET_WRITE_EVENT | OPCUA_SOCKET_CONNECT_EVENT));
        OpcUa_P_Socket_HandleFdSet(pInternalSocketManager, &readFdSet,    OPCUA_SOCKET_READ_EVENT);

    } while(!bRunOnce);

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_FinishErrorHandling;
}

/*============================================================================
* FillFdSet
*===========================================================================*/
int OpcUa_P_Socket_FillFdSet(  OpcUa_SocketManager     pSocketManager,
                               OpcUa_P_Socket_Array*   pSocketArray,
                               OpcUa_UInt32            uintEvent,
                               int                     max)
{
    OpcUa_UInt32                    uintIndex      = 0;
    OpcUa_UInt32                    uintTempEvent  = 0;
    OpcUa_InternalSocketManager*    pInternalSocketManager    = (OpcUa_InternalSocketManager*)pSocketManager;

    OPCUA_P_SOCKET_ARRAY_ZERO(pSocketArray);

    for(uintIndex = 0; uintIndex < pInternalSocketManager->uintMaxSockets; uintIndex++)
    {
        uintTempEvent = uintEvent;

        /* if socket used and valid */
        if(     (pInternalSocketManager->pSockets[uintIndex].bSocketIsInUse  != OpcUa_False)
            &&  (pInternalSocketManager->pSockets[uintIndex].bInvalidSocket  == OpcUa_False))
        {
            /* is connect event wished by caller? */
            if((uintTempEvent & OPCUA_SOCKET_CONNECT_EVENT) != 0)
            {
                /* and is connect event wished by socket? */
                if(((pInternalSocketManager->pSockets[uintIndex].Flags.EventMask) & OPCUA_SOCKET_CONNECT_EVENT) != 0)
                {
                    /* then set to connect only */
                    uintTempEvent = OPCUA_SOCKET_CONNECT_EVENT;
                }
                else
                {
                    /* else remove connect event */
                    uintTempEvent &= ~ OPCUA_SOCKET_CONNECT_EVENT;
                }
            }

            /* if only uintTemp is wished, set the socket in the fd_set */
            if(((pInternalSocketManager->pSockets[uintIndex].Flags.EventMask) & uintTempEvent) == uintTempEvent)
            {
                OPCUA_P_SOCKET_ARRAY_SET(pInternalSocketManager->pSockets[uintIndex].rawSocket, pSocketArray);
                if (pInternalSocketManager->pSockets[uintIndex].rawSocket > max)
                {
                    max = pInternalSocketManager->pSockets[uintIndex].rawSocket;
                }
            }
        }
    }

    return max;
}

/*============================================================================
* CreateServer
*===========================================================================*/
/* create a socket and configure it as a server socket */
OpcUa_RawSocket OpcUa_P_Socket_CreateServer(    OpcUa_StringA       IpAddress,
                                                OpcUa_Int16         Port,
                                                OpcUa_StatusCode*   Status)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    OpcUa_RawSocket     RawSocket   = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    OpcUa_Boolean       bIpV6       = OpcUa_True;

    if(IpAddress == OpcUa_Null)
    {
        /* bind to everything */
        uStatus = OpcUa_P_RawSocket_CreateV6(   &RawSocket,
                                                OpcUa_True,     /* Nagle off */
                                                OpcUa_True,     /* Reuse on  */
                                                OpcUa_False);   /* IPv4+6    */
        if(OpcUa_IsBad(uStatus))
        {
            bIpV6 = OpcUa_False;
            uStatus = OpcUa_P_RawSocket_Create(   &RawSocket,
                                                  OpcUa_True,     /* Nagle off */
                                                  OpcUa_True);    /* Reuse on  */
        }
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else if(strchr(IpAddress, ':'))
    {
        uStatus = OpcUa_P_RawSocket_CreateV6(   &RawSocket,
                                                OpcUa_True,     /* Nagle off */
                                                OpcUa_True,     /* Reuse on  */
                                                OpcUa_True);    /* IPv6 only */
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        bIpV6 = OpcUa_False;
        uStatus = OpcUa_P_RawSocket_Create(     &RawSocket,
                                                OpcUa_True,     /* Nagle off */
                                                OpcUa_True);    /* Reuse on  */
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_GotoErrorIfTrue((RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID), OpcUa_BadCommunicationError);

    /* set nonblocking */
    uStatus = OpcUa_P_RawSocket_SetBlockMode(   RawSocket,
                                                OpcUa_False);
    OpcUa_GotoErrorIfBad(uStatus);

    if(bIpV6)
    {
        uStatus = OpcUa_P_RawSocket_BindV6(RawSocket, IpAddress, Port);
    }
    else if(IpAddress == OpcUa_Null)
    {
        uStatus = OpcUa_P_RawSocket_Bind(RawSocket, Port);
    }
    else
    {
        uStatus = OpcUa_P_RawSocket_BindEx(RawSocket, IpAddress, Port);
    }
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_P_RawSocket_Listen(RawSocket);
    OpcUa_GotoErrorIfBad(uStatus);

    if(Status != OpcUa_Null)
    {
        *Status = uStatus;
    }

    return RawSocket;

Error:
    if(Status != OpcUa_Null)
    {
        *Status = uStatus;
    }

    /* ignore errors which may happen, when RawSocket is invalid */
    if(RawSocket != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        OpcUa_P_RawSocket_Close(RawSocket);
    }

    return (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
}

/*============================================================================
* HandleFdSet
*===========================================================================*/
OpcUa_Void OpcUa_P_Socket_HandleFdSet(  OpcUa_SocketManager     a_pSocketManager,
                                        OpcUa_P_Socket_Array*   a_pSocketArray,
                                        OpcUa_UInt32            a_uEvent)
{
    OpcUa_InternalSocketManager*    pInternalSocketManager         = (OpcUa_InternalSocketManager*)a_pSocketManager;
    OpcUa_UInt32                    uintIndex           = 0;
    OpcUa_UInt32                    uintLocalEvent      = 0;
    OpcUa_UInt32                    uintTimeDifference  = 0; /* seconds */

    for(uintIndex = 1; uintIndex < pInternalSocketManager->uintMaxSockets; uintIndex++)
    {
        uintLocalEvent = a_uEvent;

        if(    (pInternalSocketManager->pSockets[uintIndex].bSocketIsInUse != OpcUa_False)
           &&  (pInternalSocketManager->pSockets[uintIndex].bInvalidSocket == OpcUa_False))
        {
            if(   (pInternalSocketManager->pSockets[uintIndex].Flags.bClosedSocket == OpcUa_False)
               && OPCUA_P_SOCKET_ARRAY_ISSET(pInternalSocketManager->pSockets[uintIndex].rawSocket, a_pSocketArray))
            {
                if((uintLocalEvent == OPCUA_SOCKET_READ_EVENT) && (pInternalSocketManager->pSockets[uintIndex].Flags.EventMask & OPCUA_SOCKET_ACCEPT_EVENT))
                {
                    uintLocalEvent = OPCUA_SOCKET_ACCEPT_EVENT;
                }

                if(uintLocalEvent & OPCUA_SOCKET_CONNECT_EVENT)
                {
                    if(pInternalSocketManager->pSockets[uintIndex].Flags.EventMask & OPCUA_SOCKET_CONNECT_EVENT)
                    {
                        uintLocalEvent = OPCUA_SOCKET_CONNECT_EVENT;
                    }
                    else
                    {
                        uintLocalEvent &=~ OPCUA_SOCKET_CONNECT_EVENT;
                    }
                }

                /* the real reason for exception events is received through getsockopt with SO_ERROR */
                if(uintLocalEvent == OPCUA_SOCKET_CONNECT_EVENT)
                {
                    int       apiResult = 0, value = 0;
                    socklen_t size      = sizeof(value);
                    apiResult = getsockopt(pInternalSocketManager->pSockets[uintIndex].rawSocket, SOL_SOCKET, SO_ERROR, (char*)&value, &size);
                    if(apiResult == 0 && value != 0)
                    {
                        uintLocalEvent = OPCUA_SOCKET_EXCEPT_EVENT;
                    }
                }

                OpcUa_Socket_HandleEvent(&pInternalSocketManager->pSockets[uintIndex], uintLocalEvent);
            }

            else if(uintLocalEvent == OPCUA_SOCKET_EXCEPT_EVENT)
            {
                /* Only check timeout, if a timeout value is set for the socket */
                if(pInternalSocketManager->pSockets[uintIndex].uintTimeout != 0)
                {
                    /* check for Timeout too */
                    uintTimeDifference = OpcUa_P_GetTickCount() - pInternalSocketManager->pSockets[uintIndex].uintLastAccess;

                    if((int)uintTimeDifference > (int)pInternalSocketManager->pSockets[uintIndex].uintTimeout)
                    {
                        /* the connection on this socket timed out */
                        OpcUa_Socket_HandleEvent(&pInternalSocketManager->pSockets[uintIndex], OPCUA_SOCKET_TIMEOUT_EVENT);
                    }
                }
            }

            if(pInternalSocketManager->pSockets[uintIndex].Flags.bClosedSocket != OpcUa_False)
            {
                OpcUa_Socket_HandleEvent(&pInternalSocketManager->pSockets[uintIndex], OPCUA_SOCKET_CLOSE_EVENT);

                OpcUa_P_RawSocket_Close(pInternalSocketManager->pSockets[uintIndex].rawSocket);

                pInternalSocketManager->pSockets[uintIndex].rawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;

                pInternalSocketManager->pSockets[uintIndex].bSocketIsInUse = OpcUa_False;
            }

        }

    }

    return;
}

/*============================================================================
* HandleExternalEvent
*===========================================================================*/
OpcUa_StatusCode OpcUa_P_Socket_HandleExternalEvent(    OpcUa_SocketManager a_pSocketManager)
{
    OpcUa_UInt32                 uExternalEvent         = OPCUA_SOCKET_NO_EVENT;
    OpcUa_InternalSocketManager* pInternalSocketManager = (OpcUa_InternalSocketManager*)a_pSocketManager;
    unsigned char                dummy[32];

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_HandleExternalEvent");

    OpcUa_GotoErrorIfArgumentNull(a_pSocketManager);

    OpcUa_P_RawSocket_Read(pInternalSocketManager->pSockets[0].rawSocket, dummy, sizeof(dummy));

    if(pInternalSocketManager->uintLastExternalEvent != OPCUA_SOCKET_NO_EVENT)
    {
        uExternalEvent = pInternalSocketManager->uintLastExternalEvent;

        /* check for renew event set externally in list */
        if((uExternalEvent & OPCUA_SOCKET_RENEWLOOP_EVENT) != OPCUA_SOCKET_NO_EVENT)
        {
            /* loop has been interrupted externally to restart with the new/changed list */
            pInternalSocketManager->uintLastExternalEvent &= (~OPCUA_SOCKET_RENEWLOOP_EVENT);
            uStatus = OpcUa_GoodCallAgain;
        }

        /* was this the Shutdown Event, raised by the system? */
        if((uExternalEvent & OPCUA_SOCKET_SHUTDOWN_EVENT) != OPCUA_SOCKET_NO_EVENT)
        {
            /* if uStatus !=  */
            uStatus = OpcUa_GoodShutdownEvent;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Internal Function
 *===========================================================================*/
OpcUa_RawSocket OpcUa_P_Socket_CreateClient(    OpcUa_UInt16                    a_uPort,
                                                OpcUa_UInt16                    a_uRemotePort,
                                                OpcUa_StringA                   a_sRemoteAddress,
                                                OpcUa_StatusCode*               a_uStatus)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    OpcUa_RawSocket     RawSocket   = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    OpcUa_Boolean       bIpV6       = OpcUa_True;

    if(strchr(a_sRemoteAddress, ':'))
    {
        uStatus = OpcUa_P_RawSocket_CreateV6(  &RawSocket,
                                               OpcUa_True,     /* Nagle off */
                                               OpcUa_False,    /* Reuse off */
                                               OpcUa_False);   /* IPv4+6    */
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        bIpV6 = OpcUa_False;
        uStatus = OpcUa_P_RawSocket_Create(    &RawSocket,
                                               OpcUa_True,     /* Nagle off */
                                               OpcUa_False);   /* Reuse off */
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        goto Error;
    }

    /* set nonblocking */
    uStatus = OpcUa_P_RawSocket_SetBlockMode(   RawSocket,
                                                OpcUa_False);
    OpcUa_GotoErrorIfBad(uStatus);

    if(a_uPort != (OpcUa_UInt16)0)
    {
        if(bIpV6)
        {
            /* bind always to any IP address */
            uStatus = OpcUa_P_RawSocket_BindV6(RawSocket, OpcUa_Null, a_uPort);
            OpcUa_GotoErrorIfBad(uStatus);
        }
        else
        {
            /* bind always to any IP address */
            uStatus = OpcUa_P_RawSocket_Bind(RawSocket, a_uPort);
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }

    if(a_uRemotePort != 0)
    {
        if(bIpV6)
        {
            uStatus = OpcUa_P_RawSocket_ConnectV6(  RawSocket,
                                                    a_uRemotePort,
                                                    a_sRemoteAddress);
        }
        else
        {
            uStatus = OpcUa_P_RawSocket_Connect(    RawSocket,
                                                    a_uRemotePort,
                                                    a_sRemoteAddress);
        }

        if(OpcUa_IsBad(uStatus))
        {
            /* we are nonblocking and would block is not an error in this mode */
            if(uStatus != OpcUa_BadWouldBlock)
            {
                goto Error;
            }
            else
            {
                uStatus = OpcUa_Good;
            }
        }
    }

    if(a_uStatus != OpcUa_Null)
    {
        *a_uStatus = uStatus;
    }

    return RawSocket;

Error:

    if(a_uStatus != OpcUa_Null)
    {
        if(OpcUa_IsBad(uStatus))
        {
            *a_uStatus = uStatus;
        }
    }

    if(RawSocket != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        OpcUa_P_RawSocket_Close(RawSocket); /* just in case */
    }

    return (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
}
