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

/* HINT: This implementation will become a opaque handle! */

#ifndef _OpcUa_TcpConnection_H_
#define _OpcUa_TcpConnection_H_ 1

#include <opcua_connection.h>

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * OpcUa_TcpConnection_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpConnection_Create(OpcUa_Connection** connection);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_TcpConnection_H_ */
