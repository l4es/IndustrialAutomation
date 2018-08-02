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


#ifndef _OpcUa_Memory_H_
#define _OpcUa_Memory_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
 * @brief Allocates a new block of memory.
 *
 * @param nSize [in] The size of the block to allocate.
*/
OPCUA_EXPORT OpcUa_Void* OPCUA_DLLCALL OpcUa_Memory_Alloc(OpcUa_UInt32 nSize);

/**
 * @brief Reallocates a block of memory
 *
 * @param pBuffer [in] The existing memory block.
 * @param nSize   [in] The size of the block to allocate.
*/
OPCUA_EXPORT OpcUa_Void* OPCUA_DLLCALL OpcUa_Memory_ReAlloc(OpcUa_Void*  pBuffer,
                                                            OpcUa_UInt32 nSize);

/**
 * @brief Frees a block of memory.
 *
 * @param pvBuffer [in] The existing memory block.
*/
OPCUA_EXPORT OpcUa_Void OPCUA_DLLCALL OpcUa_Memory_Free(OpcUa_Void* pvBuffer);

/**
 * @brief Copies a block of memory.
 *
 * @param pBuffer      [in] The destination memory block.
 * @param nSizeInBytes [in] The size of the destination memory block.
 * @param pSource      [in] The memory block being copied.
 * @param nCount       [in] The number of bytes to copy.
 *
 * @return StatusCode:
 *   OpcUa_BadInvalidArgument if Buffer or Source equals OpcUa_Null;
 *   OpcUa_BadOutOfRange      if number of bytes to copy greater nSizeInBytes
*/
OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Memory_MemCpy(   OpcUa_Void*  pBuffer,
    OpcUa_UInt32 nSizeInBytes,
    OpcUa_Void*  pSource,
    OpcUa_UInt32 nCount);

/**
 * @brief destroys secret data values in a cyptographically safe way.
 * Does not necessarily set the memory to zero.
 * Does nothing unless security is enabled.
 *
 * @param pData        [in] The memory block to be erased.
 * @param nBytes       [in] The number of bytes to erase.
 */
OPCUA_EXPORT OpcUa_Void OPCUA_DLLCALL OpcUa_Memory_DestroySecretData(OpcUa_Void*  pData,
                                                                     OpcUa_UInt32 nBytes);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Memory_H_ */
