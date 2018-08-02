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

#ifndef _OpcUa_HttpsListener_H_
#define _OpcUa_HttpsListener_H_ 1

#include <opcua_listener.h>
#include <opcua_securelistener.h>

OPCUA_BEGIN_EXTERN_C

/**
 * @brief Function, that needs to be implemented to receive notifications about secure channel events.
 *
 * @param uSecureChannelId      [in] The id assigned to the secure channel.
 * @param eEvent                [in] What type of event on the secure channel occurred.
 * @param uStatus               [in] The result of the operation.
 * @param pbsClientCertificate  [in] The certificate of the client.
 * @param sSecurityPolicy       [in] The security policy in case of open or renew.
 * @param eMessageSecurityMode  [in] What type of event on the secure channel occurred.
 * @param uRequestedLifetime    [in] The requested securechannel lifetime.
 * @param pCallbackData         [in] Data pointer received at creation.
 */
typedef OpcUa_StatusCode (OpcUa_HttpsListener_PfnSecureChannelCallback)(
    OpcUa_UInt32                                        uSecureChannelId,
    OpcUa_SecureListener_SecureChannelEvent             eSecureChannelEvent,
    OpcUa_StatusCode                                    uStatus,
    OpcUa_ByteString*                                   pbsClientCertificate,
    OpcUa_String*                                       sSecurityPolicy,
    OpcUa_UInt16                                        uMessageSecurityModes,
    OpcUa_Void*                                         pCallbackData);


/**
  @brief Creates a new http listener object.

  @param a_ppListener [out] The new listener.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsListener_Create(   OpcUa_ByteString*                               a_pServerCertificate,
                                                            OpcUa_Key*                                      a_pServerPrivateKey,
                                                            OpcUa_Void*                                     a_pPKIConfig,
                                                            OpcUa_HttpsListener_PfnSecureChannelCallback*   a_pfSecureChannelCallback,
                                                            OpcUa_Void*                                     a_pSecureChannelCallbackData,
                                                            OpcUa_Listener**                                a_pListener);

/**
  @brief Sends an immediate http response.

  @param a_pListener        [in]  The listener.
  @param a_hConnection      [in]  The connection to write to.
  @param a_nStatusCode      [in]  The status code of the response.
  @param a_sReasonPhrase    [in]  The textual description of the status code.
  @param a_sResponseHeaders [in]  The response headers.
  @param a_pResponseData    [in]  The response data to be sent.
  @param a_uResponseLength  [in]  The length of the response data.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsListener_SendImmediateResponse(
    OpcUa_Listener*     a_pListener,
    OpcUa_Handle        a_hConnection,
    OpcUa_StatusCode    a_uStatusCode,
    OpcUa_StringA       a_sReasonPhrase,
    OpcUa_StringA       a_sResponseHeaders,
    OpcUa_Byte*         a_pResponseData,
    OpcUa_UInt32        a_uResponseLength);

OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsListener_GetSecurityPolicyConfiguration(
    OpcUa_Listener*                                   a_pListener,
    OpcUa_OutputStream*                               a_pOstrm,
    OpcUa_SecureListener_SecurityPolicyConfiguration* a_pSecurityPolicyConfiguration);

OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsListener_GetPeerInfo(
    OpcUa_Listener*                 a_pListener,
    OpcUa_OutputStream*             a_pOstrm,
    OpcUa_String*                   a_sPeerInfo);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_HttpListener_H_ */
