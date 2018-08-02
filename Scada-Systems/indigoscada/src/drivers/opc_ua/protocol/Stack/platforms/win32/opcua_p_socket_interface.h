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

#ifndef _OpcUa_Socket_Interface_H_
#define _OpcUa_Socket_Interface_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * Create a new socket manager or initialize the global one (OpcUa_Null first).
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_Create(        OpcUa_SocketManager*    pSocketManager,
                                                                    OpcUa_UInt32            nSockets,
                                                                    OpcUa_UInt32            nFlags);

/*============================================================================
 *
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_SocketManager_Delete(              OpcUa_SocketManager*    pSocketManager);

/*============================================================================
 * Create a server socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateServer(  OpcUa_SocketManager         pSocketManager,
                                                                    OpcUa_StringA               sAddress,
                                                                    OpcUa_Boolean               bListenOnAllInterfaces,
                                                                    OpcUa_Socket_EventCallback  pfnSocketCallBack,
                                                                    OpcUa_Void*                 pCallbackData,
                                                                    OpcUa_Socket*               pSocket);

/*============================================================================
 * Create a client socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateClient(  OpcUa_SocketManager         pSocketManager,
                                                                    OpcUa_StringA               sRemoteAddress,
                                                                    OpcUa_UInt16                uLocalPort,
                                                                    OpcUa_Socket_EventCallback  pfnSocketCallBack,
                                                                    OpcUa_Void*                 pCallbackData,
                                                                    OpcUa_Socket*               pSocket);

/*============================================================================
 * Signal a certain event on a socket.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_SignalEvent(   OpcUa_SocketManager pSocketManager,
                                                                    OpcUa_UInt32        uEvent,
                                                                    OpcUa_Boolean       bAllManagers);

/*============================================================================
 *
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_ServeLoop(     OpcUa_SocketManager     pSocketManager,
                                                                    OpcUa_UInt32            msecTimeout,
                                                                    OpcUa_Boolean           bRunOnce);

/*============================================================================
 * Break server loop(s) and issue event(s).
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_InterruptLoop( OpcUa_SocketManager pSocketManager,
                                                                    OpcUa_UInt32        uEvent,
                                                                    OpcUa_Boolean       bAllManagers);

/*============================================================================
 * Get last socket error
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetLastError(         OpcUa_Socket    pSocket);

/*============================================================================
 * Read Socket.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_Read(                 OpcUa_Socket    pSocket,
                                                                    OpcUa_Byte*     pBuffer,
                                                                    OpcUa_UInt32    nBufferSize,
                                                                    OpcUa_UInt32*   pBytesRead);

/*============================================================================
 * Write Socket.
 *===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_Socket_Write(                     OpcUa_Socket    pSocket,
                                                                    OpcUa_Byte*     pBuffer,
                                                                    OpcUa_UInt32    uBufferSize,
                                                                    OpcUa_Boolean   bBlock);

/*============================================================================
 * Close Socket.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_Close(                OpcUa_Socket pSocket);


#if OPCUA_P_SOCKETGETPEERINFO_V2
/*============================================================================
 * Get IP Address and Port Number of the Peer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetPeerInfo(          OpcUa_Socket RawSocket,
                                                                    OpcUa_CharA* achPeerInfoBuffer,
                                                                    OpcUa_UInt32 uiPeerInfoBufferSize);

#else /* OPCUA_P_SOCKETGETPEERINFO_V2 */
/*============================================================================
 * Get IP Address and Port Number of the Peer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetPeerInfo(          OpcUa_Socket  pSocket,
                                                                    OpcUa_UInt32* pIP,
                                                                    OpcUa_UInt16* pPort);
#endif /* OPCUA_P_SOCKETGETPEERINFO_V2 */

/*============================================================================
 * Initialize the platform network interface
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_InitializeNetwork(    OpcUa_Void);

/*============================================================================
 * Clean the platform network interface up.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_CleanupNetwork(       OpcUa_Void);

/*============================================================================
 * Convert a text encoded internet address to its binary representation.
 *===========================================================================*/
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_Socket_InetAddr(                 OpcUa_StringA sRemoteAddress);

/*============================================================================
 * Get the name of the local host.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_GetHostName(          OpcUa_CharA* pBuffer,
                                                                    OpcUa_UInt32 uiBufferLength);

/*============================================================================
 * Set socket user data
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Socket_SetUserData(          OpcUa_Socket pSocket,
                                                                    OpcUa_Void*  pvUserData);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Socket_Interface_H_ */
