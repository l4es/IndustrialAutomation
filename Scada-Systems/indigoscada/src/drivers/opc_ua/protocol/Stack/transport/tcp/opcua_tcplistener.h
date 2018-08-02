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

#ifndef _OpcUa_TcpListener_H_
#define _OpcUa_TcpListener_H_ 1

#include <opcua_listener.h>

OPCUA_BEGIN_EXTERN_C

/**
  @brief Creates a new tcp listener object.

  @param listener [out] The new listener.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_TcpListener_Create(OpcUa_Listener** listener);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_TcpListener_H_ */
