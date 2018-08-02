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
#include <opcua_socket.h>
#include <opcua_pkifactory.h>
#include <opcua_cryptofactory.h>
#include <opcua_list.h>
#include <opcua_datetime.h>
#include <opcua_utilities.h>
#include <opcua_guid.h>

/* stackcore */
#include <opcua_types.h>
#include <opcua_builtintypes.h>
#include <opcua_binaryencoder.h>

/* communication */
#include <opcua_connection.h>
#include <opcua_tcpconnection.h>
#include <opcua_secureconnection.h>
#include <opcua_tcplistener.h>

#ifdef OPCUA_HAVE_HTTPS
#include <opcua_httpsconnection.h>
#endif /* OPCUA_HAVE_HTTPS */

/* security */
#include <opcua_crypto.h>

/* client api */
#include <opcua_channel.h>
#include <opcua_asynccallstate.h>
#include <opcua_channel_internal.h>

/*============================================================================
 ** @brief Global table of known types.
 *===========================================================================*/
extern OpcUa_EncodeableTypeTable OpcUa_ProxyStub_g_EncodeableTypes;

/*============================================================================
 ** @brief Global table of supported namespaces.
 *===========================================================================*/
extern OpcUa_StringTable OpcUa_ProxyStub_g_NamespaceUris;

/*============================================================================
 * OpcUa_Channel_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Channel_Create(  OpcUa_Channel*                  a_phChannel,
                                        OpcUa_Channel_SerializerType    a_eSerializerType)
{
    OpcUa_InternalChannel* pInternalChannel = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Channel, "OpcUa_Channel_Create");

    OpcUa_ReturnErrorIfArgumentNull(a_phChannel);

    *a_phChannel = OpcUa_Null;

    /* increment counter */
    OpcUa_ProxyStub_RegisterChannel();

    pInternalChannel = (OpcUa_InternalChannel*)OpcUa_Alloc(sizeof(OpcUa_InternalChannel));
    OpcUa_GotoErrorIfAllocFailed(pInternalChannel);
    OpcUa_MemSet(pInternalChannel, 0, sizeof(OpcUa_InternalChannel));

    uStatus = OPCUA_P_MUTEX_CREATE(&(pInternalChannel->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    /* create the encoder and decoder */
    if(a_eSerializerType == OpcUa_Channel_SerializerType_Binary)
    {
        /* create encoder */
        uStatus = OpcUa_BinaryEncoder_Create(&pInternalChannel->Encoder);
        OpcUa_GotoErrorIfBad(uStatus);

        /* create decoder */
        uStatus = OpcUa_BinaryDecoder_Create(&pInternalChannel->Decoder);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* the encoding type is not supported */
    else
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }

    *a_phChannel = (OpcUa_Channel)pInternalChannel;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Channel_Delete((OpcUa_Channel*)&pInternalChannel);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_Clear
 *===========================================================================*/
OPCUA_EXPORT OpcUa_Void OpcUa_Channel_Clear(OpcUa_Channel a_hChannel)
{
    if(a_hChannel != OpcUa_Null)
    {
        OpcUa_InternalChannel* pChannel = (OpcUa_InternalChannel*)a_hChannel;

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_Clear: waiting for channel access\n");

        OPCUA_P_MUTEX_LOCK(pChannel->Mutex);

        /* unlock the channel object during the connection shutdown to allow events to check the channel state */
        OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_Clear: clearing connections\n");

        /* delete the transport connection */
        OpcUa_Connection_Delete(&pChannel->TransportConnection);

        /* delete the secure connection */
        OpcUa_Connection_Delete(&pChannel->SecureConnection);

        /* regain control over the channel object during the remaining phase */
        OPCUA_P_MUTEX_LOCK(pChannel->Mutex);

        OpcUa_Encoder_Delete(&pChannel->Encoder);
        OpcUa_Decoder_Delete(&pChannel->Decoder);

        OpcUa_String_Clear(&(pChannel->Url));
        OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);
        OPCUA_P_MUTEX_DELETE(&pChannel->Mutex);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_Clear: done\n");
    }
}

/*============================================================================
 * OpcUa_Channel_Delete
 *===========================================================================*/
OPCUA_EXPORT OpcUa_Void OpcUa_Channel_Delete(OpcUa_Channel* a_phChannel)
{
    if(a_phChannel != OpcUa_Null && *a_phChannel != OpcUa_Null)
    {
        OpcUa_InternalChannel* pChannel = (OpcUa_InternalChannel*)*a_phChannel;

        *a_phChannel = OpcUa_Null;

        OpcUa_Channel_Clear(pChannel);
        OpcUa_Free(pChannel);

        /* increment counter */
        OpcUa_ProxyStub_DeRegisterChannel();
    }
}

/*============================================================================
 * OpcUa_Channel_ReadResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_Channel_ReadResponse(
    OpcUa_Channel          a_pChannel,
    OpcUa_InputStream*     a_pIstrm,
    OpcUa_EncodeableType** a_ppResponseType,
    OpcUa_Void**           a_ppResponse)
{
    OpcUa_MessageContext cContext;
    OpcUa_Decoder* pDecoder = OpcUa_Null;
    OpcUa_InternalChannel* pChannel = OpcUa_Null;
    OpcUa_Handle hDecodeContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "OpcUa_Channel_ReadResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_ppResponseType);
    OpcUa_ReturnErrorIfArgumentNull(a_ppResponse);

    *a_ppResponseType = OpcUa_Null;
    *a_ppResponse = OpcUa_Null;

    pChannel = (OpcUa_InternalChannel*)a_pChannel;
    pDecoder = pChannel->Decoder;

    /* initialize context */
    OpcUa_MessageContext_Initialize(&cContext);

    cContext.KnownTypes    = &OpcUa_ProxyStub_g_EncodeableTypes;
    cContext.NamespaceUris = &OpcUa_ProxyStub_g_NamespaceUris;

    /* create decoder */
    uStatus = pDecoder->Open(pDecoder, a_pIstrm, &cContext, &hDecodeContext);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoError;
    }

    /* decode message */
    uStatus = pDecoder->ReadMessage((struct _OpcUa_Decoder*)hDecodeContext, a_ppResponseType, a_ppResponse);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_GotoError;
    }

    /* close decoder */
    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);

    OpcUa_MessageContext_Clear(&cContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* delete decoder and stream */
    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);

    OpcUa_EncodeableObject_Delete(*a_ppResponseType, a_ppResponse);

    /* put output parameters in a known state */
    *a_ppResponseType = OpcUa_Null;
    *a_ppResponse = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_ResponseAvailable
 *===========================================================================*/
/* This callback gets called by the connection if an event occurred. */
static OpcUa_StatusCode OpcUa_Channel_ResponseAvailable(
    OpcUa_Connection*   a_pConnection,
    OpcUa_Void*         a_pCallbackData,
    OpcUa_StatusCode    a_uOperationStatus,
    OpcUa_InputStream** a_ppIstrm)
{
    OpcUa_AsyncCallState*   pAsyncState     = OpcUa_Null;
    OpcUa_Void*             pResponse       = OpcUa_Null;
    OpcUa_EncodeableType*   pResponseType   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "ResponseAvailable");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallbackData);

    pAsyncState = (OpcUa_AsyncCallState*)a_pCallbackData;

    if(pAsyncState != OpcUa_Null)
    {
        OPCUA_P_MUTEX_LOCK(pAsyncState->WaitMutex);

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_ResponseAvailable: Operation Status 0x%X\n", a_uOperationStatus);

        if(OpcUa_IsGood(a_uOperationStatus))
        {
            /* if the caller does not give a stream, just do a notification */
            if(a_ppIstrm != OpcUa_Null && (*a_ppIstrm) != OpcUa_Null)
            {
                uStatus = OpcUa_Channel_ReadResponse(   pAsyncState->Channel,
                                                        (*a_ppIstrm),
                                                        &pResponseType,
                                                        &pResponse);
                if(OpcUa_IsGood(uStatus))
                {
                    pAsyncState->ResponseData = pResponse;
                    pAsyncState->ResponseType = pResponseType;
                    pResponse = OpcUa_Null;

                    if(pResponseType != OpcUa_Null)
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_Channel_ResponseAvailable: %s\n", pResponseType->TypeName);
                    }
                    else
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Channel_ResponseAvailable: Empty or unknown response! (0x%08X)\n", a_uOperationStatus);
                    }
                }
                else
                {
                    a_uOperationStatus  = uStatus;
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Channel_ResponseAvailable: Decoding failed! (0x%08X)\n", a_uOperationStatus);
                }
            }
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Channel_ResponseAvailable: Request failed! (0x%08X)\n", a_uOperationStatus);
        }

        /* signal the request issuer that a response is available */
        if(pAsyncState->Callback == OpcUa_Null)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_ResponseAvailable: Signalling Response!\n");

            /* this path is executed when the synchronous invoke is called. */
            uStatus = OpcUa_AsyncCallState_SignalCompletion(pAsyncState,
                                                            a_uOperationStatus);

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_ResponseAvailable: Signalling Response Done!\n");

            OPCUA_P_MUTEX_UNLOCK(pAsyncState->WaitMutex);
            /* unlike below, the asyncstate object is destroyed be the waiting thread. */
        }
        else /* invoke the application supplied callback */
        {
            pAsyncState->Status = a_uOperationStatus;

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_ResponseAvailable: Calling Application Callback!\n");

            /* user supplied callback */
            uStatus = pAsyncState->Callback(    pAsyncState->Channel,
                                                pAsyncState->ResponseData,
                                                pAsyncState->ResponseType,
                                                pAsyncState->CallbackData,
                                                pAsyncState->Status);

            /*pAsyncState->ResponseData = OpcUa_Null;*/

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_ResponseAvailable: Calling Application Callback Done!\n");

            OPCUA_P_MUTEX_UNLOCK(pAsyncState->WaitMutex);
            OpcUa_AsyncCallState_Delete(&pAsyncState);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_ResponseAvailable: Leaving with Error %08X!\n", uStatus);

    if(pAsyncState != OpcUa_Null)
    {
        OPCUA_P_MUTEX_UNLOCK(pAsyncState->WaitMutex); /* if this function is changed, check if mutex is always locked when getting to this point */
        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_BeginInvokeService
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Channel_BeginInvokeService(  OpcUa_Channel                     a_hChannel,
                                                    OpcUa_StringA                     a_sName,
                                                    OpcUa_Void*                       a_pRequest,
                                                    OpcUa_EncodeableType*             a_pRequestType,
                                                    OpcUa_Channel_PfnRequestComplete* a_pCallback,
                                                    OpcUa_Void*                       a_pCallbackData)
{
    OpcUa_MessageContext    cContext;
    OpcUa_Encoder*          pEncoder        = OpcUa_Null;
    OpcUa_OutputStream*     pSecureOstrm    = OpcUa_Null;
    OpcUa_InternalChannel*  pChannel        = OpcUa_Null;
    OpcUa_UInt32            uTimeout        = 0;
    OpcUa_AsyncCallState*   pAsyncState     = OpcUa_Null;
    OpcUa_Handle            hEncodeContext  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "OpcUa_Channel_BeginInvokeService");

    OpcUa_ReturnErrorIfArgumentNull(a_hChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestType);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallback);
    OpcUa_ReferenceParameter(a_sName);

    pChannel = (OpcUa_InternalChannel*)a_hChannel;
    pEncoder = pChannel->Encoder;

    OPCUA_P_MUTEX_LOCK(pChannel->Mutex);

    if(pChannel->SecureConnection == OpcUa_Null)
    {
        OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);
        OpcUa_GotoErrorWithStatus(OpcUa_BadServerNotConnected);
    }

    OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_Channel_BeginInvokeService: called for %s!\n", a_pRequestType->TypeName);

    /* initialize context */
    OpcUa_MessageContext_Initialize(&cContext);

    cContext.KnownTypes         = &OpcUa_ProxyStub_g_EncodeableTypes;
    cContext.NamespaceUris      = &OpcUa_ProxyStub_g_NamespaceUris;
    cContext.AlwaysCheckLengths = OPCUA_SERIALIZER_CHECKLENGTHS;

    /* retrieve the service call timeout */
    uTimeout = ((OpcUa_RequestHeader*)a_pRequest)->TimeoutHint;

    /* create output stream */
    uStatus = OpcUa_Connection_BeginSendRequest(pChannel->SecureConnection, &pSecureOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* open encoder */
    uStatus = pEncoder->Open(pEncoder, pSecureOstrm, &cContext, &hEncodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* encode message */
    uStatus = pEncoder->WriteMessage((struct _OpcUa_Encoder*)hEncodeContext, a_pRequest, a_pRequestType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* close encoder and stream */
    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

    /* allocate the call state object */
    uStatus =  OpcUa_AsyncCallState_Create( pChannel,      /* channel handle */
                                            OpcUa_Null,    /* request */
                                            OpcUa_Null,    /* request type */
                                            &pAsyncState); /* the async state object */
    OpcUa_GotoErrorIfBad(uStatus);

    pAsyncState->Callback       = a_pCallback;
    pAsyncState->CallbackData   = a_pCallbackData;

    /* finish sending the request */
    uStatus = OpcUa_Connection_EndSendRequest(  pChannel->SecureConnection,
                                                &pSecureOstrm,
                                                uTimeout,
                                                OpcUa_Channel_ResponseAvailable,
                                                (OpcUa_Void*)pAsyncState);

    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_MessageContext_Clear(&cContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Channel_BeginInvokeService: failed with 0x%08X!\n", uStatus);

    if(pSecureOstrm != OpcUa_Null)
    {
        /* error occurred during message transmission; clean up resources allocated for request */
        OpcUa_Connection_AbortSendRequest(  pChannel->SecureConnection,
                                            uStatus,
                                            OpcUa_Null,
                                            &pSecureOstrm);
    }

    /* delete encoder and stream */
    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

    OpcUa_Stream_Delete((OpcUa_Stream**)&pSecureOstrm);

    OpcUa_MessageContext_Clear(&cContext);

    if(pAsyncState)
    {
        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_InvokeService
 *===========================================================================*/
/* Main service invoke for synchronous behaviour; this function blocks until */
/* the server sends a response for this request.                             */
OpcUa_StatusCode OpcUa_Channel_InvokeService(   OpcUa_Channel           a_pChannel,
                                                OpcUa_StringA           a_sName,
                                                OpcUa_Void*             a_pRequest,
                                                OpcUa_EncodeableType*   a_pRequestType,
                                                OpcUa_Void**            a_ppResponse,
                                                OpcUa_EncodeableType**  a_ppResponseType)
{
    OpcUa_InternalChannel*  pChannel            = OpcUa_Null;
    OpcUa_OutputStream*     pOstrm              = OpcUa_Null;
    OpcUa_Encoder*          pEncoder            = OpcUa_Null;
    OpcUa_AsyncCallState*   pAsyncState         = OpcUa_Null;
    OpcUa_Handle            hEncodeContext      = OpcUa_Null;
    OpcUa_UInt32            uTimeout            = OPCUA_INFINITE;
    OpcUa_MessageContext    cContext;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "InvokeService");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestType);
    OpcUa_ReturnErrorIfArgumentNull(a_ppResponse);
    OpcUa_ReturnErrorIfArgumentNull(a_ppResponseType);

    OpcUa_ReferenceParameter(a_sName);

    *a_ppResponse = OpcUa_Null;
    *a_ppResponseType = OpcUa_Null;

    OpcUa_MessageContext_Initialize(&cContext);

    pChannel = (OpcUa_InternalChannel*)a_pChannel;
    pEncoder = pChannel->Encoder;

    OpcUa_GotoErrorIfTrue((pChannel->SecureConnection == OpcUa_Null), OpcUa_BadServerNotConnected);

    /* initialize context */
    cContext.KnownTypes         = &OpcUa_ProxyStub_g_EncodeableTypes;
    cContext.NamespaceUris      = &OpcUa_ProxyStub_g_NamespaceUris;
    cContext.AlwaysCheckLengths = OPCUA_SERIALIZER_CHECKLENGTHS;

    /* retrieve the service call timeout */
    uTimeout = ((OpcUa_RequestHeader*)a_pRequest)->TimeoutHint;

    /* create output stream through connection */
    uStatus = OpcUa_Connection_BeginSendRequest(pChannel->SecureConnection, &pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* open encoder */
    uStatus = pEncoder->Open(pEncoder, pOstrm, &cContext, &hEncodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* encode message */
    uStatus = pEncoder->WriteMessage((struct _OpcUa_Encoder*)hEncodeContext, a_pRequest, a_pRequestType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* finish encoding of message */
    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

    /* allocate the call state object */
    uStatus =  OpcUa_AsyncCallState_Create(a_pChannel, OpcUa_Null, OpcUa_Null, &pAsyncState);
    OpcUa_GotoErrorIfBad(uStatus);

    /* lock request mutex */
    /* OPCUA_P_MUTEX_LOCK(pAsyncState->WaitMutex); */
    /* On rare occasions, deadlocks occurred when the secure layers watchdog thread called  */
    /* ResponseAvailable while holding its list and tried to get access to the AsyncState   */
    /* and an error occurred in the following call between adding the request object to the */
    /* list and trying to remove it again.                                                  */
    /* Since the AsyncState does not get deleted by ResponseAvailable it should be safe to  */
    /* make the following call without locking the AsyncState. */

    /*** send request to the server. ***/
    uStatus = OpcUa_Connection_EndSendRequest(  pChannel->SecureConnection,
                                                &pOstrm,
                                                uTimeout,
                                                OpcUa_Channel_ResponseAvailable,
                                                (OpcUa_Void*)pAsyncState);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** clean up ***/
    OpcUa_MessageContext_Clear(&cContext);

    /* release request mutex */
    /* OPCUA_P_MUTEX_UNLOCK(pAsyncState->WaitMutex); */

    /* wait for notification on the socket ; timeout is handled in the layer below, hence we wait for "infinit" time */
    uStatus = OpcUa_AsyncCallState_WaitForCompletion(pAsyncState, OPCUA_INFINITE);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }
    else
    {
        *a_ppResponse = pAsyncState->ResponseData;
        *a_ppResponseType = pAsyncState->ResponseType;

        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(hEncodeContext != OpcUa_Null)
    {
        OpcUa_Encoder_Close(pEncoder, &hEncodeContext);
    }

    if(pOstrm != OpcUa_Null)
    {
        /* error occurred during message transmission; clean up resources allocated for request */
        OpcUa_Connection_AbortSendRequest(  pChannel->SecureConnection,
                                            uStatus,
                                            OpcUa_Null,
                                            &pOstrm);
    }

    OpcUa_MessageContext_Clear(&cContext);

    if(pAsyncState)
    {
        /* OPCUA_P_MUTEX_UNLOCK(pAsyncState->WaitMutex); */
        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_BeginSendEncodedRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Channel_BeginSendEncodedRequest(
    OpcUa_Channel                   a_hChannel,
    OpcUa_ByteString*               a_pRequest,
    OpcUa_UInt32                    a_uTimeout,
    OpcUa_Connection_PfnOnResponse* a_pCallback,
    OpcUa_Void*                     a_pCallbackData)
{
    OpcUa_OutputStream*     pOstrm   = OpcUa_Null;
    OpcUa_InternalChannel*  pChannel = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "BeginSendEncodedRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_hChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallback);

    pChannel = (OpcUa_InternalChannel*)a_hChannel;

    /* lock the session until the request is sent - not release until End or Abort is called. */
    OPCUA_P_MUTEX_LOCK(pChannel->Mutex);

    OpcUa_GotoErrorIfTrue((pChannel->SecureConnection == OpcUa_Null), OpcUa_BadServerNotConnected);

    /* create output stream */
    uStatus = OpcUa_Connection_BeginSendRequest(pChannel->SecureConnection, &pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* write data */
    uStatus = OpcUa_Stream_Write(pOstrm, a_pRequest->Data, a_pRequest->Length);
    OpcUa_GotoErrorIfBad(uStatus);

    /* finish sending the request */
    uStatus = OpcUa_Connection_EndSendRequest(  pChannel->SecureConnection,
                                                &pOstrm,
                                                a_uTimeout,
                                                a_pCallback,
                                                a_pCallbackData);
    OpcUa_GotoErrorIfBad(uStatus);

    /* unlock the mutex (locked by OpcUa_Channel_BeginSendRequest) */
    OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);

    /* close stream */
    OpcUa_Stream_Close((OpcUa_Stream*)pOstrm);
    OpcUa_Stream_Delete((OpcUa_Stream**)&pOstrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* unlock the mutex (locked by OpcUa_Channel_BeginSendRequest) */
    OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);

    /* delete stream */
    OpcUa_Stream_Close((OpcUa_Stream*)pOstrm);
    OpcUa_Stream_Delete((OpcUa_Stream**)&pOstrm);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_OnNotify
 *===========================================================================*/
/* this function is called asynchronously by the underlying connection object  */
/* if a connection event happens. Only used for connecting and disconnecting. */
static OpcUa_StatusCode OpcUa_Channel_OnNotify( OpcUa_Connection*     a_pConnection,
                                                OpcUa_Void*           a_pCallbackData,
                                                OpcUa_ConnectionEvent a_eEvent,
                                                OpcUa_InputStream**   a_ppInputStream,
                                                OpcUa_StatusCode      a_uOperationStatus)
{
    OpcUa_InternalChannel* pInternalChannel = (OpcUa_InternalChannel*)a_pCallbackData;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "OnNotify");

    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);
    OpcUa_ReturnErrorIfArgumentNull(pInternalChannel);

    OpcUa_ReferenceParameter(a_ppInputStream); /* a stream is not expected in this callback */

    OPCUA_P_MUTEX_LOCK(pInternalChannel->Mutex);

    switch(a_eEvent)
    {
    case OpcUa_ConnectionEvent_Connect:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Underlying connection raised connect event with status 0x%08X!\n", a_uOperationStatus);

            if(pInternalChannel->pfCallback != OpcUa_Null)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Notifying application!\n");
                uStatus = pInternalChannel->pfCallback( pInternalChannel,
                                                        pInternalChannel->pvCallbackData,
                                                        eOpcUa_Channel_Event_Connected,
                                                        a_uOperationStatus);
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Notifying application done!\n");
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Can not notify application!\n");
            }
            break;
        }
    case OpcUa_ConnectionEvent_Reconnecting:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Underlying connection is trying to reconnect!\n");
            /* no way to tell the application about the disconnect directly; only through the status codes returned with the requests. */
            break;
        }
    case OpcUa_ConnectionEvent_Disconnect:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Underlying connection raised disconnect event!\n");

            if(pInternalChannel->pfCallback != OpcUa_Null)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Notifying application!\n");
                uStatus = pInternalChannel->pfCallback( pInternalChannel,
                                                        pInternalChannel->pvCallbackData,
                                                        eOpcUa_Channel_Event_Disconnected,
                                                        a_uOperationStatus);
                pInternalChannel->pfCallback = OpcUa_Null;
                pInternalChannel->pvCallbackData = OpcUa_Null;
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_OnNotify: Can not notify application: CB %p!\n", pInternalChannel?pInternalChannel->pfCallback:OpcUa_Null);
            }
            break;
        }
    case OpcUa_ConnectionEvent_UnexpectedError:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Channel_OnNotify: Underlying connection raised unexpected error event!\n");

            if(pInternalChannel->pfCallback != OpcUa_Null)
            {
                uStatus = pInternalChannel->pfCallback( pInternalChannel,
                                                        pInternalChannel->pvCallbackData,
                                                        eOpcUa_Channel_Event_Disconnected,
                                                        a_uOperationStatus);
                pInternalChannel->pfCallback = OpcUa_Null;
                pInternalChannel->pvCallbackData = OpcUa_Null;
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Channel_OnNotify: Cannot inform client application about error 0x%08X\n", a_uOperationStatus);
            }
            break;
        }
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Channel_OnNotify: Underlying connection raised unspecified event!\n");
            break;
        }
    } /* switch on event type */

    OPCUA_P_MUTEX_UNLOCK(pInternalChannel->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(pInternalChannel->Mutex);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_DisconnectComplete
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_Channel_InternalDisconnectComplete(OpcUa_Channel      a_hChannel,
                                                                OpcUa_Void*         a_pCallbackData,
                                                                OpcUa_Channel_Event a_eEvent,
                                                                OpcUa_StatusCode    a_uStatus)
{
OpcUa_InitializeStatus(OpcUa_Module_Channel, "InternalDisconnectComplete");

    OpcUa_ReferenceParameter(a_hChannel);
    OpcUa_ReferenceParameter(a_eEvent);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_InternalDisconnectComplete called!\n");

    /* ignore connected event */
    if(a_eEvent == eOpcUa_Channel_Event_Connected)
    {
        OpcUa_ReturnStatusCode;
    }
    /* BadDisconnect means Good */
    if(a_uStatus == OpcUa_BadDisconnect)
    {
        a_uStatus = OpcUa_Good;
    }

    uStatus =  OpcUa_AsyncCallState_SignalCompletion((OpcUa_AsyncCallState*)a_pCallbackData, a_uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_Disconnect
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_BeginDisconnect(OpcUa_Channel                               a_pChannel,
                                                            OpcUa_Channel_PfnConnectionStateChanged*    a_pfCallback,
                                                            OpcUa_Void*                                 a_pCallbackData)
{
    OpcUa_InternalChannel*              pChannel            = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "BeginDisconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannel);

    pChannel = (OpcUa_InternalChannel*)a_pChannel;

    /* lock the session until the request is sent */
    OPCUA_P_MUTEX_LOCK(pChannel->Mutex);

    pChannel->pfCallback        = a_pfCallback;
    pChannel->pvCallbackData    = a_pCallbackData;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_BeginDisconnect: Beginning to disconnect!\n");

    OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);

    /* disconnect to the server - notify on standard callback */
    uStatus = OpcUa_Connection_Disconnect(  pChannel->SecureConnection,
                                            OpcUa_True);


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_Disconnect
 *===========================================================================*/
/* synchronous disconnect from server - blocks because of securechannel messages delay */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_Disconnect(OpcUa_Channel a_hChannel)
{
    OpcUa_AsyncCallState*   pAsyncState = OpcUa_Null;
    OpcUa_InternalChannel*  pChannel    = (OpcUa_InternalChannel*)a_hChannel;

OpcUa_InitializeStatus(OpcUa_Module_Channel, "Disconnect");

    OpcUa_ReturnErrorIfArgumentNull(a_hChannel);
    OpcUa_ReferenceParameter(pChannel);

    /* create waithandle */
    uStatus =  OpcUa_AsyncCallState_Create(a_hChannel, OpcUa_Null, OpcUa_Null, &pAsyncState);
    OpcUa_ReturnErrorIfBad(uStatus);

    uStatus = OpcUa_Channel_BeginDisconnect(    a_hChannel,
                                                OpcUa_Channel_InternalDisconnectComplete,
                                                (OpcUa_Void*)pAsyncState);
    OpcUa_GotoErrorIfBad(uStatus);

    /* ************************ wait for completion ************************** */
#if OPCUA_MULTITHREADED
    uStatus = OpcUa_AsyncCallState_WaitForCompletion(   pAsyncState,
                                                        pChannel->NetworkTimeout);

    if(OpcUa_IsEqual(OpcUa_BadTimeout))
    {
        /* disconnect the transport layer if the secure connection is stuck */
        uStatus = OpcUa_Connection_Disconnect(  pChannel->TransportConnection,
                                                OpcUa_True);
    }
#else
    OpcUa_ReferenceParameter(pChannel);
    uStatus = OpcUa_GoodCompletesAsynchronously;
#endif

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Channel_Disconnect: Woke up with status 0x%08X\n", uStatus);

    /* ********************** end wait for completion ************************ */

    OpcUa_AsyncCallState_Delete(&pAsyncState);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pAsyncState != OpcUa_Null)
    {
        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_InternalConnectComplete
 *===========================================================================*/
/* this function is the counterpart of OpcUa_Channel_Connect() and unlocks it */
static OpcUa_StatusCode OpcUa_Channel_InternalConnectComplete(  OpcUa_Channel       a_hChannel,
                                                                OpcUa_Void*         a_pCallbackData,
                                                                OpcUa_Channel_Event a_eEvent,
                                                                OpcUa_StatusCode    a_uStatus)
{
    OpcUa_ReturnErrorIfArgumentNull(a_pCallbackData);

    OpcUa_ReferenceParameter(a_hChannel);

    if(a_eEvent == eOpcUa_Channel_Event_Disconnected)
    {
        a_uStatus = OpcUa_BadDisconnect;
    }

    return OpcUa_AsyncCallState_SignalCompletion((OpcUa_AsyncCallState*)a_pCallbackData, a_uStatus);
}

/*============================================================================
 * OpcUa_Channel_InternalBeginConnect
 *===========================================================================*/
/* initiates an asynchronous connect process */
OpcUa_StatusCode OpcUa_Channel_BeginConnect(OpcUa_Channel                               a_pChannel,
                                            OpcUa_StringA                               a_sUrl,
                                            OpcUa_ByteString*                           a_pClientCertificate,
                                            OpcUa_Key*                                  a_pClientPrivateKey,
                                            OpcUa_ByteString*                           a_pServerCertificate,
                                            OpcUa_Void*                                 a_pPKIConfig,
                                            OpcUa_String*                               a_pRequestedSecurityPolicyUri,
                                            OpcUa_Int32                                 a_nRequestedLifetime,
                                            OpcUa_MessageSecurityMode                   a_messageSecurityMode,
                                            OpcUa_UInt32                                a_nNetworkTimeout,
                                            OpcUa_Channel_PfnConnectionStateChanged*    a_pfCallback,
                                            OpcUa_Void*                                 a_pCallbackData)
{
    OpcUa_InternalChannel*          pChannel                = (OpcUa_InternalChannel*)a_pChannel;
    OpcUa_ClientCredential*         pClientCredentials      = OpcUa_Null;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_Boolean                   bLocked                 = OpcUa_False;
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_InitializeStatus(OpcUa_Module_Channel, "BeginConnect");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientPrivateKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pfCallback);

    if(a_nNetworkTimeout == 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    if(     (a_messageSecurityMode != OpcUa_MessageSecurityMode_None)
        &&  ((a_pServerCertificate->Length <= 0) || (a_pClientCertificate->Length <= 0) || (a_pClientPrivateKey->Key.Data == OpcUa_Null)))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Channel_BeginConnect: Cannot create secure channel without certificates!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    /* lock the session until the request is sent */
#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_LOCK(pChannel->Mutex);
    bLocked = OpcUa_True;
#endif

    /* save url */
    OpcUa_String_Clear(&pChannel->Url);

    uStatus = OpcUa_String_StrnCpy( &pChannel->Url,
                                    OpcUa_String_FromCString(a_sUrl),
                                    OPCUA_STRING_LENDONTCARE);
    OpcUa_GotoErrorIfBad(uStatus);

    /* save session timeout */
    pChannel->NetworkTimeout = a_nNetworkTimeout;

    /* remove old connection objects */
    if(pChannel->TransportConnection != OpcUa_Null)
    {
        OpcUa_Connection_Delete(&pChannel->TransportConnection);
    }

    /* delete the secure connection */
    if(pChannel->SecureConnection != OpcUa_Null)
    {
        OpcUa_Connection_Delete(&pChannel->SecureConnection);
    }

    /* select the protocol type based on the url scheme */
    if(!OpcUa_String_StrnCmp(   &(pChannel->Url),
                                OpcUa_String_FromCString("opc.tcp:"),
                                8,
                                OpcUa_True))
    {
        uStatus = OpcUa_TcpConnection_Create(&pChannel->TransportConnection);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_SecureConnection_Create(pChannel->TransportConnection,
                                                pChannel->Encoder,
                                                pChannel->Decoder,
                                                &OpcUa_ProxyStub_g_NamespaceUris,
                                                &OpcUa_ProxyStub_g_EncodeableTypes,
                                                &pChannel->SecureConnection);
        OpcUa_GotoErrorIfBad(uStatus);
    }
#ifdef OPCUA_HAVE_HTTPS
    else if(!OpcUa_String_StrnCmp(  &(pChannel->Url),
                                    OpcUa_String_FromCString("https:"),
                                    6,
                                    OpcUa_True))
    {
        uStatus = OpcUa_HttpsConnection_Create(&pChannel->SecureConnection);
        OpcUa_GotoErrorIfBad(uStatus);
    }
#endif /* OPCUA_HAVE_HTTPS */
    else
    {
        uStatus = OpcUa_BadNotSupported;
        OpcUa_GotoError;
    }

    pChannel->pfCallback        = a_pfCallback;
    pChannel->pvCallbackData    = a_pCallbackData;

    /* create transport credential */
    pClientCredentials = (OpcUa_ClientCredential*)OpcUa_Alloc(sizeof(OpcUa_ClientCredential));
    OpcUa_GotoErrorIfAllocFailed(pClientCredentials);

    pClientCredentials->Credential.TheActuallyUsedCredential.messageSecurityMode         = a_messageSecurityMode;
    pClientCredentials->Credential.TheActuallyUsedCredential.nRequestedLifetime          = a_nRequestedLifetime;
    pClientCredentials->Credential.TheActuallyUsedCredential.pClientCertificate          = a_pClientCertificate;
    pClientCredentials->Credential.TheActuallyUsedCredential.pClientPrivateKey           = a_pClientPrivateKey;
    pClientCredentials->Credential.TheActuallyUsedCredential.pkiConfig                   = a_pPKIConfig;
    pClientCredentials->Credential.TheActuallyUsedCredential.pRequestedSecurityPolicyUri = a_pRequestedSecurityPolicyUri;
    pClientCredentials->Credential.TheActuallyUsedCredential.pServerCertificate          = a_pServerCertificate;

    /* lock the session until the request is sent */
#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);
    bLocked = OpcUa_False;
#endif

    /* connect asynchronously to the server - the remaining stuff is done in the callback */
    uStatus = pChannel->SecureConnection->Connect(  pChannel->SecureConnection,
                                                    &(pChannel->Url),
                                                    pClientCredentials,
                                                    pChannel->NetworkTimeout,
                                                    OpcUa_Channel_OnNotify,
                                                    (OpcUa_Void*)pChannel);

    OpcUa_Free(pClientCredentials);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* lock the session until the request is sent */
#if OPCUA_USE_SYNCHRONISATION
    if(bLocked != OpcUa_False)
    {
        OPCUA_P_MUTEX_UNLOCK(pChannel->Mutex);
    }
#endif

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Channel_Connect
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Channel_Connect( OpcUa_Channel                               a_hChannel,
                                        OpcUa_StringA                               a_sUrl,
                                        OpcUa_Channel_PfnConnectionStateChanged*    a_pfCallback,
                                        OpcUa_Void*                                 a_pvCallbackData,
                                        OpcUa_ByteString*                           a_pClientCertificate,
                                        OpcUa_Key*                                  a_pClientPrivateKey,
                                        OpcUa_ByteString*                           a_pServerCertificate,
                                        OpcUa_Void*                                 a_pPKIConfig,
                                        OpcUa_String*                               a_pRequestedSecurityPolicyUri,
                                        OpcUa_Int32                                 a_nRequestedLifetime,
                                        OpcUa_MessageSecurityMode                   a_messageSecurityMode,
                                        OpcUa_UInt32                                a_nNetworkTimeout)
{
    OpcUa_AsyncCallState*   pAsyncState = OpcUa_Null;
    OpcUa_InternalChannel*  pInternalChannel = (OpcUa_InternalChannel*)(a_hChannel);

OpcUa_InitializeStatus(OpcUa_Module_Channel, "Connect");

    OpcUa_ReturnErrorIfArgumentNull(a_hChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_sUrl);

    OpcUa_ReferenceParameter(a_pfCallback);
    OpcUa_ReferenceParameter(a_pvCallbackData);

    /* create waithandle */
    uStatus =  OpcUa_AsyncCallState_Create(a_hChannel, OpcUa_Null, OpcUa_Null, &pAsyncState);
    OpcUa_ReturnErrorIfBad(uStatus);

    /* call the async connect */
    uStatus = OpcUa_Channel_BeginConnect(   a_hChannel,
                                            a_sUrl,
                                            a_pClientCertificate,
                                            a_pClientPrivateKey,
                                            a_pServerCertificate,
                                            a_pPKIConfig,
                                            a_pRequestedSecurityPolicyUri,
                                            a_nRequestedLifetime,
                                            a_messageSecurityMode,
                                            a_nNetworkTimeout,
                                            OpcUa_Channel_InternalConnectComplete,
                                            (OpcUa_Void*)pAsyncState);
    OpcUa_GotoErrorIfBad(uStatus);

    /* ************************ wait for completion ************************** */
#if OPCUA_MULTITHREADED
    uStatus = OpcUa_AsyncCallState_WaitForCompletion(   pAsyncState,
                                                        a_nNetworkTimeout);

    if(OpcUa_IsEqual(OpcUa_BadTimeout))
    {
        /* this error was created locally. connect may happen and we need to reset the pipe */
        OpcUa_Channel_Disconnect(a_hChannel);
    }

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_LOCK(pInternalChannel->Mutex);
#endif

    /* take care of possible race condition */
    if(OpcUa_IsGood(uStatus))
    {
        uStatus = pAsyncState->Status;
    }

    if(OpcUa_IsGood(uStatus))
    {
        pInternalChannel->pfCallback        = a_pfCallback;
        pInternalChannel->pvCallbackData    = a_pvCallbackData;
    }

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(pInternalChannel->Mutex);
#endif

    OpcUa_AsyncCallState_Delete(&pAsyncState);

#else
    uStatus = OpcUa_GoodCompletesAsynchronously;
#endif
    /* ********************** end wait for completion ************************ */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* destroy async handle */
    if(pAsyncState != OpcUa_Null)
    {
        OpcUa_AsyncCallState_Delete(&pAsyncState);
    }

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_Channel_GetUsedServerCertificate
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_GetUsedServerCertificate(
    OpcUa_Channel      a_hChannel,
    OpcUa_ByteString** a_ppUsedServerCertificate,
    OpcUa_StatusCode*  a_pValidationResult)
{
    OpcUa_InternalChannel*  pChannel = (OpcUa_InternalChannel*)(a_hChannel);

OpcUa_InitializeStatus(OpcUa_Module_Channel, "GetUsedServerCertificate");

    OpcUa_GotoErrorIfArgumentNull(a_hChannel);

#ifdef OPCUA_HAVE_HTTPS
    if(!OpcUa_String_StrnCmp(  &(pChannel->Url),
                               OpcUa_String_FromCString("https:"),
                               6,
                               OpcUa_True))
    {
        uStatus = OpcUa_HttpsConnection_GetUsedServerCertificate(pChannel->SecureConnection,
                                                                 a_ppUsedServerCertificate,
                                                                 a_pValidationResult);
    }
    else
#endif /* OPCUA_HAVE_HTTPS */
    {
        uStatus = OpcUa_BadNotSupported;
        OpcUa_GotoError;
    }


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_CLIENTAPI */
