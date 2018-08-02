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

#ifndef _OpcUa_StackStatusCodes_H_
#define _OpcUa_StackStatusCodes_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * Begin of status codes internal to the stack.
 *===========================================================================*/
#define OpcUa_StartOfStackStatusCodes 0x81000000

/*============================================================================
 * The message signature is invalid.
 *===========================================================================*/
#define OpcUa_BadSignatureInvalid 0x81010000

/*============================================================================
 * The extensible parameter provided is not a valid for the service.
 *===========================================================================*/
#define OpcUa_BadExtensibleParameterInvalid 0x81040000

/*============================================================================
 * The extensible parameter provided is valid but the server does not support it.
 *===========================================================================*/
#define OpcUa_BadExtensibleParameterUnsupported 0x81050000

/*============================================================================
 * The hostname could not be resolved.
 *===========================================================================*/
#define OpcUa_BadHostUnknown 0x81060000

/*============================================================================
 * Too many posts were made to a semaphore.
 *===========================================================================*/
#define OpcUa_BadTooManyPosts 0x81070000

/*============================================================================
 * The security configuration is not valid.
 *===========================================================================*/
#define OpcUa_BadSecurityConfig 0x81080000

/*============================================================================
 * Invalid file name specified.
 *===========================================================================*/
#define OpcUa_BadFileNotFound 0x81090000

/*============================================================================
 * Accept bad result and continue anyway.
 *===========================================================================*/
#define OpcUa_BadContinue 0x810A0000

/*============================================================================
 * Invalid http method.
 *===========================================================================*/
#define OpcUa_BadHttpMethodNotAllowed 0x810B0000

/*============================================================================
 * File exists.
 *===========================================================================*/
#define OpcUa_BadFileExists 0x810C0000

OPCUA_END_EXTERN_C

#endif /* _OpcUa_StackStatusCodes_H_ */

