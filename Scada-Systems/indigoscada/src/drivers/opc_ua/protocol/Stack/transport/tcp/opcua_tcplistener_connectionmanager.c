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

#include <opcua.h>

#ifdef OPCUA_HAVE_SERVERAPI

#include <opcua_mutex.h>
#include <opcua_datetime.h>
#include <opcua_socket.h>
#include <opcua_statuscodes.h>
#include <opcua_guid.h>
#include <opcua_list.h>
#include <opcua_timer.h>

#include <opcua_tcpstream.h>

#include <opcua_tcplistener.h>
#include <opcua_tcplistener_connectionmanager.h>

/*============================================================================
 * Connection Manager Create
 *===========================================================================*/
/**
 * Create a new connection manager
 * @param a_ppConnectionManager Description
 * @return StatusCode
 */
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_Create(
    OpcUa_TcpListener_ConnectionManager** a_ppConnectionManager)
{
    OpcUa_StatusCode uStatus    = OpcUa_Good;
    OpcUa_TcpListener_ConnectionManager *pConnMngr  = OpcUa_Null;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    OpcUa_ReturnErrorIfArgumentNull(a_ppConnectionManager);

    *a_ppConnectionManager = OpcUa_Null;

    pConnMngr = (OpcUa_TcpListener_ConnectionManager*)OpcUa_Alloc(sizeof(OpcUa_TcpListener_ConnectionManager));
    OpcUa_ReturnErrorIfAllocFailed(pConnMngr);

    uStatus = OpcUa_TcpListener_ConnectionManager_Initialize(pConnMngr);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_TcpListener_ConnectionManager_Delete(&pConnMngr);
    }

    *a_ppConnectionManager = pConnMngr;
    return uStatus;
}


/*============================================================================
 * Connection Manager Initialize
 *===========================================================================*/
/**
 * @brief Initialize an allocated connection manager.
 */
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_Initialize(
    OpcUa_TcpListener_ConnectionManager* a_pConnectionManager)
{
    OpcUa_StatusCode uStatus    = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    if(a_pConnectionManager == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    OpcUa_MemSet(a_pConnectionManager, 0, sizeof(OpcUa_TcpListener_ConnectionManager));

    uStatus = OpcUa_List_Create(    &(a_pConnectionManager->Connections));
    OpcUa_ReturnErrorIfBad(uStatus);

    return uStatus;
}

/*============================================================================
 * Connection Manager Clear
 *===========================================================================*/
/**
 * @brief Clear a connection manager.
 */
OpcUa_Void OpcUa_TcpListener_ConnectionManager_Clear(
    OpcUa_TcpListener_ConnectionManager* a_pConnectionManager)
{
    if(a_pConnectionManager == OpcUa_Null)
    {
        return;
    }

    /* be sure to delete all connections before! */
    OpcUa_List_Delete(&(a_pConnectionManager->Connections));
}

/*============================================================================
 * Connection Manager Delete
 *===========================================================================*/
/**
 * @brief Delete a connection manager.
 */
OpcUa_Void OpcUa_TcpListener_ConnectionManager_Delete(
    OpcUa_TcpListener_ConnectionManager** a_ppConnectionManager)
{
    if(a_ppConnectionManager != OpcUa_Null)
    {
        OpcUa_TcpListener_ConnectionManager_Clear(*a_ppConnectionManager);
        OpcUa_Free(*a_ppConnectionManager);
        *a_ppConnectionManager = OpcUa_Null;
        return;
    }
}

/*==============================================================================*/
/* Get Connection                                                               */
/*==============================================================================*/
/**
 * @brief Retrieves a started stream for the connection identified by the socket.
 *
 * @return StatusCode
 */
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_GetConnectionBySocket(
    OpcUa_TcpListener_ConnectionManager*    a_pConnectionManager,
    OpcUa_Socket                            a_pSocket,
    OpcUa_TcpListener_Connection**          a_ppConnection)
{
    OpcUa_StatusCode                uStatus         = OpcUa_Good;
    OpcUa_TcpListener_Connection*   tmpConnection   = OpcUa_Null;

    OpcUa_ReturnErrorIfArgumentNull(a_pConnectionManager);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnectionManager->Connections);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_ppConnection);

    *a_ppConnection = OpcUa_Null;

    OpcUa_List_Enter(a_pConnectionManager->Connections);

    uStatus = OpcUa_BadNotFound;

    OpcUa_List_ResetCurrent(a_pConnectionManager->Connections);
    tmpConnection = (OpcUa_TcpListener_Connection*)OpcUa_List_GetCurrentElement(a_pConnectionManager->Connections);

    while(tmpConnection != OpcUa_Null)
    {
        if(a_pSocket == tmpConnection->Socket)
        {
            *a_ppConnection = tmpConnection;

            uStatus = OpcUa_Good;
            break;
        }
        tmpConnection = (OpcUa_TcpListener_Connection *)OpcUa_List_GetNextElement(a_pConnectionManager->Connections);
    }

    OpcUa_List_Leave(a_pConnectionManager->Connections);

    return uStatus;
}

/*==============================================================================*/
/*                                                                              */
/*==============================================================================*/
/**
* @brief Remove a connection identified by the connection object itself (if no id was assigned ie. pre validation)
*
* @return: Status Code;
*/
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_RemoveConnection(
    OpcUa_TcpListener_ConnectionManager*    a_pConnectionManager,
    OpcUa_TcpListener_Connection*           a_pConnection)
{
    OpcUa_StatusCode uStatus = OpcUa_Good;

    OpcUa_List_Enter(a_pConnectionManager->Connections);
    uStatus = OpcUa_List_DeleteElement(a_pConnectionManager->Connections, a_pConnection);
    OpcUa_List_Leave(a_pConnectionManager->Connections);

    return uStatus;
}


/*==============================================================================*/
/*                                                                              */
/*==============================================================================*/
/**
* @brief Builds a connection record for the given parameters and returns it.
*
* @return: Status Code;
*/
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_AddConnection(
    OpcUa_TcpListener_ConnectionManager*    a_pConnectionManager,
    OpcUa_TcpListener_Connection*           a_pConnection)
{
    OpcUa_StatusCode    uStatus = OpcUa_Good;

    OpcUa_GotoErrorIfArgumentNull(a_pConnection);
    OpcUa_GotoErrorIfArgumentNull(a_pConnectionManager);
    OpcUa_GotoErrorIfArgumentNull(a_pConnectionManager->Connections);

    a_pConnection->ConnectTime = OPCUA_P_DATETIME_UTCNOW(); /* expiration of connection would be DisconnectTime+Lifetime */

    OpcUa_List_Enter(a_pConnectionManager->Connections);
    uStatus = OpcUa_List_AddElement(a_pConnectionManager->Connections, a_pConnection);
    OpcUa_List_Leave(a_pConnectionManager->Connections);
    OpcUa_GotoErrorIfBad(uStatus);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ConnectionManager_AddConnection: Connection added!\n");

    return OpcUa_Good;

Error:
    return uStatus;
}

/*==============================================================================*/
/* Delete all connections                                                       */
/*==============================================================================*/
/* Iterates over all connections, closes and deletes each. The given callback is called everytime. */
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_RemoveConnections(
    OpcUa_TcpListener_ConnectionManager* a_pConnectionManager,
    OpcUa_TcpListener_ConnectionDeleteCB a_fConnectionDeleteCB)
{
    OpcUa_StatusCode                uStatus         = OpcUa_Good;
    OpcUa_TcpListener_Connection*   tcpConnection   = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    OpcUa_ReturnErrorIfArgumentNull(a_pConnectionManager);
    /*OpcUa_ReturnErrorIfArgumentNull(a_fConnectionDeleteCB);*/

    /* obtain lock on the list */
    OpcUa_List_Enter(a_pConnectionManager->Connections);

    /* prepare cursor for iteration over all elements */
    OpcUa_List_ResetCurrent(a_pConnectionManager->Connections);
    tcpConnection = (OpcUa_TcpListener_Connection*)OpcUa_List_GetCurrentElement(a_pConnectionManager->Connections);

    /* check every Connection for deletion */
    while(tcpConnection != OpcUa_Null)
    {
        if(a_fConnectionDeleteCB != OpcUa_Null)
        {
            a_fConnectionDeleteCB(  a_pConnectionManager->Listener,
                                    tcpConnection);
        }

        OpcUa_TcpListener_Connection_Delete((&tcpConnection));
        OpcUa_List_DeleteCurrentElement(a_pConnectionManager->Connections);
        tcpConnection = (OpcUa_TcpListener_Connection*)OpcUa_List_GetCurrentElement(a_pConnectionManager->Connections);
    }

    /* list must be empty here */

    /* leave it */
    OpcUa_List_Leave(a_pConnectionManager->Connections);

    return uStatus;
}

/***  TcpListener_Connection Class  ***/


/*===========================================================================*/
/* Create                                                                    */
/*===========================================================================*/
/**
* @brief Allocate and initialize a new OpcUa_TcpListener_Connection object.
*
* @return Bad status on fail; Good status on success;
*/
OpcUa_StatusCode OpcUa_TcpListener_Connection_Create(OpcUa_TcpListener_Connection** a_ppConnection)
{
    OpcUa_TcpListener_Connection*   pConnection = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    pConnection = (OpcUa_TcpListener_Connection*) OpcUa_Alloc(sizeof(OpcUa_TcpListener_Connection));
    OpcUa_ReturnErrorIfAllocFailed(pConnection);
    OpcUa_MemSet(pConnection, 0, sizeof(OpcUa_TcpListener_Connection));

    pConnection->uCurrentChunk  = 0;

    OpcUa_TcpListener_Connection_Initialize(pConnection);

    *a_ppConnection = pConnection;
    return OpcUa_Good;
}

/*==============================================================================*/
/* Initialize                                                                   */
/*==============================================================================*/
/**
* @brief Initializes the given parameter and allocates embedded objects.
* If embedded objects cannot be allocated, the given pointer is set to OpcUa_Null.
*
* @return Bad status on fail; Good status on success;
*/
OpcUa_StatusCode OpcUa_TcpListener_Connection_Initialize(OpcUa_TcpListener_Connection* a_pConnection)
{
    OpcUa_StatusCode uStatus    = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);

    OpcUa_MemSet(&(a_pConnection->ConnectTime), 0, sizeof(OpcUa_DateTime));
    OpcUa_MemSet(&(a_pConnection->DisconnectTime), 0, sizeof(OpcUa_DateTime));

    a_pConnection->Socket                   = OpcUa_Null;

#if OPCUA_P_SOCKETGETPEERINFO_V2
    OpcUa_MemSet(a_pConnection->achPeerInfo, 0, OPCUA_P_PEERINFO_MIN_SIZE);
#else /* OPCUA_P_SOCKETGETPEERINFO_V2 */
    a_pConnection->PeerPort                 = 0;
    a_pConnection->PeerIp                   = 0;
#endif /* OPCUA_P_SOCKETGETPEERINFO_V2 */

    a_pConnection->pListenerHandle          = OpcUa_Null;
    a_pConnection->pInputStream             = OpcUa_Null;
    a_pConnection->uNoOfRequestsTotal       = 0;
    a_pConnection->bConnected               = OpcUa_False;

    OpcUa_String_Initialize(&a_pConnection->sURL);

    uStatus = OPCUA_P_MUTEX_CREATE(&(a_pConnection->Mutex));
    OpcUa_ReturnErrorIfBad(uStatus);

    a_pConnection->ReceiveBufferSize        = 0;
    a_pConnection->SendBufferSize           = 0;
    a_pConnection->pSendQueue               = OpcUa_Null;
    a_pConnection->bCloseWhenDone           = OpcUa_False;
    a_pConnection->bNoRcvUntilDone          = OpcUa_False;
    a_pConnection->bRcvDataPending          = OpcUa_False;

    return OpcUa_Good;
}

/*==============================================================================*/
/* Clear                                                                        */
/*==============================================================================*/
/**
* @brief Clean up the given OpcUa_TcpListener_Connection. May be used for static memory.
*/
OpcUa_Void OpcUa_TcpListener_Connection_Clear(OpcUa_TcpListener_Connection* a_pConnection)
{
    if(a_pConnection == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemSet(&(a_pConnection->ConnectTime), 0, sizeof(OpcUa_DateTime));
    OpcUa_MemSet(&(a_pConnection->DisconnectTime), 0, sizeof(OpcUa_DateTime));

    a_pConnection->Socket                   = OpcUa_Null; /* Socket has to be freed externally. */
    a_pConnection->pListenerHandle          = OpcUa_Null;
    a_pConnection->uNoOfRequestsTotal       = 0;

    OpcUa_String_Clear(&a_pConnection->sURL);

    if(a_pConnection->pInputStream != OpcUa_Null)
    {
        OpcUa_TcpStream_Close((OpcUa_Stream*)a_pConnection->pInputStream);
        OpcUa_TcpStream_Delete((OpcUa_Stream**)&a_pConnection->pInputStream);
    }

    while(a_pConnection->pSendQueue != OpcUa_Null)
    {
        OpcUa_BufferList* pCurrentBuffer = a_pConnection->pSendQueue;
        a_pConnection->pSendQueue = pCurrentBuffer->pNext;
        OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
        OpcUa_Free(pCurrentBuffer);
    }

    if(a_pConnection->Mutex)
    {
        OPCUA_P_MUTEX_DELETE(&(a_pConnection->Mutex));
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_Connection_Clear: Done!\n");
}

/*==============================================================================*/
/* Delete                                                                       */
/*==============================================================================*/
/**
* @brief Clean up and free the given OpcUa_TcpListener_Connection.
*/
OpcUa_Void OpcUa_TcpListener_Connection_Delete(OpcUa_TcpListener_Connection** a_ppConnection)
{
    if(a_ppConnection == OpcUa_Null || *a_ppConnection == OpcUa_Null)
    {
        return;
    }

    /* clean up internal resources */
    OpcUa_TcpListener_Connection_Clear(*a_ppConnection);

    /* free instance memory */
    OpcUa_Free(*a_ppConnection);
    *a_ppConnection = OpcUa_Null;
}

/*==============================================================================*/
/* Delete                                                                       */
/*==============================================================================*/
/**
* @brief .
*/
OpcUa_StatusCode OpcUa_TcpListener_ConnectionManager_GetConnectionCount(
    OpcUa_TcpListener_ConnectionManager*    a_pConnectionManager,
    OpcUa_UInt32*                           a_pNoOfConnections)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "GetConnectionCount");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnectionManager);
    OpcUa_ReturnErrorIfArgumentNull(a_pNoOfConnections);

    OpcUa_List_GetNumberOfElements(a_pConnectionManager->Connections, a_pNoOfConnections);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* nothing */

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_SERVERAPI */

/*==============================================================================*/
/* End Of File                                                                  */
/*==============================================================================*/
