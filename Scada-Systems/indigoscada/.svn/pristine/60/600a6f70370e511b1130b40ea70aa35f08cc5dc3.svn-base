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

/* platform */
#include <opcua.h>

#ifdef OPCUA_HAVE_SERVERAPI

/* core */
#include <opcua_mutex.h>

/* types */
#include <opcua_types.h>

/* serializing */
#include <opcua_binaryencoder.h>

/* communication */
#include <opcua_securelistener.h>
#include <opcua_tcplistener.h>

#ifdef OPCUA_HAVE_HTTPS
#include <opcua_httpslistener.h>
#endif /* OPCUA_HAVE_HTTPS */

/* self */
#include <opcua_endpoint.h>
#include <opcua_servicetable.h>
#include <opcua_endpoint_internal.h>

/** @brief Output stream gets allocated before calling the application if set to OPCUA_CONFIG_YES.
  * YES is the default behavior from prior versions but with it activated every open request carries
  * memory in size of one transport chunk with it. Having a lot of open request pending async in the
  * server leads to a lot of consumed memory. (OPCUA_CONFIG_NO is preferred)
  */
#define OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM OPCUA_CONFIG_YES

/**
 * @brief Processes a request recieved on an endpoint.
 *
 * @param hEndpoint   [in] The endpoint which received the request.
 * @param pConnection [in] The connection which is the source of the request.
 * @param pIstrm      [in] The stream used to read the request.
 */
static OpcUa_StatusCode OpcUa_Endpoint_BeginProcessRequest(
    OpcUa_Endpoint              hEndpoint,
    OpcUa_Handle                pConnection,
    OpcUa_InputStream**         ppIstrm);

/**
 * @brief Returns a pointer to the function that implements the service.
 *
 * @param hEndpoint [in]  The endpoint which received the request.
 * @param hContext  [in]  The context to for a request.
 * @param ppInvoke  [out] A pointer to the service function.
 */
OpcUa_StatusCode OpcUa_Endpoint_GetServiceFunction(
    OpcUa_Endpoint           hEndpoint,
    OpcUa_Handle             hContext,
    OpcUa_PfnInvokeService** ppInvoke);


/*============================================================================
 * OpcUa_EndpointContext
 *===========================================================================*/
/**
  @brief Stores the context for a request received from a client.
*/
struct _OpcUa_EndpointContext
{
#if !OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM
    /** @brief The stream to use to write the response to. */
    OpcUa_InputStream*      pIstrm;

    /** @brief The stream to use to write the response to. */
    OpcUa_Handle            hConnection;
#endif
    /** @brief The stream to use to write the response to. */
    OpcUa_OutputStream*     pOstrm;

    /** @brief The service definition associated with the request. */
    OpcUa_ServiceType       ServiceType;

    /** @brief The id of the corresponding securechannel. */
    OpcUa_UInt32            uSecureChannelId;
};

typedef struct _OpcUa_EndpointContext OpcUa_EndpointContext;

/*============================================================================
 * OpcUa_SupportedServiceTypes
 *===========================================================================*/
extern struct _OpcUa_ServiceType* OpcUa_SupportedServiceTypes[];

/*============================================================================
 ** @brief Global table of known types.
 *===========================================================================*/
extern OpcUa_EncodeableTypeTable OpcUa_ProxyStub_g_EncodeableTypes;

/*============================================================================
 ** @brief Global table of supported namespaces.
 *===========================================================================*/
extern OpcUa_StringTable OpcUa_ProxyStub_g_NamespaceUris;

/*============================================================================
 * OpcUa_Endpoint_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_Create( OpcUa_Endpoint*                     a_phEndpoint,
                                        OpcUa_Endpoint_SerializerType       a_eSerializerType,
                                        OpcUa_ServiceType**                 a_pSupportedServices)
{
    OpcUa_EndpointInternal* pEndpointInt            = OpcUa_Null;
    OpcUa_ServiceType**     pSupportedServiceTypes  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_phEndpoint);

    if(a_pSupportedServices != OpcUa_Null)
    {
        pSupportedServiceTypes = a_pSupportedServices;
    }
    else
    {
        pSupportedServiceTypes = &OpcUa_SupportedServiceTypes[0];
    }

    *a_phEndpoint = OpcUa_Null;

    /* increment counter */
    OpcUa_ProxyStub_RegisterEndpoint();

    pEndpointInt = (OpcUa_EndpointInternal*)OpcUa_Alloc(sizeof(OpcUa_EndpointInternal));
    OpcUa_GotoErrorIfAllocFailed(pEndpointInt);
    OpcUa_MemSet(pEndpointInt, 0, sizeof(OpcUa_EndpointInternal));

    pEndpointInt->State = eOpcUa_Endpoint_State_Closed;

    /* initialize serializer */
    switch(a_eSerializerType)
    {
    case OpcUa_Endpoint_SerializerType_Binary:
        {
            pEndpointInt->EncoderType = OpcUa_EncoderType_Binary;
            break;
        }
    case OpcUa_Endpoint_SerializerType_Xml:
        {
            uStatus = OpcUa_BadNotImplemented;
            /* pEndpointInt->EncoderType = OpcUa_EncoderType_Xml; */
            break;
        }
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
        }
    }
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_MUTEX_CREATE(&(pEndpointInt->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    /* initialize supported services */
    uStatus = OpcUa_ServiceTable_AddTypes(&(pEndpointInt->SupportedServices), pSupportedServiceTypes);
    OpcUa_GotoErrorIfBad(uStatus);

    *a_phEndpoint = pEndpointInt;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Endpoint_Delete(a_phEndpoint);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_GetMessageSecureChannelId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_GetMessageSecureChannelId(  OpcUa_Endpoint  a_hEndpoint,
                                                            OpcUa_Handle    a_hContext,
                                                            OpcUa_UInt32*   a_pSecureChannelId)
{
    OpcUa_EndpointContext* pContext = (OpcUa_EndpointContext*)a_hContext;

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannelId);

    *a_pSecureChannelId = pContext->uSecureChannelId;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Endpoint_GetMessageSecureChannelSecurityPolicy
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_GetMessageSecureChannelSecurityPolicy(
    OpcUa_Endpoint                                  a_hEndpoint,
    OpcUa_Handle                                    a_hContext,
    OpcUa_Endpoint_SecurityPolicyConfiguration*     a_pSecurityPolicy)
{
    OpcUa_EndpointInternal* pEndpointInt    = (OpcUa_EndpointInternal*)a_hEndpoint;
    OpcUa_EndpointContext*  pContext        = (OpcUa_EndpointContext*)a_hContext;

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecurityPolicy);

    OpcUa_MemSet(a_pSecurityPolicy, 0, sizeof(OpcUa_Endpoint_SecurityPolicyConfiguration));

#ifdef OPCUA_HAVE_HTTPS
    /* In case of HTTPS, the transport listener is not set. */
    if(pEndpointInt->TransportListener == OpcUa_Null)
    {
        return OpcUa_HttpsListener_GetSecurityPolicyConfiguration(
                        pEndpointInt->SecureListener,
                        pContext->pOstrm,
                        a_pSecurityPolicy);
    }
    else
#endif /* OPCUA_HAVE_HTTPS */
    {
        return OpcUa_SecureListener_GetSecureChannelSecurityPolicyConfiguration(
                        pEndpointInt->SecureListener,
                        pContext->uSecureChannelId,
                        a_pSecurityPolicy);
    }
}

/*============================================================================
 * OpcUa_Endpoint_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Endpoint_Delete(OpcUa_Endpoint* a_phEndpoint)
{
    if(a_phEndpoint != OpcUa_Null && *a_phEndpoint != OpcUa_Null)
    {
        OpcUa_EndpointInternal* pEndpointInt = (OpcUa_EndpointInternal*)*a_phEndpoint;
        *a_phEndpoint = OpcUa_Null;

        OPCUA_P_MUTEX_LOCK(pEndpointInt->Mutex);
        OpcUa_Listener_Delete(&pEndpointInt->TransportListener);
        OpcUa_Listener_Delete(&pEndpointInt->SecureListener);
        OpcUa_Encoder_Delete(&pEndpointInt->Encoder);
        OpcUa_Decoder_Delete(&pEndpointInt->Decoder);
        OpcUa_String_Clear(&pEndpointInt->Url);
        OpcUa_ServiceTable_Clear(&pEndpointInt->SupportedServices);

        OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);

        OPCUA_P_MUTEX_DELETE(&(pEndpointInt->Mutex));

        OpcUa_Free(pEndpointInt);

         /* decrement counter */
        OpcUa_ProxyStub_DeRegisterEndpoint();
    }
}

/*============================================================================
 * OpcUa_Endpoint_OnNotify
 *==========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_OnNotify(   OpcUa_Listener*     a_pListener,
                                            OpcUa_Void*         a_pCallbackData,
                                            OpcUa_ListenerEvent a_eEvent,
                                            OpcUa_Handle        a_hConnection,
                                            OpcUa_InputStream** a_ppIstrm,
                                            OpcUa_StatusCode    a_uOperationStatus)
{
    OpcUa_EndpointInternal* pEndpointInt = (OpcUa_EndpointInternal*)a_pCallbackData;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "OnNotify");

    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallbackData);

    /* acquire the lock on the endpoint structure */
    OPCUA_P_MUTEX_LOCK(pEndpointInt->Mutex);

    switch(a_eEvent)
    {
    case OpcUa_ListenerEvent_Open:
        {
            pEndpointInt->Status = a_uOperationStatus;
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnNotify: Underlying listener raised open event!\n");
            break;
        }
    case OpcUa_ListenerEvent_Close:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnNotify: Underlying listener raised close event!\n");
            break;
        }
    case OpcUa_ListenerEvent_Request:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnNotify: Underlying listener raised request event!\n");

            uStatus = OpcUa_Endpoint_BeginProcessRequest(
                pEndpointInt,   /* the endpoint holding the service table */
                a_hConnection,  /* the connection object for identifiyng the communication partner */
                a_ppIstrm);      /* the stream for reading the request */

            OpcUa_GotoErrorIfBad(uStatus);

            break;
        }
    case OpcUa_ListenerEvent_UnexpectedError:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnNotify: Underlying listener raised unexpected error event!\n");
            break;
        }
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnNotify: Underlying listener raised unknown event!\n");
        }
    }

    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_Endpoint_OnNotify
 *==========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_OnSecureChannelEvent(   OpcUa_UInt32                            a_uSecureChannelId,
                                                        OpcUa_SecureListener_SecureChannelEvent a_eSecureChannelEvent,
                                                        OpcUa_StatusCode                        a_uStatus,
                                                        OpcUa_ByteString*                       a_pbsClientCertificate,
                                                        OpcUa_String*                           a_sSecurityPolicy,
                                                        OpcUa_UInt16                            a_uMessageSecurityModes,
                                                        OpcUa_Void*                             a_pCallbackData)
{
    OpcUa_EndpointInternal* pEndpoint = OpcUa_Null;

    /* get current endpoint */
    if(a_pCallbackData != OpcUa_Null)
    {
        pEndpoint = (OpcUa_EndpointInternal*)a_pCallbackData;

        /* call the endpoints callback function */
        if(pEndpoint->pfEndpointCallback)
        {
            switch(a_eSecureChannelEvent)
            {
                case eOpcUa_SecureListener_SecureChannelOpen:
                {
                    /* the .NET wrapper needs to have the security policy information passed when the channel
                       is created because it does have access to the endpoint context that normal applications use */
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnSecureChannelEvent: SecureChannel opened!\n");
                    pEndpoint->pfEndpointCallback(
                        pEndpoint,
                        pEndpoint->pvEndpointCallbackData,
                        eOpcUa_Endpoint_Event_SecureChannelOpened,
                        a_uStatus,
                        a_uSecureChannelId,
                        a_pbsClientCertificate,
                        a_sSecurityPolicy,
                        a_uMessageSecurityModes);

                    break;
                }

                case eOpcUa_SecureListener_SecureChannelClose:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnSecureChannelEvent: SecureChannel closed!\n");
                    pEndpoint->pfEndpointCallback(
                        pEndpoint,
                        pEndpoint->pvEndpointCallbackData,
                        eOpcUa_Endpoint_Event_SecureChannelClosed,
                        a_uStatus,
                        a_uSecureChannelId,
                        OpcUa_Null,
                        OpcUa_Null,
                        a_uMessageSecurityModes);

                    break;
                }
                case eOpcUa_SecureListener_SecureChannelRenew:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnSecureChannelEvent: SecureChannel renewed!\n");
                    pEndpoint->pfEndpointCallback(
                        pEndpoint,
                        pEndpoint->pvEndpointCallbackData,
                        eOpcUa_Endpoint_Event_SecureChannelRenewed,
                        a_uStatus,
                        a_uSecureChannelId,
                        OpcUa_Null,
                        OpcUa_Null,
                        0);

                    break;
                }

                default:
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_OnSecureChannelEvent: unknown SecureChannel event!\n");
                    pEndpoint->pfEndpointCallback(
                        pEndpoint,
                        pEndpoint->pvEndpointCallbackData,
                        eOpcUa_Endpoint_Event_Invalid,
                        a_uStatus,
                        a_uSecureChannelId,
                        OpcUa_Null,
                        OpcUa_Null,
                        0);

                    break;
                }
            }
        }
    }

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Endpoint_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_Open(   OpcUa_Endpoint                              a_hEndpoint,
                                        OpcUa_StringA                               a_sUrl,
                                        OpcUa_Boolean                               a_bListenOnAllInterfaces,
                                        OpcUa_Endpoint_PfnEndpointCallback*         a_pfEndpointCallback,
                                        OpcUa_Void*                                 a_pvEndpointCallbackData,
                                        OpcUa_ByteString*                           a_pServerCertificate,
                                        OpcUa_Key*                                  a_pServerPrivateKey,
                                        OpcUa_Void*                                 a_pPKIConfig,
                                        OpcUa_UInt32                                a_nNoOfSecurityPolicies,
                                        OpcUa_Endpoint_SecurityPolicyConfiguration* a_pSecurityPolicies)
{
    OpcUa_EndpointInternal* pEndpointInt = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "Open");

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);

    pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    /* acquire the lock on the endpoint structure */
    OPCUA_P_MUTEX_LOCK(pEndpointInt->Mutex);

    pEndpointInt->State = eOpcUa_Endpoint_State_Open;

    /* attach the given url to the endpoint (make copy) */
    uStatus = OpcUa_String_AttachToString(    a_sUrl,
                                              OPCUA_STRINGLENZEROTERMINATED,
                                              0,
                                              OpcUa_True,
                                              OpcUa_False,
                                              &(pEndpointInt->Url));
    OpcUa_GotoErrorIfBad(uStatus);

    /* create the encoder and decoder */
    if (pEndpointInt->EncoderType == OpcUa_EncoderType_Binary)
    {
        /* create encoder */
        uStatus = OpcUa_BinaryEncoder_Create(&pEndpointInt->Encoder);
        OpcUa_GotoErrorIfBad(uStatus);

        /* create decoder */
        uStatus = OpcUa_BinaryDecoder_Create(&pEndpointInt->Decoder);
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else /* the encoding type is not supported */
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }

    /* select the connect type based on the url scheme */
    if(!OpcUa_String_StrnCmp(   &(pEndpointInt->Url),
                                OpcUa_String_FromCString("opc.tcp:"),
                                (OpcUa_UInt32)8,
                                OpcUa_True))
    {
        uStatus = OpcUa_TcpListener_Create(     &pEndpointInt->TransportListener);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_SecureListener_Create(  pEndpointInt->TransportListener,
                                                pEndpointInt->Decoder,
                                                pEndpointInt->Encoder,
                                                &OpcUa_ProxyStub_g_NamespaceUris,
                                                &OpcUa_ProxyStub_g_EncodeableTypes,
                                                a_pServerCertificate,
                                                a_pServerPrivateKey,
                                                a_pPKIConfig,
                                                a_nNoOfSecurityPolicies,
                                                a_pSecurityPolicies,
                                                OpcUa_Endpoint_OnSecureChannelEvent,
                                                (OpcUa_Void*)pEndpointInt,
                                                &pEndpointInt->SecureListener);
        OpcUa_GotoErrorIfBad(uStatus);
    }
#ifdef OPCUA_HAVE_HTTPS
    else if(!OpcUa_String_StrnCmp(  &(pEndpointInt->Url),
                                    OpcUa_String_FromCString("https:"),
                                    (OpcUa_UInt32)6,
                                    OpcUa_True))

    {
        uStatus = OpcUa_HttpsListener_Create(   a_pServerCertificate,
                                                a_pServerPrivateKey,
                                                a_pPKIConfig,
                                                OpcUa_Endpoint_OnSecureChannelEvent,
                                                (OpcUa_Void*)pEndpointInt,
                                                &pEndpointInt->SecureListener);
        OpcUa_GotoErrorIfBad(uStatus);
    }
#endif /* OPCUA_HAVE_HTTPS */
    /* the endpoint type is not supported */
    else
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }

    pEndpointInt->pfEndpointCallback        = a_pfEndpointCallback;
    pEndpointInt->pvEndpointCallbackData    = a_pvEndpointCallbackData;

    pEndpointInt->Status = OpcUa_BadWaitingForResponse;


    /* open the endpoint. */
    uStatus = OpcUa_Listener_Open(  pEndpointInt->SecureListener,
                                    &(pEndpointInt->Url),
                                    a_bListenOnAllInterfaces,
                                    OpcUa_Endpoint_OnNotify,
                                    (OpcUa_Void*)a_hEndpoint);

    OpcUa_GotoErrorIfBad(uStatus);

    /* release lock */
    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Listener_Delete(&pEndpointInt->TransportListener);
    OpcUa_Listener_Delete(&pEndpointInt->SecureListener);
    OpcUa_Encoder_Delete(&pEndpointInt->Encoder);
    OpcUa_Decoder_Delete(&pEndpointInt->Decoder);
    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_Close(OpcUa_Endpoint a_hEndpoint)
{
    OpcUa_EndpointInternal* pEndpointInt = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);

    pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    /* acquire the lock on the endpoint structure */
    OPCUA_P_MUTEX_LOCK(pEndpointInt->Mutex);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_Close: Cleaning up!\n");

    pEndpointInt->State = eOpcUa_Endpoint_State_Closed;

    /* close listener */
    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex); /* unlocking for potential callbacks */
    uStatus = OpcUa_Listener_Close(pEndpointInt->SecureListener);
    OPCUA_P_MUTEX_LOCK(pEndpointInt->Mutex);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_Close: Done!\n");

    /* release lock */
    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(pEndpointInt->Mutex);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_ReadRequest
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_Endpoint_ReadRequest(
    OpcUa_Endpoint          a_hEndpoint,
    OpcUa_InputStream*      a_pIstrm,
    OpcUa_Void**            a_ppRequest,
    OpcUa_EncodeableType**  a_ppRequestType)
{
    OpcUa_EndpointInternal* pEndpointInt = OpcUa_Null;
    OpcUa_Decoder*          pDecoder    = OpcUa_Null;
    OpcUa_MessageContext    cContext;
    OpcUa_Handle            hDecodeContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "ReadRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_ppRequest);
    OpcUa_ReturnErrorIfArgumentNull(a_ppRequestType);

    pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    pDecoder = pEndpointInt->Decoder;
    OpcUa_ReturnErrorIfArgumentNull(pDecoder);

    *a_ppRequest     = OpcUa_Null;
    *a_ppRequestType = OpcUa_Null;

    pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    /* initialize context */
    OpcUa_MessageContext_Initialize(&cContext);

    cContext.KnownTypes    = &OpcUa_ProxyStub_g_EncodeableTypes;
    cContext.NamespaceUris = &OpcUa_ProxyStub_g_NamespaceUris;

    /* create decoder */
    uStatus = pDecoder->Open(pDecoder, a_pIstrm, &cContext, &hDecodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* decode message */
    uStatus = pDecoder->ReadMessage((struct _OpcUa_Decoder *)hDecodeContext, a_ppRequestType, a_ppRequest);
    OpcUa_GotoErrorIfBad(uStatus);

    /* close decoder */
    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_MessageContext_Clear(&cContext);
    OpcUa_EncodeableObject_Delete(*a_ppRequestType, a_ppRequest);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_WriteResponse
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_Endpoint_WriteResponse(
    OpcUa_Endpoint          a_hEndpoint,
    OpcUa_OutputStream**    a_ppOstrm,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_Void*             a_pResponse,
    OpcUa_EncodeableType*   a_pResponseType)
{
    OpcUa_EndpointInternal* pEndpointInt        = OpcUa_Null;
    OpcUa_Encoder*          pEncoder            = OpcUa_Null;
    OpcUa_MessageContext    cContext;
    OpcUa_Handle            hEncodeContext      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "WriteResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(*a_ppOstrm);

    pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    if(OpcUa_IsGood(a_uStatus))
    {
        OpcUa_ReturnErrorIfArgumentNull(a_pResponse);
        OpcUa_ReturnErrorIfArgumentNull(a_pResponseType);

        pEncoder = pEndpointInt->Encoder;
        OpcUa_ReturnErrorIfArgumentNull(pEncoder);

        OpcUa_MessageContext_Initialize(&cContext);

        cContext.KnownTypes         = &OpcUa_ProxyStub_g_EncodeableTypes;
        cContext.NamespaceUris      = &OpcUa_ProxyStub_g_NamespaceUris;
        cContext.AlwaysCheckLengths = OPCUA_SERIALIZER_CHECKLENGTHS;

        /* create encoder */
        uStatus = pEncoder->Open(pEncoder, *a_ppOstrm, &cContext, &hEncodeContext);
        OpcUa_GotoErrorIfBad(uStatus);

        /* encode message */
        uStatus = pEncoder->WriteMessage((struct _OpcUa_Encoder*)hEncodeContext, a_pResponse, a_pResponseType);
        OpcUa_GotoErrorIfBad(uStatus);

        /* delete encoder */
        OpcUa_Encoder_Close(pEncoder, &hEncodeContext);

        OpcUa_MessageContext_Clear(&cContext);
    }
    else
    {
        /* no encoding */
    }

    /* send response */
    uStatus = OpcUa_Listener_EndSendResponse(   pEndpointInt->SecureListener,
                                                a_uStatus,
                                                a_ppOstrm);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    a_uStatus = uStatus;

    if(OpcUa_IsNotEqual(OpcUa_BadDisconnect) && OpcUa_IsNotEqual(OpcUa_BadNoCommunication) && OpcUa_IsNotEqual(OpcUa_BadCommunicationError))
    {
        /* send response bad (no encryption!) */
        uStatus = OpcUa_Listener_EndSendResponse(   pEndpointInt->SecureListener,
                                                    a_uStatus,
                                                    a_ppOstrm);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Endpoint_WriteResponse: Response could not be sent; connection lost!\n");
    }

    OpcUa_Encoder_Close(pEncoder, &hEncodeContext);
    OpcUa_MessageContext_Clear(&cContext);

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_Endpoint_DeleteContext
 *===========================================================================*/
static OpcUa_Void OpcUa_Endpoint_DeleteContext( OpcUa_Endpoint a_hEndpoint,
                                                OpcUa_Handle*  a_phContext)
{
    if (a_hEndpoint != OpcUa_Null && a_phContext != OpcUa_Null)
    {
        OpcUa_EndpointContext* pContext = OpcUa_Null;

        pContext = (OpcUa_EndpointContext*)*a_phContext;

        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_DeleteContext!\n");

        /* deletes the secure stream of this context */
        /* the inner stream should already be deleted by the endsendresponse */

        OpcUa_Stream_Delete((OpcUa_Stream**)&pContext->pOstrm);
#if !OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM
        OpcUa_Stream_Delete((OpcUa_Stream**)&pContext->pIstrm);
#endif
        OpcUa_Free(pContext);

        *a_phContext = OpcUa_Null;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Endpoint_DeleteContext: NULL!\n");
    }
}

/*============================================================================
 * OpcUa_Endpoint_BeginProcessRequest
 *===========================================================================*/
/* INFO: Endpoint is locked during this call. */
static OpcUa_StatusCode OpcUa_Endpoint_BeginProcessRequest( OpcUa_Endpoint      a_hEndpoint,
                                                            OpcUa_Handle        a_hConnection,
                                                            OpcUa_InputStream** a_ppIstrm)
{
    OpcUa_EndpointInternal* pEndpointInt    = OpcUa_Null;
    OpcUa_Void*             pRequest        = OpcUa_Null;
    OpcUa_EncodeableType*   pRequestType    = OpcUa_Null;
    OpcUa_EndpointContext*  pContext        = OpcUa_Null;

#if !OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM
    OpcUa_Buffer            Buffer;
#endif /* !OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM */

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "BeginProcessRequest");

    /* check arguments */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_ppIstrm);

    pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    /* check state of endpoint - reject requests if not open; we're not trying to answer here */
    OpcUa_ReturnErrorIfTrue(pEndpointInt->State != eOpcUa_Endpoint_State_Open, OpcUa_BadShutdown);

    pContext = (OpcUa_EndpointContext*)OpcUa_Alloc(sizeof(OpcUa_EndpointContext));
    OpcUa_ReturnErrorIfAllocFailed(pContext);
    OpcUa_MemSet(pContext, 0, sizeof(OpcUa_EndpointContext));

    /* decode the request */
    uStatus = OpcUa_Endpoint_ReadRequest(   a_hEndpoint,
                                            *a_ppIstrm,
                                            &pRequest,
                                            &pRequestType);

    if((OpcUa_IsBad(uStatus)) || (pRequest == OpcUa_Null) || (pRequestType == OpcUa_Null))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Endpoint_BeginProcessRequest: ERROR READING REQUEST! Status 0x%08X \n", uStatus);
        pEndpointInt->pfEndpointCallback(   pEndpointInt,
                                            pEndpointInt->pvEndpointCallbackData,
                                            eOpcUa_Endpoint_Event_DecoderError,
                                            uStatus,
                                            0,
                                            OpcUa_Null,
                                            OpcUa_Null,
                                            OPCUA_ENDPOINT_MESSAGESECURITYMODE_INVALID);
        OpcUa_GotoError;
    }

    /* Next call is only valid if OPC UA Secure Conversation is used. */
    /* In case of HTTPS, the transport listener is not set. */
    if(pEndpointInt->TransportListener != OpcUa_Null)
    {
        /* Get securechannel Id */
        uStatus = OpcUa_SecureListener_GetChannelId(pEndpointInt->SecureListener,
                                                    *a_ppIstrm,
                                                    &pContext->uSecureChannelId);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* find out whether the endpoint supports the service. */
    uStatus = OpcUa_ServiceTable_FindService(&pEndpointInt->SupportedServices, pRequestType->TypeId, &pContext->ServiceType);
    if(OpcUa_IsEqual(OpcUa_BadServiceUnsupported))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_Endpoint_BeginProcessRequest: Unsupported Service with RequestTypeId %u requested! (HINT: %s)\n", pRequestType->TypeId, pRequestType->TypeName);
        pEndpointInt->pfEndpointCallback(   pEndpointInt,
                                            pEndpointInt->pvEndpointCallbackData,
                                            eOpcUa_Endpoint_Event_UnsupportedServiceRequested,
                                            uStatus,
                                            pContext->uSecureChannelId,
                                            OpcUa_Null,
                                            OpcUa_Null,
                                            OPCUA_ENDPOINT_MESSAGESECURITYMODE_INVALID);
        OpcUa_GotoError;
    }
    else if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Endpoint_BeginProcessRequest: Could not find service handler (0x%08X)\n", uStatus);
        OpcUa_GotoError;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_Endpoint_BeginProcessRequest: Service with RequestTypeId %u requested! (HINT: %s)\n", pRequestType->TypeId, pRequestType->TypeName);
    }

#if OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM

    /* Initialize Response Stream */
    uStatus = OpcUa_Listener_BeginSendResponse( pEndpointInt->SecureListener,
                                                a_hConnection,
                                                a_ppIstrm,
                                                &pContext->pOstrm);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Endpoint_BeginProcessRequest: Could not allocate response data! (0x%08X)\n", uStatus);
        OpcUa_GotoError;
    }

#else /* OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM */

    pContext->hConnection = a_hConnection;
    pContext->pIstrm = *a_ppIstrm;
    *a_ppIstrm = OpcUa_Null;

    /* clean up the stream buffers */
    pContext->pIstrm->DetachBuffer((OpcUa_Stream *)pContext->pIstrm, &Buffer);
    OpcUa_Buffer_Clear(&Buffer);

#endif /* OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM */

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_BeginProcessRequest: Invoking service handler!\n");

    uStatus = pContext->ServiceType.BeginInvoke(    a_hEndpoint,
                                                    pContext,
                                                    &pRequest,
                                                    pRequestType);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_BeginProcessRequest: Service handler returned! (0x%08X)\n", uStatus);

    /* does nothing if callee before nulled the parameter */
    if(pRequest != OpcUa_Null)
    {
        OpcUa_EncodeableObject_Delete(pRequestType, &pRequest);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* no cleaning up, if status is good -> everything handled before */
    if(pContext != OpcUa_Null && pContext->pOstrm != OpcUa_Null)
    {
        /* undo BeginSendResponse */
        uStatus = OpcUa_Listener_AbortSendResponse( pEndpointInt->SecureListener,
                                                    uStatus,
                                                    OpcUa_Null,
                                                    (OpcUa_OutputStream**)&(pContext->pOstrm));
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Endpoint_BeginProcessRequest: Not able to create/send response. (0x%08X)\n", uStatus);
    }

    /* delete deserialized request */
    OpcUa_EncodeableObject_Delete(pRequestType, &pRequest);

    /* delete message context */
    OpcUa_Endpoint_DeleteContext(a_hEndpoint, (OpcUa_Handle*)&pContext);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_BeginSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_BeginSendResponse(  OpcUa_Endpoint         a_hEndpoint,
                                                    OpcUa_Handle           a_hContext,
                                                    OpcUa_Void**           a_ppResponse,
                                                    OpcUa_EncodeableType** a_ppResponseType)
{
    OpcUa_EndpointContext* pContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "BeginSendResponse");

    /* check arguments */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_ppResponse);

    *a_ppResponse = OpcUa_Null;
    *a_ppResponseType = OpcUa_Null;

    pContext = (OpcUa_EndpointContext*)a_hContext;

    /* get the response type */
    *a_ppResponseType = pContext->ServiceType.ResponseType;

#if !OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM
    /* Initialize Response Stream */
    uStatus = OpcUa_Listener_BeginSendResponse( ((OpcUa_EndpointInternal*)a_hEndpoint)->SecureListener,
                                                pContext->hConnection,
                                                &pContext->pIstrm,
                                                &pContext->pOstrm);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_Endpoint_BeginSendResponse: Could not allocate response data! (0x%08X)\n", uStatus);
        OpcUa_GotoError;
    }
#endif /* !OPCUA_ENDPOINT_PREALLOCATE_RESPONSESTREAM */

    /* allocate instance of the encodeable type */
    uStatus = OpcUa_EncodeableObject_Create(*a_ppResponseType, a_ppResponse);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    *a_ppResponse = OpcUa_Null;
    *a_ppResponseType = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_CancelSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_CancelSendResponse(
    OpcUa_Endpoint        a_hEndpoint,
    OpcUa_StatusCode      a_uStatus,
    OpcUa_String*         a_psReason,
    OpcUa_Handle*         a_phContext)
{
    OpcUa_EndpointContext*  pContext     = OpcUa_Null;
    OpcUa_EndpointInternal* pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "CancelSendResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_phContext);
    OpcUa_ReturnErrorIfArgumentNull(*a_phContext);

    pContext = (OpcUa_EndpointContext*)*a_phContext;

    OpcUa_Listener_AbortSendResponse(   pEndpointInt->SecureListener,
                                        a_uStatus,
                                        a_psReason,
                                        &(pContext->pOstrm));

    OpcUa_Endpoint_DeleteContext(a_hEndpoint, a_phContext);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
/*============================================================================
 * OpcUa_Endpoint_EndSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_EndSendResponse(
    OpcUa_Endpoint        a_hEndpoint,
    OpcUa_Handle*         a_phContext,
    OpcUa_StatusCode      a_uStatusCode,
    OpcUa_Void*           a_pResponse,
    OpcUa_EncodeableType* a_pResponseType)
{
    OpcUa_EndpointContext* pContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "EndSendResponse");

    /* check arguments */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_phContext);
    OpcUa_ReturnErrorIfArgumentNull(*a_phContext);

    OPCUA_ENDPOINT_CHECKOPEN(a_hEndpoint);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_Endpoint_EndSendResponse (0x%08X)!\n", a_uStatusCode);

    if(OpcUa_IsBad(a_uStatusCode))
    {
        OpcUa_Endpoint_CancelSendResponse(  a_hEndpoint,
                                            a_uStatusCode,
                                            OpcUa_Null,
                                            a_phContext);
    }
    else
    {
        /* get the context */
        pContext = (OpcUa_EndpointContext*)*a_phContext;

        /* send the response */
        uStatus = OpcUa_Endpoint_WriteResponse( a_hEndpoint,
                                                &(pContext->pOstrm),
                                                a_uStatusCode,
                                                a_pResponse,
                                                a_pResponseType);
        OpcUa_GotoErrorIfBad(uStatus);

        OpcUa_Endpoint_DeleteContext(a_hEndpoint, a_phContext);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Endpoint_EndSendResponse: Error 0x%08X! Cancelling response!\n", uStatus);

    OpcUa_Endpoint_CancelSendResponse(  a_hEndpoint,
                                        OpcUa_Good,
                                        OpcUa_Null,
                                        a_phContext);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_GetServiceFunction
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_GetServiceFunction( OpcUa_Endpoint           a_hEndpoint,
                                                    OpcUa_Handle             a_hContext,
                                                    OpcUa_PfnInvokeService** a_ppInvoke)
{
    OpcUa_EndpointContext* pContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "GetServiceFunction");

    /* check arguments */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_ppInvoke);

    /* get the context */
    pContext = (OpcUa_EndpointContext*)a_hContext;

    *a_ppInvoke = pContext->ServiceType.Invoke;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_UpdateServiceFunctions
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_UpdateServiceFunctions( OpcUa_Endpoint              a_hEndpoint,
                                                        OpcUa_UInt32                 a_uRequestTypeId,
                                                        OpcUa_PfnBeginInvokeService* a_pBeginInvoke,
                                                        OpcUa_PfnInvokeService*      a_pInvoke)
{
    OpcUa_UInt32            ii              = 0;
    OpcUa_ServiceTable*     pTable          = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Endpoint, "UpdateServiceFunctions");

    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);

    pTable = &(((OpcUa_EndpointInternal*)a_hEndpoint)->SupportedServices);

    /* find out whether the endpoint supports the service. */
    for(ii = 0; ii < pTable->Count; ii++)
    {
        if(pTable->Entries[ii].RequestTypeId == a_uRequestTypeId)
        {
            /* update asynchronous function */
            if(a_pBeginInvoke != OpcUa_Null)
            {
                pTable->Entries[ii].BeginInvoke = a_pBeginInvoke;
            }

            /* update synchronous function */
            if(a_pInvoke != OpcUa_Null)
            {
                pTable->Entries[ii].Invoke = a_pInvoke;
            }

            break;
        }
    }

    /* service not found */
    if(ii == pTable->Count)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotFound);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Endpoint_GetCallbackData
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_GetCallbackData(OpcUa_Endpoint  a_hEndpoint,
                                                OpcUa_Void**    a_ppvCallbackData)
{
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_ppvCallbackData);

    *a_ppvCallbackData = OpcUa_Null;

    OPCUA_P_MUTEX_LOCK(((OpcUa_EndpointInternal*)a_hEndpoint)->Mutex);

    *a_ppvCallbackData = ((OpcUa_EndpointInternal*)a_hEndpoint)->pvEndpointCallbackData;

    OPCUA_P_MUTEX_UNLOCK(((OpcUa_EndpointInternal*)a_hEndpoint)->Mutex);

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Endpoint_GetServiceFunction
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Endpoint_GetPeerInfo( OpcUa_Endpoint           a_hEndpoint,
                                             OpcUa_Handle             a_hContext,
                                             OpcUa_String*            a_pPeerInfo)
{
    OpcUa_EndpointContext* pContext = OpcUa_Null;
    OpcUa_EndpointInternal* pEndpointInt = (OpcUa_EndpointInternal*)a_hEndpoint;

    /* check arguments */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pPeerInfo);

    /* get the context */
    pContext = (OpcUa_EndpointContext*)a_hContext;

#ifdef OPCUA_HAVE_HTTPS
    /* In case of HTTPS, the transport listener is not set. */
    if(pEndpointInt->TransportListener == OpcUa_Null)
    {
        return OpcUa_HttpsListener_GetPeerInfo(
                        pEndpointInt->SecureListener,
                        pContext->pOstrm,
                        a_pPeerInfo);
    }
#endif /* OPCUA_HAVE_HTTPS */

    return  OpcUa_SecureListener_GetPeerInfo(
        pEndpointInt->SecureListener,
        pContext->uSecureChannelId,
        a_pPeerInfo);
}

#endif /* OPCUA_HAVE_SERVERAPI */

