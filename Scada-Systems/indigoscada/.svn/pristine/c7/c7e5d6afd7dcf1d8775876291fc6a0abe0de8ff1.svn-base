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

#include <opcua_mutex.h>
#include <opcua_trace.h>

#include <opcua_memory.h>

#define OPCUA_P_MEMORY_ALLOC    OpcUa_ProxyStub_g_PlatformLayerCalltable->MemAlloc
#define OPCUA_P_MEMORY_REALLOC  OpcUa_ProxyStub_g_PlatformLayerCalltable->MemReAlloc
#define OPCUA_P_MEMORY_FREE     OpcUa_ProxyStub_g_PlatformLayerCalltable->MemFree
#define OPCUA_P_MEMORY_MEMCPY   OpcUa_ProxyStub_g_PlatformLayerCalltable->MemCpy

/*============================================================================
 * OpcUa_Memory_Alloc
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_Memory_Alloc(OpcUa_UInt32 nSize)
{
    return OPCUA_P_MEMORY_ALLOC(nSize);
}

/*============================================================================
 * OpcUa_Memory_ReAlloc
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_Memory_ReAlloc(   OpcUa_Void*     a_pBuffer,
                                                  OpcUa_UInt32    a_nSize)
{
    return OPCUA_P_MEMORY_REALLOC(  a_pBuffer,
                                    a_nSize);
}

/*============================================================================
 * OpcUa_Memory_Free
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_Memory_Free(OpcUa_Void* a_pBuffer)
{
    if(a_pBuffer != OpcUa_Null)
    {
        OPCUA_P_MEMORY_FREE(a_pBuffer);
    }
}

/*============================================================================
 * OpcUa_Memory_MemCpy
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Memory_MemCpy(   OpcUa_Void*     a_pBuffer,
                                        OpcUa_UInt32    a_nSizeInBytes,
                                        OpcUa_Void*     a_pSource,
                                        OpcUa_UInt32    a_nCount)
{

    return OPCUA_P_MEMORY_MEMCPY(   a_pBuffer,
                                    a_nSizeInBytes,
                                    a_pSource,
                                    a_nCount);
}

/*============================================================================
 * OpcUa_Memory_DestroySecretData
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_Memory_DestroySecretData(OpcUa_Void*  a_pData,
                                                        OpcUa_UInt32 a_nBytes)
{
    if(OpcUa_ProxyStub_g_PlatformLayerCalltable->DestroySecretData != OpcUa_Null)
    {
        OpcUa_ProxyStub_g_PlatformLayerCalltable->DestroySecretData(a_pData,
                                                                    a_nBytes);
    }
}

