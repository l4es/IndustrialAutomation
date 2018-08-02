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

/******************************************************************************************************/
/* Platform Portability Layer                                                                         */
/******************************************************************************************************/

#include <windows.h>
#include <opcua.h>
#include <memory.h>     /*  */
#ifndef _WIN32_WCE
#include <errno.h>      /* for errornumbers when using save functions */
#endif /* _WIN32_WCE */

#include <opcua_p_memory.h>


/*============================================================================
 * static variables
 *===========================================================================*/
static OpcUa_P_Memory_Alloc_Func    OpcUa_pMalloc  = NULL;
static OpcUa_P_Memory_ReAlloc_Func  OpcUa_pReAlloc = NULL;
static OpcUa_P_Memory_Free_Func     OpcUa_pFree    = NULL;

/*============================================================================
 * OpcUa_P_Set_Memory_Functions
 *===========================================================================*/
OPCUA_EXPORT
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Set_Memory_Functions(  OpcUa_P_Memory_Alloc_Func pMalloc,
                                                        OpcUa_P_Memory_ReAlloc_Func pReAlloc,
                                                        OpcUa_P_Memory_Free_Func pFree)
{
    OpcUa_pMalloc  = pMalloc;
    OpcUa_pReAlloc = pReAlloc;
    OpcUa_pFree    = pFree;
}

/*============================================================================
 * OpcUa_Memory_Alloc
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_Memory_Alloc(OpcUa_UInt32 nSize)
{
    if (OpcUa_pMalloc != NULL)
        return OpcUa_pMalloc(nSize);
    else
        return malloc(nSize);
}

/*============================================================================
 * OpcUa_Memory_ReAlloc
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_Memory_ReAlloc(OpcUa_Void* pBuffer, OpcUa_UInt32 nSize)
{
    if (OpcUa_pReAlloc != NULL)
        return OpcUa_pReAlloc(pBuffer, nSize);
    else
        return realloc(pBuffer, nSize);
}

/*============================================================================
 * OpcUa_Memory_Free
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Memory_Free(OpcUa_Void* pBuffer)
{
    if (OpcUa_pFree != NULL)
        OpcUa_pFree(pBuffer);
    else
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
#ifdef _WIN32_WCE
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
#else /* _WIN32_WCE */
#if _MSC_VER >= 1400
    errno_t result = 0;

    result = memcpy_s(pBuffer, nSizeInBytes, pSource, nCount);

    if(result == EINVAL)
    {
        return OpcUa_BadInvalidArgument;
    }

    if(result == ERANGE)
    {
        return OpcUa_BadOutOfRange;
    }

    return OpcUa_Good;
#else
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
#endif
#endif /* _WIN32_WCE */
}
