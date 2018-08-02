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

/* core */
#include <opcua.h>

#ifdef OPCUA_HAVE_CLIENTAPI

#include <opcua_mutex.h>
#include <opcua_datetime.h>
#include <opcua_socket.h>
#include <opcua_utilities.h>
#include <opcua_list.h>
#include <opcua_guid.h>
#include <opcua_timer.h>

/* types */
#include <opcua_builtintypes.h>
#include <opcua_binaryencoder.h>
#include <opcua_tcpstream.h>
#include <opcua_binaryencoder.h>

/* self */
#include <opcua_tcpconnection.h>

/* default value is 1, the alternative is a research implementation, which needs to be tested! */
#define OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM 1

/*============================================================================
 * OpcUa_SecureConnectionState
 *===========================================================================*/
 /** @brief The states a TcpConnection can be in. */
typedef enum _OpcUa_TcpConnectionState
{
    /** @brief Error state. */
    OpcUa_TcpConnectionState_Invalid,
    /** @brief Connection object connecting. */
    OpcUa_TcpConnectionState_Connecting,
    /** @brief Connection is established, communication to the server is possible. */
    OpcUa_TcpConnectionState_Connected,
    /** @brief The session was closed gracefully with a disconnect message. */
    OpcUa_TcpConnectionState_Disconnected,
    /** @brief An error message was received and the connection is inactive. */
    OpcUa_TcpConnectionState_Error
} OpcUa_TcpConnectionState;


/*============================================================================
 * OpcUa_TcpConnection
 *===========================================================================*/
/** @brief Holds all data needed to manage a tcp binary connection to an ua server. */
typedef struct _OpcUa_TcpConnection
{
    /** @brief Internal helper to verify instances. */
    OpcUa_UInt32                    SanityCheck;
    /** @brief The state of the connection to the server. */
    OpcUa_TcpConnectionState        ConnectionState;
    /** @brief The socket holding the connection to the server. */
    OpcUa_Socket                    Socket;
    /** @brief Messaging events to the holder of this connection. */
    OpcUa_Connection_PfnOnNotify*   NotifyCallback;
    /** @brief Data to pass back with the callback. */
    OpcUa_Void*                     CallbackData;
    /** @brief Synchronizing access to this connection. */
    OpcUa_Mutex                     ReadMutex;
    /** @brief An active datastream being received (message). */
    OpcUa_InputStream*              IncomingStream;
#if !OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM
    /** @brief An active datastream being sent (message). */
    OpcUa_OutputStream*             OutgoingStream;
    OpcUa_Boolean                   bOutgoingStreamIsUsed;
#endif /* !OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM */
    /** @brief The time when the connection was established. */
    OpcUa_DateTime                  ConnectTime;
    /** @brief The time when the client disconnected. */
    OpcUa_DateTime                  DisconnectTime;
    /** @brief The buffer size for receiving data on this connection. */
    OpcUa_UInt32                    ReceiveBufferSize;
    /** @brief The buffer size for sending data over this connection. */
    OpcUa_UInt32                    SendBufferSize;
    /** @brief The maximum message size accepted by this connection. */
    OpcUa_UInt32                    MaxMessageSize;
    /** @brief The maximum number of chunks per message accepted by this connection. */
    OpcUa_UInt32                    MaxChunkCount;
    /** @brief The current number of chunks in a message. If 0, the connection is waiting for the next message. */
    OpcUa_UInt32                    uCurrentChunk;
    OpcUa_String                    sURL;
#if OPCUA_MULTITHREADED
    /*! @brief Holds the socket for this connection, the thread and is the central waiting point. */
    OpcUa_SocketManager             SocketManager;
#endif /* OPCUA_MULTITHREADED */
    /*! @brief The protocol version used for this connection. */
    OpcUa_UInt32                    uProtocolVersion;
    /** @brief The queued list of data blocks to be sent. */
    OpcUa_BufferList*               pSendQueue;
}
OpcUa_TcpConnection;

/*============================================================================
 * OpcUa_TcpConnection_SanityCheck
 *===========================================================================*/
#define OpcUa_TcpConnection_SanityCheck 0x4FCC07CB

/*============================================================================
 * OpcUa_ReturnErrorIfInvalidConnection
 *===========================================================================*/
#define OpcUa_ReturnErrorIfInvalidConnection(xConnection) \
if (((OpcUa_TcpConnection*)(xConnection)->Handle)->SanityCheck != OpcUa_TcpConnection_SanityCheck) \
{ \
    return OpcUa_BadInvalidArgument; \
}

/*============================================================================
 * Forward Declaration
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_Disconnect(                OpcUa_Connection*   a_pConnection,
                                                                OpcUa_Boolean       a_bNotifyOnComplete);

OpcUa_StatusCode OpcUa_TcpConnection_BeginReceiveResponse(      OpcUa_Connection*   a_pConnection,
                                                                OpcUa_InputStream** a_ppInputStream);

OpcUa_StatusCode OpcUa_TcpConnection_GetReceiveBufferSize(      OpcUa_Connection*   a_pConnection,
                                                                OpcUa_UInt32*       a_pBufferSize);

OpcUa_StatusCode OpcUa_TcpConnection_AddToSendQueue(            OpcUa_Connection*   a_pConnection,
                                                                OpcUa_BufferList*   a_pBufferList,
                                                                OpcUa_UInt32        a_uFlags);

OpcUa_StatusCode OpcUa_TcpConnection_CheckProtocolVersion(      OpcUa_Connection*   a_pConnection,
                                                                OpcUa_UInt32        a_uProtocolVersion);

/*============================================================================
 * OpcUa_TcpListener_EventHandler Type Definition
 *===========================================================================*/
/** @brief Internal handler prototype. */
typedef OpcUa_StatusCode (*OpcUa_TcpConnection_EventHandler)(   OpcUa_Connection*   a_pConnection,
                                                                OpcUa_Socket        a_pSocket);

/*============================================================================
 * OpcUa_TcpRequestState
 *===========================================================================*/
/** @brief Tells about the current processing state of the request. */
typedef enum _OpcUa_TcpRequestState
{
    /** @brief The server is waiting for the response. */
    OpcUa_TcpRequestState_Invalid,
    /** @brief The server is waiting for the response. */
    OpcUa_TcpRequestState_Open,
    /** @brief The response arrived partially. */
    OpcUa_TcpRequestState_Started,
    /** @brief The response arrived completely, the request is finished. */
    OpcUa_TcpRequestState_Finished,
    /** @brief Either server or client cancelled the request. */
    OpcUa_TcpRequestState_Cancelled
} OpcUa_TcpRequestState;

/*============================================================================
 * Handling a disconnect from the server.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_TcpConnection_HandleDisconnect(OpcUa_Connection* a_pConnection)
{
    OpcUa_TcpConnection* pTcpConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "HandleDisconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_HandleDisconnect!\n");

    /* mark the connection as closed */
    OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);

    if(pTcpConnection->ConnectionState == OpcUa_TcpConnectionState_Disconnected)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        OpcUa_ReturnStatusCode;
    }

    /* close the socket */
    OPCUA_P_SOCKET_CLOSE(pTcpConnection->Socket);
    pTcpConnection->Socket = OpcUa_Null;
    pTcpConnection->DisconnectTime = OPCUA_P_DATETIME_UTCNOW();
    pTcpConnection->ConnectionState = OpcUa_TcpConnectionState_Disconnected;

    OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

    /* notify upper layer about disconnect */
    if(pTcpConnection->NotifyCallback != OpcUa_Null)
    {
        pTcpConnection->NotifyCallback( a_pConnection,
                                        pTcpConnection->CallbackData,
                                        OpcUa_ConnectionEvent_Disconnect,
                                        OpcUa_Null, /* no stream for this event */
                                        OpcUa_BadDisconnect);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
} /* OpcUa_TcpConnection_HandleDisconnect */

/*============================================================================
 * OpcUa_TcpConnection_ConnectionDisconnectCB
 *===========================================================================*/
/** @brief Gets called by an outstream if the connection is lost. */
static OpcUa_Void OpcUa_TcpConnection_ConnectionDisconnectCB(OpcUa_Handle a_hConnection)
{
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ConnectionDisconnectCB: Connection %p is being reported as broken!\n", a_hConnection);

    OpcUa_TcpConnection_HandleDisconnect((OpcUa_Connection*)a_hConnection);
}

/*============================================================================
 * Send a Hello message.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_TcpConnection_SendHelloMessage(OpcUa_Connection* a_pConnection)
{
    OpcUa_TcpConnection*    pTcpConnection   = OpcUa_Null;
    OpcUa_OutputStream*     pOutputStream    = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "SendHelloMessage");

    OpcUa_GotoErrorIfArgumentNull(a_pConnection);

    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpConnection);

    uStatus = OpcUa_TcpStream_CreateOutput( pTcpConnection->Socket,
                                            OpcUa_TcpStream_MessageType_Hello,
                                            OpcUa_Null,
                                            OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize, /* use initial value before handshake */
                                            OpcUa_TcpConnection_ConnectionDisconnectCB,
                                            pTcpConnection->MaxChunkCount,
                                            &pOutputStream);

    OpcUa_GotoErrorIfBad(uStatus);


    /* encode the body of a Hello message */

    /* client protocol version */
    uStatus = OpcUa_UInt32_BinaryEncode((pTcpConnection->uProtocolVersion), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* receive buffer length */
    uStatus = OpcUa_UInt32_BinaryEncode((pTcpConnection->ReceiveBufferSize), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* send buffer length */
    uStatus = OpcUa_UInt32_BinaryEncode((pTcpConnection->SendBufferSize), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* send max message size */
    uStatus = OpcUa_UInt32_BinaryEncode((pTcpConnection->MaxMessageSize), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* send max chunk count */
    uStatus = OpcUa_UInt32_BinaryEncode((pTcpConnection->MaxChunkCount), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* encode buffer length */
    uStatus = OpcUa_String_BinaryEncode(&(pTcpConnection->sURL), pOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Requesting: SB:%u RB:%u\n",   pTcpConnection->SendBufferSize,
                                                                        pTcpConnection->ReceiveBufferSize);

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
        uStatus = OpcUa_Connection_AddToSendQueue(
            a_pConnection,
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
} /* OpcUa_TcpConnection_SendHelloMessage */

/*============================================================================
 * Process Acknowledge Message
 *===========================================================================*/
/**
 * @brief Reads and parses an acknowledge message and passes the result up.
 */
static OpcUa_StatusCode OpcUa_TcpConnection_ProcessAcknowledgeMessage(  OpcUa_Connection*   a_pConnection,
                                                                        OpcUa_InputStream*  a_pInputStream)
{
    OpcUa_TcpInputStream*   pTcpInputStream         = OpcUa_Null;
    OpcUa_TcpConnection*    pTcpConnection          = OpcUa_Null;
    OpcUa_UInt32            uRevisedProtocolVersion = 0;
    OpcUa_UInt32            uRevisedRecvBufSize     = 0;
    OpcUa_UInt32            uRevisedSendBufSize     = 0;
    OpcUa_UInt32            uRevisedMaxChunkCnt     = 0;
    OpcUa_UInt32            uRevisedMessageSize     = 0;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "ProcessAcknowledgeMessage");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpConnection);

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    pTcpInputStream = (OpcUa_TcpInputStream*)a_pInputStream->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpInputStream);

    /* consistency check */
    if((a_pInputStream != pTcpConnection->IncomingStream) ||
       (pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connecting) ||
       (pTcpConnection->pSendQueue != OpcUa_Null))
    {
        return OpcUa_Bad;
    }

    /* parse the fields of an acknowledge message */
    /* The latest version of the OPC UA TCP protocol supported by the Server */
    uStatus = OpcUa_UInt32_BinaryDecode(&uRevisedProtocolVersion, a_pInputStream);
    OpcUa_GotoErrorIfBad(uStatus);
    pTcpConnection->uProtocolVersion = uRevisedProtocolVersion;

    /* revised recv buffer length */
    uStatus = OpcUa_UInt32_BinaryDecode(&uRevisedRecvBufSize, a_pInputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check the revised buffer sizes */
    /* This value shall not be larger than what the Client requested in the Hello Message */
    /* This value shall be greater or equal than 8 192 bytes (see 1.03 Errata) */
    if(uRevisedRecvBufSize > pTcpConnection->ReceiveBufferSize ||
       uRevisedRecvBufSize < 8192)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadConnectionRejected);
    }
    pTcpConnection->ReceiveBufferSize = uRevisedRecvBufSize;

    /* revised send buffer length */
    uStatus = OpcUa_UInt32_BinaryDecode(&uRevisedSendBufSize, a_pInputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check the revised buffer sizes */
    /* This value shall not be larger than what the Client requested in the Hello Message */
    /* This value shall be greater or equal than 8 192 bytes (see 1.03 Errata) */
    if(uRevisedSendBufSize > pTcpConnection->SendBufferSize ||
       uRevisedSendBufSize < 8192)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadConnectionRejected);
    }
    pTcpConnection->SendBufferSize = uRevisedSendBufSize;

    /* revised max message size */
    uStatus = OpcUa_UInt32_BinaryDecode(&uRevisedMessageSize, a_pInputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check the revised max message length */
    if(uRevisedMessageSize != 0 &&
       (pTcpConnection->MaxMessageSize == 0 ||
        pTcpConnection->MaxMessageSize > uRevisedMessageSize))
    {
        /* accept smaller messages */
        pTcpConnection->MaxMessageSize = uRevisedMessageSize;
    }

    /* revised chunk count */
    uStatus = OpcUa_UInt32_BinaryDecode(&uRevisedMaxChunkCnt, a_pInputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check the revised chunk count */
    if(uRevisedMaxChunkCnt != 0 &&
       (pTcpConnection->MaxChunkCount == 0 ||
        pTcpConnection->MaxChunkCount > uRevisedMaxChunkCnt))
    {
        /* accept less chunks */
        pTcpConnection->MaxChunkCount = uRevisedMaxChunkCnt;
    }

    /** parsing finished **/
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Set:       SB:%u RB:%u\n",
        pTcpConnection->SendBufferSize,
        pTcpConnection->ReceiveBufferSize);

    if(pTcpConnection->NotifyCallback != OpcUa_Null)
    {
        OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);
        pTcpConnection->ConnectionState = OpcUa_TcpConnectionState_Connected;
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        pTcpConnection->NotifyCallback( a_pConnection,
                                        pTcpConnection->CallbackData,
                                        OpcUa_ConnectionEvent_Connect,
                                        OpcUa_Null, /* no stream for this event */
                                        uStatus);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pTcpConnection->NotifyCallback != OpcUa_Null)
    {
        OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);
        pTcpConnection->ConnectionState = OpcUa_TcpConnectionState_Connected;
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        pTcpConnection->NotifyCallback( a_pConnection,
                                        pTcpConnection->CallbackData,
                                        OpcUa_ConnectionEvent_Connect,
                                        OpcUa_Null, /* no stream for this event */
                                        uStatus);
    }

OpcUa_FinishErrorHandling;
} /* OpcUa_TcpConnection_ProcessAcknowledgeMessage */

/*============================================================================
 * OpcUa_TcpConnection_ProcessResponse
 *===========================================================================*/
/**
* @brief Handles a message (chunk) that has to be forwarded to the securechannel layer.
*/
static OpcUa_StatusCode OpcUa_TcpConnection_ProcessResponse(OpcUa_Connection*   a_pConnection,
                                                            OpcUa_InputStream*  a_pInputStream)
{
    OpcUa_TcpInputStream*   pTcpInputStream = OpcUa_Null;
    OpcUa_TcpConnection*    pTcpConnection  = OpcUa_Null;
    OpcUa_ConnectionEvent   eEvent          = OpcUa_ConnectionEvent_Invalid;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "ProcessResponse");

    OpcUa_GotoErrorIfArgumentNull(a_pConnection);
    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_GotoErrorIfArgumentNull(pTcpConnection);

    OpcUa_GotoErrorIfArgumentNull(a_pInputStream);
    pTcpInputStream = (OpcUa_TcpInputStream*)a_pInputStream->Handle;
    OpcUa_GotoErrorIfArgumentNull(pTcpInputStream);

    if(pTcpInputStream->IsAbort != OpcUa_False)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpConnection_ProcessResponse: Message aborted after %u chunks!\n", pTcpConnection->uCurrentChunk);
        eEvent = OpcUa_ConnectionEvent_ResponseAbort;
        pTcpConnection->uCurrentChunk = 0;
    }
    else /* no abort message */
    {
        if(pTcpInputStream->IsFinal != OpcUa_False)
        {
            /* final chunk in message, reset counter */
            eEvent = OpcUa_ConnectionEvent_Response;
            pTcpConnection->uCurrentChunk = 0;
        }
        else
        {
            /* intermediary chunk, test for limit and increment */
            pTcpConnection->uCurrentChunk++;
            eEvent = OpcUa_ConnectionEvent_ResponsePartial;

            if((pTcpConnection->MaxChunkCount != 0) && (pTcpConnection->uCurrentChunk >= pTcpConnection->MaxChunkCount))
            {
                /* this message will be too large */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpConnection_ProcessResponse: Chunk count limit exceeded!\n");
                eEvent = OpcUa_ConnectionEvent_Response; /* message final */
                uStatus = OpcUa_BadTcpMessageTooLarge;   /* with error */
            }
        }
    }

    if(OpcUa_IsGood(uStatus))
    {
        /* dispatch based on message type */
        switch(pTcpInputStream->MessageType)
        {
        case OpcUa_TcpStream_MessageType_SecureChannel:
            {
                /* It is the first and only or the last chunk of a row. Message Data begins here. */
                /* Notify the upper layer of this stream. */
                if(pTcpConnection->NotifyCallback != OpcUa_Null)
                {
                    /* the securechannel wants to start reading at the signature. */
                    pTcpInputStream->Buffer.Position = 0;

                    pTcpConnection->NotifyCallback( a_pConnection,
                                                    (OpcUa_Void*)pTcpConnection->CallbackData,
                                                    eEvent,
                                                    (OpcUa_InputStream**)&a_pInputStream,
                                                    OpcUa_Good);
                }

                break;
            }
        case OpcUa_TcpStream_MessageType_Error:
            {
                OpcUa_StatusCode    uReceivedStatusCode = OpcUa_Good;
                OpcUa_String        sReason             = OPCUA_STRING_STATICINITIALIZER;
                OpcUa_UInt32        uReasonLength;
                OpcUa_StringA       psReason;

                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpConnection_ProcessResponse: Error Message!\n");

                /* status code */
                uStatus = OpcUa_UInt32_BinaryDecode(&uReceivedStatusCode, a_pInputStream);
                OpcUa_GotoErrorIfBad(uStatus);

                uStatus = OpcUa_String_BinaryDecode(&sReason,
                                                    OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength,
                                                    a_pInputStream);
                OpcUa_GotoErrorIfBad(uStatus);
                OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);
                pTcpConnection->ConnectionState = OpcUa_TcpConnectionState_Error;
                OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpConnection_ProcessResponse: Status 0x%08x!\n", uReceivedStatusCode);
                uReasonLength = OpcUa_String_StrLen(&sReason);
                psReason = OpcUa_String_GetRawString(&sReason);
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpConnection_ProcessResponse: Reason %*.*s\n", uReasonLength, uReasonLength, psReason);
                OpcUa_String_Clear(&sReason);

                if(OpcUa_IsGood(uReceivedStatusCode))
                {
                    uReceivedStatusCode = OpcUa_Bad;
                }

                /* The message is finished at this point.*/
                if(pTcpConnection->NotifyCallback != OpcUa_Null)
                {
                    pTcpConnection->NotifyCallback(  a_pConnection,
                                                    (OpcUa_Void*)pTcpConnection->CallbackData,
                                                    OpcUa_ConnectionEvent_UnexpectedError,
                                                    OpcUa_Null,
                                                    uReceivedStatusCode);
                }

                break;
            }
        default:
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "ERROR: Message Type %d cannot be handled!\n", pTcpInputStream->MessageType);
                uStatus = OpcUa_BadInternalError;
                break;
            }
        }
    }
    else
    {
        /* The message is finished at this point.*/
        /* Send the abort request to upper layers. */
        if(pTcpConnection->NotifyCallback != OpcUa_Null)
        {
            pTcpConnection->NotifyCallback( a_pConnection,
                                            (OpcUa_Void*)pTcpConnection->CallbackData,
                                            eEvent,
                                            OpcUa_Null,
                                            uStatus);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
} /* OpcUa_TcpConnection_ProcessResponse */

/*============================================================================
 * OpcUa_TcpListener_DisconnectEventHandler
 *===========================================================================*/
/**
* @brief Gets called if the connection with the server gets broke (not the UATD -> ProcessDisconnectMessage).
*/
OpcUa_StatusCode OpcUa_TcpConnection_DisconnectEventHandler(OpcUa_Connection*   a_pConnection,
                                                            OpcUa_Socket        a_pSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "DisconnectEventHandler");

    OpcUa_ReferenceParameter(a_pSocket);

    /* Call the internal handler */
    uStatus = OpcUa_TcpConnection_HandleDisconnect(a_pConnection);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
} /* OpcUa_TcpConnection_DisconnectEventHandler */

/*============================================================================
 * OpcUa_TcpConnection_ExceptEventHandler
 *===========================================================================*/
/**
 * @brief Called by the socket callback when an except event occurred on the socket.
 *
 * This may happen ie. if a connect fails because the server is not reachable.
 * The event needs to be messaged to the upper layers.
 */
OpcUa_StatusCode OpcUa_TcpConnection_ExceptEventHandler(    OpcUa_Connection*   a_pConnection,
                                                            OpcUa_Socket        a_pSocket)
{
    OpcUa_TcpConnection* pTcpConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "ExceptEventHandler");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(a_pConnection);

    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

    OpcUa_GotoErrorIfArgumentNull(pTcpConnection);

    OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);

    if(pTcpConnection->ConnectionState == OpcUa_TcpConnectionState_Connected || pTcpConnection->ConnectionState == OpcUa_TcpConnectionState_Connecting)
    {
        pTcpConnection->ConnectionState = OpcUa_TcpConnectionState_Disconnected;
    }

    OPCUA_P_SOCKET_CLOSE(a_pSocket);
    pTcpConnection->Socket = OpcUa_Null;

    OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

    if(pTcpConnection->NotifyCallback != OpcUa_Null)
    {
        pTcpConnection->NotifyCallback(
            a_pConnection,
            pTcpConnection->CallbackData,
            OpcUa_ConnectionEvent_UnexpectedError, /* TODO: See if theres a better method. Secure layer also needs to be prepared for this. */
            OpcUa_Null, /* no stream for this event */
            OpcUa_BadCommunicationError);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
/*============================================================================
 * OpcUa_TcpConnection_ConnectEventHandler
 *===========================================================================*/
/**
 * @brief Called by the socket callback when a connect event occurred.
 */
OpcUa_StatusCode OpcUa_TcpConnection_ConnectEventHandler(       OpcUa_Connection*   a_pConnection,
                                                                OpcUa_Socket        a_pSocket)
{
    OpcUa_TcpConnection* pTcpConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "ConnectEventHandler");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(a_pConnection);

    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_GotoErrorIfArgumentNull(pTcpConnection);

    /* first time we get the new socket, so store it. */
    pTcpConnection->Socket = a_pSocket;

    /* now send the hello message and do the rest in response for the acknowledge message or after timeout. */
    uStatus = OpcUa_TcpConnection_SendHelloMessage(a_pConnection);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpConnection_ReadEventHandler
 *===========================================================================*/
/**
* @brief Gets called if data is available on the socket. The connection instance must be locked here!
*/
OpcUa_StatusCode OpcUa_TcpConnection_ReadEventHandler(
    OpcUa_Connection*   a_pConnection,
    OpcUa_Socket        a_pSocket)
{
    OpcUa_TcpConnection*    pTcpConnection   = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_TcpInputStream*   pTcpInputStream  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "ReadEventHandler");

    OpcUa_GotoErrorIfArgumentNull(a_pConnection);
    OpcUa_GotoErrorIfArgumentNull(a_pConnection->Handle);
    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

    /******************************************************************************************/

    /* check if a new stream needs to be created */
    if(pTcpConnection->IncomingStream == OpcUa_Null)
    {
        /* create a new input stream */
        uStatus = OpcUa_TcpStream_CreateInput(  a_pSocket,
                                                OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize,
                                                &(pTcpConnection->IncomingStream));
        OpcUa_GotoErrorIfBad(uStatus);

        pTcpInputStream = (OpcUa_TcpInputStream *)pTcpConnection->IncomingStream->Handle;
    }

    /******************************************************************************************/

    /* notify target stream about newly avaiable data */
    uStatus = OpcUa_TcpStream_DataReady(pTcpConnection->IncomingStream);

    /******************************************************************************************/

    if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: CallAgain result for stream %p on socket %p!\n", pTcpConnection->IncomingStream, a_pSocket);
    }
    else
    {
        pTcpInputStream = (OpcUa_TcpInputStream*)pTcpConnection->IncomingStream->Handle;

        if(OpcUa_IsBad(uStatus))
        {
            /* Error happened... */
            switch(uStatus)
            {
            case OpcUa_BadDecodingError:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: OpcUa_BadDecodingError for stream %p on socket %p! (Streamstate %d)\n", pTcpConnection->IncomingStream, a_pSocket, pTcpInputStream->State);
                    uStatus = OpcUa_TcpConnection_HandleDisconnect(a_pConnection);
                    break;
                }
            case OpcUa_BadDisconnect:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: OpcUa_BadDisconnect for stream %p on socket %p! (Streamstate %d)\n", pTcpConnection->IncomingStream, a_pSocket, pTcpInputStream->State);
                    uStatus = OpcUa_TcpConnection_HandleDisconnect(a_pConnection);
                    break;
                }
            case OpcUa_BadCommunicationError:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: OpcUa_BadCommunicationError for stream %p on socket %p! (Streamstate %d)\n", pTcpConnection->IncomingStream, a_pSocket, pTcpInputStream->State);
                    uStatus = OpcUa_TcpConnection_HandleDisconnect(a_pConnection);
                    break;
                }
            case OpcUa_BadConnectionClosed:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: OpcUa_BadConnectionClosed for stream %p on socket %p! (Streamstate %d)\n", pTcpConnection->IncomingStream, a_pSocket, pTcpInputStream->State);
                    uStatus = OpcUa_TcpConnection_HandleDisconnect(a_pConnection);
                    break;
                }
            default:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: Bad (%x) status for stream %p on socket %p! (Streamstate %d)\n", uStatus, pTcpConnection->IncomingStream, a_pSocket, pTcpInputStream->State);
                    uStatus = OpcUa_TcpConnection_HandleDisconnect(a_pConnection);
                }
            }
            pTcpConnection->IncomingStream->Close((OpcUa_Stream*)pTcpConnection->IncomingStream);
            pTcpConnection->IncomingStream->Delete((OpcUa_Stream**)&(pTcpConnection->IncomingStream));
        }
        else /* Message can be processed. */
        {
            /* process message (message types handled by the client are: ack, disconnect, requests, abort) */
            switch(pTcpInputStream->MessageType)
            {
            case OpcUa_TcpStream_MessageType_Acknowledge:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: MessageType ACKNOWLEDGE\n");
                    uStatus = OpcUa_TcpConnection_ProcessAcknowledgeMessage(a_pConnection, pTcpConnection->IncomingStream);

                    /* this stream is parsed completely and can be deleted */
                    pTcpConnection->IncomingStream->Close((OpcUa_Stream*)(pTcpConnection->IncomingStream));
                    pTcpConnection->IncomingStream->Delete((OpcUa_Stream**)&(pTcpConnection->IncomingStream));

                    break;
                }
            case OpcUa_TcpStream_MessageType_Error:
            case OpcUa_TcpStream_MessageType_SecureChannel:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: MessageType MESSAGE\n");

                    uStatus = OpcUa_TcpConnection_ProcessResponse(a_pConnection, pTcpConnection->IncomingStream);

                    pTcpConnection->IncomingStream->Close((OpcUa_Stream*)pTcpConnection->IncomingStream);
                    pTcpConnection->IncomingStream->Delete((OpcUa_Stream**)&pTcpConnection->IncomingStream);
                    break;
                }
            default:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_ReadEventHandler: Invalid MessageType (%d)\n", pTcpInputStream->MessageType);

                    pTcpConnection->IncomingStream->Close((OpcUa_Stream*)pTcpConnection->IncomingStream);
                    pTcpConnection->IncomingStream->Delete((OpcUa_Stream**)&pTcpConnection->IncomingStream);
                    break;
                }
            }

        }
    }
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_TcpConnection_HandleDisconnect(a_pConnection);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_WriteEventHandler
 *===========================================================================*/
/**
* @brief Gets called if data can be written to the socket.
*/
OpcUa_StatusCode OpcUa_TcpConnection_WriteEventHandler(
    OpcUa_Connection*   a_pConnection,
    OpcUa_Socket        a_pSocket)
{
    OpcUa_TcpConnection*    pTcpConnection   = (OpcUa_TcpConnection*)a_pConnection->Handle;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "WriteEventHandler");

    OpcUa_GotoErrorIfArgumentNull(a_pConnection);
    OpcUa_GotoErrorIfArgumentNull(a_pConnection->Handle);
    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    /******************************************************************************************************/

    /* look for pending output stream */
    if(pTcpConnection != OpcUa_Null)
    {
        do {
            while(pTcpConnection->pSendQueue != OpcUa_Null)
            {
                OpcUa_BufferList *pCurrentBuffer = pTcpConnection->pSendQueue;
                OpcUa_Int32 iDataLength = pCurrentBuffer->Buffer.EndOfData - pCurrentBuffer->Buffer.Position;
                OpcUa_Int32 iDataWritten = OPCUA_P_SOCKET_WRITE(a_pSocket,
                                                                &pCurrentBuffer->Buffer.Data[pCurrentBuffer->Buffer.Position],
                                                                iDataLength,
                                                                OpcUa_False);
                if(iDataWritten<0)
                {
                    return OpcUa_TcpConnection_Disconnect(a_pConnection, OpcUa_True);
                }
                else if(iDataWritten<iDataLength)
                {
                    pCurrentBuffer->Buffer.Position += iDataWritten;
                    OpcUa_ReturnStatusCode;
                }
                else
                {
                    pTcpConnection->pSendQueue = pCurrentBuffer->pNext;
                    OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
                    OpcUa_Free(pCurrentBuffer);
                }
            } /* end while */

            if(pTcpConnection->NotifyCallback != OpcUa_Null)
            {
                pTcpConnection->NotifyCallback( a_pConnection,
                                                (OpcUa_Void*)pTcpConnection->CallbackData,
                                                OpcUa_ConnectionEvent_RefillSendQueue,
                                                OpcUa_Null,
                                                uStatus);
            }

        } while(pTcpConnection->pSendQueue != OpcUa_Null);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_SocketCallback
 *===========================================================================*/
/** @brief This function gets called if a network event occurred. */
static OpcUa_StatusCode OpcUa_TcpConnection_SocketCallback( OpcUa_Socket    a_pSocket,
                                                            OpcUa_UInt32    a_SocketEvent,
                                                            OpcUa_Void*     a_pUserData,
                                                            OpcUa_UInt16    a_nPortNumber,
                                                            OpcUa_Boolean   a_bIsSSL)
{
    OpcUa_TcpConnection_EventHandler    fEventHandler   = OpcUa_Null;
    OpcUa_Connection*                   connection      = (OpcUa_Connection*)a_pUserData;
    OpcUa_TcpConnection*                pTcpConnection  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "SocketCallback");

    OpcUa_ReferenceParameter(a_nPortNumber);
    OpcUa_ReferenceParameter(a_bIsSSL);

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(connection);

    pTcpConnection = (OpcUa_TcpConnection*)connection->Handle;
    OpcUa_GotoErrorIfArgumentNull(pTcpConnection);

#if 0 /* debug code */
    {
        OpcUa_StringA                       strEvent        = OpcUa_Null;
        switch(a_SocketEvent)
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
                strEvent = "OPCUA_SOCKET_NEED_BUFFER";
                break;
            }
        case OPCUA_SOCKET_FREE_BUFFER_EVENT:
            {
                strEvent = "OPCUA_SOCKET_FREE_BUFFER";
                break;
            }
        default:
            {
                strEvent = "ERROR DEFAULT!";
                break;
            }
        }

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, " * OpcUa_TcpConnection_SocketCallback: Socket(%x), Port(%d), Data(%d), Event(%s)\n", a_pSocket, a_nPortNumber, a_pUserData, strEvent);
    }
#endif /* debug code end */

    OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);
    if(    pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connected  /* wait for disconnect during error state */
        && pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connecting /* wait for disconnect during error state */
        && pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Error)     /* wait for disconnect during error state */
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, " * OpcUa_TcpConnection_SocketCallback: Ignoring Socket(%p) Event(%u) due state %u!\n", a_pSocket, a_SocketEvent, pTcpConnection->ConnectionState);
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        return OpcUa_Good;
    }
    OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

    switch(a_SocketEvent)
    {
    case OPCUA_SOCKET_READ_EVENT:
        {
            /* notifies an existing stream about new data or creates a new stream */
            fEventHandler = OpcUa_TcpConnection_ReadEventHandler;
            break;
        }
    case OPCUA_SOCKET_WRITE_EVENT:
        {
            fEventHandler = OpcUa_TcpConnection_WriteEventHandler;
            break;
        }
    case OPCUA_SOCKET_EXCEPT_EVENT:
        {
            fEventHandler = OpcUa_TcpConnection_ExceptEventHandler;
            break;
        }
    case OPCUA_SOCKET_TIMEOUT_EVENT:
        {
            break;
        }
    case OPCUA_SOCKET_CLOSE_EVENT:
        {
            break;
        }
    case OPCUA_SOCKET_CONNECT_EVENT:
        {
            fEventHandler = OpcUa_TcpConnection_ConnectEventHandler;
            break;
        }
    case OPCUA_SOCKET_NO_EVENT:
        {
            break;
        }
    case OPCUA_SOCKET_SHUTDOWN_EVENT:
        {
            break;
        }
    case OPCUA_SOCKET_NEED_BUFFER_EVENT:
        {
            /* fEventHandler = OpcUa_TcpConnection_NeedBufferEventHandler; */
            break;
        }
    case OPCUA_SOCKET_FREE_BUFFER_EVENT:
        {
            /* fEventHandler = OpcUa_TcpConnection_FreeBufferEventHandler; */
            break;
        }
    case OPCUA_SOCKET_ACCEPT_EVENT:
    default:
        {
            /* TODO: define some errorhandling here! */
            break;
        }
    }

    /* call the internal specialized event handler */
    if(fEventHandler != OpcUa_Null)
    {
        uStatus = fEventHandler(connection, a_pSocket);
        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpConnection_SocketCallback: Handler returned error 0x%08X!\n", uStatus);
        }
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, " * OpcUa_TcpConnection_SocketCallback: Event Handler returned.\n");

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_Connect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_Connect(
    struct _OpcUa_Connection*       a_pConnection,
    OpcUa_String*                   a_sUrl,
    OpcUa_ClientCredential*         a_pCredential,
    OpcUa_UInt32                    a_Timeout,
    OpcUa_Connection_PfnOnNotify*   a_pfnCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_TcpConnection* pTcpConnection   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "Connect");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_sUrl);
    OpcUa_ReturnErrorIfArgumentNull(a_pfnCallback);

    OpcUa_ReferenceParameter(a_pCredential);
    OpcUa_ReferenceParameter(a_Timeout);
    OpcUa_ReferenceParameter(a_pCallbackData);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_TcpConnection, a_pConnection, Connect);

    pTcpConnection   = (OpcUa_TcpConnection*)a_pConnection->Handle;

    pTcpConnection->NotifyCallback   = a_pfnCallback;
    pTcpConnection->CallbackData     = a_pCallbackData;
    pTcpConnection->ConnectionState  = OpcUa_TcpConnectionState_Connecting;

    OpcUa_String_StrnCpy(&pTcpConnection->sURL, a_sUrl, OPCUA_STRING_LENDONTCARE);

#if OPCUA_MULTITHREADED
    uStatus = OPCUA_P_SOCKETMANAGER_CREATECLIENT(   pTcpConnection->SocketManager,      /* socketmanager handle */
                                                    OpcUa_String_GetRawString(a_sUrl),  /* remote address */
                                                    0,                                  /* local port */
                                                    OpcUa_TcpConnection_SocketCallback, /* callback function */
                                                    (OpcUa_Void*)a_pConnection,         /* callback data */
                                                    &(pTcpConnection->Socket));         /* retreiving socket handle */
#else /* OPCUA_MULTITHREADED */
    uStatus = OPCUA_P_SOCKETMANAGER_CREATECLIENT(   OpcUa_Null,                         /* socketmanager handle */
                                                    OpcUa_String_GetRawString(a_sUrl),  /* remote address */
                                                    0,                                  /* local port */
                                                    OpcUa_TcpConnection_SocketCallback, /* callback function */
                                                    (OpcUa_Void*)a_pConnection,         /* callback data */
                                                    &(pTcpConnection->Socket));         /* retreiving socket handle */
#endif /* OPCUA_MULTITHREADED */
    OpcUa_GotoErrorIfBad(uStatus);

    /* tell the caller to expect a callback (only for non-blocking sockets)*/
    uStatus = OpcUa_GoodCompletesAsynchronously;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    pTcpConnection->ConnectionState = OpcUa_TcpConnectionState_Disconnected;

OpcUa_FinishErrorHandling;
}

/*===========================================================================
 * OpcUa_TcpConnection_Disconnect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_Disconnect(OpcUa_Connection* a_pConnection,
                                                OpcUa_Boolean     a_bNotifyOnComplete)
{
    OpcUa_TcpConnection*    tcpConnection   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "Disconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);

    tcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

    OPCUA_P_MUTEX_LOCK(tcpConnection->ReadMutex);

    /* check, if the connection is in the right state for being disconnected */
    if(    tcpConnection->ConnectionState == OpcUa_TcpConnectionState_Connected
        || tcpConnection->ConnectionState == OpcUa_TcpConnectionState_Connecting)
    {
        /* first: set state */
        tcpConnection->ConnectionState = OpcUa_TcpConnectionState_Disconnected;

        /* blind close without error checking */
        OPCUA_P_SOCKET_CLOSE(tcpConnection->Socket);
        tcpConnection->Socket = OpcUa_Null;
        OPCUA_P_MUTEX_UNLOCK(tcpConnection->ReadMutex);

        tcpConnection->DisconnectTime = OPCUA_P_DATETIME_UTCNOW();

        /* close socket and update connection handle. */
        if(a_bNotifyOnComplete)
        {
            if(tcpConnection->NotifyCallback != OpcUa_Null)
            {
                tcpConnection->NotifyCallback(
                    a_pConnection,                      /* source of event  */
                    tcpConnection->CallbackData,        /* callback data    */
                    OpcUa_ConnectionEvent_Disconnect,   /* the event type   */
                    OpcUa_Null,                         /* the stream       */
                    OpcUa_Good);                        /* the statuscode   */
            }
            else
            {
                /* Notify requested but no callback supplied. */
                uStatus = OpcUa_BadInvalidArgument;
            }
        }
    }
    else
    {
        OPCUA_P_MUTEX_UNLOCK(tcpConnection->ReadMutex);
        return OpcUa_BadInvalidState;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_BeginReceiveResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_BeginReceiveResponse(
    OpcUa_Connection*   a_pConnection,
    OpcUa_InputStream** a_ppInputStream)
{
    OpcUa_TcpConnection*    tcpConnection   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "BeginReceiveResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppInputStream);
    OpcUa_ReturnErrorIfInvalidConnection(a_pConnection);

    tcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

    /* need to reinitialize the IncomingStreamData structure as required */
    uStatus = OpcUa_TcpStream_CreateInput(  tcpConnection->Socket,
                                            (tcpConnection->ReceiveBufferSize==(OpcUa_UInt32)0)?(OpcUa_UInt32)OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize:tcpConnection->ReceiveBufferSize,
                                            a_ppInputStream);

    OpcUa_GotoErrorIfBad(uStatus);

    tcpConnection->IncomingStream = *a_ppInputStream;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_BeginSendMessage
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_TcpConnection_BeginSendRequest(
    OpcUa_Connection*    a_pConnection,
    OpcUa_OutputStream** a_ppOutputStream)
{
    OpcUa_TcpConnection* pTcpConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "BeginSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);

    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

    OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);

#if OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM

    if(pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connected)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        OpcUa_GotoErrorWithStatus(OpcUa_BadConnectionClosed);
    }

    /* create a tcp output stream based on the tcpConnection */
    uStatus = OpcUa_TcpStream_CreateOutput( pTcpConnection->Socket,
                                            OpcUa_TcpStream_MessageType_SecureChannel,
                                            OpcUa_Null,
                                            pTcpConnection->SendBufferSize,
                                            OpcUa_TcpConnection_ConnectionDisconnectCB,
                                            pTcpConnection->MaxChunkCount,
                                            a_ppOutputStream);
    if(OpcUa_IsBad(uStatus))
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        OpcUa_GotoErrorWithStatus(uStatus);
    }

#else

    if(pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connected)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        OpcUa_GotoErrorWithStatus(OpcUa_BadConnectionClosed);
    }

    /* reuse outgoing stream */
    if(pTcpConnection->OutgoingStream == OpcUa_Null)
    {
        if(pTcpConnection->bOutgoingStreamIsUsed != OpcUa_False)
        {
            OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpConnection_BeginSendRequest: Used outstream detected!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
        }

        /* create a tcp output stream based on the tcpConnection */
        uStatus = OpcUa_TcpStream_CreateOutput( pTcpConnection->Socket,
                                                OpcUa_TcpStream_MessageType_SecureChannel,
                                                OpcUa_Null,
                                                pTcpConnection->SendBufferSize,
                                                OpcUa_TcpConnection_ConnectionDisconnectCB,
                                                pTcpConnection->MaxChunkCount,
                                                a_ppOutputStream);

        if(OpcUa_IsBad(uStatus))
        {
            OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
            OpcUa_GotoErrorWithStatus(uStatus);
        }

        pTcpConnection->OutgoingStream = *a_ppOutputStream;
    }
    else
    {
        *a_ppOutputStream = pTcpConnection->OutgoingStream;
    }

    pTcpConnection->bOutgoingStreamIsUsed = OpcUa_True;

#endif

    OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM
    OpcUa_TcpStream_Delete((OpcUa_Stream**)a_ppOutputStream);
#endif

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_EndSendMessage
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_EndSendRequest(OpcUa_Connection*               a_pConnection,
                                                    OpcUa_OutputStream**            a_ppOutputStream,
                                                    OpcUa_UInt32                    a_uTimeout,
                                                    OpcUa_Connection_PfnOnResponse* a_pfnCallback,
                                                    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_TcpConnection*    pTcpConnection   = OpcUa_Null;
    OpcUa_TcpOutputStream*  pTcpOutputStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "EndSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOutputStream);

    OpcUa_ReturnErrorIfInvalidConnection(a_pConnection);

    /* not supported at this layer */
    OpcUa_ReferenceParameter(a_uTimeout);
    OpcUa_ReferenceParameter(a_pfnCallback);
    OpcUa_ReferenceParameter(a_pCallbackData);

    /* cast onto the backend types */
    pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_GotoErrorIfArgumentNull(pTcpConnection);

    pTcpOutputStream = (OpcUa_TcpOutputStream*)(*a_ppOutputStream)->Handle;
    OpcUa_GotoErrorIfArgumentNull(pTcpOutputStream);

#if OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM

    OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);
    if(pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connected)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

        /* clean up stream resources */
        (*a_ppOutputStream)->Delete((OpcUa_Stream**)a_ppOutputStream);

        return OpcUa_BadConnectionClosed;
    }
    OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

    /* close and flush stream */
    uStatus = (*a_ppOutputStream)->Close((OpcUa_Stream*)(*a_ppOutputStream));
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpConnection_EndSendRequest: close failed! 0x%08X \n", uStatus);
    }

    /* clean up stream resources */
    (*a_ppOutputStream)->Delete((OpcUa_Stream**)a_ppOutputStream);

#else

    OPCUA_P_MUTEX_LOCK(pTcpConnection->ReadMutex);
    /* check for consistency */
    if(     pTcpConnection->OutgoingStream == OpcUa_Null
        ||  pTcpConnection->bOutgoingStreamIsUsed == OpcUa_False)
    {
        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpConnection_EndSendRequest: no outgoing stream\n");
        OpcUa_GotoError;
    }

    if(pTcpConnection->ConnectionState != OpcUa_TcpConnectionState_Connected)
    {
        /* mark stream as available */
        pTcpConnection->bOutgoingStreamIsUsed = OpcUa_False;

        OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

        /* unlink up stream resources */
        *a_ppOutputStream = OpcUa_Null;

        return OpcUa_BadConnectionClosed;
    }

    /* close and flush stream */
    uStatus = (*a_ppOutputStream)->Flush(   (OpcUa_OutputStream*)(*a_ppOutputStream),
                                            OpcUa_True);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpConnection_EndSendRequest: close failed! 0x%08X \n", uStatus);
    }

    /* TODO: check if it is needed to reset stream buffer state! */

    /* mark stream as available */
    pTcpConnection->bOutgoingStreamIsUsed = OpcUa_False;

    /* unlink up stream resources */
    *a_ppOutputStream = OpcUa_Null;

    OPCUA_P_MUTEX_UNLOCK(pTcpConnection->ReadMutex);

#endif

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_AbortSendRequest
 *===========================================================================*/
/* INFO: null streams are allowed and say that the owner of the connection
         takes care about the stream itself. Only if non null the tcp transport
         generates an abort message. this is not handled by the ua stack because
         abort messages are always secured. */
OpcUa_StatusCode OpcUa_TcpConnection_AbortSendRequest(  OpcUa_Connection*    a_pConnection,
                                                        OpcUa_StatusCode     a_uStatus,
                                                        OpcUa_String*        a_psReason,
                                                        OpcUa_OutputStream** a_ppOutputStream)
{
    OpcUa_TcpConnection*    tcpConnection  = OpcUa_Null;
    OpcUa_TcpOutputStream*   tcpOutputStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "AbortSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfInvalidConnection(a_pConnection);

    /* cast onto the backend types */
    tcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;
    OpcUa_GotoErrorIfArgumentNull(tcpConnection);

#if OPCUA_TCPCONNECTION_DELETE_REQUEST_STREAM

    if(a_ppOutputStream != OpcUa_Null && *a_ppOutputStream != OpcUa_Null)
    {
        tcpOutputStream = (OpcUa_TcpOutputStream*)(*a_ppOutputStream)->Handle;
        OpcUa_GotoErrorIfArgumentNull(tcpOutputStream);

        /* clean up */
        OpcUa_TcpStream_Delete((OpcUa_Stream**)a_ppOutputStream);
    }
    else
    {
        /* no insecure abort messages implemented and allowed! */
        OpcUa_ReferenceParameter(a_uStatus);
        OpcUa_ReferenceParameter(a_psReason);
    }

#else

    OPCUA_P_MUTEX_LOCK(tcpConnection->ReadMutex);

    /* clean outgoing stream */
    if(tcpConnection->bOutgoingStreamIsUsed != OpcUa_False)
    {
        tcpConnection->bOutgoingStreamIsUsed = OpcUa_False;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpConnection_AbortSendRequest: no active stream detected!\n");
    }

    if(a_ppOutputStream != OpcUa_Null)
    {
        tcpOutputStream = (OpcUa_TcpOutputStream*)(*a_ppOutputStream)->Handle;
        OpcUa_GotoErrorIfArgumentNull(tcpOutputStream);

        /* TODO: reset stream buffer state! */

        a_ppOutputStream = OpcUa_Null;
    }
    else
    {
        /* no insecure abort messages implemented and allowed! */
        OpcUa_ReferenceParameter(a_uStatus);
        OpcUa_ReferenceParameter(a_psReason);
    }

    OPCUA_P_MUTEX_UNLOCK(tcpConnection->ReadMutex);

#endif
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 OpcUa_TcpConnection_GetReceiveBufferSize
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_GetReceiveBufferSize(  OpcUa_Connection*   a_pConnection,
                                                            OpcUa_UInt32*       a_pBufferSize)
{
    OpcUa_TcpConnection* tcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "GetReceiveBufferSize");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pBufferSize);

    *a_pBufferSize = tcpConnection->ReceiveBufferSize;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_AddToSendQueue
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_AddToSendQueue(  OpcUa_Connection* a_pConnection,
                                                      OpcUa_BufferList* a_pBufferList,
                                                      OpcUa_UInt32      a_uFlags)
{
    OpcUa_TcpConnection* pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

OpcUa_InitializeStatus(OpcUa_Module_TcpListener, "AddToSendQueue");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReferenceParameter(a_uFlags);

    if(pTcpConnection->pSendQueue == OpcUa_Null)
    {
        pTcpConnection->pSendQueue = a_pBufferList;
    }
    else
    {
        OpcUa_BufferList* pLastEntry = pTcpConnection->pSendQueue;
        while(pLastEntry->pNext != OpcUa_Null)
        {
            pLastEntry = pLastEntry->pNext;
        }
        pLastEntry->pNext = a_pBufferList;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_CheckProtocolVersion
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_CheckProtocolVersion(OpcUa_Connection* a_pConnection,
                                                          OpcUa_UInt32      a_uProtocolVersion)
{
    OpcUa_TcpConnection* pTcpConnection = (OpcUa_TcpConnection*)a_pConnection->Handle;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "CheckProtocolVersion");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);

    if(a_uProtocolVersion != pTcpConnection->uProtocolVersion)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadProtocolVersionUnsupported);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpConnection_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_TcpConnection_Delete(OpcUa_Connection** a_ppConnection)
{
    OpcUa_TcpConnection*    tcpConnection       = OpcUa_Null;

    if(a_ppConnection == OpcUa_Null)
    {
        return;
    }

    if(*a_ppConnection == OpcUa_Null)
    {
        return;
    }

    tcpConnection = (OpcUa_TcpConnection*)(*a_ppConnection)->Handle;
    if(tcpConnection == OpcUa_Null)
    {
        return;
    }

    /* this is a call potentially called in a thread outside the receive thread, lock the connection */
#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_LOCK(tcpConnection->ReadMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
    if(tcpConnection->ConnectionState != OpcUa_TcpConnectionState_Disconnected)
    {
        /* blind close without error checking */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpConnection_Delete: Rude disconnect!\n");
        tcpConnection->ConnectionState = OpcUa_TcpConnectionState_Disconnected;
        OPCUA_P_SOCKET_CLOSE(tcpConnection->Socket);
        tcpConnection->Socket = OpcUa_Null;
    }

#if OPCUA_MULTITHREADED

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpConnection_Delete: Stopping communication.\n");

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(tcpConnection->ReadMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* HINT: waits internally for receive thread to shutdown, so this call may block. */
    if(tcpConnection->SocketManager != OpcUa_Null)
    {
        OPCUA_P_SOCKETMANAGER_DELETE(&(tcpConnection->SocketManager));
    }
#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_LOCK(tcpConnection->ReadMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpConnection_Delete: Communication stopped.\n");

#endif /* OPCUA_MULTITHREADED */

    OpcUa_String_Clear(&tcpConnection->sURL);

    /* the architecture should prevent from getting here with active streams */
    if(tcpConnection->IncomingStream != OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpConnection_Delete: INVALID STATE! Active Streams! Internal Error!\n");
        tcpConnection->IncomingStream->Close((OpcUa_Stream*)tcpConnection->IncomingStream);
        tcpConnection->IncomingStream->Delete((OpcUa_Stream**)&tcpConnection->IncomingStream);
    }

    while(tcpConnection->pSendQueue != OpcUa_Null)
    {
        OpcUa_BufferList* pCurrentBuffer = tcpConnection->pSendQueue;
        tcpConnection->pSendQueue = pCurrentBuffer->pNext;
        OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
        OpcUa_Free(pCurrentBuffer);
    }

    /*** Free ***/
    /* clean internal resources */
#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(tcpConnection->ReadMutex);
    OPCUA_P_MUTEX_DELETE(&(tcpConnection->ReadMutex));
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* the connection implementation */
    OpcUa_Free(tcpConnection);

    /* the wrapper element */
    OpcUa_Free(*a_ppConnection);
    *a_ppConnection = OpcUa_Null;
}

/*============================================================================
 * OpcUa_TcpConnection_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_Create(OpcUa_Connection** a_ppConnection)
{
    OpcUa_TcpConnection*    tcpConnection   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_ppConnection);
    *a_ppConnection = OpcUa_Null;

    /* allocate handle that stores internal state information */
    tcpConnection = (OpcUa_TcpConnection*)OpcUa_Alloc(sizeof(OpcUa_TcpConnection));
    OpcUa_ReturnErrorIfAllocFailed(tcpConnection);

    /* initialize with null */
    OpcUa_MemSet(tcpConnection, 0, sizeof(OpcUa_TcpConnection));

    /* Todo: move from OpcUa_Connection! */
#if OPCUA_MULTITHREADED
    tcpConnection->SocketManager        = 0;
#endif /* OPCUA_MULTITHREADED */

    tcpConnection->SanityCheck          = OpcUa_TcpConnection_SanityCheck;

    tcpConnection->uProtocolVersion     = 0;
    tcpConnection->SendBufferSize       = OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize;
    tcpConnection->ReceiveBufferSize    = OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize;
    tcpConnection->MaxMessageSize       = OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength;
    tcpConnection->MaxChunkCount        = OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount;
    tcpConnection->uCurrentChunk        = 0;

    tcpConnection->ConnectionState      = OpcUa_TcpConnectionState_Disconnected;

    uStatus = OPCUA_P_MUTEX_CREATE(&(tcpConnection->ReadMutex));
    OpcUa_ReturnErrorIfBad(uStatus);

    OpcUa_String_Initialize(&tcpConnection->sURL);

    /* allocate external connection object */
    *a_ppConnection = (OpcUa_Connection*)OpcUa_Alloc(sizeof(OpcUa_Connection));
    OpcUa_GotoErrorIfAllocFailed(*a_ppConnection);
    OpcUa_MemSet(*a_ppConnection, 0, sizeof(OpcUa_Connection));

#if OPCUA_MULTITHREADED
    /* create the socket manager */
    uStatus = OPCUA_P_SOCKETMANAGER_CREATE( &(tcpConnection->SocketManager),
                                            1,
                                            OPCUA_SOCKET_NO_FLAG);

    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_MULTITHREADED */

    (*a_ppConnection)->Handle               = tcpConnection;
    (*a_ppConnection)->Connect              = OpcUa_TcpConnection_Connect;
    (*a_ppConnection)->Disconnect           = OpcUa_TcpConnection_Disconnect;
    (*a_ppConnection)->BeginSendRequest     = OpcUa_TcpConnection_BeginSendRequest;
    (*a_ppConnection)->EndSendRequest       = OpcUa_TcpConnection_EndSendRequest;
    (*a_ppConnection)->AbortSendRequest     = OpcUa_TcpConnection_AbortSendRequest;
    (*a_ppConnection)->GetReceiveBufferSize = OpcUa_TcpConnection_GetReceiveBufferSize;
    (*a_ppConnection)->Delete               = OpcUa_TcpConnection_Delete;
    (*a_ppConnection)->AddToSendQueue       = OpcUa_TcpConnection_AddToSendQueue;
    (*a_ppConnection)->CheckProtocolVersion = OpcUa_TcpConnection_CheckProtocolVersion;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_DELETE(&(tcpConnection->ReadMutex));
    OpcUa_Free(tcpConnection);

    if(a_ppConnection)
    {
        OpcUa_Free(*a_ppConnection);
        *a_ppConnection = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_CLIENTAPI */
