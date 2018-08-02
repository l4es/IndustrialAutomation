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
#include <opcua_pkifactory.h>
#include <opcua_cryptofactory.h>
#include <opcua_guid.h>
#include <opcua_string.h>
#include <opcua_datetime.h>
#include <opcua_utilities.h>
#include <opcua_timer.h>
#include <opcua_list.h>

/* stackcore */
#include <opcua_types.h>
#include <opcua_buffer.h>
#include <opcua_binaryencoder.h>
#include <opcua_crypto.h>
#include <opcua_securechannel.h>
#include <opcua_connection.h>

/* security */
#include <opcua_securechannel_types.h>
#include <opcua_tcpsecurechannel.h>
#include <opcua_securestream.h>

/* transport */
#include <opcua_tcpconnection.h>

/* header */
#include <opcua_secureconnection.h>

/*============================================================================
 * GLOBALS
 *===========================================================================*/
#define OPCUA_SECURECONNECTION_THREADSAFE      OPCUA_CONFIG_YES
#define OPCUA_SECURECONNECTION_DEBUG_MUTEX     OPCUA_CONFIG_NO

#if OPCUA_SECURECONNECTION_THREADSAFE
# if OPCUA_SECURECONNECTION_DEBUG_MUTEX
/** @brief Complete lock of the secure connection object in both directions. */
#  define OPCUA_SECURECONNECTION_LOCK(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_LOCK(xSecureConnection->RequestMutex); \
    xSecureConnection->iSyncAccessLevel++; \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OPCUA_SECURECONNECTION_LOCK:   --> %i! (Rq:%i|Rs:%i)\n", xSecureConnection->iSyncAccessLevel, xSecureConnection->iRequestAccessLevel, xSecureConnection->iResponseAccessLevel); \
}

/** @brief Complete unlock of the secure connection object in both directions. */
#  define OPCUA_SECURECONNECTION_UNLOCK(xSecureConnection) \
{ \
    if(xSecureConnection->iSyncAccessLevel <= 0) \
    { \
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OPCUA_SECURECONNECTION_UNLOCK: INVALID LOCK STATE!\n"); \
        __asm int 3 \
    } \
    xSecureConnection->iSyncAccessLevel--; \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OPCUA_SECURECONNECTION_UNLOCK: <-- %i! (Rq:%i|Rs:%i)\n", xSecureConnection->iSyncAccessLevel, xSecureConnection->iRequestAccessLevel, xSecureConnection->iResponseAccessLevel); \
    OPCUA_P_MUTEX_UNLOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete lock of the secure connection object in send direction. */
#  define OPCUA_SECURECONNECTION_LOCK_REQUEST(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_LOCK(xSecureConnection->RequestMutex); \
    xSecureConnection->iRequestAccessLevel++; \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OPCUA_SECURECONNECTION_LOCK_REQUEST:   --> %i! (To:%i|Rs:%i)\n", xSecureConnection->iRequestAccessLevel, xSecureConnection->iSyncAccessLevel, xSecureConnection->iResponseAccessLevel); \
}

/** @brief Complete unlock of the secure connection object in send direction. */
#  define OPCUA_SECURECONNECTION_UNLOCK_REQUEST(xSecureConnection) \
{ \
    if(xSecureConnection->iRequestAccessLevel <= 0) \
    { \
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OPCUA_SECURECONNECTION_UNLOCK_REQUEST: INVALID LOCK STATE!\n"); \
        __asm int 3 \
    } \
    xSecureConnection->iRequestAccessLevel--; \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OPCUA_SECURECONNECTION_UNLOCK_REQUEST: <-- %i! (To:%i|Rs:%i)\n", xSecureConnection->iRequestAccessLevel, xSecureConnection->iSyncAccessLevel, xSecureConnection->iResponseAccessLevel); \
    OPCUA_P_MUTEX_UNLOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete lock of the secure connection object in receive direction. */
#  define OPCUA_SECURECONNECTION_LOCK_RESPONSE(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_LOCK(xSecureConnection->RequestMutex); \
    xSecureConnection->iResponseAccessLevel++; \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OPCUA_SECURECONNECTION_LOCK_RESPONSE:   --> %i! (To:%i|Rq:%i)\n", xSecureConnection->iResponseAccessLevel, xSecureConnection->iSyncAccessLevel, xSecureConnection->iRequestAccessLevel); \
}

/** @brief Complete unlock of the secure connection object in receive direction. */
#  define OPCUA_SECURECONNECTION_UNLOCK_RESPONSE(xSecureConnection) \
{ \
    if(xSecureConnection->iResponseAccessLevel <= 0) \
    { \
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OPCUA_SECURECONNECTION_LOCK_RESPONSE: INVALID LOCK STATE!\n"); \
        __asm int 3 \
    } \
    xSecureConnection->iResponseAccessLevel--; \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OPCUA_SECURECONNECTION_UNLOCK_RESPONSE: <-- %i! (To:%i|Rq:%i)\n", xSecureConnection->iResponseAccessLevel, xSecureConnection->iSyncAccessLevel, xSecureConnection->iRequestAccessLevel); \
    OPCUA_P_MUTEX_UNLOCK(xSecureConnection->RequestMutex); \
}
# else /* OPCUA_SECURECONNECTION_DEBUG_MUTEX */
/** @brief Complete lock of the secure connection object in both directions. */
#  define OPCUA_SECURECONNECTION_LOCK(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_LOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete unlock of the secure connection object in both directions. */
#  define OPCUA_SECURECONNECTION_UNLOCK(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_UNLOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete lock of the secure connection object in send direction. */
#  define OPCUA_SECURECONNECTION_LOCK_REQUEST(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_LOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete unlock of the secure connection object in send direction. */
#  define OPCUA_SECURECONNECTION_UNLOCK_REQUEST(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_UNLOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete lock of the secure connection object in receive direction. */
#  define OPCUA_SECURECONNECTION_LOCK_RESPONSE(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_LOCK(xSecureConnection->RequestMutex); \
}
/** @brief Complete unlock of the secure connection object in receive direction. */
#  define OPCUA_SECURECONNECTION_UNLOCK_RESPONSE(xSecureConnection) \
{ \
    OPCUA_P_MUTEX_UNLOCK(xSecureConnection->RequestMutex); \
}
# endif /* OPCUA_SECURECONNECTION_DEBUG_MUTEX */
#else /* OPCUA_SECURECONNECTION_THREADSAFE */
# define OPCUA_SECURECONNECTION_LOCK(xSecureConnection)
# define OPCUA_SECURECONNECTION_UNLOCK(xSecureConnection)
# define OPCUA_SECURECONNECTION_LOCK_REQUEST(xSecureConnection)
# define OPCUA_SECURECONNECTION_UNLOCK_REQUEST(xSecureConnection)
# define OPCUA_SECURECONNECTION_LOCK_RESPONSE(xSecureConnection)
# define OPCUA_SECURECONNECTION_UNLOCK_RESPONSE(xSecureConnection)
#endif /* OPCUA_SECURECONNECTION_THREADSAFE */

/*============================================================================
 * OpcUa_SecureConnection_SanityCheck
 *===========================================================================*/
#define OpcUa_SecureConnection_SanityCheck 0x02348936

/*============================================================================
 * TYPES
 *===========================================================================*/

/*============================================================================
 * OpcUa_SecureConnectionState
 *===========================================================================*/
typedef enum _OpcUa_SecureConnectionState
{
    /** @brief Invalid state (never been set). */
    OpcUa_SecureConnectionState_Invalid,
    /** @brief The secureconnection is currently for the transport connection to be established. */
    OpcUa_SecureConnectionState_ConnectingTransport,
    /** @brief The secureconnection is currently waiting for the secure conversation handshake to complete. */
    OpcUa_SecureConnectionState_ConnectingSecure,
    /** @brief The secureconnection is connected to a listener. */
    OpcUa_SecureConnectionState_Connected,
    /** @brief The secureconnection is currently waiting for the secure channel to be closed. */
    OpcUa_SecureConnectionState_DisconnectingSecure,
    /** @brief The secureconnection is currently not connected to a listener. */
    OpcUa_SecureConnectionState_Disconnected,
    /** @brief The secureconnection has been newly created or an error occurred and the current state is uncertain. */
    OpcUa_SecureConnectionState_Unknown
}
OpcUa_SecureConnectionState;

/*============================================================================
 * OpcUa_SecureRequestState
 *===========================================================================*/
/** @brief Tells about the current processing state of the request. */
typedef enum _OpcUa_SecureRequestState
{
    /** @brief Invalid request state. */
    OpcUa_SecureRequestState_Invalid,
    /** @brief The server is waiting for the response. */
    OpcUa_SecureRequestState_Open,
    /** @brief The response arrived partially. */
    OpcUa_SecureRequestState_Started,
    /** @brief The response arrived completely, the request is finished. */
    OpcUa_SecureRequestState_Finished,
    /** @brief Either server or client cancelled the request. */
    OpcUa_SecureRequestState_Cancelled
} OpcUa_SecureRequestState;

/*============================================================================
 * OpcUa_SecureRequestType
 *===========================================================================*/
/** @brief Tells about the type of the request. */
typedef enum _OpcUa_SecureRequestType
{
    /** @brief Invalid request type. */
    OpcUa_SecureRequestType_Invalid,
    /** @brief The request contains a secure channel open request. */
    OpcUa_SecureRequestType_SecureChannelOpen,
    /** @brief The request contains a secure channel close request. */
    OpcUa_SecureRequestType_SecureChannelClose,
    /** @brief The request belongs to the session layer. */
    OpcUa_SecureRequestType_SessionCall
} OpcUa_SecureRequestType;

/*============================================================================
 * OpcUa_SecureRequest
 *===========================================================================*/
typedef struct _OpcUa_SecureRequest
{
    /*! The secure connection that owns the request */
    OpcUa_Connection*               Connection;
    /*! CryptoProvider */
    OpcUa_CryptoProvider*           CryptoProvider;
    /*! The callback to use when the request completes. */
    OpcUa_Connection_PfnOnResponse* Callback;
    /*! The data to pass with the callback. */
    OpcUa_Void*                     CallbackData;
    /** @brief Tells about the current processing state of the request. */
    OpcUa_SecureRequestState        State;
    /** @brief Tells about the type of the request. */
    OpcUa_SecureRequestType         Type;
    /** @brief Tells about the current security mode of the request. */
    OpcUa_MessageSecurityMode       MessageSecurityMode;
    /** @brief The client assigned id of the request. */
    OpcUa_UInt32                    RequestId;
    /** @brief The time when the request was sent to the server. */
    OpcUa_UInt32                    StartTime;
    /** @brief The time when the request is no longer valid. */
    OpcUa_UInt32                    OperationTimeout;
}
OpcUa_SecureRequest;

/*============================================================================
 * OpcUa_SecureConnection
 *===========================================================================*/
typedef struct _OpcUa_SecureConnection
{
    /*! @brief Magic number to identify the type of the connection. */
    OpcUa_UInt32                    SanityCheck;

#if OPCUA_SECURECONNECTION_THREADSAFE
    /*! @brief Critical section for exclusive access to this object. */
    OpcUa_Mutex                     RequestMutex;
#if OPCUA_SECURECONNECTION_DEBUG_MUTEX
    OpcUa_Int32                     iSyncAccessLevel;
    OpcUa_Int32                     iRequestAccessLevel;
    OpcUa_Int32                     iResponseAccessLevel;
#endif /* OPCUA_SECURECONNECTION_DEBUG_MUTEX */
#endif /* OPCUA_SECURECONNECTION_THREADSAFE */

    /*! @brief Set to OpcUa_True while the Connection is waiting for a renew response. */
    OpcUa_Boolean                   bRenewActive;

    /*! @brief The interface to the underlying transport connection. */
    OpcUa_Connection*               TransportConnection;
    /*! @brief Function getting called on events. */
    OpcUa_Connection_PfnOnNotify*   Callback;
    /*! @brief Handle to data passed back to the owner of this instance with the previous function. */
    OpcUa_Void*                     CallbackData;
    /*! @brief The current state of this connection. */
    OpcUa_SecureConnectionState     State;
    /*! @brief Handle to the encoder used by this connection. */
    OpcUa_Encoder*                  Encoder;
    /*! @brief Handle to the decoder used by this connection. */
    OpcUa_Decoder*                  Decoder;
    /*! @brief Handle to the namespace uri table used for encoding/decoding. */
    OpcUa_StringTable*              NamespaceUris;
    /*! @brief Handle to the table containing the type descriptions for the encoder/decoder. */
    OpcUa_EncodeableTypeTable*      KnownTypes;
    /*! @brief The securechannel information. */
    OpcUa_SecureChannel*            pSecureChannel;
    /*! @brief The certificate of this client. */
    OpcUa_ByteString*               ClientCertificate;
    /*! @brief The certificate of the server to which this connection will be connected. */
    OpcUa_ByteString*               ServerCertificate;
    /*! @brief The private key of this client. */
    OpcUa_Key*                      ClientPrivateKey;
    /*! @brief The PKI provider interface used for security functionality based on the used profile. */
    OpcUa_PKIProvider*              ClientPKIProvider;

    /*! @brief IntegerId for Requests. */
    OpcUa_UInt32                    uRequestId;
    /*! @brief A list with requests waiting for their request. */
    OpcUa_List*                     PendingRequests;
    /*! @brief Watchdog for the requests. */
    OpcUa_Timer                     hWatchdogTimer;

    /*! @brief The message security mode used for this connection. */
    OpcUa_MessageSecurityMode       MessageSecurityMode;
    /*! @brief URI of the used security policy. */
    OpcUa_String*                   pRequestedSecurityPolicyUri;
    /*! @brief The lifetime of the securechannel. */
    OpcUa_Int32                     nLifetime;
    /*! @brief Handle to the timer that triggers the next reconnect try. */
    OpcUa_Timer                     hRenewTimer;

    /*! @brief URL used for connecting to the server. */
    OpcUa_String                    sUrl;
    /*! @brief Maximum number of Chunks/Buffers per message. */
    OpcUa_UInt32                    nMaxBuffersPerMessage;
}
OpcUa_SecureConnection;


/*============================================================================
 * PROTOTYPES
 *===========================================================================*/
/*============================================================================
 * OpcUa_SecureConnection_BeginSendOpenSecureChannelRequest
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_BeginSendOpenSecureChannelRequest(
    OpcUa_Connection*                   pConnection,
    OpcUa_String*                       pSecurityPolicyName,
    OpcUa_MessageSecurityMode           MessageSecurityMode,
    OpcUa_CryptoProvider*               pCryptoProvider,
    OpcUa_ByteString*                   pClientCertificate,
    OpcUa_ByteString*                   pServerCertificateThumbprint,
    OpcUa_OutputStream**                ppOstrm);

/*============================================================================
 * OpcUa_SecureConnection_EndSendRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_EndSendRequest(
    OpcUa_Connection*                   pConnection,
    OpcUa_OutputStream**                ppOstrm,
    OpcUa_UInt32                        uTimeout,
    OpcUa_Connection_PfnOnResponse*     pCallback,
    OpcUa_Void*                         pCallbackData);

/*============================================================================
 * OpcUa_SecureConnection_ProcessOpenSecureChannelResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_ProcessOpenSecureChannelResponse(
    OpcUa_Connection*                   a_pConnection,
    OpcUa_InputStream*                  a_pIstrm,
    OpcUa_StatusCode                    a_responseStatus,
    OpcUa_Boolean                       a_bResponseComplete);

/*============================================================================
 * OpcUa_SecureConnection_ProcessSessionCallResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_ProcessSessionCallResponse(
    OpcUa_Connection*                   pConnection,
    OpcUa_InputStream*                  pIstrm,
    OpcUa_StatusCode                    responseStatus,
    OpcUa_Boolean                       bResponseComplete);

/*============================================================================
 * OpcUa_SecureConnection_OnNotify
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_OnNotify(
    OpcUa_Connection*                   pConnection,
    OpcUa_Void*                         pCallbackData,
    OpcUa_ConnectionEvent               eEvent,
    OpcUa_InputStream**                 ppInputStream,
    OpcUa_StatusCode                    uConnectionStatus);

/*============================================================================
 * OpcUa_SecureConnection_OnResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_InternalOnResponse(
    OpcUa_Connection*                   pConnection,
    OpcUa_Void*                         pCallbackData,
    OpcUa_StatusCode                    uResponseStatus,
    OpcUa_InputStream**                 ppIstrm,
    OpcUa_Boolean                       bRequestComplete);

/*============================================================================
 * OpcUa_SecureRequest_Delete
 *===========================================================================*/
static OpcUa_Void OpcUa_SecureRequest_Delete(
    OpcUa_SecureRequest**               ppRequest);

/*============================================================================
 * OpcUa_SecureConnection_BeginOpenSecureChannel
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_BeginOpenSecureChannel(
    OpcUa_Connection*                 a_pConnection,
    OpcUa_SecurityTokenRequestType    a_eTokenRequestType);

/*============================================================================
 * IMPLEMENTATIONS
 *===========================================================================*/

/*============================================================================
 * OpcUa_SecureConnection_GetSecurityToken
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_GetSecurityToken(  OpcUa_Connection*               a_pConnection,
                                                                        OpcUa_ChannelSecurityToken**    a_pChannelSecurityToken)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "GetSecurityToken");

    /* consider replacement with OpcUa_SecureChannel::GetCurrentSecurityToken */
    *a_pChannelSecurityToken = &((OpcUa_SecureConnection*)a_pConnection->Handle)->pSecureChannel->CurrentChannelSecurityToken;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* nothing to do */

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_WatchdogTimerCallback
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SecureConnection_WatchdogTimerCallback(    OpcUa_Void*     a_pvCallbackData,
                                                                                OpcUa_Timer     a_hTimer,
                                                                                OpcUa_UInt32    a_msecElapsed)
{
    OpcUa_Connection*       pConnection         = OpcUa_Null;
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_UInt32            CurrentTime         = 0;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;

    OpcUa_ReferenceParameter(a_msecElapsed);
    OpcUa_ReferenceParameter(a_hTimer);

    pConnection         = (OpcUa_Connection*)a_pvCallbackData;
    pSecureConnection   = (OpcUa_SecureConnection*)pConnection->Handle;

    OpcUa_List_Enter(pSecureConnection->PendingRequests);

    /* reset list */
    OpcUa_List_ResetCurrent(pSecureConnection->PendingRequests);

    /* get first element */
    pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);

    while(pSecureRequest != OpcUa_Null)
    {
        CurrentTime = OpcUa_GetTickCount();

        /* pSecureRequest->OperationTimeout: absolute count of milliseconds    */
        /* pSecureRequest->StartTime:        start time in millisecond ticks   */
        /* uTime                             current time in millisecond ticks */
        if(     pSecureRequest->OperationTimeout != OPCUA_INFINITE /* timeout has to be non infinite */
            &&  pSecureRequest->OperationTimeout != 0 /* zero means infinite as in previous versions */
            &&  (CurrentTime - pSecureRequest->StartTime) >= pSecureRequest->OperationTimeout
          )
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_WatchdogTimerCallback: Request %u timed out after %u msecs!\n", pSecureRequest->RequestId, (CurrentTime > pSecureRequest->StartTime)?(CurrentTime - pSecureRequest->StartTime):(CurrentTime + (OpcUa_UInt32_Max - pSecureRequest->StartTime)));

            /* callback finished, delete the internal resource */
            OpcUa_List_DeleteCurrentElement(pSecureConnection->PendingRequests);

            /* tell all waiting callbacks of the cancellation */
            if(pSecureRequest->Callback != OpcUa_Null)
            {
                pSecureRequest->Callback(   pConnection,                    /* source of the event      */
                                            pSecureRequest->CallbackData,   /* the callback data        */
                                            OpcUa_BadTimeout,               /* status of the request    */
                                            OpcUa_Null);                    /* the stream to read from  */
            }

            OpcUa_Free(pSecureRequest);
            pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);
        }
        else
        {
            /* get next request */
            pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetNextElement(pSecureConnection->PendingRequests);
        }
    }

    OpcUa_List_Leave(pSecureConnection->PendingRequests);

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_SecureConnection_WatchdogTimerKillCallback
 *===========================================================================*/
/* informational and debug only */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SecureConnection_WatchdogTimerKillCallback(    OpcUa_Void*     a_pvCallbackData,
                                                                                    OpcUa_Timer     a_hTimer,
                                                                                    OpcUa_UInt32    a_msecElapsed)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;

    OpcUa_ReferenceParameter(a_msecElapsed);
    OpcUa_ReferenceParameter(a_hTimer);

    pSecureConnection   = (OpcUa_SecureConnection*)(((OpcUa_Connection*)a_pvCallbackData)->Handle);

    OpcUa_List_Enter(pSecureConnection->PendingRequests);

    /* reset list */
    OpcUa_List_ResetCurrent(pSecureConnection->PendingRequests);

    /* get first element */
    pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);

    while(pSecureRequest != OpcUa_Null)
    {
        /* callback finished, delete the internal resource */
        OpcUa_List_DeleteCurrentElement(pSecureConnection->PendingRequests);

        /* tell all waiting callbacks of the cancellation */
        if(pSecureRequest->Callback != OpcUa_Null)
        {
            pSecureRequest->Callback(   (OpcUa_Connection*)a_pvCallbackData,    /* source of the event      */
                                        pSecureRequest->CallbackData,           /* the callback data        */
                                        OpcUa_BadDisconnect,                    /* status of the request    */
                                        OpcUa_Null);                            /* the stream to read from  */
        }

        OpcUa_SecureRequest_Delete(&pSecureRequest);
        pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);
    }

    OpcUa_List_Leave(pSecureConnection->PendingRequests);

    return OpcUa_Good;
}


/*============================================================================
 * OpcUa_SecureConnection_RenewTimerKillCallback
 *===========================================================================*/
/* informational and debug only */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SecureConnection_RenewTimerKillCallback(   OpcUa_Void*     a_pvCallbackData,
                                                                                OpcUa_Timer     a_hTimer,
                                                                                OpcUa_UInt32    a_msecElapsed)
{
    OpcUa_ReferenceParameter(a_msecElapsed);
    OpcUa_ReferenceParameter(a_hTimer);
    OpcUa_ReferenceParameter(a_pvCallbackData);

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_SecureConnection_RenewTimerCallback
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SecureConnection_RenewTimerCallback(   OpcUa_Void*     a_pvCallbackData,
                                                                            OpcUa_Timer     a_hTimer,
                                                                            OpcUa_UInt32    a_msecElapsed)
{
    OpcUa_Connection*       pConnection         = OpcUa_Null;
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "RenewTimerCallback");

    OpcUa_ReferenceParameter(a_hTimer);
    OpcUa_ReferenceParameter(a_msecElapsed);

    pConnection         = (OpcUa_Connection*)a_pvCallbackData;
    pSecureConnection   = (OpcUa_SecureConnection*)pConnection->Handle;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_RenewTimerCallback\n");

    OPCUA_SECURECONNECTION_LOCK(pSecureConnection);

    if(pSecureConnection->State != OpcUa_SecureConnectionState_Connected)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_RenewTimerCallback: Secure connection with channel %u in invalid state (%u) for renew!\n", pSecureConnection->pSecureChannel->SecureChannelId, pSecureConnection->State);
    }
    else
    {
        if(pSecureConnection->bRenewActive)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_RenewTimerCallback: Renew secure channel token of channel %u already under way! (%u msec)!\n", pSecureConnection->pSecureChannel->SecureChannelId, a_msecElapsed);
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_RenewTimerCallback: Starting renew secure channel token of channel %u after %u msec!\n", pSecureConnection->pSecureChannel->SecureChannelId, a_msecElapsed);
            pSecureConnection->bRenewActive = OpcUa_True;

            OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

            uStatus = OpcUa_SecureConnection_BeginOpenSecureChannel(pConnection,
                                                                    OpcUa_SecurityTokenRequestType_Renew);

            OPCUA_SECURECONNECTION_LOCK(pSecureConnection);

            if(OpcUa_IsBad(uStatus))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_RenewTimerCallback: Could not send OpenSecureChannel request to renew the secure channel token! 0x%08X\n", uStatus);
                pSecureConnection->bRenewActive = OpcUa_False;
            }
        }
    }

    OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureRequest_Create
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureRequest_Create(OpcUa_SecureRequest** a_ppRequest)
{
    OpcUa_SecureRequest* pRequest = OpcUa_Null;

    OpcUa_ReturnErrorIfArgumentNull(a_ppRequest);

    pRequest = (OpcUa_SecureRequest*)OpcUa_Alloc(sizeof(OpcUa_SecureRequest));
    OpcUa_ReturnErrorIfAllocFailed(pRequest);
    OpcUa_MemSet(pRequest, 0, sizeof(OpcUa_SecureRequest));

    *a_ppRequest = pRequest;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_SecureRequest_Delete
 *===========================================================================*/
static OpcUa_Void OpcUa_SecureRequest_Delete(OpcUa_SecureRequest** a_ppRequest)
{
    if(a_ppRequest != OpcUa_Null && *a_ppRequest != OpcUa_Null)
    {
        OpcUa_Free(*a_ppRequest);
        *a_ppRequest = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_SecureConnection_RemoveSecureRequestByType
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_RemoveSecureRequestByType(  OpcUa_Connection*       a_pConnection,
                                                                    OpcUa_SecureRequestType a_eRequestType,
                                                                    OpcUa_SecureRequest**   a_ppSecureRequest)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "RemoveSecureRequestByType");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureRequest);

    *a_ppSecureRequest = pSecureRequest;

    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    /* get the SecureRequest from the list of pending reuqests */
    OpcUa_List_Enter(pSecureConnection->PendingRequests); /****************************************/
    OpcUa_List_ResetCurrent(pSecureConnection->PendingRequests);
    pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);

    while(pSecureRequest != OpcUa_Null)
    {
        if(pSecureRequest->Type == a_eRequestType)
        {
            /* found */
            *a_ppSecureRequest = pSecureRequest;
            OpcUa_List_DeleteCurrentElement(pSecureConnection->PendingRequests);
            break;
        }

        pSecureRequest = (OpcUa_SecureRequest *)OpcUa_List_GetNextElement(pSecureConnection->PendingRequests);
    }

    OpcUa_List_Leave(pSecureConnection->PendingRequests); /****************************************/

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_RemoveSecureRequestById
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_RemoveSecureRequestById(    OpcUa_Connection*       a_pConnection,
                                                                    OpcUa_UInt32            a_uRequestId,
                                                                    OpcUa_SecureRequest**   a_ppSecureRequest)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "RemoveSecureRequestById");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureRequest);

    *a_ppSecureRequest = pSecureRequest;

    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    /* get the SecureRequest from the list of pending reuqests */
    OpcUa_List_Enter(pSecureConnection->PendingRequests); /****************************************/
    OpcUa_List_ResetCurrent(pSecureConnection->PendingRequests);
    pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);

    while(pSecureRequest != OpcUa_Null)
    {
        if(pSecureRequest->RequestId == a_uRequestId)
        {
            /* found */
            *a_ppSecureRequest = pSecureRequest;
            OpcUa_List_DeleteCurrentElement(pSecureConnection->PendingRequests);
            break;
        }

        pSecureRequest = (OpcUa_SecureRequest *)OpcUa_List_GetNextElement(pSecureConnection->PendingRequests);
    }

    OpcUa_List_Leave(pSecureConnection->PendingRequests); /****************************************/

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_AddPendingRequest
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_AddPendingRequest(   OpcUa_Connection*               a_pConnection,
                                                                    OpcUa_UInt32                    a_uRequestId,
                                                                    OpcUa_UInt32                    a_uTimeout,
                                                                    OpcUa_SecureRequestType         a_eSecureRequestType,
                                                                    OpcUa_Connection_PfnOnResponse* a_pCallback,
                                                                    OpcUa_Void*                     a_pCallbackData,
                                                                    OpcUa_SecureRequest**           a_ppSecureRequest)
{
    OpcUa_SecureRequest*    pSecureRequest          = OpcUa_Null;
    OpcUa_SecureConnection* pSecureConnection       = (OpcUa_SecureConnection*)a_pConnection->Handle;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "AddPendingRequest");

    *a_ppSecureRequest = OpcUa_Null;

    /*** create SecureRequest ***/
    uStatus = OpcUa_SecureRequest_Create(&pSecureRequest);
    OpcUa_GotoErrorIfBad(uStatus);

    pSecureRequest->RequestId           = a_uRequestId;
    pSecureRequest->Connection          = a_pConnection;
    pSecureRequest->Callback            = a_pCallback;
    pSecureRequest->CallbackData        = a_pCallbackData;
    pSecureRequest->StartTime           = OpcUa_GetTickCount();
    pSecureRequest->OperationTimeout    = a_uTimeout; /* overflow is handled in the timer */
    pSecureRequest->State               = OpcUa_SecureRequestState_Open;
    pSecureRequest->Type                = a_eSecureRequestType;

    OpcUa_List_Enter(pSecureConnection->PendingRequests); /****************************************/

    if(a_uTimeout != OPCUA_INFINITE)
    {
        OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                        "OpcUa_SecureConnection_AddPendingRequest: Added request with Id %u and timeout %u msec\n",
                        a_uRequestId,
                        a_uTimeout);
    }
    else
    {
        OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                        "OpcUa_SecureConnection_AddPendingRequest: Added request with Id %u and no timeout\n",
                        a_uRequestId);
    }

    uStatus = OpcUa_List_AddElement(pSecureConnection->PendingRequests, (OpcUa_Void*)pSecureRequest);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_List_Leave(pSecureConnection->PendingRequests);
        OpcUa_SecureRequest_Delete(&pSecureRequest);
        goto Error;
    }

    *a_ppSecureRequest = pSecureRequest;

    OpcUa_List_Leave(pSecureConnection->PendingRequests); /****************************************/

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_OnOpenSecureChannelResponse
 *===========================================================================*/
/* only called in case of error */
OpcUa_StatusCode OpcUa_SecureConnection_OnOpenSecureChannelResponse(
    OpcUa_Connection*         a_pConnection,
    OpcUa_Void*               a_pCallbackData,
    OpcUa_StatusCode          a_uRequestStatus,
    OpcUa_InputStream**       a_ppIstrm)
{
    OpcUa_OpenSecureChannelRequest* pRequest = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "OnOpenSecureChannelResponse");

    OpcUa_ReferenceParameter(a_pConnection);
    OpcUa_ReferenceParameter(a_uRequestStatus);
    OpcUa_ReferenceParameter(a_ppIstrm);

    pRequest = (OpcUa_OpenSecureChannelRequest*)a_pCallbackData;

    if(pRequest != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_OpenSecureChannel
 *===========================================================================*/
/* gets called from OnNotify on the first connect event */
static OpcUa_StatusCode OpcUa_SecureConnection_BeginOpenSecureChannel(OpcUa_Connection*                 a_pConnection,
                                                                      OpcUa_SecurityTokenRequestType    a_eTokenRequestType)
{
    OpcUa_Encoder*                      pEncoder                        = OpcUa_Null;
    OpcUa_OutputStream*                 pOstrm                          = OpcUa_Null;
    OpcUa_MessageContext                cContext;
    OpcUa_Handle                        hEncodeContext                  = OpcUa_Null;

    OpcUa_SecureConnection*             pSecureConnection               = OpcUa_Null;
    OpcUa_OpenSecureChannelRequest*     pOpenSecureChannelRequest       = OpcUa_Null;

    OpcUa_UInt32                        uTimeout                        = OPCUA_INFINITE;
    OpcUa_UInt32                        uSecureChannelId                = 0;

    OpcUa_Key                           clientNonce                     = {0, {0, OpcUa_Null}, OpcUa_Null};
    OpcUa_ByteString                    serverCertificateThumbprint     = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_MessageSecurityMode           eMessageSecurityMode            = OpcUa_MessageSecurityMode_None;
    OpcUa_Boolean                       bIsLocked                       = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "BeginOpenSecureChannel");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);
    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    /*** initialization block ***/
    OpcUa_MessageContext_Initialize(&cContext);

    pOpenSecureChannelRequest = (OpcUa_OpenSecureChannelRequest*)OpcUa_Alloc(sizeof(OpcUa_OpenSecureChannelRequest));
    OpcUa_ReturnErrorIfAllocFailed(pOpenSecureChannelRequest);
    OpcUa_OpenSecureChannelRequest_Initialize(pOpenSecureChannelRequest);

    pEncoder = pSecureConnection->Encoder;

    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);
    bIsLocked = OpcUa_True;

    switch(a_eTokenRequestType)
    {
        case OpcUa_SecurityTokenRequestType_Issue:
        {
            uSecureChannelId = 0;

            if(pSecureConnection->State != OpcUa_SecureConnectionState_ConnectingTransport)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "BeginOpenSecureChannel: Invalid State!\n");
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
            }
            pSecureConnection->State = OpcUa_SecureConnectionState_ConnectingSecure;

            uStatus = OpcUa_TcpSecureChannel_Create(&pSecureConnection->pSecureChannel);
            OpcUa_GotoErrorIfBad(uStatus);

            /*** create crypto provider ***/
            pSecureConnection->pSecureChannel->pCurrentCryptoProvider = (OpcUa_CryptoProvider*)OpcUa_Alloc(sizeof(OpcUa_CryptoProvider));
            OpcUa_GotoErrorIfAllocFailed(pSecureConnection->pSecureChannel->pCurrentCryptoProvider);
            OpcUa_MemSet(pSecureConnection->pSecureChannel->pCurrentCryptoProvider, 0, sizeof(OpcUa_CryptoProvider));

            uStatus = OPCUA_P_CRYPTOFACTORY_CREATECRYPTOPROVIDER(   OpcUa_String_GetRawString(pSecureConnection->pRequestedSecurityPolicyUri),
                                                                    pSecureConnection->pSecureChannel->pCurrentCryptoProvider);
            OpcUa_GotoErrorIfBad(uStatus);

            /* Overwrite with null securechannel id */
            pSecureConnection->pSecureChannel->SecureChannelId           = OPCUA_SECURECHANNEL_ID_INVALID;
            pSecureConnection->pSecureChannel->MessageSecurityMode       = pSecureConnection->MessageSecurityMode;
            pSecureConnection->pSecureChannel->TransportConnection       = a_pConnection;

            break;
        }
        case OpcUa_SecurityTokenRequestType_Renew:
        {
            if(pSecureConnection->State != OpcUa_SecureConnectionState_Connected)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "BeginOpenSecureChannel: Invalid State!\n");
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
            }

            /* get SecureChannelId from SecureConnection */
            uStatus = OpcUa_SecureConnection_GetChannelId(  a_pConnection,
                                                            &uSecureChannelId);
            OpcUa_GotoErrorIfBad(uStatus);

            uTimeout = (OpcUa_UInt32)(pSecureConnection->nLifetime * 0.5);

            break;
        }
        default:
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
        }
    }/* switch a_tokenRequestType */

    if(pSecureConnection->MessageSecurityMode != OpcUa_MessageSecurityMode_None)
    {
        /*** generate ReceiverCertificateThumbprint for writing it into the OpenSecureChannelRequest header ***/
        uStatus = pSecureConnection->pSecureChannel->pCurrentCryptoProvider->GetCertificateThumbprint(
            pSecureConnection->pSecureChannel->pCurrentCryptoProvider,
            pSecureConnection->ServerCertificate, /* get required length */
            &serverCertificateThumbprint);

        OpcUa_GotoErrorIfBad(uStatus);

        if(serverCertificateThumbprint.Length <= 0)
        {
            uStatus = OpcUa_Bad;
            OpcUa_GotoErrorIfBad(uStatus);
        }

        serverCertificateThumbprint.Data = (OpcUa_Byte*)OpcUa_Alloc(serverCertificateThumbprint.Length);
        OpcUa_GotoErrorIfAllocFailed(serverCertificateThumbprint.Data);

        uStatus = pSecureConnection->pSecureChannel->pCurrentCryptoProvider->GetCertificateThumbprint(
            pSecureConnection->pSecureChannel->pCurrentCryptoProvider,
            pSecureConnection->ServerCertificate, /* get the real data */
            &serverCertificateThumbprint);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* must always sign and encrypt OpenSecureChannel messages when security is used. */
    eMessageSecurityMode = pSecureConnection->MessageSecurityMode;

    if(eMessageSecurityMode != OpcUa_MessageSecurityMode_None)
    {
        eMessageSecurityMode = OpcUa_MessageSecurityMode_SignAndEncrypt;
    }

    /* create output stream through connection */
    uStatus = OpcUa_SecureConnection_BeginSendOpenSecureChannelRequest(
        a_pConnection,
        pSecureConnection->pRequestedSecurityPolicyUri,
        eMessageSecurityMode,
        pSecureConnection->pSecureChannel->pCurrentCryptoProvider,
        pSecureConnection->ClientCertificate,
        &serverCertificateThumbprint,
        &pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check MessageSecurityMode */
    switch(pSecureConnection->MessageSecurityMode)
    {
    case OpcUa_MessageSecurityMode_None:
        {
            /* generate fake client nonce that is sent to the server */
            clientNonce.Type          = OpcUa_Crypto_KeyType_Random;
            clientNonce.Key.Length    = -1;
            clientNonce.Key.Data      = OpcUa_Null;
            clientNonce.fpClearHandle = OpcUa_Null;

            break;
        }
    case OpcUa_MessageSecurityMode_Sign:
    case OpcUa_MessageSecurityMode_SignAndEncrypt:
        {
            /* use settings from the cryptoprovider for the keylength */
            uStatus = pSecureConnection->pSecureChannel->pCurrentCryptoProvider->GenerateKey(
                pSecureConnection->pSecureChannel->pCurrentCryptoProvider,
                -1,
                &clientNonce);

            OpcUa_GotoErrorIfBad(uStatus);

            clientNonce.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(clientNonce.Key.Length);
            OpcUa_GotoErrorIfAllocFailed(clientNonce.Key.Data);

            uStatus = pSecureConnection->pSecureChannel->pCurrentCryptoProvider->GenerateKey(
                pSecureConnection->pSecureChannel->pCurrentCryptoProvider,
                -1,
                &clientNonce);

            break;
        }
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

    /*** fill request header ***/
    pOpenSecureChannelRequest->RequestHeader.Timestamp      = OPCUA_P_DATETIME_UTCNOW();

    /*** fill request body ***/
    pOpenSecureChannelRequest->RequestType           = a_eTokenRequestType;
    pOpenSecureChannelRequest->ClientProtocolVersion = 0;
    pOpenSecureChannelRequest->SecurityMode          = pSecureConnection->MessageSecurityMode;
    pOpenSecureChannelRequest->ClientNonce.Length    = clientNonce.Key.Length;
    pOpenSecureChannelRequest->ClientNonce.Data      = clientNonce.Key.Data;
    pOpenSecureChannelRequest->RequestedLifetime     = pSecureConnection->nLifetime;

    /*** encode the OpenSecureChannelRequest ***/

    uStatus = pEncoder->Open(pEncoder, pOstrm, &cContext, &hEncodeContext);
    OpcUa_GotoErrorIfBad(uStatus);
    uStatus = pEncoder->WriteMessage(   (struct _OpcUa_Encoder*)hEncodeContext,
                                        (OpcUa_Void*)pOpenSecureChannelRequest,
                                        &OpcUa_OpenSecureChannelRequest_EncodeableType);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

    /* unlock the channel mutex */
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    bIsLocked = OpcUa_False;

    /*** secure and send the OpenSecureChannelRequest ***/
    uStatus = OpcUa_SecureConnection_EndSendOpenSecureChannelRequest(   a_pConnection,
                                                                        &pOstrm,
                                                                        pOpenSecureChannelRequest,
                                                                        uTimeout,
                                                                        OpcUa_SecureConnection_OnOpenSecureChannelResponse,  /* callback */
                                                                        OpcUa_Null);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** clean up ***/
    OpcUa_MessageContext_Clear(&cContext);

    OpcUa_ByteString_Clear(&serverCertificateThumbprint);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* unlock the channel mutex */
    if(bIsLocked == OpcUa_True)
    {
        OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    }

    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

    if(pOstrm != OpcUa_Null)
    {
        OpcUa_Stream_Delete(&((OpcUa_SecureStream*)pOstrm->Handle)->InnerStrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pOstrm);
    }

    OpcUa_MessageContext_Clear(&cContext);

    OpcUa_ByteString_Clear(&serverCertificateThumbprint);

    OpcUa_Key_Clear(&clientNonce);

    /* OpcUa_OpenSecureChannelRequest_Clear(pOpenSecureChannelRequest); */
    OpcUa_Free(pOpenSecureChannelRequest);

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureConnection_Connect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_Connect(
    OpcUa_Connection*               a_pConnection,
    OpcUa_String*                   a_sUrl,
    OpcUa_ClientCredential*         a_pCredentials,
    OpcUa_UInt32                    a_nTimeout,
    OpcUa_Connection_PfnOnNotify*   a_pCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_SecureConnection* pSecureConnection  = OpcUa_Null;
    OpcUa_ActualCredential* pClientCredentials = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "Connect");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_sUrl);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallback);
    OpcUa_ReturnErrorIfArgumentNull(a_pCredentials);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureConnection, a_pConnection, Connect);

    /* cast to internal types */
    pClientCredentials = &a_pCredentials->Credential.TheActuallyUsedCredential;
    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;
    OpcUa_ReturnErrorIfNull(pSecureConnection, OpcUa_BadInvalidState);
    if(pSecureConnection->State != OpcUa_SecureConnectionState_Disconnected)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_ReturnErrorIfBad(uStatus);
    }

    OpcUa_String_Clear(&pSecureConnection->sUrl);
    uStatus = OpcUa_String_StrnCpy( &pSecureConnection->sUrl,
                                    a_sUrl,
                                    OPCUA_STRING_LENDONTCARE);
    OpcUa_GotoErrorIfBad(uStatus);

    /* PKI provider */
    if(pSecureConnection->ClientPKIProvider != OpcUa_Null)
    {
        OPCUA_P_PKIFACTORY_DELETEPKIPROVIDER(pSecureConnection->ClientPKIProvider);
        OpcUa_Free(pSecureConnection->ClientPKIProvider);
        pSecureConnection->ClientPKIProvider = OpcUa_Null;
    }
    pSecureConnection->ClientPKIProvider = (OpcUa_PKIProvider*)OpcUa_Alloc(sizeof(OpcUa_PKIProvider));
    OpcUa_GotoErrorIfAllocFailed(pSecureConnection->ClientPKIProvider);
    uStatus = OPCUA_P_PKIFACTORY_CREATEPKIPROVIDER(pClientCredentials->pkiConfig, pSecureConnection->ClientPKIProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    /* security policy */
    if(pSecureConnection->pRequestedSecurityPolicyUri != OpcUa_Null)
    {
        OpcUa_String_Delete(&pSecureConnection->pRequestedSecurityPolicyUri);
    }
    pSecureConnection->pRequestedSecurityPolicyUri = (OpcUa_String*)OpcUa_Alloc(sizeof(OpcUa_String));
    OpcUa_GotoErrorIfAllocFailed(pSecureConnection->pRequestedSecurityPolicyUri);
    OpcUa_String_Initialize(pSecureConnection->pRequestedSecurityPolicyUri);
    uStatus = OpcUa_String_StrnCpy( pSecureConnection->pRequestedSecurityPolicyUri,
                                    pClientCredentials->pRequestedSecurityPolicyUri,
                                    OPCUA_STRING_LENDONTCARE);
    OpcUa_GotoErrorIfBad(uStatus);

    /* private key */
    if(pSecureConnection->ClientPrivateKey != OpcUa_Null)
    {
        OpcUa_Free(pSecureConnection->ClientPrivateKey);
        pSecureConnection->ClientPrivateKey = OpcUa_Null;
    }

    OpcUa_String_Clear(&pSecureConnection->sUrl);

    pSecureConnection->ClientPrivateKey = (OpcUa_Key*)OpcUa_Alloc(sizeof(OpcUa_Key));
    OpcUa_GotoErrorIfAllocFailed(pSecureConnection->ClientPrivateKey);
    *(pSecureConnection->ClientPrivateKey)          = *(pClientCredentials->pClientPrivateKey);

    pSecureConnection->ClientCertificate            = pClientCredentials->pClientCertificate;
    pSecureConnection->ServerCertificate            = pClientCredentials->pServerCertificate;
    pSecureConnection->MessageSecurityMode          = pClientCredentials->messageSecurityMode;
    pSecureConnection->nLifetime                    = pClientCredentials->nRequestedLifetime;

    pSecureConnection->Callback     = a_pCallback;
    pSecureConnection->CallbackData = a_pCallbackData;
    pSecureConnection->State        = OpcUa_SecureConnectionState_ConnectingTransport;

    /* connect the non-secure connection */
    uStatus = pSecureConnection->TransportConnection->Connect(  pSecureConnection->TransportConnection,
                                                                a_sUrl,
                                                                a_pCredentials,
                                                                a_nTimeout,
                                                                OpcUa_SecureConnection_OnNotify,
                                                                a_pConnection);
   OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* clean up members set in this function */
    if(pSecureConnection->ClientPrivateKey != OpcUa_Null)
    {
        OpcUa_Free(pSecureConnection->ClientPrivateKey);
        pSecureConnection->ClientPrivateKey = OpcUa_Null;
        OpcUa_String_Clear(&pSecureConnection->sUrl);
    }

    if(pSecureConnection->ClientPKIProvider != OpcUa_Null)
    {
        OPCUA_P_PKIFACTORY_DELETEPKIPROVIDER(pSecureConnection->ClientPKIProvider);
        OpcUa_Free(pSecureConnection->ClientPKIProvider);
        pSecureConnection->ClientPKIProvider = OpcUa_Null;
    }

    if(pSecureConnection->pRequestedSecurityPolicyUri != OpcUa_Null)
    {
        OpcUa_String_Delete(&pSecureConnection->pRequestedSecurityPolicyUri);
    }

    OpcUa_String_Clear(&pSecureConnection->sUrl);

    /* We must prevent the connection from using the callback later on. */
    pSecureConnection->Callback     = OpcUa_Null;
    pSecureConnection->CallbackData = OpcUa_Null;
    pSecureConnection->State        = OpcUa_SecureConnectionState_Unknown;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_CancelOpenRequests
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_CancelOpenRequests(OpcUa_Connection* a_pConnection,
                                                                  OpcUa_StatusCode  a_uStatus)
{
    OpcUa_SecureConnection* pSecureConnection = OpcUa_Null;
    OpcUa_UInt32            nPendingRequests = 0;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "CancelOpenRequests");

    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    /* signal and delete all pending requests */
    OpcUa_List_Enter(pSecureConnection->PendingRequests);

    uStatus = OpcUa_List_GetNumberOfElements(pSecureConnection->PendingRequests, &nPendingRequests);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_CancelOpenRequests: Error notifying open requests.\n");
    }    /* abandon open requests */

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_CancelOpenRequests: Canceling %u open requests.\n", nPendingRequests);

    if(nPendingRequests != 0)
    {
        /* reset list */
        OpcUa_List_ResetCurrent(pSecureConnection->PendingRequests);

        /* get first element */
        pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);

        while(pSecureRequest != OpcUa_Null)
        {
            /* tell all waiting callbacks of the cancellation */
            if(pSecureRequest->Callback != OpcUa_Null)
            {
                /* notify the upper layer about the open request */
                pSecureRequest->Callback(   a_pConnection,                  /* source of the event      */
                                            pSecureRequest->CallbackData,   /* the callback data        */
                                            a_uStatus,                      /* status of the request    */
                                            OpcUa_Null);                    /* the stream to read from  */
            }

            /* callback finished, delete the internal resource */
            OpcUa_List_DeleteCurrentElement(pSecureConnection->PendingRequests);
            OpcUa_SecureRequest_Delete(&pSecureRequest);

            /* get next request */
            pSecureRequest = (OpcUa_SecureRequest*)OpcUa_List_GetCurrentElement(pSecureConnection->PendingRequests);
        }
    }

    OpcUa_List_Leave(pSecureConnection->PendingRequests);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_AbortResponseAndClose
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_AbortResponseAndClose(   OpcUa_Connection*   a_pSecureConnectionInterface,
                                                                        OpcUa_StatusCode    a_uConnectionStatus,
                                                                        OpcUa_InputStream** a_ppTransportInputStream)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel      = OpcUa_Null;
    OpcUa_InputStream*      pSecureIstrm        = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "AbortResponseAndClose");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureConnectionInterface);

    OpcUa_ReferenceParameter(a_ppTransportInputStream);

    /*** get connection handle ***/
    pSecureConnection = (OpcUa_SecureConnection*)a_pSecureConnectionInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureConnection, OpcUa_BadInvalidState);
    pSecureChannel = pSecureConnection->pSecureChannel;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_AbortResponseAndClose: Clearing current input stream. 0x%08X\n", a_uConnectionStatus);

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(    pSecureChannel,
                                                            &pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Clear the pending input stream marker */
    uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                            OpcUa_Null);

    if(pSecureIstrm != OpcUa_Null)
    {
        pSecureStream = (OpcUa_SecureStream*)pSecureIstrm->Handle;

        /*** get SecureRequest from list of pending SecureRequests ***/
        uStatus = OpcUa_SecureConnection_RemoveSecureRequestById(   a_pSecureConnectionInterface,
                                                                    pSecureStream->RequestId,
                                                                    &pSecureRequest);
        if(pSecureRequest == OpcUa_Null)
        {
            /* no matching SecureRequest was found */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_AbortResponseAndClose: Unknown response with id %u\n", pSecureStream->RequestId);
        }
    }

    /* close connection without closing securechannel */
    pSecureConnection->TransportConnection->Disconnect(pSecureConnection->TransportConnection, OpcUa_False);

    if(pSecureRequest != OpcUa_Null)
    {
        /*** invoke callback ***/
        if(pSecureRequest->Callback != OpcUa_Null)
        {
            pSecureRequest->Callback(   a_pSecureConnectionInterface,
                                        pSecureRequest->CallbackData,
                                        a_uConnectionStatus,
                                        OpcUa_Null);
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_AbortResponseAndClose: Callback is NULL!\n");
        }

        /* delete request */
        if(pSecureRequest != OpcUa_Null)
        {
            OpcUa_SecureRequest_Delete(&pSecureRequest);
            pSecureRequest = OpcUa_Null;
        }
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        pSecureIstrm->Delete((OpcUa_Stream**)&pSecureIstrm);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureConnection_AbortResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_AbortResponse(   OpcUa_Connection*   a_pSecureConnectionInterface,
                                                                OpcUa_StatusCode    a_uConnectionStatus,
                                                                OpcUa_InputStream** a_ppTransportInputStream)
{
    OpcUa_SecureMessageType eSecureMessageType      = OpcUa_SecureMessageType_UN;
    OpcUa_SecureConnection* pSecureConnection       = OpcUa_Null;
    OpcUa_UInt32            uSecureChannelId        = 0;
    OpcUa_UInt32            uTokenId                = 0;
    OpcUa_SecureChannel*    pSecureChannel          = OpcUa_Null;
    OpcUa_InputStream*      pSecureIstrm            = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream           = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest          = OpcUa_Null;

    OpcUa_UInt32            uLoop                   = 0;
    OpcUa_StatusCode        uAbortStatusCode        = OpcUa_Good;

    OpcUa_SecurityKeyset*   pReceivingKeyset        = OpcUa_Null;

    OpcUa_CryptoProvider*   pCryptoProvider         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "AbortResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureConnectionInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportInputStream);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppTransportInputStream);

    OpcUa_ReferenceParameter(a_uConnectionStatus);

    /*** get connection handle ***/
    pSecureConnection = (OpcUa_SecureConnection*)a_pSecureConnectionInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureConnection, OpcUa_BadInvalidState);
    pSecureChannel = pSecureConnection->pSecureChannel;

    /*** check type of incoming service request ***/
    uStatus = OpcUa_SecureStream_CheckInputHeaderType(  *a_ppTransportInputStream,
                                                        &eSecureMessageType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* parse stream header */
    uStatus = OpcUa_SecureStream_DecodeSymmetricSecurityHeader( *a_ppTransportInputStream,
                                                                &uSecureChannelId,
                                                                &uTokenId);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_AbortResponse: SID %u, TID %u, Status 0x%08X\n", uSecureChannelId, uTokenId, a_uConnectionStatus);

    /* check that the securechannel id has not changed */
    if(pSecureChannel->SecureChannelId != uSecureChannelId)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(    pSecureChannel,
                                                            &pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Clear the pending input stream marker */
    uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                            OpcUa_Null);

    if(pSecureIstrm == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadUnexpectedError);
    }

    pSecureStream = (OpcUa_SecureStream*)pSecureIstrm->Handle;

    /* remove all prior buffers from the stream and make the last chunk the only one to allow decoder access to it. */
    for(uLoop = 0; uLoop < pSecureStream->nBuffers; uLoop++)
    {
        OpcUa_Buffer_Clear(&pSecureStream->Buffers[uLoop]);
    }

    pSecureStream->nBuffers = 0;

    /* Get reference to keyset for requested token id */
    uStatus = pSecureChannel->GetSecuritySet(   pSecureChannel,
                                                uTokenId,
                                                &pReceivingKeyset,
                                                OpcUa_Null,
                                                &pCryptoProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    /* append latest chunk */
    uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportInputStream,
                                                pSecureIstrm,
                                                &pReceivingKeyset->SigningKey,
                                                &pReceivingKeyset->EncryptionKey,
                                                &pReceivingKeyset->InitializationVector,
                                                pCryptoProvider,
                                                pSecureChannel);
    /* release reference to security set */
    pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                          uTokenId);

    OpcUa_GotoErrorIfBad(uStatus);

    /* decode status for the cancellation; status is required for the callback */
    uStatus = OpcUa_UInt32_BinaryDecode(&uAbortStatusCode, pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** get SecureRequest from list of pending SecureRequests ***/
    uStatus = OpcUa_SecureConnection_RemoveSecureRequestById(   a_pSecureConnectionInterface,
                                                                pSecureStream->RequestId,
                                                                &pSecureRequest);
    if(pSecureRequest == OpcUa_Null)
    {
        /* no matching SecureRequest was found */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_AbortResponse: Unknown response with id %u\n", pSecureStream->RequestId);
        OpcUa_GotoErrorWithStatus(OpcUa_BadUnknownResponse);
    }

    /*** invoke callback ***/
    if(pSecureRequest->Callback != OpcUa_Null)
    {
        pSecureRequest->Callback(   a_pSecureConnectionInterface,
                                    pSecureRequest->CallbackData,
                                    uAbortStatusCode,
                                    OpcUa_Null);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_AbortResponse: Callback is NULL!\n");
    }

    /* delete request */
    if(pSecureRequest != OpcUa_Null)
    {
        OpcUa_SecureRequest_Delete(&pSecureRequest);
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        pSecureIstrm->Delete((OpcUa_Stream**)&pSecureIstrm);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureIstrm != OpcUa_Null)
    {
        pSecureIstrm->Delete((OpcUa_Stream**)&pSecureIstrm);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_OnNotify
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureConnection_OnNotify(
    OpcUa_Connection*     a_pTransportConnection,
    OpcUa_Void*           a_pCallbackData,
    OpcUa_ConnectionEvent a_eEvent,
    OpcUa_InputStream**   a_ppTransportInputStream,
    OpcUa_StatusCode      a_uConnectionStatus)
{
    OpcUa_Connection*       pSecureConnectionInterface  = OpcUa_Null;
    OpcUa_SecureConnection* pSecureConnection           = OpcUa_Null;
    OpcUa_Timer             hTimerToDelete              = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "OnNotify");

    OpcUa_ReferenceParameter(a_pTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallbackData);

    pSecureConnectionInterface = (OpcUa_Connection*)a_pCallbackData;
    pSecureConnection = (OpcUa_SecureConnection*)pSecureConnectionInterface->Handle;

    OPCUA_SECURECONNECTION_LOCK_RESPONSE(pSecureConnection);

    switch(a_eEvent)
    {
    case OpcUa_ConnectionEvent_Connect:
        {
            /* no longer connected due to either an error or a disconnect. */
            if(OpcUa_IsBad(a_uConnectionStatus))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_OnNotify: Connect event: ERROR 0x%08X!\n", a_uConnectionStatus);
                pSecureConnection->State = OpcUa_SecureConnectionState_Unknown;
                uStatus = a_uConnectionStatus;
            }
            else
            {
                OpcUa_UInt32 uReceiveBufferSize = 0;

                /* (re-)calculate the max number of buffers accepted */
                OpcUa_Connection_GetReceiveBufferSize(  a_pTransportConnection,
                                                        &uReceiveBufferSize);
                pSecureConnection->nMaxBuffersPerMessage = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize/uReceiveBufferSize + 1;

                /* set state to connected if connected successfully. */
                if(pSecureConnection->State == OpcUa_SecureConnectionState_ConnectingTransport)
                {
                    /* we are connecting and have to issue an opensecurechannel request */
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_OnNotify: Connect event: Trying to open secure channel.\n");
                    OPCUA_SECURECONNECTION_UNLOCK_RESPONSE(pSecureConnection);
                    uStatus = OpcUa_SecureConnection_BeginOpenSecureChannel(pSecureConnectionInterface,
                                                                            OpcUa_SecurityTokenRequestType_Issue);
                    OPCUA_SECURECONNECTION_LOCK_RESPONSE(pSecureConnection);
                    if(OpcUa_IsBad(uStatus))
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_OnNotify: Could not send Open Secure Channel Request! 0x%08X\n", uStatus);
                    }
                }
            }

            /* notify the owner only if an error occurred */
            if(OpcUa_IsBad(uStatus) && pSecureConnection->Callback != OpcUa_Null)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_OnNotify: Connect event: Notifying owner! 0x%08X\n", uStatus);
                pSecureConnection->Callback(    pSecureConnectionInterface,
                                                pSecureConnection->CallbackData,
                                                a_eEvent,
                                                a_ppTransportInputStream,
                                                uStatus);
            }
            break;
        }
    case OpcUa_ConnectionEvent_Disconnect:
        {
            switch(pSecureConnection->State)
            {
            case OpcUa_SecureConnectionState_Connected:
            case OpcUa_SecureConnectionState_DisconnectingSecure:
                {
                    hTimerToDelete = pSecureConnection->hRenewTimer;
                    pSecureConnection->hRenewTimer = OpcUa_Null;
                    pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

                    /* reconnect process already active - reconnect failed */
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_OnNotify: Disconnected!\n");

                    if(pSecureConnection->Callback != OpcUa_Null)
                    {
                        /* inform the owner about the current status. */
                        pSecureConnection->Callback(pSecureConnectionInterface,
                                                    pSecureConnection->CallbackData,
                                                    OpcUa_ConnectionEvent_Disconnect,
                                                    a_ppTransportInputStream,
                                                    OpcUa_Good);
                    }

                    /* cancel all open requests */
                    OpcUa_SecureConnection_CancelOpenRequests(  pSecureConnectionInterface,
                                                                OpcUa_BadDisconnect);

                    break;
                }
            case OpcUa_SecureConnectionState_ConnectingSecure:
                {
                    pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;
                    hTimerToDelete = pSecureConnection->hRenewTimer;
                    pSecureConnection->hRenewTimer = OpcUa_Null;
                    /* transport layer was disconnected while secure connection was waiting for the open secure channel response */
                    /* handled like an open secure channel response with transport layer failure */
                    OpcUa_SecureConnection_ProcessOpenSecureChannelResponse(    pSecureConnectionInterface,
                                                                                OpcUa_Null,
                                                                                OpcUa_BadConnectionClosed,
                                                                                OpcUa_True);
                    break;
                }
            case OpcUa_SecureConnectionState_ConnectingTransport:
                {
                    pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;
                    hTimerToDelete = pSecureConnection->hRenewTimer;
                    pSecureConnection->hRenewTimer = OpcUa_Null;
                    if(pSecureConnection->Callback != OpcUa_Null)
                    {
                        pSecureConnection->Callback(    pSecureConnectionInterface,
                                                        pSecureConnection->CallbackData,
                                                        a_eEvent,
                                                        OpcUa_Null,
                                                        a_uConnectionStatus);
                    }
                }
            default:
                {
                    /* nothing to do yet */
                    break;
                }
            }

            break;
        }
    case OpcUa_ConnectionEvent_ResponsePartial:
        {
            OpcUa_SecureConnection_InternalOnResponse(  pSecureConnectionInterface,
                                                        OpcUa_Null, /* callback data */
                                                        a_uConnectionStatus,
                                                        a_ppTransportInputStream,
                                                        OpcUa_False); /* request incomplete */
            break;
        }
    case OpcUa_ConnectionEvent_Response:
        {
            if(OpcUa_IsGood(a_uConnectionStatus))
            {
                OpcUa_SecureConnection_InternalOnResponse(  pSecureConnectionInterface,
                                                            OpcUa_Null, /* callback data */
                                                            a_uConnectionStatus,
                                                            a_ppTransportInputStream,
                                                            OpcUa_True); /* request complete */
            }
            else
            {
                /* bad status */
                uStatus = OpcUa_SecureConnection_AbortResponseAndClose( pSecureConnectionInterface,
                                                                        a_uConnectionStatus,
                                                                        a_ppTransportInputStream);

                hTimerToDelete = pSecureConnection->hRenewTimer;
                pSecureConnection->hRenewTimer = OpcUa_Null;
                pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

                if(pSecureConnection->Callback != OpcUa_Null)
                {
                    /* inform the owner about the current status. */
                    pSecureConnection->Callback(pSecureConnectionInterface,
                                                pSecureConnection->CallbackData,
                                                OpcUa_ConnectionEvent_Disconnect,
                                                a_ppTransportInputStream,
                                                a_uConnectionStatus);
                }

                /* cancel all open requests */
                OpcUa_SecureConnection_CancelOpenRequests(  pSecureConnectionInterface,
                                                            OpcUa_BadDisconnect);
            }
            break;
        }
    case OpcUa_ConnectionEvent_ResponseAbort:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureConnection_OnNotify: ResponseAbort!\n");
            /* received an abort message for the current response stream (graceful) */
            OpcUa_SecureConnection_AbortResponse(   pSecureConnectionInterface,
                                                    a_uConnectionStatus,
                                                    a_ppTransportInputStream);
            break;
        }
    case OpcUa_ConnectionEvent_RefillSendQueue:
        {
            OpcUa_SecureChannel*    pSecureChannel = pSecureConnection->pSecureChannel;
            OPCUA_SECURECONNECTION_UNLOCK_RESPONSE(pSecureConnection);

            if(pSecureChannel != OpcUa_Null)
            {
                pSecureChannel->LockWriteMutex(pSecureChannel);
                OpcUa_Connection_AddToSendQueue(
                    a_pTransportConnection,
                    pSecureChannel->pPendingSendBuffers,
                    0);
                pSecureChannel->bAsyncWriteInProgress = pSecureChannel->pPendingSendBuffers != OpcUa_Null;
                pSecureChannel->pPendingSendBuffers = OpcUa_Null;
                pSecureChannel->uPendingMessageCount = 0;
                pSecureChannel->UnlockWriteMutex(pSecureChannel);
            }
            return OpcUa_Good;
        }
    case OpcUa_ConnectionEvent_UnexpectedError:
        {
            switch(pSecureConnection->State)
            {
            case OpcUa_SecureConnectionState_ConnectingSecure:
                {
                    hTimerToDelete = pSecureConnection->hRenewTimer;
                    pSecureConnection->hRenewTimer = OpcUa_Null;
                    pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

                    OpcUa_SecureConnection_ProcessOpenSecureChannelResponse(    pSecureConnectionInterface,
                                                                                OpcUa_Null,
                                                                                a_uConnectionStatus,
                                                                                OpcUa_True);
                    break;
                }
            default:
                {
                    hTimerToDelete = pSecureConnection->hRenewTimer;
                    pSecureConnection->hRenewTimer = OpcUa_Null;
                    pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

                    /* cancel all open requests */
                    OpcUa_SecureConnection_CancelOpenRequests(  pSecureConnectionInterface,
                                                                a_uConnectionStatus);

                    if(pSecureConnection->Callback != OpcUa_Null)
                    {
                        /* notify upper layer about error condition */
                        pSecureConnection->Callback(pSecureConnectionInterface,
                                                    pSecureConnection->CallbackData,
                                                    a_eEvent,
                                                    a_ppTransportInputStream,
                                                    a_uConnectionStatus);
                    }

                }
            }
            break;
        }
    default:
        {
            /* ToDo: Handle Error Response */
            break;
        }
    }

    OPCUA_SECURECONNECTION_UNLOCK_RESPONSE(pSecureConnection);

    if(hTimerToDelete != OpcUa_Null)
    {
        OpcUa_Timer_Delete(&hTimerToDelete);
    }

    uStatus = OpcUa_Good;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_InternalOnResponse
 *===========================================================================*/
/* looks at the message header and forwards the incoming stream to the
   appropriate handler. */
static OpcUa_StatusCode OpcUa_SecureConnection_InternalOnResponse(
    OpcUa_Connection*   a_pSecureConnectionInterface,
    OpcUa_Void*         a_pCallbackData,
    OpcUa_StatusCode    a_uResponseStatus,
    OpcUa_InputStream** a_ppTransportIstrm,
    OpcUa_Boolean       a_bResponseComplete)
{
    OpcUa_SecureMessageType responseType        = OpcUa_SecureMessageType_UN;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "InternalOnResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureConnectionInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppTransportIstrm);

    OpcUa_ReferenceParameter(a_pCallbackData);

    if(OpcUa_IsBad(a_uResponseStatus))
    {
        /* Transport layer reports error in message exchange; directly goto error handling */
        uStatus = a_uResponseStatus;
        OpcUa_GotoError;
    }

    /* the inner connection must call this callback once for each - even when it is deleted */
    /* if it does then the request object will leak. */
    uStatus = OpcUa_SecureStream_CheckInputHeaderType(*a_ppTransportIstrm, &responseType);
    OpcUa_GotoErrorIfBad(uStatus);

    switch(responseType)
    {
    case OpcUa_SecureMessageType_SO:
        {
            uStatus = OpcUa_SecureConnection_ProcessOpenSecureChannelResponse(  a_pSecureConnectionInterface,
                                                                                *a_ppTransportIstrm,
                                                                                a_uResponseStatus,
                                                                                a_bResponseComplete);
            break;
        }
    case OpcUa_SecureMessageType_SM:
        {
            uStatus = OpcUa_SecureConnection_ProcessSessionCallResponse(    a_pSecureConnectionInterface,
                                                                            *a_ppTransportIstrm,
                                                                            a_uResponseStatus,
                                                                            a_bResponseComplete);
            break;
        }
    default:
        {
            uStatus = OpcUa_Bad;
            break;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_BeginSendRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_BeginSendRequest(
    OpcUa_Connection*       a_pConnection,
    OpcUa_OutputStream**    a_ppSecureOstrm)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel      = OpcUa_Null;
    OpcUa_OutputStream*     pInnerOstrm         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "BeginSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureOstrm);

    *a_ppSecureOstrm = OpcUa_Null;

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureConnection, a_pConnection, BeginSendRequest);

    pSecureConnection   = (OpcUa_SecureConnection*)a_pConnection->Handle;
    pSecureChannel      = pSecureConnection->pSecureChannel;
    OpcUa_ReturnErrorIfArgumentNull(pSecureChannel);

    /* acquire lock until operation is complete */
    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);

    if(pSecureConnection->State != OpcUa_SecureConnectionState_Connected)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_BeginSendRequest: Connection is not connected!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /* check whether SecureChannel is in a correct state */
    if((pSecureChannel == OpcUa_Null)|| (pSecureChannel->State != OpcUa_SecureChannelState_Opened))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_BeginSendRequest: SecureChannel is not connected!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /* create stream for the non-secure connection */
    uStatus = pSecureConnection->TransportConnection->BeginSendRequest( pSecureConnection->TransportConnection,
                                                                        &pInnerOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* create stream for writing */
    uStatus = OpcUa_SecureStream_CreateOutput(  pInnerOstrm,
                                                eOpcUa_SecureStream_Types_StandardMessage,
                                                pSecureConnection->uRequestId++,
                                                pSecureChannel,
                                                a_ppSecureOstrm);

    OpcUa_GotoErrorIfBad(uStatus);

    /*** release lock  ***/
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* clean up */
    OpcUa_Stream_Delete((OpcUa_Stream**)&pInnerOstrm);

    /*** release lock  ***/
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_BeginSendCloseSecureChannelRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_BeginSendCloseSecureChannelRequest(
    OpcUa_Connection*       a_pConnection,
    OpcUa_OutputStream**    a_ppSecureOstrm)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_OutputStream*     pInnerOstrm         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "BeginSendCloseSecureChannelRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureOstrm);

    *a_ppSecureOstrm = OpcUa_Null;

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureConnection, a_pConnection, BeginSendRequest);

    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    /*** acquire lock until operation is complete ***/
    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);

    /*** create stream for the non-secure connection ***/
    uStatus = pSecureConnection->TransportConnection->BeginSendRequest( pSecureConnection->TransportConnection,
                                                                        &pInnerOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_SecureStream_CreateOutput(  pInnerOstrm,
                                                eOpcUa_SecureStream_Types_CloseSecureChannel,
                                                pSecureConnection->uRequestId++,
                                                pSecureConnection->pSecureChannel,
                                                a_ppSecureOstrm);

    OpcUa_GotoErrorIfBad(uStatus);

    /*** release lock  ***/
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* clean up */
    OpcUa_Stream_Delete((OpcUa_Stream**)&pInnerOstrm);

    /*** release lock  ***/
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_Disconnect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_Disconnect( OpcUa_Connection* a_pSecureConnection,
                                                    OpcUa_Boolean     a_bNotifyOnComplete)
{
    OpcUa_SecureConnection*             pSecureConnection           = OpcUa_Null;
    OpcUa_CloseSecureChannelRequest*    pCloseSecureChannelRequest  = OpcUa_Null;
    OpcUa_OutputStream*                 pSecureOstrm                = OpcUa_Null;
    OpcUa_MessageContext                cContext;
    OpcUa_Timer                         hTimerToDelete              = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "Disconnect");

    OpcUa_ReferenceParameter(a_bNotifyOnComplete);

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureConnection);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureConnection, a_pSecureConnection, Disconnect);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_Disconnect: Disconnecting!\n");

    pSecureConnection = (OpcUa_SecureConnection*)a_pSecureConnection->Handle;

    OpcUa_ReturnErrorIfArgumentNull(pSecureConnection);

    /* acquire lock until disconnect is complete */
    OPCUA_SECURECONNECTION_LOCK(pSecureConnection);

    /* delete the reconnect timer if required */
    hTimerToDelete = pSecureConnection->hRenewTimer;
    pSecureConnection->hRenewTimer = OpcUa_Null;

    /* take action based on current state */
    switch(pSecureConnection->State)
    {
    case OpcUa_SecureConnectionState_Connected:
        {
            OpcUa_UInt32                        uSecureChannelId            = 0;
            OpcUa_Encoder*                      pEncoder                    = OpcUa_Null;
            OpcUa_Handle                        hEncodeContext              = OpcUa_Null;
            OpcUa_UInt32                        uCurrentTokenId             = 0;

            pCloseSecureChannelRequest = (OpcUa_CloseSecureChannelRequest*)OpcUa_Alloc(sizeof(OpcUa_CloseSecureChannelRequest));
            OpcUa_GotoErrorIfAllocFailed(pCloseSecureChannelRequest);
            OpcUa_CloseSecureChannelRequest_Initialize(pCloseSecureChannelRequest);

            /* set the connection object to disconnecting state */
            pSecureConnection->State = OpcUa_SecureConnectionState_DisconnectingSecure;

            /* get SecureChannelId from SecureConnection */
            uStatus = OpcUa_SecureConnection_GetChannelId(a_pSecureConnection, &uSecureChannelId);
            OpcUa_GotoErrorIfBad(uStatus);

            /* get current TokenId from SecureConnection */
            uStatus = OpcUa_SecureConnection_GetCurrentTokenId(a_pSecureConnection, &uCurrentTokenId);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_SecureConnection_BeginSendCloseSecureChannelRequest(a_pSecureConnection,
                                                                                &pSecureOstrm);
            OpcUa_GotoErrorIfBad(uStatus);

            /*** fill request header ***/
            pCloseSecureChannelRequest->RequestHeader.Timestamp = OPCUA_P_DATETIME_UTCNOW();

            /* open encoder */
            pEncoder = pSecureConnection->Encoder;

            /* initialize context */
            OpcUa_MessageContext_Initialize(&cContext);

            cContext.KnownTypes    = pSecureConnection->KnownTypes;
            cContext.NamespaceUris = pSecureConnection->NamespaceUris;

            uStatus = pEncoder->Open(   pEncoder,
                                        pSecureOstrm,
                                        &cContext,
                                        &hEncodeContext);
            OpcUa_GotoErrorIfBad(uStatus);

            /* encode message */
            uStatus = pEncoder->WriteMessage(   (struct _OpcUa_Encoder*)hEncodeContext,
                                                (OpcUa_Void*)pCloseSecureChannelRequest,
                                                &OpcUa_CloseSecureChannelRequest_EncodeableType);

            /* close encoder */
            OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

            /*** cleanup request data ***/
            OpcUa_MessageContext_Clear(&cContext);

            OpcUa_GotoErrorIfBad(uStatus);

            OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

            if(hTimerToDelete != OpcUa_Null)
            {
                OpcUa_Timer_Delete(&hTimerToDelete);
            }

            /* secure and send message with standard symmetric crypto */
            uStatus = OpcUa_SecureConnection_EndSendRequest(    a_pSecureConnection,
                                                                &pSecureOstrm,
                                                                0,
                                                                OpcUa_Null,
                                                                OpcUa_Null);
            if(OpcUa_IsBad(uStatus))
            {
                OPCUA_SECURECONNECTION_LOCK(pSecureConnection);
                OpcUa_GotoErrorIfBad(uStatus);
            }
            else if(!pSecureConnection->pSecureChannel->bAsyncWriteInProgress)
            {
                OPCUA_SECURECONNECTION_LOCK(pSecureConnection);

                pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

                OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_Disconnect: Disconnecting transport.\n");
                pSecureConnection->TransportConnection->Disconnect( pSecureConnection->TransportConnection,
                                                                    OpcUa_False); /* dont notify */

                /*** invoke callback function ***/
                if((pSecureConnection->Callback != OpcUa_Null) && (a_bNotifyOnComplete != OpcUa_False))
                {
                    /* inform the owner of this connection about the connection status */
                    pSecureConnection->Callback(    a_pSecureConnection,
                                                    pSecureConnection->CallbackData,
                                                    OpcUa_ConnectionEvent_Disconnect,
                                                    OpcUa_Null,
                                                    OpcUa_Good);
                }
                else
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_Disconnect: Callback is NULL!\n");
                }

                /* cancel all open requests */
                OpcUa_SecureConnection_CancelOpenRequests(  a_pSecureConnection,
                                                            OpcUa_BadDisconnect);
            }

            if(pCloseSecureChannelRequest != OpcUa_Null)
            {
                OpcUa_CloseSecureChannelRequest_Clear(pCloseSecureChannelRequest);
                OpcUa_Free(pCloseSecureChannelRequest);
                pCloseSecureChannelRequest = OpcUa_Null;
            }

            break;
        }
    case OpcUa_SecureConnectionState_ConnectingTransport:
    case OpcUa_SecureConnectionState_ConnectingSecure:
        /* just close transport */
        {
            pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

            OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

            if(hTimerToDelete != OpcUa_Null)
            {
                OpcUa_Timer_Delete(&hTimerToDelete);
            }

            pSecureConnection->TransportConnection->Disconnect( pSecureConnection->TransportConnection,
                                                                OpcUa_False); /* dont notify */

            /*** invoke callback function ***/
            if((pSecureConnection->Callback != OpcUa_Null) && (a_bNotifyOnComplete != OpcUa_False))
            {
                /* inform the owner of this connection about the connection status */
                pSecureConnection->Callback(    a_pSecureConnection,
                                                pSecureConnection->CallbackData,
                                                OpcUa_ConnectionEvent_Disconnect,
                                                OpcUa_Null,
                                                OpcUa_Good);
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_Disconnect: Callback is NULL!\n");
            }
            break;
        }
    default:
        {
            OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

            if(hTimerToDelete != OpcUa_Null)
            {
                OpcUa_Timer_Delete(&hTimerToDelete);
            }

            uStatus = OpcUa_BadInvalidState;

            /*** invoke callback function ***/
            if((pSecureConnection->Callback != OpcUa_Null) && (a_bNotifyOnComplete != OpcUa_False))
            {
                /* inform the owner of this connection about the connection status */
                pSecureConnection->Callback(    a_pSecureConnection,
                                                pSecureConnection->CallbackData,
                                                OpcUa_ConnectionEvent_Disconnect,
                                                OpcUa_Null,
                                                OpcUa_Good);
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_Disconnect: Callback is NULL!\n");
            }

            break;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* unlock the channel mutex */
    pSecureConnection->State = OpcUa_SecureConnectionState_Disconnected;

    OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

    if(hTimerToDelete != OpcUa_Null)
    {
        OpcUa_Timer_Delete(&hTimerToDelete);
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureConnection_Disconnect: Could NOT close securechannel remotely, disconnecting transport\n");

    pSecureConnection->TransportConnection->Disconnect( pSecureConnection->TransportConnection,
                                                        OpcUa_False); /* dont notify */

    /*** invoke callback function ***/
    if((pSecureConnection->Callback != OpcUa_Null) && (a_bNotifyOnComplete != OpcUa_False))
    {
        /* inform the owner of this connection about the connection status */
        pSecureConnection->Callback(    a_pSecureConnection,
                                        pSecureConnection->CallbackData,
                                        OpcUa_ConnectionEvent_Disconnect,
                                        OpcUa_Null,
                                        OpcUa_Good);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_Disconnect: Callback is NULL!\n");
    }

    /* cancel all open requests */
    OpcUa_SecureConnection_CancelOpenRequests(  a_pSecureConnection,
                                                OpcUa_BadDisconnect);

    if(pSecureOstrm != OpcUa_Null)
    {
        OpcUa_Stream_Delete(&((OpcUa_SecureStream*)pSecureOstrm->Handle)->InnerStrm);
        pSecureOstrm->Delete((OpcUa_Stream**)&pSecureOstrm);
    }

    if(pCloseSecureChannelRequest != OpcUa_Null)
    {
        OpcUa_CloseSecureChannelRequest_Clear(pCloseSecureChannelRequest);
        OpcUa_Free(pCloseSecureChannelRequest);
    }

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureConnection_EndSendRequest
 *===========================================================================*/
/** @brief A callback of null means no pending request is stored! */
OpcUa_StatusCode OpcUa_SecureConnection_EndSendRequest(
    OpcUa_Connection*               a_pConnection,
    OpcUa_OutputStream**            a_ppOstrm,
    OpcUa_UInt32                    a_uTimeout,
    OpcUa_Connection_PfnOnResponse* a_pCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_SecureConnection* pSecureConnection       = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream           = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest          = OpcUa_Null;
    OpcUa_Boolean           bIsLocked               = OpcUa_False;
    OpcUa_UInt32            uRequestId              = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "EndSendRequest");

    /* parameter checks */
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull((*a_ppOstrm)->Handle);
    pSecureStream = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureConnection, a_pConnection, EndSendRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection->Handle);
    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);
    bIsLocked = OpcUa_True;

    /*** check connection state ***/
    if(pSecureConnection->State != OpcUa_SecureConnectionState_Connected && pSecureConnection->State != OpcUa_SecureConnectionState_DisconnectingSecure)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /*** create SecureRequest ***/
    if(a_pCallback != OpcUa_Null)
    {
        uRequestId = pSecureStream->RequestId;
        uStatus = OpcUa_SecureConnection_AddPendingRequest( a_pConnection,
                                                            pSecureStream->RequestId,
                                                            a_uTimeout,
                                                            OpcUa_SecureRequestType_SessionCall,
                                                            a_pCallback,
                                                            a_pCallbackData,
                                                            &pSecureRequest);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureConnection_EndSendRequest: ID %u\n", pSecureStream->RequestId);

    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    bIsLocked = OpcUa_False;

    /*** close the secure stream and send last chunk ***/
    uStatus = (*a_ppOstrm)->Close((OpcUa_Stream*)(*a_ppOstrm));
    OpcUa_GotoErrorIfBad(uStatus);

    /*** finish request at the transport layer ***/
    uStatus = pSecureConnection->TransportConnection->EndSendRequest(   pSecureConnection->TransportConnection,
                                                                        (OpcUa_OutputStream**)&(pSecureStream->InnerStrm),
                                                                        a_uTimeout,
                                                                        OpcUa_Null,
                                                                        OpcUa_Null);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** clean up ***/
    (*a_ppOstrm)->Delete((OpcUa_Stream**)a_ppOstrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureConnection - EndSendRequest: ERROR WHILE SENDING REQUEST (0x%08X)\n", uStatus);

    if(pSecureRequest != OpcUa_Null)
    {
        OpcUa_SecureConnection_RemoveSecureRequestById( a_pConnection,
                                                        uRequestId,
                                                        &pSecureRequest);

        if(pSecureRequest != OpcUa_Null)
        {
            OpcUa_SecureRequest_Delete(&pSecureRequest);
        }
        else
        {
            /* callback will report the status */
            uStatus = OpcUa_Good;

            /* caller not responsible for this */
            OpcUa_Stream_Delete(&pSecureStream->InnerStrm);
            (*a_ppOstrm)->Delete((OpcUa_Stream**)a_ppOstrm);
        }
    }

    if(bIsLocked == OpcUa_True)
    {
        OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_BeginSendOpenSecureChannelRequest
 *===========================================================================*/
/*
    ToDo: Add automatic renewal functionality
*/
static OpcUa_StatusCode OpcUa_SecureConnection_BeginSendOpenSecureChannelRequest(
    OpcUa_Connection*           a_pConnection,
    OpcUa_String*               a_pSecurityPolicyName,
    OpcUa_MessageSecurityMode   a_MessageSecurityMode,
    OpcUa_CryptoProvider*       a_pCryptoProvider,
    OpcUa_ByteString*           a_pClientCertificate,
    OpcUa_ByteString*           a_pServerCertificateThumbprint,
    OpcUa_OutputStream**        a_ppOstrm)
{
    OpcUa_SecureConnection* pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;
    OpcUa_OutputStream*     pInnerOstrm = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "BeginSendOpenSecureChannelRequest");

    *a_ppOstrm = OpcUa_Null;

    /* acquire lock until operation is complete */
    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);

    /* create stream for the non-secure connection */
    uStatus = pSecureConnection->TransportConnection->BeginSendRequest( pSecureConnection->TransportConnection,
                                                                        &pInnerOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* create stream for writing */
    uStatus = OpcUa_SecureStream_CreateOpenSecureChannelOutput( pInnerOstrm,
                                                                pSecureConnection->pSecureChannel,
                                                                pSecureConnection->uRequestId++,
                                                                a_pSecurityPolicyName,
                                                                a_MessageSecurityMode,
                                                                a_pCryptoProvider,
                                                                a_pClientCertificate,
                                                                pSecureConnection->ClientPrivateKey,
                                                                pSecureConnection->ServerCertificate,
                                                                a_pServerCertificateThumbprint,
                                                                a_ppOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* release lock */
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* clean up */
    OpcUa_Stream_Delete((OpcUa_Stream**)&pInnerOstrm);

    /* release lock on failure */
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_EndSendOpenSecureChannelRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_EndSendOpenSecureChannelRequest(
    OpcUa_Connection*               a_pConnection,
    OpcUa_OutputStream**            a_ppOstrm,
    OpcUa_Void*                     a_pRequest,
    OpcUa_UInt32                    a_uTimeout,
    OpcUa_Connection_PfnOnResponse* a_pCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;
    OpcUa_SecureRequest*    pSecureRequest      = OpcUa_Null;
    OpcUa_Boolean           bIsLocked           = OpcUa_False;
    OpcUa_UInt32            uRequestId          = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "EndSendOpenSecureChannelRequest");

    /* this is not used; request is set as callback data instead */
    OpcUa_ReferenceParameter(a_pCallbackData);

    /*** get SecureConnection handle ***/
    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;
    pSecureStream = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;

    /*** acquire lock until operation is complete ***/
    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);
    bIsLocked = OpcUa_True;

    /*** create and add the request SecureRequest ***/
    uRequestId = pSecureStream->RequestId;
    uStatus = OpcUa_SecureConnection_AddPendingRequest( a_pConnection,
                                                        pSecureStream->RequestId,
                                                        a_uTimeout,
                                                        OpcUa_SecureRequestType_SecureChannelOpen,
                                                        a_pCallback,
                                                        a_pRequest,
                                                        &pSecureRequest);
    OpcUa_GotoErrorIfBad(uStatus);

    /* not set by the preceding function */
    pSecureRequest->MessageSecurityMode = pSecureStream->eMessageSecurityMode;

    /*** release lock ***/
    OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    bIsLocked = OpcUa_False;

    /*** close the secure stream ***/
    uStatus = (*a_ppOstrm)->Close((OpcUa_Stream*)(*a_ppOstrm));
    OpcUa_GotoErrorIfBad(uStatus);

    /*** send request over the wire ***/
    uStatus = pSecureConnection->TransportConnection->EndSendRequest(   pSecureConnection->TransportConnection,
                                                                        (OpcUa_OutputStream**)&(pSecureStream->InnerStrm),
                                                                        a_uTimeout,
                                                                        OpcUa_Null,
                                                                        OpcUa_Null);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** clean up ***/
    (*a_ppOstrm)->Delete((OpcUa_Stream**)a_ppOstrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* abort request at the transport level */
    OpcUa_Connection_AbortSendRequest(  pSecureConnection->TransportConnection,
                                        uStatus,
                                        OpcUa_Null,
                                        (OpcUa_OutputStream**)&(pSecureStream->InnerStrm));

    if(pSecureRequest != OpcUa_Null)
    {
        OpcUa_SecureConnection_RemoveSecureRequestById( a_pConnection,
                                                        uRequestId,
                                                        &pSecureRequest);
        if(pSecureRequest != OpcUa_Null)
        {
            OpcUa_SecureRequest_Delete(&pSecureRequest);
        }
        else
        {
            /* the callback has already been called */
            uStatus = OpcUa_Good;
        }
    }

    if((*a_ppOstrm != OpcUa_Null))
    {
        (*a_ppOstrm)->Delete((OpcUa_Stream**)a_ppOstrm);
    }

    /* release lock on failure */
    if(bIsLocked == OpcUa_True)
    {
        OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_AbortSendRequest
 *===========================================================================*/
/* this includes cleaning the buffer content as far as no longer required
    (header) and preencoding the abort message header plus the additional
    fields error (status code) and reason (string); afterwards, security
    is applied and the stream is closed. */
OpcUa_StatusCode OpcUa_SecureConnection_AbortSendRequest(
    OpcUa_Connection*    a_pConnection,
    OpcUa_StatusCode     a_uStatus,
    OpcUa_String*        a_psReason,
    OpcUa_OutputStream** a_ppOstrm)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;
    OpcUa_String            sReason             = OPCUA_STRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "AbortSendRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOstrm);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureConnection, a_pConnection, AbortSendRequest);

    pSecureConnection   = (OpcUa_SecureConnection*)a_pConnection->Handle;
    pSecureStream       = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pSecureStream);

    OPCUA_SECURECONNECTION_LOCK_REQUEST(pSecureConnection);

    /* check if abort message needs to be sent. */
    if(!pSecureStream->IsClosed && pSecureStream->uNoOfFlushes != 0)
    {
        OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_AbortSendRequest: called for used stream! Triggering Abort Message!\n");

        OpcUa_String_AttachReadOnly(&sReason, "Error");

        /* mark as abort type message */
        pSecureStream->Buffers[0].Data[3] = 'A';
        pSecureStream->eMessageType = eOpcUa_SecureStream_Types_AbortMessage;

        /* set internal pointer right after header elements */
        OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], pSecureStream->uBeginOfRequestBody);
        pSecureStream->Buffers[0].EndOfData = pSecureStream->uBeginOfRequestBody;
        uStatus = OpcUa_UInt32_BinaryEncode(a_uStatus, (*a_ppOstrm));
        uStatus = OpcUa_String_BinaryEncode(a_psReason?a_psReason:&sReason, (*a_ppOstrm));
        uStatus =(*a_ppOstrm)->Close((OpcUa_Stream*)(*a_ppOstrm));
        if(OpcUa_IsBad(uStatus))
        {
            /* if abort message could not be sent, the message pipeline is corrupted TODO */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_AbortSendRequest: Could not send abort message!\n");
        }
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureConnection_AbortSendRequest: called for unused stream! Just cleaning up.\n");
        OPCUA_SECURECONNECTION_UNLOCK_REQUEST(pSecureConnection);
    }

    /* delete transport stream */
    OpcUa_Stream_Delete(&pSecureStream->InnerStrm);
    OpcUa_Stream_Delete((OpcUa_Stream**)a_ppOstrm);

    pSecureConnection->TransportConnection->AbortSendRequest(   pSecureConnection->TransportConnection,
                                                                a_uStatus,
                                                                a_psReason?a_psReason:&sReason,
                                                                OpcUa_Null); /* this should be allowed */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_SecureConnection_Delete(OpcUa_Connection** a_ppConnection)
{
    OpcUa_SecureConnection* pSecureConnection   = OpcUa_Null;

    if(a_ppConnection == OpcUa_Null)
    {
        return;
    }

    pSecureConnection = (OpcUa_SecureConnection*)(*a_ppConnection)->Handle;

    if(pSecureConnection != OpcUa_Null)
    {
        /* invalidate the memory */
        pSecureConnection->SanityCheck = 0;

        /* set during connect phase - also reset during disconnect */
        OpcUa_Free(pSecureConnection->ClientPrivateKey);
        pSecureConnection->ClientPrivateKey = OpcUa_Null;
        OpcUa_String_Clear(&pSecureConnection->sUrl);

        /* DELETE PKI Provider */
        OPCUA_P_PKIFACTORY_DELETEPKIPROVIDER(pSecureConnection->ClientPKIProvider);
        OpcUa_Free(pSecureConnection->ClientPKIProvider);
        pSecureConnection->ClientPKIProvider = OpcUa_Null;
        OpcUa_String_Delete(&pSecureConnection->pRequestedSecurityPolicyUri);

        /* delete securechannel */
        OpcUa_TcpSecureChannel_Delete(&pSecureConnection->pSecureChannel);

        /* delete the timer for the watchdog */
        if(pSecureConnection->hWatchdogTimer != OpcUa_Null)
        {
            OpcUa_Timer_Delete(&(pSecureConnection->hWatchdogTimer));
        }

        /* delete the reconnect timer if required */
        if(pSecureConnection->hRenewTimer != OpcUa_Null)
        {
            OpcUa_Timer_Delete(&(pSecureConnection->hRenewTimer));
        }

        /* the connection is closed and should not accept anymore requests from the upper layers! */

        OpcUa_SecureConnection_CancelOpenRequests(  *a_ppConnection,
                                                    OpcUa_BadOperationAbandoned);

        /* there should be no open requests anymore */
        if(pSecureConnection->PendingRequests != OpcUa_Null)
        {
            OpcUa_List_Delete(&(pSecureConnection->PendingRequests));
        }

        /* delete mutex */
        OPCUA_P_MUTEX_DELETE(&pSecureConnection->RequestMutex);

        OpcUa_Free(pSecureConnection);
        pSecureConnection = OpcUa_Null;
    }

    OpcUa_Free(*a_ppConnection);
    *a_ppConnection = OpcUa_Null;
}

/*============================================================================
 * OpcUa_SecureConnection_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_Create(
    OpcUa_Connection*               a_pInnerConnection,
    OpcUa_Encoder*                  a_pEncoder,
    OpcUa_Decoder*                  a_pDecoder,
    OpcUa_StringTable*              a_pNamespaceUris,
    OpcUa_EncodeableTypeTable*      a_pKnownTypes,
    OpcUa_Connection**              a_ppConnection)
{
    OpcUa_Connection*       pConnection         = OpcUa_Null;
    OpcUa_SecureConnection* pSecureConnection    = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_pInnerConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pEncoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pNamespaceUris);
    OpcUa_ReturnErrorIfArgumentNull(a_pKnownTypes);

    /* allocate connection object */
    pConnection = (OpcUa_Connection*)OpcUa_Alloc(sizeof(OpcUa_Connection));
    OpcUa_GotoErrorIfAllocFailed(pConnection);
    OpcUa_MemSet(pConnection, 0, sizeof(OpcUa_Connection));

    /* allocate connection handle */
    pSecureConnection = (OpcUa_SecureConnection*)OpcUa_Alloc(sizeof(OpcUa_SecureConnection));
    OpcUa_GotoErrorIfAllocFailed(pSecureConnection);
    OpcUa_MemSet(pSecureConnection, 0, sizeof(OpcUa_SecureConnection));

    /* initialize connection handle */
    pSecureConnection->SanityCheck          = OpcUa_SecureConnection_SanityCheck;
    pSecureConnection->TransportConnection  = a_pInnerConnection;
    pSecureConnection->State                = OpcUa_SecureConnectionState_Disconnected;
    pSecureConnection->Encoder              = a_pEncoder;
    pSecureConnection->Decoder              = a_pDecoder;
    pSecureConnection->NamespaceUris        = a_pNamespaceUris;
    pSecureConnection->KnownTypes           = a_pKnownTypes;
    pSecureConnection->uRequestId           = 1; /* we start with 1 since zero is error prone */

    /* initialize connection object */
    pConnection->Handle           = pSecureConnection;
    pConnection->Connect          = OpcUa_SecureConnection_Connect;
    pConnection->Disconnect       = OpcUa_SecureConnection_Disconnect;
    pConnection->BeginSendRequest = OpcUa_SecureConnection_BeginSendRequest;
    pConnection->EndSendRequest   = OpcUa_SecureConnection_EndSendRequest;
    pConnection->AbortSendRequest = OpcUa_SecureConnection_AbortSendRequest;
    pConnection->Delete           = OpcUa_SecureConnection_Delete;

    /* create mutex */
    uStatus = OPCUA_P_MUTEX_CREATE(&pSecureConnection->RequestMutex);
    OpcUa_GotoErrorIfBad(uStatus);

    *a_ppConnection = pConnection;

    /* create list for pending requests */
    uStatus = OpcUa_List_Create(&(pSecureConnection->PendingRequests));
    OpcUa_ReturnErrorIfBad(uStatus);

    /* create watchdog timer for outstanding responses. */
    uStatus = OpcUa_Timer_Create(   &(pSecureConnection->hWatchdogTimer),
                                    OPCUA_SECURECONNECTION_TIMEOUTINTERVAL,
                                    OpcUa_SecureConnection_WatchdogTimerCallback,
                                    OpcUa_SecureConnection_WatchdogTimerKillCallback,
                                    (OpcUa_Void*)(*a_ppConnection));
    OpcUa_ReturnErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureConnection != OpcUa_Null)
    {
        OPCUA_P_MUTEX_DELETE(&(pSecureConnection->RequestMutex));

        OpcUa_Free(pSecureConnection);
        pSecureConnection = OpcUa_Null;
    }

    OpcUa_Free(pConnection);
    pConnection = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_GetChannelId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_GetChannelId(
    OpcUa_Connection*       a_pConnection,
    OpcUa_UInt32*           a_pSecureChannelId)
{
    OpcUa_SecureConnection* pSecureConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "GetChannelId");

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureConnection - GetChannelId\n");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannelId);

    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    if(pSecureConnection->pSecureChannel == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    OPCUA_SECURECONNECTION_LOCK(pSecureConnection);

    *a_pSecureChannelId = pSecureConnection->pSecureChannel->SecureChannelId;

    OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    pSecureConnection = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_GetCurrentTokenId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_GetCurrentTokenId(
    OpcUa_Connection*       a_pConnection,
    OpcUa_UInt32*           a_pCurrentTokenId)
{
    OpcUa_SecureConnection* pSecureConnection = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "GetCurrentTokenId");

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureConnection - GetCurrentTokenId\n");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pCurrentTokenId);

    pSecureConnection = (OpcUa_SecureConnection*)a_pConnection->Handle;

    OPCUA_SECURECONNECTION_LOCK(pSecureConnection);

    *a_pCurrentTokenId = pSecureConnection->pSecureChannel->CurrentChannelSecurityToken.TokenId;

    OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    pSecureConnection = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_ReadResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_ReadResponse(
    OpcUa_SecureConnection*         a_pSecureConnection,
    OpcUa_InputStream*              a_pIstrm,
    OpcUa_ExtensionObject**         a_ppResponse,
    OpcUa_Boolean*                  a_pIsFault)
{
    OpcUa_Decoder*          pDecoder        = OpcUa_Null;
    OpcUa_EncodeableType*   pResponseType   = OpcUa_Null;
    OpcUa_Handle            hDecodeContext  = OpcUa_Null;
    OpcUa_MessageContext    cContext;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "ReadResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    *a_ppResponse = OpcUa_Null;
    *a_pIsFault = OpcUa_False;

    pDecoder = a_pSecureConnection->Decoder;

    OpcUa_MessageContext_Initialize(&cContext);

    cContext.KnownTypes         = a_pSecureConnection->KnownTypes;
    cContext.NamespaceUris      = a_pSecureConnection->NamespaceUris;
    cContext.AlwaysCheckLengths = OPCUA_SERIALIZER_CHECKLENGTHS;

    /* open decoder */
    uStatus = pDecoder->Open(pDecoder, a_pIstrm, &cContext, &hDecodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* decode message */
    uStatus = pDecoder->ReadMessage((struct _OpcUa_Decoder*)hDecodeContext, &pResponseType, (OpcUa_Void**)a_ppResponse);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check for fault */
    if(pResponseType->TypeId == OpcUaId_ServiceFault)
    {
        *a_pIsFault = OpcUa_True;
    }
    else if(pResponseType->TypeId != OpcUaId_OpenSecureChannelResponse)
    {
        OpcUa_EncodeableObject_Delete(pResponseType, (OpcUa_Void**)a_ppResponse);
        OpcUa_GotoErrorWithStatus(OpcUa_BadUnknownResponse);
    }

    /* close decoder */
    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_ProcessOpenSecureChannelResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_ProcessOpenSecureChannelResponse(
    OpcUa_Connection*                   a_pConnection,
    OpcUa_InputStream*                  a_pTransportIstrm,
    OpcUa_StatusCode                    a_uResponseStatus,
    OpcUa_Boolean                       a_bResponseComplete)
{
    OpcUa_OpenSecureChannelRequest*     pRequest                = OpcUa_Null;
    OpcUa_OpenSecureChannelResponse*    pResponse               = OpcUa_Null;
    OpcUa_SecureConnection*             pSecureConnection       = OpcUa_Null;

    OpcUa_InputStream*                  pSecureIstrm            = OpcUa_Null;
    OpcUa_SecureStream*                 pSecureStream           = OpcUa_Null;
    OpcUa_SecureChannel*                pSecureChannel          = OpcUa_Null;
    OpcUa_CryptoProvider*               pCryptoProvider         = OpcUa_Null;

    OpcUa_SecurityKeyset*               pReceivingKeyset        = OpcUa_Null;
    OpcUa_SecurityKeyset*               pSendingKeyset          = OpcUa_Null;

    OpcUa_UInt32                        uSecureChannelId        = 0;

    OpcUa_ByteString                    SenderCertificate       = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_ByteString                    recvCertThumbprint      = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_ByteString                    clientCertThumbprint    = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_String                        sSecurityPolicyUri      = OPCUA_STRING_STATICINITIALIZER;
    OpcUa_SecureRequest*                pSecureRequest          = OpcUa_Null;

#if OPCUA_SECURECONNECTION_VALIDATE_SERVERCERT
    OpcUa_Void*                         pCertificateStore       = OpcUa_Null;
    OpcUa_Int                           validationCode          = -1;
#endif /* OPCUA_SECURECONNECTION_VALIDATE_SERVERCERT */
    OpcUa_Boolean                       bIsFault                = OpcUa_False;

    OpcUa_MessageSecurityMode           eRevisedSecurityMode    = OpcUa_MessageSecurityMode_Invalid;

    OpcUa_Key*                          pSigningKey             = OpcUa_Null;
    OpcUa_Key*                          pEncryptionKey          = OpcUa_Null;
    OpcUa_Key*                          pInitializationVector   = OpcUa_Null;

    OpcUa_Boolean                       bSecureChannelLocked    = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "ProcessOpenSecureChannelResponse");

    pSecureConnection   = (OpcUa_SecureConnection*)(a_pConnection->Handle);
    pSecureChannel      = pSecureConnection->pSecureChannel;
    OpcUa_ReturnErrorIfArgumentNull(pSecureChannel);
    pCryptoProvider     = pSecureChannel->pCurrentCryptoProvider;

    if(a_pTransportIstrm != OpcUa_Null && OpcUa_IsGood(a_uResponseStatus))
    {
        OPCUA_SECURECONNECTION_LOCK(pSecureConnection);
        bSecureChannelLocked = OpcUa_True;

        if(pSecureConnection->State != OpcUa_SecureConnectionState_Connected &&
           pSecureConnection->State != OpcUa_SecureConnectionState_ConnectingSecure)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelResponse: Invalid State!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
        }

        /* get basic header information about the request */
        uStatus = OpcUa_SecureStream_DecodeAsymmetricSecurityHeader(a_pTransportIstrm,
                                                                    &uSecureChannelId,
                                                                    &sSecurityPolicyUri,
                                                                    &SenderCertificate,
                                                                    &recvCertThumbprint);
        OpcUa_GotoErrorIfBad(uStatus);

        OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                        "ProcessOpenSecureChannelResponse: SID %u, SURI \"%*.*s\"\n",
                        uSecureChannelId,
                        OpcUa_String_StrLen(&sSecurityPolicyUri),
                        OpcUa_String_StrLen(&sSecurityPolicyUri),
                        OpcUa_String_GetRawString(&sSecurityPolicyUri));

        /* must check that the server certificate is not changed. */
        if(pSecureConnection->MessageSecurityMode != OpcUa_MessageSecurityMode_None)
        {
            if(pSecureConnection->ServerCertificate != OpcUa_Null && pSecureConnection->ServerCertificate->Length > 0)
            {
                if(SenderCertificate.Length != pSecureConnection->ServerCertificate->Length)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid);
                }

                if(OpcUa_MemCmp(SenderCertificate.Data, pSecureConnection->ServerCertificate->Data, SenderCertificate.Length) != 0)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid);
                }
            }
            else
            {
                if(SenderCertificate.Length > 0)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid);
                }
            }
        }

        /* if security is turned on then the sender and receiver certificates must be specified. */
        if((SenderCertificate.Data != OpcUa_Null) && (recvCertThumbprint.Data != OpcUa_Null))
        {
            eRevisedSecurityMode = OpcUa_MessageSecurityMode_SignAndEncrypt;
        }

        /* if security is turned off then the sender and receiver certificates must be null. */
        else if((SenderCertificate.Data == OpcUa_Null) && (recvCertThumbprint.Data == OpcUa_Null))
        {
            eRevisedSecurityMode = OpcUa_MessageSecurityMode_None;
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelResponse: Invalid sender and/or receiver certificates specified!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid);
        }

        /* check that the message has been properly secured */
        if(pSecureConnection->MessageSecurityMode != OpcUa_MessageSecurityMode_None)
        {
            if(eRevisedSecurityMode != OpcUa_MessageSecurityMode_SignAndEncrypt)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityChecksFailed);
            }
        }

        /* check that the securechannel id has not changed */
        if(pSecureConnection->bRenewActive && pSecureChannel->SecureChannelId != uSecureChannelId)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
        }

        /* check certificate in secure modes */
#if OPCUA_SECURECONNECTION_VALIDATE_SERVERCERT
        if(eRevisedSecurityMode == OpcUa_MessageSecurityMode_SignAndEncrypt)
        {
            /* open certificate store */
            uStatus = ((OpcUa_PKIProvider*)pSecureConnection->ClientPKIProvider)->OpenCertificateStore( pSecureConnection->ClientPKIProvider,
                                                                                                        &pCertificateStore);

            if(OpcUa_IsBad(uStatus))
            {
                if(OpcUa_IsEqual(OpcUa_BadNotSupported))
                {
                    uStatus = OpcUa_Good;
                }
                else
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "ProcessOpenSecureChannelResponse: Could not open certificate store!\n");

                    if(OpcUa_IsEqual(OpcUa_Bad))
                    {
                        /* map unspecified error */
                        OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityConfig);
                    }
                    else
                    {
                        OpcUa_GotoError;
                    }
                }
            }

            /* validate certificate */
            uStatus = ((OpcUa_PKIProvider*)pSecureConnection->ClientPKIProvider)->ValidateCertificate(  pSecureConnection->ClientPKIProvider,
                                                                                                        &SenderCertificate,
                                                                                                        pCertificateStore,
                                                                                                        &validationCode);
            if(uStatus == OpcUa_BadNotSupported)
            {
                uStatus = OpcUa_Good;
            }
            else if(OpcUa_IsBad(uStatus))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelResponse: Could not validate server certificate! (0x%08X)\n", uStatus);
                ((OpcUa_PKIProvider*)pSecureConnection->ClientPKIProvider)->CloseCertificateStore(  pSecureConnection->ClientPKIProvider,
                                                                                                    &pCertificateStore);
                OpcUa_GotoError;
            }

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "ProcessOpenSecureChannelResponse: Server certificate validated! (0x%08X)\n", uStatus);

            /* close certificate store */
            uStatus = ((OpcUa_PKIProvider*)pSecureConnection->ClientPKIProvider)->CloseCertificateStore(pSecureConnection->ClientPKIProvider,
                                                                                                        &pCertificateStore);

            pCertificateStore = OpcUa_Null;
        }
#endif /* OPCUA_SECURECONNECTION_VALIDATE_SERVERCERT */

        /* look if there is a pending stream */
        uStatus = OpcUa_SecureChannel_GetPendingInputStream(    pSecureChannel,
                                                                &pSecureIstrm);
        OpcUa_GotoErrorIfBad(uStatus);

        if(pSecureIstrm == OpcUa_Null)
        {
            /*** create inputsstream ***/
            uStatus = OpcUa_SecureStream_CreateOpenSecureChannelInput(  pSecureChannel->pCurrentCryptoProvider,
                                                                        eRevisedSecurityMode,
                                                                        pSecureConnection->ClientCertificate,
                                                                        pSecureConnection->ClientPrivateKey,
                                                                        &SenderCertificate,
                                                                        &recvCertThumbprint,
                                                                        pSecureConnection->nMaxBuffersPerMessage,
                                                                        &pSecureIstrm);
            OpcUa_GotoErrorIfBad(uStatus);

            pSecureStream  = (OpcUa_SecureStream*)pSecureIstrm->Handle;
            pSigningKey    = pSecureStream->pSenderPublicKey;
            pEncryptionKey = pSecureConnection->ClientPrivateKey;

            uStatus = OpcUa_SecureStream_AppendInput(   a_pTransportIstrm,
                                                        pSecureIstrm,
                                                        pSigningKey,
                                                        pEncryptionKey,
                                                        pInitializationVector,
                                                        pCryptoProvider,
                                                        pSecureChannel);
            OpcUa_GotoErrorIfBad(uStatus);

            pSecureStream = (OpcUa_SecureStream*)pSecureIstrm->Handle;
            pSecureStream->SecureChannelId      = uSecureChannelId;
            pSecureStream->eMessageType         = eOpcUa_SecureStream_Types_OpenSecureChannel;
        }
        else
        {
            /* Reset the pending request stream. */
            uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                    OpcUa_Null);

            OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
        }

        /* look if we can start processing the stream */
        if(a_bResponseComplete == OpcUa_False)
        {
            /* Reset the pending request stream. */
            uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                    OpcUa_Null);

            OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
        }
        else /* preprocess the stream */
        {
            /* Reset the pending request stream. */
            uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                    OpcUa_Null);

            /*** get SecureRequest from list of pending SecureRequests ***/
            uStatus = OpcUa_SecureConnection_RemoveSecureRequestById(   a_pConnection,
                                                                        pSecureStream->RequestId,
                                                                        &pSecureRequest);
            OpcUa_GotoErrorIfBad(uStatus);

            if(pSecureRequest == OpcUa_Null)
            {
                /* no matching SecureRequest was found */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "ProcessOpenSecureChannelResponse: Unknown response with id %u\n", pSecureStream->RequestId);
                OpcUa_GotoErrorWithStatus(OpcUa_BadUnknownResponse);
            }

            pRequest = (OpcUa_OpenSecureChannelRequest*)pSecureRequest->CallbackData;
            OpcUa_GotoErrorIfArgumentNull(pRequest);

            /* reset buffer index to start reading from the first buffer */
            pSecureStream->nCurrentReadBuffer = 0;

            /*** read response ***/
            uStatus = OpcUa_SecureConnection_ReadResponse(  pSecureConnection,
                                                            pSecureIstrm,
                                                            (OpcUa_ExtensionObject**)&pResponse,
                                                            &bIsFault);

            OpcUa_GotoErrorIfBad(uStatus);

            /*** check for a fault body ***/
            if(bIsFault != OpcUa_False)
            {
                OpcUa_ServiceFault* pFault = (OpcUa_ServiceFault*)pResponse;
                uStatus = pFault->ResponseHeader.ServiceResult;

                OpcUa_ServiceFault_Clear(pFault);
                OpcUa_Free(pFault);

                pResponse = OpcUa_Null;

                if(!OpcUa_IsBad(uStatus))
                {
                    uStatus = OpcUa_BadUnexpectedError;
                }

                OpcUa_GotoErrorIfBad(uStatus);
            }

            uStatus = OpcUa_Connection_CheckProtocolVersion(pSecureConnection->TransportConnection,
                                                            pResponse->ServerProtocolVersion);
            OpcUa_GotoErrorIfBad(uStatus);

            /* derive the keys from the nonces */
            uStatus = OpcUa_SecureChannel_DeriveKeys(   pRequest->SecurityMode,
                                                        pSecureChannel->pCurrentCryptoProvider,
                                                        &pRequest->ClientNonce,
                                                        &pResponse->ServerNonce,
                                                        &pSendingKeyset,
                                                        &pReceivingKeyset);
            OpcUa_GotoErrorIfBad(uStatus);

            /*** renew or create securechannel ***/
            if(pSecureConnection->bRenewActive)
            {
                /* existing securechannel */
                pSecureConnection->bRenewActive = OpcUa_False;
                uStatus = pSecureChannel->Renew(pSecureChannel,
                                                (OpcUa_Handle)a_pConnection,
                                                pResponse->SecurityToken,
                                                pRequest->SecurityMode,
                                                pSecureConnection->ClientCertificate,
                                                &SenderCertificate,
                                                pReceivingKeyset,
                                                pSendingKeyset,
                                                pCryptoProvider);

                /* immediately activate the new keyset */
                pSecureChannel->bCurrentTokenActive = OpcUa_True;
            }
            else
            {
                /* new securechannel */
                pSecureChannel->SecureChannelId = uSecureChannelId;
                uStatus = pSecureChannel->Open( pSecureChannel,
                                                (OpcUa_Handle)a_pConnection,
                                                pResponse->SecurityToken,
                                                pRequest->SecurityMode,
                                                pSecureConnection->ClientCertificate,
                                                &SenderCertificate,
                                                pReceivingKeyset,
                                                pSendingKeyset,
                                                pCryptoProvider);
            }

            OpcUa_GotoErrorIfBad(uStatus);

            pSecureConnection->State = OpcUa_SecureConnectionState_Connected;

            if(pSecureConnection->hRenewTimer == OpcUa_Null)
            {
                uStatus = OpcUa_Timer_Create(   &pSecureConnection->hRenewTimer,
                                                (OpcUa_UInt32)(pResponse->SecurityToken.RevisedLifetime * 0.75),
                                                OpcUa_SecureConnection_RenewTimerCallback,
                                                OpcUa_SecureConnection_RenewTimerKillCallback,
                                                (OpcUa_Void*)a_pConnection);
            }

            /*** invoke callback function ***/
            if(pSecureConnection->Callback != OpcUa_Null)
            {
                /* inform the owner of this connection about the connection status */
                pSecureConnection->Callback((OpcUa_Connection*)a_pConnection,
                                            pSecureConnection->CallbackData,
                                            OpcUa_ConnectionEvent_Connect,
                                            OpcUa_Null,
                                            OpcUa_Good);
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "INFO: Callback is NULL!\n");
            }



            OpcUa_ByteString_Clear(&SenderCertificate);
            OpcUa_ByteString_Clear(&recvCertThumbprint);
        }

        bSecureChannelLocked = OpcUa_False;
        OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);
    }
    else /* if(a_pTransportIstrm != OpcUa_Null && OpcUa_IsGood(a_uResponseStatus)) */
    {
        /* error happened */

        /*** get SecureRequest from list of pending SecureRequests ***/
        OpcUa_SecureConnection_RemoveSecureRequestByType(   a_pConnection,
                                                            OpcUa_SecureRequestType_SecureChannelOpen,
                                                            &pSecureRequest);
        if(pSecureRequest != OpcUa_Null)
        {
            pRequest = (OpcUa_OpenSecureChannelRequest*)pSecureRequest->CallbackData;
        }

        /*** invoke callback function ***/
        if(pSecureConnection->Callback != OpcUa_Null)
        {
            /* inform the owner of this connection about the connection status */
            pSecureConnection->Callback((OpcUa_Connection*)a_pConnection,
                                        pSecureConnection->CallbackData,
                                        OpcUa_ConnectionEvent_Connect,
                                        OpcUa_Null,
                                        a_uResponseStatus);
        }
    }

    /*** clean up ***/
    if(pResponse != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelResponse_Clear(pResponse);
        OpcUa_Free(pResponse);
    }

    if(pRequest != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pSecureIstrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIstrm);
    }

    if(pSecureRequest != OpcUa_Null)
    {
        OpcUa_SecureRequest_Delete(&pSecureRequest);
    }

    OpcUa_String_Clear(&sSecurityPolicyUri);
    OpcUa_ByteString_Clear(&clientCertThumbprint);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(bSecureChannelLocked != OpcUa_False)
    {
        OPCUA_SECURECONNECTION_UNLOCK(pSecureConnection);
    }

    /*** get SecureRequest from list of pending SecureRequests ***/
    if(pSecureRequest == OpcUa_Null)
    {
        OpcUa_SecureConnection_RemoveSecureRequestByType(   a_pConnection,
                                                            OpcUa_SecureRequestType_SecureChannelOpen,
                                                            &pSecureRequest);
    }

    if(pSecureRequest != OpcUa_Null)
    {
        pRequest = (OpcUa_OpenSecureChannelRequest*)pSecureRequest->CallbackData;
    }

    /*** invoke callback function ***/
    if(pSecureConnection->Callback != OpcUa_Null)
    {
        /* inform the owner of this connection about the connection status */
        pSecureConnection->Callback((OpcUa_Connection*)a_pConnection,
                                    pSecureConnection->CallbackData,
                                    OpcUa_ConnectionEvent_Connect,
                                    OpcUa_Null,
                                    uStatus);
    }

    OpcUa_ByteString_Clear(&SenderCertificate);
    OpcUa_ByteString_Clear(&recvCertThumbprint);

    if(pResponse != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelResponse_Clear(pResponse);
        OpcUa_Free(pResponse);
    }

    if(pRequest != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pSecureIstrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIstrm);
    }

    if(pSecureRequest != OpcUa_Null)
    {
        OpcUa_SecureRequest_Delete(&pSecureRequest);
    }

    OpcUa_String_Clear(&sSecurityPolicyUri);
    OpcUa_ByteString_Clear(&clientCertThumbprint);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureConnection_ProcessSessionCallResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureConnection_ProcessSessionCallResponse(
    OpcUa_Connection*                   a_pConnection,
    OpcUa_InputStream*                  a_pTransportIstrm,
    OpcUa_StatusCode                    a_responseStatus,
    OpcUa_Boolean                       a_bResponseComplete)
{
    OpcUa_SecureConnection* pSecureConnection       = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel          = OpcUa_Null;
    OpcUa_CryptoProvider*   pCryptoProvider         = OpcUa_Null;
    OpcUa_InputStream*      pSecureIstrm            = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream           = OpcUa_Null;

    OpcUa_UInt32            uSecureChannelId        = 0;
    OpcUa_UInt32            uTokenId                = 0;

    OpcUa_SecureRequest*    pSecureRequest          = OpcUa_Null;

    OpcUa_SecurityKeyset*   pReceivingKeyset        = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureConnection, "ProcessSessionCallResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pTransportIstrm);

    pSecureConnection   = (OpcUa_SecureConnection*)(a_pConnection)->Handle;
    pSecureChannel      = pSecureConnection->pSecureChannel;
    OpcUa_ReturnErrorIfArgumentNull(pSecureChannel);

    uStatus = OpcUa_SecureStream_DecodeSymmetricSecurityHeader( a_pTransportIstrm,
                                                                &uSecureChannelId,
                                                                &uTokenId);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                    "ProcessSessionCallResponse: Received chunk with SID %u, TID %u\n",
                    uSecureChannelId,
                    uTokenId);

    /* check that the securechannel id has not changed */
    if(pSecureChannel->SecureChannelId != uSecureChannelId)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(pSecureChannel,
                                                        &pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pSecureIstrm == OpcUa_Null)
    {
        /* Get reference to keyset for requested token id */
        uStatus = pSecureChannel->GetSecuritySet(   pSecureChannel,
                                                    uTokenId,
                                                    &pReceivingKeyset,
                                                    OpcUa_Null,
                                                    &pCryptoProvider);
        OpcUa_GotoErrorIfBad(uStatus);

        /*** create inputsstream ***/
        uStatus = OpcUa_SecureStream_CreateInput(   pCryptoProvider,
                                                    pSecureChannel->MessageSecurityMode,
                                                    pSecureConnection->nMaxBuffersPerMessage,
                                                    &pSecureIstrm);
        if(OpcUa_IsBad(uStatus))
        {
            pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                                  uTokenId);
            OpcUa_GotoError;
        }

        uStatus = OpcUa_SecureStream_AppendInput(   a_pTransportIstrm,
                                                    pSecureIstrm,
                                                    &pReceivingKeyset->SigningKey,
                                                    &pReceivingKeyset->EncryptionKey,
                                                    &pReceivingKeyset->InitializationVector,
                                                    pCryptoProvider,
                                                    pSecureChannel);
        /* release reference to security set */
        pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                              uTokenId);
        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_ProcessSessionCallResponse: Could not append chunk! (0x%08X)", uStatus);

            OpcUa_GotoError;
        }

        pSecureStream                       = (OpcUa_SecureStream*)pSecureIstrm->Handle;
        pSecureStream->SecureChannelId      = uSecureChannelId;
        pSecureStream->eMessageType         = eOpcUa_SecureStream_Types_StandardMessage;
    }
    else
    {
        /* Get reference to keyset for requested token id */
        uStatus = pSecureChannel->GetSecuritySet(   pSecureChannel,
                                                    uTokenId,
                                                    &pReceivingKeyset,
                                                    OpcUa_Null,
                                                    &pCryptoProvider);
        OpcUa_GotoErrorIfBad(uStatus);

        /* append keep the secure stream pending and waiting for further chunks */
        uStatus = OpcUa_SecureStream_AppendInput(   a_pTransportIstrm,
                                                    pSecureIstrm,
                                                    &pReceivingKeyset->SigningKey,
                                                    &pReceivingKeyset->EncryptionKey,
                                                    &pReceivingKeyset->InitializationVector,
                                                    pCryptoProvider,
                                                    pSecureChannel);
        /* release reference to security set */
        pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                              uTokenId);

        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureConnection_ProcessSessionCallResponse: Could not append chunk! (0x%08X)", uStatus);

            OpcUa_GotoError;
        }

        /* get shortcut to handle */
        pSecureStream = (OpcUa_SecureStream*)pSecureIstrm->Handle;
    }

    /* look if we can start processing the stream */
    if(a_bResponseComplete == OpcUa_False)
    {
        /* Set the current stream as the pending request stream and leave. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                pSecureIstrm);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ProcessSessionCallResponse: Waiting for more chunks!\n");
    }
    else /* preprocess the stream */
    {
        /* Clear the pending input stream marker */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                OpcUa_Null);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ProcessSessionCallResponse: All %u chunks received; start processing!\n", pSecureStream->nBuffers);

        /*** get SecureRequest from list of pending SecureRequests ***/
        uStatus = OpcUa_SecureConnection_RemoveSecureRequestById(   a_pConnection,
                                                                    pSecureStream->RequestId,
                                                                    &pSecureRequest);
        if(pSecureRequest == OpcUa_Null)
        {
            /* no matching SecureRequest was found */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ProcessSessionCallResponse: Unknown response with id %u\n", pSecureStream->RequestId);
            OpcUa_GotoErrorWithStatus(OpcUa_BadUnknownResponse);
        }

        /*** invoke callback ***/
        if(pSecureRequest->Callback != OpcUa_Null)
        {
            /* reset buffer index to start reading from the first buffer */
            pSecureStream->nCurrentReadBuffer = 0;

            pSecureRequest->Callback(   a_pConnection,
                                        pSecureRequest->CallbackData,
                                        a_responseStatus,
                                        &pSecureIstrm);
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "WARNING: Callback is NULL!\n");
        }

        /* delete request */
        if(pSecureRequest != OpcUa_Null)
        {
            OpcUa_SecureRequest_Delete(&pSecureRequest);
            pSecureRequest = OpcUa_Null;
        }

        if(pSecureIstrm != OpcUa_Null)
        {
            pSecureIstrm->Delete((OpcUa_Stream**)&pSecureIstrm);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* delete request */
    if(pSecureRequest != OpcUa_Null)
    {
        OpcUa_SecureRequest_Delete(&pSecureRequest);
        pSecureRequest = OpcUa_Null;
    }

    /* null input stream handle */
    if(pSecureChannel != OpcUa_Null)
    {
        OpcUa_SecureChannel_SetPendingInputStream(  pSecureChannel,
                                                    OpcUa_Null);
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        pSecureIstrm->Delete((OpcUa_Stream**)&pSecureIstrm);
    }

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_CLIENTAPI */

/*============================================================================
 * End Of File
 *===========================================================================*/
