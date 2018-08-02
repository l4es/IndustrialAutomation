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

#ifndef _OpcUa_Channel_H_
#define _OpcUa_Channel_H_ 1

#ifdef OPCUA_HAVE_CLIENTAPI

OPCUA_BEGIN_EXTERN_C

#ifdef OPCUA_SUPPORT_PREENCODED_MESSAGES
#include <opcua_stream.h>
#include <opcua_connection.h>
#endif

/**
 * @brief The Channel Handle.
 */
typedef OpcUa_Void* OpcUa_Channel;

/**
 * @brief Types of events that can occur at a channel and get reported to the application.
 */
typedef enum eOpcUa_Channel_Event
{
    /** @brief Reserved/Invalid/Ignore */
    eOpcUa_Channel_Event_Invalid,
    /** @brief A secure channel has been connected. (ignore for sync api) */
    eOpcUa_Channel_Event_Connected,
    /** @brief A secure channel has been disconnected. */
    eOpcUa_Channel_Event_Disconnected
} OpcUa_Channel_Event;

/**
 * @brief Types of serializers supported by the endpoint.
 *
 * @see OpcUa_Channel_Create
 */
enum _OpcUa_Channel_SerializerType
{
    OpcUa_Channel_SerializerType_Invalid,
    OpcUa_Channel_SerializerType_Binary,
    OpcUa_Channel_SerializerType_Xml
};
typedef enum _OpcUa_Channel_SerializerType OpcUa_Channel_SerializerType;

/**
 * @brief Called by the stack to report an asynchronous connection event.
 *
 * @param pChannel         [in] The channel used to send the request.
 * @param pCallbackData    [in] The callback data specified when the request was sent.
 * @param eEvent           [in] The event that occurred.
 * @param uStatus          [in] The status code, with which the operation completed.
 */
typedef OpcUa_StatusCode (OpcUa_Channel_PfnConnectionStateChanged)(
    OpcUa_Channel       hChannel,
    OpcUa_Void*         pCallbackData,
    OpcUa_Channel_Event eEvent,
    OpcUa_StatusCode    uStatus);

/**
 * @brief Creates a new channel.
 *
 * @param ppChannel    [out] The new channel.
 * @param eEncoderType [in]  The type of encoder to use for messages.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_Create(
    OpcUa_Channel*               phChannel,
    OpcUa_Channel_SerializerType eSerializerType);

/**
 * @brief Deletes a channel.
 *
 * @param ppChannel [in/out] The channel to delete.
 */
OPCUA_EXPORT OpcUa_Void OpcUa_Channel_Delete(
    OpcUa_Channel* phChannel);

/**
 * @brief Establishes a network connection with the server but does not create the session.
 *
 * @param pChannel                      [in] The channel to connect.
 * @param sUrl                          [in] The url of the server to connect to.
 * @param pfCallback                    [in] Function to call for channel event notification.
 * @param pvCallbackData                [in] Gets passed back to the application in pfCallback.
 * @param pClientCertificate            [in] The clients certificate.
 * @param pClientPrivateKey             [in] The clients private key matching the public key in the certificate.
 * @param pServerCertificate            [in] The certificate of the server.
 * @param pPKIConfig                    [in] Implementation dependent configuration for the PKI.
 * @param pRequestedSecurityPolicyUri   [in] URI defining the security parameter set applied to the connection.
 * @param nRequestedLifetime            [in] The requested lifetime for the security token.
 * @param messageSecurityMode           [in] The message security mode requested for the communication.
 * @param nNetworkTimeout               [in] The network timeout. Also used for disconnect.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_Connect(
    OpcUa_Channel                               hChannel,
    OpcUa_StringA                               sUrl,
    OpcUa_Channel_PfnConnectionStateChanged*    pfCallback,
    OpcUa_Void*                                 pvCallbackData,
    OpcUa_ByteString*                           pClientCertificate,
    OpcUa_Key*                                  pClientPrivateKey,
    OpcUa_ByteString*                           pServerCertificate,
    OpcUa_Void*                                 pPKIConfig,
    OpcUa_String*                               pRequestedSecurityPolicyUri,
    OpcUa_Int32                                 nRequestedLifetime,
    OpcUa_MessageSecurityMode                   messageSecurityMode,
    OpcUa_UInt32                                nNetworkTimeout);

/**
 * @brief Establishes a network connection with the server including the secure conversation but does not create the session.
 *
 * @param pChannel                      [in] The channel to connect.
 * @param sUrl                          [in] The url of the server to connect to.
 * @param pClientCertificate            [in] The certificate of the client.
 * @param pClientPrivateKey             [in] The private key for the certificate.
 * @param pServerCertificate            [in] The certificate of the server.
 * @param pPKIConfig                    [in] The platform dependent pki configuration.
 * @param pRequestedSecurityPolicyUri   [in] The URI of the OPC UA security policy to use for this connection.
 * @param nRequestedLifetime            [in] The Lifetime of the connection.
 * @param messageSecurityMode           [in] The constant for None, Sign or SignAndEncrypt mode.
 * @param nNetworkTimeout               [in] The network timeout. Also used for disconnect.
 * @param pfCallback                    [in] Function to call when connection is established.
 * @param pCallbackData                 [in] Data to pass to pfCallback.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_BeginConnect(
    OpcUa_Channel                               pChannel,
    OpcUa_StringA                               sUrl,
    OpcUa_ByteString*                           pClientCertificate,
    OpcUa_Key*                                  pClientPrivateKey,
    OpcUa_ByteString*                           pServerCertificate,
    OpcUa_Void*                                 pPKIConfig,
    OpcUa_String*                               pRequestedSecurityPolicyUri,
    OpcUa_Int32                                 nRequestedLifetime,
    OpcUa_MessageSecurityMode                   messageSecurityMode,
    OpcUa_UInt32                                nNetworkTimeout,
    OpcUa_Channel_PfnConnectionStateChanged*    pfCallback,
    OpcUa_Void*                                 pCallbackData);

/**
 * @brief Closes the network connection with the server.
 *
 * @param pChannel [in] The session to disconnect.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_Disconnect(OpcUa_Channel pChannel);

/**
 * @brief Closes the network connection with the server asynchronously.
 *
 * @param pChannel      [in] The session to disconnect.
 * @param pfCallback    [in] Function to call when connection is closed.
 * @param pCallbackData [in] Data to pass to pfCallback.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_BeginDisconnect(
    OpcUa_Channel                               pChannel,
    OpcUa_Channel_PfnConnectionStateChanged*    pfCallback,
    OpcUa_Void*                                 pCallbackData);

/**
 * @brief Called by the stack to report an asynchronous request that completed.
 *
 * @param pChannel         [in] The session used to send the request.
 * @param hAsyncState      [in] The async call state object.
 * @param pCallbackData    [in] The callback data specified when the request was sent.
 */
typedef OpcUa_StatusCode (OpcUa_Channel_PfnRequestComplete)(
    OpcUa_Channel         hChannel,
    OpcUa_Void*           pResponse,
    OpcUa_EncodeableType* pResponseType,
    OpcUa_Void*           pCallbackData,
    OpcUa_StatusCode      uStatus);

/**
 * @brief Invokes a service.
 *
 * @param pChannel       [in]  The session to use.
 * @param sName          [in]  The name of the service being invoked.
 * @param pRequest       [in]  The request body.
 * @param pRequestType   [in]  The type of request.
 * @param ppResponse     [out] The response body.
 * @param ppResponseType [out] The type of response.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_InvokeService(
    OpcUa_Channel          hChannel,
    OpcUa_StringA          sName,
    OpcUa_Void*            pRequest,
    OpcUa_EncodeableType*  pRequestType,
    OpcUa_Void**           ppResponse,
    OpcUa_EncodeableType** ppResponseType);

/**
 * @brief Invokes a service asynchronously.
 *
 * @param pChannel       [in]  The session to use.
 * @param sName          [in]  The name of the service being invoked.
 * @param pRequest       [in]  The request body.
 * @param pRequestType   [in]  The type of request.
 * @param pCallback      [in]  The callback to use when the response arrives.
 * @param pCallbackData  [in]  The data to pass to the callback.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_BeginInvokeService(
    OpcUa_Channel                     hChannel,
    OpcUa_StringA                     sName,
    OpcUa_Void*                       pRequest,
    OpcUa_EncodeableType*             pRequestType,
    OpcUa_Channel_PfnRequestComplete* pCallback,
    OpcUa_Void*                       pCallbackData);

#ifdef OPCUA_SUPPORT_PREENCODED_MESSAGES
/**
 * @brief Sends a message to the server.
 *
 * @param hChannel      [in]  The channel to use.
 * @param pRequest      [in]  The request data to send.
 * @param uTimeout      [in]  The callback to use when the response arrives.
 * @param pCallback     [in]  The callback to use when the response arrives.
 * @param pCallbackData [in]  The data to pass to the callback.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_BeginSendEncodedRequest(
    OpcUa_Channel                   hChannel,
    OpcUa_ByteString*               pRequest,
    OpcUa_UInt32                    uTimeout,
    OpcUa_Connection_PfnOnResponse* pCallback,
    OpcUa_Void*                     pCallbackData);
#endif

/**
 * @brief Returns the actually used https-server certificate.
 *
 * @param hChannel                 [in]   The channel to use.
 * @param ppUsedServerCertificate  [out]  The server certificate.
 * @param pValidationResult        [out]  The validation result.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Channel_GetUsedServerCertificate(
    OpcUa_Channel      hChannel,
    OpcUa_ByteString** ppUsedServerCertificate,
    OpcUa_StatusCode*  pValidationResult);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_CLIENTAPI */
#endif /* _OpcUa_Channel_H_ */
