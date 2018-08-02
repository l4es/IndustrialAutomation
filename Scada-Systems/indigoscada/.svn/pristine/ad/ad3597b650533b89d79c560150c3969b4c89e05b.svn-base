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

#ifndef _OpcUa_Stream_H_
#define _OpcUa_Stream_H_ 1

#include <opcua_buffer.h>

OPCUA_BEGIN_EXTERN_C

/** @brief a simple list of buffers to be used by AddToSendQueue */
typedef struct _OpcUa_BufferList OpcUa_BufferList;

struct _OpcUa_BufferList
{
    OpcUa_Buffer        Buffer;
    OpcUa_BufferList*   pNext;
};

struct _OpcUa_Stream;

/**
  @brief An input stream.
*/
typedef struct _OpcUa_Stream OpcUa_InputStream;

/**
  @brief An output stream.
*/
typedef struct _OpcUa_Stream OpcUa_OutputStream;

/**
  @brief Reads data from the stream.

  If no data is available then count is set to zero.
  If at the end of the stream then return code is 'BadEndOfStream'.

  The callback must be provided for non-blocking streams.
  If if the stream is non-blocking no data is currently available then return
  code is 'BadStreamNoDataAvailable'.

  @param istrm        [in]     The stream.
  @param buffer       [in]     The buffer to copy the data into.
  @param count        [in/out] The length of the buffer/amount of data copied.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_Read(
    OpcUa_InputStream*             istrm,
    OpcUa_Byte*                    buffer,
    OpcUa_UInt32*                  count);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnRead)(
    OpcUa_InputStream*             istrm,
    OpcUa_Byte*                    buffer,
    OpcUa_UInt32*                  count);

/**
  @brief Writes data to the stream.

  @param ostrm  [in] The stream.
  @param buffer [in] The buffer to copy the data from.
  @param count  [in] The amount of data to write.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_Write(
    OpcUa_OutputStream* ostrm,
    OpcUa_Byte*         buffer,
    OpcUa_UInt32        count);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnWrite)(
    OpcUa_OutputStream* ostrm,
    OpcUa_Byte*         buffer,
    OpcUa_UInt32        count);

/**
  @brief Flushes any data in the output buffer.

  @param ostrm    [in] The stream.
  @param lastCall [in] Whether this is the last time data will be written to the stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_Flush(
    OpcUa_OutputStream* ostrm,
    OpcUa_Boolean       lastCall);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnFlush)(
    OpcUa_OutputStream* ostrm,
    OpcUa_Boolean       lastCall);

/**
  @brief Closes a stream and releases all resources allocated to it.

  During normal operation all streams must be closed. Deleting a stream without
  closing it could have unexpected side effects.

  @param strm [in] The stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_Close(
    struct _OpcUa_Stream* strm);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnClose)(
    struct _OpcUa_Stream* strm);

/**
  @brief Frees the stream structure.

  This function aborts all I/O operations and frees all memory. It should be
  used only to clean up when the stream is not needed.

  @param strm [in] The stream to free.
*/
OPCUA_EXPORT OpcUa_Void OpcUa_Stream_Delete(
    struct _OpcUa_Stream** strm);

typedef OpcUa_Void (OpcUa_Stream_PfnDelete)(
    struct _OpcUa_Stream** strm);

/**
  @brief Returns the current position in the stream.

  Returns the number of bytes written to/read from the stream even if the stream
  does not support SetPosition.

  @param strm     [in]  The stream.
  @param position [out] The current position.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_GetPosition(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32*         position);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnGetPosition)(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32*         position);

/**
  @brief Sets the current position in the stream.

  Returns 'BadNotSupported' for streams that do not support seeking.

  Some streams will support partial seeks within the most recent buffer. If the
  position is set to 'Start' then the position should be set to the start of
  available data. The callers can use GetPosition to determine the absolute position
  in the buffer.

  @param strm     [in] The stream.
  @param position [in] The new position.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_SetPosition(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32          position);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnSetPosition)(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32          position);

/**
  @brief Get the internal buffer size of the stream.

  Returns 'BadNotSupported' for streams that do not support this operation.

  Some streams use fixed internal buffer sizes. Output streams cut messages
  at this border and it is the maximum length of incoming messages. Streams
  with virtually unlimited bufferspace return infinite.

  @param strm     [in] The stream.
  @param pLength [out] The maximum length of the chunk.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_GetChunkLength(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32*         pLength);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnGetChunkLength)(
    struct _OpcUa_Stream* strm,
    OpcUa_UInt32*         pLength);


/**
  @brief Get the internal buffer of the stream.

  Returns 'BadNotSupported' for streams that do not support this operation.

  @param strm   [in] The stream.
  @param buffer [in] The buffer to be attached to the stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_AttachBuffer(
    struct _OpcUa_Stream*   strm,
    OpcUa_Buffer*           buffer);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnAttachBuffer)(
    struct _OpcUa_Stream*   strm,
    OpcUa_Buffer*           buffer);

/**
  @brief Set the internal buffer of the stream.

  Returns 'BadNotSupported' for streams that do not support this operation.

  Some streams

  @param strm     [in] The stream.
  @param pBuffer [out] The buffer of the stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Stream_DetachBuffer(
    struct _OpcUa_Stream*   strm,
    OpcUa_Buffer*           pBuffer);

typedef OpcUa_StatusCode (OpcUa_Stream_PfnDetachBuffer)(
    struct _OpcUa_Stream*   strm,
    OpcUa_Buffer*           pBuffer);

/**
  @brief A generic stream.
*/
typedef struct _OpcUa_Stream
{
    /*! @brief The type of stream (Input or Output). */
    OpcUa_Int32 Type;

    /*! @brief An opaque handle that contain data specific to the stream implementation. */
    OpcUa_Handle Handle;

    /*! @brief Returns the current position in the stream. */
    OpcUa_Stream_PfnGetPosition* GetPosition;

    /*! @brief Sets the current position in the stream. */
    OpcUa_Stream_PfnSetPosition* SetPosition;

    /*! @brief Returns the internal chunk length. */
    OpcUa_Stream_PfnGetChunkLength* GetChunkLength;

    /*! @brief Detach the internal buffer. */
    OpcUa_Stream_PfnDetachBuffer* DetachBuffer;

    /*! @brief Attach an external buffer. */
    OpcUa_Stream_PfnAttachBuffer* AttachBuffer;

    /*! @brief Closes the stream. */
    OpcUa_Stream_PfnClose* Close;

    /*! @brief Frees the structure. */
    OpcUa_Stream_PfnDelete* Delete;

    /************************************************************/

    /*! @brief Only if the type of stream is OpcUa_StreamType_Input. */

    /*! @brief Reads data from the stream. */
    OpcUa_Stream_PfnRead* Read;

    /************************************************************/

    /*! @brief Only if the type of stream OpcUa_StreamType_Output. */

    /*! @brief Writes data to the stream. */
    OpcUa_Stream_PfnWrite* Write;

    /*! @brief Flushes data to the stream. */
    OpcUa_Stream_PfnFlush* Flush;

}
OpcUa_Stream;

/**
  @brief Set the stream position to the start of the available data.
*/
#define OpcUa_StreamPosition_Start OpcUa_BufferPosition_Start

/**
  @brief Set the stream position to the end of the available data.
*/
#define OpcUa_StreamPosition_End   OpcUa_BufferPosition_End

#define OpcUa_StreamType_Input  0x01
#define OpcUa_StreamType_Output 0x02

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Stream_H_ */
