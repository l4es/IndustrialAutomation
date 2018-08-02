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

#include <stdlib.h>
#include <memory.h>
#include <errno.h>      /* for errornumbers when using save functions */

#include <opcua_p_internal.h>


/*============================================================================
 * OpcUa_Memory_Alloc
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_Memory_Alloc(OpcUa_UInt32 nSize)
{
    return malloc(nSize);
}

/*============================================================================
 * OpcUa_Memory_ReAlloc
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_Memory_ReAlloc(OpcUa_Void* pBuffer, OpcUa_UInt32 nSize)
{
    return realloc(pBuffer, nSize);
}

/*============================================================================
 * OpcUa_Memory_Free
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Memory_Free(OpcUa_Void* pBuffer)
{
    free(pBuffer);
}

/*============================================================================
 * OpcUa_Memory_MemCpy
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Memory_MemCpy(
    OpcUa_Void*  pBuffer,
    OpcUa_UInt32 nSizeInBytes,
    OpcUa_Void*  pSource,
    OpcUa_UInt32 nCount)
{
    if(     pBuffer == OpcUa_Null
        ||  pSource == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    if(nSizeInBytes < nCount)
    {
        return OpcUa_BadOutOfRange;
    }

    if(memcpy(pBuffer, pSource, nCount) != pBuffer)
    {
        return OpcUa_BadInvalidArgument;
    }

    return OpcUa_Good;
}

