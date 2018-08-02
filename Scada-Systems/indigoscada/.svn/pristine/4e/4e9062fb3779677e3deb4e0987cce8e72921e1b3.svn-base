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

#ifndef _OpcUa_SecureListener_H_
#define _OpcUa_SecureListener_H_ 1

#ifdef OPCUA_HAVE_SERVERAPI

#include <opcua_listener.h>
#include <opcua_encoder.h>
#include <opcua_decoder.h>
#include <opcua_securechannel_types.h>

OPCUA_BEGIN_EXTERN_C

/**
 * @brief Types of events that can occur on secure channels.
*/
typedef enum eOpcUa_SecureListener_SecureChannelEvent
{
    eOpcUa_SecureListener_SecureChannelOpen,
    eOpcUa_SecureListener_SecureChannelClose,
    eOpcUa_SecureListener_SecureChannelRenew,
    eOpcUa_SecureListener_SecureChannelUnkown
} OpcUa_SecureListener_SecureChannelEvent;


/**
 * @brief Associates a supported security policy with message security modes.
 */
#ifndef OPCUA_ENDPOINT_SECURITYPOLICYCONFIGURATION_DEFINED
struct _OpcUa_Endpoint_SecurityPolicyConfiguration
{
    /** @brief The URI of a supported security policy. */
    OpcUa_String        sSecurityPolicy;
    /** @brief The message security modes allowed for the security policy. (bitmask) */
    OpcUa_UInt16        uMessageSecurityModes;
    /** @brief The client certificate, if provided. */
    OpcUa_ByteString*   pbsClientCertificate;
};
#define OPCUA_ENDPOINT_SECURITYPOLICYCONFIGURATION_DEFINED
#endif

typedef struct _OpcUa_Endpoint_SecurityPolicyConfiguration OpcUa_SecureListener_SecurityPolicyConfiguration;

/**
 * @brief Function, that needs to be implemented to receive notifications about secure channel events.
 *
 * @param uSecureChannelId      [in] The id assigned to the secure channel.
 * @param eEvent                [in] What type of event on the secure channel occurred.
 * @param uStatus               [in] The result of the operation.
 * @param pbsClientCertificate  [in] The certificate of the client.
 * @param sSecurityPolicy       [in] The security policy in case of open or renew.
 * @param eMessageSecurityMode  [in] What type of event on the secure channel occurred.
 * @param pCallbackData         [in] Data pointer received at creation.
 */
typedef OpcUa_StatusCode (OpcUa_SecureListener_PfnSecureChannelCallback)(
    OpcUa_UInt32                                        uSecureChannelId,
    OpcUa_SecureListener_SecureChannelEvent             eSecureChannelEvent,
    OpcUa_StatusCode                                    uStatus,
    OpcUa_ByteString*                                   pbsClientCertificate,
    OpcUa_String*                                       sSecurityPolicy,
    OpcUa_UInt16                                        uMessageSecurityModes,
    OpcUa_Void*                                         pCallbackData);

/**
  @brief Creates a new secure listener object.

  @param pInnerListener       [in]  The inner the listener is attached to.
  @param ppSecureListener     [out] The new listener.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureListener_Create(
    OpcUa_Listener*                                     pInnerListener,
    OpcUa_Decoder*                                      pDecoder,
    OpcUa_Encoder*                                      pEncoder,
    OpcUa_StringTable*                                  pNamespaceUris,
    OpcUa_EncodeableTypeTable*                          pKnownTypes,
    OpcUa_ByteString*                                   pServerCertificate,
    OpcUa_Key*                                          pServerPrivateKey,
    OpcUa_Void*                                         pPKIConfig,
    OpcUa_UInt32                                        nNoSecurityPolicies,
    OpcUa_SecureListener_SecurityPolicyConfiguration*   pSecurityPolicyConfigurations,
    OpcUa_SecureListener_PfnSecureChannelCallback*      pfSecureChannelCallback,
    OpcUa_Void*                                         pSecureChannelCallbackData,
    OpcUa_Listener**                                    ppSecureListener);


OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureListener_GetChannelId(
    OpcUa_Listener*                                     pListener,
    OpcUa_InputStream*                                  pIstrm,
    OpcUa_UInt32*                                       puChannelId);


OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureListener_GetSecureChannelSecurityPolicyConfiguration(
    OpcUa_Listener*                                     pListener,
    OpcUa_UInt32                                        uChannelId,
    OpcUa_SecureListener_SecurityPolicyConfiguration*   pSecurityPolicyConfiguration);

OpcUa_StatusCode OpcUa_SecureListener_GetPeerInfo(
    OpcUa_Listener*                                     a_pListener,
    OpcUa_UInt32                                        a_uChannelId,
    OpcUa_String*                                       a_pPeerInfo);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_SERVERAPI */

#endif /* _OpcUa_SecureListener_H_ */
