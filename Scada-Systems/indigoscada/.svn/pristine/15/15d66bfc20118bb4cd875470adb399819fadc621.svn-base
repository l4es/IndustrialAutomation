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

#ifndef _OpcUa_TcpSecureChannel_H_
#define _OpcUa_TcpSecureChannel_H_ 1

OPCUA_BEGIN_EXTERN_C
/**
  @brief The Tcp SecureChannel.
  */
struct _OpcUa_TcpSecureChannel
{
    /** @brief Just a dummy to keep. */
    OpcUa_Void*                  pvDummy;
};

typedef struct _OpcUa_TcpSecureChannel OpcUa_TcpSecureChannel;

/**
  @brief Creates a new securechannel object.

  A secure connection is always layed on top of a regular connection.

  @param ppSecureChannel           [out] The new securechannel.
*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Create(OpcUa_SecureChannel**   ppSecureChannel);

/**
  @brief Deletes a securechannel object.

  A secure connection is always layed on top of a regular connection.

  @param ppSecureChannel [in]  The securechannel to delete.
*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Delete(OpcUa_SecureChannel**   ppSecureChannel);

OPCUA_END_EXTERN_C

#endif
