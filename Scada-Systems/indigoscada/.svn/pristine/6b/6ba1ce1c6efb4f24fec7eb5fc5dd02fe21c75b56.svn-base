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
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


/* UA platform definitions */
#include <opcua_p_internal.h>

/* UA platform definitions */
#include <opcua_datetime.h>

#include <opcua_p_semaphore.h>
#include <opcua_p_thread.h>
#include <opcua_p_mutex.h>
#include <opcua_p_utilities.h>
#include <opcua_p_memory.h>

/* own headers */
#include <opcua_p_socket.h>
#include <opcua_p_socket_internal.h>
#include <opcua_p_socket_interface.h>

#if OPCUA_MULTITHREADED
/*============================================================================
 * This function serves a single socket manager in multithreading configuration.
 *===========================================================================*/
/* HINT: That is a thread entry point and wrapper for the real serverloop. */
 OpcUa_Void OpcUa_P_SocketManager_ServerLoopThread(OpcUa_Void* a_pArgument)
{
    OpcUa_StatusCode                uStatus                 = OpcUa_Good;                   /* only needed for internal reasons */
    OpcUa_InternalSocketManager*    pInternalSocketManager  = (OpcUa_InternalSocketManager*)a_pArgument;
    OpcUa_Int32                     iSocketManagerSlot;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "NetworkThread: Message Loop started...\n");

    do
    {
        uStatus = OpcUa_P_SocketManager_ServeLoopInternal(  pInternalSocketManager,
                                                            OpcUa_UInt32_Max,
                                                            OpcUa_False);

        if(OpcUa_IsEqual(OpcUa_GoodShutdownEvent))
        {
            /* leave this loop if a shutdown was signalled */
            break;
        }

    } while(OpcUa_IsGood(uStatus));

    /* Debug Output */
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "NetworkThread: Message Loop shutting down! (0x%08X)\n", uStatus);

    if(pInternalSocketManager->Flags.bSpawnThreadOnAccept != 0)
    {
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        for(iSocketManagerSlot = 0; iSocketManagerSlot < OPCUA_SOCKET_MAXMANAGERS; iSocketManagerSlot++)
        {
            if(pInternalSocketManager->pSocketManagers[iSocketManagerSlot] != OpcUa_Null)
            {
                OpcUa_InternalSocketManager *pSpawnedSocketManager = pInternalSocketManager->pSocketManagers[iSocketManagerSlot];
                pInternalSocketManager->pSocketManagers[iSocketManagerSlot] = OpcUa_Null;

                pSpawnedSocketManager->pThreadToJoin  = pInternalSocketManager->pThreadToJoin;
                pInternalSocketManager->pThreadToJoin = pSpawnedSocketManager->pThread;
                pSpawnedSocketManager->pThread        = OpcUa_Null;

                OpcUa_P_SocketManager_InterruptLoop(pSpawnedSocketManager,
                                                    OPCUA_SOCKET_SHUTDOWN_EVENT,
                                                    OpcUa_False);
            }
        }

#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        if(pInternalSocketManager->pThreadToJoin != OpcUa_Null)
        {
            OpcUa_P_Thread_Delete(&pInternalSocketManager->pThreadToJoin);
        }
    }

    return;
}
#else
static OpcUa_SocketManager OpcUa_Socket_g_SocketManager = OpcUa_Null;
#endif /* OPCUA_MULTITHREADED */


/*============================================================================
 * Create a new signal socket
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_SocketManager_NewSignalSocket(OpcUa_SocketManager a_pSocketManager)
{
    OpcUa_InternalSocket*           pIntSignalSocket = OpcUa_Null;
    OpcUa_InternalSocketManager*    pInternalSocketManager      = (OpcUa_InternalSocketManager*)a_pSocketManager;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "NewSignalSocket");

    OpcUa_GotoErrorIfArgumentNull(a_pSocketManager);

    pIntSignalSocket = (OpcUa_InternalSocket*)OpcUa_SocketManager_FindFreeSocket(a_pSocketManager, OpcUa_True);

    if(pIntSignalSocket == OpcUa_Null)
    {
        uStatus = OpcUa_BadResourceUnavailable;
        goto Error;
    }

    uStatus = OpcUa_P_RawSocket_CreateSocketPair( &pIntSignalSocket->rawSocket,
                                                  &pInternalSocketManager->pCookie);
    OpcUa_GotoErrorIfBad(uStatus);

    pIntSignalSocket->Flags.EventMask =   OPCUA_SOCKET_CLOSE_EVENT
                                        | OPCUA_SOCKET_READ_EVENT
                                        | OPCUA_SOCKET_EXCEPT_EVENT
                                        | OPCUA_SOCKET_TIMEOUT_EVENT;

    uStatus = OpcUa_P_RawSocket_SetBlockMode (pIntSignalSocket->rawSocket, OpcUa_False);
    if (OpcUa_IsBad(uStatus))
    {
        OpcUa_P_RawSocket_Close(pIntSignalSocket->rawSocket);
        OpcUa_P_RawSocket_Close(pInternalSocketManager->pCookie);
        OpcUa_GotoErrorWithStatus(uStatus);
    }

    uStatus = OpcUa_P_RawSocket_SetBlockMode (pInternalSocketManager->pCookie, OpcUa_False);
    if (OpcUa_IsBad(uStatus))
    {
        OpcUa_P_RawSocket_Close(pIntSignalSocket->rawSocket);
        OpcUa_P_RawSocket_Close(pInternalSocketManager->pCookie);
        OpcUa_GotoErrorWithStatus(uStatus);
    }

    OPCUA_SOCKET_SETVALID(pIntSignalSocket);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pIntSignalSocket)
    {
        pIntSignalSocket->rawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
        OPCUA_SOCKET_INVALIDATE(pIntSignalSocket);
    }

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * Break server loop(s) and issue event(s).
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_InterruptLoop( OpcUa_SocketManager a_pSocketManager,
                                                                    OpcUa_UInt32        a_uEvent,
                                                                    OpcUa_Boolean       a_bAllManagers)
{
    OpcUa_RawSocket                 pSignalSocket;
    OpcUa_InternalSocketManager*    pInternalSocketManager;
    unsigned char                   dummy[1] = {0};

OpcUa_InitializeStatus(OpcUa_Module_Socket, "InterruptLoop");

    OpcUa_ReferenceParameter(a_bAllManagers);

    if(a_uEvent == OPCUA_SOCKET_NO_EVENT)
    {
        return OpcUa_BadInternalError;
    }

    if(a_pSocketManager == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    pInternalSocketManager = (OpcUa_InternalSocketManager*)a_pSocketManager;

    pSignalSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;

#if OPCUA_MULTITHREADED

    /* get exclusive access to the list */
#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */


    /************ core begin ************/
    /* set event(s) */
    if ((pInternalSocketManager->uintLastExternalEvent & a_uEvent) != a_uEvent)
    {
        pInternalSocketManager->uintLastExternalEvent |= a_uEvent;

        /* get the signal socket, which is hidden as a cookie */
        pSignalSocket = pInternalSocketManager->pCookie;
    }

    /************* core end *************/

    /* release exclusive access to the list */
#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* if there, send a dummy byte to break selects */
    if(pSignalSocket != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        OpcUa_P_RawSocket_Write(pSignalSocket, dummy, sizeof(dummy));
    }

#endif /* OPCUA_MULTITHREADED */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Call the main serve loop for a certain socket list.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_ServeLoop( OpcUa_SocketManager a_pSocketManager,
                                                                OpcUa_UInt32        a_msecTimeout,
                                                                OpcUa_Boolean       a_bRunOnce)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "ServeLoop");

#if !OPCUA_MULTITHREADED
    if(a_pSocketManager == OpcUa_Null)
    {
        a_pSocketManager = OpcUa_Socket_g_SocketManager;
    }
#endif

    uStatus =  OpcUa_P_SocketManager_ServeLoopInternal( a_pSocketManager,
                                                        a_msecTimeout,
                                                        a_bRunOnce);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Create a new socket manager or initialize the global one (OpcUa_Null first).
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_Create(OpcUa_SocketManager*    a_pSocketManager,
                                                            OpcUa_UInt32            a_nSockets,
                                                            OpcUa_UInt32            a_nFlags)
{
    OpcUa_InternalSocketManager*    pInternalSocketManager   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SocketManager_Create");

    if(a_nFlags & 0xFFFFFFF8)
    {
        return OpcUa_BadInvalidArgument;
    }

    if(a_nSockets > OPCUA_P_SOCKETMANAGER_NUMBEROFSOCKETS)
    {
        return OpcUa_BadInvalidArgument;
    }

    /* set number of socket to maximum */
    if(a_nSockets == 0)
    {
        a_nSockets = OPCUA_P_SOCKETMANAGER_NUMBEROFSOCKETS;
    }

    a_nSockets += 1; /* add signal socket to requested sockets */

#if !OPCUA_MULTITHREADED
    if(a_pSocketManager == OpcUa_Null && OpcUa_Socket_g_SocketManager == OpcUa_Null)
    {
        a_pSocketManager = &OpcUa_Socket_g_SocketManager;
    }
#endif

    if(a_pSocketManager == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    *a_pSocketManager = OpcUa_SocketManager_Alloc();
    OpcUa_GotoErrorIfAllocFailed(*a_pSocketManager);

    pInternalSocketManager = (OpcUa_InternalSocketManager*)*a_pSocketManager;

    OpcUa_SocketManager_Initialize(pInternalSocketManager);

#if OPCUA_USE_SYNCHRONISATION
    uStatus = OpcUa_P_Mutex_Create(&pInternalSocketManager->pMutex);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* preallocate socket structures for all possible sockets (maxsockets) */
    uStatus = OpcUa_SocketManager_CreateSockets((OpcUa_SocketManager)pInternalSocketManager, a_nSockets);
    OpcUa_GotoErrorIfBad(uStatus);

    pInternalSocketManager->uintLastExternalEvent  = OPCUA_SOCKET_NO_EVENT;

    /* set the behaviour flags */
    if((a_nFlags & OPCUA_SOCKET_SPAWN_THREAD_ON_ACCEPT)    != OPCUA_SOCKET_NO_FLAG)
    {
        pInternalSocketManager->Flags.bSpawnThreadOnAccept      = OpcUa_True;
    }

    if((a_nFlags & OPCUA_SOCKET_REJECT_ON_NO_THREAD)       != OPCUA_SOCKET_NO_FLAG)
    {
        pInternalSocketManager->Flags.bRejectOnThreadFail       = OpcUa_True;
    }

    if((a_nFlags & OPCUA_SOCKET_DONT_CLOSE_ON_EXCEPT)      != OPCUA_SOCKET_NO_FLAG)
    {
        pInternalSocketManager->Flags.bDontCloseOnExcept        = OpcUa_True;
    }

    uStatus = OpcUa_P_SocketManager_NewSignalSocket(pInternalSocketManager);
    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_MULTITHREADED
    if (pInternalSocketManager->Flags.bSpawnThreadOnAccept)
    {
        /* create a semaphore with no free resources for which a host can wait to be signalled. */
        uStatus = OpcUa_P_Semaphore_Create(&pInternalSocketManager->pStartupSemaphore, 0, 1);
        OpcUa_GotoErrorIfBad(uStatus);

        pInternalSocketManager->pSocketManagers = OpcUa_P_Memory_Alloc(sizeof(OpcUa_InternalSocketManager*) * OPCUA_SOCKET_MAXMANAGERS);
        OpcUa_GotoErrorIfAllocFailed(pInternalSocketManager->pSocketManagers);
        OpcUa_MemSet(pInternalSocketManager->pSocketManagers, 0, sizeof(OpcUa_InternalSocketManager*) * OPCUA_SOCKET_MAXMANAGERS);
    }

    /* if multithreaded, create and start the server thread if the list is not the global list. */
    uStatus = OpcUa_P_Thread_Create(&pInternalSocketManager->pThread); /* make raw thread */
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_P_Thread_Start( pInternalSocketManager->pThread,
                                    OpcUa_P_SocketManager_ServerLoopThread,
                                    (OpcUa_Void*)pInternalSocketManager);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_MULTITHREADED */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pInternalSocketManager != OpcUa_Null)
    {
#if OPCUA_MULTITHREADED
        if(pInternalSocketManager->pThread != OpcUa_Null)
        {
            OpcUa_P_Thread_Delete(&pInternalSocketManager->pThread);
        }
        if(pInternalSocketManager->pSocketManagers != OpcUa_Null)
        {
            OpcUa_P_Memory_Free(pInternalSocketManager->pSocketManagers);
        }
        if(pInternalSocketManager->pStartupSemaphore != OpcUa_Null)
        {
            OpcUa_P_Semaphore_Delete(&pInternalSocketManager->pStartupSemaphore);
        }
#endif /* OPCUA_MULTITHREADED */
        if(pInternalSocketManager->pCookie != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
        {
            OpcUa_P_RawSocket_Close(pInternalSocketManager->pCookie);
        }
        if(pInternalSocketManager->pSockets != OpcUa_Null)
        {
            if(pInternalSocketManager->pSockets[0].rawSocket != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
            {
                OpcUa_P_RawSocket_Close(pInternalSocketManager->pSockets[0].rawSocket);
            }
            OpcUa_P_Memory_Free(pInternalSocketManager->pSockets);
        }
#if OPCUA_USE_SYNCHRONISATION
        if(pInternalSocketManager->pMutex != OpcUa_Null)
        {
            OpcUa_P_Mutex_Delete(&pInternalSocketManager->pMutex);
        }
#endif /* OPCUA_USE_SYNCHRONISATION */
        OpcUa_P_Memory_Free(pInternalSocketManager);
    }

    *a_pSocketManager = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Delete SocketManager Type (closes all sockets)
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_SocketManager_Delete(OpcUa_SocketManager* a_pSocketManager)
{
    OpcUa_InternalSocketManager* pInternalSocketManager = OpcUa_Null;
    OpcUa_UInt32                 uintIndex              = 0;

#if !OPCUA_MULTITHREADED
    if(a_pSocketManager == OpcUa_Null && OpcUa_Socket_g_SocketManager != OpcUa_Null)
    {
        a_pSocketManager = &OpcUa_Socket_g_SocketManager;
    }
#endif

    if(a_pSocketManager == OpcUa_Null || *a_pSocketManager == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SocketManager_Delete: Invalid Socket Manager!\n");
        return;
    }

    pInternalSocketManager = (OpcUa_InternalSocketManager*)*a_pSocketManager;

    /* send shutdown event to serveloop */
    OpcUa_P_SocketManager_InterruptLoop(pInternalSocketManager, OPCUA_SOCKET_SHUTDOWN_EVENT, OpcUa_False);

#if OPCUA_MULTITHREADED
    if(pInternalSocketManager->pThread != OpcUa_Null)
    {
        OpcUa_P_Thread_Delete(&(pInternalSocketManager->pThread));
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SocketManager_Delete: Invalid Thread Handle!\n");
        return;
    }
#endif /* OPCUA_MULTITHREADED */

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* handle the socket list content (close, cleanup, etc.) */
    if(pInternalSocketManager->pSockets != OpcUa_Null)
    {
        for(uintIndex = 0; uintIndex < pInternalSocketManager->uintMaxSockets; uintIndex++)
        {
            OpcUa_Socket pSocketTemp = &(pInternalSocketManager->pSockets[uintIndex]);

            if(   (pInternalSocketManager->pSockets[uintIndex].bSocketIsInUse != OpcUa_False)
               && (pInternalSocketManager->pSockets[uintIndex].bInvalidSocket == OpcUa_False))
            {
                OpcUa_Socket_HandleEvent(pSocketTemp, OPCUA_SOCKET_CLOSE_EVENT);
                OpcUa_P_RawSocket_Close(pInternalSocketManager->pSockets[uintIndex].rawSocket);
            }

            OpcUa_Socket_Clear(pSocketTemp);
        }

        OpcUa_P_Memory_Free(pInternalSocketManager->pSockets);
    } /* if(pInternalSocketManager->pSockets != OpcUa_Null) */

    OpcUa_P_RawSocket_Close(pInternalSocketManager->pCookie);

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocketManager->pMutex);
    OpcUa_P_Mutex_Delete(&pInternalSocketManager->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

#if OPCUA_MULTITHREADED
    if(pInternalSocketManager->pStartupSemaphore != OpcUa_Null)
    {
        OpcUa_P_Semaphore_Delete(&pInternalSocketManager->pStartupSemaphore);
    }
    if(pInternalSocketManager->pSocketManagers != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(pInternalSocketManager->pSocketManagers);
    }
#endif

    OpcUa_P_Memory_Free(*a_pSocketManager);
    *a_pSocketManager = OpcUa_Null;

    return;
}

/*============================================================================
 * Initialize the platform network interface
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_InitializeNetwork(OpcUa_Void)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "InitializeNetwork");

    uStatus = OpcUa_P_RawSocket_InitializeNetwork();
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* cleanup everything */
    OpcUa_P_Socket_CleanupNetwork();

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Clean the platform network interface up.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_CleanupNetwork(OpcUa_Void)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "CleanupNetwork");

    /* cleanup platform networking */
    uStatus = OpcUa_P_RawSocket_CleanupNetwork();
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Create a server socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateServer(  OpcUa_SocketManager         a_pSocketManager,
                                                                    OpcUa_StringA               a_sAddress,
                                                                    OpcUa_Boolean               a_bListenOnAllInterfaces,
                                                                    OpcUa_Socket_EventCallback  a_pfnSocketCallBack,
                                                                    OpcUa_Void*                 a_pCallbackData,
                                                                    OpcUa_Socket*               a_pSocket)
{
    OpcUa_InternalSocketManager*    pInternalSocketManager  = OpcUa_Null;
    OpcUa_UInt16                    uPort                   = 0;
    OpcUa_StringA                   sRemoteAdress           = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "CreateServer");

#if !OPCUA_MULTITHREADED
    if(a_pSocketManager == OpcUa_Null)
    {
        a_pSocketManager = OpcUa_Socket_g_SocketManager;
    }
#endif

    OpcUa_ReturnErrorIfArgumentNull(a_pSocketManager);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    pInternalSocketManager = (OpcUa_InternalSocketManager*)a_pSocketManager;

    /* parse address */
    uStatus = OpcUa_P_ParseUrl( a_sAddress,
                                &sRemoteAdress,
                                &uPort);

    OpcUa_ReturnErrorIfBad(uStatus);
    if(a_bListenOnAllInterfaces)
    {
        if(sRemoteAdress != OpcUa_Null)
        {
            OpcUa_P_Memory_Free(sRemoteAdress);
            sRemoteAdress = OpcUa_Null;
        }
    }

    uStatus = OpcUa_SocketManager_InternalCreateServer( pInternalSocketManager,
                                                        sRemoteAdress,
                                                        uPort,
                                                        a_pfnSocketCallBack,
                                                        a_pCallbackData,
                                                        a_pSocket);
    OpcUa_GotoErrorIfBad(uStatus);


    OpcUa_P_SocketManager_InterruptLoop(pInternalSocketManager,
                                        OPCUA_SOCKET_RENEWLOOP_EVENT,
                                        OpcUa_False);

    if(sRemoteAdress != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(sRemoteAdress);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(sRemoteAdress != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(sRemoteAdress);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Create a client socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateClient(  OpcUa_SocketManager         a_hSocketManager,
                                                                    OpcUa_StringA               a_sRemoteAddress,
                                                                    OpcUa_UInt16                a_uLocalPort,
                                                                    OpcUa_Socket_EventCallback  a_pfnSocketCallBack,
                                                                    OpcUa_Void*                 a_pCallbackData,
                                                                    OpcUa_Socket*               a_pSocket)
{
    OpcUa_InternalSocket*        pNewClientSocket       = OpcUa_Null;
    OpcUa_InternalSocketManager* pInternalSocketManager = OpcUa_Null;
    OpcUa_UInt16                 uPort                  = 0;
    OpcUa_StringA                sRemoteAdress          = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "CreateClient");

#if !OPCUA_MULTITHREADED
    if(a_hSocketManager == OpcUa_Null)
    {
        a_hSocketManager = OpcUa_Socket_g_SocketManager;
    }
#endif

    OpcUa_ReturnErrorIfArgumentNull(a_hSocketManager);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_sRemoteAddress);

    /* parse address */
    uStatus = OpcUa_P_ParseUrl( a_sRemoteAddress,
                                &sRemoteAdress,
                                &uPort);
    OpcUa_ReturnErrorIfBad(uStatus);

    pInternalSocketManager = (OpcUa_InternalSocketManager*)a_hSocketManager;

    pNewClientSocket = (OpcUa_InternalSocket*)OpcUa_SocketManager_FindFreeSocket(   (OpcUa_SocketManager)pInternalSocketManager,
                                                                                    OpcUa_False);
    if (pNewClientSocket == OpcUa_Null)
    {
        OpcUa_P_Memory_Free(sRemoteAdress);
        uStatus = OpcUa_BadMaxConnectionsReached; /* no socket left in the list. */
        goto Error;
    }

    /* Create client socket. */
    pNewClientSocket->rawSocket = OpcUa_P_Socket_CreateClient(  a_uLocalPort,
                                                                uPort,
                                                                sRemoteAdress,
                                                                &uStatus);

    OpcUa_P_Memory_Free(sRemoteAdress);

    OpcUa_GotoErrorIfTrue(  pNewClientSocket->rawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID,
                            OpcUa_BadCommunicationError);

    pNewClientSocket->pfnEventCallback       = a_pfnSocketCallBack;
    pNewClientSocket->pvUserData             = a_pCallbackData;
    pNewClientSocket->Flags.bOwnThread       = OpcUa_False;
    pNewClientSocket->Flags.EventMask        = OPCUA_SOCKET_READ_EVENT
                                             | OPCUA_SOCKET_EXCEPT_EVENT
                                             | OPCUA_SOCKET_CONNECT_EVENT
                                             | OPCUA_SOCKET_TIMEOUT_EVENT;

    /* return the new client socket */
    *a_pSocket = pNewClientSocket;

    OPCUA_SOCKET_SETVALID(pNewClientSocket);

    /* break loop to add new socket into eventing */
    uStatus = OpcUa_P_SocketManager_InterruptLoop(  a_hSocketManager,
                                                    OPCUA_SOCKET_RENEWLOOP_EVENT,
                                                    OpcUa_False);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pNewClientSocket != OpcUa_Null)
    {
        if(pNewClientSocket->rawSocket != (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
        {
            OpcUa_P_RawSocket_Close(pNewClientSocket->rawSocket);
            pNewClientSocket->rawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
        }
        OPCUA_SOCKET_INVALIDATE(pNewClientSocket);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get last socket error
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetLastError(OpcUa_Socket a_pSocket)
{
    OpcUa_SocketServiceTable** ppSocketServiceTable = (OpcUa_SocketServiceTable**)a_pSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    return (*ppSocketServiceTable)->SocketGetLastError(a_pSocket);
}

/*============================================================================
 * Signal a certain event on a socket.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_SignalEvent(   OpcUa_SocketManager     a_pSocketManager,
                                                                    OpcUa_UInt32            a_uEvent,
                                                                    OpcUa_Boolean           a_bAllManagers)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "SignalEvent");

    uStatus = OpcUa_P_SocketManager_InterruptLoop(a_pSocketManager, a_uEvent, a_bAllManagers);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Read Socket.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_Read( OpcUa_Socket    a_pSocket,
                                                    OpcUa_Byte*     a_pBuffer,
                                                    OpcUa_UInt32    a_nBufferSize,
                                                    OpcUa_UInt32*   a_pBytesRead)
{
    OpcUa_SocketServiceTable** ppSocketServiceTable = (OpcUa_SocketServiceTable**)a_pSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    return (*ppSocketServiceTable)->SocketRead(a_pSocket, a_pBuffer, a_nBufferSize, a_pBytesRead);
}

/*============================================================================
 * Write Socket.
 *===========================================================================*/
/* returns number of bytes written to the socket */
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_Socket_Write( OpcUa_Socket    a_pSocket,
                                                OpcUa_Byte*     a_pBuffer,
                                                OpcUa_UInt32    a_uBufferSize,
                                                OpcUa_Boolean   a_bBlock)
{
    OpcUa_SocketServiceTable** ppSocketServiceTable = (OpcUa_SocketServiceTable**)a_pSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    return (*ppSocketServiceTable)->SocketWrite(a_pSocket, a_pBuffer, a_uBufferSize, a_bBlock);
}

/*============================================================================
 * Close Socket.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_Close(OpcUa_Socket a_pSocket)
{
    OpcUa_SocketServiceTable** ppSocketServiceTable = (OpcUa_SocketServiceTable**)a_pSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    return (*ppSocketServiceTable)->SocketClose(a_pSocket);
}

/*============================================================================
 * Network Byte Order Conversion Helper Functions
 *===========================================================================*/
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_Socket_NToHL(OpcUa_UInt32 a_netLong)
{
    return OpcUa_P_RawSocket_NToHL(a_netLong);
}

OpcUa_UInt16 OPCUA_DLLCALL OpcUa_P_Socket_NToHS(OpcUa_UInt16 a_netShort)
{
    return OpcUa_P_RawSocket_NToHS(a_netShort);
}

OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_Socket_HToNL(OpcUa_UInt32 a_hstLong)
{
    return OpcUa_P_RawSocket_HToNL(a_hstLong);
}

OpcUa_UInt16 OPCUA_DLLCALL OpcUa_P_Socket_HToNS(OpcUa_UInt16 a_hstShort)
{
    return OpcUa_P_RawSocket_HToNS(a_hstShort);
}


/*============================================================================
 * Get IP Address and Port Number of the Peer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetPeerInfo(  OpcUa_Socket a_pSocket,
                                                            OpcUa_CharA* a_achPeerInfoBuffer,
                                                            OpcUa_UInt32 a_uiPeerInfoBufferSize)
{
    OpcUa_SocketServiceTable** ppSocketServiceTable = (OpcUa_SocketServiceTable**)a_pSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    return (*ppSocketServiceTable)->SocketGetPeerInfo(a_pSocket, a_achPeerInfoBuffer, a_uiPeerInfoBufferSize);
}

/*============================================================================
 * Convert OpcUa_StringA into binary ip address
 *===========================================================================*/
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_Socket_InetAddr(OpcUa_StringA a_sRemoteAddress)
{
    return OpcUa_P_RawSocket_InetAddr(a_sRemoteAddress);
}

/*============================================================================
 * Get the name of the local host.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetHostName( OpcUa_CharA* a_pBuffer,
                                                           OpcUa_UInt32 a_uiBufferLength)
{
    int iRet = gethostname( (char*)a_pBuffer,
                            (int)a_uiBufferLength);

    return (iRet==0)?OpcUa_Good:OpcUa_Bad;
}

/*============================================================================
 * Set socket user data
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_SetUserData( OpcUa_Socket a_pSocket,
                                                           OpcUa_Void*  a_pvUserData)
{
    OpcUa_SocketServiceTable** ppSocketServiceTable = (OpcUa_SocketServiceTable**)a_pSocket;

    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    return (*ppSocketServiceTable)->SocketSetUserData(a_pSocket, a_pvUserData);

}
