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

#ifndef OpcUa_SecureListener_ChannelManager_
#define OpcUa_SecureListener_ChannelManager_

#ifdef OPCUA_HAVE_SERVERAPI

OPCUA_BEGIN_EXTERN_C

typedef OpcUa_Void (OPCUA_DLLCALL OpcUa_SecureListener_ChannelManager_ChannelRemovedCallback)(
    OpcUa_SecureChannel* pSecureChannel,
    OpcUa_Void*          pvCallbackData);

typedef struct _OpcUa_SecureListener_ChannelManager OpcUa_SecureListener_ChannelManager;

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_Create(
    OpcUa_SecureListener_ChannelManager_ChannelRemovedCallback* pChannelTimeoutCallback,
    OpcUa_Void*                                                 pvChannelTimeoutCallbackData,
    OpcUa_SecureListener_ChannelManager**                       ppChannelManager);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_Initialize(
    OpcUa_SecureListener_ChannelManager_ChannelRemovedCallback* pChannelTimeoutCallback,
    OpcUa_Void*                                                 pvChannelTimeoutCallbackData,
    OpcUa_SecureListener_ChannelManager*                        pChannelManager);

/* @brief */
OpcUa_Void OpcUa_SecureListener_ChannelManager_Clear(
    OpcUa_SecureListener_ChannelManager* pChannelManager);

/* @brief */
OpcUa_Void OpcUa_SecureListener_ChannelManager_Delete(
    OpcUa_SecureListener_ChannelManager** ppChannelManager);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_IsValidChannelID(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_UInt32                         uSecureChannelID);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_AddChannel(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_SecureChannel*                 pChannel);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_ReleaseChannel(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_SecureChannel**                ppSecureChannel);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_SetSecureChannelID(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_SecureChannel*                 pSecureChannel,
    OpcUa_UInt32                         uSecureChannelID);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_SetTransportConnection(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_SecureChannel*                 pSecureChannel,
    OpcUa_Handle                         hTransportConnection);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_UInt32                         uSecureChannelID,
    OpcUa_SecureChannel**                ppSecureChannel);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(
    OpcUa_SecureListener_ChannelManager* pChannelManager,
    OpcUa_Handle                         hTransportConnection,
    OpcUa_SecureChannel**                ppSecureChannel);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_SERVERAPI */

#endif
