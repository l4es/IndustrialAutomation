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

#ifndef _OpcUa_Utilities_H_
#define _OpcUa_Utilities_H_ 1

#include <opcua_platformdefs.h>

OPCUA_BEGIN_EXTERN_C

enum _OpcUa_ProtocolType
{
    OpcUa_ProtocolType_Invalid,
    OpcUa_ProtocolType_Http,
    OpcUa_ProtocolType_Tcp
};
typedef enum _OpcUa_ProtocolType OpcUa_ProtocolType;


/**
 * @brief Sorts an array.
 *
 * @param pElements     [in] The array of elements to sort.
 * @param nElementCount [in] The number of elements in the array.
 * @param nElementSize  [in] The size a single element in the array.
 * @param pfnCompare    [in] The function used to compare elements.
 * @param pContext      [in] A context that is passed to the compare function.
 */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_QSort(   OpcUa_Void*       pElements,
                                OpcUa_UInt32      nElementCount,
                                OpcUa_UInt32      nElementSize,
                                OpcUa_PfnCompare* pfnCompare,
                                OpcUa_Void*       pContext);

/**
 * @brief Searches a sorted array.
 *
 * @param pKey          [in] The element to find.
 * @param pElements     [in] The array of elements to sort.
 * @param nElementCount [in] The number of elements in the array.
 * @param nElementSize  [in] The size a single element in the array.
 * @param pfnCompare    [in] The function used to compare elements.
 * @param pContext      [in] A context that is passed to the compare function.
 */
OPCUA_EXPORT
OpcUa_Void* OpcUa_BSearch(  OpcUa_Void*       pKey,
                            OpcUa_Void*       pElements,
                            OpcUa_UInt32      nElementCount,
                            OpcUa_UInt32      nElementSize,
                            OpcUa_PfnCompare* pfnCompare,
                            OpcUa_Void*       pContext);

/**
 * @brief Returns the CRT errno constant.
 */
OPCUA_EXPORT
OpcUa_UInt32 OpcUa_GetLastError();

/**
 * @brief Returns the number of milliseconds since the system or process was started.
 */
OPCUA_EXPORT
OpcUa_UInt32 OpcUa_GetTickCount();

/**
 * @brief Convert string to integer.
 */
#define OpcUa_CharAToInt(xChar) OpcUa_ProxyStub_g_PlatformLayerCalltable->CharToInt(xChar)

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Utilities_H_ */
