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
#include <opcua_socket.h>
#include <opcua_list.h>
#include <opcua_binaryencoder.h>
#include <opcua_tcpconnection.h>
#include <opcua_tcplistener.h>
#include <opcua_tcpstream.h>

/* for debugging reasons */
#include <opcua_p_binary.h>

#define OpcUa_TcpOutputStream_SanityCheck 0x5B5941A2
#define OpcUa_TcpInputStream_SanityCheck 0x5B5941A6

/* only for testing, dont deactivate. may get removed. */
#define OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER OPCUA_CONFIG_YES

#if !OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER
#  error NOT SUPPORTED!
#endif /* OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER */

/*============================================================================
 * OpcUa_TcpStream_DetachBuffer
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_DetachBuffer(  OpcUa_Stream*   a_pStream,
                                                OpcUa_Buffer*   a_pBuffer)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "DetachBuffer");

    OpcUa_GotoErrorIfArgumentNull(a_pStream);
    OpcUa_GotoErrorIfArgumentNull(a_pBuffer);

    switch(a_pStream->Type)
    {
    case OpcUa_StreamType_Output:
        {
            OpcUa_TcpOutputStream* pTcpOutputStream = (OpcUa_TcpOutputStream*)(a_pStream->Handle);

            *a_pBuffer = pTcpOutputStream->Buffer;
            pTcpOutputStream->Buffer.Data = OpcUa_Null;
            OpcUa_Buffer_Clear(&pTcpOutputStream->Buffer);

            break;
        }
    case OpcUa_StreamType_Input:
        {
            OpcUa_TcpInputStream* pTcpInputStream = (OpcUa_TcpInputStream*)a_pStream;

            *a_pBuffer = pTcpInputStream->Buffer;
            pTcpInputStream->Buffer.Data = OpcUa_Null;
            OpcUa_Buffer_Clear(&pTcpInputStream->Buffer);

            pTcpInputStream->State = OpcUa_TcpStream_State_Empty;
            pTcpInputStream->Base.Close((OpcUa_Stream*)pTcpInputStream);

            break;
        }
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
            OpcUa_GotoError;
        }
    }


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpStream_AttachBuffer
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_AttachBuffer(  OpcUa_Stream*   a_pStream,
                                                OpcUa_Buffer*   a_pBuffer)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "AttachBuffer");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);

    switch(a_pStream->Type)
    {
    case OpcUa_StreamType_Output:
        {
            OpcUa_TcpOutputStream*  pTcpOutputStream    = (OpcUa_TcpOutputStream*)(a_pStream->Handle);
            OpcUa_Buffer            OldBuffer           = pTcpOutputStream->Buffer;

            pTcpOutputStream->Buffer = *a_pBuffer;

            /* create same state as data would have been written into the stream */
            uStatus = OpcUa_Buffer_SetPosition(&pTcpOutputStream->Buffer, OpcUa_BufferPosition_End);
            if(OpcUa_IsBad(uStatus))
            {
                /* restore old buffer */
                pTcpOutputStream->Buffer = OldBuffer;

            }

            a_pBuffer->Data = OpcUa_Null;
            OpcUa_Buffer_Clear(a_pBuffer);

            break;
        }
    case OpcUa_StreamType_Input:
        {
            OpcUa_TcpInputStream* pTcpInputStream = (OpcUa_TcpInputStream*)(a_pStream->Handle);

            OpcUa_ReferenceParameter(pTcpInputStream);

            uStatus = OpcUa_BadNotSupported;

            break;
        }
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
            OpcUa_GotoError;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpStream_GetChunkLength
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_GetChunkLength(    OpcUa_Stream* a_pStream,
                                                    OpcUa_UInt32* a_puLength)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "GetChunkLength");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_puLength);

    switch(a_pStream->Type)
    {
    case OpcUa_StreamType_Input:
        {
            OpcUa_TcpInputStream* istrm = (OpcUa_TcpInputStream*)(a_pStream->Handle);
            *a_puLength = istrm->BufferSize;
            break;
        }
    case OpcUa_StreamType_Output:
        {
            OpcUa_TcpOutputStream* ostrm = (OpcUa_TcpOutputStream*)(a_pStream->Handle);
            *a_puLength = ostrm->BufferSize;
            break;
        }
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
            OpcUa_GotoError;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpStream_Read
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_Read(
    OpcUa_InputStream*             a_pIstrm,            /* Stream with TcpStream handle */
    OpcUa_Byte*                    a_pTargetBuffer,     /* The destination buffer. */
    OpcUa_UInt32*                  a_puCount)           /* How many bytes should be delivered. */
{
    OpcUa_TcpInputStream* pTcpInputStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "Read");

    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pTargetBuffer);
    OpcUa_ReturnErrorIfArgumentNull(a_puCount);
    OpcUa_ReturnErrorIfInvalidStream(a_pIstrm, Read);

    /* HINTS: we dont want to trigger a socket recv for every element, so the
              data gets buffered internally in the stream to read "as much as
              possible" (well, not really, trying to predict message borders
              implicitly through buffer sizes.) in one api call for performance
              reasons.

              A read looks, if the requested amount of data is available in
              the internal buffer and copies it into the target. The caller
              must swap into the right byte order afterwards.
              */

    /* resolve stream handle to tcp stream */
    pTcpInputStream  = (OpcUa_TcpInputStream*)(a_pIstrm->Handle);

    /* check for end of stream */
    uStatus = OpcUa_Buffer_Read(&(pTcpInputStream->Buffer), a_pTargetBuffer, a_puCount);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_TcpStream_Write
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_Write(
    OpcUa_OutputStream* a_pOstrm,           /* the stream to write the value into */
    OpcUa_Byte*         a_pInBuffer,        /* the value to write */
    OpcUa_UInt32        a_uInBufferSize)    /* the size of the value to write */
{
    OpcUa_TcpOutputStream*  pTcpOutputStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "Write");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pInBuffer);

    pTcpOutputStream = (OpcUa_TcpOutputStream*)a_pOstrm->Handle;

    OpcUa_ReturnErrorIfInvalidStream(a_pOstrm, Write);

    if(pTcpOutputStream->Closed)
    {
        return OpcUa_BadInvalidState;
    }

    /* write data to output buffer - flush to network as required */
    if((pTcpOutputStream->Buffer.Position + a_uInBufferSize) > pTcpOutputStream->Buffer.Size)
    {
        if(pTcpOutputStream->MessageType == OpcUa_TcpStream_MessageType_SecureChannel)
        {
            /* The secure channel should never trigger automatic flushing. */
            /* At this point something went wrong. */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "ERROR: automatic flush in secure channel message should not happen!");
            OpcUa_GotoErrorWithStatus(OpcUa_BadEndOfStream);
        }

        /* data wouldnt fit into the buffer -> flush to network. */
        /* curser will be reset by flush. */
        uStatus = OpcUa_TcpStream_Flush(    a_pOstrm,
                                            OpcUa_False);
        OpcUa_ReturnErrorIfBad(uStatus);

        /* HINT: This should never happen by the secure channel layer. */
        /*       It will alway know, when to flush by itself. */
        /*       To be safe, check the current position after a write. */
    }

    uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), a_pInBuffer, a_uInBufferSize);
    OpcUa_ReturnErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpStream_Flush
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_Flush(
    OpcUa_OutputStream* a_pOstrm,
    OpcUa_Boolean       a_bLastCall)
{
    OpcUa_TcpOutputStream*  pTcpOutputStream = OpcUa_Null;
    OpcUa_UInt32            tempDataLength  = 0;
    OpcUa_Int32             iDataWritten    = 0;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "Flush");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);
    OpcUa_ReturnErrorIfInvalidStream(a_pOstrm, Flush);

    pTcpOutputStream = (OpcUa_TcpOutputStream*)a_pOstrm->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pTcpOutputStream);

    OpcUa_GotoErrorIfTrue((pTcpOutputStream->Closed), OpcUa_BadInvalidState);

    if(pTcpOutputStream->MaxNoOfFlushes != 0 && ((pTcpOutputStream->NoOfFlushes + 1) >= pTcpOutputStream->MaxNoOfFlushes) && (a_bLastCall == OpcUa_False))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_TcpStream_Flush: Flush no. %u with %u max flushes and final flag %u -> Too many chunks!\n", (pTcpOutputStream->NoOfFlushes + 1), pTcpOutputStream->MaxNoOfFlushes, a_bLastCall);
        return OpcUa_BadTcpMessageTooLarge;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_Flush: Flush no. %u with %u max flushes and final flag %u!\n", (pTcpOutputStream->NoOfFlushes + 1), pTcpOutputStream->MaxNoOfFlushes, a_bLastCall);
    }

#if 1
    if(!OpcUa_Buffer_IsEmpty(&pTcpOutputStream->Buffer))
    {
#endif
    tempDataLength = pTcpOutputStream->Buffer.Position;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_Flush: Messagelength is %d!%s\n", tempDataLength, a_bLastCall?" Last Call!":"");

    /* update header only for own messages; securechannel headers don't get touched */
    if(pTcpOutputStream->MessageType != OpcUa_TcpStream_MessageType_SecureChannel)
    {
        /* update chunk flag */
        if(a_bLastCall != OpcUa_False)
        {
                /* change signature to message complete */
                pTcpOutputStream->Buffer.Data[3] = 'F';
        }

        /* update size */
        pTcpOutputStream->Buffer.Position = 4;

        uStatus = OpcUa_UInt32_BinaryEncode(tempDataLength, a_pOstrm);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    pTcpOutputStream->Buffer.Position = 0;

    /* send to network */
    iDataWritten = OPCUA_P_SOCKET_WRITE(pTcpOutputStream->Socket,
                                        &pTcpOutputStream->Buffer.Data[pTcpOutputStream->Buffer.Position],
                                        tempDataLength,
#if OPCUA_TCPSTREAM_BLOCKINGWRITE
                                        OpcUa_True);
#else /* OPCUA_TCPSTREAM_BLOCKINGWRITE */
                                        OpcUa_False);
#endif /* OPCUA_TCPSTREAM_BLOCKINGWRITE */

    pTcpOutputStream->NoOfFlushes++;

    if(iDataWritten < (OpcUa_Int32)tempDataLength)
    {
        if(iDataWritten < (OpcUa_Int32)0)
        {
            uStatus = OPCUA_P_SOCKET_GETLASTERROR(pTcpOutputStream->Socket);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_TcpStream_Flush: Error writing to socket: 0x%08X!\n", uStatus);

            /* Notify connection! */
            if((pTcpOutputStream->NotifyDisconnect != OpcUa_Null) && (pTcpOutputStream->hConnection != OpcUa_Null))
            {
                pTcpOutputStream->NotifyDisconnect(pTcpOutputStream->hConnection);
            }

            OpcUa_GotoErrorWithStatus(OpcUa_BadDisconnect);
        }
        else
        {
            /* keep as outgoing stream */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_Flush: Only %u bytes of %u written!\n", iDataWritten, tempDataLength);
            /* store position */
            pTcpOutputStream->Buffer.Position = (OpcUa_UInt32)iDataWritten;
            pTcpOutputStream->Buffer.EndOfData = tempDataLength;
            OpcUa_GotoErrorWithStatus(OpcUa_BadWouldBlock);
        }
    }

    /* prepare new flags */
    if(a_bLastCall == OpcUa_False)
    {
        /* Stream will be used again. Reset position. */
        switch(pTcpOutputStream->MessageType)
        {
        case OpcUa_TcpStream_MessageType_SecureChannel:
            {
                /* securechannel will need to encode its header by itself. */
                /* flush during write will cause problems! */
                uStatus = OpcUa_Buffer_SetPosition(&pTcpOutputStream->Buffer, 0);
                break;
            }
        /* we handle our own messages */
        case OpcUa_TcpStream_MessageType_Hello:
        case OpcUa_TcpStream_MessageType_Acknowledge:
        case OpcUa_TcpStream_MessageType_Error:
            {
                uStatus = OpcUa_Buffer_SetPosition(&pTcpOutputStream->Buffer, 3);
                /* encode as new fragment */
                uStatus = OpcUa_Buffer_Write(&pTcpOutputStream->Buffer, (OpcUa_Byte*)"F", 1);
                uStatus = OpcUa_Buffer_SetPosition(&pTcpOutputStream->Buffer, 8);
                break;
            }
        default:
            {
                /* unknown message type */
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
        }
    }
    else
    {
        /* this was the last call -> stream is doomed! */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_Flush: Buffer emptied!\n");
        OpcUa_Buffer_SetEmpty(&pTcpOutputStream->Buffer);
    }
#if 1
    }
    else
    {
        /*OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_Flush: Empty tcp stream flush ignored.\n");*/
    }
#endif

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpStream_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_Close(OpcUa_Stream* a_pStrm)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "Close");

    OpcUa_GotoErrorIfArgumentNull(a_pStrm);
    OpcUa_ReturnErrorIfInvalidStream(a_pStrm, Close);

    if(a_pStrm->Type == OpcUa_StreamType_Output)
    {
        if(((OpcUa_TcpOutputStream*)(a_pStrm->Handle))->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        /* flush buffer if data is available. */
        if(!OpcUa_Buffer_IsEmpty(&(((OpcUa_TcpOutputStream*)a_pStrm->Handle))->Buffer))
        {
            OpcUa_TcpStream_Flush((OpcUa_OutputStream*)a_pStrm, OpcUa_True);
        }

        ((OpcUa_TcpOutputStream*)(a_pStrm->Handle))->Closed = OpcUa_True;
    }
    else if(a_pStrm->Type == OpcUa_StreamType_Input)
    {
        if(((OpcUa_TcpInputStream*)(a_pStrm->Handle))->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        /* TODO: closing a stream before the end of the message could screw things up.
           Need to read rest of message from stream before closing. Thats complex,
           because the rest of the message may be delayed, so we would have to block here,
           what we don't want. Handle this stream like before, but mark it as abandoned!
           If the stream is complete, it will not be handled but deleted immediately.
           Intermediary read events are not further processed. */

        ((OpcUa_TcpInputStream*)(a_pStrm->Handle))->Closed = OpcUa_True;
    }
    else
    {
        return OpcUa_BadInvalidState;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpStream_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_TcpStream_Delete(OpcUa_Stream** a_ppStrm)
{
    if(a_ppStrm == OpcUa_Null)
    {
        /* Errorcondition - should not happen. */
        return;
    }

    if((*a_ppStrm) == OpcUa_Null)
    {
        /* Errorcondition - should not happen. */
        return;
    }


    if((*a_ppStrm)->Type == OpcUa_StreamType_Output)
    {
        OpcUa_TcpOutputStream* ostrm = (OpcUa_TcpOutputStream*)((*a_ppStrm)->Handle);

        OpcUa_Buffer_Clear(&(ostrm->Buffer));

        OpcUa_Free(*a_ppStrm);
        *a_ppStrm = OpcUa_Null;
    }
    else if((*a_ppStrm)->Type == OpcUa_StreamType_Input)
    {
        OpcUa_TcpInputStream* istrm = (OpcUa_TcpInputStream*)((*a_ppStrm)->Handle);

        if(!istrm->Closed)
        {
            /* Errorcondition - should not happen. */
            return;
        }

        /* clear buffer */
        /* Delete ignores OpcUa_Null, so if the buffer got detached by the upper layer, this works, too. */
        OpcUa_Buffer_Clear(&(istrm->Buffer));

        /* OpcUa_Free(istrm);*/
        OpcUa_Free(*a_ppStrm);
        *a_ppStrm = OpcUa_Null;
    }
    else
    {
        /* Errorcondition - should not happen. */
        return;
    }
}

/*============================================================================
 * OpcUa_TcpStream_GetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_GetPosition(
    OpcUa_Stream* a_pStrm,
    OpcUa_UInt32* a_pPosition)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "GetPosition");

    OpcUa_ReturnErrorIfArgumentNull(a_pStrm);
    OpcUa_ReturnErrorIfInvalidStream(a_pStrm, GetPosition);
    OpcUa_ReferenceParameter(a_pPosition);

    if(a_pStrm->Type == OpcUa_StreamType_Output)
    {
        OpcUa_TcpOutputStream* tcpStream = (OpcUa_TcpOutputStream*)(a_pStrm->Handle);

        if(tcpStream->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        *a_pPosition = tcpStream->Buffer.Position;
    }
    else if(a_pStrm->Type == OpcUa_StreamType_Input)
    {
        OpcUa_TcpInputStream* tcpStream = (OpcUa_TcpInputStream*)(a_pStrm->Handle);

        if(tcpStream->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        *a_pPosition = tcpStream->Buffer.Position;
    }
    else
    {
        uStatus = OpcUa_BadInvalidArgument;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Stream_SetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_SetPosition(
    OpcUa_Stream* a_pStrm,
    OpcUa_UInt32  a_uPosition)
{
OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "SetPosition");

    OpcUa_ReturnErrorIfArgumentNull(a_pStrm);
    OpcUa_ReturnErrorIfInvalidStream(a_pStrm, SetPosition);

    if(a_pStrm->Type == OpcUa_StreamType_Output)
    {
        OpcUa_TcpOutputStream* tcpStream = (OpcUa_TcpOutputStream*)(a_pStrm->Handle);

        if(tcpStream->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        /* set the position */
        uStatus = OpcUa_Buffer_SetPosition(&(tcpStream->Buffer), a_uPosition);
    }
    else if(a_pStrm->Type == OpcUa_StreamType_Input)
    {
        OpcUa_TcpInputStream* tcpStream = (OpcUa_TcpInputStream*)(a_pStrm->Handle);

        if(tcpStream->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        uStatus = OpcUa_Buffer_SetPosition(&tcpStream->Buffer, a_uPosition);
    }
    else
    {
        uStatus = OpcUa_BadInvalidArgument;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}



/*============================================================================
 * OpcUa_TcpStream_CreateInput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_CreateInput(   OpcUa_Socket        a_hSocket,
                                                OpcUa_UInt32        a_uBufferSize,
                                                OpcUa_InputStream** a_ppIstrm)
{
    OpcUa_TcpInputStream*   pTcpInputStream  = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "CreateInput");

    OpcUa_ReturnErrorIfArgumentNull(a_ppIstrm);

    *a_ppIstrm = OpcUa_Null;

    pTcpInputStream = (OpcUa_TcpInputStream*)OpcUa_Alloc(sizeof(OpcUa_TcpInputStream));
    OpcUa_GotoErrorIfAllocFailed(pTcpInputStream);
    OpcUa_MemSet(pTcpInputStream, 0, sizeof(OpcUa_TcpInputStream));

    pTcpInputStream->SanityCheck     = OpcUa_TcpInputStream_SanityCheck;
    pTcpInputStream->Closed          = OpcUa_False;
    pTcpInputStream->Socket          = a_hSocket;
    pTcpInputStream->MessageLength   = 0;
    pTcpInputStream->State           = OpcUa_TcpStream_State_Empty;
    pTcpInputStream->IsFinal         = OpcUa_False;
    pTcpInputStream->BufferSize      = a_uBufferSize;

    *a_ppIstrm = (OpcUa_InputStream*)pTcpInputStream;

    (*a_ppIstrm)->Type              = OpcUa_StreamType_Input;
    (*a_ppIstrm)->Handle            = pTcpInputStream;
    (*a_ppIstrm)->GetPosition       = OpcUa_TcpStream_GetPosition;
    (*a_ppIstrm)->SetPosition       = OpcUa_TcpStream_SetPosition;
    (*a_ppIstrm)->GetChunkLength    = OpcUa_TcpStream_GetChunkLength;
    (*a_ppIstrm)->DetachBuffer      = OpcUa_TcpStream_DetachBuffer;
    (*a_ppIstrm)->AttachBuffer      = OpcUa_TcpStream_AttachBuffer;
    (*a_ppIstrm)->Close             = OpcUa_TcpStream_Close;
    (*a_ppIstrm)->Delete            = OpcUa_TcpStream_Delete;
    (*a_ppIstrm)->Read              = OpcUa_TcpStream_Read;


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Free(pTcpInputStream);

    *a_ppIstrm = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

#if !OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER
/*============================================================================
 * OpcUa_TcpStream_EncodeChunkHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_EncodeChunkHeader( OpcUa_OutputStream* a_pOstrm)
{
    OpcUa_TcpOutputStream* pTcpOutputStream = (OpcUa_TcpOutputStream*)a_pOstrm->Handle;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "EncodeChunkHeader");

    /* prepare message header */
    switch(pTcpOutputStream->MessageType)
    {
    case OpcUa_TcpStream_MessageType_Error:
        {
            /* encode message type signature */
            uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), (OpcUa_Byte*)OPCUA_TCPSTREAM_SIGNATURE_ERROR, 4);
            OpcUa_GotoErrorIfBad(uStatus);
            /* reserve message length field */
            uStatus = OpcUa_UInt32_BinaryEncode(0, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);
            break;
        }
    case OpcUa_TcpStream_MessageType_Hello:
        {
            /* encode message type signature */
            uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), (OpcUa_Byte*)OPCUA_TCPSTREAM_SIGNATURE_HELLO, 4);
            OpcUa_GotoErrorIfBad(uStatus);
            /* reserve message length field */
            uStatus = OpcUa_UInt32_BinaryEncode(0xFFFFFFFF, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);
            break;
        }
    case OpcUa_TcpStream_MessageType_Acknowledge:
        {
            /* encode message type signature */
            uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), (OpcUa_Byte*)OPCUA_TCPSTREAM_SIGNATURE_ACKNOWLEDGE, 4);
            OpcUa_GotoErrorIfBad(uStatus);
            /* reserve message length field */
            uStatus = OpcUa_UInt32_BinaryEncode(0xAAAAAAAA, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);
            break;
        }
    case OpcUa_TcpStream_MessageType_Unknown:
    case OpcUa_TcpStream_MessageType_SecureChannel:
        {
            /* no preencoding possible */
            break;
        }
    case OpcUa_TcpStream_MessageType_Invalid:
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
            goto Error;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
#endif /* OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER */

/*============================================================================
 * OpcUa_TcpStream_CreateOutput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpStream_CreateOutput(  OpcUa_Socket                        a_hSocket,
                                                OpcUa_TcpStream_MessageType         a_eMessageType,
                                                OpcUa_Byte**                        a_ppAttachBuffer,
                                                OpcUa_UInt32                        a_uBufferSize,
                                                OpcUa_TcpStream_PfnNotifyDisconnect a_pfnDisconnectCB,
                                                OpcUa_UInt32                        a_uMaxNoOfFlushes,
                                                OpcUa_OutputStream**                a_ppOstrm)
{
    OpcUa_TcpOutputStream*  pTcpOutputStream = OpcUa_Null;
    OpcUa_Byte*             pData            = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "CreateOutput");

    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    *a_ppOstrm = OpcUa_Null;

    if(a_ppAttachBuffer != OpcUa_Null)
    {
        /* if the caller wants the stream to be attached, dont alloc it */
        OpcUa_ReturnErrorIfArgumentNull(*a_ppAttachBuffer);

        /* allocate tcp out stream */
        pTcpOutputStream = (OpcUa_TcpOutputStream*)OpcUa_Alloc(sizeof(OpcUa_TcpOutputStream));
        OpcUa_GotoErrorIfAllocFailed(pTcpOutputStream);
        OpcUa_MemSet(pTcpOutputStream, 0, sizeof(OpcUa_TcpOutputStream));

        /* set data pointer to external buffer */
        pData = *a_ppAttachBuffer;
    }
    else if(a_eMessageType == OpcUa_TcpStream_MessageType_SecureChannel)
    {
        /* allocate tcp out stream */
        pTcpOutputStream = (OpcUa_TcpOutputStream*)OpcUa_Alloc(sizeof(OpcUa_TcpOutputStream));
        OpcUa_GotoErrorIfAllocFailed(pTcpOutputStream);
        OpcUa_MemSet(pTcpOutputStream, 0, sizeof(OpcUa_TcpOutputStream));
    }
    else
    {
#if OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER
        /* allocate the stream and buffer in one go */
        OpcUa_UInt32 uAllocSize = (sizeof(OpcUa_TcpOutputStream) + a_uBufferSize);

        /* allocate tcp out stream */
        pTcpOutputStream = (OpcUa_TcpOutputStream*)OpcUa_Alloc(uAllocSize);
        OpcUa_GotoErrorIfAllocFailed(pTcpOutputStream);
        OpcUa_MemSet(pTcpOutputStream, 0, sizeof(OpcUa_TcpOutputStream));

        /* set datapointer to datasegment in stream */
        pData = (OpcUa_Byte*)((OpcUa_Byte*)pTcpOutputStream + sizeof(OpcUa_TcpOutputStream));
#else
        /* allocate tcp out stream */
        pTcpOutputStream = (OpcUa_TcpOutputStream*)OpcUa_Alloc(sizeof(OpcUa_TcpOutputStream));
        OpcUa_GotoErrorIfAllocFailed(pTcpOutputStream);
        OpcUa_MemSet(pTcpOutputStream, 0, sizeof(OpcUa_TcpOutputStream));
#endif /* OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER */
    }

    pTcpOutputStream->SanityCheck        = OpcUa_TcpOutputStream_SanityCheck;
    pTcpOutputStream->MessageType        = a_eMessageType;
    pTcpOutputStream->Closed             = OpcUa_False;
    pTcpOutputStream->Socket             = a_hSocket;
    pTcpOutputStream->BufferSize         = a_uBufferSize;
    pTcpOutputStream->NotifyDisconnect   = a_pfnDisconnectCB;
    pTcpOutputStream->MaxNoOfFlushes     = a_uMaxNoOfFlushes;

    /* now initialize superclass members */
    *a_ppOstrm = (OpcUa_OutputStream*)pTcpOutputStream;

    (*a_ppOstrm)->Type              = OpcUa_StreamType_Output;
    (*a_ppOstrm)->Handle            = pTcpOutputStream;
    (*a_ppOstrm)->GetPosition       = OpcUa_TcpStream_GetPosition;
    (*a_ppOstrm)->SetPosition       = OpcUa_TcpStream_SetPosition;
    (*a_ppOstrm)->GetChunkLength    = OpcUa_TcpStream_GetChunkLength;
    (*a_ppOstrm)->DetachBuffer      = OpcUa_TcpStream_DetachBuffer;
    (*a_ppOstrm)->AttachBuffer      = OpcUa_TcpStream_AttachBuffer;
    (*a_ppOstrm)->Close             = OpcUa_TcpStream_Close;
    (*a_ppOstrm)->Delete            = OpcUa_TcpStream_Delete;
    (*a_ppOstrm)->Write             = OpcUa_TcpStream_Write;
    (*a_ppOstrm)->Flush             = OpcUa_TcpStream_Flush;

    /* create internal buffer with fixed buffersize. */
    uStatus = OpcUa_Buffer_Initialize(  &(pTcpOutputStream->Buffer), /* instance           */
                                        pData,                      /* bufferdata         */
                                        a_uBufferSize,              /* buffersize         */
                                        a_uBufferSize,              /* blocksize          */
                                        a_uBufferSize,              /* maxsize            */
                                        OpcUa_False);               /* do not free buffer */
    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER

    /* prepare message header */
    switch(pTcpOutputStream->MessageType)
    {
    case OpcUa_TcpStream_MessageType_Error:
        {
            /* encode message type signature */
            uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), (OpcUa_Byte*)OPCUA_TCPSTREAM_SIGNATURE_ERROR, 4);
            OpcUa_GotoErrorIfBad(uStatus);
            /* reserve message length field */
            uStatus = OpcUa_UInt32_BinaryEncode(0, *a_ppOstrm);
            OpcUa_GotoErrorIfBad(uStatus);
            break;
        }
    case OpcUa_TcpStream_MessageType_Hello:
        {
            /* encode message type signature */
            uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), (OpcUa_Byte*)OPCUA_TCPSTREAM_SIGNATURE_HELLO, 4);
            OpcUa_GotoErrorIfBad(uStatus);
            /* reserve message length field */
            uStatus = OpcUa_UInt32_BinaryEncode(0xFFFFFFFF, *a_ppOstrm);
            OpcUa_GotoErrorIfBad(uStatus);
            break;
        }
    case OpcUa_TcpStream_MessageType_Acknowledge:
        {
            /* encode message type signature */
            uStatus = OpcUa_Buffer_Write(&(pTcpOutputStream->Buffer), (OpcUa_Byte*)OPCUA_TCPSTREAM_SIGNATURE_ACKNOWLEDGE, 4);
            OpcUa_GotoErrorIfBad(uStatus);
            /* reserve message length field */
            uStatus = OpcUa_UInt32_BinaryEncode(0xAAAAAAAA, *a_ppOstrm);
            OpcUa_GotoErrorIfBad(uStatus);
            break;
        }
    case OpcUa_TcpStream_MessageType_Unknown:
    case OpcUa_TcpStream_MessageType_SecureChannel:
        {
            /* no preencoding possible */
            break;
        }
    case OpcUa_TcpStream_MessageType_Invalid:
    default:
        {
            uStatus = OpcUa_BadInvalidArgument;
            goto Error;
        }
    }

#endif /* OPCUA_TCPSTREAM_PREENCODE_CHUNK_HEADER */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_TcpStream_Delete((OpcUa_Stream**)&pTcpOutputStream);

    *a_ppOstrm = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
* OpcUa_TcpStream_CheckHeader
*===========================================================================*/
/** @brief Parses the message header. */
static OpcUa_StatusCode OpcUa_TcpStream_CheckHeader(OpcUa_InputStream* a_InputStream)
{
    OpcUa_StatusCode        uStatus         = OpcUa_Good;
    OpcUa_TcpInputStream*   pTcpInputStream = OpcUa_Null;
    OpcUa_UInt32            nTempPosition   = 0;
    OpcUa_CharA             chTemp          = 'x';

    pTcpInputStream = (OpcUa_TcpInputStream*)(a_InputStream->Handle);

    if(pTcpInputStream->Buffer.EndOfData > 0)
    {
        pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_Unknown;
        pTcpInputStream->State = OpcUa_TcpStream_State_HeaderStarted;
    }


    /* HINT: simplified: waits for full signature; could verify from the first byte on. */
    if(pTcpInputStream->MessageType == OpcUa_TcpStream_MessageType_Unknown)
    {
        if(pTcpInputStream->Buffer.EndOfData >= OPCUA_TCPSTREAM_SIGNATURE_LENGTH)
        {
            pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_Invalid;

            chTemp = pTcpInputStream->Buffer.Data[nTempPosition];

            switch(chTemp)
            {
            case 'H': /* Hello HELF */
                {
                    OpcUa_Int res = OpcUa_MemCmp((OpcUa_Void*)&pTcpInputStream->Buffer.Data[nTempPosition],
                                                 (OpcUa_Void*)OPCUA_TCPSTREAM_SIGNATURE_HELLO,
                                                 OPCUA_TCPSTREAM_SIGNATURE_LENGTH);

                    /* will be processed directly in the host */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_Hello;

                    if(res != 0)
                    {
                        return OpcUa_BadEncodingError;
                    }

                    nTempPosition += 3;

                    break;
                }
            case 'A': /* Acknowledge ACKF */
                {
                    OpcUa_Int res = OpcUa_MemCmp((OpcUa_Void*)&pTcpInputStream->Buffer.Data[nTempPosition],
                                                 (OpcUa_Void*)OPCUA_TCPSTREAM_SIGNATURE_ACKNOWLEDGE,
                                                 OPCUA_TCPSTREAM_SIGNATURE_LENGTH);

                    /* will be processed directly in the host */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_Acknowledge;

                    if(res != 0)
                    {
                        return OpcUa_BadEncodingError;
                    }

                    nTempPosition += 3;
                    break;
                }
            case 'E': /* Error ERRF */
                {
                    /* will be processed directly in the host */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_Error;

                    nTempPosition += 3;
                    break;
                }
            case 'O': /* OpenSecureChannel  OPNF */
                {
                    OpcUa_Int res = OpcUa_MemCmp((OpcUa_Void*)&pTcpInputStream->Buffer.Data[nTempPosition],
                                                 (OpcUa_Void*)OPCUA_TCPSTREAM_SIGNATURE_SECURECHANNEL_OPEN,
                                                 OPCUA_TCPSTREAM_SIGNATURE_LENGTH - 1);

                    /* will be forwarded to securechannel */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_SecureChannel;

                    if(res != 0)
                    {
                        return OpcUa_BadEncodingError;
                    }

                    nTempPosition += 3;
                    break;
                }
            case 'C': /* CloseSecureChannel CLOF */
                {
                    OpcUa_Int res = OpcUa_MemCmp((OpcUa_Void*)&pTcpInputStream->Buffer.Data[nTempPosition],
                                                 (OpcUa_Void*)OPCUA_TCPSTREAM_SIGNATURE_SECURECHANNEL_CLOSE,
                                                 OPCUA_TCPSTREAM_SIGNATURE_LENGTH - 1);

                    /* will be forwarded to securechannel */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_SecureChannel;

                    if(res != 0)
                    {
                        return OpcUa_BadEncodingError;
                    }

                    nTempPosition += 3;
                    break;
                }
            case 'M': /* Message            MSG? */
                {
                    OpcUa_Int res = OpcUa_MemCmp((OpcUa_Void*)&pTcpInputStream->Buffer.Data[nTempPosition],
                                                 (OpcUa_Void*)OPCUA_TCPSTREAM_SIGNATURE_SECURECHANNEL_MESSAGE,
                                                 OPCUA_TCPSTREAM_SIGNATURE_LENGTH - 1);

                    /* will be forwarded to securechannel */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_SecureChannel;

                    if(res != 0)
                    {
                        return OpcUa_BadEncodingError;
                    }

                    nTempPosition += 3;
                    break;
                }
            default:
                {
                    /* invalid signature */
                    pTcpInputStream->MessageType = OpcUa_TcpStream_MessageType_Invalid;
                    break;
                }
            }

            if(    pTcpInputStream->MessageType == OpcUa_TcpStream_MessageType_Unknown
                || pTcpInputStream->MessageType == OpcUa_TcpStream_MessageType_Invalid)
            {
                /* enough data was available, but the first bytes did not match the defined signatures */
                return OpcUa_BadDecodingError;
            }

            chTemp = pTcpInputStream->Buffer.Data[nTempPosition];

            if(     pTcpInputStream->MessageType == OpcUa_TcpStream_MessageType_Error
                ||  pTcpInputStream->MessageType == OpcUa_TcpStream_MessageType_Hello
                ||  pTcpInputStream->MessageType == OpcUa_TcpStream_MessageType_Acknowledge)
            {
                /* ignore this field and treat this message as final */
                pTcpInputStream->IsFinal = OpcUa_True;
                pTcpInputStream->IsAbort = OpcUa_False;
            }
            else
            {
                /* check if message is last in chain */
                if(chTemp == 'F')
                {
                    pTcpInputStream->IsFinal = OpcUa_True;
                    pTcpInputStream->IsAbort = OpcUa_False;
                }
                else if(chTemp == 'C')
                {
                    pTcpInputStream->IsFinal = OpcUa_False;
                    pTcpInputStream->IsAbort = OpcUa_False;
                }
                else if(chTemp == 'A')
                {
                    /* no more chunks for this message */
                    pTcpInputStream->IsFinal = OpcUa_True;
                    pTcpInputStream->IsAbort = OpcUa_True;
                }
                else
                {
                    return OpcUa_BadDecodingError;
                }
            }
        }
        else /* if(pTcpInputStream->EndOfData >= OPCUA_TCPSTREAM_SIGNATURE_LENGTH) */
        {
            /* not enough data, call me again */
            return OpcUa_GoodCallAgain;
        }
    }

    /* signature done */
    nTempPosition++;
    /*OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_CheckHeader: Signature parsed!\n");*/

    /* parse length field */
    if(    pTcpInputStream->MessageType != OpcUa_TcpStream_MessageType_Unknown
        && pTcpInputStream->MessageType != OpcUa_TcpStream_MessageType_Invalid)
    {

        /* Check if message length is already in the buffer! */
        if((pTcpInputStream->Buffer.EndOfData - nTempPosition) >= OPCUA_TCPSTREAM_MESSAGELENGTH_LENGTH)
        {
            /* from here, we read through the stream interface; update the state variables */
            pTcpInputStream->Buffer.Position = nTempPosition;

            /* since the data is available for sure, no blocking can happen in the following call */
            uStatus = OpcUa_UInt32_BinaryDecode(&(pTcpInputStream->MessageLength), a_InputStream);
            OpcUa_ReturnErrorIfBad(uStatus);
            /* OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_CheckHeader: Messagelength is %d!\n", pTcpInputStream->MessageLength); */
        }
        else
        {
            /* not enough data, call me again */
            return OpcUa_GoodCallAgain;
        }
    }

    return OpcUa_Good;
}



/*============================================================================
 * OpcUa_TcpStream_DataReady
 *===========================================================================*/
/** @brief Called if data is available for reading on a socket attached to a stream.
  *
  * This is kind of a read event handler of the pTcpInputStream. The Listener
  * calls this function, if new data is available on the socket. Dependend of
  * the stream state, it starts handling the tcpstream relevant data and
  * gives feedback to the listener, which takes further action, ie. calls
  * the handler.
  *
  * @param a_pIstrm [ in] The stream for which data is ready to be received.
  *
  * @return StatusCode
  */
OpcUa_StatusCode OpcUa_TcpStream_DataReady(OpcUa_InputStream* a_pIstrm)
{
    OpcUa_TcpInputStream*   pTcpInputStream  = OpcUa_Null;
    OpcUa_UInt32            nLength         = 0;

OpcUa_InitializeStatus(OpcUa_Module_TcpStream, "DataReady");

    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm->Handle);

    pTcpInputStream = (OpcUa_TcpInputStream*)a_pIstrm->Handle;

    /************************************************************************************/
    /* prepare the stream to read from socket */

    if(pTcpInputStream->State == OpcUa_TcpStream_State_Empty)
    {
        /* This is a new stream and a new message. */
        OpcUa_Byte* pData = (OpcUa_Byte*)OpcUa_Alloc(pTcpInputStream->BufferSize);
        OpcUa_ReturnErrorIfAllocFailed(pData);

        uStatus = OpcUa_Buffer_Initialize(  &pTcpInputStream->Buffer,
                                            pData,
                                            0,
                                            pTcpInputStream->BufferSize,
                                            pTcpInputStream->BufferSize,
                                            OpcUa_True);

        if(OpcUa_IsBad(uStatus))
        {
            OpcUa_Buffer_Clear(&pTcpInputStream->Buffer);
            OpcUa_ReturnStatusCode;
        }

        /* set amount to read from socket */
        nLength = OPCUA_TCPSTREAM_MESSAGEHEADER_LENGTH;
    }
    else /* data has already been received into the stream buffer */
    {
        /* Calculate length of data to read */
        if(pTcpInputStream->MessageLength == 0)
        {
            /* read until end of header */
            nLength = OPCUA_TCPSTREAM_MESSAGEHEADER_LENGTH - pTcpInputStream->Buffer.Position;
        }
        else
        {
            /* header was received and message length is known, receive remaining body data */
            nLength = pTcpInputStream->MessageLength - pTcpInputStream->Buffer.Position;
        }
    }

    /************************************************************************************/
    /* based on the current stream state, read and do further processing */

    switch(pTcpInputStream->State)
    {
    case OpcUa_TcpStream_State_Empty:
    case OpcUa_TcpStream_State_HeaderStarted: /* header not yet completed */
        {
            /* security check for length exceeds buffersize (which would be an error) */
            if(nLength + pTcpInputStream->Buffer.Position > pTcpInputStream->BufferSize)
            {
               uStatus = OpcUa_BadInvalidArgument; /* message is too large */
               OpcUa_GotoError;
            }

            /* Read! */
            /*OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_DataReady: (Empty|HeaderStarted) Trying to read %d bytes...\n", nLength);*/
            uStatus = OPCUA_P_SOCKET_READ(  pTcpInputStream->Socket,
                                            &(pTcpInputStream->Buffer.Data[pTcpInputStream->Buffer.Position]),
                                            nLength,
                                            &nLength);
            if(OpcUa_IsBad(uStatus))
            {
                #ifdef _WIN32
                /* HA: 23.02.2010 CPU Q9550 (Intel Core 2 Quad 2.83 GHz Prozessor) returned during a stressful
                 * test a OpcUa_BadWouldBlock and a message length of 0.
                 * Without the handling of that return the connection broke down
                 */
                if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
                {
                    pTcpInputStream->State = OpcUa_TcpStream_State_HeaderStarted;
                    return OpcUa_GoodCallAgain;
                }
                #endif
                return uStatus;
            }

            /* Update stream markers. Current Position is the new end of data. */
            pTcpInputStream->Buffer.EndOfData   = pTcpInputStream->Buffer.EndOfData + nLength;
            pTcpInputStream->Buffer.Position    = pTcpInputStream->Buffer.EndOfData;

            /*OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_DataReady: %d bytes received\n", nLength);*/

            /* Header had not been fully received (last time). Try parsing now! */
            uStatus = OpcUa_TcpStream_CheckHeader(a_pIstrm);

            /* return errors and call again status */
            if(OpcUa_IsBad(uStatus) || OpcUa_IsEqual(OpcUa_GoodCallAgain))
            {
                return uStatus;
            }

            /* Header is complete now */
            pTcpInputStream->State = OpcUa_TcpStream_State_HeaderComplete;

            if (pTcpInputStream->MessageLength < pTcpInputStream->Buffer.Position)
            {
                uStatus = OpcUa_BadInvalidArgument; /* message is too small */
                OpcUa_GotoError;
            }

            /* Recalculate Length based on MessageLength, which must be known by now */
            nLength = pTcpInputStream->MessageLength - pTcpInputStream->Buffer.Position;

            /* no "break;" here, since after header is complete,    */
            /* the handling for message body can start immediately  */
        }
        /* fall thru */
    case OpcUa_TcpStream_State_HeaderComplete: /* Header was completed, currently message body */
        {
            /* security check for length exceeds buffersize (which would be an error) */
            if(nLength + pTcpInputStream->Buffer.Position > pTcpInputStream->BufferSize)
            {
               uStatus = OpcUa_BadInvalidArgument; /* message is too large */
               OpcUa_GotoError;
            }

            /* Read! (this might be a second read (actually in most cases) in one event) */
            /*OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_DataReady: (HeaderComplete) Trying to read %d bytes...\n", nLength);*/
            uStatus = OPCUA_P_SOCKET_READ(  pTcpInputStream->Socket,
                                            &(pTcpInputStream->Buffer.Data[pTcpInputStream->Buffer.Position]),
                                            nLength,
                                            &nLength);

            /* return errors and call again status */
            if(OpcUa_IsBad(uStatus))
            {
                if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
                {
                    /* in this case, only the first 8 Bytes have been received. */
                    /* the header was completed but no further data available.  */
                    /* should happen very rarely; must be tested. */
                    return OpcUa_GoodCallAgain;
                }
                else
                {
                    /* bad statuscode; connection closed */
                    return uStatus;
                }
            }

            /* Update OpcUa_Buffer markers. (directly without using buffer methods) Current Position is the new end of data. */
            pTcpInputStream->Buffer.EndOfData   = pTcpInputStream->Buffer.EndOfData + nLength;
            pTcpInputStream->Buffer.Position    = pTcpInputStream->Buffer.EndOfData;

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_TcpStream_DataReady: total %d bytes (%d last) of %d (w/o header) received.\n", pTcpInputStream->Buffer.Position, nLength, pTcpInputStream->MessageLength - OPCUA_TCPSTREAM_MESSAGEHEADER_LENGTH);

            /* Check if message is now complete and notify caller if needed. */
            if(pTcpInputStream->MessageLength <= pTcpInputStream->Buffer.EndOfData)
            {
                /* complete, set forth with OpcUa_TcpStream_State_MessageComplete */
                pTcpInputStream->State = OpcUa_TcpStream_State_MessageComplete;
            }
            else
            {
                /* if not, call again when more data is available */
                return OpcUa_GoodCallAgain;
            }
        }
        /* fall thru */
    case OpcUa_TcpStream_State_MessageComplete: /* just in case... */
        {
            /* The message has been completely received and dispatched to the upper layer. */
            /* This must be the next chunk. */
            pTcpInputStream->Buffer.Position = OPCUA_TCPSTREAM_MESSAGEHEADER_LENGTH;
            break;
        }
    default:
        {
            uStatus = OpcUa_BadInternalError;
            break;
        }
    } /* switch(pTcpInputStream->State) */

    /************************************************************************************/

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
