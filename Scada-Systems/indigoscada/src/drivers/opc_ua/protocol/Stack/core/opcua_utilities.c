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

#include <opcua.h>
#include <opcua_utilities.h>

#define OPCUA_P_QSORT           OpcUa_ProxyStub_g_PlatformLayerCalltable->qSort
#define OPCUA_P_BSEARCH         OpcUa_ProxyStub_g_PlatformLayerCalltable->bSearch
#define OPCUA_P_GETTICKCOUNT    OpcUa_ProxyStub_g_PlatformLayerCalltable->UtilGetTickCount
#define OPCUA_P_GETLASTERROR    OpcUa_ProxyStub_g_PlatformLayerCalltable->UtilGetLastError


/*============================================================================
 * Quick Sort
 *===========================================================================*/
OpcUa_StatusCode OpcUa_QSort(   OpcUa_Void*       a_pElements,
                                OpcUa_UInt32      a_nElementCount,
                                OpcUa_UInt32      a_nElementSize,
                                OpcUa_PfnCompare* a_pfnCompare,
                                OpcUa_Void*       a_pContext)
{
    if(     a_pElements     == OpcUa_Null
        ||  a_pfnCompare    == OpcUa_Null
        ||  a_nElementCount == 0
        ||  a_nElementSize  == 0)
    {
        return OpcUa_BadInvalidArgument;
    }

    OPCUA_P_QSORT(    a_pElements,
                      a_nElementCount,
                      a_nElementSize,
                      a_pfnCompare,
                      a_pContext);

    return OpcUa_Good;
}

/*============================================================================
 * Binary Search on sorted array
 *===========================================================================*/
OpcUa_Void* OpcUa_BSearch(  OpcUa_Void*       a_pKey,
                            OpcUa_Void*       a_pElements,
                            OpcUa_UInt32      a_nElementCount,
                            OpcUa_UInt32      a_nElementSize,
                            OpcUa_PfnCompare* a_pfnCompare,
                            OpcUa_Void*       a_pContext)
{
    if(     a_pElements     == OpcUa_Null
        ||  a_pKey          == OpcUa_Null
        ||  a_pfnCompare    == OpcUa_Null
        ||  a_nElementCount == 0
        ||  a_nElementSize  == 0)
    {
        return OpcUa_Null;
    }

    return OPCUA_P_BSEARCH( a_pKey,
                            a_pElements,
                            a_nElementCount,
                            a_nElementSize,
                            a_pfnCompare,
                            a_pContext);
}

/*============================================================================
 * Access to errno
 *===========================================================================*/
OpcUa_UInt32 OpcUa_GetLastError()
{
    return OPCUA_P_GETLASTERROR();
}

/*============================================================================
 * OpcUa_GetTickCount
 *===========================================================================*/
OpcUa_UInt32 OpcUa_GetTickCount()
{
    return OPCUA_P_GETTICKCOUNT();
}
