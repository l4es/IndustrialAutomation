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
#include <opcua_string.h>
#include <opcua_datetime.h>
#include <opcua_socket.h>
#include <opcua_statuscodes.h>
#include <opcua_list.h>
#include <opcua_utilities.h>

#include <opcua_tcpstream.h>
#include <opcua_binaryencoder.h>

#include <opcua_tcplistener.h>

typedef struct _OpcUa_TcpListener OpcUa_TcpListener;

#include <opcua_tcplistener_connectionmanager.h>

/* for debugging reasons */
#include <opcua_p_binary.h>
#include <opcua_memorystream.h>

extern OpcUa_Guid OpcUa_Guid_Null;

/*============================================================================
 * Prototypes
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_Open(
    struct _OpcUa_Listener*         a_Listener,
    OpcUa_String*                   a_Url,
    OpcUa_Boolean                   a_bListenOnAllInterfaces,
    OpcUa_Listener_PfnOnNotify*     a_Callback,
    OpcUa_Void*                     a_CallbackData);

OpcUa_StatusCode OpcUa_TcpListener_Close(
    OpcUa_Listener*                 a_Listener);

OpcUa_StatusCode OpcUa_TcpListener_BeginSendResponse(
    OpcUa_Listener*                 a_Listener,
    OpcUa_Handle                    a_pConnection,
    OpcUa_InputStream**             a_istrm,
    OpcUa_OutputStream**            a_ostrm);

OpcUa_StatusCode OpcUa_TcpListener_EndSendResponse(
    struct _OpcUa_Listener*         a_Listener,
    OpcUa_StatusCode                a_uStatus,
    OpcUa_OutputStream**            a_ostrm);

OpcUa_StatusCode OpcUa_TcpListener_AbortSendResponse(
    struct _OpcUa_Listener*         a_Listener,
    OpcUa_StatusCode                a_uStatus,
    OpcUa_String*                   a_psReason,
    OpcUa_OutputStream**            a_ostrm);

OpcUa_StatusCode OpcUa_TcpListener_CloseConnection(
    struct _OpcUa_Listener*         a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_StatusCode                a_uStatus);

OpcUa_StatusCode OpcUa_TcpListener_ProcessDisconnect(
    OpcUa_Listener*                 a_pListener,
    OpcUa_TcpListener_Connection*   a_pTcpConnection);

static OpcUa_StatusCode OpcUa_TcpListener_SendErrorMessage(
    OpcUa_Listener*                 a_pListener,
    OpcUa_TcpListener_Connection*   a_pTcpConnection,
    OpcUa_StatusCode                a_uStatus,
    OpcUa_String*                   a_sReason);

OpcUa_StatusCode OpcUa_TcpListener_GetReceiveBufferSize(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_UInt32*                   a_pBufferSize);

OpcUa_StatusCode OpcUa_TcpListener_GetPeerInfo(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_String*                   a_sPeerInfo);

OpcUa_StatusCode OpcUa_TcpListener_AddToSendQueue(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_BufferList*               a_pBufferList,
    OpcUa_UInt32                    a_uFlags);

OpcUa_StatusCode OpcUa_TcpListener_CheckProtocolVersion(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_UInt32                    a_uProtocolVersion);

/*============================================================================
 * OpcUa_TcpListener_SanityCheck
 *===========================================================================*/
#define OpcUa_TcpListener_SanityCheck 0xE339EF96

/*============================================================================
 * OpcUa_TcpListener
 *===========================================================================*/
 /** @brief This struct represents a listener for tcp transport. */
struct _OpcUa_TcpListener
{
/* This is inherited from the OpcUa_Listener. */

    /** @brief The base class. */
    OpcUa_Listener              Base;

/* End inherited from the OpcUa_Listener. */

    /** @brief Internal control value. */
    OpcUa_UInt32                SanityCheck;
    /** @brief Synchronize access to the listener. */
    OpcUa_Mutex                 Mutex;
    /** @brief The listen socket (either part of the global or the own socket list). */
    OpcUa_Socket                Socket;
#if OPCUA_MULTITHREADED
    /** @brief In multithreaded environments, each listener manages its own list of sockets. */
    OpcUa_SocketManager         SocketManager;
#endif /* OPCUA_MULTITHREADED */
    /** @brief The function which receives notifications about listener events. */
    OpcUa_Listener_PfnOnNotify* Callback;
    /** @brief Data passed with the callback function. */
    OpcUa_Void*                 CallbackData;
    /** @brief The default message chunk size for communicating with this listener. */
    OpcUa_UInt32                DefaultChunkSize;
    /** @brief This list contains all pending requests, which are not fully received
     *  yet. Once a request is completely received, it gets dispatched to the
     *  upper layer. */
    OpcUa_List*                 PendingMessages;
    /** @brief Holds the information about connected clients and helps verifying requests. */
    OpcUa_TcpListener_ConnectionManager* ConnectionManager;
};


/*============================================================================
 * OpcUa_TcpListener_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_TcpListener_Delete(OpcUa_Listener** a_ppListener)
{
    OpcUa_TcpListener* pTcpListener = OpcUa_Null;
    OpcUa_InputStream* pInputStream = OpcUa_Null;

    if(a_ppListener == OpcUa_Null || *a_ppListener == OpcUa_Null)
    {
        return;
    }

    pTcpListener = (OpcUa_TcpListener*)(*a_ppListener)->Handle;

    if(pTcpListener != OpcUa_Null)
    {
        OPCUA_P_MUTEX_LOCK(pTcpListener->Mutex);
        pTcpListener->SanityCheck = 0;

        /* delete all pending messages */
        OpcUa_List_Enter(pTcpListener->PendingMessages);
        OpcUa_List_ResetCurrent(pTcpListener->PendingMessages);
        pInputStream = (OpcUa_InputStream *)OpcUa_List_GetCurrentElement(pTcpListener->PendingMessages);
        while(pInputStream != OpcUa_Null)
        {
            OpcUa_List_DeleteCurrentElement(pTcpListener->PendingMessages);
            pInputStream->Close((OpcUa_Stream*)pInputStream);
            pInputStream->Delete((OpcUa_Stream**)&pInputStream);
            pInputStream = (OpcUa_InputStream *)OpcUa_List_GetCurrentElement(pTcpListener->PendingMessages);
        }
        OpcUa_List_Leave(pTcpListener->PendingMessages);
        OpcUa_List_Delete(&(pTcpListener->PendingMessages));

        OpcUa_TcpListener_ConnectionManager_Delete(&(pTcpListener->ConnectionManager));

        OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);
        OPCUA_P_MUTEX_DELETE(&(pTcpListener->Mutex));

        OpcUa_Free(pTcpListener);
    }

    *a_ppListener = OpcUa_Null;
}

/*============================================================================
 * OpcUa_TcpListener_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_Create(OpcUa_Listener** a_pListener)
{
    OpcUa_TcpListener*  pTcpListener = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);

    /* allocate listener object */
    *a_pListener = (OpcUa_Listener*)OpcUa_Alloc(sizeof(OpcUa_TcpListener));
    OpcUa_GotoErrorIfAllocFailed(*a_pListener);
    OpcUa_MemSet(*a_pListener, 0, sizeof(OpcUa_TcpListener));
    pTcpListener = (OpcUa_TcpListener*)*a_pListener;

    /* initialize listener pTcpListener */
    pTcpListener->SanityCheck = OpcUa_TcpListener_SanityCheck;
    pTcpListener->DefaultChunkSize = OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize;

    uStatus = OPCUA_P_MUTEX_CREATE(&(pTcpListener->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_List_Create(&(pTcpListener->PendingMessages));
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_TcpListener_ConnectionManager_Create(&(pTcpListener->ConnectionManager));
    OpcUa_GotoErrorIfBad(uStatus);
    pTcpListener->ConnectionManager->Listener = *a_pListener;

    /* HINT: socket and socket list get managed in open/close */

    /* initialize listener object */
    (*a_pListener)->Handle                  = pTcpListener;
    (*a_pListener)->Open                    = OpcUa_TcpListener_Open;
    (*a_pListener)->Close                   = OpcUa_TcpListener_Close;
    (*a_pListener)->BeginSendResponse       = OpcUa_TcpListener_BeginSendResponse;
    (*a_pListener)->EndSendResponse         = OpcUa_TcpListener_EndSendResponse;
    (*a_pListener)->AbortSendResponse       = OpcUa_TcpListener_AbortSendResponse;
    (*a_pListener)->CloseConnection         = OpcUa_TcpListener_CloseConnection;
    (*a_pListener)->GetReceiveBufferSize    = OpcUa_TcpListener_GetReceiveBufferSize;
    (*a_pListener)->Delete                  = OpcUa_TcpListener_Delete;
    (*a_pListener)->AddToSendQueue          = OpcUa_TcpListener_AddToSendQueue;
    (*a_pListener)->GetPeerInfo             = OpcUa_TcpListener_GetPeerInfo;
    (*a_pListener)->CheckProtocolVersion    = OpcUa_TcpListener_CheckProtocolVersion;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(*a_pListener != OpcUa_Null)
    {
        OpcUa_TcpListener_ConnectionManager_Delete(&(pTcpListener->ConnectionManager));
        OpcUa_List_Delete(&(pTcpListener->PendingMessages));
        OPCUA_P_MUTEX_DELETE(&(pTcpListener->Mutex));
        OpcUa_Free(*a_pListener);
        *a_pListener = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_GetReceiveBufferSize
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_GetReceiveBufferSize(OpcUa_Listener*     a_pListener,
                                                        OpcUa_Handle        a_hConnection,
                                                        OpcUa_UInt32*       a_pBufferSize)
{
    OpcUa_TcpListener_Connection* pTcpListenerConnection = (OpcUa_TcpListener_Connection*)a_hConnection;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "GetReceiveBufferSize");

    OpcUa_ReturnErrorIfArgumentNull(a_hConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pBufferSize);

    OpcUa_ReferenceParameter(a_pListener);

    *a_pBufferSize = pTcpListenerConnection->ReceiveBufferSize;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_GetPeerInfo
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_GetPeerInfo(OpcUa_Listener*     a_pListener,
                                               OpcUa_Handle        a_hConnection,
                                               OpcUa_String*       a_pPeerInfo)
{
    OpcUa_TcpListener_Connection* pTcpListenerConnection = (OpcUa_TcpListener_Connection*)a_hConnection;
    /* return peer information in format "opc.tcp://xxx.xxx.xxx.xxx:ppppp\0" => max length == 32 */
    /* return peer information in format "opc.tcp://1234:5678:1234:5678:1234:5678:1234:5678:ppppp\0" => max length == 56 */
    OpcUa_CharA pRawString[74];

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "GetPeerInfo");

    OpcUa_ReferenceParameter(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pPeerInfo);

    OpcUa_SPrintfA( pRawString,
#if OPCUA_USE_SAFE_FUNCTIONS
                    (sizeof(pRawString) / sizeof(pRawString[0])),
#endif /* OPCUA_USE_SAFE_FUNCTIONS */
#if OPCUA_P_SOCKETGETPEERINFO_V2
                    "opc.tcp://%s",
                    pTcpListenerConnection->achPeerInfo);
#else
                    "opc.tcp://%u.%u.%u.%u:%u",
                    (pTcpListenerConnection->PeerIp >> 24) & 0xFF,
                    (pTcpListenerConnection->PeerIp >> 16) & 0xFF,
                    (pTcpListenerConnection->PeerIp >> 8) & 0xFF,
                    pTcpListenerConnection->PeerIp & 0xFF,
                    pTcpListenerConnection->PeerPort);
#endif

    uStatus = OpcUa_String_StrnCpy( a_pPeerInfo,
                                    OpcUa_String_FromCString(pRawString),
                                    OPCUA_STRING_LENDONTCARE);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_AddToSendQueue
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_AddToSendQueue(OpcUa_Listener*   a_pListener,
                                                  OpcUa_Handle      a_hConnection,
                                                  OpcUa_BufferList* a_pBufferList,
                                                  OpcUa_UInt32      a_uFlags)
{
    OpcUa_TcpListener_Connection* pTcpListenerConnection = (OpcUa_TcpListener_Connection*)a_hConnection;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "AddToSendQueue");

    OpcUa_ReturnErrorIfArgumentNull(a_hConnection);
    OpcUa_ReferenceParameter(a_pListener);

    if(pTcpListenerConnection->pSendQueue == OpcUa_Null)
    {
        pTcpListenerConnection->pSendQueue = a_pBufferList;
    }
    else
    {
        OpcUa_BufferList* pLastEntry = pTcpListenerConnection->pSendQueue;
        while(pLastEntry->pNext != OpcUa_Null)
        {
            pLastEntry = pLastEntry->pNext;
        }
        pLastEntry->pNext = a_pBufferList;
    }

    if(a_uFlags & OPCUA_LISTENER_CLOSE_WHEN_DONE)
    {
        pTcpListenerConnection->bCloseWhenDone = OpcUa_True;
    }

    if(a_uFlags & OPCUA_LISTENER_NO_RCV_UNTIL_DONE)
    {
        pTcpListenerConnection->bNoRcvUntilDone = OpcUa_True;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_CheckProtocolVersion
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_CheckProtocolVersion(OpcUa_Listener*   a_pListener,
                                                        OpcUa_Handle      a_hConnection,
                                                        OpcUa_UInt32      a_uProtocolVersion)
{
    OpcUa_TcpListener_Connection* pTcpListenerConnection = (OpcUa_TcpListener_Connection*)a_hConnection;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "CheckProtocolVersion");

    OpcUa_ReturnErrorIfArgumentNull(a_hConnection);
    OpcUa_ReferenceParameter(a_pListener);

    if(a_uProtocolVersion != pTcpListenerConnection->uProtocolVersion)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadProtocolVersionUnsupported);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_CloseConnection
 *===========================================================================*/
/** @brief Close a particular connection of this listener. */
OpcUa_StatusCode OpcUa_TcpListener_CloseConnection( OpcUa_Listener*     a_pListener,
                                                    OpcUa_Handle        a_hConnection,
                                                    OpcUa_StatusCode    a_uStatus)
{
    OpcUa_TcpListener_Connection*   pTcpListenerConnection  = (OpcUa_TcpListener_Connection*)a_hConnection;
    OpcUa_TcpListener*              pTcpListener            = (OpcUa_TcpListener*)a_pListener->Handle;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "CloseConnection");

    OpcUa_ReturnErrorIfArgumentNull(a_hConnection);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpListener_CloseConnection: Connection %p is being closed! 0x%08X\n", a_hConnection, a_uStatus);

    if(OpcUa_IsBad(a_uStatus) && a_uStatus != OpcUa_BadDisconnect)
    {
        uStatus = OpcUa_TcpListener_SendErrorMessage(   a_pListener,
                                                        pTcpListenerConnection,
                                                        a_uStatus,
                                                        OpcUa_Null);
    }

    if(pTcpListenerConnection->pSendQueue != OpcUa_Null)
    {
        pTcpListenerConnection->bCloseWhenDone = OpcUa_True;
        OpcUa_ReturnStatusCode;
    }

    uStatus = OpcUa_TcpListener_ConnectionManager_RemoveConnection( pTcpListener->ConnectionManager,
                                                                    (OpcUa_TcpListener_Connection*)a_hConnection);

    if(OpcUa_IsGood(uStatus))
    {
        uStatus = OPCUA_P_SOCKET_CLOSE(pTcpListenerConnection->Socket);
        OpcUa_TcpListener_Connection_Delete(&pTcpListenerConnection);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpListener_CloseConnection: Error 0x%08X removing connection %p! \n", a_uStatus, a_hConnection);
    }


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_ConnectionDisconnectCB
 *===========================================================================*/
/** @brief Gets called by an outstream if the connection is lost. */
static OpcUa_Void OpcUa_TcpListener_ConnectionDisconnectCB(OpcUa_Handle a_hConnection)
{
    OpcUa_Listener*                 pListener               = OpcUa_Null;
    OpcUa_TcpListener_Connection*   pTcpListenerConnection  = (OpcUa_TcpListener_Connection*)a_hConnection;

    OpcUa_ReferenceParameter(pTcpListenerConnection);
    OpcUa_ReferenceParameter(pListener);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpListener_ConnectionDisconnectCB: Connection %p is being reported as disconnected!\n", a_hConnection);
}

/*============================================================================
 * OpcUa_TcpListener_BeginSendResponse
 *===========================================================================*/
/* prepare a response (out) stream for a certain connection and related to   */
/* a certain request (in) stream                                             */

OpcUa_StatusCode OpcUa_TcpListener_BeginSendResponse(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_pConnection,
    OpcUa_InputStream**             a_ppTransportIStrm,
    OpcUa_OutputStream**            a_ppOstrm)
{
    OpcUa_TcpListener_Connection*   pTcpListenerConnection  = (OpcUa_TcpListener_Connection*)a_pConnection;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "BeginSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIStrm);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);

    OpcUa_ReturnErrorIfArgumentNull(a_pListener->BeginSendResponse);

    /* initialize outparameter */
    *a_ppOstrm = OpcUa_Null;

    /* close and delete the incoming stream - double close is ignored (uncritical) */
    (*a_ppTransportIStrm)->Close((OpcUa_Stream*)(*a_ppTransportIStrm));
    (*a_ppTransportIStrm)->Delete((OpcUa_Stream**)a_ppTransportIStrm);

    /* create buffer for writing */
    uStatus = OpcUa_TcpStream_CreateOutput( pTcpListenerConnection->Socket,            /* create stream on that socket */
                                            OpcUa_TcpStream_MessageType_SecureChannel, /* initialize as chunk */
                                            OpcUa_Null,                                /* no buffer to attach */
                                            pTcpListenerConnection->SendBufferSize,    /* flush border size */
                                            OpcUa_TcpListener_ConnectionDisconnectCB,  /* function to call, if stream detects disconnect */
                                            pTcpListenerConnection->MaxChunkCount,     /* maximum number of chunks allowed */
                                            a_ppOstrm);                                /* use that handle */
    OpcUa_ReturnErrorIfBad(uStatus);

    ((OpcUa_TcpOutputStream*)((*a_ppOstrm)->Handle))->hConnection = a_pConnection;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Send an Error message.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_TcpListener_SendErrorMessage( OpcUa_Listener*                 a_pListener,
                                                            OpcUa_TcpListener_Connection*   a_pTcpConnection,
                                                            OpcUa_StatusCode                a_uStatus,
                                                            OpcUa_String*                   a_sReason)
{
    OpcUa_TcpListener*      pTcpListener        = OpcUa_Null;
    OpcUa_OutputStream*     pOutputStream       = OpcUa_Null;
    OpcUa_String            sReason             = OPCUA_STRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "SendErrorMessage");

    OpcUa_GotoErrorIfArgumentNull(a_pListener);
    OpcUa_GotoErrorIfArgumentNull(a_pTcpConnection);

    pTcpListener = (OpcUa_TcpListener*)a_pListener;

    if(a_pTcpConnection->bConnected == OpcUa_False)
    {
        OpcUa_ReturnStatusCode;
    }

#if OPCUA_P_SOCKETGETPEERINFO_V2
    OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM,
                "OpcUa_TcpListener_SendErrorMessage: to %s (socket %p) with StatusCode 0x%08X\n",
                a_pTcpConnection->achPeerInfo,
                a_pTcpConnection->Socket,
                a_uStatus);
#else
    OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM,
                "OpcUa_TcpListener_SendErrorMessage: to %d.%d.%d.%d:%d (socket %p) with StatusCode 0x%08X\n",
                (OpcUa_Int)(a_pTcpConnection->PeerIp>>24)&0xFF,
                (OpcUa_Int)(a_pTcpConnection->PeerIp>>16)&0xFF,
                (OpcUa_Int)(a_pTcpConnection->PeerIp>>8) &0xFF,
                (OpcUa_Int) a_pTcpConnection->PeerIp     &0xFF,
                a_pTcpConnection->PeerPort,
                a_pTcpConnection->Socket,
                a_uStatus);
#endif

    /* create the output stream for the errormessage */
    uStatus = OpcUa_TcpStream_CreateOutput( a_pTcpConnection->Socket,
                                            OpcUa_TcpStream_MessageType_Error,
                                            OpcUa_Null,
                                            pTcpListener->DefaultChunkSize,
                                            OpcUa_TcpListener_ConnectionDisconnectCB,
                                            a_pTcpConnection->MaxChunkCount,
                                            &pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* encode the body of an Error message */

    /* status code */
    uStatus = OpcUa_UInt32_BinaryEncode(a_uStatus, pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_String_BinaryEncode(a_sReason?a_sReason:&sReason, pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    if(a_pTcpConnection->pSendQueue == OpcUa_Null)
    {
        uStatus = pOutputStream->Flush(pOutputStream, OpcUa_True);
    }

    if(a_pTcpConnection->pSendQueue != OpcUa_Null || OpcUa_IsEqual(OpcUa_BadWouldBlock))
    {
        OpcUa_Buffer      Buffer;
        OpcUa_BufferList* pBufferList = OpcUa_Alloc(sizeof(OpcUa_BufferList));
        OpcUa_GotoErrorIfAllocFailed(pBufferList);
        uStatus = pOutputStream->DetachBuffer((OpcUa_Stream*)pOutputStream, &Buffer);
        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Free(pBufferList);
            OpcUa_GotoError;
        }
        pBufferList->Buffer = Buffer;
        pBufferList->Buffer.Data = OpcUa_Alloc(pBufferList->Buffer.Size);
        pBufferList->Buffer.FreeBuffer = OpcUa_True;
        pBufferList->pNext = OpcUa_Null;
        if(pBufferList->Buffer.Data == OpcUa_Null)
        {
            OpcUa_Free(pBufferList);
            OpcUa_Buffer_Clear(&Buffer);
            OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);
        }
        if(a_pTcpConnection->pSendQueue != OpcUa_Null)
        {
            pBufferList->Buffer.EndOfData = pBufferList->Buffer.Position;
            pBufferList->Buffer.Position  = 0;
        }
        OpcUa_MemCpy(pBufferList->Buffer.Data, pBufferList->Buffer.EndOfData,
                     Buffer.Data, Buffer.EndOfData);
        uStatus = OpcUa_Listener_AddToSendQueue(
            a_pListener,
            a_pTcpConnection,
            pBufferList,
            0);
        OpcUa_Buffer_Clear(&Buffer);
    }
    OpcUa_GotoErrorIfBad(uStatus);

    /* finish stream and delete it */
    uStatus = pOutputStream->Close((OpcUa_Stream*)pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);
    pOutputStream->Delete((OpcUa_Stream**)&pOutputStream);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if((pOutputStream != OpcUa_Null) && (pOutputStream->Delete != OpcUa_Null))
    {
        pOutputStream->Delete((OpcUa_Stream**)&pOutputStream);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_EndSendResponse
 *===========================================================================*/
/* a bad status means, that the operation is to be abandoned */
OpcUa_StatusCode OpcUa_TcpListener_EndSendResponse(
    struct _OpcUa_Listener* a_pListener,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_OutputStream**    a_ppOstrm)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "OpcUa_TcpListener_EndSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOstrm);

    OpcUa_ReturnErrorIfArgumentNull(a_pListener->EndSendResponse);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_EndSendResponse: Status 0x%08X\n", a_uStatus);

    /* trigger error message */
    if(OpcUa_IsGood(a_uStatus))
    {
        /* close stream (flushes the Content on the wire). */
        uStatus = (*a_ppOstrm)->Close((OpcUa_Stream*)*a_ppOstrm);
    }

    /* delete without flushing and decrement request count */
    OpcUa_TcpStream_Delete((OpcUa_Stream**)a_ppOstrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_AbortSendResponse
 *===========================================================================*/
/* a bad status means, that the operation is to be abandoned */
OpcUa_StatusCode OpcUa_TcpListener_AbortSendResponse(
    struct _OpcUa_Listener* a_pListener,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_String*           a_psReason,
    OpcUa_OutputStream**    a_ppOutputStream)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "OpcUa_TcpListener_AbortSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);

    OpcUa_ReturnErrorIfArgumentNull(a_pListener->AbortSendResponse);

    if(a_ppOutputStream != OpcUa_Null)
    {
        /* clean up */
        OpcUa_TcpStream_Delete((OpcUa_Stream**)a_ppOutputStream);
    }
    else
    {
        /* no insecure abort messages implemented and allowed! */
        OpcUa_ReferenceParameter(a_uStatus);
        OpcUa_ReferenceParameter(a_psReason);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_LookForPendingMessage
 *===========================================================================*/
/* should be handled by the connection manager, since no interleaving is possible by design! */
OpcUa_StatusCode OpcUa_TcpListener_LookForPendingMessage(   OpcUa_TcpListener*  a_pTcpListener,
                                                            OpcUa_Socket        a_pSocket,
                                                            OpcUa_InputStream** a_pInputStream)
{
    OpcUa_InputStream*      pInputStream    = OpcUa_Null;
    OpcUa_TcpInputStream*   pTcpInputStream = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    OpcUa_ReturnErrorIfArgumentNull(a_pTcpListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);

    OpcUa_List_Enter(a_pTcpListener->PendingMessages);

    *a_pInputStream = OpcUa_Null;

    OpcUa_List_ResetCurrent(a_pTcpListener->PendingMessages);
    pInputStream = (OpcUa_InputStream*)OpcUa_List_GetCurrentElement(a_pTcpListener->PendingMessages);

    while(pInputStream != OpcUa_Null)
    {
        pTcpInputStream = (OpcUa_TcpInputStream *)pInputStream->Handle;

        if(pTcpInputStream != OpcUa_Null && pTcpInputStream->Socket == a_pSocket)
        {
            /* found */
            OpcUa_List_DeleteElement(a_pTcpListener->PendingMessages, (OpcUa_Void*)pInputStream);
            *a_pInputStream = pInputStream;
            OpcUa_List_Leave(a_pTcpListener->PendingMessages);
            return OpcUa_Good;
        }
        else
        {
            /* get next element */
            pInputStream = (OpcUa_InputStream*)OpcUa_List_GetNextElement(a_pTcpListener->PendingMessages);
        }
    }

    OpcUa_List_Leave(a_pTcpListener->PendingMessages);

    return OpcUa_BadNotFound;
}

/*============================================================================
 * OpcUa_TcpListener_ProcessRequest
 *===========================================================================*/
/**
* @brief Handles a UATM (Request), UATC (Request Chunk) message.
*/
OpcUa_StatusCode OpcUa_TcpListener_ProcessRequest(
    OpcUa_Listener*                 a_pListener,
    OpcUa_TcpListener_Connection*   a_pTcpConnection,
    OpcUa_InputStream**             a_ppInputStream)
{
    OpcUa_TcpListener*      pTcpListener    = OpcUa_Null;
    OpcUa_TcpInputStream*   pTcpInputStream = OpcUa_Null;
    OpcUa_ListenerEvent     eEvent          = OpcUa_ListenerEvent_Invalid;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "ProcessRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_ppInputStream);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppInputStream);

    pTcpListener     = (OpcUa_TcpListener*)a_pListener->Handle;
    pTcpInputStream  = (OpcUa_TcpInputStream*)(*a_ppInputStream)->Handle;

    if(pTcpInputStream->IsAbort != OpcUa_False)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpListener_ProcessRequest: Message aborted after %u received chunks while %u are allowed!\n", a_pTcpConnection->uCurrentChunk, a_pTcpConnection->MaxChunkCount);
        eEvent = OpcUa_ListenerEvent_RequestAbort;
        a_pTcpConnection->uCurrentChunk = 0;
    }
    else
    {
        if(pTcpInputStream->IsFinal != OpcUa_False)
        {
            /* last chunk in message, reset counter */
            eEvent = OpcUa_ListenerEvent_Request;
            a_pTcpConnection->uCurrentChunk = 0;
        }
        else
        {
            /* intermediary chunk, test for limit and increment */
            a_pTcpConnection->uCurrentChunk++;
            eEvent = OpcUa_ListenerEvent_RequestPartial;
            if((a_pTcpConnection->MaxChunkCount != 0) && (a_pTcpConnection->uCurrentChunk >= a_pTcpConnection->MaxChunkCount))
            {
                /* this message will be too large */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpListener_ProcessRequest: Chunk count limit exceeded!\n");
                eEvent = OpcUa_ListenerEvent_Request;  /* message final */
                uStatus = OpcUa_BadTcpMessageTooLarge; /* with error */
            }
        }
    }

    if(OpcUa_IsGood(uStatus))
    {
        /* send notification that request is ready to be read. */
        /* this call goes most probably to the secure channel handler. */
        if(pTcpListener->Callback != OpcUa_Null)
        {
            a_pTcpConnection->uNoOfRequestsTotal++;

            /* the securechannel needs all data, including headers */
            pTcpInputStream->Buffer.Position = 0;

            uStatus = pTcpListener->Callback(
                a_pListener,                            /* the event source          */
                (OpcUa_Void*)pTcpListener->CallbackData,/* the callback data         */
                eEvent,                                 /* the event type            */
                (OpcUa_Handle)a_pTcpConnection,         /* handle for the connection */
                a_ppInputStream,                        /* the ready input stream    */
                OpcUa_Good);                            /* event status code         */
        }
        else
        {
            /* delete and close input stream */
            OpcUa_TcpStream_Close((OpcUa_Stream*)(*a_ppInputStream));
            OpcUa_TcpStream_Delete((OpcUa_Stream**)a_ppInputStream);
        }
    }
    else
    {
        /* an error occurred - inform the owner of this listener */

        /* delete and close input stream immediately */
        OpcUa_TcpStream_Close((OpcUa_Stream*)(*a_ppInputStream));
        OpcUa_TcpStream_Delete((OpcUa_Stream**)a_ppInputStream);

        if(pTcpListener->Callback != OpcUa_Null)
        {
            a_pTcpConnection->uNoOfRequestsTotal++;

            uStatus = pTcpListener->Callback(
                a_pListener,                            /* the event source          */
                (OpcUa_Void*)pTcpListener->CallbackData,/* the callback data         */
                eEvent,
                (OpcUa_Handle)a_pTcpConnection,         /* handle for the connection */
                OpcUa_Null,                             /* the ready input stream    */
                uStatus);                               /* event status code         */
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
} /* OpcUa_TcpListener_ProcessRequest */

/*============================================================================
 * OpcUa_TcpListener_SendAcknowledgeMessage
 *===========================================================================*/
/**
* @brief Handles the response to a hello message.
*/
OpcUa_StatusCode OpcUa_TcpListener_SendAcknowledgeMessage(
    OpcUa_Listener*                 a_pListener,
    OpcUa_TcpListener_Connection*   a_pTcpConnection)
{
    OpcUa_OutputStream*     pOutputStream    = OpcUa_Null;
    OpcUa_UInt32            uProtocolVersion = 0;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "SendAcknowledgeMessage");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pTcpConnection);

    uStatus = OpcUa_TcpStream_CreateOutput( a_pTcpConnection->Socket,
                                            OpcUa_TcpStream_MessageType_Acknowledge,
                                            OpcUa_Null,
                                            a_pTcpConnection->SendBufferSize,
                                            OpcUa_TcpListener_ConnectionDisconnectCB,
                                            a_pTcpConnection->MaxChunkCount,
                                            &pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* encode acknowledge fields */

    /* The latest version of the OPC UA TCP protocol supported by the Server */
    uStatus = OpcUa_UInt32_BinaryEncode(uProtocolVersion, pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* revised receivebuffer */
    uStatus = OpcUa_UInt32_BinaryEncode((a_pTcpConnection->ReceiveBufferSize), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* revised sendbuffer */
    uStatus = OpcUa_UInt32_BinaryEncode((a_pTcpConnection->SendBufferSize), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* send max message size */
    uStatus = OpcUa_UInt32_BinaryEncode((a_pTcpConnection->MaxMessageSize), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* send max chunk count */
    uStatus = OpcUa_UInt32_BinaryEncode((a_pTcpConnection->MaxChunkCount), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = pOutputStream->Flush(pOutputStream, OpcUa_True);
    if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
    {
        OpcUa_Buffer      Buffer;
        OpcUa_BufferList* pBufferList = OpcUa_Alloc(sizeof(OpcUa_BufferList));
        OpcUa_GotoErrorIfAllocFailed(pBufferList);
        uStatus = pOutputStream->DetachBuffer((OpcUa_Stream*)pOutputStream, &Buffer);
        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Free(pBufferList);
            OpcUa_GotoError;
        }
        pBufferList->Buffer = Buffer;
        pBufferList->Buffer.Data = OpcUa_Alloc(pBufferList->Buffer.Size);
        pBufferList->Buffer.FreeBuffer = OpcUa_True;
        pBufferList->pNext = OpcUa_Null;
        if(pBufferList->Buffer.Data == OpcUa_Null)
        {
            OpcUa_Free(pBufferList);
            OpcUa_Buffer_Clear(&Buffer);
            OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);
        }
        OpcUa_MemCpy(pBufferList->Buffer.Data, pBufferList->Buffer.EndOfData,
                     Buffer.Data, Buffer.EndOfData);
        uStatus = OpcUa_Listener_AddToSendQueue(
            a_pListener,
            a_pTcpConnection,
            pBufferList,
            OPCUA_LISTENER_NO_RCV_UNTIL_DONE);
        OpcUa_Buffer_Clear(&Buffer);
    }
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = pOutputStream->Close((OpcUa_Stream*)pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    pOutputStream->Delete((OpcUa_Stream**)&pOutputStream);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pOutputStream != OpcUa_Null)
    {
        pOutputStream->Delete((OpcUa_Stream**)&pOutputStream);
    }

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_ProcessHelloMessage
 *===========================================================================*/
/**
 * @brief Handles a Hello message.
 *
 * @param pListener      The listener that hosts the socket from which the message is being received.
 * @param istrm          The stream containing the UAMH.
 */
OpcUa_StatusCode OpcUa_TcpListener_ProcessHelloMessage(
    OpcUa_Listener*                 a_pListener,
    OpcUa_InputStream*              a_istrm)
{
    OpcUa_TcpListener*              pTcpListener    = OpcUa_Null;
    OpcUa_TcpInputStream*           pTcpInputStream = OpcUa_Null;
    OpcUa_TcpListener_Connection*   pConnection     = OpcUa_Null;
#if OPCUA_TCPLISTENER_USEEXTRAMAXCONNSOCKET
    OpcUa_UInt32                    uConnections    = 0;
#endif

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "ProcessHelloMessage");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    pTcpListener = (OpcUa_TcpListener*)a_pListener->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpListener);
    OpcUa_ReturnErrorIfArgumentNull(a_istrm);
    pTcpInputStream = (OpcUa_TcpInputStream*)a_istrm->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpInputStream);

    /* create and add a new connection object for the accepted connection  */
    uStatus = OpcUa_TcpListener_Connection_Create(&pConnection);
    OpcUa_GotoErrorIfBad(uStatus);

    pConnection->Socket          = pTcpInputStream->Socket;
    pConnection->pListenerHandle = (OpcUa_Listener*)a_pListener;


#if OPCUA_P_SOCKETGETPEERINFO_V2
    uStatus = OPCUA_P_SOCKET_GETPEERINFO(pConnection->Socket, pConnection->achPeerInfo, OPCUA_P_PEERINFO_MIN_SIZE);
    if(OpcUa_IsGood(uStatus))
    {
        /* Give some debug information. */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM,
                    "OpcUa_TcpListener_ProcessHelloMessage: Transport connection from %s accepted on socket %p!\n",
                    pConnection->achPeerInfo,
                    pConnection->Socket);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpListener_ProcessHelloMessage: Could not retrieve connection information for socket %p!\n", pConnection->Socket);
    }
#else /* OPCUA_P_SOCKETGETPEERINFO_V2 */
    uStatus = OPCUA_P_SOCKET_GETPEERINFO(pTcpInputStream->Socket, &(pConnection->PeerIp), &(pConnection->PeerPort));
    if(OpcUa_IsGood(uStatus))
    {
        /* Give some debug information. */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM,
                    "OpcUa_TcpListener_ProcessHelloMessage: Transport connection from %d.%d.%d.%d:%d accepted on socket %p!\n",
                    (OpcUa_Int)(pConnection->PeerIp>>24)&0xFF,
                    (OpcUa_Int)(pConnection->PeerIp>>16)&0xFF,
                    (OpcUa_Int)(pConnection->PeerIp>>8) &0xFF,
                    (OpcUa_Int) pConnection->PeerIp     &0xFF,
                    pConnection->PeerPort,
                    pConnection->Socket);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "OpcUa_TcpListener_ProcessHelloMessage: Could not retrieve connection information for socket %p!\n", pConnection->Socket);
    }
#endif /* OPCUA_P_SOCKETGETPEERINFO_V2 */

    uStatus = OpcUa_TcpListener_ConnectionManager_AddConnection(pTcpListener->ConnectionManager, pConnection);
    OpcUa_GotoErrorIfBad(uStatus);

    /* protocol version */
    uStatus = OpcUa_UInt32_BinaryDecode(&pConnection->uProtocolVersion, a_istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* requested send buffer size (this is the receive buffer in the server) */
    uStatus = OpcUa_UInt32_BinaryDecode(&(pConnection->ReceiveBufferSize), a_istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* requested receive buffer size (this is the send buffer in the server) */
    uStatus = OpcUa_UInt32_BinaryDecode(&(pConnection->SendBufferSize), a_istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* requested max message size */
    uStatus = OpcUa_UInt32_BinaryDecode(&(pConnection->MaxMessageSize), a_istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* requested max chunk count */
    uStatus = OpcUa_UInt32_BinaryDecode(&(pConnection->MaxChunkCount), a_istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* requested receive buffer size (this is the send buffer in the server) */
    uStatus = OpcUa_String_BinaryDecode(&(pConnection->sURL), 4096, a_istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* parsing finished */
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Requested: PV:%u SB:%u RB:%u MMS:%u MCC:%u\n",
        pConnection->uProtocolVersion,
        pConnection->SendBufferSize,
        pConnection->ReceiveBufferSize,
        pConnection->MaxMessageSize,
        pConnection->MaxChunkCount);

    pConnection->SendBufferSize     = (pConnection->SendBufferSize    > (OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize)?(OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize:pConnection->SendBufferSize;
    pConnection->ReceiveBufferSize  = (pConnection->ReceiveBufferSize > (OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize)?(OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize:pConnection->ReceiveBufferSize;

    if(OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength != 0 &&
       (pConnection->MaxMessageSize == 0 ||
        pConnection->MaxMessageSize > (OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength))
    {
        pConnection->MaxMessageSize = (OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength;
    }

    if(OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount != 0 &&
       (pConnection->MaxChunkCount == 0 ||
        pConnection->MaxChunkCount > (OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount))
    {
        pConnection->MaxChunkCount = (OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount;
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Set:            SB:%u RB:%u MMS:%u MCC:%u\n",
        pConnection->SendBufferSize,
        pConnection->ReceiveBufferSize,
        pConnection->MaxMessageSize,
        pConnection->MaxChunkCount);

    pConnection->bConnected = OpcUa_True;

    /* This value shall be greater or equal than 8 192 bytes (see 1.03 Errata) */
    if(pConnection->SendBufferSize < 8192 || pConnection->ReceiveBufferSize < 8192)
    {
        uStatus = OpcUa_TcpListener_CloseConnection(a_pListener,
                                                    pConnection,
                                                    OpcUa_BadConnectionRejected);
        OpcUa_ReturnStatusCode;
    }

#if OPCUA_TCPLISTENER_USEEXTRAMAXCONNSOCKET
    OpcUa_TcpListener_ConnectionManager_GetConnectionCount(pTcpListener->ConnectionManager,
                                                           &uConnections);

    if(uConnections >= OPCUA_TCPLISTENER_MAXCONNECTIONS)
    {
        uStatus = OpcUa_TcpListener_CloseConnection(a_pListener,
                                                    pConnection,
                                                    OpcUa_BadMaxConnectionsReached);
        OpcUa_ReturnStatusCode;
    }
#endif /* OPCUA_TCPLISTENER_USEEXTRAMAXCONNSOCKET */

    /* the request is verified and an acknowledge can be sent to the new client */
    OpcUa_TcpListener_SendAcknowledgeMessage(a_pListener, pConnection);

    pTcpListener->Callback( a_pListener,                        /* the source of the event          */
                            pTcpListener->CallbackData,         /* the callback data                */
                            OpcUa_ListenerEvent_ChannelOpened,  /* the event that occurred          */
                            (OpcUa_Handle)pConnection,          /* the handle for the connection    */
                            OpcUa_Null,                         /* the non existing stream          */
                            OpcUa_Good);                        /* status                           */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pConnection != OpcUa_Null)
    {
        /* ignore result; it doesnt matter, if it was not yet registered */
        OpcUa_TcpListener_ConnectionManager_RemoveConnection(pTcpListener->ConnectionManager, pConnection);
        OpcUa_TcpListener_Connection_Delete(&pConnection);
    }

    OPCUA_P_SOCKET_CLOSE(pTcpInputStream->Socket);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_EventHandler Type
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_ProcessDisconnect(   OpcUa_Listener*                 a_pListener,
                                                        OpcUa_TcpListener_Connection*   a_pTcpConnection)
{
    OpcUa_TcpListener* pTcpListener = (OpcUa_TcpListener*)a_pListener;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "ProcessDisconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pTcpConnection);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ProcessDisconnect: Connection with socket %p reported as lost!\n", a_pTcpConnection->Socket);

    OPCUA_P_MUTEX_LOCK(a_pTcpConnection->Mutex);

    if(a_pTcpConnection->bConnected == OpcUa_False)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ProcessDisconnect: Client connection %p with socket %p already set to disconnected!\n", a_pTcpConnection, a_pTcpConnection->Socket);
        uStatus = OpcUa_Good;
        OPCUA_P_MUTEX_UNLOCK(a_pTcpConnection->Mutex);
        OpcUa_ReturnStatusCode;
    }

    /* now, that the upper layers are informed, we can safely remove the resources for the broken connection. */
    uStatus = OpcUa_TcpListener_ConnectionManager_RemoveConnection( pTcpListener->ConnectionManager,
                                                                    a_pTcpConnection);

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ProcessDisconnect: Client connection %p with socket %p already removed!\n", a_pTcpConnection, a_pTcpConnection->Socket);
        uStatus = OpcUa_Good;
        OPCUA_P_MUTEX_UNLOCK(a_pTcpConnection->Mutex);
        OpcUa_ReturnStatusCode;
    }

    a_pTcpConnection->bConnected = OpcUa_False;
    a_pTcpConnection->DisconnectTime = OPCUA_P_DATETIME_UTCNOW();

    if(a_pTcpConnection->bCloseWhenDone == OpcUa_False)
    {
        /* notify about successful closing of the listener */
        pTcpListener->Callback( a_pListener,                        /* the source of the event          */
                                pTcpListener->CallbackData,         /* the callback data                */
                                OpcUa_ListenerEvent_ChannelClosed,  /* the event that occurred          */
                                (OpcUa_Handle)a_pTcpConnection,     /* the handle for the connection    */
                                OpcUa_Null,                         /* the non existing stream          */
                                OpcUa_Good);                        /* status                           */
    }

    OPCUA_P_MUTEX_UNLOCK(a_pTcpConnection->Mutex);

    OpcUa_TcpListener_Connection_Delete(&a_pTcpConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_EventHandler Type
 *===========================================================================*/
/** @brief Internal handler prototype. */
typedef OpcUa_StatusCode (*OpcUa_TcpListener_EventHandler)(OpcUa_Listener*  a_pListener,
                                                           OpcUa_Socket     a_pSocket);

/*============================================================================
 * OpcUa_TcpListener_ReadEventHandler
 *===========================================================================*/
/**
* @brief Gets called if data is available on the socket.
*/
OpcUa_StatusCode OpcUa_TcpListener_ReadEventHandler(
    OpcUa_Listener* a_pListener,
    OpcUa_Socket    a_pSocket)
{
    OpcUa_TcpListener*              pTcpListener            = OpcUa_Null;
    OpcUa_TcpListener_Connection*   pTcpListenerConnection  = OpcUa_Null;
    OpcUa_InputStream*              pInputStream            = OpcUa_Null;
    OpcUa_TcpInputStream*           pTcpInputStream         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "ReadEventHandler");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);
    pTcpListener = (OpcUa_TcpListener *)a_pListener->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpListener);

    /******************************************************************************************************/

    /* look if an active connection is available for the socket. */
    uStatus = OpcUa_TcpListener_ConnectionManager_GetConnectionBySocket(pTcpListener->ConnectionManager,
                                                                        a_pSocket,
                                                                        &pTcpListenerConnection);
    if(OpcUa_IsBad(uStatus) && OpcUa_IsNotEqual(OpcUa_BadNotFound))
    {
        OpcUa_GotoError;
    }

    /******************************************************************************************************/

    /* try to find started stream either in pTcpListenerConnection or in floating messages list */
    if(pTcpListenerConnection != OpcUa_Null)
    {
        /* A connection object exists for this socket. (Hello message was received and validated.) */
        if(pTcpListenerConnection->bNoRcvUntilDone == OpcUa_True)
        {
            pTcpListenerConnection->bRcvDataPending = OpcUa_True;
            OpcUa_ReturnStatusCode;
        }
        pInputStream = pTcpListenerConnection->pInputStream;
        pTcpListenerConnection->pInputStream = OpcUa_Null;
    }
    else
    {
        /* no connection object is available, so this is the first message (and most probably a hello message) */
        /* look if a pending hello message for this socket exists; the connection gets created after the hello message is validated */
        uStatus = OpcUa_TcpListener_LookForPendingMessage(pTcpListener, a_pSocket, &pInputStream);
        if(OpcUa_IsBad(uStatus) && OpcUa_IsNotEqual(OpcUa_BadNotFound))
        {
            /* something unexpected happened */
            OpcUa_GotoError;
        }
    }

    /******************************************************************************************************/

    /* create stream if no one was found */
    if(pInputStream == OpcUa_Null)
    {
        /* set the receiving buffer size to its default size */
        if(pTcpListenerConnection != OpcUa_Null)
        {
            uStatus = OpcUa_TcpStream_CreateInput(  a_pSocket,
                                                    pTcpListenerConnection->ReceiveBufferSize,
                                                    &pInputStream);
            OpcUa_GotoErrorIfBad(uStatus);
        }
        else
        {
            uStatus = OpcUa_TcpStream_CreateInput(  a_pSocket,
                                                    OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize,
                                                    &pInputStream);
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }

    /******************************************************************************************************/

    /* now, we have a stream -> read the available data; further processing takes place in the callback */
    uStatus = OpcUa_TcpStream_DataReady(pInputStream);

    /******************************************************************************************************/

    if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
    {
        /* prepare to append further data later */

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ReadEventHandler: CallAgain result for stream %p on socket %p!\n", pInputStream, a_pSocket);

        if(pTcpListenerConnection != 0)
        {
            /* if we reach this point, the message cannot be a uamh */
            pTcpListenerConnection->pInputStream = pInputStream;
        }
        else
        {
            /* no pTcpListenerConnection to append it, so store it in our temporary list; must be uamh */
            OpcUa_List_Enter(pTcpListener->PendingMessages);
            uStatus = OpcUa_List_AddElement(pTcpListener->PendingMessages, pInputStream);
            OpcUa_List_Leave(pTcpListener->PendingMessages);
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }
    else /* process message */
    {
        pTcpInputStream = (OpcUa_TcpInputStream*)pInputStream->Handle;

        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_CharA* sError = OpcUa_Null;

            /* Error happened... */
            switch(uStatus)
            {
            case OpcUa_BadDecodingError:
                {
                    sError = (OpcUa_CharA*)"OpcUa_BadDecodingError";
                    break;
                }
            case OpcUa_BadCommunicationError:
                {
                    sError = (OpcUa_CharA*)"OpcUa_BadCommunicationError";
                    break;
                }
            case OpcUa_BadDisconnect:
                {
                    sError = (OpcUa_CharA*)"OpcUa_BadDisconnect";
                    break;
                }
            case OpcUa_BadConnectionClosed:
                {
                    sError = (OpcUa_CharA*)"OpcUa_BadConnectionClosed";
                    break;
                }
            default:
                {
                    sError = (OpcUa_CharA*)"unmapped";
                }
            }

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ReadEventHandler: socket %p; status 0x%08X (%s)\n", a_pSocket, uStatus, sError);

            OpcUa_GotoError;
        }
        else /* Message can be processed. */
        {
            /* process message */
            switch(pTcpInputStream->MessageType)
            {
            case OpcUa_TcpStream_MessageType_Hello:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ReadEventHandler: MessageType HELLO\n");
                    if(pTcpListenerConnection == OpcUa_Null)
                    {
                        uStatus = OpcUa_TcpListener_ProcessHelloMessage(a_pListener, pInputStream);
                        OpcUa_TcpStream_Close((OpcUa_Stream*)pInputStream);
                        OpcUa_TcpStream_Delete((OpcUa_Stream**)&pInputStream);
                    }
                    else
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpListener_ReadEventHandler: Received duplicate HELLO request!\n");
                        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
                    }

                    break;
                }
            case OpcUa_TcpStream_MessageType_SecureChannel:
                {
                    /* This is the standard message used during communication.  */
                    /* Abort is used here to rollback the data pipe up to the seclayer. */
                    /* Maybe we will need a own handler for this. */

                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ReadEventHandler: MessageType SecureChannel Message\n");

                    if(pTcpListenerConnection != OpcUa_Null)
                    {
                        uStatus = OpcUa_TcpListener_ProcessRequest( a_pListener,
                                                                    pTcpListenerConnection,
                                                                    &pInputStream);

                        if(pInputStream != OpcUa_Null)
                        {
                            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpListener_ReadEventHandler: InputStream wasn't correctly released! Deleting it!\n");
                            OpcUa_TcpStream_Close((OpcUa_Stream*)pInputStream);
                            OpcUa_TcpStream_Delete((OpcUa_Stream**)&pInputStream);
                        }

                        if(OpcUa_IsBad(uStatus))
                        {
                            /* this is probably intended: mask trace to make it not look like an error */
                            if(OpcUa_IsNotEqual(OpcUa_BadDisconnect))
                            {
                                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpListener_ReadEventHandler: Process Request returned an error (0x%08X)!\n", uStatus);
                            }
                        }
                    }
                    else
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpListener_ReadEventHandler: Received request for nonexisting connection!\n");
                        OPCUA_P_SOCKET_CLOSE(pTcpInputStream->Socket);
                        OpcUa_TcpStream_Close((OpcUa_Stream*)pInputStream);
                        OpcUa_TcpStream_Delete((OpcUa_Stream**)&pInputStream);
                    }

                    break;
                }
            default:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_ReadEventHandler: Invalid MessageType (%d)\n", pTcpInputStream->MessageType);
                    OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
                    break;
                }
            }
        }
    } /* if(OpcUa_IsEqual(OpcUa_GoodCallAgain)) */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_P_SOCKET_CLOSE(a_pSocket);

    if(pInputStream != OpcUa_Null)
    {
        OpcUa_TcpStream_Close((OpcUa_Stream*)pInputStream);
        OpcUa_TcpStream_Delete((OpcUa_Stream**)&pInputStream);
    }

    if(pTcpListenerConnection != OpcUa_Null)
    {
        /* Notify about connection loss. */
        OpcUa_TcpListener_ProcessDisconnect(    a_pListener,
                                                pTcpListenerConnection);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_TimeoutEventHandler
 *===========================================================================*/
/**
* @brief Gets called in case of a timeout on the socket.
*/
OpcUa_StatusCode OpcUa_TcpListener_TimeoutEventHandler(
    OpcUa_Listener* a_pListener,
    OpcUa_Socket    a_pSocket)
{
    OpcUa_TcpListener*              pTcpListener            = OpcUa_Null;
    OpcUa_TcpListener_Connection*   pTcpListenerConnection  = OpcUa_Null;
    OpcUa_InputStream*              pInputStream            = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "TimeoutEventHandler");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);
    pTcpListener = (OpcUa_TcpListener *)a_pListener->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpListener);

    /******************************************************************************************************/

    /* look if an active connection is available for the socket. */
    uStatus = OpcUa_TcpListener_ConnectionManager_GetConnectionBySocket(pTcpListener->ConnectionManager,
                                                                        a_pSocket,
                                                                        &pTcpListenerConnection);
    if(OpcUa_IsBad(uStatus) && OpcUa_IsNotEqual(OpcUa_BadNotFound))
    {
        OpcUa_GotoError;
    }

    /******************************************************************************************************/

    /* try to find started stream either in pTcpListenerConnection or in floating messages list */
    if(pTcpListenerConnection != OpcUa_Null)
    {
        /* A connection object exists for this socket. (Hello message was received and validated.) */
        pInputStream = pTcpListenerConnection->pInputStream;
        pTcpListenerConnection->pInputStream = OpcUa_Null;
    }
    else
    {
        /* no connection object is available, so this is the first message (and most probably a hello message) */
        /* look if a pending hello message for this socket exists; the connection gets created after the hello message is validated */
        uStatus = OpcUa_TcpListener_LookForPendingMessage(pTcpListener, a_pSocket, &pInputStream);
        if(OpcUa_IsBad(uStatus) && OpcUa_IsNotEqual(OpcUa_BadNotFound))
        {
            /* something unexpected happened */
            OpcUa_GotoError;
        }
    }

    /******************************************************************************************************/

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpListener_TimeoutEventHandler: socket %p\n", a_pSocket);

    OPCUA_P_SOCKET_CLOSE(a_pSocket);

    if(pInputStream != OpcUa_Null)
    {
        OpcUa_TcpStream_Close((OpcUa_Stream*)pInputStream);
        OpcUa_TcpStream_Delete((OpcUa_Stream**)&pInputStream);
    }

    if(pTcpListenerConnection != OpcUa_Null)
    {
        /* Notify about connection loss. */
        OpcUa_TcpListener_ProcessDisconnect(    a_pListener,
                                                pTcpListenerConnection);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpListener_WriteEventHandler
 *===========================================================================*/
/**
* @brief Gets called if data can be written to the socket.
*/
OpcUa_StatusCode OpcUa_TcpListener_WriteEventHandler(
    OpcUa_Listener* a_pListener,
    OpcUa_Socket    a_pSocket)
{
    OpcUa_TcpListener*              pTcpListener            = OpcUa_Null;
    OpcUa_TcpListener_Connection*   pTcpListenerConnection  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "WriteEventHandler");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);
    pTcpListener = (OpcUa_TcpListener *)a_pListener->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpListener);

    /******************************************************************************************************/

    /* look if an active connection is available for the socket. */
    uStatus = OpcUa_TcpListener_ConnectionManager_GetConnectionBySocket(pTcpListener->ConnectionManager,
                                                                        a_pSocket,
                                                                        &pTcpListenerConnection);
    if(OpcUa_IsBad(uStatus))
    {
        /* no connection available */
        OpcUa_GotoError;
    }

    /******************************************************************************************************/

    /* look for pending output stream */
    if(pTcpListenerConnection != OpcUa_Null)
    {
        do {
            while(pTcpListenerConnection->pSendQueue != OpcUa_Null)
            {
                OpcUa_BufferList *pCurrentBuffer = pTcpListenerConnection->pSendQueue;
                OpcUa_Int32 iDataLength = pCurrentBuffer->Buffer.EndOfData - pCurrentBuffer->Buffer.Position;
                OpcUa_Int32 iDataWritten = OPCUA_P_SOCKET_WRITE(a_pSocket,
                                                                &pCurrentBuffer->Buffer.Data[pCurrentBuffer->Buffer.Position],
                                                                iDataLength,
                                                                OpcUa_False);
                if(iDataWritten<0)
                {
                    return OpcUa_TcpListener_TimeoutEventHandler(a_pListener, a_pSocket);
                }
                else if(iDataWritten<iDataLength)
                {
                    pCurrentBuffer->Buffer.Position += iDataWritten;
                    if((pTcpListenerConnection->bNoRcvUntilDone == OpcUa_False) &&
                       (pTcpListenerConnection->bRcvDataPending == OpcUa_True))
                    {
                        pTcpListenerConnection->bRcvDataPending = OpcUa_False;
                        uStatus = OpcUa_TcpListener_ReadEventHandler(a_pListener, a_pSocket);
                    }
                    OpcUa_ReturnStatusCode;
                }
                else
                {
                    pTcpListenerConnection->pSendQueue = pCurrentBuffer->pNext;
                    OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
                    OpcUa_Free(pCurrentBuffer);
                }
            } /* end while */

            if(pTcpListenerConnection->bCloseWhenDone == OpcUa_True)
            {
                break;
            }
            pTcpListenerConnection->bNoRcvUntilDone = OpcUa_False;
            pTcpListener->Callback(
                a_pListener,                            /* the event source */
                (OpcUa_Void*)pTcpListener->CallbackData,/* the callback data */
                OpcUa_ListenerEvent_RefillSendQueue,    /* the event that occurred */
                pTcpListenerConnection,                 /* a connection handle */
                OpcUa_Null,                             /* the input stream for the event (none in this case) */
                uStatus);                               /* a status code for the event */

        } while(pTcpListenerConnection->pSendQueue != OpcUa_Null);
        if(pTcpListenerConnection->bCloseWhenDone == OpcUa_True)
        {
            uStatus = OpcUa_TcpListener_TimeoutEventHandler(a_pListener, a_pSocket);
        }
        else if((pTcpListenerConnection->bNoRcvUntilDone == OpcUa_False) &&
                (pTcpListenerConnection->bRcvDataPending == OpcUa_True))
        {
            pTcpListenerConnection->bRcvDataPending = OpcUa_False;
            uStatus = OpcUa_TcpListener_ReadEventHandler(a_pListener, a_pSocket);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_EventCallback
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_EventCallback(
    OpcUa_Socket    a_pSocket,
    OpcUa_UInt32    a_uSocketEvent,
    OpcUa_Void*     a_pUserData,
    OpcUa_UInt16    a_uPortNumber,
    OpcUa_Boolean   a_bIsSSL)
{
    OpcUa_StringA                   strEvent        = OpcUa_Null;
    OpcUa_Listener*                 listener        = (OpcUa_Listener*)a_pUserData;
    OpcUa_TcpListener*              pTcpListener    = (OpcUa_TcpListener*)listener->Handle;
    OpcUa_TcpListener_EventHandler  fEventHandler   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "EventCallback");

    OpcUa_ReferenceParameter(a_bIsSSL);
    OpcUa_ReferenceParameter(a_uPortNumber);

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);


#if 1 /* debug code */
    switch(a_uSocketEvent)
    {
    case OPCUA_SOCKET_NO_EVENT:
        {
            strEvent = "OPCUA_SOCKET_NO_EVENT";
            break;
        }
    case OPCUA_SOCKET_READ_EVENT:
        {
            strEvent = "OPCUA_SOCKET_READ_EVENT";
            break;
        }
    case OPCUA_SOCKET_WRITE_EVENT:
        {
            strEvent = "OPCUA_SOCKET_WRITE_EVENT";
            break;
        }
    case OPCUA_SOCKET_EXCEPT_EVENT:
        {
            strEvent = "OPCUA_SOCKET_EXCEPT_EVENT";
            break;
        }
    case OPCUA_SOCKET_TIMEOUT_EVENT:
        {
            strEvent = "OPCUA_SOCKET_TIMEOUT_EVENT";
            break;
        }
    case OPCUA_SOCKET_CLOSE_EVENT:
        {
            strEvent = "OPCUA_SOCKET_CLOSE_EVENT";
            break;
        }
    case OPCUA_SOCKET_CONNECT_EVENT:
        {
            strEvent = "OPCUA_SOCKET_CONNECT_EVENT";
            break;
        }
    case OPCUA_SOCKET_ACCEPT_EVENT:
        {
            strEvent = "OPCUA_SOCKET_ACCEPT_EVENT";
            break;
        }
    case OPCUA_SOCKET_SHUTDOWN_EVENT:
        {
            strEvent = "OPCUA_SOCKET_SHUTDOWN_EVENT";
            break;
        }
    case OPCUA_SOCKET_NEED_BUFFER_EVENT:
        {
            strEvent = "OPCUA_SOCKET_NEED_BUFFER_EVENT";
            break;
        }
    case OPCUA_SOCKET_FREE_BUFFER_EVENT:
        {
            strEvent = "OPCUA_SOCKET_FREE_BUFFER_EVENT";
            break;
        }
    default:
        {
            strEvent = "ERROR DEFAULT!";
            break;
        }
    }
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, " * OpcUa_TcpListener_EventCallback: Socket(%p), Port(%d), Data(%p), Event(%s)\n", a_pSocket, a_uPortNumber, a_pUserData, strEvent);
    /* debug code end */
#endif

    switch(a_uSocketEvent)
    {
    case OPCUA_SOCKET_READ_EVENT:
        {
            /* notifies an existing stream about new data or creates a new stream */
            fEventHandler = OpcUa_TcpListener_ReadEventHandler;
            break;
        }
    case OPCUA_SOCKET_EXCEPT_EVENT:
        {
            fEventHandler = OpcUa_TcpListener_TimeoutEventHandler;
            break;
        }
    case OPCUA_SOCKET_WRITE_EVENT:
        {
            fEventHandler = OpcUa_TcpListener_WriteEventHandler;
            break;
        }
    case OPCUA_SOCKET_NEED_BUFFER_EVENT:
        {
            /* fEventHandler = OpcUa_TcpListener_NeedBufferEventHandler; */
            break;
        }
    case OPCUA_SOCKET_FREE_BUFFER_EVENT:
        {
            /* fEventHandler = OpcUa_TcpListener_FreeBufferEventHandler; */
            break;
        }
    case OPCUA_SOCKET_TIMEOUT_EVENT:
        {
            fEventHandler = OpcUa_TcpListener_TimeoutEventHandler;
            break;
        }
    case OPCUA_SOCKET_CLOSE_EVENT:
        {
            fEventHandler = OpcUa_TcpListener_TimeoutEventHandler;
            break;
        }
    case OPCUA_SOCKET_NO_EVENT:
    case OPCUA_SOCKET_SHUTDOWN_EVENT:
    case OPCUA_SOCKET_ACCEPT_EVENT:
        {
            break;
        }
    default:
        {
            /* unexpected error, report to upper layer. */
            pTcpListener->Callback(
                listener,                               /* the event source */
                (OpcUa_Void*)pTcpListener->CallbackData, /* the callback data */
                OpcUa_ListenerEvent_UnexpectedError,    /* the event that occurred */
                OpcUa_Null,                             /* a connection handle */
                OpcUa_Null,                             /* the input stream for the event (none in this case) */
                uStatus);                               /* a status code for the event */

            break;
        }
    }

    /* call the internal specialized event handler */
    if(fEventHandler != OpcUa_Null)
    {
        uStatus = fEventHandler(listener, a_pSocket);
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, " * OpcUa_TcpListener_EventCallback: Event Handler returned.\n");

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpListener_ConnectionCloseCallback
 *===========================================================================*/
 /** @brief Callback function for the Connection Manager on connection deletion.
  *
  *  @param Listener The listener the tcp connection belongs to.
  *  @param TcpConnection The tcp connection that is being deleted.
  */
OpcUa_Void OpcUa_TcpListener_ConnectionDeleteCallback(  OpcUa_Listener*                 a_pListener,
                                                        OpcUa_TcpListener_Connection*   a_pTcpConnection)
{
    OpcUa_ReferenceParameter(a_pListener);

#if OPCUA_P_SOCKETGETPEERINFO_V2
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG,
            "OpcUa_TcpListener_ConnectionDeleteCallback: Connection to peer %s (socket %p) gets closed!!\n",
            a_pTcpConnection->achPeerInfo,
            a_pTcpConnection->Socket);
#else
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG,
            "OpcUa_TcpListener_ConnectionDeleteCallback: Connection to peer %d.%d.%d.%d:%d (socket %p) gets closed!!\n",
            (OpcUa_Int)(a_pTcpConnection->PeerIp>>24)&0xFF,
            (OpcUa_Int)(a_pTcpConnection->PeerIp>>16)&0xFF,
            (OpcUa_Int)(a_pTcpConnection->PeerIp>>8) &0xFF,
            (OpcUa_Int) a_pTcpConnection->PeerIp     &0xFF,
            a_pTcpConnection->PeerPort,
            a_pTcpConnection->Socket);
#endif

    if(a_pTcpConnection->Socket != OpcUa_Null)
    {
        /* OPCUA_P_SOCKET_CLOSE(a_pTcpConnection->Socket); */
        a_pTcpConnection->Socket = OpcUa_Null;
    }

    /* TODO: consider invoking owner callback and tell about the closing. */

    return;
}

/*============================================================================
 * OpcUa_TcpListener_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_Close(OpcUa_Listener* a_pListener)
{
    OpcUa_TcpListener*                      pTcpListener    = OpcUa_Null;
    OpcUa_InputStream*                      pInputStream    = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_TcpListener);

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_TcpListener, a_pListener, Close);

    pTcpListener     = (OpcUa_TcpListener*)a_pListener->Handle;

    /* lock connection and close the socket. */
    OPCUA_P_MUTEX_LOCK(pTcpListener->Mutex);

    /* check if already stopped */
    if(pTcpListener->Socket != OpcUa_Null)
    {
        /* only close listening socket, which should be in the global list. */
        OPCUA_P_SOCKET_CLOSE(pTcpListener->Socket);
        pTcpListener->Socket = OpcUa_Null;
    }

#if OPCUA_MULTITHREADED

    OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);

    /* check if socket list handle is valid */
    if(pTcpListener->SocketManager != OpcUa_Null)
    {
        /* stops the thread and closes socket */
        OPCUA_P_SOCKETMANAGER_DELETE(&(pTcpListener->SocketManager));
    }

    /* lock connection and close the socket. */
    OPCUA_P_MUTEX_LOCK(pTcpListener->Mutex);

#endif /* OPCUA_MULTITHREADED */

    /* cleanup all connections */
    OpcUa_TcpListener_ConnectionManager_RemoveConnections(  pTcpListener->ConnectionManager,
                                                            OpcUa_TcpListener_ConnectionDeleteCallback);

    OpcUa_List_Enter(pTcpListener->PendingMessages);
    OpcUa_List_ResetCurrent(pTcpListener->PendingMessages);
    pInputStream = (OpcUa_InputStream *)OpcUa_List_GetCurrentElement(pTcpListener->PendingMessages);
    while(pInputStream != OpcUa_Null)
    {
        OpcUa_List_DeleteCurrentElement(pTcpListener->PendingMessages);
        pInputStream->Close((OpcUa_Stream*)pInputStream);
        pInputStream->Delete((OpcUa_Stream**)&pInputStream);
        pInputStream = (OpcUa_InputStream *)OpcUa_List_GetCurrentElement(pTcpListener->PendingMessages);
    }
    OpcUa_List_Leave(pTcpListener->PendingMessages);

    OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);

    /* notify about successful closing of the listener */
    pTcpListener->Callback( a_pListener,                /* the source of the event          */
                            pTcpListener->CallbackData, /* the callback data                */
                            OpcUa_ListenerEvent_Close,  /* the event that occurred          */
                            OpcUa_Null,                 /* the handle for the connection    */
                            OpcUa_Null,                 /* the non existing stream          */
                            OpcUa_Good);                /* status                           */



    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_TcpListener_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpListener_Open(
    struct _OpcUa_Listener*     a_pListener,
    OpcUa_String*               a_sUrl,
    OpcUa_Boolean               a_bListenOnAllInterfaces,
    OpcUa_Listener_PfnOnNotify* a_pfnCallback,
    OpcUa_Void*                 a_pCallbackData)
{
    OpcUa_TcpListener*  pTcpListener        = OpcUa_Null;
    OpcUa_UInt32        uSocketManagerFlags = OPCUA_SOCKET_NO_FLAG;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "Open");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_sUrl);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_TcpListener, a_pListener, Open);

    pTcpListener = (OpcUa_TcpListener*)a_pListener->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpListener);

    if(OpcUa_ProxyStub_g_Configuration.bTcpListener_ClientThreadsEnabled != OpcUa_False)
    {
        uSocketManagerFlags |= OPCUA_SOCKET_SPAWN_THREAD_ON_ACCEPT | OPCUA_SOCKET_REJECT_ON_NO_THREAD;
    }

    /********************************************************************/

    /* lock listener while thread is starting */
    OPCUA_P_MUTEX_LOCK(pTcpListener->Mutex);

    /* check if thread already started */
    if(pTcpListener->Socket != OpcUa_Null)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);
        return OpcUa_BadInvalidState;
    }

    pTcpListener->Callback     = a_pfnCallback;
    pTcpListener->CallbackData = a_pCallbackData;

    /********************************************************************/

    /* start up socket handling for this listener */
#if OPCUA_MULTITHREADED
    /* check if socket list handle not yet set */
    if(pTcpListener->SocketManager != OpcUa_Null)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);
        return OpcUa_BadInvalidState;
    }

    uStatus = OPCUA_P_SOCKETMANAGER_CREATE( &(pTcpListener->SocketManager),
                                            OPCUA_TCPLISTENER_MAXCONNECTIONS + 1, /* add one for listen socket */
                                            uSocketManagerFlags);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_SOCKETMANAGER_CREATESERVER(   pTcpListener->SocketManager,
                                                    OpcUa_String_GetRawString(a_sUrl),
                                                    a_bListenOnAllInterfaces,
                                                    OpcUa_TcpListener_EventCallback,
                                                    (OpcUa_Void*)a_pListener,
                                                    &(pTcpListener->Socket));

#else /* OPCUA_MULTITHREADED */

    /* single thread socket created on global socket manager */
    uStatus = OPCUA_P_SOCKETMANAGER_CREATESERVER(   OpcUa_Null,
                                                    OpcUa_String_GetRawString(a_sUrl),
                                                    a_bListenOnAllInterfaces,
                                                    OpcUa_TcpListener_EventCallback,
                                                    (OpcUa_Void*)a_pListener,
                                                    &(pTcpListener->Socket));

#endif /* OPCUA_MULTITHREADED */
    OpcUa_GotoErrorIfBad(uStatus);

    /********************************************************************/

    /* notify about successful opening of the listener */
    pTcpListener->Callback( a_pListener,                /* the source of the event          */
                            pTcpListener->CallbackData, /* the callback data                */
                            OpcUa_ListenerEvent_Open,   /* the event that occurred          */
                            OpcUa_Null,                 /* the handle for the connection    */
                            OpcUa_Null,                 /* the non existing stream          */
                            OpcUa_Good);                /* status                           */

    OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_TcpListener_Close(a_pListener);

    OPCUA_P_MUTEX_UNLOCK(pTcpListener->Mutex);

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_SERVERAPI */
