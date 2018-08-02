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

#ifndef _OpcUa_SecureConnection_H_
#define _OpcUa_SecureConnection_H_ 1

#ifdef OPCUA_HAVE_CLIENTAPI

#include <opcua_connection.h>
#include <opcua_encoder.h>
#include <opcua_decoder.h>

OPCUA_BEGIN_EXTERN_C

/**
  @brief Creates a new secure connection object.

  A secure connection is always layed on top of a regular connection.

  @param pInnerConnection [in]  The connection to layer on top of.
  @param pEncoder         [in]  The encoder used by the channel.
  @param pDecoder         [in]  The decoder used by the channel.
  @param ppConnection     [out] The new connection.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_Create(
    OpcUa_Connection*               pInnerConnection,
    OpcUa_Encoder*                  pEncoder,
    OpcUa_Decoder*                  pDecoder,
    OpcUa_StringTable*              pNamespaceUris,
    OpcUa_EncodeableTypeTable*      pKnownTypes,
    OpcUa_Connection**              ppConnection);

/**
  @brief returns the securechannelid of an existing connection.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_GetChannelId(
    OpcUa_Connection*       pConnection,
    OpcUa_UInt32*           pSecureChannelId);

/**
  @brief returns the current tokenId of an existing connection.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_GetCurrentTokenId(
    OpcUa_Connection*       pConnection,
    OpcUa_UInt32*           pCurrentTokenId);


/**
  @brief returns the securechannelid of an existing connection.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_SetClientPrivateKey(
    OpcUa_Connection*       pConnection,
    OpcUa_ByteString*       pClientPrivateKey);

/**
  @brief sets the client certificate for an existing connection.
*/
OpcUa_StatusCode OpcUa_SecureConnection_SetClientCertificate(
    OpcUa_Connection*       pConnection,
    OpcUa_ByteString*       pClientCertificate);

/**
  @brief sets the client certificate for an existing connection.

*/
OpcUa_StatusCode OpcUa_SecureConnection_SetServerCertificate(
    OpcUa_Connection*       pConnection,
    OpcUa_ByteString*       pServerCertificate);

/**
  @brief returns the client certificate for an existing connection.
*/
OpcUa_StatusCode OpcUa_SecureConnection_GetClientCertificate(
    OpcUa_Connection*       pConnection,
    OpcUa_ByteString*       pClientCertificate);

/**
  @brief sets the pki provider of a connection.
*/
OpcUa_StatusCode OpcUa_SecureConnection_SetClientPKIProvider(
    OpcUa_Connection*       pConnection,
    OpcUa_PKIProvider*      pClientClientPKIProvider);

/**
  @brief
  @param pConnection [in]
  @param ppOstrm     [out]
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_EndSendOpenSecureChannelRequest(
    OpcUa_Connection*               a_pConnection,
    OpcUa_OutputStream**            a_ppOstrm,
    OpcUa_Void*                     a_pRequest,
    OpcUa_UInt32                    a_uTimeout,
    OpcUa_Connection_PfnOnResponse* a_pCallback,
    OpcUa_Void*                     a_pCallbackData);

/**
  @brief
  @param pConnection [in]
  @param ppOstrm     [out]
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_EndSendCloseSecureChannelRequest(
    OpcUa_Connection*               pConnection,
    OpcUa_OutputStream**            ppOstrm,
    OpcUa_UInt32                    uSecureChannelId,
    OpcUa_UInt32                    timeout,
    OpcUa_Connection_PfnOnResponse* pCallback,
    OpcUa_Void*                     pCallbackData);

/**
  @brief
  @param pConnection [in]
  @param ppOstrm     [out]
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureConnection_GetSecurityToken(
    OpcUa_Connection*               a_pConnection,
    OpcUa_ChannelSecurityToken**    a_pChannelSecurityToken);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_CLIENTAPI */

#endif /* _OpcUa_SecureConnection_H_ */
