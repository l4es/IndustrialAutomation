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
#include <opcua_stream.h>


/*============================================================================
 * OpcUa_Stream_Read
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_Read(
    OpcUa_InputStream*             istrm,
    OpcUa_Byte*                    buffer,
    OpcUa_UInt32*                  count)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Stream);
    OpcUa_ReturnErrorIfArgumentNull(istrm);
    OpcUa_ReturnErrorIfArgumentNull(istrm->Read);

    return istrm->Read(istrm, buffer, count);
}

/*============================================================================
 * OpcUa_Stream_Write
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_Write(
    OpcUa_OutputStream* ostrm,
    OpcUa_Byte*         buffer,
    OpcUa_UInt32        count)
{
    /*OpcUa_StatusCode uStatus = OpcUa_Good;*/
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Stream);

    OpcUa_ReturnErrorIfArgumentNull(ostrm);
    OpcUa_ReturnErrorIfArgumentNull(ostrm->Write);

    return ostrm->Write(ostrm, buffer, count);
}

/*============================================================================
 * OpcUa_Stream_Flush
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_Flush(
    OpcUa_OutputStream* ostrm,
    OpcUa_Boolean       lastCall)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Stream);
    OpcUa_ReturnErrorIfArgumentNull(ostrm);
    OpcUa_ReturnErrorIfArgumentNull(ostrm->Flush);

    return ostrm->Flush(ostrm, lastCall);
}

/*============================================================================
 * OpcUa_Stream_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_Close(
    OpcUa_Stream* strm)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Stream);
    OpcUa_ReturnErrorIfArgumentNull(strm);
    OpcUa_ReturnErrorIfArgumentNull(strm->Close);

    return strm->Close(strm);
}

/*============================================================================
 * OpcUa_Stream_GetChunkLength
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_GetChunkLength(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32*         length)
{
    OpcUa_ReturnErrorIfArgumentNull(strm);
    OpcUa_ReturnErrorIfArgumentNull(length);
    OpcUa_ReturnErrorIfArgumentNull(strm->GetChunkLength);

    return strm->GetChunkLength(strm, length);
}

/*============================================================================
 * OpcUa_Stream_AttachBuffer
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_AttachBuffer(
    struct _OpcUa_Stream*   strm,
    OpcUa_Buffer*           buffer)
{
    OpcUa_ReturnErrorIfArgumentNull(strm);
    OpcUa_ReturnErrorIfArgumentNull(buffer);
    OpcUa_ReturnErrorIfArgumentNull(strm->AttachBuffer);

    return strm->AttachBuffer(strm, buffer);
}


/*============================================================================
 * OpcUa_Stream_DetachBuffer
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_DetachBuffer(
    struct _OpcUa_Stream*   strm,
    OpcUa_Buffer*           buffer)
{
    OpcUa_ReturnErrorIfArgumentNull(strm);
    OpcUa_ReturnErrorIfArgumentNull(buffer);
    OpcUa_ReturnErrorIfArgumentNull(strm->DetachBuffer);

    return strm->DetachBuffer(strm, buffer);
}

/*============================================================================
 * OpcUa_Stream_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Stream_Delete(
    struct _OpcUa_Stream** strm)
{
    if (strm != OpcUa_Null && *strm != OpcUa_Null)
    {
        (*strm)->Delete(strm);
    }
}

/*============================================================================
 * OpcUa_Stream_GetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_GetPosition(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32*         position)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Stream);
    OpcUa_ReturnErrorIfArgumentNull(strm);
    OpcUa_ReturnErrorIfArgumentNull(strm->GetPosition);

    return strm->GetPosition(strm, position);
}

/*============================================================================
 * OpcUa_Stream_SetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Stream_SetPosition(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32          position)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Stream);
    OpcUa_ReturnErrorIfArgumentNull(strm);
    OpcUa_ReturnErrorIfArgumentNull(strm->SetPosition);

    return strm->SetPosition(strm, position);
}
