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
#ifdef OPCUA_HAVE_HTTPSAPI

#include <opcua_mutex.h>
#include <opcua_datetime.h>
#include <opcua_socket.h>
#include <opcua_utilities.h>
#include <opcua_list.h>
#include <opcua_timer.h>

/* types */
#include <opcua_builtintypes.h>
#include <opcua_httpsstream.h>

/* internals */
#include <opcua_https_internal.h>

/* self */
#include <opcua_httpsconnection.h>

/*============================================================================
 * configuration
 *===========================================================================*/
/** @brief If set to yes, Connect tries to establish a tcp connection to the host immediately. */
#define OPCUA_HTTPSCONNECTION_CONNECT_IMMEDIATELY       OPCUA_CONFIG_YES

/** @brief If set to yes, Connect tries to establish a tcp connection to the host immediately. */
#define OPCUA_HTTPSCONNECTION_CONNECTION_KEEP_ALIVE     OPCUA_CONFIG_YES

/* Should PKI data be passed down to TLS layer? */
#define OPCUA_HTTPSCONNECTION_USE_TLS_CREDENTIALS       OPCUA_CONFIG_YES

/* Set to yes, if requests should be protected by a mutex. */
#define OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS      OPCUA_CONFIG_YES

/*============================================================================
 * auto config and helper macros
 *===========================================================================*/
/* disable OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS if synchronization is off */
#if !OPCUA_USE_SYNCHRONISATION
# undef OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS
# define OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS     OPCUA_CONFIG_NO
#endif /* !OPCUA_USE_SYNCHRONISATION */

/* Request lock and unlock helper. */
#if OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS
# define OPCUA_HTTPSCONNECTION_REQUEST_LOCK(xRequest)   OPCUA_P_MUTEX_LOCK((xRequest)->Mutex)
# define OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(xRequest) OPCUA_P_MUTEX_UNLOCK((xRequest)->Mutex)
#else
# define OPCUA_HTTPSCONNECTION_REQUEST_LOCK(xRequest)
# define OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(xRequest)
#endif /* OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS */

/*============================================================================
 * OpcUa_HttpsConnectionState
 *===========================================================================*/
 /** @brief The states a HttpConnection can be in. */
typedef enum _OpcUa_HttpsConnectionState
{
    /** @brief Error state. */
    OpcUa_HttpsConnectionState_Invalid,
    /** @brief Connection object connecting. */
    OpcUa_HttpsConnectionState_Connecting,
    /** @brief Prepared request is waiting for connection. */
    OpcUa_HttpsConnectionState_RequestPrepared,
    /** @brief Connection is established, communication to the server is possible. */
    OpcUa_HttpsConnectionState_Connected,
    /** @brief Connection is established and upper layer is preparing a request. */
    OpcUa_HttpsConnectionState_PreparingForRequest,
    /** @brief Connection is established and a request has been sent. */
    OpcUa_HttpsConnectionState_WaitingForResponse,
    /** @brief The session was closed gracefully with a disconnect message. */
    OpcUa_HttpsConnectionState_Disconnected,
    /** @brief An error message was received and the connection is inactive. */
    OpcUa_HttpsConnectionState_Error
} OpcUa_HttpsConnectionState;

/*============================================================================
 * OpcUa_HttpsRequest
 *===========================================================================*/
/** @brief Represents an HTTPS UA request. */
typedef struct _OpcUa_HttpsConnection_Request
{
    /** @brief Uplink to the parent connection object. */
    OpcUa_Connection*               pConnection;
    /** @brief The state of the connection to the server. */
    OpcUa_HttpsConnectionState      ConnectionState;
#if OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS
    /** @brief Synchronizing access to this request and socket. */
    OpcUa_Mutex                     Mutex;
#endif /* OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS */
    /** @brief The socket holding the connection to the server. */
    OpcUa_Socket                    Socket;
    /** @brief An active datastream being received (message). */
    OpcUa_InputStream*              IncomingStream;
    /** @brief An active datastream being sent (message). */
    OpcUa_OutputStream*             OutgoingStream;
    /** @brief The time when the request was sent to the server. */
    OpcUa_UInt32                    RequestStartTime;
    /** @brief The time when the request is no longer valid. */
    OpcUa_UInt32                    RequestTimeout;
    /*! The callback to use when the request completes. */
    OpcUa_Connection_PfnOnResponse* RequestCallback;
    /*! The data to pass with the callback. */
    OpcUa_Void*                     RequestCallbackData;
    /*! Shall the connection status notification callback be invoked?. */
    OpcUa_Boolean                   bNotify;
    /** @brief The queued list of data blocks to be sent. */
    OpcUa_BufferList*               pSendQueue;
} OpcUa_HttpsConnection_Request;

/*============================================================================
 * OpcUa_HttpsConnection
 *===========================================================================*/
/** @brief Holds all data needed to manage a http connection to an ua server. */
typedef struct _OpcUa_HttpsConnection
{
    /** @brief Internal helper to verify instances. */
    OpcUa_UInt32                    SanityCheck;
    /** @brief Messaging events to the holder of this connection. */
    OpcUa_Connection_PfnOnNotify*   NotifyCallback;
    /** @brief Data to pass back with the callback. */
    OpcUa_Void*                     CallbackData;
#if OPCUA_USE_SYNCHRONISATION
    /** @brief Synchronizing access to this connection. */
    OpcUa_Mutex                     Mutex;
#endif /* OPCUA_USE_SYNCHRONISATION */
    /** @brief The complete URL of the server to connect to. */
    OpcUa_String                    sURL;
    /** @brief The cached host part of the URL. */
    OpcUa_String                    sHostName;
    /** @brief The cached path part of the URL. */
    OpcUa_String                    sResourcePath;
    /** @brief The requested security policy. */
    OpcUa_String                    sSecurityPolicy;
    /*! @brief Watchdog for the requests. */
    OpcUa_Timer                     hWatchdogTimer;
#if OPCUA_MULTITHREADED
    /*! @brief Holds the socket for this connection, the thread and is the central waiting point. */
    OpcUa_SocketManager             SocketManager;
#endif /* OPCUA_MULTITHREADED */
    /** @brief Certificate used for SSL/TLS connections. */
    OpcUa_ByteString*               pCertificate;
    /** @brief Private key used for SSL/TLS connections.*/
    OpcUa_Key                       PrivateKey;
    /** @brief PKI configuration for SSL/TLS connections. */
    OpcUa_Void*                     pPkiConfig;
    /** @brief Requested Server Certificate for SSL/TLS connections. */
    OpcUa_ByteString*               pServerCertificate;
    /** @brief Actually used Server Certificate for SSL/TLS connections. */
    OpcUa_ByteString                bsUsedServerCertificate;
    /** @brief hResult from Server Certificate Validation. */
    OpcUa_StatusCode                hValidationResult;
    /*! @brief Array of pending requests. */
    OpcUa_HttpsConnection_Request   arrHttpsRequests[OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS];
}
OpcUa_HttpsConnection;

/*============================================================================
 * OpcUa_HttpsConnection_SanityCheck
 *===========================================================================*/
#define OpcUa_HttpsConnection_SanityCheck 0x3C439750

/*============================================================================
 * OpcUa_ReturnErrorIfInvalidConnection
 *===========================================================================*/
#define OpcUa_ReturnErrorIfInvalidConnection(xConnection) \
if (((OpcUa_HttpsConnection*)(xConnection)->Handle)->SanityCheck != OpcUa_HttpsConnection_SanityCheck) \
{ \
    return OpcUa_BadInvalidArgument; \
}

/*============================================================================
 * OpcUa_HttpsConnection_EventHandler Type Definition
 *===========================================================================*/
/** @brief Internal handler prototype. */
typedef OpcUa_StatusCode (*OpcUa_HttpsConnection_EventHandler)(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_Socket                    a_hSocket);

OpcUa_StatusCode OpcUa_HttpsConnection_Disconnect(
    OpcUa_Connection*   a_pConnection,
    OpcUa_Boolean       a_bNotifyOnComplete);

/*============================================================================
 * OpcUa_HttpsConnection_WatchdogTimerCallback
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_HttpsConnection_WatchdogTimerCallback( OpcUa_Void*     a_pvCallbackData,
                                                                            OpcUa_Timer     a_hTimer,
                                                                            OpcUa_UInt32    a_msecElapsed)
{

    OpcUa_Connection*       pConnection         = OpcUa_Null;
    OpcUa_HttpsConnection*  pHttpsConnection    = OpcUa_Null;
    OpcUa_UInt32            CurrentTime         = 0;
    OpcUa_UInt32            uIndex              = 0;

    OpcUa_ReferenceParameter(a_msecElapsed);
    OpcUa_ReferenceParameter(a_hTimer);

    pConnection         = (OpcUa_Connection*)a_pvCallbackData;
    pHttpsConnection    = (OpcUa_HttpsConnection*)pConnection->Handle;

    OPCUA_P_MUTEX_LOCK(pHttpsConnection->Mutex);

    CurrentTime = OpcUa_GetTickCount();

    /* for each pending request */
    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        OpcUa_HttpsConnection_Request* pRequest = &pHttpsConnection->arrHttpsRequests[uIndex];

        OPCUA_HTTPSCONNECTION_REQUEST_LOCK(pRequest);

        /* pHttpsConnection->OperationTimeout: absolute count of milliseconds    */
        /* pHttpsConnection->StartTime:        start time in millisecond ticks   */
        /* uTime                               current time in millisecond ticks */
        if(     pRequest->RequestCallback != OpcUa_Null /* request has to be in use */
            &&  pRequest->RequestTimeout != OPCUA_INFINITE /* timeout has to be non infinite */
            &&  pRequest->RequestTimeout != 0 /* zero means infinite as in previous versions */
            &&  (OpcUa_Int32)(CurrentTime - pRequest->RequestStartTime) >= (OpcUa_Int32)pRequest->RequestTimeout
          )
        {
            OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                            "OpcUa_HttpsConnection_WatchdogTimerCallback: Request timed out after %u msecs!\n",
                            CurrentTime - pRequest->RequestStartTime);

            /* tell all waiting callbacks of the cancellation */
            if(pRequest->RequestCallback != OpcUa_Null)
            {
                OpcUa_Connection_PfnOnResponse* pfnRequestCallback      = pRequest->RequestCallback;
                OpcUa_Void*                     pvRequestCallbackData   = pRequest->RequestCallbackData;

                pRequest->RequestCallback       = OpcUa_Null;
                pRequest->RequestCallbackData   = OpcUa_Null;

                OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);

                pfnRequestCallback( pConnection,            /* source of the event      */
                                    pvRequestCallbackData,  /* the callback data        */
                                    OpcUa_BadTimeout,       /* status of the request    */
                                    OpcUa_Null);            /* the stream to read from  */
            }
            else
            {
                OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
            }
        }
        else
        {
            if(    pRequest->RequestCallback == OpcUa_Null && uIndex != 0
               &&  pRequest->ConnectionState == OpcUa_HttpsConnectionState_Connected
               &&  (OpcUa_Int32)(CurrentTime - pRequest->RequestStartTime) >= OPCUA_HTTPSCONNECTION_KEEP_ALIVE_TIMEOUT)
            {
                pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;

                OPCUA_P_SOCKET_CLOSE(pRequest->Socket);
                pRequest->Socket = OpcUa_Null;

                pRequest->OutgoingStream = OpcUa_Null;

                if(pRequest->IncomingStream != OpcUa_Null)
                {
                    pRequest->IncomingStream->Close((OpcUa_Stream*)pRequest->IncomingStream);
                    pRequest->IncomingStream->Delete((OpcUa_Stream**)&pRequest->IncomingStream);
                    pRequest->IncomingStream = OpcUa_Null;
                }

                while(pRequest->pSendQueue != OpcUa_Null)
                {
                    OpcUa_BufferList* pCurrentBuffer = pRequest->pSendQueue;
                    pRequest->pSendQueue = pCurrentBuffer->pNext;
                    OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
                    OpcUa_Free(pCurrentBuffer);
                }
            }

            OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
        }
    }

    OPCUA_P_MUTEX_UNLOCK(pHttpsConnection->Mutex);

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_HttpsConnection_WatchdogTimerKillCallback
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_HttpsConnection_WatchdogTimerKillCallback( OpcUa_Void*     a_pvCallbackData,
                                                                                OpcUa_Timer     a_hTimer,
                                                                                OpcUa_UInt32    a_msecElapsed)
{
    OpcUa_HttpsConnection*  pHttpsConnection    = OpcUa_Null;
    OpcUa_UInt32            uIndex              = 0;

    OpcUa_ReferenceParameter(a_msecElapsed);
    OpcUa_ReferenceParameter(a_hTimer);

    pHttpsConnection = (OpcUa_HttpsConnection*)(((OpcUa_Connection*)a_pvCallbackData)->Handle);

    /* for each pending request */
    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        OpcUa_HttpsConnection_Request* pRequest = &pHttpsConnection->arrHttpsRequests[uIndex];

        OPCUA_HTTPSCONNECTION_REQUEST_LOCK(pRequest);

        /* tell all waiting callbacks of the cancellation */
        if(pRequest->RequestCallback != OpcUa_Null)
        {
            OpcUa_Connection_PfnOnResponse* pfnRequestCallback      = pRequest->RequestCallback;
            OpcUa_Void*                     pvRequestCallbackData   = pRequest->RequestCallbackData;

            pRequest->RequestCallback       = OpcUa_Null;
            pRequest->RequestCallbackData   = OpcUa_Null;

            OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);

            pfnRequestCallback( (OpcUa_Connection*)a_pvCallbackData,    /* source of the event      */
                                pvRequestCallbackData,                  /* the callback data        */
                                OpcUa_BadDisconnect,                    /* status of the request    */
                                OpcUa_Null);                            /* the stream to read from  */
        }
        else
        {
            OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
        }
    }

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_HttpsConnection_GetRequestByStream
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_GetRequestByStream(  OpcUa_HttpsConnection*          a_pHttpConnection,
                                                            OpcUa_OutputStream*             a_pOutputStream,
                                                            OpcUa_HttpsConnection_Request** a_ppRequest)
{
    OpcUa_HttpsConnection_Request*  pRequest    = OpcUa_Null;
    OpcUa_UInt32                    uIndex      = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "GetRequestByStream");

    OpcUa_ReturnErrorIfArgumentNull(a_pHttpConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_ppRequest);

    *a_ppRequest = OpcUa_Null;

    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        pRequest = &a_pHttpConnection->arrHttpsRequests[uIndex];

        OPCUA_HTTPSCONNECTION_REQUEST_LOCK(pRequest);

        if(pRequest->OutgoingStream == a_pOutputStream)
        {
            *a_ppRequest = pRequest;
            break;
        }

        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

    if(*a_ppRequest == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotFound);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_GetRequestInState
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_GetRequestInState(   OpcUa_HttpsConnection*            a_pHttpConnection,
                                                            OpcUa_HttpsConnectionState        a_eState,
                                                            OpcUa_HttpsConnection_Request**   a_ppRequest)
{
    OpcUa_HttpsConnection_Request*  pRequest    = OpcUa_Null;
    OpcUa_UInt32                    uIndex      = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "GetRequestInState");

    OpcUa_ReturnErrorIfArgumentNull(a_pHttpConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppRequest);

    *a_ppRequest = OpcUa_Null;

    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        pRequest = &a_pHttpConnection->arrHttpsRequests[uIndex];

        OPCUA_HTTPSCONNECTION_REQUEST_LOCK(pRequest);

        if(pRequest->ConnectionState == a_eState)
        {
            *a_ppRequest = pRequest;
            break;
        }

        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

    if(*a_ppRequest == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotFound);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Handling a disconnect from the server.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsConnection_HandleDisconnect(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_StatusCode                a_uReason)
{
    OpcUa_Connection*      pConnection     = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "HandleDisconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest->pConnection);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_HandleDisconnect: request %p!\n", a_pRequest);

    pConnection     = a_pRequest->pConnection;

    /* mark the connection as closed */
    OPCUA_HTTPSCONNECTION_REQUEST_LOCK(a_pRequest);

    OPCUA_P_SOCKET_CLOSE(a_pRequest->Socket);
    a_pRequest->Socket = OpcUa_Null;

    if(a_pRequest->ConnectionState == OpcUa_HttpsConnectionState_RequestPrepared)
    {
        a_pRequest->OutgoingStream->Delete((OpcUa_Stream**)&a_pRequest->OutgoingStream);
    }

    a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;

    a_pRequest->OutgoingStream = OpcUa_Null;

    if(a_pRequest->IncomingStream != OpcUa_Null)
    {
        a_pRequest->IncomingStream->Close((OpcUa_Stream*)a_pRequest->IncomingStream);
        a_pRequest->IncomingStream->Delete((OpcUa_Stream**)&a_pRequest->IncomingStream);
        a_pRequest->IncomingStream = OpcUa_Null;
    }

    while(a_pRequest->pSendQueue != OpcUa_Null)
    {
        OpcUa_BufferList* pCurrentBuffer = a_pRequest->pSendQueue;
        a_pRequest->pSendQueue = pCurrentBuffer->pNext;
        OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
        OpcUa_Free(pCurrentBuffer);
    }

    /* notify upper layer about disconnect */
    if(a_pRequest->RequestCallback != OpcUa_Null)
    {
        OpcUa_Connection_PfnOnResponse* pfnRequestCallback      = a_pRequest->RequestCallback;
        OpcUa_Void*                     pvRequestCallbackData   = a_pRequest->RequestCallbackData;

        a_pRequest->RequestCallback        = OpcUa_Null;
        a_pRequest->RequestCallbackData    = OpcUa_Null;

        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_HandleDisconnect: notify!\n");

        pfnRequestCallback( pConnection,
                            pvRequestCallbackData,
                            a_uReason,
                            OpcUa_Null); /* no stream for this event */

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_HandleDisconnect: notify done!\n");
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_HandleDisconnect: no notification possible!\n");
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);
    }

    /* if the first connection fails, shutdown completely */
    if(a_pRequest->bNotify != OpcUa_False)
    {
        OpcUa_HttpsConnection_Disconnect(pConnection, OpcUa_True);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_ConvertHttpStatusCode
 *===========================================================================*/
/**
 * @brief Handles a message (chunk) that has to be forwarded to the owner.
 */
static OpcUa_StatusCode OpcUa_HttpsConnection_ConvertHttpStatusCode(
    OpcUa_Stream*       a_pStream,
    OpcUa_StatusCode*   a_puStatusCode)
{
    OpcUa_UInt32 uMessageStatus = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "ConvertHttpStatusCode");

    /* get HTTP status code */
    uStatus = OpcUa_HttpsStream_GetStatusCode(a_pStream, &uMessageStatus);

    switch(uMessageStatus)
    {
    case OPCUA_HTTP_STATUS_OK:                              {*a_puStatusCode = OpcUa_Good; break;}
    case OPCUA_HTTP_STATUS_MULTIPLE_CHOICES:                {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_MOVED_PERMANENTLY:               {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_MOVED_TEMPORARILY:               {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_SEE_OTHER:                       {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_NOT_MODIFIED:                    {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_USE_PROXY:                       {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_TEMPORARY_REDIRECT:              {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_BAD_REQUEST:                     {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_UNAUTHORIZED:                    {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_PAYMENT_REQUIRED:                {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_FORBIDDEN:                       {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_NOT_FOUND:                       {*a_puStatusCode = OpcUa_BadNotFound; break;}
    case OPCUA_HTTP_STATUS_METHOD_NOT_ALLOWED:              {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_NOT_ACCEPTABLE:                  {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED:   {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_REQUEST_TIMEOUT:                 {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_CONFLICT:                        {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_GONE:                            {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_LENGTH_REQUIRED:                 {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_PRECONDITION_FAILED:             {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE:        {*a_puStatusCode = OpcUa_BadRequestTooLarge; break;}
    case OPCUA_HTTP_STATUS_REQUEST_URI_TOO_LONG:            {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE:          {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_UNPROCESSABLE_ENTITY:            {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_INTERNAL_SERVER_ERROR:           {*a_puStatusCode = OpcUa_BadInternalError; break;}
    case OPCUA_HTTP_STATUS_NOT_IMPLEMENTED:                 {*a_puStatusCode = OpcUa_BadNotImplemented; break;}
    case OPCUA_HTTP_STATUS_BAD_GATEWAY:                     {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_SERVICE_UNAVAILABLE:             {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    case OPCUA_HTTP_STATUS_GATEWAY_TIMEOUT:                 {*a_puStatusCode = OpcUa_BadTimeout; break;}
    case OPCUA_HTTP_STATUS_VERSION_NOT_SUPPORTED:           {*a_puStatusCode = OpcUa_BadCommunicationError; break;}
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsConnection_ConvertHttpStatusCode: unhandled HTTP status code (%u) returned!\n", uMessageStatus);
            *a_puStatusCode = OpcUa_Bad;
            OpcUa_GotoError;
        }
    }

    if(OpcUa_IsBad(*a_puStatusCode))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsConnection_ConvertHttpStatusCode: HTTP error (%u) mapped to 0x%08X !\n", uMessageStatus, *a_puStatusCode);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
/*============================================================================
 * OpcUa_HttpsConnection_ProcessResponse
 *===========================================================================*/
/**
 * @brief Handles a message (chunk) that has to be forwarded to the owner.
 */
static OpcUa_StatusCode OpcUa_HttpsConnection_ProcessResponse(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_InputStream*              a_pInputStream)
{
    OpcUa_String sHeaderValue = OPCUA_STRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "ProcessResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest->pConnection);

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ProcessResponse: Response for request %p\n", a_pRequest);

    uStatus = OpcUa_HttpsStream_GetHeader(  (OpcUa_Stream*)a_pInputStream,
                                            OpcUa_String_FromCString("Connection"),
                                            OpcUa_False,
                                            &sHeaderValue);

    if(OpcUa_IsEqual(OpcUa_GoodNoData))
    {
        a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_Connected;
    }
    else
    {
        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_HttpsConnection_ProcessResponse: Error looking for header \"Connection\"!\n");
        }
        else
        {
            if(!OpcUa_String_StrnCmp(&sHeaderValue, OpcUa_String_FromCString("close"), OPCUA_STRING_LENDONTCARE, OpcUa_True))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ProcessResponse: Connection close!\n");

                a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;
                OPCUA_P_SOCKET_CLOSE(a_pRequest->Socket);
                a_pRequest->Socket = OpcUa_Null;
            }
            else
            {
#if OPCUA_HTTPSCONNECTION_CONNECTION_KEEP_ALIVE
                if(!OpcUa_String_StrnCmp(&sHeaderValue, OpcUa_String_FromCString("keep-alive"), OPCUA_STRING_LENDONTCARE, OpcUa_True))
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ProcessResponse: Connection keep-alive!\n");
                }
                else
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ProcessResponse: Connection header with unexpected value!\n");
                }

                a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_Connected;
                a_pRequest->RequestStartTime = OpcUa_GetTickCount();
#else
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_HttpsConnection_ProcessResponse: Connection header with unexpected value!\n");
#endif
            }

            OpcUa_String_Clear(&sHeaderValue);
        }
    }

    if(a_pRequest->RequestCallback != OpcUa_Null)
    {
        OpcUa_Connection_PfnOnResponse* pfnRequestCallback      = a_pRequest->RequestCallback;
        OpcUa_Void*                     pvRequestCallbackData   = a_pRequest->RequestCallbackData;
        OpcUa_UInt32                    uMessageStatus          = 0;

        a_pRequest->RequestCallback     = OpcUa_Null;
        a_pRequest->RequestCallbackData = OpcUa_Null;

        OpcUa_HttpsConnection_ConvertHttpStatusCode(    (OpcUa_Stream*)a_pInputStream,
                                                        &uMessageStatus);

        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

        if(OpcUa_IsGood(uMessageStatus))
        {
            OpcUa_String HeaderValue = OPCUA_STRING_STATICINITIALIZER;

            uStatus = OpcUa_HttpsStream_GetHeader(  (OpcUa_Stream*)a_pInputStream,
                                                    OpcUa_String_FromCString("Content-Type"),
                                                    OpcUa_False,
                                                   &HeaderValue);

            if(     OpcUa_IsBad(uStatus)
                ||  OpcUa_String_IsEmpty(   &HeaderValue)
                ||  OpcUa_String_IsNull(    &HeaderValue)
                ||  OpcUa_String_StrnCmp(   &HeaderValue,
                                            OpcUa_String_FromCString("application/octet-stream"),
                                            OPCUA_STRING_LENDONTCARE,
                                            OpcUa_False)
              )
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_HttpsConnection_ProcessResponse: content-type missing!\n");
                pfnRequestCallback( a_pRequest->pConnection,
                                    (OpcUa_Void*)pvRequestCallbackData,
                                    OpcUa_BadEncodingError,
                                    OpcUa_Null);
            }
            else
            {
                pfnRequestCallback( a_pRequest->pConnection,
                                    (OpcUa_Void*)pvRequestCallbackData,
                                    uMessageStatus,
                                    (OpcUa_InputStream**)&a_pInputStream);
            }

            OpcUa_String_Clear(&HeaderValue);
        }
        else
        {
            pfnRequestCallback( a_pRequest->pConnection,
                                (OpcUa_Void*)pvRequestCallbackData,
                                uMessageStatus,
                                OpcUa_Null);
        }

        OPCUA_HTTPSCONNECTION_REQUEST_LOCK(a_pRequest);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_ExceptEventHandler
 *===========================================================================*/
/**
 * @brief Called by the socket callback when an except event occurred on the socket.
 *
 * This may happen ie. if a connect fails because the server is not reachable.
 * The event needs to be messaged to the upper layers.
 */
OpcUa_StatusCode OpcUa_HttpsConnection_ExceptEventHandler(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_Socket                    a_hSocket)
{
    OpcUa_HttpsConnection*          pHttpConnection         = OpcUa_Null;
    OpcUa_Connection_PfnOnResponse* pfnRequestCallback      = OpcUa_Null;
    OpcUa_Void*                     pvRequestCallbackData   = OpcUa_Null;
    OpcUa_Connection*               pConnection             = OpcUa_Null;
    OpcUa_Boolean                   bIsConnecting           = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "ExceptEventHandler");

    OpcUa_ReturnErrorIfArgumentNull(a_hSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest->pConnection);

    OPCUA_HTTPSCONNECTION_REQUEST_LOCK(a_pRequest);

    if(a_pRequest->Socket != a_hSocket)
    {
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsConnection_ExceptEventHandler: wrong socket %p on request %p (expected %p)!\n", a_hSocket, a_pRequest, a_pRequest->Socket);
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pConnection     = a_pRequest->pConnection;
    pHttpConnection = (OpcUa_HttpsConnection*)pConnection->Handle;

    if(a_pRequest->ConnectionState == OpcUa_HttpsConnectionState_Connecting)
    {
        bIsConnecting = OpcUa_True;
    }
    else if(a_pRequest->ConnectionState == OpcUa_HttpsConnectionState_RequestPrepared)
    {
        a_pRequest->OutgoingStream->Delete((OpcUa_Stream**)&a_pRequest->OutgoingStream);
    }

    a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;

    OPCUA_P_SOCKET_CLOSE(a_hSocket);
    a_pRequest->Socket = OpcUa_Null;

    a_pRequest->OutgoingStream = OpcUa_Null;

    if(a_pRequest->IncomingStream != OpcUa_Null)
    {
        a_pRequest->IncomingStream->Close((OpcUa_Stream*)a_pRequest->IncomingStream);
        a_pRequest->IncomingStream->Delete((OpcUa_Stream**)&a_pRequest->IncomingStream);
        a_pRequest->IncomingStream = OpcUa_Null;
    }

    while(a_pRequest->pSendQueue != OpcUa_Null)
    {
        OpcUa_BufferList* pCurrentBuffer = a_pRequest->pSendQueue;
        a_pRequest->pSendQueue = pCurrentBuffer->pNext;
        OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
        OpcUa_Free(pCurrentBuffer);
    }

    pfnRequestCallback      = a_pRequest->RequestCallback;
    pvRequestCallbackData   = a_pRequest->RequestCallbackData;

    a_pRequest->RequestCallback        = OpcUa_Null;
    a_pRequest->RequestCallbackData    = OpcUa_Null;

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

    if(pfnRequestCallback != OpcUa_Null)
    {
        pfnRequestCallback( pConnection,
                            pvRequestCallbackData,
                            OpcUa_BadCommunicationError,
                            OpcUa_Null); /* no stream for this event */
    }

    if(a_pRequest->bNotify != OpcUa_False)
    {
        if(bIsConnecting != OpcUa_False)
        {
            OpcUa_HttpsConnection_Disconnect(pConnection, OpcUa_False);

            if(pHttpConnection->NotifyCallback != OpcUa_Null)
            {
                pHttpConnection->NotifyCallback(pConnection,
                                                pHttpConnection->CallbackData,
                                                OpcUa_ConnectionEvent_Connect,
                                                OpcUa_Null, /* no stream for this event */
                                                OpcUa_BadCommunicationError);
            }
        }
        else
        {
            OpcUa_HttpsConnection_Disconnect(pConnection, OpcUa_True);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_WriteEventHandler
 *===========================================================================*/
/**
* @brief Gets called if data can be written to the socket.
*/
OpcUa_StatusCode OpcUa_HttpsConnection_WriteEventHandler(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_Socket                    a_pSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpConnection, "WriteEventHandler");

    OpcUa_GotoErrorIfArgumentNull(a_pRequest);
    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    OPCUA_HTTPSCONNECTION_REQUEST_LOCK(a_pRequest);

    /* look for pending output stream */
    while(a_pRequest->pSendQueue != OpcUa_Null)
    {
        OpcUa_BufferList*        pCurrentBuffer = a_pRequest->pSendQueue;
        OpcUa_Int32              iDataLength    = pCurrentBuffer->Buffer.EndOfData - pCurrentBuffer->Buffer.Position;

        OpcUa_Int32              iDataWritten   = OPCUA_P_SOCKET_WRITE(a_pRequest->Socket,
                                                                       &pCurrentBuffer->Buffer.Data[pCurrentBuffer->Buffer.Position],
                                                                       iDataLength,
                                                                       OpcUa_False);
        if(iDataWritten < 0)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadCommunicationError);
        }
        else if(iDataWritten == 0)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_WriteEventHandler: no data sent\n");
            OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);
            uStatus = OpcUa_GoodCallAgain;
            OpcUa_ReturnStatusCode;
        }
        else if(iDataWritten < iDataLength)
        {
            pCurrentBuffer->Buffer.Position += iDataWritten;

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_WriteEventHandler: data partially sent (%i bytes)!\n", iDataWritten);

            OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);
            uStatus = OpcUa_GoodCallAgain;
            OpcUa_ReturnStatusCode;
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_WriteEventHandler: data sent!\n");
            a_pRequest->pSendQueue = pCurrentBuffer->pNext;
            OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
            OpcUa_Free(pCurrentBuffer);
        }
    } /* end while */

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

    OpcUa_HttpsConnection_HandleDisconnect(a_pRequest, uStatus);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnectionRequest_AddToSendQueue
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsConnectionRequest_AddToSendQueue(OpcUa_HttpsConnection_Request*  a_pRequest,
                                                                    OpcUa_BufferList*               a_pBufferList)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpListener, "AddToSendQueue");

    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);

    if(a_pRequest->pSendQueue == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnectionRequest_AddToSendQueue: Set buffer list of request %p to %p.\n", a_pRequest, a_pBufferList);
        a_pRequest->pSendQueue = a_pBufferList;
    }
    else
    {
        OpcUa_BufferList* pLastEntry = a_pRequest->pSendQueue;
        while(pLastEntry->pNext != OpcUa_Null)
        {
            pLastEntry = pLastEntry->pNext;
        }

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnectionRequest_AddToSendQueue: Append buffer list %p to request %p.\n", a_pBufferList, a_pRequest);

        pLastEntry->pNext = a_pBufferList;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_AddStreamToSendQueue
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsConnection_AddStreamToSendQueue(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_OutputStream*             a_pOutputStream)
{
    OpcUa_BufferList*        pEntry = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "OpcUa_HttpsConnection_AddStreamToSendQueue");

    pEntry = OpcUa_Alloc(sizeof(OpcUa_BufferList));
    OpcUa_GotoErrorIfAllocFailed(pEntry);

    do
    {
        if(pEntry != OpcUa_Null)
        {
            pEntry->pNext = OpcUa_Null;
            uStatus = a_pOutputStream->DetachBuffer((OpcUa_Stream*)a_pOutputStream, &pEntry->Buffer);

            if(OpcUa_IsGood(uStatus))
            {
                uStatus = OpcUa_HttpsConnectionRequest_AddToSendQueue(  a_pRequest,
                                                                        pEntry);
                if(OpcUa_IsBad(uStatus))
                {
                    /* free Entry */
                    OpcUa_Buffer_Clear(&pEntry->Buffer);
                    OpcUa_Free(pEntry);
                    break;
                }
                else
                {
                    /* continue with next entry */
                    pEntry = OpcUa_Alloc(sizeof(OpcUa_BufferList));
                }
            }
            else
            {
                /* free entry */
                OpcUa_Free(pEntry);

                if(OpcUa_IsEqual(OpcUa_BadNoData))
                {
                    /* mask error - everything went fine, all buffers detached */
                    uStatus = OpcUa_Good;
                }
                break;
            }
        }
        else
        {
            uStatus = OpcUa_BadOutOfMemory;
        }

    } while(OpcUa_IsGood(uStatus));

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_GetUsedServerCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_GetUsedServerCertificate(
    OpcUa_Connection*                       a_pConnection,
    OpcUa_ByteString**                      a_ppUsedServerCertificate,
    OpcUa_StatusCode*                       a_pValidationResult)
{
    OpcUa_HttpsConnection* pHttpsConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "GetUsedServerCertificate");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_ppUsedServerCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_ppUsedServerCertificate);

    pHttpsConnection = (OpcUa_HttpsConnection*)a_pConnection->Handle;

    OpcUa_ReturnErrorIfTrue(pHttpsConnection->bsUsedServerCertificate.Data == OpcUa_Null,
                            OpcUa_BadInvalidState);
    *a_ppUsedServerCertificate = &pHttpsConnection->bsUsedServerCertificate;
    *a_pValidationResult       = pHttpsConnection->hValidationResult;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_SslEventHandler
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_SslEventHandler( OpcUa_Socket        a_hSocket,
                                                        OpcUa_Void*         a_pUserData,
                                                        OpcUa_ByteString*   a_pCertificate,
                                                        OpcUa_StatusCode    a_uResult)
{
    OpcUa_HttpsConnection_Request*  pRequest            = OpcUa_Null;
    OpcUa_HttpsConnection*          pHttpsConnection    = OpcUa_Null;
    OpcUa_ByteString*               pServerCertificate  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "SslEventHandler");

    OpcUa_ReferenceParameter(a_hSocket);
    OpcUa_ReferenceParameter(a_uResult);

    OpcUa_ReturnErrorIfArgumentNull(a_pUserData);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);

    pRequest           = (OpcUa_HttpsConnection_Request*)a_pUserData;
    pHttpsConnection   = (OpcUa_HttpsConnection*)pRequest->pConnection->Handle;
    pServerCertificate = pHttpsConnection->pServerCertificate;

    if(a_uResult == OpcUa_BadNotSupported && pHttpsConnection->pPkiConfig != OpcUa_Null
       && ((OpcUa_P_OpenSSL_CertificateStore_Config*)pHttpsConnection->pPkiConfig)->PkiType == OpcUa_NO_PKI)
    {
        /* ignore errors from PkiType OpcUa_NO_PKI */
        uStatus = OpcUa_BadContinue;
    }

    if(pServerCertificate != OpcUa_Null && pServerCertificate->Length > 0
       && pServerCertificate->Data != OpcUa_Null)
    {
        if(a_pCertificate->Length != pServerCertificate->Length
           || OpcUa_MemCmp(a_pCertificate->Data, pServerCertificate->Data,
                           pServerCertificate->Length))
        {
            uStatus = OpcUa_BadCertificateInvalid;
        }
    }
    else if(pHttpsConnection->bsUsedServerCertificate.Data != OpcUa_Null)
    {
        if(a_pCertificate->Length != pHttpsConnection->bsUsedServerCertificate.Length
           || OpcUa_MemCmp(a_pCertificate->Data, pHttpsConnection->bsUsedServerCertificate.Data,
                           pHttpsConnection->bsUsedServerCertificate.Length))
        {
            uStatus = OpcUa_BadCertificateInvalid;
        }
    }
    else
    {
        pHttpsConnection->bsUsedServerCertificate.Data = OpcUa_Alloc(a_pCertificate->Length);
        OpcUa_GotoErrorIfAllocFailed(pHttpsConnection->bsUsedServerCertificate.Data);
        pHttpsConnection->bsUsedServerCertificate.Length = a_pCertificate->Length;
        OpcUa_MemCpy(pHttpsConnection->bsUsedServerCertificate.Data,
                     pHttpsConnection->bsUsedServerCertificate.Length,
                     a_pCertificate->Data, a_pCertificate->Length);
        pHttpsConnection->hValidationResult = a_uResult;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_ConnectEventHandler
 *===========================================================================*/
/**
 * @brief Called by the socket callback when a connect event occurred.
 */
OpcUa_StatusCode OpcUa_HttpsConnection_ConnectEventHandler(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_Socket                    a_hSocket)
{
    OpcUa_HttpsConnection* pHttpConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "ConnectEventHandler");

    OpcUa_ReturnErrorIfArgumentNull(a_hSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest->pConnection);

    pHttpConnection = (OpcUa_HttpsConnection*)a_pRequest->pConnection->Handle;

    OpcUa_ReturnErrorIfArgumentNull(pHttpConnection);

    OPCUA_HTTPSCONNECTION_REQUEST_LOCK(a_pRequest);

    if(a_pRequest->Socket != a_hSocket)
    {
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsConnection_ConnectEventHandler: wrong socket %p on request %p (expected %p)!\n", a_hSocket, a_pRequest, a_pRequest->Socket);
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    if(a_pRequest->ConnectionState == OpcUa_HttpsConnectionState_RequestPrepared)
    {
        OpcUa_OutputStream* pOutputStream = a_pRequest->OutgoingStream;

        /* progress normally */
        a_pRequest->OutgoingStream = OpcUa_Null;
        a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_WaitingForResponse;

        OpcUa_HttpsStream_SetSocket(    pOutputStream,
                                        a_pRequest->Socket);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ConnectEventHandler: Sending buffered request!\n");

        /* close and flush stream */
        uStatus = pOutputStream->Close((OpcUa_Stream*)pOutputStream);
        if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
        {
            /* try to put stream content into buffer queue for delayed sending */
            uStatus = OpcUa_HttpsConnection_AddStreamToSendQueue(a_pRequest, pOutputStream);
        }

        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_HttpsConnection_ConnectEventHandler: could not send request! 0x%08X \n", uStatus);
        }

        /* clean up stream resources */
        pOutputStream->Delete((OpcUa_Stream**)&pOutputStream);
    }
    else
    {
        a_pRequest->ConnectionState = OpcUa_HttpsConnectionState_Connected;
    }

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

    if(a_pRequest->bNotify != OpcUa_False)
    {
        if(pHttpConnection->NotifyCallback != OpcUa_Null)
        {
            pHttpConnection->NotifyCallback(a_pRequest->pConnection,
                                            pHttpConnection->CallbackData,
                                            OpcUa_ConnectionEvent_Connect,
                                            OpcUa_Null, /* no stream for this event */
                                            uStatus);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_ReadEventHandler
 *===========================================================================*/
/**
 * @brief Gets called if data is available on the socket. The connection instance must be locked here!
 */
OpcUa_StatusCode OpcUa_HttpsConnection_ReadEventHandler(
    OpcUa_HttpsConnection_Request*  a_pRequest,
    OpcUa_Socket                    a_hSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "ReadEventHandler");

    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest->pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest->pConnection->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_hSocket);

    OPCUA_HTTPSCONNECTION_REQUEST_LOCK(a_pRequest);

    /******************************************************************************************/

    /* check if a new stream needs to be created */
    if(a_pRequest->IncomingStream == OpcUa_Null)
    {
        /* create a new input stream */
        uStatus = OpcUa_HttpsStream_CreateInput(a_hSocket,
                                               OpcUa_HttpsStream_MessageType_Response,
                                               &(a_pRequest->IncomingStream));
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /******************************************************************************************/

    /* notify target stream about newly available data */
    uStatus = OpcUa_HttpsStream_DataReady(a_pRequest->IncomingStream);

    /******************************************************************************************/

    if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: CallAgain result for stream %p on socket %p!\n", a_pRequest->IncomingStream, a_hSocket);
    }
    else
    {
        if(OpcUa_IsBad(uStatus))
        {
            /* Error happened... */
            switch(uStatus)
            {
                case OpcUa_BadDecodingError:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: OpcUa_BadDecodingError for stream %p on socket %p!\n", a_pRequest->IncomingStream, a_hSocket);
                    break;
                }
                case OpcUa_BadDisconnect:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: OpcUa_BadDisconnect for stream %p on socket %p!\n", a_pRequest->IncomingStream, a_hSocket);
                    break;
                }
                case OpcUa_BadCommunicationError:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: OpcUa_BadCommunicationError for stream %p on socket %p!\n", a_pRequest->IncomingStream, a_hSocket);
                    break;
                }
                case OpcUa_BadConnectionClosed:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: OpcUa_BadConnectionClosed for stream %p on socket %p!\n", a_pRequest->IncomingStream, a_hSocket);
                    break;
                }
                default:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: Bad (%x) status for stream %p on socket %p!\n", uStatus, a_pRequest->IncomingStream, a_hSocket);
                }
            }

            a_pRequest->IncomingStream->Close((OpcUa_Stream*)a_pRequest->IncomingStream);
            a_pRequest->IncomingStream->Delete((OpcUa_Stream**)&(a_pRequest->IncomingStream));

            OpcUa_GotoError;
        }
        else /* Message can be processed. */
        {
            OpcUa_HttpsStream_MessageType    eMessageType    = OpcUa_HttpsStream_MessageType_Unknown;
            OpcUa_HttpsStream_GetMessageType((OpcUa_Stream*)a_pRequest->IncomingStream, &eMessageType);

            if(eMessageType == OpcUa_HttpsStream_MessageType_Response)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: MessageType RESPONSE\n");

                uStatus = OpcUa_HttpsConnection_ProcessResponse(a_pRequest, a_pRequest->IncomingStream);

                if(a_pRequest->IncomingStream != OpcUa_Null)
                {
                    a_pRequest->IncomingStream->Close((OpcUa_Stream*)a_pRequest->IncomingStream);
                    a_pRequest->IncomingStream->Delete((OpcUa_Stream**)&a_pRequest->IncomingStream);
                    a_pRequest->IncomingStream = OpcUa_Null;
                }
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_ReadEventHandler: Invalid MessageType (%d)\n", eMessageType);

                a_pRequest->IncomingStream->Close((OpcUa_Stream*)a_pRequest->IncomingStream);
                a_pRequest->IncomingStream->Delete((OpcUa_Stream**)&a_pRequest->IncomingStream);
            }
        }
    }

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(a_pRequest);

    OpcUa_HttpsConnection_HandleDisconnect(a_pRequest, uStatus);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_SocketCallback
 *===========================================================================*/
/** @brief This function gets called if a network event occurred. */
static OpcUa_StatusCode OpcUa_HttpsConnection_SocketCallback(
    OpcUa_Socket    a_hSocket,
    OpcUa_UInt32    a_uSocketEvent,
    OpcUa_Void*     a_pUserData,
    OpcUa_UInt16    a_nPortNumber,
    OpcUa_Boolean   a_bIsSSL)
{
    OpcUa_StringA                       strEvent        = OpcUa_Null;
    OpcUa_HttpsConnection_EventHandler  fEventHandler   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "SocketCallback");

    OpcUa_ReferenceParameter(a_nPortNumber);
    OpcUa_ReferenceParameter(a_bIsSSL);

    OpcUa_ReturnErrorIfArgumentNull(a_hSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_pUserData);

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

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, " * OpcUa_HttpsConnection_SocketCallback: Socket(%p), Port(%d), Data(%p), Event(%s)\n", a_hSocket, a_nPortNumber, a_pUserData, strEvent);
    /* debug code end */
#endif

    switch(a_uSocketEvent)
    {
        case OPCUA_SOCKET_READ_EVENT:
        {
            /* notifies an existing stream about new data or creates a new stream */
            fEventHandler = OpcUa_HttpsConnection_ReadEventHandler;
            break;
        }
        case OPCUA_SOCKET_WRITE_EVENT:
        {
            fEventHandler = OpcUa_HttpsConnection_WriteEventHandler;
            break;
        }
        case OPCUA_SOCKET_EXCEPT_EVENT:
        {
            fEventHandler = OpcUa_HttpsConnection_ExceptEventHandler;
            break;
        }
        case OPCUA_SOCKET_TIMEOUT_EVENT:
        {
            break;
        }
        case OPCUA_SOCKET_CLOSE_EVENT:
        {
            fEventHandler = OpcUa_HttpsConnection_ExceptEventHandler;
            break;
        }
        case OPCUA_SOCKET_CONNECT_EVENT:
        {
            fEventHandler = OpcUa_HttpsConnection_ConnectEventHandler;
            break;
        }
        case OPCUA_SOCKET_NO_EVENT:
        case OPCUA_SOCKET_SHUTDOWN_EVENT:
        case OPCUA_SOCKET_NEED_BUFFER_EVENT:
        case OPCUA_SOCKET_FREE_BUFFER_EVENT:
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
        uStatus = fEventHandler((OpcUa_HttpsConnection_Request*)a_pUserData, a_hSocket);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_ParseURL
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_ParseURL(    OpcUa_String*   a_psUrl,
                                                    OpcUa_String*   a_psHost,
                                                    OpcUa_String*   a_psResourcePath)
{
    OpcUa_StringA   sUrl        = OpcUa_Null;
    OpcUa_String    sUrlTemp    = OPCUA_STRING_STATICINITIALIZER;
    OpcUa_UInt32    uLen        = 0;
    OpcUa_StringA   sTemp1      = OpcUa_Null;
    OpcUa_StringA   sTemp2      = OpcUa_Null;
    OpcUa_Int       iLen        = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "ParseURL");

    OpcUa_ReturnErrorIfArgumentNull(a_psUrl);

    sUrl = OpcUa_String_GetRawString(a_psUrl);
    uLen = OpcUa_String_StrSize(a_psUrl);
    OpcUa_ReturnErrorIfArgumentNull(sUrl);

    /* create zero terminated version of url */
    uStatus = OpcUa_String_AttachToString(  sUrl,
                                            uLen,
                                            0,
                                            OpcUa_True,
                                            OpcUa_True,
                                            &sUrlTemp);
    OpcUa_GotoErrorIfBad(uStatus);

    sUrl = OpcUa_String_GetRawString(&sUrlTemp);

    if(sUrl == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }

    /* find start of host name */
    sTemp1 = OpcUa_StrStrA(sUrl, "//");

    if(sTemp1 == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSyntaxError);
    }

    sTemp1 = sTemp1 + 2;

    /* get end of host name */
    sTemp2 = OpcUa_StrChrA(sTemp1, '/');

    if(a_psHost != OpcUa_Null)
    {
        if(sTemp2 != OpcUa_Null)
        {
            iLen = (OpcUa_Int)(sTemp2 - sTemp1);
        }
        else
        {
            iLen = uLen - (OpcUa_Int)(sTemp1 - sUrl);
        }

        if(iLen == 0)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadSyntaxError);
        }

        /* set hostname - raw string is zero terminated */
        uStatus =   OpcUa_String_AttachToString(    sTemp1,
                                                    iLen,
                                                    0,
                                                    OpcUa_True,
                                                    OpcUa_True,
                                                    a_psHost);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* get start of resource path */
    if(sTemp2 != OpcUa_Null)
    {
        sTemp1 = sTemp2;

        if(sTemp2 != OpcUa_Null)
        {
            iLen = uLen - (OpcUa_Int)(sTemp2 - sUrl);

            /* set resource path - raw string is zero terminated */
            uStatus =   OpcUa_String_AttachToString(    sTemp1,
                                                        iLen,
                                                        0,
                                                        OpcUa_True,
                                                        OpcUa_True,
                                                        a_psResourcePath);
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }

    if(sTemp2 == OpcUa_Null)
    {
        /* no resource path - set root */
        uStatus =   OpcUa_String_AttachToString(    "/",
                                                    1,
                                                    0,
                                                    OpcUa_True,
                                                    OpcUa_True,
                                                    a_psResourcePath);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_String_Clear(&sUrlTemp);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_String_Clear(&sUrlTemp);
    OpcUa_String_Clear(a_psHost);
    OpcUa_String_Clear(a_psResourcePath);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_Connect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_Connect(
    struct _OpcUa_Connection*       a_pConnection,
    OpcUa_String*                   a_sUrl,
    OpcUa_ClientCredential*         a_pCredential,
    OpcUa_UInt32                    a_Timeout,
    OpcUa_Connection_PfnOnNotify*   a_pfnCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_HttpsConnection*          pHttpConnection = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest        = OpcUa_Null;
    OpcUa_String                    sNull           = OPCUA_STRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "Connect");

    OpcUa_ReferenceParameter(a_Timeout);

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_sUrl);
    OpcUa_ReturnErrorIfArgumentNull(a_pfnCallback);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_HttpsConnection, a_pConnection, Connect);

    pHttpConnection   = (OpcUa_HttpsConnection*)a_pConnection->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pHttpConnection);

    /* security credentials */
#if OPCUA_HTTPSCONNECTION_USE_TLS_CREDENTIALS
    pHttpConnection->PrivateKey                 = *(a_pCredential->Credential.TheActuallyUsedCredential.pClientPrivateKey);
#endif

    pHttpConnection->pCertificate               = a_pCredential->Credential.TheActuallyUsedCredential.pClientCertificate;
    pHttpConnection->pPkiConfig                 = a_pCredential->Credential.TheActuallyUsedCredential.pkiConfig;
    pHttpConnection->pServerCertificate         = a_pCredential->Credential.TheActuallyUsedCredential.pServerCertificate;
    OpcUa_ByteString_Clear(&pHttpConnection->bsUsedServerCertificate);

#if OPCUA_HTTPSCONNECTION_CONNECT_IMMEDIATELY

    /* Get free request object! */
    uStatus = OpcUa_HttpsConnection_GetRequestInState(  pHttpConnection,
                                                        OpcUa_HttpsConnectionState_Disconnected,
                                                        &pRequest);
    OpcUa_GotoErrorIfBad(uStatus);

    pRequest->ConnectionState           = OpcUa_HttpsConnectionState_Connecting;
    pRequest->bNotify                   = OpcUa_True;

    /* store security policy if provided */
    if(     a_pCredential                                                                   != OpcUa_Null
        &&  a_pCredential->Credential.TheActuallyUsedCredential.pRequestedSecurityPolicyUri != OpcUa_Null)
    {
        uStatus =OpcUa_String_StrnCpy(  &pHttpConnection->sSecurityPolicy,
                                        a_pCredential->Credential.TheActuallyUsedCredential.pRequestedSecurityPolicyUri,
                                        OPCUA_STRING_LENDONTCARE);
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        pHttpConnection->sSecurityPolicy = sNull;
    }

    /* store URL */
    uStatus = OpcUa_String_StrnCpy(&pHttpConnection->sURL, a_sUrl, OPCUA_STRING_LENDONTCARE);
    OpcUa_GotoErrorIfBad(uStatus);

    /* get URL elements for HTTP header */
    uStatus = OpcUa_HttpsConnection_ParseURL(   a_sUrl,
                                                &pHttpConnection->sHostName,
                                                &pHttpConnection->sResourcePath);
    OpcUa_GotoErrorIfBad(uStatus);

    pHttpConnection->NotifyCallback     = a_pfnCallback;
    pHttpConnection->CallbackData       = a_pCallbackData;

#if OPCUA_MULTITHREADED
    uStatus = OPCUA_P_SOCKETMANAGER_CREATESSLCLIENT(pHttpConnection->SocketManager,      /* socketmanager handle */
                                                    OpcUa_String_GetRawString(a_sUrl),   /* remote address */
                                                    0,                                   /* local port */
#if OPCUA_HTTPSCONNECTION_USE_TLS_CREDENTIALS
                                                    pHttpConnection->pCertificate,
                                                    &pHttpConnection->PrivateKey,
#else
                                                    OpcUa_Null,
                                                    OpcUa_Null,
#endif
                                                    pHttpConnection->pPkiConfig,
                                                    OpcUa_HttpsConnection_SocketCallback,  /* callback function */
                                                    OpcUa_HttpsConnection_SslEventHandler, /* certificate validation */
                                                    (OpcUa_Void*)pRequest,                 /* callback data */
                                                    &(pRequest->Socket));                  /* retreiving socket handle */
#else /* OPCUA_MULTITHREADED */
    uStatus = OPCUA_P_SOCKETMANAGER_CREATESSLCLIENT(OpcUa_Null,                          /* socketmanager handle */
                                                    OpcUa_String_GetRawString(a_sUrl),   /* remote address */
                                                    0,                                   /* local port */
#if OPCUA_HTTPSCONNECTION_USE_TLS_CREDENTIALS
                                                    pHttpConnection->pCertificate,
                                                    &pHttpConnection->PrivateKey,
#else
                                                    OpcUa_Null,
                                                    OpcUa_Null,
#endif
                                                    pHttpConnection->pPkiConfig,
                                                    OpcUa_HttpsConnection_SocketCallback,  /* callback function */
                                                    OpcUa_HttpsConnection_SslEventHandler, /* certificate validation */
                                                    (OpcUa_Void*)pRequest,                 /* callback data */
                                                    &(pRequest->Socket));                  /* retreiving socket handle */
#endif /* OPCUA_MULTITHREADED */
    OpcUa_GotoErrorIfBad(uStatus);

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);

    /* tell the caller to expect a callback (only for non-blocking sockets)*/
    uStatus = OpcUa_GoodCompletesAsynchronously;

#else

    /* store URL */
    uStatus = OpcUa_String_StrnCpy(&pHttpConnection->sURL, a_sUrl, OPCUA_STRING_LENDONTCARE);

    /* get URL elements for HTTP header */
    uStatus = OpcUa_HttpsConnection_ParseURL(   a_sUrl,
                                                &pHttpConnection->sHostName,
                                                &pHttpConnection->sResourcePath);
    OpcUa_GotoErrorIfBad(uStatus);

    pHttpConnection->NotifyCallback     = a_pfnCallback;
    pHttpConnection->CallbackData       = a_pCallbackData;

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);

    a_pfnCallback(  a_pConnection,
                    a_pCallbackData,
                    OpcUa_ConnectionEvent_Connect,
                    OpcUa_Null, /* no stream for this event */
                    OpcUa_Good);

#endif

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pRequest != OpcUa_Null)
    {
        pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

OpcUa_FinishErrorHandling;
}

/*===========================================================================
 * OpcUa_HttpsConnection_Disconnect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_Disconnect(
    OpcUa_Connection*   a_pConnection,
    OpcUa_Boolean       a_bNotifyOnComplete)
{
    OpcUa_HttpsConnection*          pHttpConnection = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest        = OpcUa_Null;
    OpcUa_UInt32                    uIndex          = 0;
    OpcUa_Timer                     hTimerToDelete  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "Disconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);

    pHttpConnection = (OpcUa_HttpsConnection*)a_pConnection->Handle;

    OPCUA_P_MUTEX_LOCK(pHttpConnection->Mutex);

    /* check all open requests and disconnect them! */
    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        pRequest = &pHttpConnection->arrHttpsRequests[uIndex];

        /* check, if the connection is in the right state for being disconnected */
        OPCUA_HTTPSCONNECTION_REQUEST_LOCK(pRequest);
        if(pRequest->ConnectionState == OpcUa_HttpsConnectionState_RequestPrepared)
        {
            pRequest->OutgoingStream->Delete((OpcUa_Stream**)&pRequest->OutgoingStream);
        }
        pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;
        pRequest->bNotify         = OpcUa_False;
        if(pRequest->Socket != OpcUa_Null)
        {
            /* blind close without error checking */
            OPCUA_P_SOCKET_CLOSE(pRequest->Socket);
            pRequest->Socket = OpcUa_Null;
        }
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

    hTimerToDelete = pHttpConnection->hWatchdogTimer;
    pHttpConnection->hWatchdogTimer = OpcUa_Null;

    OPCUA_P_MUTEX_UNLOCK(pHttpConnection->Mutex);

    /* delete the timer for the watchdog */
    if(hTimerToDelete != OpcUa_Null)
    {
        OpcUa_Timer_Delete(&hTimerToDelete);
    }

    /* close socket and update connection handle. */
    if(a_bNotifyOnComplete)
    {
        if(pHttpConnection->NotifyCallback != OpcUa_Null)
        {
            pHttpConnection->NotifyCallback(a_pConnection,                      /* source of event  */
                                            pHttpConnection->CallbackData,      /* callback data    */
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


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_BeginSendRequest
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsConnection_BeginSendRequest(
    OpcUa_Connection*    a_pConnection,
    OpcUa_OutputStream** a_ppOutputStream)
{
    OpcUa_HttpsConnection*          pHttpConnection = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest        = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "BeginSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);

    pHttpConnection = (OpcUa_HttpsConnection*)a_pConnection->Handle;

    OPCUA_P_MUTEX_LOCK(pHttpConnection->Mutex);

    if(pHttpConnection->hWatchdogTimer == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsConnection_BeginSendRequest: Connection is not connected!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /* Fetch usable request. */
    uStatus = OpcUa_HttpsConnection_GetRequestInState(  pHttpConnection,
                                                        OpcUa_HttpsConnectionState_Connected,
                                                        &pRequest);

    if(OpcUa_IsBad(uStatus))
    {
        /* if no object is available in the above state, try to connect another request object first. */
        uStatus = OpcUa_HttpsConnection_GetRequestInState(  pHttpConnection,
                                                            OpcUa_HttpsConnectionState_Disconnected,
                                                            &pRequest);
        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_BeginSendRequest: No free connection rescource available!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadResourceUnavailable);
        }
    }

    /* must close the existing outgoing stream before creating another. */
    if(pRequest->OutgoingStream != OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_BeginSendRequest: Open outstream detected!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /* create an http output stream based on the httpConnection */
    uStatus = OpcUa_HttpsStream_CreateRequest(pRequest->Socket,
                                             OPCUA_HTTP_METHOD_POST,
                                             OpcUa_String_GetRawString(&pHttpConnection->sResourcePath),
                                             "User-Agent: OPC-ANSI-C-HTTPS-API/0.1\r\n"
#if OPCUA_HTTPSCONNECTION_CONNECTION_KEEP_ALIVE
                                             "Connection: keep-alive\r\n"
#endif
                                             "Content-Type: application/octet-stream\r\n",
                                             a_ppOutputStream);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoError;
    }

    /* Set "Host" header. */
    uStatus = OpcUa_HttpsStream_SetHeader(  (OpcUa_Stream*)(*a_ppOutputStream),
                                            OpcUa_String_FromCString("Host"),
                                            &pHttpConnection->sHostName);

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoError;
    }

    /* Set SecurityPolicy header. */
    uStatus = OpcUa_HttpsStream_SetHeader(  (OpcUa_Stream*)(*a_ppOutputStream),
                                            OpcUa_String_FromCString(OPCUA_HTTPS_SECURITYPOLICYHEADER),
                                            &pHttpConnection->sSecurityPolicy);

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoError;
    }

    pRequest->OutgoingStream = *a_ppOutputStream;

    if(pRequest->ConnectionState == OpcUa_HttpsConnectionState_Disconnected)
    {
        pRequest->ConnectionState   = OpcUa_HttpsConnectionState_Connecting;

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_BeginSendRequest: New transport connection required for request %p!\n", pRequest);
    }
    else
    {
        pRequest->ConnectionState   = OpcUa_HttpsConnectionState_PreparingForRequest;

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_BeginSendRequest: Reusing existent connection %p for request %p!\n", pHttpConnection, pRequest);
    }

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    OPCUA_P_MUTEX_UNLOCK(pHttpConnection->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pRequest != OpcUa_Null)
    {
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

    OPCUA_P_MUTEX_UNLOCK(pHttpConnection->Mutex);

    OpcUa_HttpsStream_Delete((OpcUa_Stream**)a_ppOutputStream);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_EndSendRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_EndSendRequest(
    OpcUa_Connection*               a_pConnection,
    OpcUa_OutputStream**            a_ppOutputStream,
    OpcUa_UInt32                    a_uTimeout,
    OpcUa_Connection_PfnOnResponse* a_pfnCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_HttpsConnection*          pHttpConnection     = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest            = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "EndSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOutputStream);
    OpcUa_ReturnErrorIfArgumentNull((*a_ppOutputStream)->Handle);

    OpcUa_ReturnErrorIfInvalidConnection(a_pConnection);

    /* cast onto the backend types */
    pHttpConnection = (OpcUa_HttpsConnection*)a_pConnection->Handle;

    /* fetch matching request object from output stream */
    uStatus = OpcUa_HttpsConnection_GetRequestByStream( pHttpConnection,
                                                        *a_ppOutputStream,
                                                        &pRequest);

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoError;
    }

    /* check for consistency */
    if(pRequest->RequestCallback != OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_HttpsConnection_EndSendRequest: Request in wrong state\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /* set request data */
    pRequest->RequestTimeout        = a_uTimeout;
    pRequest->RequestStartTime      = OpcUa_GetTickCount();
    pRequest->RequestCallback       = a_pfnCallback;
    pRequest->RequestCallbackData   = a_pCallbackData;

    /* check for valid connection state */
    switch(pRequest->ConnectionState)
    {
    case OpcUa_HttpsConnectionState_Connecting:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_EndSendRequest: create new connection for request %p.\n", pRequest);

#if OPCUA_MULTITHREADED
            uStatus = OPCUA_P_SOCKETMANAGER_CREATESSLCLIENT(pHttpConnection->SocketManager,      /* socketmanager handle */
                                                            OpcUa_String_GetRawString(&pHttpConnection->sURL),   /* remote address */
                                                            0,                                   /* local port */
#if OPCUA_HTTPSCONNECTION_USE_TLS_CREDENTIALS
                                                            pHttpConnection->pCertificate,
                                                            &pHttpConnection->PrivateKey,
#else
                                                            OpcUa_Null,
                                                            OpcUa_Null,
#endif
                                                            pHttpConnection->pPkiConfig,
                                                            OpcUa_HttpsConnection_SocketCallback, /* callback function */
                                                            OpcUa_HttpsConnection_SslEventHandler,/* certificate validation */
                                                            (OpcUa_Void*)pRequest,                /* callback data */
                                                            &(pRequest->Socket));                 /* retreiving socket handle */
#else /* OPCUA_MULTITHREADED */
            uStatus = OPCUA_P_SOCKETMANAGER_CREATESSLCLIENT(OpcUa_Null,                          /* socketmanager handle */
                                                            OpcUa_String_GetRawString(&pHttpConnection->sURL),   /* remote address */
                                                            0,                                   /* local port */
#if OPCUA_HTTPSCONNECTION_USE_TLS_CREDENTIALS
                                                            pHttpConnection->pCertificate,
                                                            &pHttpConnection->PrivateKey,
#else
                                                            OpcUa_Null,
                                                            OpcUa_Null,
#endif
                                                            pHttpConnection->pPkiConfig,
                                                            OpcUa_HttpsConnection_SocketCallback, /* callback function */
                                                            OpcUa_HttpsConnection_SslEventHandler,/* certificate validation */
                                                            (OpcUa_Void*)pRequest,                /* callback data */
                                                            &(pRequest->Socket));                 /* retreiving socket handle */
#endif /* OPCUA_MULTITHREADED */

            if(OpcUa_IsBad(uStatus))
            {
                pRequest->OutgoingStream        = OpcUa_Null;
                pRequest->RequestCallback       = OpcUa_Null;
                pRequest->RequestCallbackData   = OpcUa_Null;
                pRequest->ConnectionState  = OpcUa_HttpsConnectionState_Disconnected;
                OpcUa_GotoError;
            }

            pRequest->ConnectionState = OpcUa_HttpsConnectionState_RequestPrepared;

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_EndSendRequest: waiting for connection %p.\n", pHttpConnection);

            (*a_ppOutputStream) = OpcUa_Null;

            break;
        }
    case OpcUa_HttpsConnectionState_PreparingForRequest:    /* connection reuse */
        {
            /* progress normally */
            pRequest->OutgoingStream = OpcUa_Null;
            pRequest->ConnectionState = OpcUa_HttpsConnectionState_WaitingForResponse;

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsConnection_EndSendRequest: sending request %p on connection %p.\n", pRequest, pHttpConnection);

            /* close and flush stream */
            uStatus = (*a_ppOutputStream)->Close((OpcUa_Stream*)(*a_ppOutputStream));
            if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
            {
                /* try to put stream content into buffer queue for delayed sending */
                uStatus = OpcUa_HttpsConnection_AddStreamToSendQueue(pRequest, *a_ppOutputStream);
            }

            /* clean up stream resources */
            (*a_ppOutputStream)->Delete((OpcUa_Stream**)a_ppOutputStream);

            if(OpcUa_IsBad(uStatus))
            {
                pRequest->RequestCallback       = OpcUa_Null;
                pRequest->RequestCallbackData   = OpcUa_Null;
                pRequest->ConnectionState       = OpcUa_HttpsConnectionState_Connected;
                OpcUa_GotoError;
            }

            break;
        }
    default:
        {
            pRequest->OutgoingStream        = OpcUa_Null;
            pRequest->RequestCallback       = OpcUa_Null;
            pRequest->RequestCallbackData   = OpcUa_Null;

            OpcUa_GotoErrorWithStatus(OpcUa_BadConnectionClosed);
        }
    }

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pRequest != OpcUa_Null)
    {
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_HttpsConnection_EndSendRequest: could not send request! 0x%08X (cookie %p)\n", uStatus, a_pCallbackData);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_AbortSendRequest
 *===========================================================================*/
/* INFO: null streams are allowed and say that the owner of the connection
         takes care about the stream itself. Only if non null the http transport
         generates an abort message. this is not handled by the ua stack because
         abort messages are always secured. */
OpcUa_StatusCode OpcUa_HttpsConnection_AbortSendRequest(
    OpcUa_Connection*       a_pConnection,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_String*           a_psReason,
    OpcUa_OutputStream**    a_ppOutputStream)
{
    OpcUa_HttpsConnection*          pHttpConnection = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest        = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "AbortSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOutputStream);
    OpcUa_ReturnErrorIfArgumentNull((*a_ppOutputStream)->Handle);

    OpcUa_ReturnErrorIfInvalidConnection(a_pConnection);

    /* no insecure abort messages implemented and allowed! */
    OpcUa_ReferenceParameter(a_uStatus);
    OpcUa_ReferenceParameter(a_psReason);

    /* cast onto the backend type */
    pHttpConnection = (OpcUa_HttpsConnection*)a_pConnection->Handle;

    /* fetch matching request object from output stream */
    uStatus = OpcUa_HttpsConnection_GetRequestByStream( pHttpConnection,
                                                        *a_ppOutputStream,
                                                        &pRequest);
    OpcUa_GotoErrorIfBad(uStatus);

    /* clean outgoing stream */
    pRequest->OutgoingStream = OpcUa_Null;

    if(pRequest->ConnectionState == OpcUa_HttpsConnectionState_Connecting)
    {
        pRequest->ConnectionState = OpcUa_HttpsConnectionState_Disconnected;
    }
    else if(pRequest->ConnectionState == OpcUa_HttpsConnectionState_PreparingForRequest)
    {
        pRequest->ConnectionState = OpcUa_HttpsConnectionState_Connected;
    }

    OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);

    /* clean up */
    OpcUa_HttpsStream_Delete((OpcUa_Stream**)a_ppOutputStream);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pRequest != OpcUa_Null)
    {
        OPCUA_HTTPSCONNECTION_REQUEST_UNLOCK(pRequest);
    }

    OpcUa_HttpsStream_Delete((OpcUa_Stream**)a_ppOutputStream);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsConnection_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_HttpsConnection_Delete(OpcUa_Connection** a_ppConnection)
{
    OpcUa_HttpsConnection*          pHttpConnection = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest        = OpcUa_Null;
    OpcUa_UInt32                    uIndex          = 0;

    if(a_ppConnection == OpcUa_Null)
    {
        return;
    }

    if(*a_ppConnection == OpcUa_Null)
    {
        return;
    }

    pHttpConnection = (OpcUa_HttpsConnection*)(*a_ppConnection)->Handle;
    if(pHttpConnection == OpcUa_Null)
    {
        return;
    }

    /* delete the timer for the watchdog */
    if(pHttpConnection->hWatchdogTimer != OpcUa_Null)
    {
        OpcUa_Timer_Delete(&(pHttpConnection->hWatchdogTimer));
    }

#if OPCUA_MULTITHREADED
    /* HINT: waits internally for receive thread to shutdown, so this call may block. */
    if(pHttpConnection->SocketManager != OpcUa_Null)
    {
        OPCUA_P_SOCKETMANAGER_DELETE(&(pHttpConnection->SocketManager));
    }
#endif /* OPCUA_MULTITHREADED */

    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        pRequest = &pHttpConnection->arrHttpsRequests[uIndex];

        pRequest->RequestCallback        = OpcUa_Null;
        pRequest->RequestCallbackData    = OpcUa_Null;

        if(pRequest->ConnectionState == OpcUa_HttpsConnectionState_RequestPrepared)
        {
            pRequest->OutgoingStream->Delete((OpcUa_Stream**)&pRequest->OutgoingStream);
        }

        /* the architecture should prevent from getting here with active streams */
        if(pRequest->IncomingStream != OpcUa_Null)
        {
            pRequest->IncomingStream->Close((OpcUa_Stream*)pRequest->IncomingStream);
            pRequest->IncomingStream->Delete((OpcUa_Stream**)&pRequest->IncomingStream);
        }

        while(pRequest->pSendQueue != OpcUa_Null)
        {
            OpcUa_BufferList* pCurrentBuffer = pRequest->pSendQueue;
            pRequest->pSendQueue = pCurrentBuffer->pNext;
            OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
            OpcUa_Free(pCurrentBuffer);
        }

#if OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS
        if(pRequest->Mutex != OpcUa_Null)
        {
            OPCUA_P_MUTEX_DELETE(&pRequest->Mutex);
            pRequest->Mutex = OpcUa_Null;
        }
#endif /* OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS */
    }

    OpcUa_String_Clear(&pHttpConnection->sURL);
    OpcUa_String_Clear(&pHttpConnection->sHostName);
    OpcUa_String_Clear(&pHttpConnection->sResourcePath);
    OpcUa_String_Clear(&pHttpConnection->sSecurityPolicy);
    OpcUa_ByteString_Clear(&pHttpConnection->bsUsedServerCertificate);

    /*** Free ***/
    /* clean internal resources */
    OPCUA_P_MUTEX_DELETE(&(pHttpConnection->Mutex));

    /* the connection implementation */
    OpcUa_Free(pHttpConnection);

    /* the wrapper element */
    OpcUa_Free(*a_ppConnection);
    *a_ppConnection = OpcUa_Null;
}

/*============================================================================
 * OpcUa_HttpsConnection_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsConnection_Create(OpcUa_Connection** a_ppConnection)
{
    OpcUa_HttpsConnection*          pHttpConnection = OpcUa_Null;
    OpcUa_HttpsConnection_Request*  pRequest        = OpcUa_Null;
    OpcUa_UInt32                    uIndex          = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpConnection, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_ppConnection);

    /* allocate external connection object */
    *a_ppConnection = (OpcUa_Connection*)OpcUa_Alloc(sizeof(OpcUa_Connection));
    OpcUa_ReturnErrorIfAllocFailed(*a_ppConnection);
    OpcUa_MemSet(*a_ppConnection, 0, sizeof(OpcUa_Connection));

    /* allocate handle that stores internal state information */
    pHttpConnection = (OpcUa_HttpsConnection*)OpcUa_Alloc(sizeof(OpcUa_HttpsConnection));
    OpcUa_GotoErrorIfAllocFailed(pHttpConnection);
    OpcUa_MemSet(pHttpConnection, 0, sizeof(OpcUa_HttpsConnection));

    (*a_ppConnection)->Handle = pHttpConnection;

#if OPCUA_MULTITHREADED
    pHttpConnection->SocketManager   = OpcUa_Null;
#endif /* OPCUA_MULTITHREADED */

    pHttpConnection->SanityCheck     = OpcUa_HttpsConnection_SanityCheck;

    uStatus = OPCUA_P_MUTEX_CREATE(&(pHttpConnection->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_String_Initialize(&pHttpConnection->sURL);
    OpcUa_String_Initialize(&pHttpConnection->sHostName);
    OpcUa_String_Initialize(&pHttpConnection->sResourcePath);
    OpcUa_ByteString_Initialize(&pHttpConnection->bsUsedServerCertificate);

    for(uIndex = 0; uIndex < OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS; uIndex++)
    {
        pRequest = &pHttpConnection->arrHttpsRequests[uIndex];

        OpcUa_MemSet(pRequest, 0, sizeof(OpcUa_HttpsConnection_Request));

        pRequest->ConnectionState       = OpcUa_HttpsConnectionState_Disconnected;
        pRequest->pConnection           = *a_ppConnection;
        pRequest->bNotify               = OpcUa_False;
#if OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS
        uStatus = OPCUA_P_MUTEX_CREATE(&pRequest->Mutex);
#endif /* OPCUA_HTTPSCONNECTION_SYNCHRONIZE_REQUESTS */
        OpcUa_GotoErrorIfBad(uStatus);
    }

#if OPCUA_MULTITHREADED
    /* create the socket manager */
    uStatus = OPCUA_P_SOCKETMANAGER_CREATE(&(pHttpConnection->SocketManager),
                                           OPCUA_HTTPS_CONNECTION_MAXPENDINGREQUESTS,
                                           OPCUA_SOCKET_NO_FLAG);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_MULTITHREADED */

    /* create watchdog timer for requests. */
    uStatus = OpcUa_Timer_Create(   &(pHttpConnection->hWatchdogTimer),
                                    OPCUA_HTTPSCONNECTION_TIMEOUTINTERVAL,
                                    OpcUa_HttpsConnection_WatchdogTimerCallback,
                                    OpcUa_HttpsConnection_WatchdogTimerKillCallback,
                                    (OpcUa_Void*)(*a_ppConnection));
    OpcUa_ReturnErrorIfBad(uStatus);

    (*a_ppConnection)->Connect              = OpcUa_HttpsConnection_Connect;
    (*a_ppConnection)->Disconnect           = OpcUa_HttpsConnection_Disconnect;
    (*a_ppConnection)->BeginSendRequest     = OpcUa_HttpsConnection_BeginSendRequest;
    (*a_ppConnection)->EndSendRequest       = OpcUa_HttpsConnection_EndSendRequest;
    (*a_ppConnection)->AbortSendRequest     = OpcUa_HttpsConnection_AbortSendRequest;
    (*a_ppConnection)->GetReceiveBufferSize = OpcUa_Null;
    (*a_ppConnection)->Delete               = OpcUa_HttpsConnection_Delete;
    (*a_ppConnection)->AddToSendQueue       = OpcUa_Null;
    (*a_ppConnection)->CheckProtocolVersion = OpcUa_Null;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_HttpsConnection_Delete(a_ppConnection);

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_HTTPSAPI */
#endif /* OPCUA_HAVE_CLIENTAPI */
