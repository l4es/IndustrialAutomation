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

#ifndef _OpcUa_Socket_Ssl_H_
#define _OpcUa_Socket_Ssl_H_ 1

OPCUA_BEGIN_EXTERN_C

#if OPCUA_P_SOCKETMANAGER_SUPPORT_SSL

/*============================================================================
 * Create a SSL server socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateSslServer(  OpcUa_SocketManager              pSocketManager,
                                                                       OpcUa_StringA                    sAddress,
                                                                       OpcUa_Boolean                    bListenOnAllInterfaces,
                                                                       OpcUa_ByteString*                pServerCertificate,
                                                                       OpcUa_Key*                       pServerPrivateKey,
                                                                       OpcUa_Void*                      pPKIConfig,
                                                                       OpcUa_Socket_EventCallback       pfnSocketCallBack,
                                                                       OpcUa_Socket_CertificateCallback pfnCertificateCallBack,
                                                                       OpcUa_Void*                      pCallbackData,
                                                                       OpcUa_Socket*                    pSocket);

/*============================================================================
 * Create a SSL client socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateSslClient(  OpcUa_SocketManager              pSocketManager,
                                                                       OpcUa_StringA                    sRemoteAddress,
                                                                       OpcUa_UInt16                     uLocalPort,
                                                                       OpcUa_ByteString*                pClientCertificate,
                                                                       OpcUa_Key*                       pClientPrivateKey,
                                                                       OpcUa_Void*                      pPKIConfig,
                                                                       OpcUa_Socket_EventCallback       pfnSocketCallBack,
                                                                       OpcUa_Socket_CertificateCallback pfnCertificateCallBack,
                                                                       OpcUa_Void*                      pCallbackData,
                                                                       OpcUa_Socket*                    pSocket);

#endif /* OPCUA_P_SOCKETMANAGER_SUPPORT_SSL */

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Socket_Ssl_H_ */

