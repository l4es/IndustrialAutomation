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

#ifdef OPCUA_HAVE_SERVERAPI

#include <opcua_mutex.h>
#include <opcua_pkifactory.h>
#include <opcua_cryptofactory.h>
#include <opcua_datetime.h>
#include <opcua_list.h>
#include <opcua_thread.h>
#include <opcua_threadpool.h>

/* stackcore */
#include <opcua_identifiers.h>
#include <opcua_binaryencoder.h>
#include <opcua_securechannel.h>
#include <opcua_listener.h>

/* security */
#include <opcua_tcpsecurechannel.h>
#include <opcua_soapsecurechannel.h>
#include <opcua_securelistener_channelmanager.h>
#include <opcua_securechannel_types.h>
#include <opcua_securestream.h>

/* header */
#include <opcua_securelistener.h>
#include <opcua_securelistener_policymanager.h>

/** @brief Map enumeration OpcUa_MessageSecurityMode to bitfield */
#define OPCUA_ENDPOINT_MESSAGESECURITYMODE_FROM_ENUM(xMode) (OpcUa_UInt16)((xMode) == OpcUa_MessageSecurityMode_SignAndEncrypt \
                                                             ? OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_SIGNANDENCRYPT : (xMode))

/** @brief internal configuration - only for test */
#define OPCUA_SECURELISTENER_ALLOW_NOPKI OPCUA_CONFIG_NO

/** @brief Return OpcUa_BadDisconnect after the service for closing the securechannel was called. Fixes an issue with clients that do nothing after sending the CSC. */
#define OPCUA_SECURELISTENER_CLOSE_CONNECTION_ON_CLOSE_SECURECHANNEL OPCUA_CONFIG_YES

/*============================================================================
 * Prototypes
 *===========================================================================*/

/*============================================================================
 * OpcUa_SecureListener_BeginSendOpenSecureChannelResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_BeginSendOpenSecureChannelResponse(
    OpcUa_Listener*             a_pListener,
    OpcUa_SecureChannel*        a_pSecureChannel,
    OpcUa_InputStream*          a_pSecureIstrm,
    OpcUa_String*               a_pSecurityPolicyUri,
    OpcUa_MessageSecurityMode   a_MessageSecurityMode,
    OpcUa_CryptoProvider*       a_pCryptoProvider,
    OpcUa_ByteString*           a_pServerCertificate,
    OpcUa_ByteString*           a_pClientCertificateThumbprint,
    OpcUa_OutputStream**        a_ppSecureOstrm);

/*============================================================================
 * OpcUa_SecureListener_EndSendOpenSecureChannelResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_EndSendOpenSecureChannelResponse(
    OpcUa_Listener*             pListener,
    OpcUa_OutputStream**        ppSecureOstrm,
    OpcUa_CryptoProvider*       pCryptoProvider,
    OpcUa_ByteString*           pClientCertificate,
    OpcUa_ByteString*           pServerCertificate,
    OpcUa_MessageSecurityMode   messageSecurityMode,
    OpcUa_StatusCode            uStatus);

/*============================================================================
 * OpcUa_SecureListener_ProcessOpenSecureChannelRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ProcessOpenSecureChannelRequest(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_InputStream**             a_ppTransportIstrm,
    OpcUa_Boolean                   a_bRequestComplete);

/*============================================================================
 * OpcUa_SecureListener_ProcessCloseSecureChannelRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ProcessCloseSecureChannelRequest(
    OpcUa_Listener*                 pListener,
    OpcUa_Handle                    hConnection,
    OpcUa_InputStream**             ppTransportIstrm,
    OpcUa_Boolean                   bRequestComplete);

/*============================================================================
 * OpcUa_SecureListener_ProcessSessionCallRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ProcessSessionCallRequest(
    OpcUa_Listener*                 pListener,
    OpcUa_Handle                    hConnection,
    OpcUa_InputStream**             ppTransportIstrm,
    OpcUa_Boolean                   bRequestComplete);

/*============================================================================
 * OpcUa_SecureListener_OnNotify
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_OnNotify(
    OpcUa_Listener*                 pListener,
    OpcUa_Void*                     pCallbackData,
    OpcUa_ListenerEvent             eEvent,
    OpcUa_Handle                    hConnection,
    OpcUa_InputStream**             ppIstrm,
    OpcUa_StatusCode                uOperationStatus);

/*============================================================================
 * OpcUa_SecureListener_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_Open(
    OpcUa_Listener*                 pListener,
    OpcUa_String*                   sUrl,
    OpcUa_Boolean                   bListenOnAllInterfaces,
    OpcUa_Listener_PfnOnNotify*     pCallback,
    OpcUa_Void*                     pCallbackData);

/*============================================================================
 * OpcUa_SecureListener_ProcessRequest
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_ProcessRequest(
    OpcUa_Listener*                 pListener,
    OpcUa_Handle                    hConnection,
    OpcUa_InputStream**             ppIstrm,
    OpcUa_Boolean                   bRequestComplete);

/*============================================================================
 * OpcUa_SecureListener_SanityCheck
 *===========================================================================*/
#define OpcUa_SecureListener_SanityCheck 0xA0A40F79

/*============================================================================
 * OpcUa_SecureListenerState
 *===========================================================================*/
typedef enum _OpcUa_SecureListenerState
{
    OpcUa_SecureListenerState_Open,
    OpcUa_SecureListenerState_Closed,
    OpcUa_SecureListenerState_Unknown
}
OpcUa_SecureListenerState;

/*============================================================================
 * OpcUa_SecureListener
 *===========================================================================*/
typedef struct _OpcUa_SecureListener
{
    OpcUa_UInt32                                    SanityCheck;
    OpcUa_Mutex                                     Mutex;
    OpcUa_Listener*                                 TransportListener;
    OpcUa_Listener_PfnOnNotify*                     Callback;
    OpcUa_Void*                                     CallbackData;
    OpcUa_SecureListener_PfnSecureChannelCallback*  SecureChannelCallback;
    OpcUa_Void*                                     SecureChannelCallbackData;
    OpcUa_SecureListenerState                       State;
    OpcUa_SecureListener_ChannelManager*            ChannelManager;
    OpcUa_SecureListener_PolicyManager*             PolicyManager;
    OpcUa_PKIProvider*                              ServerPKIProvider;
    OpcUa_Decoder*                                  Decoder;
    OpcUa_Encoder*                                  Encoder;
    OpcUa_StringTable*                              NamespaceUris;
    OpcUa_EncodeableTypeTable*                      KnownTypes;
    OpcUa_ByteString*                               pServerCertificate;
    OpcUa_Key                                       ServerPrivateKey;
    OpcUa_UInt32                                    uNextSecureChannelId;
}
OpcUa_SecureListener;

/*============================================================================
 * OpcUa_SecureListener_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_Open(
    OpcUa_Listener*             a_pListener,
    OpcUa_String*               a_sUrl,
    OpcUa_Boolean               a_bListenOnAllInterfaces,
    OpcUa_Listener_PfnOnNotify* a_pCallback,
    OpcUa_Void*                 a_pCallbackData)
{
    OpcUa_SecureListener* pSecureListener = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "Open");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_sUrl);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallback);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, a_pListener, Open);

    pSecureListener = (OpcUa_SecureListener*)a_pListener->Handle;

    /* acquire lock until open is complete */
    OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);

    if(pSecureListener->State != OpcUa_SecureListenerState_Closed)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    pSecureListener->Callback     = a_pCallback;
    pSecureListener->CallbackData = a_pCallbackData;
    pSecureListener->State        = OpcUa_SecureListenerState_Unknown;

    /* open the non-secure listener */
    uStatus = OpcUa_Listener_Open(  pSecureListener->TransportListener,
                                    a_sUrl,
                                    a_bListenOnAllInterfaces,
                                    OpcUa_SecureListener_OnNotify,
                                    a_pListener);
    OpcUa_GotoErrorIfBad(uStatus);

    /* release lock */
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* release lock on failure */
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_ProcessRequest
 *===========================================================================*/
/* looks at the message type header field in the input stream and calls the
   appropriate handler */
static OpcUa_StatusCode OpcUa_SecureListener_ProcessRequest(
    OpcUa_Listener*         a_pSecureListenerInterface,
    OpcUa_Handle            a_hTransportConnection,
    OpcUa_InputStream**     a_ppTransportIstrm,
    OpcUa_Boolean           a_bRequestComplete)
{
    OpcUa_SecureListener*       pSecureListener                 = OpcUa_Null;
    OpcUa_SecureMessageType     requestType                     = OpcUa_SecureMessageType_UN;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ProcessRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListenerInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppTransportIstrm);

    /*** get SecureListener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pSecureListenerInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    /*** acquire lock until callback is complete. ***/
    OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);

    /* shutdown check */
    OpcUa_GotoErrorIfTrue((pSecureListener->State != OpcUa_SecureListenerState_Open), OpcUa_BadShutdown);

    /*** check type of incoming service request ***/
    uStatus = OpcUa_SecureStream_CheckInputHeaderType(  *a_ppTransportIstrm,
                                                        &requestType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* based on the service type, take the appropriate handler. */
    switch(requestType)
    {
    /* OpenSecureChannel */
    case OpcUa_SecureMessageType_SO:
        {
            uStatus = OpcUa_SecureListener_ProcessOpenSecureChannelRequest(     a_pSecureListenerInterface,
                                                                                a_hTransportConnection,
                                                                                a_ppTransportIstrm,
                                                                                a_bRequestComplete);
            break;
        }
    /* CloseSecureChannel */
    case OpcUa_SecureMessageType_SC:
        {
            uStatus = OpcUa_SecureListener_ProcessCloseSecureChannelRequest(    a_pSecureListenerInterface,
                                                                                a_hTransportConnection,
                                                                                a_ppTransportIstrm,
                                                                                a_bRequestComplete);
            break;
        }
    /* SecureMessage - standard protocol message */
    case OpcUa_SecureMessageType_SM:
        {
            uStatus = OpcUa_SecureListener_ProcessSessionCallRequest(           a_pSecureListenerInterface,
                                                                                a_hTransportConnection,
                                                                                a_ppTransportIstrm,
                                                                                a_bRequestComplete);
            break;

        }
    /* Undefined - Error Status */
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "ProcessRequest: Invalid message header detected!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

    /*** release lock. ***/
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /*** release lock. ***/
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

    if(a_ppTransportIstrm != OpcUa_Null && *a_ppTransportIstrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)*a_ppTransportIstrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)a_ppTransportIstrm);
    }

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_SecureChannel* pSecureChannel = OpcUa_Null;

        OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(
            pSecureListener->ChannelManager,
            a_hTransportConnection,
            &pSecureChannel);

        if(pSecureChannel != OpcUa_Null)
        {
            if((OpcUa_SecureMessageType_SC != requestType) &&
               (OpcUa_SecureMessageType_UN != requestType))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_ProcessRequest: Closing channel due error 0x%08X!\n", uStatus);
                pSecureChannel->Close(pSecureChannel);
            }

            pSecureChannel->LockWriteMutex(pSecureChannel);
            if(pSecureChannel->bAsyncWriteInProgress)
            {
                OpcUa_Listener_AddToSendQueue(
                    pSecureListener->TransportListener,
                    pSecureChannel->TransportConnection,
                    pSecureChannel->pPendingSendBuffers,
                    0);
                pSecureChannel->bAsyncWriteInProgress = OpcUa_False;
                pSecureChannel->pPendingSendBuffers = OpcUa_Null;
                pSecureChannel->uPendingMessageCount = 0;
            }
            OpcUa_SecureListener_ChannelManager_SetTransportConnection(
                    pSecureListener->ChannelManager,
                    pSecureChannel,
                    OpcUa_Null);
            pSecureListener->TransportListener->CloseConnection(
                    pSecureListener->TransportListener,
                    a_hTransportConnection,
                    uStatus);
            pSecureChannel->UnlockWriteMutex(pSecureChannel);
        }

        OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);
    }
    else
    {
        if(OpcUa_SecureMessageType_SC != requestType)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_ProcessRequest: NOT closing channel due error 0x%08X! Ignored!\n", uStatus);
        }
    }

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureListener_AbortRequestAndClose
 *===========================================================================*/
/** @brief Delete the stream currently being received and close underlying connection if needed.  */
static OpcUa_StatusCode OpcUa_SecureListener_AbortRequestAndClose(
    OpcUa_Listener*     a_pSecureListenerInterface,
    OpcUa_Handle        a_hTransportConnection,
    OpcUa_StatusCode    a_uOperationStatus)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;
    OpcUa_InputStream*      pSecureIStrm    = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "AbortRequestAndClose");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListenerInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_AbortRequestAndClose: Clearing current input stream. 0x%08X\n", a_uOperationStatus);

    /*** get listener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pSecureListenerInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(
                pSecureListener->ChannelManager,
                a_hTransportConnection,
                &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(    pSecureChannel,
                                                            &pSecureIStrm);

    uStatus = OpcUa_SecureChannel_SetPendingInputStream(pSecureChannel,
                                                        OpcUa_Null);

    if(pSecureIStrm != OpcUa_Null)
    {
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIStrm);
    }

    pSecureChannel->LockWriteMutex(pSecureChannel);
    if(pSecureChannel->bAsyncWriteInProgress)
    {
        OpcUa_Listener_AddToSendQueue(
            pSecureListener->TransportListener,
            pSecureChannel->TransportConnection,
            pSecureChannel->pPendingSendBuffers,
            0);
        pSecureChannel->bAsyncWriteInProgress = OpcUa_False;
        pSecureChannel->pPendingSendBuffers = OpcUa_Null;
        pSecureChannel->uPendingMessageCount = 0;
    }
    OpcUa_SecureListener_ChannelManager_SetTransportConnection(
            pSecureListener->ChannelManager,
            pSecureChannel,
            OpcUa_Null);
    pSecureListener->TransportListener->CloseConnection(
            pSecureListener->TransportListener,
            a_hTransportConnection,
            a_uOperationStatus);
    pSecureChannel->UnlockWriteMutex(pSecureChannel);

    OPCUA_SECURECHANNEL_LOCK(pSecureChannel);
    pSecureChannel->uExpirationCounter = 0;
    pSecureChannel->uOverlapCounter = 0;
    OPCUA_SECURECHANNEL_UNLOCK(pSecureChannel);

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
                pSecureListener->ChannelManager,
                &pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureListener_AbortRequest
 *===========================================================================*/
/** @brief Check the input stream and delete the stream currently being received
           if everything is ok. Close underlying connection if needed. This is the
           handler for abort messages. */
static OpcUa_StatusCode OpcUa_SecureListener_AbortRequest(
    OpcUa_Listener*         a_pSecureListenerInterface,
    OpcUa_Handle            a_hTransportConnection,
    OpcUa_InputStream**     a_ppTransportIstrm,
    OpcUa_StatusCode        a_uOperationStatus)
{
    OpcUa_SecureListener*   pSecureListener         = OpcUa_Null;
    OpcUa_InputStream*      pSecureIStrm            = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel          = OpcUa_Null;
    OpcUa_SecureMessageType requestType             = OpcUa_SecureMessageType_UN;
    OpcUa_UInt32            uTokenId                = 0;
    OpcUa_UInt32            uSecureChannelId        = OPCUA_SECURECHANNEL_ID_INVALID;
    OpcUa_Boolean           bLock                   = OpcUa_False;

    OpcUa_SecurityKeyset*   pReceivingKeyset        = OpcUa_Null;
    OpcUa_CryptoProvider*   pCryptoProvider         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "AbortRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListenerInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIstrm);

    OpcUa_ReferenceParameter(a_uOperationStatus);

    /*** get listener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pSecureListenerInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    /*** check type of incoming service request ***/
    uStatus = OpcUa_SecureStream_CheckInputHeaderType(  *a_ppTransportIstrm,
                                                        &requestType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* parse stream header */
    uStatus = OpcUa_SecureStream_DecodeSymmetricSecurityHeader( *a_ppTransportIstrm,
                                                                &uSecureChannelId,
                                                                &uTokenId);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_AbortRequest: SID %u, TID %u, Status 0x%08X\n", uSecureChannelId, uTokenId, a_uOperationStatus);

    /*** acquire lock until callback is complete. ***/
    OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);
    bLock = OpcUa_True;

    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(
                pSecureListener->ChannelManager,
                a_hTransportConnection,
                &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(pSecureChannel,
                                                        &pSecureIStrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Get reference to keyset for requested token id */
    uStatus = pSecureChannel->GetSecuritySet(   pSecureChannel,
                                                uTokenId,
                                                &pReceivingKeyset,
                                                OpcUa_Null,
                                                &pCryptoProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    /* this is the final chunk */
    uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportIstrm,
                                                pSecureIStrm,
                                                &pReceivingKeyset->SigningKey,
                                                &pReceivingKeyset->EncryptionKey,
                                                &pReceivingKeyset->InitializationVector,
                                                pCryptoProvider,
                                                pSecureChannel);
    /* release reference to security set */
    pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                          uTokenId);

    OpcUa_GotoErrorIfBad(uStatus);


    /* TODO: decoding of status and reason for the cancellation; not required by now */


    /* stream is unlinked */
    (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_AbortRequest: Clearing current input stream.\n");

    OpcUa_SecureChannel_SetPendingInputStream(  pSecureChannel,
                                                OpcUa_Null);

    /* unlock resources */
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
                pSecureListener->ChannelManager,
                &pSecureChannel);

    OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIStrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(bLock)
    {
        OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);
    }

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    if(a_ppTransportIstrm != OpcUa_Null)
    {
        (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);
    }

    uStatus = OpcUa_SecureListener_AbortRequestAndClose(a_pSecureListenerInterface,
                                                        a_hTransportConnection,
                                                        uStatus);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_OpenChannel
 *===========================================================================*/
/** @brief Create and initialize a new secure channel. Close the underlying
           connection on error. This is the handler for channel openend messages. */
static OpcUa_StatusCode OpcUa_SecureListener_OpenChannel(
    OpcUa_Listener*         a_pSecureListenerInterface,
    OpcUa_Handle            a_hTransportConnection,
    OpcUa_StatusCode        a_uOperationStatus)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel  = OpcUa_Null;
    OpcUa_UInt32            uReceiveBufferSize = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "OpenChannel");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListenerInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReferenceParameter(a_uOperationStatus);

    /*** get listener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pSecureListenerInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    /* create TcpSecureChannel */
    uStatus = OpcUa_TcpSecureChannel_Create(&pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    pSecureChannel->TransportConnection = a_hTransportConnection;
    pSecureChannel->SecureChannelId = OPCUA_SECURECHANNEL_ID_INVALID;
    pSecureChannel->uOverlapCounter = (OpcUa_UInt32)(OPCUA_SECURELISTENER_CHANNELTIMEOUT/OPCUA_SECURELISTENER_WATCHDOG_INTERVAL);

    /* Calculate max number of chunks per message. */
    uStatus = OpcUa_Listener_GetReceiveBufferSize(pSecureListener->TransportListener,
                                                  pSecureChannel->TransportConnection,
                                                  &uReceiveBufferSize);
    OpcUa_GotoErrorIfBad(uStatus);
    pSecureChannel->nMaxBuffersPerMessage = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize/uReceiveBufferSize + 1;

    /* Get the peer information from transport listener here - the listener is definitely valid in this context */
    uStatus = OpcUa_Listener_GetPeerInfo(pSecureListener->TransportListener,
                                         pSecureChannel->TransportConnection,
                                         &pSecureChannel->sPeerInfo);
    OpcUa_GotoErrorIfBad(uStatus);

    /* add SecureChannel to SecureChannelManager */
    uStatus = OpcUa_SecureListener_ChannelManager_AddChannel(pSecureListener->ChannelManager, pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureChannel != OpcUa_Null)
    {
        OpcUa_TcpSecureChannel_Delete(&pSecureChannel);
    }

    pSecureListener->TransportListener->CloseConnection(
           pSecureListener->TransportListener,
           a_hTransportConnection,
           uStatus);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_OnNotify
 *===========================================================================*/
/* Gets called from the non secure listener on events. */
static OpcUa_StatusCode OpcUa_SecureListener_OnNotify(
    OpcUa_Listener*         a_pTransportListener,
    OpcUa_Void*             a_pCallbackData,
    OpcUa_ListenerEvent     a_eEvent,
    OpcUa_Handle            a_hTransportConnection,
    OpcUa_InputStream**     a_ppTransportIstrm,
    OpcUa_StatusCode        a_uOperationStatus)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "OnNotify");

    OpcUa_ReturnErrorIfArgumentNull(a_pTransportListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallbackData);

    /* this check must fail if this function is called after the listener is deleted. */
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, (OpcUa_Listener*)a_pCallbackData, Open);

    pSecureListener = (OpcUa_SecureListener*)((OpcUa_Listener*)a_pCallbackData)->Handle;

    switch(a_eEvent)
    {
    case OpcUa_ListenerEvent_Open:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: Transport Open\n");

            if(pSecureListener->Callback != OpcUa_Null)
            {
                pSecureListener->Callback(  (OpcUa_Listener*)a_pCallbackData,
                                            pSecureListener->CallbackData,
                                            a_eEvent,
                                            a_hTransportConnection,
                                            OpcUa_Null,
                                            a_uOperationStatus);
            }

            break;
        }
    case OpcUa_ListenerEvent_Close:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: Transport Close\n");

            if(pSecureListener->Callback != OpcUa_Null)
            {
                pSecureListener->Callback(  (OpcUa_Listener*)a_pCallbackData,
                                            pSecureListener->CallbackData,
                                            a_eEvent,
                                            a_hTransportConnection,
                                            OpcUa_Null,
                                            a_uOperationStatus);
            }

            break;
        }
    case OpcUa_ListenerEvent_ChannelOpened:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: Transport Connection Opened\n");

            uStatus = OpcUa_SecureListener_OpenChannel( (OpcUa_Listener*)a_pCallbackData,
                                                        a_hTransportConnection,
                                                        a_uOperationStatus);
            OpcUa_GotoErrorIfBad(uStatus);

            break;
        }
    case OpcUa_ListenerEvent_ChannelClosed:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: Transport Connection Closed\n");

            /* get securechannel for the connection */
            uStatus = OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(  pSecureListener->ChannelManager,
                                                                                            a_hTransportConnection,
                                                                                            &pSecureChannel);
            OpcUa_GotoErrorIfBad(uStatus);

            /* unlink connection handle from securechannel id! */
            /* this securechannel is invalid until a service with the corresponding securechannel id is called! */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureListener_OnNotify: Transport Connection lost for SecureChannel %u\n", pSecureChannel->SecureChannelId);
            pSecureChannel->LockWriteMutex(pSecureChannel);
            if(pSecureChannel->bAsyncWriteInProgress)
            {
                OpcUa_Listener_AddToSendQueue(
                    pSecureListener->TransportListener,
                    pSecureChannel->TransportConnection,
                    pSecureChannel->pPendingSendBuffers,
                    OPCUA_LISTENER_CLOSE_WHEN_DONE);
                pSecureChannel->bAsyncWriteInProgress = OpcUa_False;
                pSecureChannel->pPendingSendBuffers = OpcUa_Null;
                pSecureChannel->uPendingMessageCount = 0;
            }
            OpcUa_SecureListener_ChannelManager_SetTransportConnection(
                    pSecureListener->ChannelManager,
                    pSecureChannel,
                    OpcUa_Null);
            pSecureChannel->UnlockWriteMutex(pSecureChannel);

            OPCUA_SECURECHANNEL_LOCK(pSecureChannel);
            pSecureChannel->uExpirationCounter = 0;
            pSecureChannel->uOverlapCounter = 0;
            OPCUA_SECURECHANNEL_UNLOCK(pSecureChannel);

            OpcUa_SecureListener_ChannelManager_ReleaseChannel(
                    pSecureListener->ChannelManager,
                    &pSecureChannel);

            break;
        }
    case OpcUa_ListenerEvent_Request:
        {
            if(OpcUa_IsBad(a_uOperationStatus))
            {
                /* called in case if an error occurs on lower layers */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_OnNotify: Request with 0x%08X; aborting and closing connection ...\n", a_uOperationStatus);
                return OpcUa_SecureListener_AbortRequestAndClose(   (OpcUa_Listener*)a_pCallbackData,
                                                                    a_hTransportConnection,
                                                                    a_uOperationStatus);
            }
            else
            {
                /* this call handles a standard request message */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: Request\n");
                return OpcUa_SecureListener_ProcessRequest( (OpcUa_Listener*)a_pCallbackData,
                                                            a_hTransportConnection,
                                                            a_ppTransportIstrm,
                                                            OpcUa_True);
            }

        }
    case OpcUa_ListenerEvent_RequestPartial:
        {
            if(OpcUa_IsBad(a_uOperationStatus))
            {
                /* called in case if an error occurs on lower layers */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_OnNotify: RequestPartial with 0x%08X; aborting and closing connection ...\n", a_uOperationStatus);
                return OpcUa_SecureListener_AbortRequestAndClose(   (OpcUa_Listener*)a_pCallbackData,
                                                                    a_hTransportConnection,
                                                                    a_uOperationStatus);
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: RequestPartial\n");
                /* this call handles a standard request message */
                return OpcUa_SecureListener_ProcessRequest((OpcUa_Listener*)a_pCallbackData,
                                                            a_hTransportConnection,
                                                            a_ppTransportIstrm,
                                                            OpcUa_False);
            }
        }
    case OpcUa_ListenerEvent_RequestAbort:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: RequestAbort!\n");
            /* received an abort message for the current request stream */
            return OpcUa_SecureListener_AbortRequest(   (OpcUa_Listener*)a_pCallbackData,
                                                        a_hTransportConnection,
                                                        a_ppTransportIstrm,
                                                        a_uOperationStatus);
        }
    case OpcUa_ListenerEvent_RefillSendQueue:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: RefillSendQueue\n");
            /* get securechannel for the connection */
            uStatus = OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(  pSecureListener->ChannelManager,
                                                                                            a_hTransportConnection,
                                                                                            &pSecureChannel);
            OpcUa_GotoErrorIfBad(uStatus);

            pSecureChannel->LockWriteMutex(pSecureChannel);
            OpcUa_Listener_AddToSendQueue(
                pSecureListener->TransportListener,
                pSecureChannel->TransportConnection,
                pSecureChannel->pPendingSendBuffers,
                pSecureChannel->uPendingMessageCount > OPCUA_SECURECONNECTION_MAXPENDINGMESSAGES
                ? OPCUA_LISTENER_NO_RCV_UNTIL_DONE : 0);
            pSecureChannel->bAsyncWriteInProgress = pSecureChannel->pPendingSendBuffers != OpcUa_Null;
            pSecureChannel->pPendingSendBuffers = OpcUa_Null;
            pSecureChannel->uPendingMessageCount = 0;
            pSecureChannel->UnlockWriteMutex(pSecureChannel);

            OpcUa_SecureListener_ChannelManager_ReleaseChannel(
                    pSecureListener->ChannelManager,
                    &pSecureChannel);
            break;
        }
    case OpcUa_ListenerEvent_UnexpectedError:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: UnexpectedError\n");
            break;
        }
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_OnNotify: Default\n");
            break;
        }
    }

    /* all events except Requests: */
    /* acquire lock until callback is complete. */
    OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);

    /* no longer open due to either an error or a close. */
    if(OpcUa_IsBad(a_uOperationStatus))
    {
        pSecureListener->State = OpcUa_SecureListenerState_Closed;
    }
    else
    {
        /* set state to open if opened successfully. */
        if(a_eEvent == OpcUa_ListenerEvent_Open)
        {
            pSecureListener->State = OpcUa_SecureListenerState_Open;
        }
    }

    /* release lock. */
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_Close(OpcUa_Listener* a_pListener)
{
    OpcUa_SecureListener* pSecureListener = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, a_pListener, Close);

    pSecureListener = (OpcUa_SecureListener*)a_pListener->Handle;

    /* acquire lock until open is complete */
    OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);

    if(pSecureListener->State != OpcUa_SecureListenerState_Open)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    pSecureListener->State = OpcUa_SecureListenerState_Closed;

    /* cleanup all channels */

    /* release lock */
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

    /* close the non-secure listener */
    uStatus = OpcUa_Listener_Close(pSecureListener->TransportListener);
    OpcUa_ReturnErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* release lock on failure */
    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_GetSecureChannelId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_GetSecureChannelId(
    OpcUa_InputStream*  a_pSecureIstrm,
    OpcUa_UInt32*       a_pSecureChannelId)
{
    OpcUa_SecureStream* pSecureStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "GetSecureChannelId");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannelId);

    pSecureStream = (OpcUa_SecureStream*)a_pSecureIstrm->Handle;

    *a_pSecureChannelId = pSecureStream->SecureChannelId;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_BeginSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_BeginSendResponse(OpcUa_Listener*      a_pListener,
                                                        OpcUa_Handle         a_hSecureConnection,
                                                        OpcUa_InputStream**  a_ppSecureIstrm,
                                                        OpcUa_OutputStream** a_ppSecureOstrm)
{
    OpcUa_SecureListener*   pSecureListener     = OpcUa_Null;
    OpcUa_SecureStream*     pSecureInputStream  = OpcUa_Null;
    OpcUa_OutputStream*     pTransportOstrm     = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "BeginSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppSecureIstrm);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, a_pListener, BeginSendResponse);

    *a_ppSecureOstrm    = OpcUa_Null;
    pSecureListener     = (OpcUa_SecureListener*)a_pListener->Handle;
    pSecureInputStream  = (OpcUa_SecureStream*)(*a_ppSecureIstrm)->Handle;

    /*** get appropriate SecureChannel ***/
    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(  pSecureListener->ChannelManager,
                                                                                pSecureInputStream->SecureChannelId,
                                                                                &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    /* close incoming stream */
    uStatus = OpcUa_Stream_Close((OpcUa_Stream*)(*a_ppSecureIstrm));
    OpcUa_GotoErrorIfBad(uStatus);

    /* check whether SecureChannel is in a correct state */
    OPCUA_SECURECHANNEL_LOCK(pSecureChannel);
    if(     (pSecureChannel         == OpcUa_Null)
        ||  (pSecureChannel->State  != OpcUa_SecureChannelState_Opened))
    {
        uStatus = OpcUa_BadInvalidState;
        OPCUA_SECURECHANNEL_UNLOCK(pSecureChannel);
        OpcUa_GotoError;
    }
    OPCUA_SECURECHANNEL_UNLOCK(pSecureChannel);

    /* create inner (transport) stream */
    uStatus = OpcUa_Listener_BeginSendResponse( pSecureListener->TransportListener,
                                                a_hSecureConnection,
                                                (OpcUa_InputStream**)&pSecureInputStream->InnerStrm,
                                                &pTransportOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* create output stream */
    uStatus = OpcUa_SecureStream_CreateOutput(  pTransportOstrm,
                                                eOpcUa_SecureStream_Types_StandardMessage,
                                                pSecureInputStream->RequestId,
                                                pSecureChannel,
                                                a_ppSecureOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* delete stream */
    OpcUa_Stream_Delete((OpcUa_Stream**)a_ppSecureIstrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_BeginSendResponse: fail with 0x%08X", uStatus);
    OpcUa_Stream_Delete((OpcUa_Stream**)&pTransportOstrm);

    if(pSecureInputStream->InnerStrm != OpcUa_Null)
    {
        pSecureInputStream->InnerStrm->Delete(&pSecureInputStream->InnerStrm);
    }

    /* delete stream */
    OpcUa_Stream_Delete((OpcUa_Stream**)a_ppSecureIstrm);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_EndSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_EndSendResponse(
    OpcUa_Listener*         a_pListener,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_OutputStream**    a_ppOstrm)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream   = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "EndSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, a_pListener, EndSendResponse);

    pSecureListener     = (OpcUa_SecureListener*)a_pListener->Handle;
    pSecureStream       = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_EndSendResponse: ID %u, Status 0x%08X\n", pSecureStream->RequestId, a_uStatus);

    /* find securechannel and look for transport validation */
    pSecureChannel = pSecureStream->pSecureChannel;

    /* check if the channel has a valid transport connection. */
    if(pSecureChannel->TransportConnection == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_EndSendResponse: SecureChannel %u has no transport connection!\n", pSecureChannel->SecureChannelId);
        OpcUa_GotoErrorWithStatus(OpcUa_BadNoCommunication);
    }


    if(OpcUa_IsGood(a_uStatus))
    {
        /*** close secure stream - triggers send of last message chunk -> no network access after this call. ***/
        uStatus = ((OpcUa_Stream*)(*a_ppOstrm))->Close((OpcUa_Stream*)(*a_ppOstrm));

        /*** finish call at the transport listener - clean transport stream object ***/
        uStatus = OpcUa_Listener_EndSendResponse(   pSecureListener->TransportListener,
                                                    uStatus,
                                                    (OpcUa_OutputStream**)&pSecureStream->InnerStrm);
    }
    else
    {
        /* bad status code means, the application could not handle a problem -> trigger error message */
        /*** finish call at the transport listener - clean transport stream object ***/
        uStatus = OpcUa_Listener_EndSendResponse(   pSecureListener->TransportListener,
                                                    a_uStatus,
                                                    (OpcUa_OutputStream**)&pSecureStream->InnerStrm);
    }

    ((OpcUa_Stream*)(*a_ppOstrm))->Delete((OpcUa_Stream**)(a_ppOstrm));
    OpcUa_ReturnErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_EndSendResponse: Error 0x%08X while sending message\n", uStatus);

OpcUa_FinishErrorHandling;
} /* OpcUa_SecureListener_EndSendResponse */

/*============================================================================
 * OpcUa_SecureListener_AbortSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_AbortSendResponse(
    OpcUa_Listener*         a_pListener,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_String*           a_psReason,
    OpcUa_OutputStream**    a_ppOstrm)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream   = OpcUa_Null;
    OpcUa_String            sReason         = OPCUA_STRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "AbortSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOstrm);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, a_pListener, AbortSendResponse);

    pSecureListener     = (OpcUa_SecureListener*)a_pListener->Handle;
    pSecureStream       = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pSecureStream);

    /* check if abort message needs to be sent. */
    if(!pSecureStream->IsClosed && pSecureStream->uNoOfFlushes != 0 && OpcUa_IsBad(a_uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureListener_AbortSendResponse: called for used stream! Triggering Abort Message!\n");

        /* mark as abort type message */
        pSecureStream->Buffers[0].Data[3] = 'A';

        /* set internal pointer right after header elements */
        OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], pSecureStream->uBeginOfRequestBody);
        pSecureStream->Buffers[0].EndOfData = pSecureStream->uBeginOfRequestBody;
        uStatus = OpcUa_UInt32_BinaryEncode(a_uStatus, (*a_ppOstrm));
        uStatus = OpcUa_String_BinaryEncode(a_psReason?a_psReason:&sReason, (*a_ppOstrm));
        uStatus = (*a_ppOstrm)->Close((OpcUa_Stream*)(*a_ppOstrm));
        if(OpcUa_IsBad(uStatus))
        {
            /* if abort message could not be sent, the message pipeline is corrupted TODO */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_AbortSendResponse: Could not send abort message!\n");
        }
    }

    /* delete transport stream */
    OpcUa_Stream_Delete(&pSecureStream->InnerStrm);
    OpcUa_Stream_Delete((OpcUa_Stream**)a_ppOstrm);

    pSecureListener->TransportListener->AbortSendResponse(  pSecureListener->TransportListener,
                                                            a_uStatus,
                                                            a_psReason?a_psReason:&sReason,
                                                            OpcUa_Null); /* this should be allowed */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
} /* OpcUa_SecureListener_AbortSendResponse */

/*============================================================================
 * OpcUa_SecureListener_BeginSendOpenSecureChannelResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_BeginSendOpenSecureChannelResponse(
    OpcUa_Listener*             a_pListener,
    OpcUa_SecureChannel*        a_pSecureChannel,
    OpcUa_InputStream*          a_pSecureIstrm,
    OpcUa_String*               a_pSecurityPolicyUri,
    OpcUa_MessageSecurityMode   a_MessageSecurityMode,
    OpcUa_CryptoProvider*       a_pCryptoProvider,
    OpcUa_ByteString*           a_pServerCertificate,
    OpcUa_ByteString*           a_pClientCertificateThumbprint,
    OpcUa_OutputStream**        a_ppSecureOstrm)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream   = OpcUa_Null;
    OpcUa_OutputStream*     pInnerOstrm     = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "BeginSendOpenSecureChannelResponse");

    *a_ppSecureOstrm = OpcUa_Null;

    pSecureListener = (OpcUa_SecureListener*)a_pListener->Handle;
    pSecureStream   = (OpcUa_SecureStream*)a_pSecureIstrm->Handle;

    /* close incoming stream */
    uStatus = a_pSecureIstrm->Close((OpcUa_Stream*)a_pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = pSecureListener->TransportListener->BeginSendResponse(    pSecureListener->TransportListener,
                                                                        a_pSecureChannel->TransportConnection,
                                                                        (OpcUa_InputStream**)&pSecureStream->InnerStrm,
                                                                        &pInnerOstrm);

    OpcUa_GotoErrorIfBad(uStatus);

    /* create output stream */
    uStatus = OpcUa_SecureStream_CreateOpenSecureChannelOutput( pInnerOstrm,
                                                                a_pSecureChannel,
                                                                pSecureStream->RequestId,
                                                                a_pSecurityPolicyUri,
                                                                a_MessageSecurityMode,
                                                                a_pCryptoProvider,
                                                                a_pServerCertificate,
                                                                &pSecureListener->ServerPrivateKey,
                                                                &a_pSecureChannel->ClientCertificate,
                                                                a_pClientCertificateThumbprint,
                                                                a_ppSecureOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pInnerOstrm != OpcUa_Null)
    {
        OpcUa_Stream_Delete((OpcUa_Stream**)&pInnerOstrm);
        pInnerOstrm = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_EndSendOpenSecureChannelResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_EndSendOpenSecureChannelResponse(
    OpcUa_Listener*             a_pListener,
    OpcUa_OutputStream**        a_ppOstrm,
    OpcUa_CryptoProvider*       a_pCryptoProvider,
    OpcUa_ByteString*           a_pClientCertificate,
    OpcUa_ByteString*           a_pServerCertificate,
    OpcUa_MessageSecurityMode   a_messageSecurityMode,
    OpcUa_StatusCode            a_uStatus)
{
    OpcUa_SecureListener*   pSecureListener     = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "EndSendOpenSecureChannelResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOstrm);

    OpcUa_ReferenceParameter(a_pCryptoProvider);
    OpcUa_ReferenceParameter(a_pClientCertificate);
    OpcUa_ReferenceParameter(a_pServerCertificate);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureListener, a_pListener, EndSendResponse);

    pSecureListener = (OpcUa_SecureListener*)a_pListener->Handle;
    pSecureStream   = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;

    OpcUa_ReferenceParameter(a_messageSecurityMode);

    /*** close the secure stream ***/
    uStatus = (*a_ppOstrm)->Close((OpcUa_Stream*)(*a_ppOstrm));
    OpcUa_GotoErrorIfBad(uStatus);

    /*** send response over the wire ***/
    uStatus = pSecureListener->TransportListener->EndSendResponse(  pSecureListener->TransportListener,
                                                                    a_uStatus,
                                                                    (OpcUa_OutputStream**)&pSecureStream->InnerStrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** cleanup ***/
    if((*a_ppOstrm) != OpcUa_Null)
    {
        (*a_ppOstrm)->Delete((OpcUa_Stream**)a_ppOstrm);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureListener_EndSendOpenSecureChannelResponse: Error 0x%08X while sending message\n", uStatus);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_SecureListener_Delete(OpcUa_Listener** a_ppListener)
{
    OpcUa_SecureListener*   pSecureListener = OpcUa_Null;

    if(a_ppListener == OpcUa_Null || *a_ppListener == OpcUa_Null)
    {
        return;
    }

    pSecureListener = (OpcUa_SecureListener*)(*a_ppListener)->Handle;

    OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);

    if(pSecureListener->ChannelManager != OpcUa_Null)
    {
        OpcUa_SecureListener_ChannelManager_Delete(&pSecureListener->ChannelManager);
    }

    if(pSecureListener->PolicyManager != OpcUa_Null)
    {
        OpcUa_SecureListener_PolicyManager_Delete(&pSecureListener->PolicyManager);
    }

    if(pSecureListener->ServerPKIProvider != OpcUa_Null)
    {
        OPCUA_P_PKIFACTORY_DELETEPKIPROVIDER(pSecureListener->ServerPKIProvider);
        OpcUa_Free(pSecureListener->ServerPKIProvider);
        pSecureListener->ServerPKIProvider = OpcUa_Null;
    }

    pSecureListener->SanityCheck = 0;

    OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

    OPCUA_P_MUTEX_DELETE(&(pSecureListener->Mutex));

    OpcUa_Free(pSecureListener);
    pSecureListener = OpcUa_Null;

    OpcUa_Free(*a_ppListener);
    *a_ppListener = OpcUa_Null;
}

/*============================================================================
 * OpcUa_SecureListener_ChannelRemovedCallback
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_SecureListener_ChannelRemovedCallback(   OpcUa_SecureChannel*    a_pSecureChannel,
                                                                        OpcUa_Void*             a_pvCallbackData)
{
    OpcUa_SecureListener* pSecureListener = (OpcUa_SecureListener*)a_pvCallbackData;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureListener_ChannelRemovedCallback: SecureChannel %u timed out!\n", a_pSecureChannel->SecureChannelId);

    if(a_pSecureChannel->TransportConnection != OpcUa_Null)
    {
        /* TODO: this still crashes!
        pSecureListener->TransportListener->CloseConnection(    pSecureListener->TransportListener,
                                                                a_pSecureChannel->TransportConnection,
                                                                OpcUa_Good);
        */
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelRemovedCallback: SecureChannel %u has no transport connection set!\n", a_pSecureChannel->SecureChannelId);
    }

    if(     OpcUa_Null != pSecureListener
        &&  OpcUa_Null != pSecureListener->SecureChannelCallback)
    {
        pSecureListener->SecureChannelCallback( a_pSecureChannel->SecureChannelId,
                                                eOpcUa_SecureListener_SecureChannelClose,
                                                OpcUa_BadSecureChannelClosed,
                                                OpcUa_Null,
                                                OpcUa_Null,
                                                0,
                                                pSecureListener->SecureChannelCallbackData);
    }
}

/*============================================================================
 * OpcUa_SecureListener_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_Create(
    OpcUa_Listener*                                     a_pInnerListener,
    OpcUa_Decoder*                                      a_pDecoder,
    OpcUa_Encoder*                                      a_pEncoder,
    OpcUa_StringTable*                                  a_pNamespaceUris,
    OpcUa_EncodeableTypeTable*                          a_pKnownTypes,
    OpcUa_ByteString*                                   a_pServerCertificate,
    OpcUa_Key*                                          a_pServerPrivateKey,
    OpcUa_Void*                                         a_pPKIConfig,
    OpcUa_UInt32                                        a_nNoSecurityPolicies,
    OpcUa_SecureListener_SecurityPolicyConfiguration*   a_pSecurityPolicyConfigurations,
    OpcUa_SecureListener_PfnSecureChannelCallback*      a_pfSecureChannelCallback,
    OpcUa_Void*                                         a_SecureChannelCallbackData,
    OpcUa_Listener**                                    a_ppListener)
{
    OpcUa_SecureListener*                pSecureListener    = OpcUa_Null;
    OpcUa_SecureListener_ChannelManager* pChannelManager    = OpcUa_Null;
    OpcUa_SecureListener_PolicyManager*  pPolicyManager     = OpcUa_Null;

    OpcUa_UInt32                         nCounter           = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_pInnerListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pEncoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pNamespaceUris);
    OpcUa_ReturnErrorIfArgumentNull(a_pKnownTypes);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecurityPolicyConfigurations);
    OpcUa_ReturnErrorIfArgumentNull(a_ppListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pPKIConfig);

#if !OPCUA_SECURELISTENER_ALLOW_NOPKI
    OpcUa_ReturnErrorIfArgumentNull(a_pServerCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerPrivateKey);
    OpcUa_ReturnErrorIfTrue((a_pServerPrivateKey->Key.Data == OpcUa_Null),  OpcUa_BadInvalidArgument);
#endif /* OPCUA_SECURELISTENER_ALLOW_NOPKI */

    /* allocate listener object */
    *a_ppListener = (OpcUa_Listener*)OpcUa_Alloc(sizeof(OpcUa_Listener));
    OpcUa_GotoErrorIfAllocFailed(*a_ppListener);
    OpcUa_MemSet(*a_ppListener, 0, sizeof(OpcUa_Listener));

    /* allocate listener handle */
    pSecureListener = (OpcUa_SecureListener*)OpcUa_Alloc(sizeof(OpcUa_SecureListener));
    OpcUa_GotoErrorIfAllocFailed(pSecureListener);
    OpcUa_MemSet(pSecureListener, 0, sizeof(OpcUa_SecureListener));

    /* create SecureChannelManager */
    uStatus = OpcUa_SecureListener_ChannelManager_Create(
        OpcUa_SecureListener_ChannelRemovedCallback,
        (OpcUa_Void*)pSecureListener,
        &pChannelManager);
    OpcUa_GotoErrorIfBad(uStatus);

    /* create PolicyManager */
    uStatus = OpcUa_SecureListener_PolicyManager_Create(&pPolicyManager);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Add security policies */
    for(nCounter = 0; nCounter < a_nNoSecurityPolicies; nCounter++)
    {
        /* TODO: error check and proper handling */
        uStatus = OpcUa_SecureListener_PolicyManager_AddSecurityPolicyConfiguration(
            pPolicyManager,
            &(a_pSecurityPolicyConfigurations[nCounter]));
    }

    /* create PKI provider for the server */
    pSecureListener->ServerPKIProvider = (OpcUa_PKIProvider*)OpcUa_Alloc(sizeof(OpcUa_PKIProvider));
    OpcUa_GotoErrorIfAllocFailed(pSecureListener->ServerPKIProvider);

    uStatus = OPCUA_P_PKIFACTORY_CREATEPKIPROVIDER( a_pPKIConfig, pSecureListener->ServerPKIProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    /* initialize listener handle */
    pSecureListener->Decoder            = a_pDecoder;
    pSecureListener->Encoder            = a_pEncoder;
    pSecureListener->NamespaceUris      = a_pNamespaceUris;
    pSecureListener->KnownTypes         = a_pKnownTypes;
    pSecureListener->pServerCertificate = a_pServerCertificate; /* all UA applications require application instance certificate, even if "NoSecurity-Policy" is selected*/

    if(a_pServerPrivateKey != OpcUa_Null)
    {
        pSecureListener->ServerPrivateKey = *a_pServerPrivateKey;
    }

    pSecureListener->SanityCheck                = OpcUa_SecureListener_SanityCheck;
    pSecureListener->TransportListener          = a_pInnerListener;
    pSecureListener->State                      = OpcUa_SecureListenerState_Closed;
    pSecureListener->ChannelManager             = pChannelManager;
    pSecureListener->PolicyManager              = pPolicyManager;
    pSecureListener->SecureChannelCallback      = a_pfSecureChannelCallback;
    pSecureListener->SecureChannelCallbackData  = a_SecureChannelCallbackData;
    pSecureListener->uNextSecureChannelId       = 1;

    /* create mutex */
    uStatus = OPCUA_P_MUTEX_CREATE(&(pSecureListener->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    /* initialize listener object */
    (*a_ppListener)->Handle              = pSecureListener;
    (*a_ppListener)->Open                = OpcUa_SecureListener_Open;
    (*a_ppListener)->Close               = OpcUa_SecureListener_Close;
    (*a_ppListener)->BeginSendResponse   = OpcUa_SecureListener_BeginSendResponse;
    (*a_ppListener)->EndSendResponse     = OpcUa_SecureListener_EndSendResponse;
    (*a_ppListener)->AbortSendResponse   = OpcUa_SecureListener_AbortSendResponse;
    (*a_ppListener)->CloseConnection     = OpcUa_Null; /*OpcUa_SecureListener_CloseConnection;*/
    (*a_ppListener)->Delete              = OpcUa_SecureListener_Delete;
    (*a_ppListener)->GetPeerInfo         = OpcUa_Null; /*OpcUa_SecureListener_GetPeerInfo;*/

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureListener != OpcUa_Null)
    {
        OPCUA_P_MUTEX_DELETE(&(pSecureListener->Mutex));
        if(pSecureListener->ServerPKIProvider != OpcUa_Null)
        {
            OPCUA_P_PKIFACTORY_DELETEPKIPROVIDER(pSecureListener->ServerPKIProvider);
            OpcUa_Free(pSecureListener->ServerPKIProvider);
        }
        OpcUa_Free(pSecureListener);
    }

    if(pChannelManager != OpcUa_Null)
    {
        OpcUa_SecureListener_ChannelManager_Delete(&pChannelManager);
    }

    if(pPolicyManager != OpcUa_Null)
    {
        OpcUa_SecureListener_PolicyManager_Delete(&pPolicyManager);
    }


    OpcUa_Free(*a_ppListener);
    *a_ppListener = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_ReadRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ReadRequest(  OpcUa_SecureListener*   a_pSecureListener,
                                                    OpcUa_InputStream*      a_pIstrm,
                                                    OpcUa_UInt32            a_expectedTypeId,
                                                    OpcUa_Void**            a_ppRequest,
                                                    OpcUa_EncodeableType**  a_ppRequestType)
{
    OpcUa_Decoder*          pDecoder        = OpcUa_Null;
    OpcUa_EncodeableType*   pRequestType    = OpcUa_Null;
    OpcUa_Handle            hDecodeContext  = OpcUa_Null;
    OpcUa_MessageContext    cContext;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ReadRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    *a_ppRequest    = OpcUa_Null;

    pDecoder        = a_pSecureListener->Decoder;

    OpcUa_MessageContext_Initialize(&cContext);

    cContext.KnownTypes         = a_pSecureListener->KnownTypes;
    cContext.NamespaceUris      = a_pSecureListener->NamespaceUris;
    cContext.AlwaysCheckLengths = OPCUA_SERIALIZER_CHECKLENGTHS;

    /* open decoder */
    uStatus = pDecoder->Open(pDecoder, a_pIstrm, &cContext, &hDecodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* decode message */
    uStatus = pDecoder->ReadMessage((struct _OpcUa_Decoder*)hDecodeContext, &pRequestType, a_ppRequest);
    OpcUa_GotoErrorIfBad(uStatus);

    if(a_expectedTypeId != 0 && pRequestType->TypeId != a_expectedTypeId)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadUnexpectedError);
    }

    if(a_ppRequestType != OpcUa_Null)
    {
        *a_ppRequestType = pRequestType;
    }

    /* close decoder */
    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);
    OpcUa_EncodeableObject_Delete(pRequestType, a_ppRequest);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_WriteResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_WriteResponse( OpcUa_SecureListener*   a_pSecureListener,
                                                            OpcUa_OutputStream**    a_ppOstrm,
                                                            OpcUa_Void*             a_pResponse,
                                                            OpcUa_EncodeableType*   a_pResponseType)
{
    OpcUa_Encoder*          pEncoder        = OpcUa_Null;
    OpcUa_MessageContext    cContext;
    OpcUa_Handle            hEncodeContext  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "WriteResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pResponse);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponse);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseType);

    pEncoder = a_pSecureListener->Encoder;
    OpcUa_ReturnErrorIfArgumentNull(pEncoder);

    OpcUa_MessageContext_Initialize(&cContext);

    cContext.KnownTypes         = a_pSecureListener->KnownTypes;
    cContext.NamespaceUris      = a_pSecureListener->NamespaceUris;
    cContext.AlwaysCheckLengths = OPCUA_SERIALIZER_CHECKLENGTHS;

    /* open encoder */
    uStatus = pEncoder->Open(pEncoder, *a_ppOstrm, &cContext, &hEncodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* encode message */
    uStatus = pEncoder->WriteMessage((struct _OpcUa_Encoder*)hEncodeContext, a_pResponse, a_pResponseType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* delete encoder */
    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_SendOpenSecureChannelResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureListener_SendOpenSecureChannelResponse(
    OpcUa_Listener*                     a_pListener,
    OpcUa_SecureChannel**               a_ppSecureChannel,
    OpcUa_CryptoProvider*               a_pCryptoProvider,
    OpcUa_InputStream*                  a_pSecureIstrm,
    OpcUa_String*                       a_pSecurityPolicyUri,
    OpcUa_ChannelSecurityToken*         a_pSecurityToken,
    OpcUa_Key*                          a_pServerNonce,
    OpcUa_OpenSecureChannelRequest*     a_pRequest,
    OpcUa_ByteString*                   a_pClientCertificate)
{
    OpcUa_OutputStream*                 pSecureOstrm                = OpcUa_Null;
    OpcUa_SecureListener*               pSecureListener             = (OpcUa_SecureListener*)a_pListener->Handle;
    OpcUa_SecureChannel*                pSecureChannel              = *a_ppSecureChannel;
    OpcUa_OpenSecureChannelResponse*    pResponse                   = OpcUa_Null;
    OpcUa_ByteString                    clientCertificateThumbprint = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_MessageSecurityMode           eSecurityMode               = OpcUa_MessageSecurityMode_None;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "SendOpenSecureChannelResponse");

    if(a_pClientCertificate->Length > 0)
    {
        /*** get the client's certificate thumbprint for the OpenSecureChannelResponse header ***/
        uStatus = a_pCryptoProvider->GetCertificateThumbprint(  a_pCryptoProvider,
                                                                a_pClientCertificate,
                                                                &clientCertificateThumbprint);
        OpcUa_GotoErrorIfBad(uStatus);

        if(clientCertificateThumbprint.Length > 0)
        {
            clientCertificateThumbprint.Data = (OpcUa_Byte*)OpcUa_Alloc(clientCertificateThumbprint.Length*sizeof(OpcUa_Byte));
            OpcUa_GotoErrorIfAllocFailed(clientCertificateThumbprint.Data);

            uStatus = a_pCryptoProvider->GetCertificateThumbprint(  a_pCryptoProvider,
                                                                    a_pClientCertificate,
                                                                    &clientCertificateThumbprint);
            OpcUa_GotoErrorIfBad(uStatus);
        }
        else
        {
            uStatus = OpcUa_Bad;
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }

    /* must always sign and encrypt open secure channel requests. */
    eSecurityMode = pSecureChannel->MessageSecurityMode;

    if(eSecurityMode != OpcUa_MessageSecurityMode_None)
    {
        eSecurityMode = OpcUa_MessageSecurityMode_SignAndEncrypt;
    }

    /* start response */
    uStatus = OpcUa_SecureListener_BeginSendOpenSecureChannelResponse(
        a_pListener,
        pSecureChannel,
        a_pSecureIstrm,
        a_pSecurityPolicyUri,
        eSecurityMode,
        a_pCryptoProvider,
        pSecureListener->pServerCertificate,
        &clientCertificateThumbprint,
        &pSecureOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* pSecureChannel is now owned by pSecureOstrm */
    *a_ppSecureChannel = OpcUa_Null;

    /*** fill in OpenSecureChannelResponse ***/
    uStatus = OpcUa_EncodeableObject_Create(    &OpcUa_OpenSecureChannelResponse_EncodeableType,
                                                (OpcUa_Void**)&pResponse);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** fill response header ***/
    OpcUa_OpenSecureChannelResponse_Initialize(pResponse);

    /* the same session id is used like in the request */
    pResponse->ResponseHeader.RequestHandle             = a_pRequest->RequestHeader.RequestHandle;
    pResponse->ResponseHeader.Timestamp                 = OPCUA_P_DATETIME_UTCNOW();

    OpcUa_ExtensionObject_Initialize(&pResponse->ResponseHeader.AdditionalHeader);
    pResponse->ResponseHeader.ServiceResult             = OpcUa_Good;

    /*** fill response body ***/

    pResponse->ServerProtocolVersion = 0;

    /* copy byte string content and clear pointers in source */
    pResponse->ServerNonce = a_pServerNonce->Key;
    OpcUa_ByteString_Initialize(&(a_pServerNonce->Key));

    OpcUa_MemCpy(   &pResponse->SecurityToken,
                    sizeof(OpcUa_ChannelSecurityToken),
                    a_pSecurityToken,
                    sizeof(OpcUa_ChannelSecurityToken));

    /*** encode response body ***/
    uStatus = OpcUa_SecureListener_WriteResponse(   pSecureListener,
                                                    &pSecureOstrm,
                                                    (OpcUa_Void*)pResponse,
                                                    &OpcUa_OpenSecureChannelResponse_EncodeableType);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** end send response ***/
    uStatus = OpcUa_SecureListener_EndSendOpenSecureChannelResponse(    a_pListener,
                                                                        &pSecureOstrm,
                                                                        a_pCryptoProvider,
                                                                        a_pClientCertificate,
                                                                        pSecureListener->pServerCertificate,
                                                                        pSecureChannel->MessageSecurityMode,
                                                                        uStatus);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pResponse != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelResponse_Clear(pResponse);
        OpcUa_Free(pResponse);
    }

    OpcUa_ByteString_Clear(&clientCertificateThumbprint);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureOstrm != OpcUa_Null)
    {
        OpcUa_Stream_Delete(&((OpcUa_SecureStream*)pSecureOstrm->Handle)->InnerStrm);
        pSecureOstrm->Delete((OpcUa_Stream**)&pSecureOstrm);
    }

    if(pResponse != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelResponse_Clear(pResponse);
        OpcUa_Free(pResponse);
    }

    OpcUa_ByteString_Clear(&clientCertificateThumbprint);

OpcUa_FinishErrorHandling;
}

/*============================================================================
* OpcUa_SecureListener_ValidateCertificate
*===========================================================================*/
/** @brief Internal helper for the certificate validation process. */
static OpcUa_StatusCode OpcUa_SecureListener_ValidateCertificate(   OpcUa_SecureListener* a_pSecureListener,
                                                                    OpcUa_ByteString*     a_pCertificate)
{
    OpcUa_Int           iValidationCode     = 0;
    OpcUa_PKIProvider*  pPkiProvider        = (OpcUa_PKIProvider*)a_pSecureListener->ServerPKIProvider;
    OpcUa_Void*         pCertificateStore   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ValidateCertificate");

    /* open certificate store */
    uStatus = pPkiProvider->OpenCertificateStore(   pPkiProvider,
                                                    &pCertificateStore);

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_ValidateCertificate: Could not open certificate store!\n");

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

    /* validate certificate */
    uStatus = pPkiProvider->ValidateCertificate(pPkiProvider,
                                                a_pCertificate,
                                                pCertificateStore,
                                                &iValidationCode);

    /* mask valid errorcode */
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_ValidateCertificate: Validation failed with 0x%08X\n", uStatus);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ValidateCertificate: success\n");
    }

    /* close certificate store */
    pPkiProvider->CloseCertificateStore(pPkiProvider,
                                        &pCertificateStore);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
* OpcUa_SecureListener_ProcessOpenSecureChannelRequest
*===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ProcessOpenSecureChannelRequest(
    OpcUa_Listener*     a_pSecureListenerInterface,
    OpcUa_Handle        a_hTransportConnection,
    OpcUa_InputStream** a_ppTransportIstrm,
    OpcUa_Boolean       a_bRequestComplete)
{
    OpcUa_SecureListener*                               pSecureListener                 = OpcUa_Null;
    OpcUa_InputStream*                                  pSecureIStrm                    = OpcUa_Null;
    OpcUa_SecureStream*                                 pSecureStream                   = OpcUa_Null;
    OpcUa_SecureChannel*                                pSecureChannel                  = OpcUa_Null;
    OpcUa_CryptoProvider*                               pCryptoProvider                 = OpcUa_Null;
    OpcUa_UInt32                                        uSecureChannelId                = OPCUA_SECURECHANNEL_ID_INVALID;

    OpcUa_String                                        sSecurityPolicyUri              = OPCUA_STRING_STATICINITIALIZER;
    OpcUa_StringA                                       szPolicyUri                     = OpcUa_Null;

    OpcUa_ByteString                                    SenderCertificate               = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_ByteString                                    ReceiverCertificateThumbprint   = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_Key                                           serverNonce;
    OpcUa_ChannelSecurityToken*                         pSecurityToken                  = OpcUa_Null;

    OpcUa_Boolean                                       bRenewChannel                   = OpcUa_False;
    OpcUa_OpenSecureChannelRequest*                     pRequest                        = OpcUa_Null;

    OpcUa_SecurityKeyset*                               pReceivingKeyset                = OpcUa_Null;
    OpcUa_SecurityKeyset*                               pSendingKeyset                  = OpcUa_Null;
    OpcUa_MessageSecurityMode                           eRequestedSecurityMode          = OpcUa_MessageSecurityMode_Invalid;
    OpcUa_Key*                                          pSigningKey                     = OpcUa_Null;
    OpcUa_Key*                                          pEncryptionKey                  = OpcUa_Null;

    OpcUa_SecureListener_SecureChannelEvent             eSecureChannelEvent             = eOpcUa_SecureListener_SecureChannelUnkown;
    OpcUa_SecureListener_SecurityPolicyConfiguration    secConfig;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ProcessOpenSecureChannelRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListenerInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIstrm);

    OpcUa_Key_Initialize(&serverNonce);

    /*** get listener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pSecureListenerInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    /* parse stream header */
    uStatus = OpcUa_SecureStream_DecodeAsymmetricSecurityHeader(*a_ppTransportIstrm,
                                                                &uSecureChannelId,
                                                                &sSecurityPolicyUri,
                                                                &SenderCertificate,
                                                                &ReceiverCertificateThumbprint);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                    "ProcessOpenSecureChannelRequest: SID %u, SURI \"%*.*s\"\n",
                    uSecureChannelId,
                    OpcUa_String_StrLen(&sSecurityPolicyUri),
                    OpcUa_String_StrLen(&sSecurityPolicyUri),
                    OpcUa_String_GetRawString(&sSecurityPolicyUri));

    OpcUa_MemSet(&secConfig, 0, sizeof(OpcUa_SecureListener_SecurityPolicyConfiguration));

    /* if security is turned on then the sender and receiver certificates must be specified. */
    if((SenderCertificate.Data != OpcUa_Null) && (ReceiverCertificateThumbprint.Data != OpcUa_Null))
    {
        secConfig.uMessageSecurityModes = OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_SIGNANDENCRYPT;
        eRequestedSecurityMode = OpcUa_MessageSecurityMode_SignAndEncrypt;
    }
    /* if security is turned off then the sender and receiver certificates must be null. */
    else if((SenderCertificate.Data == OpcUa_Null) && (ReceiverCertificateThumbprint.Data == OpcUa_Null))
    {
        secConfig.uMessageSecurityModes = OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_NONE;
        eRequestedSecurityMode = OpcUa_MessageSecurityMode_None;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelRequest: Invalid sender and/or receiver certificates specified!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    /* not very nice */
    OpcUa_MemCpy(&secConfig.sSecurityPolicy, sizeof(OpcUa_String), &sSecurityPolicyUri, sizeof(OpcUa_String));

    if(uSecureChannelId == OPCUA_SECURECHANNEL_ID_INVALID)
    {
        uStatus = OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(  pSecureListener->ChannelManager,
                                                                                        a_hTransportConnection,
                                                                                        &pSecureChannel);
        OpcUa_GotoErrorIfBad(uStatus);

        if(pSecureChannel->SecureChannelId != OPCUA_SECURECHANNEL_ID_INVALID)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_ProcessOpenSecureChannelRequest: SecureChannel id already assigned!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
        }

        bRenewChannel = OpcUa_False;
    }
    else
    {
        uStatus = OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(  pSecureListener->ChannelManager,
                                                                                    uSecureChannelId,
                                                                                    &pSecureChannel);
        OpcUa_GotoErrorIfBad(uStatus);

        if(pSecureChannel->TransportConnection != a_hTransportConnection)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_ProcessOpenSecureChannelRequest: SecureChannel id used by wrong transport connection!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
        }

        bRenewChannel = OpcUa_True;
    }

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(pSecureChannel,
                                                        &pSecureIStrm);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pSecureIStrm == OpcUa_Null)
    {
        /** check for a valid security policy ***/
        uStatus = OpcUa_SecureListener_PolicyManager_IsValidSecurityPolicy(
            pSecureListener->PolicyManager,
            &secConfig.sSecurityPolicy);

        if(OpcUa_IsBad(uStatus))
        {
            /* check if a discovery only channel can be openned */
            OpcUa_Boolean discoveryOnly = OpcUa_False;

            if(OpcUa_String_IsNull(&pSecureChannel->SecurityPolicyUri))
            {
                OpcUa_StatusCode    uStatusTemp = OpcUa_Good;
                OpcUa_String        sPolicyNone = OPCUA_STRING_STATICINITIALIZER;

                uStatusTemp = OpcUa_String_AttachReadOnly(&sPolicyNone, OpcUa_SecurityPolicy_None);
                OpcUa_GotoErrorIfTrue(OpcUa_IsBad(uStatusTemp), uStatusTemp);

                if((OpcUa_String_StrnCmp(&sPolicyNone, &sSecurityPolicyUri, OPCUA_STRING_LENDONTCARE, OpcUa_False) == 0))
                {
                    discoveryOnly = OpcUa_True;
                }
            }

            if(!discoveryOnly)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelRequest: Invalid Policy Configuration requested!\n");
                OpcUa_GotoError;
            }

            pSecureChannel->DiscoveryOnly = OpcUa_True;

            uStatus = OpcUa_String_StrnCpy( &pSecureChannel->SecurityPolicyUri,
                                            &sSecurityPolicyUri,
                                            OPCUA_STRING_LENDONTCARE);
            OpcUa_GotoErrorIfBad(uStatus);

            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelRequest: Discovery Only Channel Created\n");
        }

        if(bRenewChannel)
        {
            /* check if the security policy is being changed. */
            if((OpcUa_String_StrnCmp(&pSecureChannel->SecurityPolicyUri, &sSecurityPolicyUri, OPCUA_STRING_LENDONTCARE, OpcUa_False) != 0))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityPolicyRejected);
            }

            /* must check that the client certificate is not changed. */
            if(pSecureChannel->ClientCertificate.Length > 0)
            {
                if(SenderCertificate.Length != pSecureChannel->ClientCertificate.Length)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid);
                }

                if(OpcUa_MemCmp(SenderCertificate.Data, pSecureChannel->ClientCertificate.Data, SenderCertificate.Length) != 0)
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
        else /* assign the security policy to the channel */
        {
            uStatus = OpcUa_String_StrnCpy( &pSecureChannel->SecurityPolicyUri,
                                            &sSecurityPolicyUri,
                                            OPCUA_STRING_LENDONTCARE);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        /*** create CryptoProvider ***/
        pCryptoProvider = (OpcUa_CryptoProvider*)OpcUa_Alloc(sizeof(OpcUa_CryptoProvider));
        OpcUa_GotoErrorIfAllocFailed(pCryptoProvider);

        szPolicyUri = OpcUa_String_GetRawString(&sSecurityPolicyUri);

        uStatus = OPCUA_P_CRYPTOFACTORY_CREATECRYPTOPROVIDER(szPolicyUri, pCryptoProvider);
        OpcUa_GotoErrorIfBad(uStatus);

        if((eRequestedSecurityMode == OpcUa_MessageSecurityMode_SignAndEncrypt) ||
           (eRequestedSecurityMode == OpcUa_MessageSecurityMode_Sign))
        {
            /*** validate certificate ***/
            uStatus = OpcUa_SecureListener_ValidateCertificate( pSecureListener,
                                                                &SenderCertificate);

            if(OpcUa_IsBad(uStatus))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelRequest: Client Certificate could not be validated! (0x%08X)\n", uStatus);

                if(pSecureListener->SecureChannelCallback != OpcUa_Null)
                {
                    OpcUa_String sTempUri;
                    OpcUa_String_Initialize(&sTempUri);
                    OpcUa_String_AttachReadOnly(&sTempUri, szPolicyUri);

                    pSecureListener->SecureChannelCallback( uSecureChannelId,
                                                            eOpcUa_SecureListener_SecureChannelOpen,
                                                            uStatus,
                                                            &SenderCertificate,
                                                            &sTempUri,
                                                            secConfig.uMessageSecurityModes,
                                                            pSecureListener->SecureChannelCallbackData);

                    /* return general status code instead of specific code (needed for CTT) */
                    switch(uStatus)
                    {
                        case OpcUa_BadCertificateUntrusted:
                        case OpcUa_BadCertificateRevoked:
                        case OpcUa_BadCertificateIssuerRevoked:
                        case OpcUa_BadCertificateRevocationUnknown:
                        case OpcUa_BadCertificateIssuerRevocationUnknown:
                            uStatus = OpcUa_BadSecurityChecksFailed;
                    }

                }

                OpcUa_GotoError;
            }

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "ProcessOpenSecureChannelRequest: Client Certificate validated! (0x%08X)\n", uStatus);


            /** create server nonce **/
            OpcUa_ByteString_Initialize(&serverNonce.Key);

            /* get the length for memory allocation */
            uStatus = pCryptoProvider->GenerateKey(pCryptoProvider, -1, &serverNonce);
            OpcUa_GotoErrorIfBad(uStatus);

            serverNonce.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(serverNonce.Key.Length);
            OpcUa_GotoErrorIfAllocFailed(serverNonce.Key.Data);

            /* generate ServerNonce */
            uStatus = pCryptoProvider->GenerateKey( pCryptoProvider, serverNonce.Key.Length, &serverNonce);
            OpcUa_GotoErrorIfBad(uStatus);
        }
        else
        {
            /* create fake server nonce */

            /* generate fake server nonce that is sent to the client */
            serverNonce.Type            = OpcUa_Crypto_KeyType_Random;
            serverNonce.Key.Length      = -1;
            serverNonce.Key.Data        = OpcUa_Null;
            serverNonce.fpClearHandle   = OpcUa_Null;
        }

        /* create inputstream and check if processing can be started or delayed until last chunk is received. */
        uStatus = OpcUa_SecureStream_CreateOpenSecureChannelInput(  pCryptoProvider,
                                                                    eRequestedSecurityMode,
                                                                    pSecureListener->pServerCertificate,
                                                                    &pSecureListener->ServerPrivateKey,
                                                                    &SenderCertificate,
                                                                    &ReceiverCertificateThumbprint,
                                                                    1,
                                                                    &pSecureIStrm);
        OpcUa_GotoErrorIfBad(uStatus);

        pSecureStream  = (OpcUa_SecureStream*)pSecureIStrm->Handle;
        pSigningKey    = pSecureStream->pSenderPublicKey;
        pEncryptionKey = &pSecureListener->ServerPrivateKey;

        uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportIstrm,
                                                    pSecureIStrm,
                                                    pSigningKey,    /* (pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel) ? pSecureStream->pSenderPublicKey : &pSecureStream->pKeyset->SigningKey */
                                                    pEncryptionKey, /* (pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel) ? pSecureStream->pPrivateKey : &pSecureStream->pKeyset->EncryptionKey */
                                                    OpcUa_Null,
                                                    pCryptoProvider,
                                                    pSecureChannel);
        OpcUa_GotoErrorIfBad(uStatus);

        /* transport stream is unlinked; given handle can be invalidated */
        *a_ppTransportIstrm = OpcUa_Null;

        pSecureStream                   = (OpcUa_SecureStream*)pSecureIStrm->Handle;
        pSecureStream->SecureChannelId  = uSecureChannelId;
        pSecureStream->eMessageType     = eOpcUa_SecureStream_Types_StandardMessage;
    }
    else /* we had a pending input stream */
    {
        /* Reset the pending request stream. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(pSecureChannel,
                                                            OpcUa_Null);

        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }

    /* look if we can start processing the stream */
    if(a_bRequestComplete == OpcUa_False)
    {
        /* Reset the pending request stream. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(pSecureChannel,
                                                            OpcUa_Null);

        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }
    else /* preprocess the stream */
    {
        /* Reset the pending request stream. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(pSecureChannel,
                                                            OpcUa_Null);

        /* reset buffer index to start reading from the first buffer */
        pSecureStream->nCurrentReadBuffer = 0;

        /*** read request ***/
        uStatus = OpcUa_SecureListener_ReadRequest( pSecureListener,
                                                    pSecureIStrm,
                                                    OpcUaId_OpenSecureChannelRequest,
                                                    (OpcUa_Void**)&pRequest,
                                                    OpcUa_Null);

        if((pSecureChannel->DiscoveryOnly != OpcUa_False) &&
           (OpcUa_IsBad(uStatus) || pRequest->RequestHeader.AdditionalHeader.Encoding != OpcUa_ExtensionObjectEncoding_None))
        {
            uStatus = OpcUa_BadServiceUnsupported;
        }

        OpcUa_GotoErrorIfBad(uStatus);

        /* check that the ClientProtocolVersion matches the ClientHello message */
        uStatus = OpcUa_Listener_CheckProtocolVersion(pSecureListener->TransportListener,
                                                      a_hTransportConnection,
                                                      pRequest->ClientProtocolVersion);
        OpcUa_GotoErrorIfBad(uStatus);

        /* check that the message has been properly secured */
        if(pRequest->SecurityMode != OpcUa_MessageSecurityMode_None)
        {
            if(secConfig.uMessageSecurityModes != OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_SIGNANDENCRYPT)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityChecksFailed);
            }
        }

        /* verify request type and security mode. */
        if(bRenewChannel)
        {
            if(pRequest->RequestType != OpcUa_SecurityTokenRequestType_Renew)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadRequestTypeInvalid);
            }

            if(pRequest->SecurityMode != pSecureChannel->MessageSecurityMode)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityPolicyRejected);
            }

            /* renew SecurityToken */
            uStatus = pSecureChannel->RenewSecurityToken(   pSecureChannel,
                                                            &pSecureChannel->CurrentChannelSecurityToken,
                                                            pRequest->RequestedLifetime,
                                                            &pSecurityToken);
            OpcUa_GotoErrorIfBad(uStatus);

            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelRequest: Renew: Revised Lifetime of channel %u from %u to %u ms!\n", pSecureChannel->SecureChannelId, pRequest->RequestedLifetime, pSecurityToken->RevisedLifetime);
        }
        else
        {
            if(pRequest->RequestType != OpcUa_SecurityTokenRequestType_Issue)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadRequestTypeInvalid);
            }

            /* update and validate the security mode. */
            switch(pRequest->SecurityMode)
            {
                case OpcUa_MessageSecurityMode_None:
                {
                    secConfig.uMessageSecurityModes = OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_NONE;
                    break;
                }

                case OpcUa_MessageSecurityMode_Sign:
                {
                    secConfig.uMessageSecurityModes = OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_SIGN;
                    break;
                }

                case OpcUa_MessageSecurityMode_SignAndEncrypt:
                {
                    secConfig.uMessageSecurityModes = OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_SIGNANDENCRYPT;
                    break;
                }
                default:
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityModeRejected);
                }
            }

            uStatus = OpcUa_SecureListener_PolicyManager_IsValidSecurityPolicyConfiguration(
                pSecureListener->PolicyManager,
                &secConfig);

            if(pSecureChannel->DiscoveryOnly == OpcUa_False)
            {
                OpcUa_GotoErrorIfBad(uStatus);
            }

            pSecureChannel->MessageSecurityMode = pRequest->SecurityMode;

            /*** new securechannel ***/
            OpcUa_SecureListener_ChannelManager_SetSecureChannelID(pSecureListener->ChannelManager,
                                                                   pSecureChannel,
                                                                   pSecureListener->uNextSecureChannelId++);

            /* skip 0 or duplicate channel ID */
            while(OpcUa_IsBad(OpcUa_SecureListener_ChannelManager_IsValidChannelID(
                                                                   pSecureListener->ChannelManager,
                                                                   pSecureListener->uNextSecureChannelId)))
            {
                pSecureListener->uNextSecureChannelId++;
            }

            /* generate SecurityToken */
            uStatus = pSecureChannel->GenerateSecurityToken(pSecureChannel,
                                                            pRequest->RequestedLifetime,
                                                            &pSecurityToken);
            OpcUa_GotoErrorIfBad(uStatus);

            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "ProcessOpenSecureChannelRequest: Open: Revised Lifetime of Channel %u from %u to %u ms!\n", pSecureChannel->SecureChannelId, pRequest->RequestedLifetime, pSecurityToken->RevisedLifetime);
        }

        /*** derive channel keys ***/
        uStatus = OpcUa_SecureChannel_DeriveKeys(   pSecureChannel->MessageSecurityMode,
                                                    pCryptoProvider,
                                                    &pRequest->ClientNonce,
                                                    &serverNonce.Key,
                                                    &pReceivingKeyset, /* Client key is used for receiving on this side. */
                                                    &pSendingKeyset);  /* Server key is used for sending on this side.*/
        OpcUa_GotoErrorIfBad(uStatus);

        /* check whether new or existing securechannel */
        if(bRenewChannel)
        {
            /* renew SecureChannel */
            uStatus = pSecureChannel->Renew(pSecureChannel,
                                            a_hTransportConnection,
                                            *pSecurityToken,
                                            pRequest->SecurityMode,
                                            &SenderCertificate,
                                            pSecureListener->pServerCertificate,
                                            pReceivingKeyset, /* Client key set is used for receiving on this side. */
                                            pSendingKeyset,   /* Server key set is used for receiving on this side. */
                                            pCryptoProvider);
            OpcUa_GotoErrorIfBad(uStatus);

            eSecureChannelEvent = eOpcUa_SecureListener_SecureChannelRenew;
        }
        else
        {
            /* open SecureChannel */
            uStatus = pSecureChannel->Open( pSecureChannel,
                                            a_hTransportConnection,
                                            *pSecurityToken,
                                            pRequest->SecurityMode,
                                            &SenderCertificate,
                                            pSecureListener->pServerCertificate,
                                            pReceivingKeyset, /* Client key set is used for receiving on this side. */
                                            pSendingKeyset,   /* Server key set is used for receiving on this side. */
                                            pCryptoProvider);
            OpcUa_GotoErrorIfBad(uStatus);

            eSecureChannelEvent = eOpcUa_SecureListener_SecureChannelOpen;
        }

        if(pSecureListener->SecureChannelCallback != OpcUa_Null)
        {
            OpcUa_String sTempUri;
            OpcUa_String_Initialize(&sTempUri);
            OpcUa_String_AttachReadOnly(&sTempUri, szPolicyUri);

            pSecureListener->SecureChannelCallback( pSecureChannel->SecureChannelId,
                                                    eSecureChannelEvent,
                                                    uStatus,
                                                    &SenderCertificate,
                                                    &sTempUri,
                                                    secConfig.uMessageSecurityModes,
                                                    pSecureListener->SecureChannelCallbackData);
        }

        /* send response */
        uStatus = OpcUa_SecureListener_SendOpenSecureChannelResponse(   a_pSecureListenerInterface,
                                                                        &pSecureChannel,
                                                                        pCryptoProvider,
                                                                        pSecureIStrm,
                                                                        &sSecurityPolicyUri,
                                                                        pSecurityToken,
                                                                        &serverNonce,
                                                                        pRequest,
                                                                        &SenderCertificate);

        OpcUa_ByteString_Clear(&SenderCertificate);
        OpcUa_ByteString_Clear(&ReceiverCertificateThumbprint);
        OpcUa_Key_Clear(&serverNonce);
    } /* if(a_bRequestComplete == OpcUa_False) */

    /*** clean up ***/
    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    if(pRequest != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

    if(pSecurityToken != OpcUa_Null)
    {
        OpcUa_ChannelSecurityToken_Clear(pSecurityToken);
        OpcUa_Free(pSecurityToken);
    }

    if(pSecureStream != OpcUa_Null && pSecureStream->InnerStrm != OpcUa_Null)
    {
        pSecureStream->InnerStrm->Delete(&pSecureStream->InnerStrm);
    }

    if(pSecureIStrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pSecureIStrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIStrm);
    }

    OpcUa_String_Clear(&sSecurityPolicyUri);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_ByteString_Clear(&SenderCertificate);
    OpcUa_ByteString_Clear(&ReceiverCertificateThumbprint);
    OpcUa_Key_Clear(&serverNonce);

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    if(pRequest != OpcUa_Null)
    {
        OpcUa_OpenSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

    if(pSecurityToken != OpcUa_Null)
    {
        OpcUa_Free(pSecurityToken);
    }

    if(pCryptoProvider)
    {
        OPCUA_P_CRYPTOFACTORY_DELETECRYPTOPROVIDER(pCryptoProvider);
        OpcUa_Free(pCryptoProvider);
    }

    if(pSecureStream != OpcUa_Null && pSecureStream->InnerStrm != OpcUa_Null)
    {
        pSecureStream->InnerStrm->Delete(&pSecureStream->InnerStrm);
        *a_ppTransportIstrm = OpcUa_Null;
    }

    if(pSecureIStrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pSecureIStrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIStrm);
    }

    if(a_ppTransportIstrm != OpcUa_Null && (*a_ppTransportIstrm) != OpcUa_Null)
    {
        (*a_ppTransportIstrm)->Close((OpcUa_Stream*)(*a_ppTransportIstrm));
        (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);
    }

    OpcUa_String_Clear(&sSecurityPolicyUri);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_ProcessCloseSecureChannelRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ProcessCloseSecureChannelRequest(
    OpcUa_Listener*                     a_pSecureListenerInterface,
    OpcUa_Handle                        a_hTransportConnection,
    OpcUa_InputStream**                 a_ppTransportIstrm,
    OpcUa_Boolean                       a_bRequestComplete)
{
    OpcUa_SecureListener*               pSecureListener         = OpcUa_Null;
    OpcUa_InputStream*                  pSecureIstrm            = OpcUa_Null;
    OpcUa_SecureStream*                 pSecureStream           = OpcUa_Null;
    OpcUa_SecureChannel*                pSecureChannel          = OpcUa_Null;
    OpcUa_CryptoProvider*               pCryptoProvider         = OpcUa_Null;

    OpcUa_CloseSecureChannelRequest*    pRequest                = OpcUa_Null;

    OpcUa_UInt32                        uTokenId                = 0;
    OpcUa_UInt32                        uSecureChannelId        = 0;

    OpcUa_SecurityKeyset*               pReceivingKeyset        = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ProcessCloseSecureChannelRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListenerInterface);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIstrm);

    /*** get listener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pSecureListenerInterface->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    /* parse stream header */
    uStatus = OpcUa_SecureStream_DecodeSymmetricSecurityHeader( *a_ppTransportIstrm,
                                                                &uSecureChannelId,
                                                                &uTokenId);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                    "ProcessCloseSecureChannelRequest: SID %u, TID %u\n",
                    uSecureChannelId,
                    uTokenId);

    /*** find appropriate channel in channel manager or create CryptoProvider with SecurityPolicy ***/
    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(  pSecureListener->ChannelManager,
                                                                                uSecureChannelId,
                                                                                &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pSecureChannel->TransportConnection != a_hTransportConnection)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(    pSecureChannel,
                                                            &pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Get reference to keyset for requested token id */
    uStatus = pSecureChannel->GetSecuritySet(   pSecureChannel,
                                                uTokenId,
                                                &pReceivingKeyset,
                                                OpcUa_Null,
                                                &pCryptoProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pSecureIstrm == OpcUa_Null)
    {
        /* create inputstream and check if processing can be started or delayed until last chunk is received. */
        uStatus = OpcUa_SecureStream_CreateInput(   pCryptoProvider,
                                                    pSecureChannel->MessageSecurityMode,
                                                    pSecureChannel->DiscoveryOnly ? OPCUA_SECURELISTENER_DISCOVERY_MAXCHUNKS :
                                                    pSecureChannel->nMaxBuffersPerMessage,
                                                    &pSecureIstrm);
        if(OpcUa_IsBad(uStatus))
        {
            pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                                  uTokenId);
            OpcUa_GotoError;
        }

        /* append keep the secure stream pending and waiting for further chunks */
        uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportIstrm,
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

        /* delete transport stream */
        (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);

        pSecureStream                       = (OpcUa_SecureStream*)pSecureIstrm->Handle;
        pSecureStream->SecureChannelId      = uSecureChannelId;
        pSecureStream->eMessageType         = eOpcUa_SecureStream_Types_StandardMessage;
    }
    else
    {
        /* append keep the secure stream pending and waiting for further chunks */
        uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportIstrm,
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

        pSecureStream = (OpcUa_SecureStream*)pSecureIstrm->Handle;

        /* buffer content is saved, instream can be deleted */
        (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);
    }

    /* look if we can start processing the stream */
    if(a_bRequestComplete == OpcUa_False)
    {
        /* Set the current stream as the pending request stream and leave. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(pSecureChannel,
                                                            pSecureIstrm);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "ProcessCloseSecureChannelRequest: Waiting for more chunks!\n");

        pSecureIstrm = OpcUa_Null;
    }
    else /* preprocess the stream */
    {
        /* Reset the pending request stream. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(pSecureChannel,
                                                            OpcUa_Null);

        /* get uTokenId */
        if(uTokenId != pSecureChannel->CurrentChannelSecurityToken.TokenId)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "ProcessCloseSecureChannelRequest: TokenId does not match!\n");
        }

        /* reset buffer index to start reading from the first buffer */
        pSecureStream->nCurrentReadBuffer = 0;

        /*** read encoded request ***/
        uStatus = OpcUa_SecureListener_ReadRequest( pSecureListener,
                                                    pSecureIstrm,
                                                    OpcUaId_CloseSecureChannelRequest,
                                                    (OpcUa_Void**)&pRequest,
                                                    OpcUa_Null);

        if((pSecureChannel->DiscoveryOnly != OpcUa_False) &&
           (OpcUa_IsBad(uStatus) || pRequest->RequestHeader.AdditionalHeader.Encoding != OpcUa_ExtensionObjectEncoding_None))
        {
            uStatus = OpcUa_BadServiceUnsupported;
        }

        OpcUa_GotoErrorIfBad(uStatus);

        /*** invoke callback ***/
        if(pSecureListener->SecureChannelCallback)
        {
            pSecureListener->SecureChannelCallback( pSecureChannel->SecureChannelId,
                                                    eOpcUa_SecureListener_SecureChannelClose,
                                                    OpcUa_Good,
                                                    OpcUa_Null,
                                                    OpcUa_Null,
                                                    0,
                                                    pSecureListener->SecureChannelCallbackData);
        }

        /* close the SecureChannel */
        uStatus = pSecureChannel->Close(pSecureChannel);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /*** clean up ***/
    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    if(pRequest != OpcUa_Null)
    {
        OpcUa_CloseSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pSecureIstrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIstrm);
    }

#if OPCUA_SECURELISTENER_CLOSE_CONNECTION_ON_CLOSE_SECURECHANNEL
    /* transport layer closes the connection on this status code */
    uStatus = OpcUa_BadDisconnect;
#endif /* OPCUA_SECURELISTENER_CLOSE_CONNECTION_ON_CLOSE_SECURECHANNEL */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    if(pRequest != OpcUa_Null)
    {
        OpcUa_CloseSecureChannelRequest_Clear(pRequest);
        OpcUa_Free(pRequest);
    }

    if(a_ppTransportIstrm != OpcUa_Null && (*a_ppTransportIstrm) != OpcUa_Null)
    {
        (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);
    }

    if(pSecureIstrm != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pSecureIstrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIstrm);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_ValidateDiscoveryChannel
 *===========================================================================*/
#if OPCUA_SECURELISTENER_DISCOVERY_MAXCHUNKS == 1
OpcUa_StatusCode OpcUa_SecureListener_ValidateDiscoveryChannel(OpcUa_SecureListener* a_pSecureListener,
                                                               OpcUa_InputStream*    a_pSecureIstrm)
{
    OpcUa_UInt32          uPosition    = 0;
    OpcUa_Void*           pRequest     = OpcUa_Null;
    OpcUa_EncodeableType* pRequestType = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ValidateDiscoveryChannel");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureIstrm);

    uStatus = OpcUa_Stream_GetPosition((OpcUa_Stream*)a_pSecureIstrm, &uPosition);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** read request ***/
    uStatus = OpcUa_SecureListener_ReadRequest( a_pSecureListener,
                                                a_pSecureIstrm,
                                                0,
                                                &pRequest,
                                                &pRequestType);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadServiceUnsupported);
    }

    if(pRequestType->TypeId != OpcUaId_GetEndpointsRequest
       && pRequestType->TypeId != OpcUaId_FindServersRequest
#if OPCUA_SECURELISTENER_DISCOVERY_ALLOW_FSON
       && pRequestType->TypeId != OpcUaId_FindServersOnNetworkRequest
#endif
       )
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadServiceUnsupported);
    }

    if(((OpcUa_RequestHeader*)pRequest)->AdditionalHeader.Encoding != OpcUa_ExtensionObjectEncoding_None)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadServiceUnsupported);
    }

    uStatus = OpcUa_Stream_SetPosition((OpcUa_Stream*)a_pSecureIstrm, uPosition);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_EncodeableObject_Delete(pRequestType, &pRequest);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_EncodeableObject_Delete(pRequestType, &pRequest);

OpcUa_FinishErrorHandling;
}
#else /* OPCUA_SECURELISTENER_DISCOVERY_MAXCHUNKS > 1 */
#define OpcUa_MakeFourByteNodeId(x) (0x00000001 | (((x)&0x0000FFFF)<<16))

OpcUa_StatusCode OpcUa_SecureListener_ValidateDiscoveryChannel(OpcUa_SecureListener* a_pSecureListener,
                                                               OpcUa_InputStream*    a_pSecureIstrm)
{
    OpcUa_UInt32 uTypeId = 0;
    OpcUa_UInt32 uPosition = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ValidateDiscoveryChannel");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureIstrm);

    uStatus = OpcUa_Stream_GetPosition((OpcUa_Stream*)a_pSecureIstrm, &uPosition);
    OpcUa_GotoErrorIfBad(uStatus);

    /* We can't read multiple chunks and jump back again. */
    uStatus = OpcUa_UInt32_BinaryDecode(&uTypeId, a_pSecureIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    if(uTypeId != OpcUa_MakeFourByteNodeId(OpcUaId_GetEndpointsRequest_Encoding_DefaultBinary)
       && uTypeId != OpcUa_MakeFourByteNodeId(OpcUaId_FindServersRequest_Encoding_DefaultBinary)
#if OPCUA_SECURELISTENER_DISCOVERY_ALLOW_FSON
       && uTypeId != OpcUa_MakeFourByteNodeId(OpcUaId_FindServersOnNetworkRequest_Encoding_DefaultBinary)
#endif
       )
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadServiceUnsupported);
    }

    uStatus = OpcUa_Stream_SetPosition((OpcUa_Stream*)a_pSecureIstrm, uPosition);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
#endif

/*============================================================================
 * OpcUa_SecureListener_ProcessSessionCallRequest
 *===========================================================================*/
/* HINT: Function assumes that its called with SecureListener object locked once!
         SecureListener is released during callback. */
OpcUa_StatusCode OpcUa_SecureListener_ProcessSessionCallRequest(
    OpcUa_Listener*                 a_pListener,
    OpcUa_Handle                    a_hConnection,
    OpcUa_InputStream**             a_ppTransportIstrm,
    OpcUa_Boolean                   a_bRequestComplete)
{
    OpcUa_SecureListener*   pSecureListener         = OpcUa_Null;
    OpcUa_InputStream*      pSecureIStrm            = OpcUa_Null;
    OpcUa_SecureStream*     pSecureStream           = OpcUa_Null;
    OpcUa_SecureChannel*    pSecureChannel          = OpcUa_Null;
    OpcUa_CryptoProvider*   pCryptoProvider         = OpcUa_Null;

    OpcUa_UInt32            uTokenId                = 0;
    OpcUa_UInt32            uSecureChannelId        = OPCUA_SECURECHANNEL_ID_INVALID;
    OpcUa_SecurityKeyset*   pReceivingKeyset        = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ProcessSessionCallRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_hConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppTransportIstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppTransportIstrm);

    /*** get listener handle ***/
    pSecureListener = (OpcUa_SecureListener*)a_pListener->Handle;
    OpcUa_ReturnErrorIfNull(pSecureListener, OpcUa_BadInvalidState);

    /* parse stream header */
    uStatus = OpcUa_SecureStream_DecodeSymmetricSecurityHeader( *a_ppTransportIstrm,
                                                                &uSecureChannelId,
                                                                &uTokenId);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                    "ProcessSessionCallRequest: SID %u, TID %u\n",
                    uSecureChannelId,
                    uTokenId);

    /*** find appropriate channel in channel manager or create CryptoProvider with SecurityPolicy ***/
    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(  pSecureListener->ChannelManager,
                                                                                uSecureChannelId,
                                                                                &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pSecureChannel->TransportConnection != a_hConnection)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    /* look if there is a pending stream */
    uStatus = OpcUa_SecureChannel_GetPendingInputStream(pSecureChannel,
                                                        &pSecureIStrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Get reference to keyset for requested token id */
    uStatus = pSecureChannel->GetSecuritySet(   pSecureChannel,
                                                uTokenId,
                                                &pReceivingKeyset,
                                                OpcUa_Null,
                                                &pCryptoProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    if(pSecureIStrm == OpcUa_Null)
    {
        /* create inputstream and check if processing can be started or delayed until last chunk is received. */
        uStatus = OpcUa_SecureStream_CreateInput(   pCryptoProvider,
                                                    pSecureChannel->MessageSecurityMode,
                                                    pSecureChannel->DiscoveryOnly ? OPCUA_SECURELISTENER_DISCOVERY_MAXCHUNKS :
                                                    pSecureChannel->nMaxBuffersPerMessage,
                                                    &pSecureIStrm);
        if(OpcUa_IsBad(uStatus))
        {
            pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                                  uTokenId);
            OpcUa_GotoError;
        }

        pSecureStream                       = (OpcUa_SecureStream*)pSecureIStrm->Handle;
        pSecureStream->SecureChannelId      = uSecureChannelId;
        pSecureStream->eMessageType         = eOpcUa_SecureStream_Types_StandardMessage;
        pSecureStream->eMessageSecurityMode = pSecureChannel->MessageSecurityMode;
    }
    else
    {
        pSecureStream = (OpcUa_SecureStream*)pSecureIStrm->Handle;
    }

    /* look if we can start processing the stream */
    if(a_bRequestComplete == OpcUa_False)
    {
        /* this is not the final chunk */
        uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportIstrm,
                                                    pSecureIStrm,
                                                    &pReceivingKeyset->SigningKey,
                                                    &pReceivingKeyset->EncryptionKey,
                                                    &pReceivingKeyset->InitializationVector,
                                                    pCryptoProvider,
                                                    pSecureChannel);
        /* release reference to security set */
        pSecureChannel->ReleaseSecuritySet(   pSecureChannel,
                                              uTokenId);

        OpcUa_GotoErrorIfBad(uStatus);

        /* Set the current stream as the pending request stream and leave. */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                pSecureIStrm);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ProcessSessionCallRequest: Waiting for more chunks!\n");

        /* buffer has been saved; transport stream wont be used again; release it */
        (*a_ppTransportIstrm)->Delete((OpcUa_Stream**)a_ppTransportIstrm);
    }
    else /* preprocess the stream */
    {
        /* HINT: add + 1 since the last chunks is not appended yet. */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ProcessSessionCallRequest: All %u chunks received; start processing!\n", pSecureStream->nBuffers + 1);

        pSecureStream->SecureChannelId = pSecureChannel->SecureChannelId;

        /* this is the final chunk */
        uStatus = OpcUa_SecureStream_AppendInput(   *a_ppTransportIstrm,
                                                    pSecureIStrm,
                                                    &pReceivingKeyset->SigningKey,
                                                    &pReceivingKeyset->EncryptionKey,
                                                    &pReceivingKeyset->InitializationVector,
                                                    pCryptoProvider,
                                                    pSecureChannel);
        /* release reference to security set */
        pSecureChannel->ReleaseSecuritySet( pSecureChannel,
                                            uTokenId);

        OpcUa_GotoErrorIfBad(uStatus);

        /* Clear the pending input stream marker */
        uStatus = OpcUa_SecureChannel_SetPendingInputStream(    pSecureChannel,
                                                                OpcUa_Null);
        OpcUa_GotoErrorIfBad(uStatus);

        /* reset buffer index to start reading from the first buffer */
        pSecureStream->nCurrentReadBuffer = 0;

        /* ensure that discovery only channels don't process non-discovery requests */
        if(pSecureChannel->DiscoveryOnly)
        {
            uStatus = OpcUa_SecureListener_ValidateDiscoveryChannel(pSecureListener, pSecureIStrm);
            if(OpcUa_IsBad(uStatus))
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureListener_ProcessSessionCallRequest: NonDiscovery Service requested through non secure channel.\n");
                OpcUa_GotoError;
            }
        }


        /*** invoke callback */
        /* this goes to the owner of the secure listener, which is the endpoint ***/
        if(pSecureListener->Callback != OpcUa_Null)
        {
            /*** release lock. ***/
            OPCUA_P_MUTEX_UNLOCK(pSecureListener->Mutex);

            uStatus = pSecureListener->Callback(
                a_pListener,                        /* the source of the event          */
                pSecureListener->CallbackData,      /* the callback data                */
                OpcUa_ListenerEvent_Request,        /* the type of the event            */
                a_hConnection,                      /* the handle for the connection    */
                &pSecureIStrm,                      /* the stream to read from          */
                uStatus);                           /* the event status                 */
            if(pSecureChannel->uPendingMessageCount > OPCUA_SECURECONNECTION_MAXPENDINGMESSAGES)
            {
                pSecureChannel->LockWriteMutex(pSecureChannel);
                OpcUa_Listener_AddToSendQueue(
                    pSecureListener->TransportListener,
                    pSecureChannel->TransportConnection,
                    pSecureChannel->pPendingSendBuffers,
                    OPCUA_LISTENER_NO_RCV_UNTIL_DONE);
                pSecureChannel->pPendingSendBuffers = OpcUa_Null;
                pSecureChannel->uPendingMessageCount = 0;
                pSecureChannel->UnlockWriteMutex(pSecureChannel);
            }

            if(pSecureIStrm == OpcUa_Null)
            {
                /* stream is unlinked */
                *a_ppTransportIstrm = OpcUa_Null;
            }
            else
            {
                OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIStrm);
            }

            /*** acquire lock until callback is complete. ***/
            OPCUA_P_MUTEX_LOCK(pSecureListener->Mutex);
        }
    }

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* clear pending input stream  */
    OpcUa_SecureChannel_SetPendingInputStream(  pSecureChannel,
                                                OpcUa_Null);

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

    /* delete stream */
    OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureIStrm);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_GetChannelId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_GetChannelId(
    OpcUa_Listener*     a_pListener,
    OpcUa_InputStream*  a_pIstrm,
    OpcUa_UInt32*       a_puChannelId)
{
    OpcUa_SecureStream* pStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "OpcUa_SecureListener_GetChannelId");

    OpcUa_ReturnErrorIfArgumentNull(a_puChannelId);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    pStream = (OpcUa_SecureStream*)a_pIstrm->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pStream);

    OpcUa_ReferenceParameter(a_pListener);

    *a_puChannelId = pStream->SecureChannelId;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_GetSecureChannelSecurityPolicyConfiguration
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_GetSecureChannelSecurityPolicyConfiguration(
    OpcUa_Listener*                                     a_pListener,
    OpcUa_UInt32                                        a_uChannelId,
    OpcUa_SecureListener_SecurityPolicyConfiguration*   a_pSecurityPolicyConfiguration)
{
    OpcUa_SecureListener*       pSecureListener     = (OpcUa_SecureListener*)a_pListener->Handle;
    OpcUa_SecureChannel*        pSecureChannel      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "GetSecureChannelSecurityPolicyConfiguration");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecurityPolicyConfiguration);

    /* get the securechannel with the given channel id and return the */
    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(
        pSecureListener->ChannelManager,
        a_uChannelId,
        &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    /* maybe this could be the wrong provider if the channel gets renewed between
       the message is received and the call to this function ... the only ways to
       solve this problem would be to store or reference the policy uri in the
       message context (pretty safe, because the referenced data is) or to use the
       channel security token. */
    OpcUa_String_AttachToString(pSecureChannel->pCurrentCryptoProvider->Name,
                                OPCUA_STRING_LENDONTCARE,
                                0,
                                OpcUa_False,
                                OpcUa_False,
                                &a_pSecurityPolicyConfiguration->sSecurityPolicy);

    a_pSecurityPolicyConfiguration->uMessageSecurityModes = OPCUA_ENDPOINT_MESSAGESECURITYMODE_FROM_ENUM(pSecureChannel->MessageSecurityMode);
    a_pSecurityPolicyConfiguration->pbsClientCertificate = OpcUa_Null;

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
            pSecureListener->ChannelManager,
            &pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureListener_GetPeerInfo
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_GetPeerInfo(
    OpcUa_Listener*                                     a_pListener,
    OpcUa_UInt32                                        a_uChannelId,
    OpcUa_String*                                       a_pPeerInfo)
{
    OpcUa_SecureListener*       pSecureListener     = (OpcUa_SecureListener*)a_pListener->Handle;
    OpcUa_SecureChannel*        pSecureChannel      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "GetPeerInfo");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pPeerInfo);

    /* get the securechannel with the given channel id */
    uStatus = OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(
        pSecureListener->ChannelManager,
        a_uChannelId,
        &pSecureChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    /* return the peer info of the secure channel */
    uStatus = OpcUa_String_StrnCpy( a_pPeerInfo,
        &pSecureChannel->sPeerInfo,
        OPCUA_STRING_LENDONTCARE);

    OpcUa_SecureListener_ChannelManager_ReleaseChannel(
        pSecureListener->ChannelManager,
        &pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_SERVERAPI */

