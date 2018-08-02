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

#ifndef _OpcUa_HttpsStream_H_
#define _OpcUa_HttpsStream_H_ 1

#ifdef OPCUA_HAVE_HTTPSAPI

#include <opcua_stream.h>

OPCUA_BEGIN_EXTERN_C

#define OPCUA_HTTP_REASON_OK                            "OK"
#define OPCUA_HTTP_METHOD_POST                          "POST"
#define OPCUA_HTTP_METHOD_GET                           "GET"

#define OPCUA_HTTPSSTREAM_OUTPUT_HAS_HEADERCOLLECTION   OPCUA_CONFIG_NO

#define OPCUA_HTTPSSTREAM_ALLOW_OPTIONS_METHOD          OPCUA_CONFIG_NO

#define OPCUA_HTTPSTREAM_AUTOMATIC_CHUNKING             OPCUA_CONFIG_YES

/*============================================================================
 * OpcUa_HttpsStream_MessageType
 *===========================================================================*/
/** @brief Types of http protocol messages. */
enum _OpcUa_HttpsStream_MessageType
{
    /** @brief Unknown Message Type */
    OpcUa_HttpsStream_MessageType_Unknown,

    /** @brief Invalid Message Type */
    OpcUa_HttpsStream_MessageType_Invalid,

    /** @brief Request message is sent from a client to a server */
    OpcUa_HttpsStream_MessageType_Request,

    /** @brief Server responds to a client with a response message */
    OpcUa_HttpsStream_MessageType_Response
};
typedef enum _OpcUa_HttpsStream_MessageType OpcUa_HttpsStream_MessageType;

/*============================================================================
 * OpcUa_HttpsStream_State
 *===========================================================================*/
/** @brief States for http protocol messages. */
enum _OpcUa_HttpsStream_State
{
    /** @brief Stream has no data. */
    OpcUa_HttpsStream_State_Empty,
    /** @brief The start line has begun but not finished. */
    OpcUa_HttpsStream_State_StartLine,
    /** @brief Message headers are being received. */
    OpcUa_HttpsStream_State_Headers,
    /** @brief Message footers are being processed. */
    OpcUa_HttpsStream_State_Footers,
    /** @brief Message is body being received. */
    OpcUa_HttpsStream_State_Body,
    /** @brief The complete message has been received. */
    OpcUa_HttpsStream_State_MessageComplete
};

typedef enum _OpcUa_HttpsStream_State OpcUa_HttpsStream_State;

/*============================================================================
 * OpcUa_HttpsStream_Method
 *===========================================================================*/
/** @brief Methods of http calls. */
enum _OpcUa_HttpsStream_Method
{
    /** @brief Stream has no data. */
    OpcUa_HttpsStream_Method_Invalid,
    /** @brief http get method. */
    OpcUa_HttpsStream_Method_Get,
    /** @brief http post method. */
    OpcUa_HttpsStream_Method_Head,
    /** @brief http post method. */
    OpcUa_HttpsStream_Method_Post,
    /** @brief http options method. */
    OpcUa_HttpsStream_Method_Options,
    /** @brief Not a request. */
    OpcUa_HttpsStream_Method_Response
};

typedef enum _OpcUa_HttpsStream_Method OpcUa_HttpsStream_Method;

/*============================================================================
 * OpcUa_HttpsStream_CreateInput
 *===========================================================================*/
/** @brief Allocates a new stream to read a message from a socket.
 *  @param a_hSocket        [in]  The socket to read from.
 *  @param a_eMessageType   [in]  Type of the message.
 *  @param a_ppInputStream  [out] The new input stream.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsStream_CreateInput(
    OpcUa_Socket                    a_hSocket,
    OpcUa_HttpsStream_MessageType   a_eMessageType,
    OpcUa_InputStream**             a_ppInputStream);

/*============================================================================
 * OpcUa_HttpsStream_CreateRequest
 *===========================================================================*/
/** @brief Allocates a new stream to write a message to a socket.
 *  @param a_hSocket            [in]  The socket to write to.
 *  @param a_sRequestMethod     [in]  The method to be applied to the resource.
 *  @param a_sRequestUri        [in]  An uniform identifier of the resource.
 *  @param a_sRequestHeaders    [in]  The message headers.
 *  @param a_ppOutputStream     [out] The new output stream.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsStream_CreateRequest(
    OpcUa_Socket                            a_hSocket,
    OpcUa_StringA                           a_sRequestMethod,
    OpcUa_StringA                           a_sRequestUri,
    OpcUa_StringA                           a_sRequestHeaders,
    OpcUa_OutputStream**                    a_ppOutputStream);

/*============================================================================
 * OpcUa_HttpsStream_CreateResponse
 *===========================================================================*/
/** @brief Allocates a new stream to write a message to a socket.
 *  @param a_hSocket            [in]  The socket to write to.
 *  @param a_uStatusCode        [in]  The status code of the response.
 *  @param a_sReasonPhrase      [in]  The textual description of the status code.
 *  @param a_sResponseHeaders   [in]  The response headers.
 *  @param a_ppOutputStream     [out] The new output stream.
 */
OPCUA_EXPORT OpcUa_StatusCode OpcUa_HttpsStream_CreateResponse(
    OpcUa_Socket                            a_hSocket,
    OpcUa_UInt32                            a_uStatusCode,
    OpcUa_StringA                           a_sReasonPhrase,
    OpcUa_StringA                           a_sResponseHeaders,
    OpcUa_Handle                            a_hConnection,
    OpcUa_OutputStream**                    a_ppOutputStream);

/*============================================================================
 * OpcUa_Stream_Close
 *===========================================================================*/
/** @brief Closes the stream. To be called before delete. Causes the sending
 *  of the buffered data if the parameter is an output stream.
 */
OpcUa_StatusCode OpcUa_HttpsStream_Close(OpcUa_Stream* a_pStream);

/*============================================================================
 * OpcUa_Stream_Delete
 *===========================================================================*/
/** @brief Delete the stream and all associated resources. */
OpcUa_Void OpcUa_HttpsStream_Delete(OpcUa_Stream** a_ppStream);

/*============================================================================
 * OpcUa_HttpsStream_GetMessageType
 *===========================================================================*/
/** @brief Gets the type of the message being sent/received */
OpcUa_StatusCode OpcUa_HttpsStream_GetMessageType(
    OpcUa_Stream*                    a_pStream,
    OpcUa_HttpsStream_MessageType*   a_pMessageType);

/*============================================================================
 * OpcUa_HttpsStream_GetMessageType
 *===========================================================================*/
/** @brief Gets the method type of the message. */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_HttpsStream_GetMethod(
    OpcUa_Stream*                   a_pStream,
    OpcUa_HttpsStream_Method*       a_pMethod);

/*============================================================================
 * OpcUa_HttpsStream_GetRequestUri
 *===========================================================================*/
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_HttpsStream_GetRequestUri(
    OpcUa_InputStream*  a_pInputStream,
    OpcUa_Boolean       a_bCopy,
    OpcUa_String*       a_pRequestUri);

/*============================================================================
* OpcUa_HttpsStream_GetSocket
*===========================================================================*/
/** @brief Gets the communication socket associated with the stream */
OpcUa_StatusCode OpcUa_HttpsStream_GetSocket(
    OpcUa_Stream*   a_pStream,
    OpcUa_Socket*   a_pSocket);

/*============================================================================
 * OpcUa_HttpsStream_SetHeader
 *===========================================================================*/
/** @brief Sets the value of the specified header */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_HttpsStream_SetHeader(
    OpcUa_Stream*   a_pStream,
    OpcUa_String*   a_pHeaderName,
    OpcUa_String*   a_pHeaderValue);

/*============================================================================
 * OpcUa_HttpsStream_GetHeader
 *===========================================================================*/
/** @brief Gets the value of the specified header */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_HttpsStream_GetHeader(
    OpcUa_Stream*   a_pStream,
    OpcUa_String*   a_pHeaderName,
    OpcUa_Boolean   a_bGiveCopy,
    OpcUa_String*   a_pHeaderValue);

/*============================================================================
 * OpcUa_HttpsStream_GetStatusCode
 *===========================================================================*/
/** @brief Gets the status code of the response if any */
OpcUa_StatusCode OpcUa_HttpsStream_GetStatusCode(
    OpcUa_Stream*   a_pStream,
    OpcUa_UInt32*   a_pStatusCode);

/*============================================================================
 * OpcUa_HttpsStream_GetConnection
 *===========================================================================*/
/** @brief Gets the handle of the underlying connection */
OpcUa_StatusCode OpcUa_HttpsStream_GetConnection(
    OpcUa_OutputStream* a_pOutputStream,
    OpcUa_Handle*       a_pConnection);

/*============================================================================
* OpcUa_HttpsStream_GetConnection
*===========================================================================*/
/** @brief Gets the current state of the stream */
OpcUa_StatusCode OpcUa_HttpsStream_GetState(
    OpcUa_InputStream*       a_pInputStream,
    OpcUa_HttpsStream_State* a_pStreamState);

/*============================================================================
* OpcUa_HttpsStream_SetSocket
*===========================================================================*/
/** @brief Sets the socket handle of the stream. */
OpcUa_StatusCode OpcUa_HttpsStream_SetSocket(
    OpcUa_OutputStream*      a_pOutputStream,
    OpcUa_Socket             a_pSocket);

/*============================================================================
 * OpcUa_HttpsStream_DataReady
 *===========================================================================*/
/** @brief A lower layer tells the stream, that a read operation is possible. */
OpcUa_StatusCode OpcUa_HttpsStream_DataReady(OpcUa_InputStream* a_pInputStream);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_HTTPSAPI */
#endif /* _OpcUa_HttpsStream_H_ */
