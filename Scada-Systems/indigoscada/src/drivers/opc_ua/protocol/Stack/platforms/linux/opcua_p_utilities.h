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

#ifndef _OpcUa_P_Utilities_H_
#define _OpcUa_P_Utilities_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
 * @see OpcUa_P_QSort
 */
OpcUa_Void OPCUA_DLLCALL OpcUa_P_QSort( OpcUa_Void*       pElements,
                                        OpcUa_UInt32      nElementCount,
                                        OpcUa_UInt32      nElementSize,
                                        OpcUa_PfnCompare* pfnCompare,
                                        OpcUa_Void*       pContext);

/**
 * @see OpcUa_P_BSearch
 */
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_BSearch(  OpcUa_Void*       pKey,
                                            OpcUa_Void*       pElements,
                                            OpcUa_UInt32      nElementCount,
                                            OpcUa_UInt32      nElementSize,
                                            OpcUa_PfnCompare* pfnCompare,
                                            OpcUa_Void*       pContext);

/**
 * @see OpcUa_P_GetLastError
 */
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_GetLastError(void);

/**
 * @see OpcUa_P_GetTickCount
 */
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_GetTickCount(void);

/**
 * @see OpcUa_P_CharAToInt
 */
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_CharAToInt(   OpcUa_StringA sValue);

/**
 * @see OpcUa_P_ParseUrl
 */
OpcUa_StatusCode OpcUa_P_ParseUrl(  OpcUa_StringA   psUrl,
                                    OpcUa_StringA*  psIpAddress,
                                    OpcUa_UInt16*   puPort);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_P_Utilities_H_ */

