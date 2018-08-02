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

#ifndef _OpcUa_Https_Internal_H_
#define _OpcUa_Https_Internal_H_ 1

#ifdef OPCUA_HAVE_HTTPSAPI

#include <opcua_buffer.h>

OPCUA_BEGIN_EXTERN_C

/** @brief HTTP headers are copied from the stream (alternatively only attached). */
#define OPCUA_HTTPS_COPYHEADERS                                     OPCUA_CONFIG_NO

/** @brief Allow HTTP GET request to pass this layer. */
#define OPCUA_HTTPS_ALLOW_GET                                       OPCUA_CONFIG_NO

/** @brief Allow HTTP/1.0 request to pass this layer. */
#define OPCUA_HTTPS_ALLOW_HTTP10                                    OPCUA_CONFIG_NO

/*============================================================================
 * Used HTTP version
 *===========================================================================*/
#define OPCUA_HTTP_VERSION                                          "HTTP/1.1"

 /*============================================================================
 * OPCUA-SecurityPolicy
 *===========================================================================*/
#define OPCUA_HTTPS_SECURITYPOLICYHEADER                            "OPCUA-SecurityPolicy"

/*============================================================================
 * HTTP Status Code Definitions
 *===========================================================================*/
#define OPCUA_HTTP_STATUS_OK                                        200
#define OPCUA_HTTP_STATUS_OK_TEXT                                   "OK"
#define OPCUA_HTTP_STATUS_NO_CONTENT                                200
#define OPCUA_HTTP_STATUS_NO_CONTENT_TEXT                           "No Content"
#define OPCUA_HTTP_STATUS_MULTIPLE_CHOICES                          300
#define OPCUA_HTTP_STATUS_MOVED_PERMANENTLY                         301
#define OPCUA_HTTP_STATUS_MOVED_TEMPORARILY                         302
#define OPCUA_HTTP_STATUS_SEE_OTHER                                 303
#define OPCUA_HTTP_STATUS_NOT_MODIFIED                              304
#define OPCUA_HTTP_STATUS_USE_PROXY                                 305
#define OPCUA_HTTP_STATUS_TEMPORARY_REDIRECT                        307
#define OPCUA_HTTP_STATUS_BAD_REQUEST                               400
#define OPCUA_HTTP_STATUS_BAD_REQUEST_TEXT                          "Bad Request"
#define OPCUA_HTTP_STATUS_UNAUTHORIZED                              401
#define OPCUA_HTTP_STATUS_PAYMENT_REQUIRED                          402
#define OPCUA_HTTP_STATUS_FORBIDDEN                                 403
#define OPCUA_HTTP_STATUS_NOT_FOUND                                 404
#define OPCUA_HTTP_STATUS_NOT_FOUND_TEXT                            "Not Found"
#define OPCUA_HTTP_STATUS_METHOD_NOT_ALLOWED                        405
#define OPCUA_HTTP_STATUS_METHOD_NOT_ALLOWED_TEXT                   "Method Not Allowed"
#define OPCUA_HTTP_STATUS_NOT_ACCEPTABLE                            406
#define OPCUA_HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED             407
#define OPCUA_HTTP_STATUS_REQUEST_TIMEOUT                           408
#define OPCUA_HTTP_STATUS_CONFLICT                                  409
#define OPCUA_HTTP_STATUS_GONE                                      410
#define OPCUA_HTTP_STATUS_LENGTH_REQUIRED                           411
#define OPCUA_HTTP_STATUS_PRECONDITION_FAILED                       412
#define OPCUA_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE                  413
#define OPCUA_HTTP_STATUS_REQUEST_URI_TOO_LONG                      414
#define OPCUA_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE                    415
#define OPCUA_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_TEXT               "Unsupported Media Type"
#define OPCUA_HTTP_STATUS_UNPROCESSABLE_ENTITY                      422
#define OPCUA_HTTP_STATUS_INTERNAL_SERVER_ERROR                     500
#define OPCUA_HTTP_STATUS_INTERNAL_SERVER_ERROR_TEXT                "Internal Server Error"
#define OPCUA_HTTP_STATUS_NOT_IMPLEMENTED                           501
#define OPCUA_HTTP_STATUS_NOT_IMPLEMENTED_TEXT                      "Not Implemented"
#define OPCUA_HTTP_STATUS_BAD_GATEWAY                               502
#define OPCUA_HTTP_STATUS_SERVICE_UNAVAILABLE                       503
#define OPCUA_HTTP_STATUS_GATEWAY_TIMEOUT                           504
#define OPCUA_HTTP_STATUS_VERSION_NOT_SUPPORTED                     505

/*============================================================================
 * OpcUa_Https_WriteStringToBuffer
 *===========================================================================*/
/** @brief writes string bytes into a buffer */
#define OpcUa_Https_WriteStringToBuffer(xBuffer, xString)                   \
    OpcUa_Buffer_Write( xBuffer,                                            \
                        (OpcUa_Byte*)OpcUa_String_GetRawString(xString),    \
                        OpcUa_String_StrSize(xString));

/*============================================================================
 * OpcUa_HttpsHeader
 *===========================================================================*/
/** @brief Data structure that represents a single http header */
struct _OpcUa_HttpsHeader
{
    OpcUa_String    Name;
    OpcUa_String    Value;
};

typedef struct _OpcUa_HttpsHeader OpcUa_HttpsHeader;

OpcUa_Void OpcUa_HttpsHeader_Initialize(OpcUa_HttpsHeader* a_pHttpHeader);

OpcUa_StatusCode OpcUa_HttpsHeader_Create(
    OpcUa_String*       a_pHeaderName,
    OpcUa_String*       a_pHeaderValue,
    OpcUa_HttpsHeader** a_ppHttpHeader);

OpcUa_Void OpcUa_HttpsHeader_Clear(OpcUa_HttpsHeader* a_pValue);

OpcUa_Void OpcUa_HttpsHeader_Delete(OpcUa_HttpsHeader** a_ppHttpHeader);

OpcUa_StatusCode OpcUa_HttpsHeader_Serialize(
    OpcUa_HttpsHeader*  a_pHeader,
    OpcUa_Buffer*       a_pBuffer);

OpcUa_StatusCode OpcUa_HttpsHeader_Parse(
    OpcUa_String*       a_pMessageLine,
    OpcUa_HttpsHeader** a_ppHttpHeader);

/*============================================================================
 * OpcUa_HttpsRequestLine
 *===========================================================================*/
/** @brief Data structure that represents an http request line */
struct _OpcUa_HttpsRequestLine
{
    OpcUa_String    RequestMethod;
    OpcUa_String    RequestUri;
    OpcUa_String    HttpVersion;
};

typedef struct _OpcUa_HttpsRequestLine OpcUa_HttpsRequestLine;

OpcUa_Void OpcUa_HttpsRequestLine_Initialize(OpcUa_HttpsRequestLine* a_pValue);

OpcUa_Void OpcUa_HttpsRequestLine_Clear(OpcUa_HttpsRequestLine* a_pValue);

OpcUa_StatusCode OpcUa_HttpsRequestLine_Serialize(
    OpcUa_HttpsRequestLine* a_pRequestLine,
    OpcUa_Buffer*           a_pBuffer);

OpcUa_StatusCode OpcUa_HttpsRequestLine_Parse(
    OpcUa_String*            a_pMessageLine,
    OpcUa_HttpsRequestLine*  a_pRequestLine);

/*============================================================================
 * OpcUa_HttpsStatusLine
 *===========================================================================*/
/** @brief Data structure that represents an http status line */
struct _OpcUa_HttpsStatusLine
{
    OpcUa_String    HttpVersion;
    OpcUa_UInt32    StatusCode;
    OpcUa_String    ReasonPhrase;
};

typedef struct _OpcUa_HttpsStatusLine OpcUa_HttpsStatusLine;

OpcUa_Void OpcUa_HttpsStatusLine_Initialize(OpcUa_HttpsStatusLine* a_pValue);

OpcUa_Void OpcUa_HttpsStatusLine_Clear(OpcUa_HttpsStatusLine* a_pValue);

OpcUa_StatusCode OpcUa_HttpsStatusLine_Serialize(
    OpcUa_HttpsStatusLine*  a_pStatusLine,
    OpcUa_Buffer*           a_pBuffer);

OpcUa_StatusCode OpcUa_HttpsStatusLine_Parse(
    OpcUa_String*           a_pMessageLine,
    OpcUa_HttpsStatusLine*  a_pStatusLine);

typedef struct _OpcUa_List OpcUa_HttpsHeaderCollection;

#define OpcUa_HttpsHeaderCollection_Create(xCollection) OpcUa_List_Create((OpcUa_List**)xCollection);

OpcUa_Void OpcUa_HttpsHeaderCollection_Clear(OpcUa_HttpsHeaderCollection* a_pHeaderCollection);

OpcUa_Void OpcUa_HttpsHeaderCollection_Delete(OpcUa_HttpsHeaderCollection** a_ppHeaderCollection);

OpcUa_StatusCode OpcUa_HttpsHeaderCollection_AddHeader(
    OpcUa_HttpsHeaderCollection* a_pHeaderCollection,
    OpcUa_HttpsHeader*           a_pHttpHeader);

OpcUa_StatusCode OpcUa_HttpsHeaderCollection_FindHeader(
    OpcUa_HttpsHeaderCollection* a_pHeaderCollection,
    OpcUa_String*                a_pHeaderName,
    OpcUa_HttpsHeader**          a_ppHttpHeader);

OpcUa_StatusCode OpcUa_HttpsHeaderCollection_GetValue(
    OpcUa_HttpsHeaderCollection* a_pHeaderCollection,
    OpcUa_String*                a_pHeaderName,
    OpcUa_Boolean                a_bGiveCopy,
    OpcUa_String*                a_pHeaderValue);

OpcUa_StatusCode OpcUa_HttpsHeaderCollection_SetValue(
    OpcUa_HttpsHeaderCollection* a_pHeaderCollection,
    OpcUa_String*                a_pHeaderName,
    OpcUa_String*                a_pHeaderValue);

OpcUa_StatusCode OpcUa_HttpsHeaderCollection_Serialize(
    OpcUa_HttpsHeaderCollection* a_pHeaderCollection,
    OpcUa_Buffer*                a_pBuffer);

OpcUa_StatusCode OpcUa_HttpsHeaderCollection_Parse(
    OpcUa_String*                 a_pHeaderString,
    OpcUa_HttpsHeaderCollection** a_ppHeaderCollection);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_HTTPSAPI */
#endif /* _OpcUa_Https_Internal_H_ */
