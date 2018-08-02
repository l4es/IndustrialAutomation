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

#ifndef _OpcUa_P_Memory_H_
#define _OpcUa_P_Memory_H_ 1

OPCUA_BEGIN_EXTERN_C

typedef OpcUa_Void* (OPCUA_DLLCALL *OpcUa_P_Memory_Alloc_Func)(OpcUa_UInt32);
typedef OpcUa_Void* (OPCUA_DLLCALL *OpcUa_P_Memory_ReAlloc_Func)(OpcUa_Void*, OpcUa_UInt32);
typedef OpcUa_Void (OPCUA_DLLCALL *OpcUa_P_Memory_Free_Func)(OpcUa_Void*);

OPCUA_EXPORT
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Set_Memory_Functions(  OpcUa_P_Memory_Alloc_Func pMalloc,
                                                        OpcUa_P_Memory_ReAlloc_Func pReAlloc,
                                                        OpcUa_P_Memory_Free_Func pFree);

/**
 * @see OpcUa_Memory_Alloc
 */
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_Memory_Alloc(         OpcUa_UInt32 nSize);

/**
 * @brief Reallocates a new block of memory
 *
 * @param pBuffer [in] The existing memory block.
 * @param nSize   [in] The size of the block to allocate.
 */
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_Memory_ReAlloc(       OpcUa_Void*  pBuffer,
                                                        OpcUa_UInt32 nSize);

/**
 * @see OpcUa_Memory_Free
 */
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Memory_Free(           OpcUa_Void* pvBuffer);

/**
 * @see OpcUa_Memory_MemCpy
 */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Memory_MemCpy(   OpcUa_Void*  pBuffer,
                                                        OpcUa_UInt32 nSizeInBytes,
                                                        OpcUa_Void*  pSource,
                                                        OpcUa_UInt32 nCount);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_P_Memory_H_ */

