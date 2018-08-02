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

#ifdef OPCUA_HAVE_HTTPSAPI

#include <opcua_list.h>
#include <opcua_socket.h>
#include <opcua_utilities.h>
#include <opcua_https_internal.h>
#include <opcua_httpsstream.h>

#define OpcUa_HttpsOutputStream_SanityCheck 0x83514D85
#define OpcUa_HttpsInputStream_SanityCheck 0xB4155377

#if OPCUA_HTTPS_MAX_RECV_HEADER_LENGTH > OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH
# error Implementation prohibits a HTTP header size larger than receive buffer size.
#endif

/*============================================================================
 * OpcUa_HttpsStartLine
 *===========================================================================*/
/** @brief An union that contains either the request line or the status line */
union _OpcUa_HttpsStartLine
{
    OpcUa_HttpsRequestLine RequestLine;
    OpcUa_HttpsStatusLine  StatusLine;
};

typedef union _OpcUa_HttpsStartLine OpcUa_HttpsStartLine;

/*============================================================================
 * OpcUa_HttpsInputStream
 *===========================================================================*/
/** @brief Private data structure for an OpcUa_Stream that allows reading from
  * a socket.
  */
struct _OpcUa_HttpsInputStream
{
    /** @brief Inherited Fields from OpcUa_InputStream. @see OpcUa_InputStream */
    OpcUa_InputStream                       Base;

    /* Subclass Fields */
    /** @brief Check type of interface implementation. */
    OpcUa_UInt32                            SanityCheck;
    /** @brief Type of the message to be received. @see OpcUa_HttpsStream_MessageType */
    OpcUa_HttpsStream_MessageType           MessageType;
    /** @brief Type of the HTTP method used. @see OpcUa_HttpsStream_Method */
    OpcUa_HttpsStream_Method                eMethod;
    /** @brief The communication handle with which the message was received from. */
    OpcUa_Socket                            Socket;
    /** @brief Tells whether the stream is closed for reading. */
    OpcUa_Boolean                           Closed;
    /** @brief The current state of the stream. @see OpcUa_HttpsStream_State */
    OpcUa_HttpsStream_State                 State;
    /** @brief Message start line. */
    OpcUa_HttpsStartLine                    StartLine;
    /** @brief Message headers. */
    OpcUa_HttpsHeaderCollection*            Headers;
    /** @brief The size of the message body. -1 if chunked. */
    OpcUa_Int32                             iContentLength;
    /** @brief The recent message line. */
    OpcUa_String                            MessageLine;
    /** @brief The current count of buffers used in the stream. nBuffers is 1-based, index is 0 based. Last buffer is adressed by nBuffers-1 ( = nCurrentReadBuffer) */
    OpcUa_UInt32                            nBuffers;
    /** @brief The absolute position spanning all included buffers. Returned in GetPosition. */
    OpcUa_UInt32                            nAbsolutePosition;
    /** @brief Current chunk length. */
    OpcUa_Int32                             iCurrentChunkLength;
    /** @brief Current chunk data received. */
    OpcUa_Int32                             iCurrentChunkDataReceived;
    /** @brief Number of characters till next chunk border. */
    OpcUa_UInt32                            nCurrentChunkCharactersLeft;
    /** @brief The index of the currently used buffer for reading. */
    OpcUa_UInt32                            nCurrentReadBuffer;
    /** @brief The internal message buffers. */
    OpcUa_Buffer                            Buffer[OPCUA_HTTPS_MAX_RECV_BUFFER_COUNT];

};
typedef struct _OpcUa_HttpsInputStream OpcUa_HttpsInputStream;

/*============================================================================
 * OpcUa_HttpsOutputStream
 *===========================================================================*/
/** @brief Private data structure for an OpcUa_Stream that allows writing to
  * a socket.
  */
struct _OpcUa_HttpsOutputStream
{
    /** @brief Inherited Fields from OpcUa_OutputStream. @see OpcUa_OutputStream */
    OpcUa_OutputStream                      Base;

    /* Subclass (Http) Fields */
    /** @brief Check type of interface implementation. */
    OpcUa_UInt32                            SanityCheck;
    /** @brief Type of the message to be sent. @see OpcUa_HttpsStream_MessageType */
    OpcUa_HttpsStream_MessageType           MessageType;
    /** @brief The communication handle with which the message is sent. */
    OpcUa_Socket                            Socket;
    /** @brief Tells whether the stream is closed for reading. */
    OpcUa_Boolean                           Closed;
    /** @brief Handle of the underlying connection. */
    OpcUa_Void*                             hConnection;
    /** @brief Message start line. */
    OpcUa_HttpsStartLine                    StartLine;
    /** @brief HTTP method. */
    OpcUa_HttpsStream_Method                Method;
#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION
    /** @brief Message headers. */
    OpcUa_HttpsHeaderCollection*            Headers;
#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */
    OpcUa_Buffer                            HeaderBuffer;
#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */
    /** @brief Counts the number of chunks already send. (index of next buffer to send) */
    OpcUa_UInt32                            nChunksSend;
    /** @brief The current count of buffers used in the stream. nBuffers is 1-based, index is 0 based. Last buffer is adressed by nBuffers-1 ( = nCurrentReadBuffer) */
    OpcUa_UInt32                            nBuffers;
    /** @brief The absolute position spanning all included buffers. Returned in GetPosition. */
    OpcUa_UInt32                            nAbsolutePosition;
    /** @brief The internal message buffers. */
    OpcUa_Buffer                            Buffer[OPCUA_HTTPS_MAX_SEND_CHUNK_COUNT];
};
typedef struct _OpcUa_HttpsOutputStream OpcUa_HttpsOutputStream;


/*============================================================================
 * OpcUa_ReturnErrorIfInvalidStream
 *===========================================================================*/
/** @brief check instance */
#define OpcUa_ReturnErrorIfInvalidStream(xStream, xMethod) \
if (    (    (((OpcUa_HttpsInputStream*)(xStream->Handle))->SanityCheck != OpcUa_HttpsInputStream_SanityCheck) \
          && (((OpcUa_HttpsOutputStream*)(xStream->Handle))->SanityCheck != OpcUa_HttpsOutputStream_SanityCheck)) \
     || (xStream->xMethod != OpcUa_HttpsStream_##xMethod)) \
{ \
    return OpcUa_BadInvalidArgument; \
}

/*============================================================================
 * OpcUa_HttpsStream_Flush Prototype
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_Flush(   OpcUa_OutputStream* pOutputStream,
                                            OpcUa_Boolean       bLastCall);

/*============================================================================
 * OpcUa_HttpsStream_ReadLine Prototype
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsStream_ReadLine( OpcUa_InputStream*  a_pInputStream,
                                                    OpcUa_String*       a_pMessageLine);

/*============================================================================
 * OpcUa_HttpsStream_ReadChunkLength Prototype
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsStream_ReadChunkLength(  OpcUa_InputStream*  a_pInputStream,
                                                            OpcUa_Int32*        a_piChunkLength);

/*============================================================================
 * OpcUa_Https_ReadCharFromBuffer
 *===========================================================================*/
/** @brief reads the next available character */
static OpcUa_StatusCode OpcUa_Https_ReadCharFromBuffer(
    OpcUa_Buffer*   a_pBuffer,
    OpcUa_CharA*    a_pValue)
{
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);

    if(a_pBuffer->EndOfData - a_pBuffer->Position <= 0)
    {
        return OpcUa_BadEndOfStream;
    }

    *a_pValue = (OpcUa_CharA)(a_pBuffer->Data[a_pBuffer->Position++]);

    return OpcUa_Good;
}

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION
/*============================================================================
 * OpcUa_HttpsStartLine_Serialize
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsStartLine_Serialize(
    OpcUa_HttpsStartLine*           a_pStartLine,
    OpcUa_HttpsStream_MessageType   a_eMessageType,
    OpcUa_Buffer*                   a_pBuffer)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "OpcUa_HttpsStartLine_Serialize");

    OpcUa_ReturnErrorIfArgumentNull(a_pStartLine);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    switch(a_eMessageType)
    {
        case OpcUa_HttpsStream_MessageType_Request:
        {
            uStatus = OpcUa_HttpsRequestLine_Serialize(&a_pStartLine->RequestLine, a_pBuffer);
            break;
        }

        case OpcUa_HttpsStream_MessageType_Response:
        {
            uStatus = OpcUa_HttpsStatusLine_Serialize(&a_pStartLine->StatusLine, a_pBuffer);
            break;
        }

        default:
        {
            uStatus = OpcUa_BadInvalidArgument;
            break;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
#endif

/*============================================================================
 * OpcUa_HttpsStream_DetachBuffer
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_DetachBuffer(
    OpcUa_Stream*   a_pStream,
    OpcUa_Buffer*   a_pBuffer)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "DetachBuffer");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    switch(a_pStream->Type)
    {
        case OpcUa_StreamType_Output:
        {
            OpcUa_HttpsOutputStream*    pHttpOutputStream   = (OpcUa_HttpsOutputStream*)a_pStream->Handle;
            OpcUa_UInt32                uBuffer             = 0;

            /* HINT: nBuffers contains the highest valid index */

            while(      (uBuffer <= pHttpOutputStream->nBuffers)
                    &&  (OpcUa_Buffer_IsEmpty(&pHttpOutputStream->Buffer[uBuffer])))
            {
                uBuffer++;
            }

            if(     (uBuffer <= pHttpOutputStream->nBuffers)
                 && (pHttpOutputStream->Buffer[uBuffer].Data != OpcUa_Null))
            {
                *a_pBuffer = pHttpOutputStream->Buffer[uBuffer];
                pHttpOutputStream->Buffer[uBuffer].Data = OpcUa_Null;
                OpcUa_Buffer_SetEmpty(&pHttpOutputStream->Buffer[uBuffer]);
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "bad no data\n");
                OpcUa_GotoErrorWithStatus(OpcUa_BadNoData);
            }

            break;
        }
        case OpcUa_StreamType_Input:
        {
            OpcUa_HttpsInputStream* pHttpInputStream = (OpcUa_HttpsInputStream*)a_pStream->Handle;

#if !OPCUA_HTTPS_COPYHEADERS
            /* clear references to buffer content */
            OpcUa_HttpsHeaderCollection_Delete(&pHttpInputStream->Headers);

            if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
            {
                OpcUa_HttpsRequestLine_Clear(&pHttpInputStream->StartLine.RequestLine);
            }

            else if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Response)
            {
                OpcUa_HttpsStatusLine_Clear(&pHttpInputStream->StartLine.StatusLine);
            }
#endif

            *a_pBuffer = pHttpInputStream->Buffer[0];
            pHttpInputStream->Buffer[0].Data = OpcUa_Null;
            OpcUa_Buffer_Clear(&pHttpInputStream->Buffer[0]);

            pHttpInputStream->State = OpcUa_HttpsStream_State_Empty;
            pHttpInputStream->Base.Close((OpcUa_Stream*)pHttpInputStream);

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
 * OpcUa_HttpsStream_AttachBuffer
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_AttachBuffer(
    OpcUa_Stream*   a_pStream,
    OpcUa_Buffer*   a_pBuffer)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "AttachBuffer");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);

    switch(a_pStream->Type)
    {
        case OpcUa_StreamType_Output:
        {
            OpcUa_HttpsOutputStream* pHttpOutputStream   = (OpcUa_HttpsOutputStream*)a_pStream->Handle;
            OpcUa_Buffer             OldBuffer           = pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers];

            pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers] = *a_pBuffer;

            /* create same state as data would have been written into the stream */
            uStatus = OpcUa_Buffer_SetPosition(&pHttpOutputStream->Buffer, OpcUa_BufferPosition_End);
            if(OpcUa_IsBad(uStatus))
            {
                /* restore old buffer */
                pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers] = OldBuffer;
            }

            a_pBuffer->Data = OpcUa_Null;
            OpcUa_Buffer_Clear(a_pBuffer);

            break;
        }
        case OpcUa_StreamType_Input:
        {
            OpcUa_HttpsInputStream* pHttpInputStream = (OpcUa_HttpsInputStream*)a_pStream->Handle;

            OpcUa_ReferenceParameter(pHttpInputStream);

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
 * OpcUa_HttpsStream_GetChunkLength
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetChunkLength(
    OpcUa_Stream* a_pStream,
    OpcUa_UInt32* a_puLength)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetChunkLength");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_puLength);

    /* http messages are regarded as indivisible */
    *a_puLength = 0;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_Read
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_Read(
    OpcUa_InputStream*             a_pInputStream,  /* Stream with HttpStream handle */
    OpcUa_Byte*                    a_pTargetBuffer, /* The destination buffer. */
    OpcUa_UInt32*                  a_puCount)       /* How many bytes should be delivered. */
{
    OpcUa_HttpsInputStream* pHttpInputStream    = OpcUa_Null;
    OpcUa_UInt32            uBytesAvailable     = 0;
    OpcUa_UInt32            uBytesRequested     = 0;
    OpcUa_Boolean           bReadAgain          = OpcUa_True;
    OpcUa_Byte*             pTargetBuffer       = OpcUa_Null;
    OpcUa_String            sMessageLine        = OPCUA_STRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "Read");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pTargetBuffer);
    OpcUa_ReturnErrorIfArgumentNull(a_puCount);
    OpcUa_ReturnErrorIfInvalidStream(a_pInputStream, Read);

    uBytesRequested  = *a_puCount;
    pTargetBuffer    = a_pTargetBuffer;

    pHttpInputStream = (OpcUa_HttpsInputStream*)a_pInputStream->Handle;

    do
    {
        /* check and skip chunk border */
        if(pHttpInputStream->nCurrentChunkCharactersLeft == 0)
        {
            if(pHttpInputStream->iContentLength < 0)
            {
                do
                {
                    uStatus = OpcUa_HttpsStream_ReadLine(a_pInputStream, &sMessageLine);
                    OpcUa_GotoErrorIfBad(uStatus);

                    if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
                    {
                        OpcUa_GotoErrorWithStatus(OpcUa_BadEndOfStream);
                    }
                } while(!OpcUa_String_IsEmpty(&sMessageLine));

                uStatus = OpcUa_HttpsStream_ReadChunkLength(a_pInputStream,
                                                            &pHttpInputStream->iCurrentChunkLength);
                OpcUa_GotoErrorIfBad(uStatus);

                if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadEndOfStream);
                }

                /* substract the number of chunk delimiters */
                pHttpInputStream->nCurrentChunkCharactersLeft = (OpcUa_UInt32)pHttpInputStream->iCurrentChunkLength;
            }
            else
            {
                pHttpInputStream->nCurrentChunkCharactersLeft = pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData - pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].Position;
            }
        }

        /* set available amount of data as maximum */
        uBytesAvailable = pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData - pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].Position;

        /* check if number of bytes till chunk end is less */
        if(pHttpInputStream->nCurrentChunkCharactersLeft < uBytesAvailable)
        {
            uBytesAvailable = pHttpInputStream->nCurrentChunkCharactersLeft;
        }

        if(uBytesAvailable >= uBytesRequested)
        {
            /* ok - all or more of the bytes requested are in one buffer */
            uBytesAvailable = uBytesRequested;
        }
        else
        {
            /* not all can be delivered */
            if(uBytesAvailable == pHttpInputStream->nCurrentChunkCharactersLeft)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Read: end of chunk reached: %u requested, %u available in chunk; need to skip)!\n", uBytesRequested, uBytesAvailable);
            }
            else
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Read: end of buffer reached: %u requested, %u available in buffer; need to skip)!\n", uBytesRequested, uBytesAvailable);
            }
        }

        if(uBytesAvailable > 0)
        {
            /* read requested data */
            uStatus = OpcUa_Buffer_Read( &pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer],
                                         pTargetBuffer,
                                         &uBytesAvailable);
            OpcUa_GotoErrorIfBad(uStatus);

            /* prepare for next read */
            uBytesRequested -= uBytesAvailable;

            /* adjust number of bytes left in chunk */
            pHttpInputStream->nCurrentChunkCharactersLeft -= uBytesAvailable;
        }

        if(uBytesRequested == 0)
        {
            bReadAgain = OpcUa_False;
        }
        else
        {
            if(pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData == pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].Position)
            {
                if(pHttpInputStream->nCurrentReadBuffer < pHttpInputStream->nBuffers)
                {
                    /* skip to next buffer */
                    pHttpInputStream->nCurrentReadBuffer++;
                    pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].Position = 0;
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Read: Buffer skip to index %u of %u!\n", pHttpInputStream->nCurrentReadBuffer, pHttpInputStream->nBuffers);
                }
                else
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsStream_Read: End of Stream reached! Buffer %u of %u!\n", pHttpInputStream->nCurrentReadBuffer, pHttpInputStream->nBuffers);
                    uStatus = OpcUa_BadEndOfStream;
                    bReadAgain = OpcUa_False;
                }
            }

            pTargetBuffer += uBytesAvailable;
        }
    } while(bReadAgain != OpcUa_False);

    if(uBytesRequested > 0)
    {
        uStatus = OpcUa_BadEndOfStream;
    }

    pHttpInputStream->nAbsolutePosition += *a_puCount - uBytesRequested;

    *a_puCount -= uBytesRequested;

    OpcUa_String_Clear(&sMessageLine);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_String_Clear(&sMessageLine);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_Write
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_Write(
    OpcUa_OutputStream* a_pOutputStream,    /* the stream to write the value into */
    OpcUa_Byte*         a_pBuffer,          /* the value to write */
    OpcUa_UInt32        a_uBufferSize)      /* the size of the value to write */
{
    OpcUa_HttpsOutputStream*    pHttpOutputStream   = OpcUa_Null;
    OpcUa_UInt32                uMaxCount           = 0;
    OpcUa_UInt32                uDataLeft           = 0;
    OpcUa_UInt32                uDataWritten        = 0;
    OpcUa_Byte*                 pWriteStart         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "Write");

    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    pHttpOutputStream = (OpcUa_HttpsOutputStream*)a_pOutputStream->Handle;

    OpcUa_ReturnErrorIfInvalidStream(a_pOutputStream, Write);

    if(pHttpOutputStream->Closed)
    {
        return OpcUa_BadInvalidState;
    }

    /* do the writing */
    uMaxCount   = pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers].Size - pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers].Position;
    uDataLeft   = a_uBufferSize;
    pWriteStart = a_pBuffer;

    /* check if bufferlimit will be hit */
    if(uMaxCount < a_uBufferSize)
    {
        /* we need at least 1 flush */
        while(uMaxCount < uDataLeft)
        {
            OpcUa_Byte* pData = OpcUa_Null;

            /* write as much as possible into the stream */
            uStatus = OpcUa_Buffer_Write(&pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers], pWriteStart, uMaxCount);
            OpcUa_GotoErrorIfBad(uStatus);

            uDataWritten += uMaxCount;

            if(pHttpOutputStream->nBuffers == (OPCUA_HTTPS_MAX_SEND_CHUNK_COUNT - 1))
            {
                /* max buffers already reached */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsStream_Write: Message too long!\n");
                OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
            }

            /* skip onto the next buffer */
            pHttpOutputStream->nBuffers++;

            pData = OpcUa_Alloc(OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH);
            OpcUa_GotoErrorIfAllocFailed(pData);

            /* create an internal buffer. */
            uStatus = OpcUa_Buffer_Initialize(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]),      /* instance        */
                                              pData,                                /* bufferdata      */
                                              0,                                    /* datasize        */
                                              OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH,   /* blocksize       */
                                              OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH,   /* maxsize         */
                                              OpcUa_True);                          /* should be freed */
            if(OpcUa_IsBad(uStatus))
            {
                OpcUa_Free(pData);
                OpcUa_GotoError;
            }

            /* reserve header space */
            OpcUa_Buffer_SetEndOfData(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]), OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);
            OpcUa_Buffer_SetPosition(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]), OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);

            /* reserve footer space */
            pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers].Size -= OPCUA_HTTPS_MAX_SEND_FOOTER_LENGTH;

            /* stream is now reset */
            /* recalculate state */
            uDataLeft   = a_uBufferSize - uDataWritten;  /* amount of data left */
            pWriteStart = &a_pBuffer[uDataWritten]; /* begin of data left */
            uMaxCount   = pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers].Size - pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers].Position;
        }
    }
    else
    {
        uDataLeft = a_uBufferSize;  /* amount of data to write */
        pWriteStart = a_pBuffer;    /* begin of data to write */
    }

    uStatus = OpcUa_Buffer_Write(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]), pWriteStart, uDataLeft);
    OpcUa_ReturnErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_SendChunk
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_SendChunk(   OpcUa_HttpsOutputStream*    a_pHttpOutputStream,
                                                OpcUa_Buffer*               a_pChunkBuffer)
{
    OpcUa_Byte*  pMessageData   = OpcUa_Null;
    OpcUa_UInt32 uMessageStart  = 0;
    OpcUa_UInt32 uMessageLength = 0;
    OpcUa_Int32  iBytesWritten  = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "SendChunk");

    /* get message data */
    uStatus = OpcUa_Buffer_GetData(a_pChunkBuffer, &pMessageData, &uMessageLength);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_GetPosition(a_pChunkBuffer, &uMessageStart);
    OpcUa_GotoErrorIfBad(uStatus);

    uMessageLength -= uMessageStart;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_SendChunk: Sending block of %d bytes!\n", uMessageLength);

    /* send to network */
    iBytesWritten = OPCUA_P_SOCKET_WRITE(a_pHttpOutputStream->Socket,
                                         &pMessageData[uMessageStart],
                                         uMessageLength,
#if OPCUA_HTTPSSTREAM_BLOCKINGWRITE
                                         OpcUa_True);
#else /* OPCUA_HTTPSSTREAM_BLOCKINGWRITE */
                                         OpcUa_False);
#endif /* OPCUA_HTTPSSTREAM_BLOCKINGWRITE */

    if(iBytesWritten < (OpcUa_Int32)uMessageLength)
    {
        if(iBytesWritten < (OpcUa_Int32)0)
        {
            uStatus = OPCUA_P_SOCKET_GETLASTERROR(a_pHttpOutputStream->Socket);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_SendChunk: Error writing to socket: 0x%08X!\n", uStatus);

            OpcUa_GotoErrorWithStatus(OpcUa_BadDisconnect);
        }
        else
        {
            /* keep as outgoing stream */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_SendChunk: Only %u bytes of %u written!\n", iBytesWritten, uMessageLength);

            /* store position */
            OpcUa_Buffer_SetPosition(a_pChunkBuffer, uMessageStart + (OpcUa_UInt32)iBytesWritten);

            OpcUa_GotoErrorWithStatus(OpcUa_BadWouldBlock);
        }
    }

    /* empty an output buffer */
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_SendChunk: Buffer emptied!\n");
    OpcUa_Buffer_SetEmpty(a_pChunkBuffer);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_SerializeHeaders
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_SerializeHeaders(OpcUa_HttpsOutputStream*    a_pHttpOutputStream,
                                                    OpcUa_Buffer*               a_pChunkBuffer)
{
#if !OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION
    OpcUa_Byte*     pData   = OpcUa_Null;
    OpcUa_UInt32    uLength = 0;
#endif

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "SerializeHeaders");

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

    /* place the message start line into output buffer */
    uStatus = OpcUa_HttpsStartLine_Serialize(   &a_pHttpOutputStream->StartLine,
                                                a_pHttpOutputStream->MessageType,
                                                a_pChunkBuffer);
    OpcUa_GotoErrorIfBad(uStatus);

    /* place the message headers into output buffer */
    uStatus = OpcUa_HttpsHeaderCollection_Serialize(a_pHttpOutputStream->Headers,
                                                    a_pChunkBuffer);
    OpcUa_GotoErrorIfBad(uStatus);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

    uStatus = OpcUa_Buffer_GetData( &a_pHttpOutputStream->HeaderBuffer,
                                    &pData,
                                    &uLength);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   a_pChunkBuffer,
                                    pData,
                                    uLength);
    OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_SerializeChunkHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_SerializeChunkHeader(OpcUa_Buffer*   a_pChunkBuffer,
                                                        OpcUa_UInt32    a_uChunkLength,
                                                        OpcUa_Boolean   a_bPrepend)
{
    OpcUa_CharA achChunkLength[13];

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "SerializeChunkHeader");

    /* format content-length value */
    OpcUa_SPrintfA(achChunkLength,
#if OPCUA_USE_SAFE_FUNCTIONS
                   sizeof(achChunkLength)/sizeof(achChunkLength[0]),
#endif
                   "%X\r\n",
                   (unsigned int)a_uChunkLength);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_SerializeChunkHeader: Chunk length is %u.\n", a_uChunkLength);

    if(a_bPrepend == OpcUa_False)
    {
        uStatus = OpcUa_Buffer_Write(a_pChunkBuffer, (OpcUa_Byte*)&achChunkLength, OpcUa_StrLenA((OpcUa_StringA)&achChunkLength));
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        OpcUa_UInt32 uPosition      = 0;
        OpcUa_UInt32 uHeaderLength  = OpcUa_StrLenA((OpcUa_StringA)&achChunkLength);

        /* get begin of data */
        uStatus = OpcUa_Buffer_GetPosition( a_pChunkBuffer,
                                           &uPosition);
        OpcUa_GotoErrorIfBad(uStatus);

        /* set to header start position */
        uStatus = OpcUa_Buffer_SetPosition( a_pChunkBuffer,
                                            uPosition - uHeaderLength);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_Buffer_Write(a_pChunkBuffer, (OpcUa_Byte*)&achChunkLength, OpcUa_StrLenA(((OpcUa_StringA)&achChunkLength)));
        OpcUa_GotoErrorIfBad(uStatus);

        /* reset position to begin of data */
        uStatus = OpcUa_Buffer_SetPosition( a_pChunkBuffer,
                                            uPosition - uHeaderLength);
        OpcUa_GotoErrorIfBad(uStatus);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_PrepareChunk
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_PrepareChunk(OpcUa_HttpsOutputStream*    a_pHttpOutputStream,
                                                OpcUa_Buffer*               a_pChunkBuffer,
                                                OpcUa_Boolean               a_bChunkedEncoding,
                                                OpcUa_Boolean               a_bFirstChunk,
                                                OpcUa_Boolean               a_bLastChunk)
{
    OpcUa_UInt32 uHeaderStart = 0;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "PrepareChunk");

    if(!OpcUa_Buffer_IsEmpty(a_pChunkBuffer))
    {
        OpcUa_Byte*     pFooterData     = OpcUa_Null;
        OpcUa_UInt32    uFooterLength   = 0;
        OpcUa_UInt32    uBodyEnd        = 0;
        OpcUa_UInt32    uBodyLength     = 0;

        /* release reserved footer space */
        a_pChunkBuffer->Size += OPCUA_HTTPS_MAX_SEND_FOOTER_LENGTH;

        if(a_bChunkedEncoding != OpcUa_False)
        {
            if(a_bLastChunk == OpcUa_False)
            {
                pFooterData     = (OpcUa_Byte*)"\r\n";
                uFooterLength   = 2;
            }
            else
            {
                pFooterData     = (OpcUa_Byte*)"\r\n0\r\n\r\n";
                uFooterLength   = 7;
            }
        }
        else
        {
            pFooterData     = OpcUa_Null;
            uFooterLength   = 0;
        }

        OpcUa_Buffer_GetPosition(a_pChunkBuffer, &uBodyEnd);

        uBodyLength = uBodyEnd - OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH;

        if(a_bFirstChunk != OpcUa_False)
        {
            OpcUa_UInt32    uHeaderLength   = 0;
            OpcUa_Byte*     pHeaderData     = OpcUa_Null;
            OpcUa_Buffer*   pHeaderBuffer   = OpcUa_Null;

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

            OpcUa_Buffer    HeaderBuffer;
            pHeaderData     = OpcUa_Alloc(OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);
            OpcUa_GotoErrorIfAllocFailed(pHeaderData);

            OpcUa_Buffer_Initialize(   &HeaderBuffer,
                                        pHeaderData,
                                        0,
                                        OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH,
                                        OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH,
                                        OpcUa_True);

            /* serialize headers into temp buffer */
            uStatus = OpcUa_HttpsStream_SerializeHeaders(a_pHttpOutputStream, &HeaderBuffer);

            pHeaderBuffer = &HeaderBuffer;

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

            pHeaderBuffer = &a_pHttpOutputStream->HeaderBuffer;

            uStatus = OpcUa_Buffer_Write(pHeaderBuffer, (OpcUa_Byte*)"\r\n", 2);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_Buffer_GetData(pHeaderBuffer, &pHeaderData, OpcUa_Null);
            OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

            if(OpcUa_IsGood(uStatus))
            {
                if(a_bChunkedEncoding != OpcUa_False)
                {
                    uStatus = OpcUa_HttpsStream_SerializeChunkHeader(pHeaderBuffer, uBodyLength, OpcUa_False);
                }

                if(OpcUa_IsGood(uStatus))
                {
                    OpcUa_Buffer_GetPosition(pHeaderBuffer, &uHeaderLength);
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_PrepareChunk: HTTP header length is %u.\n", uHeaderLength);

                    uHeaderStart = OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH - uHeaderLength;

                    /* prepend header to body */
                    OpcUa_Buffer_SetPosition(a_pChunkBuffer, uHeaderStart);

                    uStatus = OpcUa_Buffer_Write( a_pChunkBuffer, pHeaderData, uHeaderLength);
                }
            }

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION
            OpcUa_Buffer_Clear(&HeaderBuffer);
#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

            OpcUa_GotoErrorIfBad(uStatus);
        }
        else
        {
            if(a_bChunkedEncoding != OpcUa_False)
            {
                /* set write position to begin of body */
                OpcUa_Buffer_SetPosition(a_pChunkBuffer, OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);

                OpcUa_HttpsStream_SerializeChunkHeader(a_pChunkBuffer, uBodyLength, OpcUa_True);
                OpcUa_GotoErrorIfBad(uStatus);

                OpcUa_Buffer_GetPosition(a_pChunkBuffer, &uHeaderStart);
            }
        }

        /* store the footer */
        if(pFooterData != OpcUa_Null)
        {
            /* set write position to end of body */
            OpcUa_Buffer_SetPosition(a_pChunkBuffer, uBodyEnd);

            /* write footer data into stream */
            uStatus = OpcUa_Buffer_Write(a_pChunkBuffer, pFooterData, uFooterLength);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        /* set position to begin of data */
        OpcUa_Buffer_SetPosition(a_pChunkBuffer, uHeaderStart);
    }
    else /* chunk is empty */
    {
        uStatus = OpcUa_HttpsStream_SerializeHeaders(a_pHttpOutputStream, a_pChunkBuffer);
        OpcUa_GotoErrorIfBad(uStatus);

        /* set position to begin of data */
        OpcUa_Buffer_SetPosition(a_pChunkBuffer, OpcUa_BufferPosition_Start);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_Flush
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_Flush(   OpcUa_OutputStream* a_pOutputStream,
                                            OpcUa_Boolean       a_bLastCall)
{
    OpcUa_HttpsOutputStream*    pHttpOutputStream   = OpcUa_Null;
    OpcUa_UInt32                uCurrentBuffer      = 0;
    OpcUa_Boolean               bWouldBlock         = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "Flush");

    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream->Handle);
    OpcUa_ReturnErrorIfInvalidStream(a_pOutputStream, Flush);

    /* Current implementation allows only one flush after all data has been serialized. */
    OpcUa_ReturnErrorIfTrue((a_bLastCall == OpcUa_False), OpcUa_BadInvalidArgument);

    pHttpOutputStream = (OpcUa_HttpsOutputStream*)a_pOutputStream->Handle;
    OpcUa_GotoErrorIfTrue((pHttpOutputStream->Closed), OpcUa_BadInvalidState);

    /* check whether the message body is allowed */
    if(pHttpOutputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
    {
        if(OpcUa_String_StrnCmp(&(pHttpOutputStream->StartLine.RequestLine.RequestMethod),
                                OpcUa_String_FromCString("HEAD"),
                                OPCUA_STRING_LENDONTCARE,
                                OpcUa_False) == 0)
        {
            return OpcUa_BadInvalidState;
        }
    }

    /* more state checks */
    if(pHttpOutputStream->MessageType == OpcUa_HttpsStream_MessageType_Response)
    {
        if((pHttpOutputStream->StartLine.StatusLine.StatusCode >= 100) &&
           (pHttpOutputStream->StartLine.StatusLine.StatusCode <  200))
        {
            return OpcUa_BadInvalidState;
        }

        if((pHttpOutputStream->StartLine.StatusLine.StatusCode == 204) ||
           (pHttpOutputStream->StartLine.StatusLine.StatusCode == 304))
        {
            return OpcUa_BadInvalidState;
        }
    }

    /* prepare message headers */
    if(!OpcUa_Buffer_IsEmpty(&pHttpOutputStream->Buffer[0]))
    {
        /* check if message consists of more than one chunk */
#if OPCUA_HTTPSTREAM_AUTOMATIC_CHUNKING
        if(pHttpOutputStream->nBuffers == 0)
        {
            /* only one chunk required -> use "Content-Length" header. */
            OpcUa_Byte*  pContentData   = OpcUa_Null;
            OpcUa_UInt32 uContentLength = 0;
            OpcUa_CharA chContentLength[20];

            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Flush: Message consists of 1 chunk.\n");

            /* get content-length value */
            uStatus = OpcUa_Buffer_GetData(&pHttpOutputStream->Buffer[0], &pContentData, &uContentLength);
            OpcUa_GotoErrorIfBad(uStatus);

            uContentLength -= OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH;

            /* format content-length value */
            OpcUa_SPrintfA(chContentLength,
#if OPCUA_USE_SAFE_FUNCTIONS
                           sizeof(chContentLength)/sizeof(chContentLength[0]),
#endif
                           "%u",
                           (unsigned int)uContentLength);

            /* update the "Content-Length" header */
#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

            uStatus = OpcUa_HttpsHeaderCollection_SetValue(pHttpOutputStream->Headers,
                                                           OpcUa_String_FromCString("Content-Length"),
                                                           OpcUa_String_FromCString(chContentLength));
            OpcUa_GotoErrorIfBad(uStatus);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

            uStatus = OpcUa_Buffer_Write(   &pHttpOutputStream->HeaderBuffer,
                                            (OpcUa_Byte*)"Content-Length: ",
                                            16);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_Buffer_Write(   &pHttpOutputStream->HeaderBuffer,
                                            (OpcUa_Byte*)chContentLength,
                                            OpcUa_StrLenA(chContentLength));
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_Buffer_Write(   &pHttpOutputStream->HeaderBuffer,
                                            (OpcUa_Byte*)"\r\n",
                                            2);
            OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */
        }
        else
#endif /* OPCUA_HTTPSTREAM_AUTOMATIC_CHUNKING */
        {
            /* Message consists of multiple chunks -> use "Transfer-Encoding: chunked" header. */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Flush: Message consists of %u chunks.\n", pHttpOutputStream->nBuffers + 1);

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

            /* update the "Content-Length" header */
            uStatus = OpcUa_HttpsHeaderCollection_SetValue(pHttpOutputStream->Headers,
                                                           OpcUa_String_FromCString("Transfer-Encoding"),
                                                           OpcUa_String_FromCString("chunked"));
            OpcUa_GotoErrorIfBad(uStatus);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

            uStatus = OpcUa_Buffer_Write(   &pHttpOutputStream->HeaderBuffer,
                                            (OpcUa_Byte*)"Transfer-Encoding: chunked\r\n",
                                            28);
            OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */
        }
    } /* if buffer is not empty */


    /* process all buffers */
    for(uCurrentBuffer = 0; uCurrentBuffer <= pHttpOutputStream->nBuffers; uCurrentBuffer++)
    {
        /* prepare data in chunk buffer */
        uStatus = OpcUa_HttpsStream_PrepareChunk(   pHttpOutputStream,
                                                    &pHttpOutputStream->Buffer[uCurrentBuffer],
#if OPCUA_HTTPSTREAM_AUTOMATIC_CHUNKING
                                                    (pHttpOutputStream->nBuffers > 0)?OpcUa_True:OpcUa_False,
#else /* OPCUA_HTTPSTREAM_AUTOMATIC_CHUNKING */
                                                    OpcUa_True,
#endif /* OPCUA_HTTPSTREAM_AUTOMATIC_CHUNKING */
                                                    (uCurrentBuffer)?OpcUa_False:OpcUa_True,
                                                    (uCurrentBuffer == pHttpOutputStream->nBuffers)?OpcUa_True:OpcUa_False);
        OpcUa_GotoErrorIfBad(uStatus);

        /* write data to socket */
        if(bWouldBlock == OpcUa_False)
        {
            if(!OpcUa_Buffer_IsEmpty(&pHttpOutputStream->Buffer[uCurrentBuffer]))
            {
                uStatus = OpcUa_HttpsStream_SendChunk(   pHttpOutputStream,
                                                        &pHttpOutputStream->Buffer[uCurrentBuffer]);

                if(OpcUa_IsBad(uStatus))
                {
                    if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
                    {
                        bWouldBlock = OpcUa_True;
                    }
                    else
                    {
                        OpcUa_GotoErrorIfBad(uStatus);
                    }
                }
                else
                {
                    pHttpOutputStream->nChunksSend++;
                }
            } /* send if not empty */
        }
        else
        {
            pHttpOutputStream->nChunksSend++;
        }
    } /* for each chunk */

    if(bWouldBlock != OpcUa_False)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadWouldBlock);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Flush: All chunks send successfully!\n");
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if !OPCUA_HTTPSSTREAM_BLOCKINGWRITE
    if(OpcUa_IsNotEqual(OpcUa_BadWouldBlock))
    {
#endif
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_HttpsStream_Flush: could not flush stream. (0x%08X)\n", uStatus);
#if !OPCUA_HTTPSSTREAM_BLOCKINGWRITE
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Flush: send would block\n");
    }
#endif

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_Close(OpcUa_Stream* a_pStream)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfInvalidStream(a_pStream, Close);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
        if(((OpcUa_HttpsOutputStream*)(a_pStream->Handle))->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        uStatus = OpcUa_HttpsStream_Flush((OpcUa_OutputStream*)a_pStream, OpcUa_True);

        ((OpcUa_HttpsOutputStream*)(a_pStream->Handle))->Closed = OpcUa_True;
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        if(((OpcUa_HttpsInputStream*)(a_pStream->Handle))->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        /* TODO: closing a stream before the end of the message could screw things up.
           Need to read rest of message from stream before closing. Thats complex,
           because the rest of the message may be delayed, so we would have to block here,
           what we don't want. Handle this stream like before, but mark it as abandoned!
           If the stream is complete, it will not be handled but deleted immediately.
           Intermediary read events are not further processed. */

        ((OpcUa_HttpsInputStream*)(a_pStream->Handle))->Closed = OpcUa_True;
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
 * OpcUa_HttpsStream_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_HttpsStream_Delete(OpcUa_Stream** a_ppStream)
{
    if(a_ppStream == OpcUa_Null)
    {
        /* error condition - should not happen */
        return;
    }

    if((*a_ppStream) == OpcUa_Null)
    {
        /* error condition - should not happen */
        return;
    }

    if((*a_ppStream)->Type == OpcUa_StreamType_Output)
    {
        OpcUa_UInt32                uCurrentBuffer  = 0;
        OpcUa_HttpsOutputStream*    pOutputStream   = (OpcUa_HttpsOutputStream*)((*a_ppStream)->Handle);

        for(uCurrentBuffer = 0; uCurrentBuffer <= pOutputStream->nBuffers; uCurrentBuffer++)
        {
            OpcUa_Buffer_Clear(&(pOutputStream->Buffer[uCurrentBuffer]));
        }

        if(pOutputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
        {
            OpcUa_HttpsRequestLine_Clear(&pOutputStream->StartLine.RequestLine);
        }

        else if(pOutputStream->MessageType == OpcUa_HttpsStream_MessageType_Response)
        {
            OpcUa_HttpsStatusLine_Clear(&pOutputStream->StartLine.StatusLine);
        }

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

        OpcUa_HttpsHeaderCollection_Delete(&pOutputStream->Headers);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

        OpcUa_Buffer_Clear(&pOutputStream->HeaderBuffer);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

        OpcUa_Free(*a_ppStream);
        *a_ppStream = OpcUa_Null;
    }
    else if((*a_ppStream)->Type == OpcUa_StreamType_Input)
    {
        OpcUa_UInt32            uCurrentBuffer  = 0;
        OpcUa_HttpsInputStream* pInputStream    = (OpcUa_HttpsInputStream*)((*a_ppStream)->Handle);

        if(pInputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
        {
            OpcUa_HttpsRequestLine_Clear(&pInputStream->StartLine.RequestLine);
        }

        else if(pInputStream->MessageType == OpcUa_HttpsStream_MessageType_Response)
        {
            OpcUa_HttpsStatusLine_Clear(&pInputStream->StartLine.StatusLine);
        }

        OpcUa_HttpsHeaderCollection_Delete(&pInputStream->Headers);

        /* clear buffer */
        /* Delete ignores OpcUa_Null, so if the buffer got detached by the upper layer, this works, too. */
        for(uCurrentBuffer = 0; uCurrentBuffer <= pInputStream->nBuffers; uCurrentBuffer++)
        {
            OpcUa_Buffer_Clear(&(pInputStream->Buffer[uCurrentBuffer]));
        }

        OpcUa_String_Clear(&(pInputStream->MessageLine));

        OpcUa_Free(*a_ppStream);
        *a_ppStream = OpcUa_Null;
    }
    else
    {
        /* error condition - should not happen */
        return;
    }
}

/*============================================================================
 * OpcUa_HttpsStream_GetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetPosition(
    OpcUa_Stream* a_pStream,
    OpcUa_UInt32* a_pPosition)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetPosition");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfInvalidStream(a_pStream, GetPosition);
    OpcUa_ReferenceParameter(a_pPosition);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
        OpcUa_HttpsOutputStream* pHttpOutputStream = (OpcUa_HttpsOutputStream*)a_pStream->Handle;

        if(pHttpOutputStream->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        *a_pPosition = pHttpOutputStream->nAbsolutePosition;
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        OpcUa_HttpsInputStream* pHttpInputStream = (OpcUa_HttpsInputStream*)a_pStream->Handle;

        if(pHttpInputStream->Closed)
        {
            return OpcUa_BadInvalidState;
        }

        *a_pPosition = pHttpInputStream->nAbsolutePosition;
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
 * OpcUa_HttpsStream_SetPosition
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_SetPosition(
    OpcUa_Stream* a_pStream,
    OpcUa_UInt32  a_uPosition)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "SetPosition");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfInvalidStream(a_pStream, SetPosition);
    OpcUa_ReferenceParameter(a_uPosition);

    uStatus = OpcUa_BadNotImplemented;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_GetMessageType
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetMessageType(
    OpcUa_Stream*                   a_pStream,
    OpcUa_HttpsStream_MessageType*  a_pMessageType)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetMessageType");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageType);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
        *a_pMessageType = ((OpcUa_HttpsOutputStream*)a_pStream->Handle)->MessageType;
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        *a_pMessageType = ((OpcUa_HttpsInputStream*)a_pStream->Handle)->MessageType;
    }
    else
    {
        *a_pMessageType = OpcUa_HttpsStream_MessageType_Unknown;
        uStatus         = OpcUa_BadInvalidArgument;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_GetMethod
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetMethod(
    OpcUa_Stream*               a_pStream,
    OpcUa_HttpsStream_Method*   a_pMethod)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetMethod");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pMethod);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
        *a_pMethod = OpcUa_HttpsStream_Method_Invalid;
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        OpcUa_HttpsInputStream* pHttpInputStream = (OpcUa_HttpsInputStream*)a_pStream->Handle;

        if(pHttpInputStream->eMethod != OpcUa_HttpsStream_Method_Invalid)
        {
            *a_pMethod = pHttpInputStream->eMethod;
        }
        else
        {
            if(0 == OpcUa_String_StrnCmp(   &pHttpInputStream->StartLine.RequestLine.RequestMethod,
                                            OpcUa_String_FromCString("POST"),
                                            4,
                                            OpcUa_False))
            {
                pHttpInputStream->eMethod = OpcUa_HttpsStream_Method_Post;
                *a_pMethod = OpcUa_HttpsStream_Method_Post;
            }
            else if(0 == OpcUa_String_StrnCmp(  &pHttpInputStream->StartLine.RequestLine.RequestMethod,
                                                OpcUa_String_FromCString("GET"),
                                                3,
                                                OpcUa_False))
            {
                pHttpInputStream->eMethod = OpcUa_HttpsStream_Method_Get;
                *a_pMethod = OpcUa_HttpsStream_Method_Get;
            }
            else if(0 == OpcUa_String_StrnCmp(  &pHttpInputStream->StartLine.RequestLine.RequestMethod,
                                                OpcUa_String_FromCString("HEAD"),
                                                4,
                                                OpcUa_False))
            {
                pHttpInputStream->eMethod = OpcUa_HttpsStream_Method_Head;
                *a_pMethod = OpcUa_HttpsStream_Method_Head;
            }
            else if(0 == OpcUa_String_StrnCmp(  &pHttpInputStream->StartLine.RequestLine.RequestMethod,
                                                OpcUa_String_FromCString("OPTIONS"),
                                                3,
                                                OpcUa_False))
            {
                pHttpInputStream->eMethod = OpcUa_HttpsStream_Method_Options;
                *a_pMethod = OpcUa_HttpsStream_Method_Options;
            }
            else
            {
                *a_pMethod = OpcUa_HttpsStream_Method_Invalid;
            }
        }
    }
    else
    {
        *a_pMethod = OpcUa_HttpsStream_Method_Invalid;
        uStatus    = OpcUa_BadInvalidArgument;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_GetSocket
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetSocket(
    OpcUa_Stream*   a_pStream,
    OpcUa_Socket*   a_pSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetSocket");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pSocket);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
        *a_pSocket = ((OpcUa_HttpsOutputStream*)a_pStream->Handle)->Socket;
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        *a_pSocket = ((OpcUa_HttpsInputStream*)a_pStream->Handle)->Socket;
    }
    else
    {
        *a_pSocket = OpcUa_Null;
        uStatus    = OpcUa_BadInvalidArgument;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_SetHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_SetHeader(
    OpcUa_Stream*   a_pStream,
    OpcUa_String*   a_pHeaderName,
    OpcUa_String*   a_pHeaderValue)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "SetHeader");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pHeaderName);
    OpcUa_ReturnErrorIfArgumentNull(a_pHeaderValue);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

        uStatus = OpcUa_HttpsHeaderCollection_SetValue(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->Headers,
                                                       a_pHeaderName,
                                                       a_pHeaderValue);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

        OpcUa_StringA   strRaw  = OpcUa_Null;
        OpcUa_UInt32    uLength = 0;

        strRaw  = OpcUa_String_GetRawString(a_pHeaderName);
        uLength = OpcUa_String_StrSize(a_pHeaderName);

        uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->HeaderBuffer),
                                        (OpcUa_Byte*)strRaw,
                                        uLength);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->HeaderBuffer),
                                        (OpcUa_Byte*)": ",
                                        2);
        OpcUa_GotoErrorIfBad(uStatus);

        strRaw  = OpcUa_String_GetRawString(a_pHeaderValue);
        uLength = OpcUa_String_StrSize(a_pHeaderValue);

        uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->HeaderBuffer),
                                        (OpcUa_Byte*)strRaw,
                                        uLength);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->HeaderBuffer),
                                        (OpcUa_Byte*)"\r\n",
                                        2);
        OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        uStatus = OpcUa_HttpsHeaderCollection_SetValue(((OpcUa_HttpsInputStream*)a_pStream->Handle)->Headers,
                                                       a_pHeaderName,
                                                       a_pHeaderValue);
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
* OpcUa_HttpsStream_SetSocket
*===========================================================================*/
/** @brief Sets the socket handle of the stream. */
OpcUa_StatusCode OpcUa_HttpsStream_SetSocket(   OpcUa_OutputStream* a_pOutputStream,
                                                OpcUa_Socket        a_pSocket)
{
    OpcUa_HttpsOutputStream* pHttpOutputStream = (OpcUa_HttpsOutputStream*)a_pOutputStream;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "SetSocket");

    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream);

    pHttpOutputStream->Socket = a_pSocket;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_GetHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetHeader(
    OpcUa_Stream*   a_pStream,
    OpcUa_String*   a_pHeaderName,
    OpcUa_Boolean   a_bGiveCopy,
    OpcUa_String*   a_pHeaderValue)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetHeader");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pHeaderName);
    OpcUa_ReturnErrorIfArgumentNull(a_pHeaderValue);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

        uStatus = OpcUa_HttpsHeaderCollection_GetValue(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->Headers,
                                                       a_pHeaderName,
                                                       a_bGiveCopy,
                                                       a_pHeaderValue);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

        uStatus = OpcUa_BadNotSupported;

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        uStatus = OpcUa_HttpsHeaderCollection_GetValue(((OpcUa_HttpsInputStream*)a_pStream->Handle)->Headers,
                                                       a_pHeaderName,
                                                       a_bGiveCopy,
                                                       a_pHeaderValue);
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
 * OpcUa_HttpsStream_GetStatusCode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetStatusCode(
    OpcUa_Stream*   a_pStream,
    OpcUa_UInt32*   a_pStatusCode)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetStatusCode");

    OpcUa_ReturnErrorIfArgumentNull(a_pStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pStatusCode);

    if(a_pStream->Type == OpcUa_StreamType_Output)
    {
        if(((OpcUa_HttpsOutputStream*)a_pStream->Handle)->MessageType == OpcUa_HttpsStream_MessageType_Response)
        {
            *a_pStatusCode = ((OpcUa_HttpsOutputStream*)a_pStream->Handle)->StartLine.StatusLine.StatusCode;
        }
        else
        {
            uStatus = OpcUa_BadInvalidArgument;
        }
    }
    else if(a_pStream->Type == OpcUa_StreamType_Input)
    {
        if(((OpcUa_HttpsInputStream*)a_pStream->Handle)->MessageType == OpcUa_HttpsStream_MessageType_Response)
        {
            *a_pStatusCode = ((OpcUa_HttpsInputStream*)a_pStream->Handle)->StartLine.StatusLine.StatusCode;
        }
        else
        {
            uStatus = OpcUa_BadInvalidArgument;
        }
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
 * OpcUa_HttpsStream_GetConnection
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetConnection(
    OpcUa_OutputStream* a_pOutputStream,
    OpcUa_Handle*       a_pConnection)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "OpcUa_HttpsStream_GetConnection");

    OpcUa_ReturnErrorIfArgumentNull(a_pOutputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pConnection);

    OpcUa_ReturnErrorIfTrue(a_pOutputStream->Type != OpcUa_StreamType_Output,
                            OpcUa_BadInvalidArgument);

    *a_pConnection = ((OpcUa_HttpsOutputStream*)a_pOutputStream->Handle)->hConnection;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_GetState
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetState(
    OpcUa_InputStream*       a_pInputStream,
    OpcUa_HttpsStream_State* a_pStreamState)
{
OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "OpcUa_HttpsStream_GetState");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pStreamState);

    OpcUa_ReturnErrorIfTrue(a_pInputStream->Type != OpcUa_StreamType_Input,
                            OpcUa_BadInvalidArgument);

    *a_pStreamState = ((OpcUa_HttpsInputStream*)a_pInputStream->Handle)->State;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_CreateInput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_CreateInput(
    OpcUa_Socket                    a_hSocket,
    OpcUa_HttpsStream_MessageType   a_eMessageType,
    OpcUa_InputStream**             a_ppInputStream)
{
    OpcUa_HttpsInputStream* pHttpInputStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "CreateInput");

    OpcUa_ReturnErrorIfArgumentNull(a_hSocket);
    OpcUa_ReturnErrorIfArgumentNull(a_ppInputStream);

    OpcUa_ReturnErrorIfTrue(    a_eMessageType != OpcUa_HttpsStream_MessageType_Request
                             && a_eMessageType != OpcUa_HttpsStream_MessageType_Response,
                            OpcUa_BadInvalidArgument);

    *a_ppInputStream = OpcUa_Null;

    pHttpInputStream = (OpcUa_HttpsInputStream*)OpcUa_Alloc(sizeof(OpcUa_HttpsInputStream));
    OpcUa_GotoErrorIfAllocFailed(pHttpInputStream);
    OpcUa_MemSet(pHttpInputStream, 0, sizeof(OpcUa_HttpsInputStream));

    pHttpInputStream->SanityCheck   = OpcUa_HttpsInputStream_SanityCheck;
    pHttpInputStream->MessageType   = a_eMessageType;
    pHttpInputStream->Closed        = OpcUa_False;
    pHttpInputStream->Socket        = a_hSocket;
    pHttpInputStream->State         = OpcUa_HttpsStream_State_Empty;
    pHttpInputStream->eMethod       = OpcUa_HttpsStream_Method_Invalid;

    /* create message start line */
    if(a_eMessageType == OpcUa_HttpsStream_MessageType_Request)
    {
        OpcUa_HttpsRequestLine_Initialize(&pHttpInputStream->StartLine.RequestLine);
    }
    else if(a_eMessageType == OpcUa_HttpsStream_MessageType_Response)
    {
        OpcUa_HttpsStatusLine_Initialize(&pHttpInputStream->StartLine.StatusLine);
    }

    OpcUa_String_Initialize(&pHttpInputStream->MessageLine);

    *a_ppInputStream = (OpcUa_InputStream*)pHttpInputStream;

    (*a_ppInputStream)->Type            = OpcUa_StreamType_Input;
    (*a_ppInputStream)->Handle          = pHttpInputStream;
    (*a_ppInputStream)->GetPosition     = OpcUa_HttpsStream_GetPosition;
    (*a_ppInputStream)->SetPosition     = OpcUa_HttpsStream_SetPosition;
    (*a_ppInputStream)->GetChunkLength  = OpcUa_HttpsStream_GetChunkLength;
    (*a_ppInputStream)->DetachBuffer    = OpcUa_HttpsStream_DetachBuffer;
    (*a_ppInputStream)->AttachBuffer    = OpcUa_HttpsStream_AttachBuffer;
    (*a_ppInputStream)->Close           = OpcUa_HttpsStream_Close;
    (*a_ppInputStream)->Delete          = OpcUa_HttpsStream_Delete;
    (*a_ppInputStream)->Read            = OpcUa_HttpsStream_Read;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pHttpInputStream != OpcUa_Null)
    {
        OpcUa_Free(pHttpInputStream);
    }

    *a_ppInputStream = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_CreateOutput
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsStream_CreateOutput(
    OpcUa_Socket                          a_hSocket,
    OpcUa_HttpsStream_MessageType         a_eMessageType,
    OpcUa_StringA                         a_sMessageHeaders,
    OpcUa_OutputStream**                  a_ppOutputStream)
{
    OpcUa_HttpsOutputStream*    pHttpOutputStream   = OpcUa_Null;
    OpcUa_Byte*                 pBufferData         = OpcUa_Null;

#if !OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION
    OpcUa_Byte*                 pHeaderData         = OpcUa_Null;
#endif

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "CreateOutput");

    OpcUa_ReturnErrorIfTrue(    a_eMessageType != OpcUa_HttpsStream_MessageType_Request
                             && a_eMessageType != OpcUa_HttpsStream_MessageType_Response,
                            OpcUa_BadInvalidArgument);

    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);

    *a_ppOutputStream = OpcUa_Null;

    /* allocate http out stream */
    pHttpOutputStream = (OpcUa_HttpsOutputStream*)OpcUa_Alloc(sizeof(OpcUa_HttpsOutputStream));
    OpcUa_GotoErrorIfAllocFailed(pHttpOutputStream);
    OpcUa_MemSet(pHttpOutputStream, 0, sizeof(OpcUa_HttpsOutputStream));

    pHttpOutputStream->SanityCheck        = OpcUa_HttpsOutputStream_SanityCheck;
    pHttpOutputStream->MessageType        = a_eMessageType;
    pHttpOutputStream->Closed             = OpcUa_False;
    pHttpOutputStream->Socket             = a_hSocket;

    /* create message start line */
    if(a_eMessageType == OpcUa_HttpsStream_MessageType_Request)
    {
        OpcUa_HttpsRequestLine_Initialize(&pHttpOutputStream->StartLine.RequestLine);
    }
    else if(a_eMessageType == OpcUa_HttpsStream_MessageType_Response)
    {
        OpcUa_HttpsStatusLine_Initialize(&pHttpOutputStream->StartLine.StatusLine);
    }

    /* create header collection */

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

    if(a_sMessageHeaders != OpcUa_Null)
    {
        uStatus = OpcUa_HttpsHeaderCollection_Parse(OpcUa_String_FromCString(a_sMessageHeaders),
                                                    &(pHttpOutputStream->Headers));
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        uStatus = OpcUa_HttpsHeaderCollection_Create(&(pHttpOutputStream->Headers));
        OpcUa_GotoErrorIfBad(uStatus);
    }

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

    OpcUa_ReferenceParameter(a_sMessageHeaders);

    pHeaderData = OpcUa_Alloc(OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);
    OpcUa_GotoErrorIfAllocFailed(pHeaderData);

    uStatus = OpcUa_Buffer_Initialize(  &(pHttpOutputStream->HeaderBuffer),
                                        pHeaderData,
                                        0,
                                        OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH,
                                        OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH,
                                        OpcUa_True);
    OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

    pHttpOutputStream->nBuffers = 0;

    /* create an internal buffer. */
    pBufferData = (OpcUa_Byte*)OpcUa_Alloc(OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH);
    OpcUa_GotoErrorIfAllocFailed(pBufferData);

    uStatus = OpcUa_Buffer_Initialize(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]),                /* instance        */
                                      pBufferData,                                                              /* bufferdata      */
                                      0,                                                                        /* datasize        */
                                      OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH,                                       /* blocksize       */
                                      OPCUA_HTTPS_MAX_SEND_BUFFER_LENGTH,                                       /* maxsize         */
                                      OpcUa_True);                                                              /* should be freed */
    OpcUa_GotoErrorIfBad(uStatus);

    /* reserve header space */
    OpcUa_Buffer_SetEndOfData(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]), OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);
    OpcUa_Buffer_SetPosition(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]), OPCUA_HTTPS_MAX_SEND_HEADER_LENGTH);

    /* reserve footer space */
    pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers].Size -= OPCUA_HTTPS_MAX_SEND_FOOTER_LENGTH;

    *a_ppOutputStream                      = (OpcUa_OutputStream*)pHttpOutputStream;

    /* now initialize superclass members */
    (*a_ppOutputStream)->Type              = OpcUa_StreamType_Output;
    (*a_ppOutputStream)->Handle            = pHttpOutputStream;
    (*a_ppOutputStream)->GetPosition       = OpcUa_HttpsStream_GetPosition;
    (*a_ppOutputStream)->SetPosition       = OpcUa_HttpsStream_SetPosition;
    (*a_ppOutputStream)->GetChunkLength    = OpcUa_HttpsStream_GetChunkLength;
    (*a_ppOutputStream)->DetachBuffer      = OpcUa_HttpsStream_DetachBuffer;
    (*a_ppOutputStream)->AttachBuffer      = OpcUa_HttpsStream_AttachBuffer;
    (*a_ppOutputStream)->Close             = OpcUa_HttpsStream_Close;
    (*a_ppOutputStream)->Delete            = OpcUa_HttpsStream_Delete;
    (*a_ppOutputStream)->Write             = OpcUa_HttpsStream_Write;
    (*a_ppOutputStream)->Flush             = OpcUa_HttpsStream_Flush;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pHttpOutputStream != OpcUa_Null)
    {
        OpcUa_Buffer_Clear(&(pHttpOutputStream->Buffer[pHttpOutputStream->nBuffers]));

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

        OpcUa_HttpsHeaderCollection_Delete(&pHttpOutputStream->Headers);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

        OpcUa_Buffer_Clear(&pHttpOutputStream->HeaderBuffer);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

        OpcUa_Free(pHttpOutputStream);
    }

    *a_ppOutputStream = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_CreateRequest
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_CreateRequest(
    OpcUa_Socket                            a_hSocket,
    OpcUa_StringA                           a_sRequestMethod,
    OpcUa_StringA                           a_sRequestUri,
    OpcUa_StringA                           a_sRequestHeaders,
    OpcUa_OutputStream**                    a_ppOutputStream)
{
#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION
    OpcUa_HttpsRequestLine* pHttpRequestLine = OpcUa_Null;
#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "CreateRequest");

    OpcUa_ReturnErrorIfArgumentNull(a_sRequestMethod);
    OpcUa_ReturnErrorIfArgumentNull(a_sRequestUri);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);

    uStatus = OpcUa_HttpsStream_CreateOutput(a_hSocket,
                                             OpcUa_HttpsStream_MessageType_Request,
                                             a_sRequestHeaders,
                                             a_ppOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

    pHttpRequestLine = &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->StartLine.RequestLine);
    OpcUa_GotoErrorIfNull(pHttpRequestLine, OpcUa_BadInternalError);

    uStatus = OpcUa_String_AttachCopy(&(pHttpRequestLine->RequestMethod), a_sRequestMethod);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_String_AttachCopy(&(pHttpRequestLine->RequestUri), a_sRequestUri);
    OpcUa_GotoErrorIfBad(uStatus);

#else /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)a_sRequestMethod,
                                    OpcUa_StrLenA(a_sRequestMethod));
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)"\x20",
                                    1);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)a_sRequestUri,
                                    OpcUa_StrLenA(a_sRequestUri));
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)"\x20",
                                    1);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)OPCUA_HTTP_VERSION,
                                    8);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)"\r\n",
                                    2);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                    (OpcUa_Byte*)a_sRequestHeaders,
                                    OpcUa_StrLenA(a_sRequestHeaders));
    OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION */


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_HttpsStream_Delete((OpcUa_Stream**)a_ppOutputStream);

    *a_ppOutputStream = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_CreateResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_CreateResponse(
    OpcUa_Socket                            a_hSocket,
    OpcUa_UInt32                            a_uStatusCode,
    OpcUa_StringA                           a_sReasonPhrase,
    OpcUa_StringA                           a_sResponseHeaders,
    OpcUa_Handle                            a_hConnection,
    OpcUa_OutputStream**                    a_ppOutputStream)
{
    OpcUa_HttpsStatusLine* pHttpStatusLine = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "CreateResponse");

    OpcUa_ReturnErrorIfArgumentNull(a_ppOutputStream);

    /* only three-digit status codes are allowed */
    OpcUa_ReturnErrorIfTrue(a_uStatusCode < 100 || a_uStatusCode > 999, OpcUa_BadInvalidArgument);

    uStatus = OpcUa_HttpsStream_CreateOutput(a_hSocket,
                                             OpcUa_HttpsStream_MessageType_Response,
                                             a_sResponseHeaders,
                                             a_ppOutputStream);
    OpcUa_GotoErrorIfBad(uStatus);

    pHttpStatusLine = &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->StartLine.StatusLine);
    OpcUa_GotoErrorIfNull(pHttpStatusLine, OpcUa_BadInternalError);

    pHttpStatusLine->StatusCode = a_uStatusCode;

    if(a_sReasonPhrase != OpcUa_Null)
    {
        uStatus = OpcUa_String_AttachCopy(&(pHttpStatusLine->ReasonPhrase), a_sReasonPhrase);
        OpcUa_GotoErrorIfBad(uStatus);
    }

#if !OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION

    uStatus = OpcUa_HttpsStatusLine_Serialize(  &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->StartLine.StatusLine),
                                                &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer));
    OpcUa_GotoErrorIfBad(uStatus);

    if(a_sResponseHeaders != OpcUa_Null)
    {
        uStatus = OpcUa_Buffer_Write(   &(((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->HeaderBuffer),
                                        (OpcUa_Byte*)a_sResponseHeaders,
                                        OpcUa_StrLenA(a_sResponseHeaders));
        OpcUa_GotoErrorIfBad(uStatus);
    }

#endif

    ((OpcUa_HttpsOutputStream*)((*a_ppOutputStream)->Handle))->hConnection = a_hConnection;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_HttpsStream_Delete((OpcUa_Stream**)a_ppOutputStream);

    *a_ppOutputStream = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_Receive
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsStream_Receive(
    OpcUa_InputStream*  a_pInputStream,
    OpcUa_UInt32        a_uCountExpected,
    OpcUa_UInt32*       a_pCountActual)
{
    OpcUa_HttpsInputStream* pHttpInputStream    = OpcUa_Null;
    OpcUa_Byte*             pBufferedData       = OpcUa_Null;
    OpcUa_UInt32            uBufferLeft         = 0;
    OpcUa_UInt32            uBytesBuffered      = 0;
    OpcUa_UInt32            uBytesToRead        = a_uCountExpected;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "Receive");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCountActual);

    pHttpInputStream = (OpcUa_HttpsInputStream*)a_pInputStream->Handle;

    /* shortcuts */
    uBytesBuffered = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData;
    pBufferedData  = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Data;
    uBufferLeft    = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Size - uBytesBuffered;

    /* cap amount of bytes requested to remaining buffer space */
    if(uBufferLeft < a_uCountExpected)
    {
        uBytesToRead = uBufferLeft;
    }

    /* read data from the network */
    uStatus = OPCUA_P_SOCKET_READ(pHttpInputStream->Socket,
                                  &(pBufferedData[uBytesBuffered]),
                                  uBytesToRead,
                                  a_pCountActual);

    /* adjust the status code if necessary */
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_Receive: Read from socket failed with 0x%08X\n", uStatus);
        uStatus = (uStatus == OpcUa_BadWouldBlock)? OpcUa_GoodCallAgain: uStatus;
        OpcUa_GotoError;
    }

    /* update the end of data marker */
    pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData = uBytesBuffered + *a_pCountActual;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    *a_pCountActual = 0;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_ReadLine
 *===========================================================================*/
/** @brief reads a line of characters */
static OpcUa_StatusCode OpcUa_HttpsStream_ReadLine(
    OpcUa_InputStream*  a_pInputStream,
    OpcUa_String*       a_pMessageLine)
{
    OpcUa_UInt32            LineStart           = 0;
    OpcUa_UInt32            LineEnd             = 0;
    OpcUa_HttpsInputStream* pHttpInputStream    = OpcUa_Null;
    OpcUa_CharA*            pLineContent        = OpcUa_Null;
    OpcUa_CharA             chAsciiChar         = '\x00';
    OpcUa_UInt32            uCharCount          = 0;
    OpcUa_UInt32            uCurrentReadBuffer  = 0;
    OpcUa_Boolean           bCR                 = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "ReadLine");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageLine);

    pHttpInputStream = (OpcUa_HttpsInputStream*)a_pInputStream->Handle;
    uCurrentReadBuffer = pHttpInputStream->nCurrentReadBuffer;

    uStatus = OpcUa_Buffer_GetPosition(&pHttpInputStream->Buffer[uCurrentReadBuffer],
                                       &LineStart);
    OpcUa_ReturnErrorIfBad(uStatus);

    for(uCharCount = 0; uCharCount < OPCUA_HTTPS_MAX_RECV_HEADER_LINE_LENGTH; uCharCount++)
    {
        do
        {
            uStatus = OpcUa_Https_ReadCharFromBuffer(&pHttpInputStream->Buffer[uCurrentReadBuffer], &chAsciiChar);

            if(uStatus == OpcUa_BadEndOfStream)
            {
                if(uCurrentReadBuffer > pHttpInputStream->nCurrentReadBuffer)
                {
                    /* header must not span more than two buffers */
                    OpcUa_GotoErrorWithStatus(OpcUa_BadDecodingError);
                }
                else if(uCurrentReadBuffer < pHttpInputStream->nBuffers)
                {
                    uCurrentReadBuffer++;
                    uStatus = OpcUa_GoodCallAgain;
                }
                else
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_GoodCallAgain);
                }
            }

        } while(uStatus == OpcUa_GoodCallAgain);

        OpcUa_GotoErrorIfBad(uStatus);

        if(chAsciiChar == '\r')
        {
            OpcUa_GotoErrorIfTrue(bCR != OpcUa_False, OpcUa_BadDecodingError);
            bCR = OpcUa_True;
        }
        else if(chAsciiChar == '\n')
        {
            OpcUa_GotoErrorIfTrue(bCR != OpcUa_True, OpcUa_BadDecodingError);
            break;
        }
    } /* peek until \r\n */

    uStatus = OpcUa_Buffer_GetPosition(&pHttpInputStream->Buffer[uCurrentReadBuffer],
                                       &LineEnd);
    OpcUa_GotoErrorIfBad(uStatus);

    /* assuming that a message line is not longer than a whole buffer */
    if(LineEnd > LineStart)
    {
        uCharCount = LineEnd - LineStart;
        OpcUa_GotoErrorIfTrue(uCharCount < 2, OpcUa_BadInternalError);

        /* do not include CR and LF characters into the resulting string */
        uCharCount -= 2;

        pLineContent = (OpcUa_CharA*)&(pHttpInputStream->Buffer[uCurrentReadBuffer].Data[LineStart]);

        uStatus = OpcUa_String_AttachToString(pLineContent,
                                              uCharCount,
                                              uCharCount,
#if OPCUA_HTTPS_COPYHEADERS
                                              OpcUa_True,
#else
                                              OpcUa_False,
#endif
                                              OpcUa_False,
                                              a_pMessageLine);
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else if(LineEnd == 1)
    {
        uCharCount = LineEnd + (pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData - LineStart);
        OpcUa_GotoErrorIfTrue(uCharCount < 2, OpcUa_BadInternalError);

        /* do not include CR and LF characters into the resulting string */
        uCharCount -= 2;

        pLineContent = (OpcUa_CharA*)&(pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].Data[LineStart]);

        uStatus = OpcUa_String_AttachToString(pLineContent,
                                              uCharCount,
                                              uCharCount,
#if OPCUA_HTTPS_COPYHEADERS
                                              OpcUa_True,
#else
                                              OpcUa_False,
#endif
                                              OpcUa_False,
                                              a_pMessageLine);
        OpcUa_GotoErrorIfBad(uStatus);

        pHttpInputStream->nCurrentReadBuffer = uCurrentReadBuffer;
    }
    else
    {
        OpcUa_String sTemp = OPCUA_STRING_STATICINITIALIZER;
        uCharCount = LineEnd + (pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData - LineStart);
        OpcUa_GotoErrorIfTrue(uCharCount < 2, OpcUa_BadInternalError);

        pLineContent = (OpcUa_CharA*)&(pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].Data[LineStart]);

        uStatus = OpcUa_String_AttachToString(pLineContent,
                                              pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData - LineStart,
                                              pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer].EndOfData - LineStart,
                                              OpcUa_True,
                                              OpcUa_False,
                                              a_pMessageLine);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_String_AttachToString((OpcUa_CharA*)pHttpInputStream->Buffer[uCurrentReadBuffer].Data,
                                              LineEnd - 2,
                                              LineEnd - 2,
                                              OpcUa_False,
                                              OpcUa_False,
                                              &sTemp);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_String_StrnCat( a_pMessageLine,
                                        &sTemp,
                                        LineEnd - 2); /* do not include CR and LF characters into the resulting string */
        OpcUa_GotoErrorIfBad(uStatus);

        pHttpInputStream->nCurrentReadBuffer = uCurrentReadBuffer;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsStream_ReadLine: Error reading HTTP protocol line. %u character read.\n", uCharCount);
    }

    if(uCurrentReadBuffer > pHttpInputStream->nCurrentReadBuffer)
    {
        OpcUa_Buffer_SetPosition(&pHttpInputStream->Buffer[uCurrentReadBuffer], 0);
    }

    OpcUa_Buffer_SetPosition(&pHttpInputStream->Buffer[pHttpInputStream->nCurrentReadBuffer],
                             LineStart);

    OpcUa_String_Clear(a_pMessageLine);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_ReadChunkLength
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_HttpsStream_ReadChunkLength(  OpcUa_InputStream*  a_pInputStream,
                                                            OpcUa_Int32*        a_piChunkLength)
{
    OpcUa_String            sChunkHeader        = OPCUA_STRING_STATICINITIALIZER;
    OpcUa_Int               iResult             = 0;
#if !OPCUA_HTTPS_COPYHEADERS
    OpcUa_CharA             chRawString[20];
#endif

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "ReadChunkLength");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_piChunkLength);

    *a_piChunkLength = 0;

    uStatus = OpcUa_HttpsStream_ReadLine(a_pInputStream, &sChunkHeader);
    OpcUa_GotoErrorIfBad(uStatus);

    if(uStatus == OpcUa_GoodCallAgain)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_ReadChunkLength: Chunk size line not complete!\n");
        *a_piChunkLength = -1;
        OpcUa_GotoError;
    }

#if OPCUA_HTTPS_COPYHEADERS
    iResult = OpcUa_SScanfA(OpcUa_String_GetRawString(&sChunkHeader), "%X", a_piChunkLength);
#else
    OpcUa_MemSet(chRawString, 0, sizeof(chRawString));
    uStatus = OpcUa_MemCpy(chRawString, sizeof(chRawString)-1, OpcUa_String_GetRawString(&sChunkHeader), OpcUa_String_StrSize(&sChunkHeader));
    OpcUa_GotoErrorIfBad(uStatus);
    iResult = OpcUa_SScanfA(chRawString, "%X", a_piChunkLength);
#endif
    if((iResult != 1) || (*a_piChunkLength < 0) || (*a_piChunkLength > OPCUA_HTTPS_MAX_RECV_MESSAGE_LENGTH))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsStream_ReadChunkLength: Chunk size could not be read!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadRequestHeaderInvalid);
    }

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_ReadChunkLength: chunk length is %i.\n", *a_piChunkLength);

    OpcUa_String_Clear(&sChunkHeader);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_String_Clear(&sChunkHeader);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_ProcessHeaders
 *===========================================================================*/
/** @brief processes message headers */
static OpcUa_StatusCode OpcUa_HttpsStream_ProcessHeaders(
    OpcUa_InputStream*  a_pInputStream)
{
    OpcUa_HttpsInputStream* pHttpInputStream    = OpcUa_Null;
    OpcUa_HttpsHeader*      pHttpHeader         = OpcUa_Null;
    OpcUa_Boolean           bLengthValid        = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "ProcessHeaders");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);

    pHttpInputStream = (OpcUa_HttpsInputStream*)a_pInputStream->Handle;

    pHttpInputStream->iContentLength = 0;

    OpcUa_List_ResetCurrent(pHttpInputStream->Headers);
    pHttpHeader = (OpcUa_HttpsHeader*)OpcUa_List_GetCurrentElement(pHttpInputStream->Headers);

    while(pHttpHeader != OpcUa_Null)
    {
        /* check for content length header */
        if(OpcUa_String_StrnCmp(&pHttpHeader->Name, OpcUa_String_FromCString("Content-Length"), OPCUA_STRING_LENDONTCARE, OpcUa_True) == 0)
        {
            if(OpcUa_String_IsEmpty(&pHttpHeader->Value))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
            }

            /* error if content length already set by other header. */
            OpcUa_GotoErrorIfTrue((pHttpInputStream->iContentLength != 0), OpcUa_BadRequestHeaderInvalid);

            pHttpInputStream->iContentLength = (OpcUa_Int32)OpcUa_CharAToInt(OpcUa_String_GetRawString(&pHttpHeader->Value));
            OpcUa_GotoErrorIfTrue((pHttpInputStream->iContentLength <= 0), OpcUa_BadRequestHeaderInvalid);
            bLengthValid = OpcUa_True;
        }

        /* check for transfer encoding header. */
        if(OpcUa_String_StrnCmp(&pHttpHeader->Name, OpcUa_String_FromCString("Transfer-Encoding"), OPCUA_STRING_LENDONTCARE, OpcUa_True) == 0)
        {
            if(OpcUa_String_IsEmpty(&pHttpHeader->Value))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
            }

            /* error if content length already set by other header. */
            OpcUa_GotoErrorIfTrue((pHttpInputStream->iContentLength != 0), OpcUa_BadRequestHeaderInvalid);

            /* -1 marks chunked encoding */
            pHttpInputStream->iContentLength = -1;
            bLengthValid = OpcUa_True;
        }

#if OPCUA_HTTPSSTREAM_BLOCKINGWRITE
        /* handle 100 continue */
        if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
        {
            if(OpcUa_String_StrnCmp(&pHttpHeader->Name, OpcUa_String_FromCString("Expect"), OPCUA_STRING_LENDONTCARE, OpcUa_True) == 0)
            {
                if(!OpcUa_String_IsEmpty(&pHttpHeader->Name))
                {
                    if(OpcUa_String_StrnCmp(&pHttpHeader->Name, OpcUa_String_FromCString("100-continue"), OPCUA_STRING_LENDONTCARE, OpcUa_False) == 0)
                    {
                        OpcUa_CharA* sHttpResponse   = "HTTP/1.1 100 Continue\r\n\r\n";
                        OpcUa_UInt32 uResponseLength = OpcUa_StrLenA(sHttpResponse);

                        OPCUA_P_SOCKET_WRITE(pHttpInputStream->Socket,
                                             (OpcUa_Byte*)sHttpResponse,
                                             uResponseLength,
                                             OpcUa_True);
                    }
                }
            }
        }
#endif

        pHttpHeader = (OpcUa_HttpsHeader*)OpcUa_List_GetNextElement(pHttpInputStream->Headers);
    }

    if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
    {
        OpcUa_HttpsStream_Method eMethod = OpcUa_HttpsStream_Method_Invalid;

        uStatus = OpcUa_HttpsStream_GetMethod(  (OpcUa_Stream*)a_pInputStream,
                                                &eMethod);
        OpcUa_GotoErrorIfBad(uStatus);

        /* allow missing length information for GET and OPTIONS */
        if(     eMethod != OpcUa_HttpsStream_Method_Get
            &&  eMethod != OpcUa_HttpsStream_Method_Options)
        {
            OpcUa_GotoErrorIfTrue((bLengthValid == OpcUa_False), OpcUa_BadSyntaxError);
        }
    }
    else
    {
        OpcUa_GotoErrorIfTrue((bLengthValid == OpcUa_False), OpcUa_BadSyntaxError);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsStream_ProcessHeaders: Error during HTTP header parsing!\n");
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_CheckRequestLine
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_CheckRequestLine(OpcUa_HttpsInputStream* a_pHttpInputStream)
{
    OpcUa_HttpsStream_Method eMethod = OpcUa_HttpsStream_Method_Invalid;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "CheckRequestLine");

    uStatus = OpcUa_HttpsStream_GetMethod(  (OpcUa_Stream*)a_pHttpInputStream,
                                            &eMethod);

    /* for now only POST and HEAD requests are supported */
    if(     eMethod != OpcUa_HttpsStream_Method_Post
        &&  eMethod != OpcUa_HttpsStream_Method_Head
#if OPCUA_HTTPS_ALLOW_GET
        &&  eMethod != OpcUa_HttpsStream_Method_Get
#endif /* OPCUA_HTTPS_ALLOW_GET */
#if OPCUA_HTTPSSTREAM_ALLOW_OPTIONS_METHOD
        &&  eMethod != OpcUa_HttpsStream_Method_Options
#endif /* OPCUA_HTTPSSTREAM_ALLOW_OPTIONS_METHOD */
                            )
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadHttpMethodNotAllowed);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_HttpsStream_CheckRequestLine: Unsupported request method!\n");

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_GetRequestUri
 *===========================================================================*/
OpcUa_StatusCode OpcUa_HttpsStream_GetRequestUri(OpcUa_InputStream* a_pInputStream,
                                                 OpcUa_Boolean      a_bCopy,
                                                 OpcUa_String*      a_pRequestUri)
{
    OpcUa_HttpsInputStream* pHttpInputStream = a_pInputStream->Handle;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "GetRequestUri");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestUri);

    if(a_bCopy != OpcUa_False)
    {
        OpcUa_String_StrnCpy(   a_pRequestUri,
                               &pHttpInputStream->StartLine.RequestLine.RequestUri,
                                OPCUA_STRING_LENDONTCARE);
    }
    else

    {
        OpcUa_String_AttachToString(    OpcUa_String_GetRawString(&pHttpInputStream->StartLine.RequestLine.RequestUri),
                                        OpcUa_String_StrSize(&pHttpInputStream->StartLine.RequestLine.RequestUri),
                                        OpcUa_String_StrSize(&pHttpInputStream->StartLine.RequestLine.RequestUri),
                                        OpcUa_False,
                                        OpcUa_False,
                                        a_pRequestUri);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_HttpsStream_DataReady
 *===========================================================================*/
/** @brief Called if data is available for reading on a socket attached to a stream.
  *
  * This is kind of a read event handler of the pHttpInputStream. The Listener
  * calls this function, if new data is available on the socket. Dependend of
  * the stream state, it starts handling the http stream relevant data and
  * gives feedback to the listener, which takes further action, ie. calls
  * the handler.
  *
  * @param a_pIstrm [ in] The stream for which data is ready to be received.
  *
  * @return StatusCode
  */
OpcUa_StatusCode OpcUa_HttpsStream_DataReady(OpcUa_InputStream* a_pInputStream)
{
    OpcUa_HttpsInputStream* pHttpInputStream    = OpcUa_Null;
    OpcUa_UInt32            uExpectedLength     = OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH;
    OpcUa_UInt32            uActualLength       = 0;
    OpcUa_String            sMessageLine        = OPCUA_STRING_STATICINITIALIZER;
    OpcUa_HttpsHeader*      pMessageHeader      = OpcUa_Null;
    OpcUa_Boolean           bReadAgain          = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_HttpStream, "DataReady");

    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputStream->Handle);

    pHttpInputStream = (OpcUa_HttpsInputStream*)a_pInputStream->Handle;

    do
    {
        if(pHttpInputStream->State == OpcUa_HttpsStream_State_Empty)
        {
            /* This is a new stream and a new message. Initialize first buffer. */
            OpcUa_Byte* pBufferData = (OpcUa_Byte*)OpcUa_Alloc(OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH);
            OpcUa_GotoErrorIfAllocFailed(pBufferData);

            uStatus = OpcUa_Buffer_Initialize(&pHttpInputStream->Buffer[0],
                                              pBufferData,
                                              0,
                                              OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH,
                                              OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH,
                                              OpcUa_True);
            if(OpcUa_IsBad(uStatus))
            {
                OpcUa_Buffer_Clear(&pHttpInputStream->Buffer[0]);
                OpcUa_ReturnStatusCode;
            }

            OpcUa_Buffer_SetEmpty(&pHttpInputStream->Buffer[0]);

            uStatus = OpcUa_HttpsHeaderCollection_Create(&pHttpInputStream->Headers);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        if(pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData < pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Size)
        {
            /* calculate length of data to read */
            if(pHttpInputStream->State == OpcUa_HttpsStream_State_Body &&
               pHttpInputStream->iContentLength > 0)
            {
                uActualLength = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData - pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Position;
                uExpectedLength = pHttpInputStream->iContentLength - uActualLength;
            }
            else
            {
                uExpectedLength = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Size - pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData;
            }
        }
        else
        {
            /* last buffer is filled - switch to the next one if possible */
            if(pHttpInputStream->nBuffers < OPCUA_HTTPS_MAX_RECV_BUFFER_COUNT - 1)
            {
                /* This is a new stream and a new message. */
                OpcUa_Byte* pBufferData = (OpcUa_Byte*)OpcUa_Alloc(OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH);
                OpcUa_GotoErrorIfAllocFailed(pBufferData);

                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: Preparing new buffer of size %u.\n", OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH);

                /* skip current receive buffer index. */
                pHttpInputStream->nBuffers++;

                uStatus = OpcUa_Buffer_Initialize(&pHttpInputStream->Buffer[pHttpInputStream->nBuffers],
                                                  pBufferData,
                                                  0,
                                                  OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH,
                                                  OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH,
                                                  OpcUa_True);
                if(OpcUa_IsBad(uStatus))
                {
                    OpcUa_Buffer_Clear(&pHttpInputStream->Buffer[pHttpInputStream->nBuffers]);
                    OpcUa_ReturnStatusCode;
                }

                OpcUa_Buffer_SetEmpty(&pHttpInputStream->Buffer[pHttpInputStream->nBuffers]);
                uExpectedLength = OPCUA_HTTPS_MAX_RECV_BUFFER_LENGTH;
            }
            else
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadRequestTooLarge);
            }
        }

        /* receive data from the network */
        uStatus = OpcUa_HttpsStream_Receive(a_pInputStream, uExpectedLength, &uActualLength);

        /* check if all bytes were read or more are available. */
        if(OpcUa_IsEqual(OpcUa_GoodCallAgain))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: Received %u of %u requested bytes. More bytes available.\n", uActualLength, uExpectedLength);
            bReadAgain = OpcUa_False;
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: Received %u of %u requested bytes.\n", uActualLength, uExpectedLength);
            bReadAgain = OpcUa_True;
        }

        OpcUa_GotoErrorIfBad(uStatus);

        /************************* message part state machine ***************************************************************/
        switch(pHttpInputStream->State)
        {
            case OpcUa_HttpsStream_State_Empty:/*****************************************************************************/
            {
                pHttpInputStream->State = OpcUa_HttpsStream_State_StartLine;
            }
            /* fall thru */
            case OpcUa_HttpsStream_State_StartLine:/*************************************************************************/
            {
                /* read message start line */
                uStatus = OpcUa_HttpsStream_ReadLine(a_pInputStream, &sMessageLine);
                OpcUa_GotoErrorIfBad(uStatus);

                if(uStatus == OpcUa_GoodCallAgain)
                {
                    break;
                }

                if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Request)
                {
                    uStatus = OpcUa_HttpsRequestLine_Parse(&sMessageLine, &pHttpInputStream->StartLine.RequestLine);
                    OpcUa_GotoErrorIfBad(uStatus);

                    uStatus = OpcUa_HttpsStream_CheckRequestLine(pHttpInputStream);
                    OpcUa_GotoErrorIfBad(uStatus);
                }
                else if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Response)
                {
                    uStatus = OpcUa_HttpsStatusLine_Parse(&sMessageLine, &pHttpInputStream->StartLine.StatusLine);
                    OpcUa_GotoErrorIfBad(uStatus);
                }

                pHttpInputStream->State = OpcUa_HttpsStream_State_Headers;
            }
            /* fall thru */
            case OpcUa_HttpsStream_State_Headers:/***************************************************************************/
            {
                for(;;)
                {
                    uStatus = OpcUa_HttpsStream_ReadLine(a_pInputStream, &sMessageLine);
                    OpcUa_GotoErrorIfBad(uStatus);

                    if(uStatus == OpcUa_GoodCallAgain)
                    {
                        break;
                    }

                    if(OpcUa_String_IsEmpty(&sMessageLine))
                    {
                        break;
                    }

                    uStatus = OpcUa_HttpsHeader_Parse(&sMessageLine, &pMessageHeader);
                    OpcUa_GotoErrorIfBad(uStatus);

                    uStatus = OpcUa_HttpsHeaderCollection_AddHeader(pHttpInputStream->Headers, pMessageHeader);
                    if(OpcUa_IsBad(uStatus))
                    {
                        OpcUa_HttpsHeader_Delete(&pMessageHeader);
                        OpcUa_GotoError;
                    }
                }

                if(uStatus == OpcUa_GoodCallAgain)
                {
                    break;
                }

                uStatus = OpcUa_HttpsStream_ProcessHeaders(a_pInputStream);
                OpcUa_GotoErrorIfBad(uStatus);

                if(pHttpInputStream->iContentLength < 0)
                {
                    pHttpInputStream->iCurrentChunkLength = -1;
                }

                if(pHttpInputStream->MessageType == OpcUa_HttpsStream_MessageType_Response)
                {
                    /* response is being received, check whether the message body is allowed */
                    if((pHttpInputStream->StartLine.StatusLine.StatusCode >= 100) &&
                       (pHttpInputStream->StartLine.StatusLine.StatusCode <  200))
                    {
                        /* informational response (1xx) */
                        pHttpInputStream->State = OpcUa_HttpsStream_State_MessageComplete;
                    }

                    if((pHttpInputStream->StartLine.StatusLine.StatusCode == 204) ||
                       (pHttpInputStream->StartLine.StatusLine.StatusCode == 304))
                    {
                        /* no content (204) or not modified response (304) */
                        pHttpInputStream->State = OpcUa_HttpsStream_State_MessageComplete;
                    }
                }

                if(pHttpInputStream->State == OpcUa_HttpsStream_State_MessageComplete)
                {
                    pHttpInputStream->iContentLength = 0;
                }

                if(pHttpInputStream->iContentLength != 0)
                {
                    pHttpInputStream->State = OpcUa_HttpsStream_State_Body;

                    /* subtract header */
                    pHttpInputStream->iCurrentChunkDataReceived = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData - pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Position;
                    uActualLength = 0;
                }
            }
            /* fall thru */
            case OpcUa_HttpsStream_State_Body:/******************************************************************************/
            {
OpcUa_HttpsStream_State_Body:
                if(pHttpInputStream->iContentLength > 0)
                {
                    /* no chunking - old implementation */
                    pHttpInputStream->iCurrentChunkDataReceived += uActualLength;
                    uActualLength = 0;

                    if(uStatus == OpcUa_GoodCallAgain)
                    {
                        break;
                    }

                    if(pHttpInputStream->iCurrentChunkDataReceived < pHttpInputStream->iContentLength)
                    {
                        uStatus = OpcUa_GoodCallAgain;
                        break;
                    }
                }
                else if(pHttpInputStream->iContentLength < 0)
                {
                    /* chunking */
                    pHttpInputStream->iCurrentChunkDataReceived += uActualLength;
                    uActualLength = 0;

                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: %i body bytes in buffer.\n", pHttpInputStream->iCurrentChunkDataReceived);

                    if(pHttpInputStream->iCurrentChunkLength < 0)
                    {
                        /* last chunk was complete - parse next chunk header */
                        uStatus = OpcUa_HttpsStream_ReadChunkLength( a_pInputStream, &pHttpInputStream->iCurrentChunkLength);
                        OpcUa_GotoErrorIfBad(uStatus);

                        if(uStatus == OpcUa_GoodCallAgain)
                        {
                            break;
                        }

                        pHttpInputStream->iCurrentChunkDataReceived = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData - pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Position;

                    }

                    if(pHttpInputStream->iCurrentChunkDataReceived < pHttpInputStream->iCurrentChunkLength)
                    {
                        /* CHUNK NOT COMPLETE - need more data */
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: Chunk of size %i bytes, %i bytes received. (%u bytes remaining)\n", pHttpInputStream->iCurrentChunkLength, pHttpInputStream->iCurrentChunkDataReceived, pHttpInputStream->iCurrentChunkLength - pHttpInputStream->iCurrentChunkDataReceived);
                        uStatus = OpcUa_GoodCallAgain;
                        break;
                    }
                    else
                    {
                        OpcUa_UInt32 uDataRemaining;

                        /* LAST CHUNK COMPLETE */
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: Chunk of size %i bytes complete.\n", pHttpInputStream->iCurrentChunkLength);

                        /* prepare for reading footer lines */
                        uDataRemaining = pHttpInputStream->iCurrentChunkDataReceived - pHttpInputStream->iCurrentChunkLength;
                        pHttpInputStream->Buffer[pHttpInputStream->nBuffers].Position = pHttpInputStream->Buffer[pHttpInputStream->nBuffers].EndOfData - uDataRemaining;

                        pHttpInputStream->nCurrentReadBuffer = pHttpInputStream->nBuffers;
                        pHttpInputStream->State = OpcUa_HttpsStream_State_Footers;
                        goto OpcUa_HttpsStream_State_Footers;
                    }
                }
            }
            /* fall thru */
            case OpcUa_HttpsStream_State_MessageComplete:/*******************************************************************/
            {
                bReadAgain = OpcUa_False;
                break;
            }
            case OpcUa_HttpsStream_State_Footers:/*******************************************************************/
            {
OpcUa_HttpsStream_State_Footers:
                for(;;)
                {
                    uStatus = OpcUa_HttpsStream_ReadLine(a_pInputStream, &sMessageLine);
                    OpcUa_GotoErrorIfBad(uStatus);

                    if(uStatus == OpcUa_GoodCallAgain)
                    {
                        break;
                    }

                    if(OpcUa_String_IsEmpty(&sMessageLine))
                    {
                        break;
                    }
                }

                if(uStatus == OpcUa_GoodCallAgain)
                {
                    break;
                }

                if(pHttpInputStream->iCurrentChunkLength == 0)
                {
                    /* chunk size 0 */
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_HttpsStream_DataReady: Data in %u buffers complete.\n", pHttpInputStream->nBuffers + 1);
                    pHttpInputStream->State = OpcUa_HttpsStream_State_MessageComplete;

                    /* reset state variables to prepare stream for reading */
                    pHttpInputStream->Buffer[0].Position = 0;
                    pHttpInputStream->nCurrentReadBuffer = 0;
                    uStatus = OpcUa_Good;
                    bReadAgain = OpcUa_False;
                    break;
                }

                pHttpInputStream->iCurrentChunkLength = -1;
                pHttpInputStream->iCurrentChunkDataReceived = 0;
                pHttpInputStream->State = OpcUa_HttpsStream_State_Body;
                goto OpcUa_HttpsStream_State_Body;
            }
            default:/********************************************************************************************************/
            {
                uStatus = OpcUa_BadInternalError;
                break;
            }
        }
    }
    while(bReadAgain);

    OpcUa_String_Clear(&sMessageLine);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_String_Clear(&sMessageLine);

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_HTTPSAPI */
