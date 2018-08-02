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
#include <opcua_memory.h>
#include <opcua_buffer.h>



/*============================================================================
 * OpcUa_Buffer_SanityCheck
 *===========================================================================*/
#define OpcUa_Buffer_SanityCheck 0x43824B55

/*============================================================================
 * OpcUa_ValidateBuffer
 *===========================================================================*/
#define OpcUa_ReturnErrorIfInvalidBuffer() \
if (((OpcUa_Buffer*)a_Handle)->SanityCheck != OpcUa_Buffer_SanityCheck) \
{ \
    return OpcUa_BadInvalidArgument; \
}

/*============================================================================
 * OpcUa_Buffer_Initialize
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_Initialize(   OpcUa_Buffer* a_pBuffer,
                                            OpcUa_Byte*   a_pbyData,
                                            OpcUa_UInt32  a_uDataSize,
                                            OpcUa_UInt32  a_uBlockSize,
                                            OpcUa_UInt32  a_uMaxSize,
                                            OpcUa_Boolean a_bFreeBuffer)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    OpcUa_MemSet(a_pBuffer, 0, sizeof(OpcUa_Buffer));

    a_pBuffer->SanityCheck = OpcUa_Buffer_SanityCheck;
    a_pBuffer->Data        = a_pbyData;
    a_pBuffer->Size        = (a_pbyData != OpcUa_Null)?a_uBlockSize:0;
    a_pBuffer->EndOfData   = a_uDataSize;
    a_pBuffer->Position    = 0;
    a_pBuffer->BlockSize   = (a_uBlockSize > 0)? a_uBlockSize: 1;
    a_pBuffer->MaxSize     = a_uMaxSize;
    a_pBuffer->FreeBuffer  = a_bFreeBuffer;

    return OpcUa_Good; /* nothing can go wrong beside invalid arguments */
}


/*============================================================================
 * OpcUa_Buffer_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_Create(   OpcUa_Byte*    a_pbyData,
                                        OpcUa_UInt32   a_uDataSize,
                                        OpcUa_UInt32   a_uBlockSize,
                                        OpcUa_UInt32   a_uMaxSize,
                                        OpcUa_Boolean  a_bFreeBuffer,
                                        OpcUa_Buffer** a_ppBuffer)
{
    OpcUa_StatusCode    uStatus = OpcUa_Good;
    OpcUa_Buffer*       pBuffer = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_ppBuffer);

    *a_ppBuffer = OpcUa_Null;

    pBuffer = (OpcUa_Buffer*)OpcUa_Alloc(sizeof(OpcUa_Buffer));
    OpcUa_ReturnErrorIfAllocFailed(pBuffer);

    uStatus = OpcUa_Buffer_Initialize(  pBuffer,
                                        a_pbyData,
                                        a_uDataSize,
                                        a_uBlockSize,
                                        a_uMaxSize,
                                        a_bFreeBuffer);

    *a_ppBuffer = pBuffer;

    return uStatus;
}



/*============================================================================
 * OpcUa_Buffer_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Buffer_Delete(OpcUa_Buffer** handle)
{
    if (handle != OpcUa_Null && *handle != OpcUa_Null)
    {
        OpcUa_Buffer* buffer = (OpcUa_Buffer*)*handle;

        if(buffer->FreeBuffer)
        {
            OpcUa_Free(buffer->Data);
        }

        OpcUa_Free(buffer);

        *handle = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_Buffer_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_Buffer_Clear(OpcUa_Buffer* a_pBuffer)
{
    if(a_pBuffer != OpcUa_Null)
    {
        if(a_pBuffer->FreeBuffer)
        {
            OpcUa_Free(a_pBuffer->Data);
        }

        OpcUa_MemSet(a_pBuffer, 0, sizeof(OpcUa_Buffer));
    }
}

/*============================================================================
 * OpcUa_Buffer_Read
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Buffer_Read(
    OpcUa_Handle  a_Handle,
    OpcUa_Byte*   data,
    OpcUa_UInt32* count)
{
    OpcUa_UInt32 bytesToRead = 0;
    OpcUa_Buffer* buffer = OpcUa_Null;
    OpcUa_StatusCode uStatus = OpcUa_Good;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_Handle);
    OpcUa_ReturnErrorIfArgumentNull(data);
    OpcUa_ReturnErrorIfArgumentNull(count);

    OpcUa_ReturnErrorIfInvalidBuffer();

    buffer = (OpcUa_Buffer*)a_Handle;

    if (*count == 0)
    {
        return OpcUa_Good;
    }

    bytesToRead = buffer->EndOfData - buffer->Position;

    if (bytesToRead == 0)
    {
        *count = 0;
        return OpcUa_BadEndOfStream;
    }

    if (bytesToRead > *count)
    {
        bytesToRead = *count;
    }

    OpcUa_MemCpy(data, *count, buffer->Data+buffer->Position, bytesToRead);
    buffer->Position += bytesToRead;

    *count = bytesToRead;

    return uStatus;
}

/*============================================================================
 * OpcUa_Buffer_Skip
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_Skip(
    OpcUa_Handle  a_Handle,
    OpcUa_UInt32  a_uLength)
{
    OpcUa_UInt32    uBytesAvailable = 0;
    OpcUa_Buffer*   pBuffer         = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);
    OpcUa_ReturnErrorIfArgumentNull(a_Handle);
    OpcUa_ReturnErrorIfInvalidBuffer();

    /* we're done skip length is zero */
    if(a_uLength == 0)
    {
        return OpcUa_Good;
    }

    pBuffer = (OpcUa_Buffer*)a_Handle;

    /* check if remaining buffer content is larger than amount of bytes to skip */
    uBytesAvailable = pBuffer->EndOfData - pBuffer->Position;

    if(uBytesAvailable == 0 || uBytesAvailable < a_uLength)
    {
        return OpcUa_BadEndOfStream;
    }

    /* set current position to new value */
    pBuffer->Position += a_uLength;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Buffer_Write
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_Write(
    OpcUa_Handle a_Handle,
    OpcUa_Byte*  a_pData,
    OpcUa_UInt32 a_uCount)
{
    OpcUa_UInt32        bytesAvailable = 0;
    OpcUa_Buffer*       buffer = OpcUa_Null;
    OpcUa_StatusCode    uStatus = OpcUa_Good;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pData);

    OpcUa_ReturnErrorIfInvalidBuffer();

    buffer = (OpcUa_Buffer*)a_Handle;

    bytesAvailable = buffer->Size - buffer->Position;

    if(bytesAvailable < a_uCount)
    {
        /* Try to prevent to get into this codepath by setting the buffer big enough. */

        OpcUa_Byte*  newData = OpcUa_Null;
        OpcUa_UInt32 newSize = buffer->Size;

        newSize += a_uCount - bytesAvailable;

        /* round up to BlockSize */
        if(newSize % buffer->BlockSize)
        {
            newSize += buffer->BlockSize - newSize % buffer->BlockSize;
        }

        /* all or nothing write - fail if the entire block can't be written */
        if(     buffer->MaxSize != 0
            &&  newSize > buffer->MaxSize)
        {
            return OpcUa_BadEndOfStream;
        }

        /* Reallocate new buffer */
        newData = (OpcUa_Byte *)OpcUa_ReAlloc(buffer->Data, newSize);

        if (newData == OpcUa_Null)
        {
            return OpcUa_BadOutOfMemory;
        }

        buffer->Data = newData;
        buffer->Size = newSize;

        bytesAvailable = buffer->Size - buffer->Position;
    }

    /* copy new data into buffer */
    OpcUa_MemCpy(   buffer->Data+buffer->Position,
                    bytesAvailable,
                    a_pData,
                    a_uCount);

    buffer->Position += a_uCount;

    /* update end of data marker */
    if (buffer->EndOfData < buffer->Position)
    {
        buffer->EndOfData = buffer->Position;
    }

    return uStatus;
}

/*============================================================================
 * OpcUa_Buffer_GetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_GetPosition(
    OpcUa_Handle  a_Handle,
    OpcUa_UInt32* position)
{
    OpcUa_Buffer* buffer = OpcUa_Null;
    OpcUa_StatusCode uStatus = OpcUa_Good;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_Handle);
    OpcUa_ReturnErrorIfArgumentNull(position);
    OpcUa_ReturnErrorIfInvalidBuffer();

    buffer = (OpcUa_Buffer*)a_Handle;

    *position = buffer->Position;

    return uStatus;
}

/*============================================================================
 * OpcUa_Buffer_SetPosition
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Buffer_SetPosition(
    OpcUa_Handle a_Handle,
    OpcUa_UInt32 a_uPosition)
{
    OpcUa_Buffer* pBuffer = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_Handle);
    OpcUa_ReturnErrorIfInvalidBuffer();

    pBuffer = (OpcUa_Buffer*)a_Handle;

    if (a_uPosition == OpcUa_BufferPosition_Start)
    {
        pBuffer->Position = 0;
        return OpcUa_Good;
    }

    if (a_uPosition == OpcUa_BufferPosition_End)
    {
        pBuffer->Position = pBuffer->EndOfData;
        return OpcUa_Good;
    }

    if (a_uPosition > pBuffer->EndOfData)
    {
        return OpcUa_BadEndOfStream;
    }

    pBuffer->Position = a_uPosition;
    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Buffer_SetEmpty
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_SetEmpty(OpcUa_Buffer* a_pBuffer)
{
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    a_pBuffer->EndOfData = 0;
    a_pBuffer->Position  = 0;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Buffer_IsEmpty
 *===========================================================================*/
OpcUa_Boolean OpcUa_Buffer_IsEmpty(OpcUa_Buffer* a_pBuffer)
{
    return (((a_pBuffer == OpcUa_Null)||(a_pBuffer->EndOfData==0))?OpcUa_True:OpcUa_False);
}

/*============================================================================
 * OpcUa_Buffer_SetEndOfData
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Buffer_SetEndOfData(
    OpcUa_Handle a_Handle,
    OpcUa_UInt32 a_uEndOfData)
{
    OpcUa_Buffer* pBuffer = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_Handle);
    OpcUa_ReturnErrorIfInvalidBuffer();

    pBuffer = (OpcUa_Buffer*)a_Handle;

    if(a_uEndOfData > pBuffer->Size)
    {
        return OpcUa_BadEndOfStream;
    }

    pBuffer->EndOfData = a_uEndOfData;

    return OpcUa_Good;
}
/*============================================================================
 * OpcUa_Buffer_GetData
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Buffer_GetData(
    OpcUa_Handle  a_Handle,
    OpcUa_Byte**  a_ppData,
    OpcUa_UInt32* a_uLength)
{
    OpcUa_Buffer* pBuffer = OpcUa_Null;
    OpcUa_StatusCode uStatus = OpcUa_Good;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Buffer);

    OpcUa_ReturnErrorIfArgumentNull(a_Handle);

    OpcUa_ReturnErrorIfInvalidBuffer();

    pBuffer = (OpcUa_Buffer*)a_Handle;

    if(a_ppData != OpcUa_Null)
    {
        *a_ppData = pBuffer->Data;
    }

    if(a_uLength != OpcUa_Null)
    {
        *a_uLength = pBuffer->EndOfData;
    }

    return uStatus;
}
