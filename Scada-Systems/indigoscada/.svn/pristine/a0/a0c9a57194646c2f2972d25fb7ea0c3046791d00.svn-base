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

/* core */
#include <opcua.h>
#include <opcua_guid.h>
#include <opcua_mutex.h>

/* types */
#include <opcua_builtintypes.h>
#include <opcua_encodeableobject.h>
#include <opcua_p_binary.h>

/* self */
#include <opcua_binaryencoder.h>
#include <opcua_binaryencoderinternal.h>

/*============================================================================
 * OpcUa_BinaryDecoder
 *
 * Stores the state of a memory stream.
 *
 * Istrm  - The stream to write data to.
 * Closed - Whether the encoder has been closed.
 *===========================================================================*/
typedef struct _OpcUa_BinaryDecoder
{
    OpcUa_UInt32          SanityCheck;
    OpcUa_InputStream*    Istrm;
    OpcUa_MessageContext* Context;
    OpcUa_Boolean         Closed;
    OpcUa_UInt32          RecursionDepth;
}
OpcUa_BinaryDecoder;

/*============================================================================
 * OpcUa_BinaryDecoder_SanityCheck
 *
 * The sanity check reduces the likely hood of a fatal error caused by
 * casting a bad handle to a OpcUa_BinaryDecoder structure. The value
 * was created by generating a new guid and taking the first for bytes.
 *===========================================================================*/
#define OpcUa_BinaryDecoder_SanityCheck 0x032150D3

/*============================================================================
 * OpcUa_BinaryDecoder_VerifyState
 *===========================================================================*/
#define OpcUa_BinaryDecoder_VerifyState(xType) \
OpcUa_ReturnErrorIfInvalidObject(OpcUa_BinaryDecoder, a_pDecoder, Read##xType); \
pHandle = (OpcUa_BinaryDecoder*)a_pDecoder->Handle; \
OpcUa_ReturnErrorIfArgumentNull(pHandle); \
OpcUa_ReturnErrorIfTrue(pHandle->Closed, OpcUa_BadInvalidState);

/*============================================================================
 * OpcUa_Decode_FixedLengthType
 *===========================================================================*/
#define OpcUa_Decode_FixedLengthType(xType) \
{ \
    OpcUa_##xType##_Wire oValue; \
    OpcUa_UInt32 uBytesRead = sizeof(OpcUa_##xType##_Wire); \
 \
    OpcUa_##xType##_Initialize(a_pValue); \
 \
    uStatus = a_pIstrm->Read(a_pIstrm, (OpcUa_Byte*)&oValue, &uBytesRead); \
    OpcUa_GotoErrorIfBad(uStatus); \
 \
    if (uBytesRead != sizeof(OpcUa_##xType##_Wire)) \
    { \
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported); \
    } \
 \
    uStatus = OpcUa_##xType##_P_WireToNative(a_pValue, &oValue); \
    OpcUa_GotoErrorIfBad(uStatus); \
}

/*============================================================================
 * Implement_OpcUa_BinaryDecoder_ReadFixedLengthType
 *===========================================================================*/
#define Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(xType) \
OpcUa_BinaryDecoder* pHandle = OpcUa_Null; \
\
OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_Read" #xType); \
\
OpcUa_ReturnErrorIfArgumentNull(a_pDecoder); \
OpcUa_ReturnErrorIfArgumentNull(a_pValue); \
OpcUa_ReferenceParameter(a_sFieldName); \
OpcUa_BinaryDecoder_VerifyState(xType); \
\
uStatus = OpcUa_##xType##_BinaryDecode(a_pValue, pHandle->Istrm); \
OpcUa_GotoErrorIfBad(uStatus); \
\
OpcUa_ReturnStatusCode; \
OpcUa_BeginErrorHandling; \
\
    OpcUa_##xType##_Clear(a_pValue); \
\
OpcUa_FinishErrorHandling;

/*============================================================================
 * OpcUa_Decode_ArrayType
 *===========================================================================*/
#define OpcUa_Decode_ArrayType(xType) \
{ \
    OpcUa_Int32 ii = 0; \
    OpcUa_Int32 iLength = -1; \
    OpcUa_##xType* pArray = OpcUa_Null; \
    \
    *a_ppArray = OpcUa_Null; \
    *a_pCount  = 0; \
    \
    uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &iLength); \
    OpcUa_GotoErrorIfBad(uStatus); \
    \
    if (iLength == -1) \
    { \
        OpcUa_ReturnStatusCode; \
    } \
    \
    if (pHandle->Context->MaxArrayLength > 0 && (OpcUa_UInt32)iLength > pHandle->Context->MaxArrayLength) \
    { \
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded); \
    } \
    \
    if ((OpcUa_UInt32)iLength > pHandle->Context->MaxMessageLength/sizeof(OpcUa_##xType)) \
    { \
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded); \
    } \
    \
    pArray = (OpcUa_##xType*)OpcUa_Alloc(sizeof(OpcUa_##xType)*iLength); \
    OpcUa_GotoErrorIfAllocFailed(pArray); \
    OpcUa_MemSet(pArray, 0, sizeof(OpcUa_##xType)*iLength); \
    \
    *a_ppArray = pArray; \
    *a_pCount  = iLength; \
    \
    for (ii = 0; ii < iLength; ii++) \
    { \
        uStatus = OpcUa_BinaryDecoder_Read##xType(a_pDecoder, OpcUa_Null, &(pArray[ii])); \
        OpcUa_GotoErrorIfBad(uStatus); \
    } \
}

/*============================================================================
 * OpcUa_Clear_SimpleArrayType
 *===========================================================================*/
#define OpcUa_Clear_SimpleArrayType(xType) \
OpcUa_Free(*a_ppArray); \
*a_ppArray = OpcUa_Null; \
*a_pCount  = 0;

/*============================================================================
 * OpcUa_Clear_ComplexArrayType
 *===========================================================================*/
#define OpcUa_Clear_ComplexArrayType(xType) \
{ \
    OpcUa_Int32 ii = 0; \
    \
    for (ii = 0; ii < *a_pCount; ii++) \
    { \
        OpcUa_##xType##_Clear(&((*a_ppArray)[ii])); \
    } \
    \
    OpcUa_Clear_SimpleArrayType(xType); \
}

/*============================================================================
 * OpcUa_BinaryDecoder_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_Open(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_InputStream*     a_pIstrm,
    OpcUa_MessageContext*  a_pContext,
    OpcUa_Handle*          a_phDecodeContext)
{
    struct _OpcUa_Decoder*  pDecoderContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_Open");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pContext);
    OpcUa_ReturnErrorIfArgumentNull(a_phDecodeContext);
    OpcUa_ReturnErrorIfArgumentNull(((OpcUa_BinaryDecoder*)a_pDecoder->Handle));
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_BinaryDecoder, a_pDecoder, Open);

    *a_phDecodeContext = OpcUa_Null;

    OpcUa_GotoErrorIfTrue(!((OpcUa_BinaryDecoder*)a_pDecoder->Handle)->Closed, OpcUa_BadInvalidState);

    /* create handle */
    pDecoderContext = (struct _OpcUa_Decoder*)OpcUa_Alloc(sizeof(struct _OpcUa_Decoder));
    OpcUa_GotoErrorIfAllocFailed(pDecoderContext);
    OpcUa_MemCpy(pDecoderContext, sizeof(struct _OpcUa_Decoder), a_pDecoder, sizeof(struct _OpcUa_Decoder));

    pDecoderContext->Handle = OpcUa_Alloc(sizeof(OpcUa_BinaryDecoder));
    OpcUa_GotoErrorIfAllocFailed(pDecoderContext->Handle);

    ((OpcUa_BinaryDecoder*)pDecoderContext->Handle)->SanityCheck    = ((OpcUa_BinaryDecoder*)a_pDecoder->Handle)->SanityCheck;
    ((OpcUa_BinaryDecoder*)pDecoderContext->Handle)->Closed         = OpcUa_False;
    ((OpcUa_BinaryDecoder*)pDecoderContext->Handle)->Istrm          = a_pIstrm;
    ((OpcUa_BinaryDecoder*)pDecoderContext->Handle)->Context        = a_pContext;
    ((OpcUa_BinaryDecoder*)pDecoderContext->Handle)->RecursionDepth = 0;

    *a_phDecodeContext = pDecoderContext;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pDecoderContext != OpcUa_Null)
    {
        OpcUa_Free(pDecoderContext);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_Close(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_Handle*          a_phDecodeContext)
{
    struct _OpcUa_Decoder*  pDecoderContext = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_phDecodeContext);
    OpcUa_ReturnErrorIfArgumentNull(*a_phDecodeContext);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_BinaryDecoder, a_pDecoder, Close);

    pDecoderContext = (struct _OpcUa_Decoder*)*a_phDecodeContext;

    OpcUa_Free(pDecoderContext->Handle);
    OpcUa_Free(pDecoderContext);

    *a_phDecodeContext = OpcUa_Null;


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* nothing to do */

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_BinaryDecoder_Delete(
    struct _OpcUa_Decoder** a_ppDecoder)
{
    if (a_ppDecoder != OpcUa_Null && *a_ppDecoder != OpcUa_Null)
    {
        OpcUa_BinaryDecoder* pHandle = (OpcUa_BinaryDecoder*)(*a_ppDecoder)->Handle;
        OpcUa_Free(pHandle);

        OpcUa_Free(*a_ppDecoder);
        *a_ppDecoder = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_BinaryDecoder_PushNamespace
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_PushNamespace(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_String*          a_sNamespaceUri)
{
    OpcUa_ReferenceParameter(a_pDecoder);
    OpcUa_ReferenceParameter(a_sNamespaceUri);

    /* not used in the binary encoding */

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_BinaryDecoder_PopNamespace
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_PopNamespace(
    struct _OpcUa_Decoder* a_pDecoder)
{
    OpcUa_ReferenceParameter(a_pDecoder);

    /* not used in the binary encoding */

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Boolean_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Boolean_BinaryDecode(
    OpcUa_Boolean*     a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Boolean_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Boolean);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadBoolean
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadBoolean(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Boolean*         a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Boolean);
}

/*============================================================================
 * OpcUa_SByte_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SByte_BinaryDecode(
    OpcUa_SByte*       a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_SByte_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(SByte);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadSByte
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadSByte(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_SByte*           a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(SByte);
}

/*============================================================================
 * OpcUa_Byte_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Byte_BinaryDecode(
    OpcUa_Byte*        a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Byte_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Byte);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadByte
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadByte(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Byte*            a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Byte);
}

/*============================================================================
 * OpcUa_Int16_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int16_BinaryDecode(
    OpcUa_Int16*       a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Int16_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Int16);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadInt16
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadInt16(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Int16*           a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Int16);
}

/*============================================================================
 * OpcUa_UInt16_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt16_BinaryDecode(
    OpcUa_UInt16*      a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_UInt16_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(UInt16);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadUInt16
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadUInt16(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_UInt16*          a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(UInt16);
}

/*============================================================================
 * OpcUa_Int32_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int32_BinaryDecode(
    OpcUa_Int32*       a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Int32_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Int32);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadInt32
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadInt32(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Int32*           a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Int32);
}

/*============================================================================
 * OpcUa_UInt32_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt32_BinaryDecode(
    OpcUa_UInt32*      a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_UInt32_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(UInt32);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadUInt32
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadUInt32(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_UInt32*          a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(UInt32);
}

/*============================================================================
 * OpcUa_Int64_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int64_BinaryDecode(
    OpcUa_Int64*       a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Int64_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Int64);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadInt64
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadInt64(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Int64*           a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Int64);
}

/*============================================================================
 * OpcUa_Int64_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt64_BinaryDecode(
    OpcUa_UInt64*      a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_UInt64_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(UInt64);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadUInt64
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadUInt64(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_UInt64*          a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(UInt64);
}

/*============================================================================
 * OpcUa_Float_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Float_BinaryDecode(
    OpcUa_Float*       a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Float_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Float);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadFloat
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadFloat(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Float*           a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Float);
}

/*============================================================================
 * OpcUa_Double_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Double_BinaryDecode(
    OpcUa_Double*      a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Double_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Decode_FixedLengthType(Double);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDouble
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDouble(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Double*          a_pValue)
{
    Implement_OpcUa_BinaryDecoder_ReadFixedLengthType(Double);
}

/*============================================================================
 * OpcUa_String_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_String_BinaryDecode(
    OpcUa_String*      a_pValue,
    OpcUa_UInt32       a_nMaxStringLength,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_Int32 nLength = -1;
    OpcUa_UInt32 uBytesRead = 0;
    OpcUa_StringA pRawString = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_String_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_String_Initialize(a_pValue);

    /* decode length */
    uStatus = OpcUa_Int32_BinaryDecode(&nLength, a_pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check for null string */
    if (nLength == -1)
    {
        OpcUa_ReturnStatusCode;
    }

    /* check maximum string length */
    if (a_nMaxStringLength > 0 && (OpcUa_UInt32)nLength > a_nMaxStringLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
    }

    /* allocate bytes for string */
    pRawString = (OpcUa_StringA)OpcUa_Alloc(sizeof(OpcUa_Char_Wire)*(nLength+1));
    OpcUa_GotoErrorIfAllocFailed(pRawString);

    /* read bytes of string */
    uBytesRead = nLength;
    uStatus = a_pIstrm->Read(a_pIstrm, (OpcUa_Byte*)pRawString, &uBytesRead);
    OpcUa_GotoErrorIfBad(uStatus);

    if (uBytesRead != (OpcUa_UInt32)nLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadExpectedStreamToBlock);
    }

    /* ensure null terminated */
    pRawString[nLength] = '\0';

    /* attach string */
    uStatus = OpcUa_String_AttachWithOwnership(a_pValue, pRawString);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Free(pRawString);
    OpcUa_String_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadString
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadString(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_String*          a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadString");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(String);

    uStatus = OpcUa_String_BinaryDecode(a_pValue, pHandle->Context->MaxStringLength, pHandle->Istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_DateTime_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_DateTime_BinaryDecode(
    OpcUa_DateTime*    a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_Int64 nValue = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_DateTime_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_DateTime_Initialize(a_pValue);

    uStatus = OpcUa_Int64_BinaryDecode(&nValue, a_pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    a_pValue->dwHighDateTime = (OpcUa_UInt32)(nValue >> 32);
    a_pValue->dwLowDateTime  = (OpcUa_UInt32)(nValue &  0x00000000FFFFFFFF);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_DateTime_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDateTime
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDateTime(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_DateTime*        a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDateTime");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DateTime);

    uStatus = OpcUa_DateTime_BinaryDecode(a_pValue, pHandle->Istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Guid_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Guid_BinaryDecode(
    OpcUa_Guid*        a_pValue,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_UInt32 uBytesRead = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_Guid_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_Guid_Initialize(a_pValue);

    uStatus = OpcUa_UInt32_BinaryDecode(&a_pValue->Data1, a_pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_UInt16_BinaryDecode(&a_pValue->Data2, a_pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_UInt16_BinaryDecode(&a_pValue->Data3, a_pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    uBytesRead = sizeof(a_pValue->Data4);
    uStatus = a_pIstrm->Read(a_pIstrm, (OpcUa_Byte*)a_pValue->Data4, &uBytesRead);
    OpcUa_GotoErrorIfBad(uStatus);

    if (uBytesRead != sizeof(a_pValue->Data4))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadExpectedStreamToBlock);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Guid_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadGuid
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadGuid(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Guid*            a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadGuid");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Guid);

    uStatus = OpcUa_Guid_BinaryDecode(a_pValue, pHandle->Istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_ByteString_BinaryDecode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_ByteString_BinaryDecode(
    OpcUa_ByteString*  a_pValue,
    OpcUa_UInt32       a_nMaxByteStringLength,
    OpcUa_InputStream* a_pIstrm)
{
    OpcUa_Int32 nLength = -1;
    OpcUa_UInt32 uBytesRead = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_ByteString_BinaryDecode");

    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_ByteString_Initialize(a_pValue);

    /* decode length */
    uStatus = OpcUa_Int32_BinaryDecode(&nLength,a_pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check for null string */
    if (nLength == -1)
    {
        OpcUa_ReturnStatusCode;
    }

    /* check maximum string length */
    if (a_nMaxByteStringLength > 0 && (OpcUa_UInt32)nLength > a_nMaxByteStringLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
    }

    /* allocate bytes for string */
    a_pValue->Data = (OpcUa_Byte*)OpcUa_Alloc(nLength);

    OpcUa_GotoErrorIfAllocFailed(a_pValue->Data);
    a_pValue->Length = nLength;

    /* read bytes of string */
    uBytesRead = nLength;
    uStatus = a_pIstrm->Read(a_pIstrm, a_pValue->Data, &uBytesRead);
    OpcUa_GotoErrorIfBad(uStatus);

    if (uBytesRead != (OpcUa_UInt32)nLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadExpectedStreamToBlock);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_ByteString_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadByteString
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadByteString(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_ByteString*      a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadByteString");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ByteString);

    uStatus = OpcUa_ByteString_BinaryDecode(a_pValue, pHandle->Context->MaxByteStringLength, pHandle->Istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadXmlElement
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadXmlElement(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_XmlElement*      a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadXmlElement");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(XmlElement);

    uStatus = OpcUa_BinaryDecoder_ReadByteString(a_pDecoder, a_sFieldName, a_pValue);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_XmlElement_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadNodeIdBody
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_BinaryDecoder_ReadNodeIdBody(
    OpcUa_Decoder*      a_pDecoder,
    OpcUa_NodeId*       a_pValue,
    OpcUa_NodeEncoding  a_eEncodingType)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadNodeIdBody");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);

    OpcUa_NodeId_Initialize(a_pValue);

    switch (a_eEncodingType & OpcUa_NodeEncoding_TypeMask)
    {
        case OpcUa_NodeEncoding_TwoByte:
        {
            OpcUa_Byte nIdentifier = 0;
            uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &nIdentifier);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->IdentifierType = OpcUa_IdentifierType_Numeric;
            a_pValue->Identifier.Numeric = nIdentifier;
            break;
        }

        case OpcUa_NodeEncoding_FourByte:
        {
            OpcUa_Byte nNamespace = 0;
            OpcUa_UInt16 nIdentifier = 0;

            uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &nNamespace);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &nIdentifier);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->NamespaceIndex = nNamespace;
            a_pValue->IdentifierType = OpcUa_IdentifierType_Numeric;
            a_pValue->Identifier.Numeric = nIdentifier;
            break;
        }

        case OpcUa_NodeEncoding_Numeric:
        {
            OpcUa_UInt16 nNamespace = 0;
            OpcUa_UInt32 nIdentifier = 0;

            uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &nNamespace);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_BinaryDecoder_ReadUInt32(a_pDecoder, OpcUa_Null, &nIdentifier);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->NamespaceIndex = nNamespace;
            a_pValue->IdentifierType = OpcUa_IdentifierType_Numeric;
            a_pValue->Identifier.Numeric = nIdentifier;
            break;
        }

        case OpcUa_NodeEncoding_String:
        {
            uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &a_pValue->NamespaceIndex);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_BinaryDecoder_ReadString(a_pDecoder, OpcUa_Null, &a_pValue->Identifier.String);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->IdentifierType = OpcUa_IdentifierType_String;
            break;
        }

        case OpcUa_NodeEncoding_Guid:
        {
            uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &a_pValue->NamespaceIndex);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->Identifier.Guid = (OpcUa_Guid*)OpcUa_Alloc(sizeof(OpcUa_Guid));
            OpcUa_GotoErrorIfAllocFailed(a_pValue->Identifier.Guid);

            a_pValue->IdentifierType = OpcUa_IdentifierType_Guid;

            uStatus = OpcUa_BinaryDecoder_ReadGuid(a_pDecoder, OpcUa_Null, a_pValue->Identifier.Guid);
            OpcUa_GotoErrorIfBad(uStatus);
            break;
        }

        case OpcUa_NodeEncoding_ByteString:
        {
            uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &a_pValue->NamespaceIndex);
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_BinaryDecoder_ReadByteString(a_pDecoder, OpcUa_Null, &a_pValue->Identifier.ByteString);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->IdentifierType = OpcUa_IdentifierType_Opaque;
            break;
        }
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_NodeId_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadNodeId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadNodeId(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_NodeId*          a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_NodeEncoding eEncodingType = (OpcUa_NodeEncoding)0;
    OpcUa_Byte byEncodingType = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadNodeId");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(NodeId);

    /* read the encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &byEncodingType);
    OpcUa_GotoErrorIfBad(uStatus);

    eEncodingType = (OpcUa_NodeEncoding)byEncodingType;

    /* read node id body. */
    uStatus = OpcUa_BinaryDecoder_ReadNodeIdBody(a_pDecoder, a_pValue, eEncodingType);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadExpandedNodeId
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadExpandedNodeId(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_ExpandedNodeId*  a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_Byte uEncodingType = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadExpandedNodeId");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ExpandedNodeId);

    OpcUa_ExpandedNodeId_Initialize(a_pValue);

    /* read the encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &uEncodingType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read node id body. */
    uStatus = OpcUa_BinaryDecoder_ReadNodeIdBody(a_pDecoder, &a_pValue->NodeId, (OpcUa_NodeEncoding)uEncodingType);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read namespace uri. */
    if ((uEncodingType & OpcUa_NodeEncoding_UriMask) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadString(a_pDecoder, OpcUa_Null, &a_pValue->NamespaceUri);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read server index. */
    if ((uEncodingType & OpcUa_NodeEncoding_ServerIndexMask) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadUInt32(a_pDecoder, OpcUa_Null, &a_pValue->ServerIndex);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_ExpandedNodeId_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadStatusCode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadStatusCode(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_StatusCode*      a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadStatusCode");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(StatusCode);

    uStatus = OpcUa_UInt32_BinaryDecode(a_pValue, pHandle->Istrm);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    *a_pValue = OpcUa_Good;

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDiagnosticInfo
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDiagnosticInfo(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_DiagnosticInfo*  a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_Byte uEncodingByte = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDiagnosticInfo");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DiagnosticInfo);

    OpcUa_DiagnosticInfo_Initialize(a_pValue);

    /* read encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &uEncodingByte);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read symbolic id */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_SymbolicId) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &a_pValue->SymbolicId);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read namespace uri */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_NamespaceUri) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &a_pValue->NamespaceUri);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read locale */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_Locale) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &a_pValue->Locale);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read localized text */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_LocalizedText) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &a_pValue->LocalizedText);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read additional info */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_AdditionalInfo) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadString(a_pDecoder, OpcUa_Null, &a_pValue->AdditionalInfo);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read inner status code */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_InnerStatusCode) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadStatusCode(a_pDecoder, OpcUa_Null, &a_pValue->InnerStatusCode);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read inner diagnostic info */
    if ((uEncodingByte & OpcUa_DiagnosticInfo_EncodingByte_InnerDiagnosticInfo) != 0)
    {
        a_pValue->InnerDiagnosticInfo = (OpcUa_DiagnosticInfo*)OpcUa_Alloc(sizeof(OpcUa_DiagnosticInfo));
        OpcUa_GotoErrorIfAllocFailed(a_pValue->InnerDiagnosticInfo);
        OpcUa_DiagnosticInfo_Initialize(a_pValue->InnerDiagnosticInfo);

        if (pHandle->RecursionDepth > pHandle->Context->MaxRecursionDepth)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
        }

        pHandle->RecursionDepth++;
        uStatus = OpcUa_BinaryDecoder_ReadDiagnosticInfo(a_pDecoder, OpcUa_Null, a_pValue->InnerDiagnosticInfo);
        pHandle->RecursionDepth--;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_DiagnosticInfo_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadLocalizedText
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadLocalizedText(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_LocalizedText*   a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_Byte uEncodingByte = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadLocalizedText");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(LocalizedText);

    OpcUa_LocalizedText_Initialize(a_pValue);

    /* read encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &uEncodingByte);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read locale */
    if ((uEncodingByte & OpcUa_LocalizedText_EncodingByte_Locale) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadString(a_pDecoder, OpcUa_Null, &a_pValue->Locale);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read test */
    if ((uEncodingByte & OpcUa_LocalizedText_EncodingByte_Text) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadString(a_pDecoder, OpcUa_Null, &a_pValue->Text);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_LocalizedText_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadQualifiedName
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadQualifiedName(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_QualifiedName*   a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadQualifiedName");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(QualifiedName);

    OpcUa_QualifiedName_Initialize(a_pValue);

    OpcUa_Field_Read(UInt16, NamespaceIndex);
    OpcUa_Field_Read(String, Name);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_QualifiedName_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadEncodeable
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadEncodeable(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_EncodeableType*  a_pType,
    OpcUa_Void*            a_pValue);

/*============================================================================
 * OpcUa_BinaryDecoder_FindBodyType
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_BinaryDecoder_FindBodyType(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_ExtensionObject* a_pValue,
    OpcUa_EncodeableType** a_ppType)
{
    OpcUa_StringA pNamespaceUri = OpcUa_Null;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_FindBodyType");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_ppType);

    *a_ppType = OpcUa_Null;

    pHandle = (OpcUa_BinaryDecoder*)a_pDecoder->Handle;

    /* lookup namespace uri */
    if (a_pValue->TypeId.NodeId.NamespaceIndex != 0)
    {
        OpcUa_String sNamespaceUri;
        uStatus = OpcUa_StringTable_FindString(pHandle->Context->NamespaceUris, a_pValue->TypeId.NodeId.NamespaceIndex, &sNamespaceUri);
        OpcUa_GotoErrorIfBad(uStatus);

        if (uStatus == OpcUa_GoodNoData)
        {
            OpcUa_ReturnStatusCode;
        }

        /* function returns a readonly string reference that is part of the decoder struct */
        pNamespaceUri = OpcUa_String_GetRawString(&sNamespaceUri);
    }

    if (a_pValue->TypeId.NodeId.IdentifierType != OpcUa_IdentifierType_Numeric)
    {
        uStatus = OpcUa_GoodNoData;
        OpcUa_ReturnStatusCode;
    }

    /* lookup encodeable. */
    uStatus = OpcUa_EncodeableTypeTable_Find(
        pHandle->Context->KnownTypes,
        a_pValue->TypeId.NodeId.Identifier.Numeric,
        pNamespaceUri,
        a_ppType);

    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    *a_ppType = OpcUa_Null;

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadExtensionObject
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadExtensionObject(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_ExtensionObject* a_pValue)
{
    OpcUa_Int32 nLength = 0;
    OpcUa_Byte uEncodingByte = 0;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_EncodeableType* pType = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadExtensionObject");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ExtensionObject);

    OpcUa_ExtensionObject_Initialize(a_pValue);

    /* read type id */
    uStatus = OpcUa_BinaryDecoder_ReadNodeId(a_pDecoder, OpcUa_Null, &a_pValue->TypeId.NodeId);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &uEncodingByte);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check for null body */
    if (uEncodingByte == OpcUa_ExtensionObjectEncoding_None)
    {
        a_pValue->Encoding = OpcUa_ExtensionObjectEncoding_None;
        OpcUa_ReturnStatusCode;
    }

    /*  handle XML encoded bodies */
    if (uEncodingByte == OpcUa_ExtensionObjectEncoding_Xml)
    {
        a_pValue->Encoding = OpcUa_ExtensionObjectEncoding_Xml;
        uStatus = OpcUa_BinaryDecoder_ReadXmlElement(a_pDecoder, OpcUa_Null, &a_pValue->Body.Xml);
        OpcUa_GotoErrorIfBad(uStatus);

        /* all done */
        OpcUa_ReturnStatusCode;
    }

    /*  check for unexpected encoding byte. */
    if (uEncodingByte != OpcUa_ExtensionObjectEncoding_Binary)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadDataEncodingUnsupported);
    }

    uStatus = OpcUa_BinaryDecoder_FindBodyType(a_pDecoder, a_pValue, &pType);
    OpcUa_GotoErrorIfBad(uStatus);

    if (pType != OpcUa_Null)
    {
        OpcUa_Int32 nIndex = 0;
        OpcUa_UInt32 uBodyStart = 0;
        OpcUa_UInt32 uBodyEnd = 0;

        /* allocate instance of the encodeable type */
        uStatus = OpcUa_EncodeableObject_Create(pType, &a_pValue->Body.EncodeableObject.Object);
        OpcUa_GotoErrorIfBad(uStatus);

        a_pValue->Body.EncodeableObject.Type = pType;
        a_pValue->Encoding = OpcUa_ExtensionObjectEncoding_EncodeableObject;

        /* skip body length since type is known */
        uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &nLength);
        OpcUa_GotoErrorIfBad(uStatus);

        /* get the start position */
        uStatus = pHandle->Istrm->GetPosition((OpcUa_Stream*)pHandle->Istrm, &uBodyStart);
        OpcUa_GotoErrorIfBad(uStatus);

        /* read body */
        uStatus = OpcUa_BinaryDecoder_ReadEncodeable(
            a_pDecoder,
            OpcUa_Null,
            a_pValue->Body.EncodeableObject.Type,
            a_pValue->Body.EncodeableObject.Object);

        OpcUa_GotoErrorIfBad(uStatus);

        /* get the end position */
        uStatus = pHandle->Istrm->GetPosition((OpcUa_Stream*)pHandle->Istrm, &uBodyEnd);
        OpcUa_GotoErrorIfBad(uStatus);

        /* the body length must match */
        if ((OpcUa_UInt32)nLength != (uBodyEnd - uBodyStart))
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadDecodingError);
        }

        /* update type id */
        if (a_pValue->TypeId.NodeId.IdentifierType != OpcUa_IdentifierType_Numeric ||
            a_pValue->TypeId.NodeId.Identifier.Numeric != pType->BinaryEncodingTypeId ||
            a_pValue->TypeId.NodeId.NamespaceIndex != 0)
        {
            OpcUa_ExpandedNodeId_Clear(&a_pValue->TypeId);

            a_pValue->TypeId.NodeId.IdentifierType = OpcUa_IdentifierType_Numeric;
            a_pValue->TypeId.NodeId.Identifier.Numeric = pType->BinaryEncodingTypeId;

#if 1 /* CHZ: consider alternative below; pType->NamespaceUri must be sz anyway */
            OpcUa_String_AttachReadOnly(&a_pValue->TypeId.NamespaceUri, pType->NamespaceUri);

            uStatus = OpcUa_StringTable_FindIndex(
                pHandle->Context->NamespaceUris,
                &a_pValue->TypeId.NamespaceUri,
                &nIndex);

            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->TypeId.NodeId.NamespaceIndex = (OpcUa_UInt16)nIndex;
            OpcUa_String_Clear(&a_pValue->TypeId.NamespaceUri);
#else
            uStatus = OpcUa_StringTable_FindIndex(
                pHandle->Context->NamespaceUris,
                OpcUa_String_FromCString(pType->NamespaceUri),
                &nIndex);
            OpcUa_GotoErrorIfBad(uStatus);

            a_pValue->TypeId.NodeId.NamespaceIndex = (OpcUa_UInt16)nIndex;
#endif
        }

        /* all done */
        OpcUa_ReturnStatusCode;
    }

    /* read as a opaque ByteString. */
    a_pValue->Encoding = OpcUa_ExtensionObjectEncoding_Binary;
    uStatus = OpcUa_BinaryDecoder_ReadByteString(a_pDecoder, OpcUa_Null, &a_pValue->Body.Binary);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_ExtensionObject_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadVariant
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadVariant(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Variant*         a_pValue);

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDataValue
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDataValue(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_DataValue*       a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_Byte uEncodingByte = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDataValue");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DataValue);

    OpcUa_DataValue_Initialize(a_pValue);

    /* read encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &uEncodingByte);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read value */
    if ((uEncodingByte & OpcUa_DataValue_EncodingByte_Value) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadVariant(a_pDecoder, OpcUa_Null, &a_pValue->Value);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read status code */
    if ((uEncodingByte & OpcUa_DataValue_EncodingByte_StatusCode) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadStatusCode(a_pDecoder, OpcUa_Null, &a_pValue->StatusCode);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read source timestamp */
    if ((uEncodingByte & OpcUa_DataValue_EncodingByte_SourceTimestamp) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadDateTime(a_pDecoder, OpcUa_Null, &a_pValue->SourceTimestamp);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read source picoseconds */
    if ((uEncodingByte & OpcUa_DataValue_EncodingByte_SourcePicoseconds) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &a_pValue->SourcePicoseconds);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read server timestamp */
    if ((uEncodingByte & OpcUa_DataValue_EncodingByte_ServerTimestamp) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadDateTime(a_pDecoder, OpcUa_Null, &a_pValue->ServerTimestamp);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* read server picoseconds */
    if ((uEncodingByte & OpcUa_DataValue_EncodingByte_ServerPicoseconds) != 0)
    {
        uStatus = OpcUa_BinaryDecoder_ReadUInt16(a_pDecoder, OpcUa_Null, &a_pValue->ServerPicoseconds);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_DataValue_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadEncodeable
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadEncodeable(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_EncodeableType*  a_pType,
    OpcUa_Void*            a_pValue)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadEncodeable");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Encodeable);

    a_pType->Initialize(a_pValue);

    if (pHandle->RecursionDepth > pHandle->Context->MaxRecursionDepth)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
    }

    pHandle->RecursionDepth++;
    uStatus = a_pType->Decode(a_pValue, a_pDecoder);
    pHandle->RecursionDepth--;
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    a_pType->Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadEnumerated
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadEnumerated(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_EnumeratedType*  a_pType,
    OpcUa_Int32*           a_pValue)
{
    OpcUa_StringA pName;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadEnumerated");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Enumerated);

    *a_pValue = 0;

    uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, a_pValue);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_EnumeratedType_FindName(a_pType, *a_pValue, &pName);
    if (pName == OpcUa_Null)
    {
        uStatus = OpcUa_EnumeratedType_FindValue(a_pType, "Invalid", a_pValue);
    }
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    *a_pValue = 0;

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadBooleanArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadBooleanArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Boolean**        a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadBooleanArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(BooleanArray);

    OpcUa_Decode_ArrayType(Boolean);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Boolean);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadSByteArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadSByteArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_SByte**          a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadSByteArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(SByteArray);

    OpcUa_Decode_ArrayType(SByte);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(SByte);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadByteArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadByteArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Byte**           a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadByteArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ByteArray);

    OpcUa_Decode_ArrayType(Byte);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Byte);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadInt16Array
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadInt16Array(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Int16**          a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadInt16Array");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Int16Array);

    OpcUa_Decode_ArrayType(Int16);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Int16);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadUInt16Array
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadUInt16Array(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_UInt16**         a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadUInt16Array");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(UInt16Array);

    OpcUa_Decode_ArrayType(UInt16);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(UInt16);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadInt32Array
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadInt32Array(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Int32**          a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadInt32Array");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Int32Array);

    OpcUa_Decode_ArrayType(Int32);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Int32);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadUInt32Array
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadUInt32Array(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_UInt32**         a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadUInt32Array");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(UInt32Array);

    OpcUa_Decode_ArrayType(UInt32);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(UInt32);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadInt64Array
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadInt64Array(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Int64**          a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadInt64Array");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Int64Array);

    OpcUa_Decode_ArrayType(Int64);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Int64);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadUInt64Array
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadUInt64Array(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_UInt64**         a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadUInt64Array");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(UInt64Array);

    OpcUa_Decode_ArrayType(UInt64);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(UInt64);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadFloatArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadFloatArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Float**          a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadFloatArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(FloatArray);

    OpcUa_Decode_ArrayType(Float);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Float);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDoubleArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDoubleArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Double**         a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDoubleArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DoubleArray);

    OpcUa_Decode_ArrayType(Double);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Double);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadStringArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadStringArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_String**         a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadStringArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(StringArray);

    OpcUa_Decode_ArrayType(String);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(String);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDateTimeArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDateTimeArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_DateTime**       a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDateTimeArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DateTimeArray);

    OpcUa_Decode_ArrayType(DateTime);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(DateTime);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadGuidArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadGuidArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Guid**           a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadGuidArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(GuidArray);

    OpcUa_Decode_ArrayType(Guid);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(Guid);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadByteStringArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadByteStringArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_ByteString**     a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadByteStringArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ByteStringArray);

    OpcUa_Decode_ArrayType(ByteString);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(ByteString);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadXmlElementArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadXmlElementArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_XmlElement**     a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadXmlElementArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(XmlElementArray);

    OpcUa_Decode_ArrayType(XmlElement);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(XmlElement);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadNodeIdArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadNodeIdArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_NodeId**         a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadNodeIdArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(NodeIdArray);

    OpcUa_Decode_ArrayType(NodeId);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(NodeId);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadExpandedNodeIdArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadExpandedNodeIdArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_ExpandedNodeId** a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadExpandedNodeIdArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ExpandedNodeIdArray);

    OpcUa_Decode_ArrayType(ExpandedNodeId);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(ExpandedNodeId);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadStatusCodeArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadStatusCodeArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_StatusCode**     a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadStatusCodeArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(StatusCodeArray);

    OpcUa_Decode_ArrayType(StatusCode);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(StatusCode);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDiagnosticInfoArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDiagnosticInfoArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_DiagnosticInfo** a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDiagnosticInfoArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DiagnosticInfoArray);

    OpcUa_Decode_ArrayType(DiagnosticInfo);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(DiagnosticInfo);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadLocalizedTextArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadLocalizedTextArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_LocalizedText**  a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadLocalizedTextArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(LocalizedTextArray);

    OpcUa_Decode_ArrayType(LocalizedText);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(LocalizedText);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadQualifiedNameArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadQualifiedNameArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_QualifiedName**  a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadQualifiedNameArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(QualifiedNameArray);

    OpcUa_Decode_ArrayType(QualifiedName);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(QualifiedName);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadExtensionObjectArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadExtensionObjectArray(
    struct _OpcUa_Decoder*  a_pDecoder,
    OpcUa_StringA           a_sFieldName,
    OpcUa_ExtensionObject** a_ppArray,
    OpcUa_Int32*            a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadExtensionObjectArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(ExtensionObjectArray);

    OpcUa_Decode_ArrayType(ExtensionObject);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(ExtensionObject);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadDataValueArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadDataValueArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_DataValue**      a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadDataValueArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(DataValueArray);

    OpcUa_Decode_ArrayType(DataValue);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(DataValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadVariantArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadVariantArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Variant**        a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadVariantArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(VariantArray);

    OpcUa_Decode_ArrayType(Variant);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_ComplexArrayType(Variant);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadEncodeableArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadEncodeableArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_EncodeableType*  a_pType,
    OpcUa_Void**           a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_Int32 ii = 0;
    OpcUa_Int32 iLength = 0;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadEncodeableArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(EncodeableArray);

    *a_ppArray = OpcUa_Null;
    *a_pCount  = 0;

    /* read array length */
    uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &iLength);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check for null array */
    if (iLength == -1)
    {
        OpcUa_ReturnStatusCode;
    }

    /* check if limits exceeded */
    if (pHandle->Context->MaxArrayLength > 0 && (OpcUa_UInt32)iLength > pHandle->Context->MaxArrayLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
    }

    *a_ppArray = OpcUa_Alloc(a_pType->AllocationSize*iLength);
    OpcUa_GotoErrorIfAllocFailed(*a_ppArray);
    *a_pCount = iLength;

    /* initialize elements of array */
    for (ii = 0; ii < iLength; ii++)
    {
        OpcUa_UInt32 uPosition = ii*a_pType->AllocationSize;
        a_pType->Initialize(&(((OpcUa_Byte*)(*a_ppArray))[uPosition]));
    }

    /* read elements of array */
    for (ii = 0; ii < iLength; ii++)
    {
        OpcUa_UInt32 uPosition = ii*a_pType->AllocationSize;

        uStatus = OpcUa_BinaryDecoder_ReadEncodeable(
            a_pDecoder,
            OpcUa_Null,
            a_pType,
            &(((OpcUa_Byte*)(*a_ppArray))[uPosition]));

        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* clear elements of array */
    if (*a_ppArray != OpcUa_Null)
    {
        for (ii = 0; ii < *a_pCount; ii++)
        {
            OpcUa_UInt32 uPosition = ii*a_pType->AllocationSize;
            a_pType->Clear(&(((OpcUa_Byte*)(*a_ppArray))[uPosition]));
        }

        OpcUa_Free(*a_ppArray);

        *a_ppArray = OpcUa_Null;
        *a_pCount  = 0;
    }

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadEnumeratedArray
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadEnumeratedArray(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_EnumeratedType*  a_pType,
    OpcUa_Int32**          a_ppArray,
    OpcUa_Int32*           a_pCount)
{
    OpcUa_StringA pName;
    OpcUa_Int32 ii = 0;
    OpcUa_Int32 iLength = 0;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadEnumeratedArray");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppArray);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);
    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(EnumeratedArray);

    *a_ppArray = OpcUa_Null;
    *a_pCount  = 0;

    /* read array length */
    uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &iLength);
    OpcUa_GotoErrorIfBad(uStatus);

    /* check for null array */
    if (iLength == -1)
    {
        OpcUa_ReturnStatusCode;
    }

    /* check if limits exceeded */
    if (pHandle->Context->MaxArrayLength > 0 && (OpcUa_UInt32)iLength > pHandle->Context->MaxArrayLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
    }

    *a_ppArray = (OpcUa_Int32 *)OpcUa_Alloc(sizeof(OpcUa_Int32)*iLength);
    OpcUa_GotoErrorIfAllocFailed(*a_ppArray);
    *a_pCount = iLength;

    /* initialize elements of array */
    for (ii = 0; ii < iLength; ii++)
    {
        uStatus = OpcUa_BinaryDecoder_ReadInt32(a_pDecoder, OpcUa_Null, &((*a_ppArray)[ii]));
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_EnumeratedType_FindName(a_pType, (*a_ppArray)[ii], &pName);
        if (pName == OpcUa_Null)
        {
            uStatus = OpcUa_EnumeratedType_FindValue(a_pType, "Invalid", &(*a_ppArray)[ii]);
        }
        OpcUa_GotoErrorIfBad(uStatus);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Clear_SimpleArrayType(Int32);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Variant_BinaryDecode_ValueType
 *===========================================================================*/
/** @brief Helper macro for encoding a Variant of value type. */
#define OpcUa_Variant_BinaryDecode_ValueType(xName) \
uStatus = OpcUa_BinaryDecoder_Read##xName(a_pDecoder, OpcUa_Null, &a_pValue->Value.xName); \
OpcUa_GotoErrorIfBad(uStatus);

/*============================================================================
 * OpcUa_Variant_BinaryDecode_ReferenceType
 *===========================================================================*/
/** @brief Helper macro for encoding a Variant of reference type. */
#define OpcUa_Variant_BinaryDecode_ReferenceType(xName) \
a_pValue->Value.xName = (OpcUa_##xName *)OpcUa_Alloc(sizeof(OpcUa_##xName)); \
OpcUa_GotoErrorIfAllocFailed(a_pValue->Value.xName); \
uStatus = OpcUa_BinaryDecoder_Read##xName(a_pDecoder, OpcUa_Null, a_pValue->Value.xName); \
OpcUa_GotoErrorIfBad(uStatus);

/*============================================================================
 * OpcUa_AnyArray_BinaryDecode
 *===========================================================================*/
/** @brief Helper macro for encoding a Variant of array type. */
#define OpcUa_AnyArray_BinaryDecode(xName) \
uStatus = OpcUa_BinaryDecoder_Read##xName##Array(a_pDecoder, OpcUa_Null, &a_pValue->Value.Array.Value.xName##Array, &a_pValue->Value.Array.Length); \
OpcUa_GotoErrorIfBad(uStatus);

/*============================================================================
 * OpcUa_BinaryDecoder_ReadVariant
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadVariant(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_StringA          a_sFieldName,
    OpcUa_Variant*         a_pValue)
{
    OpcUa_Byte uEncodingByte = 0;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadVariant");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);
    OpcUa_ReferenceParameter(a_sFieldName);
    OpcUa_BinaryDecoder_VerifyState(Variant);

    OpcUa_Variant_Initialize(a_pValue);

    /* read encoding byte */
    uStatus = OpcUa_BinaryDecoder_ReadByte(a_pDecoder, OpcUa_Null, &uEncodingByte);
    OpcUa_ReturnErrorIfBad(uStatus);

    if (pHandle->RecursionDepth > pHandle->Context->MaxRecursionDepth)
    {
        uStatus = OpcUa_BadEncodingLimitsExceeded;
        OpcUa_ReturnStatusCode;
    }

    pHandle->RecursionDepth++;
    a_pValue->Datatype = (OpcUa_Byte)(OpcUa_Variant_TypeMask & uEncodingByte);

    /* check for arrays. */
    if (uEncodingByte & OpcUa_Variant_ArrayMask)
    {
        a_pValue->ArrayType = OpcUa_VariantArrayType_Array;
    }

    if (a_pValue->ArrayType != OpcUa_VariantArrayType_Scalar)
    {
        switch(a_pValue->Datatype)
        {
            case OpcUaType_Boolean:
            {
                OpcUa_AnyArray_BinaryDecode(Boolean);
                break;
            }

            case OpcUaType_SByte:
            {
                OpcUa_AnyArray_BinaryDecode(SByte);
                break;
            }

            case OpcUaType_Byte:
            {
                OpcUa_AnyArray_BinaryDecode(Byte);
                break;
            }

            case OpcUaType_Int16:
            {
                OpcUa_AnyArray_BinaryDecode(Int16);
                break;
            }

            case OpcUaType_UInt16:
            {
                OpcUa_AnyArray_BinaryDecode(UInt16);
                break;
            }

            case OpcUaType_Int32:
            {
                OpcUa_AnyArray_BinaryDecode(Int32);
                break;
            }

            case OpcUaType_UInt32:
            {
                OpcUa_AnyArray_BinaryDecode(UInt32);
                break;
            }

            case OpcUaType_Int64:
            {
                OpcUa_AnyArray_BinaryDecode(Int64);
                break;
            }

            case OpcUaType_UInt64:
            {
                OpcUa_AnyArray_BinaryDecode(UInt64);
                break;
            }

            case OpcUaType_Float:
            {
                OpcUa_AnyArray_BinaryDecode(Float);
                break;
            }

            case OpcUaType_Double:
            {
                OpcUa_AnyArray_BinaryDecode(Double);
                break;
            }

            case OpcUaType_String:
            {
                OpcUa_AnyArray_BinaryDecode(String);
                break;
            }

            case OpcUaType_DateTime:
            {
                OpcUa_AnyArray_BinaryDecode(DateTime);
                break;
            }

            case OpcUaType_Guid:
            {
                OpcUa_AnyArray_BinaryDecode(Guid);
                break;
            }

            case OpcUaType_ByteString:
            {
                OpcUa_AnyArray_BinaryDecode(ByteString);
                break;
            }

            case OpcUaType_XmlElement:
            {
                OpcUa_AnyArray_BinaryDecode(XmlElement);
                break;
            }

            case OpcUaType_NodeId:
            {
                OpcUa_AnyArray_BinaryDecode(NodeId);
                break;
            }

            case OpcUaType_ExpandedNodeId:
            {
                OpcUa_AnyArray_BinaryDecode(ExpandedNodeId);
                break;
            }

            case OpcUaType_StatusCode:
            {
                OpcUa_AnyArray_BinaryDecode(StatusCode);
                break;
            }

            case OpcUaType_LocalizedText:
            {
                OpcUa_AnyArray_BinaryDecode(LocalizedText);
                break;
            }

            case OpcUaType_QualifiedName:
            {
                OpcUa_AnyArray_BinaryDecode(QualifiedName);
                break;
            }

            case OpcUaType_ExtensionObject:
            {
                OpcUa_AnyArray_BinaryDecode(ExtensionObject);
                break;
            }

            case OpcUaType_DataValue:
            {
                OpcUa_AnyArray_BinaryDecode(DataValue);
                break;
            }

            case OpcUaType_Variant:
            {
                OpcUa_AnyArray_BinaryDecode(Variant);
                break;
            }

            default:
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingError);
            }
        }

        /* check for array dimensions. */
        if (uEncodingByte & OpcUa_Variant_ArrayDimensionsMask)
        {
            OpcUa_Int32 ii = 0;
            OpcUa_Int32 iExpectedLength = a_pValue->Value.Array.Length;
            OpcUa_Int32 iNoOfDimensions = 0;
            OpcUa_Int32* pDimensions = OpcUa_Null;
            OpcUa_Void* pArray = OpcUa_Null;

            /* read the array of dimensions */
            uStatus = OpcUa_BinaryDecoder_ReadInt32Array(
                a_pDecoder,
                OpcUa_Null,
                &pDimensions,
                &iNoOfDimensions);

            OpcUa_GotoErrorIfBad(uStatus);

            for (ii = 0; ii < iNoOfDimensions; ii++)
            {
                if (pDimensions[ii] <= 0 || iExpectedLength % pDimensions[ii] != 0)
                {
                    OpcUa_Free(pDimensions);
                    OpcUa_GotoErrorWithStatus(OpcUa_BadDecodingError);
                }
                iExpectedLength /= pDimensions[ii];
            }

            /* the matrix is stored as one dimensional array which will be freed on error */
            if (iNoOfDimensions <= 0 || iExpectedLength != 1)
            {
                OpcUa_Free(pDimensions);
                OpcUa_GotoErrorWithStatus(OpcUa_BadDecodingError);
            }

            /* copy array data into matrix structure */
            a_pValue->ArrayType = OpcUa_VariantArrayType_Matrix;

            pArray = a_pValue->Value.Array.Value.Array;
            a_pValue->Value.Matrix.Value.Array = pArray;

            a_pValue->Value.Matrix.NoOfDimensions = iNoOfDimensions;
            a_pValue->Value.Matrix.Dimensions = pDimensions;
        }
    }
    else
    {
        switch(a_pValue->Datatype)
        {
            case OpcUaType_Null:
            {
                break;
            }

            case OpcUaType_Boolean:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Boolean);
                break;
            }

            case OpcUaType_SByte:
            {
                OpcUa_Variant_BinaryDecode_ValueType(SByte);
                break;
            }

            case OpcUaType_Byte:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Byte);
                break;
            }

            case OpcUaType_Int16:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Int16);
                break;
            }

            case OpcUaType_UInt16:
            {
                OpcUa_Variant_BinaryDecode_ValueType(UInt16);
                break;
            }

            case OpcUaType_Int32:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Int32);
                break;
            }

            case OpcUaType_UInt32:
            {
                OpcUa_Variant_BinaryDecode_ValueType(UInt32);
                break;
            }

            case OpcUaType_Int64:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Int64);
                break;
            }

            case OpcUaType_UInt64:
            {
                OpcUa_Variant_BinaryDecode_ValueType(UInt64);
                break;
            }

            case OpcUaType_Float:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Float);
                break;
            }

            case OpcUaType_Double:
            {
                OpcUa_Variant_BinaryDecode_ValueType(Double);
                break;
            }

            case OpcUaType_String:
            {
                OpcUa_Variant_BinaryDecode_ValueType(String);
                break;
            }

            case OpcUaType_DateTime:
            {
                OpcUa_Variant_BinaryDecode_ValueType(DateTime);
                break;
            }

            case OpcUaType_Guid:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(Guid);
                break;
            }

            case OpcUaType_ByteString:
            {
                OpcUa_Variant_BinaryDecode_ValueType(ByteString);
                break;
            }

            case OpcUaType_XmlElement:
            {
                OpcUa_Variant_BinaryDecode_ValueType(XmlElement);
                break;
            }

            case OpcUaType_NodeId:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(NodeId);
                break;
            }

            case OpcUaType_ExpandedNodeId:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(ExpandedNodeId);
                break;
            }

            case OpcUaType_StatusCode:
            {
                OpcUa_Variant_BinaryDecode_ValueType(StatusCode);
                break;
            }

            case OpcUaType_LocalizedText:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(LocalizedText);
                break;
            }

            case OpcUaType_QualifiedName:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(QualifiedName);
                break;
            }

            case OpcUaType_ExtensionObject:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(ExtensionObject);
                break;
            }

            case OpcUaType_DataValue:
            {
                OpcUa_Variant_BinaryDecode_ReferenceType(DataValue);
                break;
            }

            default:
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingError);
            }
        }
    }

    pHandle->RecursionDepth--;
    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    pHandle->RecursionDepth--;
    OpcUa_Variant_Clear(a_pValue);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_ReadMessage
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_ReadMessage(
    OpcUa_Decoder*         a_pDecoder,
    OpcUa_EncodeableType** a_ppMessageType,
    OpcUa_Void**           a_ppMessage)
{
    OpcUa_NodeId cTypeId;
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;
    OpcUa_MessageContext* pContext = OpcUa_Null;
    OpcUa_EncodeableType* pMessageType = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_ReadMessage");

    OpcUa_ReturnErrorIfArgumentNull(a_pDecoder);
    OpcUa_ReturnErrorIfArgumentNull(a_ppMessage);
    OpcUa_ReturnErrorIfArgumentNull(a_ppMessageType);

    pHandle = (OpcUa_BinaryDecoder*)a_pDecoder->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pHandle);
    OpcUa_ReturnErrorIfTrue(pHandle->Closed, OpcUa_BadInvalidState);
    pContext = pHandle->Context;
    OpcUa_ReturnErrorIfArgumentNull(pContext);

    *a_ppMessage = OpcUa_Null;
    pMessageType = *a_ppMessageType;

    /* read type id */
    uStatus = a_pDecoder->ReadNodeId(a_pDecoder, OpcUa_Null, &cTypeId);
    OpcUa_GotoErrorIfBad(uStatus);

    /* do not support non-UA messages */
    if (cTypeId.IdentifierType != OpcUa_IdentifierType_Numeric || cTypeId.NamespaceIndex != 0)
    {
        OpcUa_NodeId_Clear(&cTypeId);
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }

    /* check if actual type matches the expected type */
    if (pMessageType == OpcUa_Null || pMessageType->BinaryEncodingTypeId != cTypeId.Identifier.Numeric)
    {
        uStatus = OpcUa_EncodeableTypeTable_Find(
            pHandle->Context->KnownTypes,
            cTypeId.Identifier.Numeric,
            OpcUa_Null,
            &pMessageType);

        OpcUa_GotoErrorIfBad(uStatus);

        /* check if type was found */
        OpcUa_GotoErrorIfTrue(pMessageType == OpcUa_Null, OpcUa_BadNotFound);
    }

    *a_ppMessageType = pMessageType;

    /* allocate instance of the encodeable type */
    uStatus = OpcUa_EncodeableObject_Create(pMessageType, a_ppMessage);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read message */
    uStatus = OpcUa_BinaryDecoder_ReadEncodeable(
        a_pDecoder,
        OpcUa_Null,
        pMessageType,
        *a_ppMessage);

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if (*a_ppMessage != OpcUa_Null)
    {
        pMessageType->Clear(*a_ppMessage);
        OpcUa_Free(*a_ppMessage);
    }

    *a_ppMessage = OpcUa_Null;
    *a_ppMessageType = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_BinaryDecoder_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_BinaryDecoder_Create(
    OpcUa_Decoder** a_ppDecoder)
{
    OpcUa_BinaryDecoder* pHandle = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "OpcUa_BinaryDecoder_Create");

    OpcUa_ReturnErrorIfArgumentNull(a_ppDecoder);

    *a_ppDecoder = OpcUa_Null;

    pHandle = (OpcUa_BinaryDecoder*)OpcUa_Alloc(sizeof(OpcUa_BinaryDecoder));
    OpcUa_GotoErrorIfAllocFailed(pHandle);
    OpcUa_MemSet(pHandle, 0, sizeof(OpcUa_BinaryDecoder));

    pHandle->SanityCheck = OpcUa_BinaryDecoder_SanityCheck;
    pHandle->Closed      = OpcUa_True;
    pHandle->Istrm       = OpcUa_Null;
    pHandle->Context     = OpcUa_Null;

    *a_ppDecoder = (OpcUa_Decoder*)OpcUa_Alloc(sizeof(OpcUa_Decoder));
    OpcUa_GotoErrorIfAllocFailed(*a_ppDecoder);
    OpcUa_MemSet(*a_ppDecoder, 0, sizeof(OpcUa_Decoder));

    (*a_ppDecoder)->Handle      = pHandle;
    (*a_ppDecoder)->DecoderType = OpcUa_EncoderType_Binary;

    (*a_ppDecoder)->Open                     = OpcUa_BinaryDecoder_Open;
    (*a_ppDecoder)->Close                    = OpcUa_BinaryDecoder_Close;
    (*a_ppDecoder)->Delete                   = OpcUa_BinaryDecoder_Delete;
    (*a_ppDecoder)->PushNamespace            = OpcUa_BinaryDecoder_PushNamespace;
    (*a_ppDecoder)->PopNamespace             = OpcUa_BinaryDecoder_PopNamespace;
    (*a_ppDecoder)->ReadBoolean              = OpcUa_BinaryDecoder_ReadBoolean;
    (*a_ppDecoder)->ReadSByte                = OpcUa_BinaryDecoder_ReadSByte;
    (*a_ppDecoder)->ReadByte                 = OpcUa_BinaryDecoder_ReadByte;
    (*a_ppDecoder)->ReadInt16                = OpcUa_BinaryDecoder_ReadInt16;
    (*a_ppDecoder)->ReadUInt16               = OpcUa_BinaryDecoder_ReadUInt16;
    (*a_ppDecoder)->ReadInt32                = OpcUa_BinaryDecoder_ReadInt32;
    (*a_ppDecoder)->ReadUInt32               = OpcUa_BinaryDecoder_ReadUInt32;
    (*a_ppDecoder)->ReadInt64                = OpcUa_BinaryDecoder_ReadInt64;
    (*a_ppDecoder)->ReadUInt64               = OpcUa_BinaryDecoder_ReadUInt64;
    (*a_ppDecoder)->ReadFloat                = OpcUa_BinaryDecoder_ReadFloat;
    (*a_ppDecoder)->ReadDouble               = OpcUa_BinaryDecoder_ReadDouble;
    (*a_ppDecoder)->ReadString               = OpcUa_BinaryDecoder_ReadString;
    (*a_ppDecoder)->ReadDateTime             = OpcUa_BinaryDecoder_ReadDateTime;
    (*a_ppDecoder)->ReadGuid                 = OpcUa_BinaryDecoder_ReadGuid;
    (*a_ppDecoder)->ReadByteString           = OpcUa_BinaryDecoder_ReadByteString;
    (*a_ppDecoder)->ReadXmlElement           = OpcUa_BinaryDecoder_ReadXmlElement;
    (*a_ppDecoder)->ReadNodeId               = OpcUa_BinaryDecoder_ReadNodeId;
    (*a_ppDecoder)->ReadExpandedNodeId       = OpcUa_BinaryDecoder_ReadExpandedNodeId;
    (*a_ppDecoder)->ReadStatusCode           = OpcUa_BinaryDecoder_ReadStatusCode;
    (*a_ppDecoder)->ReadDiagnosticInfo       = OpcUa_BinaryDecoder_ReadDiagnosticInfo;
    (*a_ppDecoder)->ReadLocalizedText        = OpcUa_BinaryDecoder_ReadLocalizedText;
    (*a_ppDecoder)->ReadQualifiedName        = OpcUa_BinaryDecoder_ReadQualifiedName;
    (*a_ppDecoder)->ReadExtensionObject      = OpcUa_BinaryDecoder_ReadExtensionObject;
    (*a_ppDecoder)->ReadDataValue            = OpcUa_BinaryDecoder_ReadDataValue;
    (*a_ppDecoder)->ReadVariant              = OpcUa_BinaryDecoder_ReadVariant;
    (*a_ppDecoder)->ReadEncodeable           = OpcUa_BinaryDecoder_ReadEncodeable;
    (*a_ppDecoder)->ReadEnumerated           = OpcUa_BinaryDecoder_ReadEnumerated;
    (*a_ppDecoder)->ReadBooleanArray         = OpcUa_BinaryDecoder_ReadBooleanArray;
    (*a_ppDecoder)->ReadSByteArray           = OpcUa_BinaryDecoder_ReadSByteArray;
    (*a_ppDecoder)->ReadByteArray            = OpcUa_BinaryDecoder_ReadByteArray;
    (*a_ppDecoder)->ReadInt16Array           = OpcUa_BinaryDecoder_ReadInt16Array;
    (*a_ppDecoder)->ReadUInt16Array          = OpcUa_BinaryDecoder_ReadUInt16Array;
    (*a_ppDecoder)->ReadInt32Array           = OpcUa_BinaryDecoder_ReadInt32Array;
    (*a_ppDecoder)->ReadUInt32Array          = OpcUa_BinaryDecoder_ReadUInt32Array;
    (*a_ppDecoder)->ReadInt64Array           = OpcUa_BinaryDecoder_ReadInt64Array;
    (*a_ppDecoder)->ReadUInt64Array          = OpcUa_BinaryDecoder_ReadUInt64Array;
    (*a_ppDecoder)->ReadFloatArray           = OpcUa_BinaryDecoder_ReadFloatArray;
    (*a_ppDecoder)->ReadDoubleArray          = OpcUa_BinaryDecoder_ReadDoubleArray;
    (*a_ppDecoder)->ReadStringArray          = OpcUa_BinaryDecoder_ReadStringArray;
    (*a_ppDecoder)->ReadDateTimeArray        = OpcUa_BinaryDecoder_ReadDateTimeArray;
    (*a_ppDecoder)->ReadGuidArray            = OpcUa_BinaryDecoder_ReadGuidArray;
    (*a_ppDecoder)->ReadByteStringArray      = OpcUa_BinaryDecoder_ReadByteStringArray;
    (*a_ppDecoder)->ReadXmlElementArray      = OpcUa_BinaryDecoder_ReadXmlElementArray;
    (*a_ppDecoder)->ReadNodeIdArray          = OpcUa_BinaryDecoder_ReadNodeIdArray;
    (*a_ppDecoder)->ReadExpandedNodeIdArray  = OpcUa_BinaryDecoder_ReadExpandedNodeIdArray;
    (*a_ppDecoder)->ReadStatusCodeArray      = OpcUa_BinaryDecoder_ReadStatusCodeArray;
    (*a_ppDecoder)->ReadDiagnosticInfoArray  = OpcUa_BinaryDecoder_ReadDiagnosticInfoArray;
    (*a_ppDecoder)->ReadLocalizedTextArray   = OpcUa_BinaryDecoder_ReadLocalizedTextArray;
    (*a_ppDecoder)->ReadQualifiedNameArray   = OpcUa_BinaryDecoder_ReadQualifiedNameArray;
    (*a_ppDecoder)->ReadExtensionObjectArray = OpcUa_BinaryDecoder_ReadExtensionObjectArray;
    (*a_ppDecoder)->ReadDataValueArray       = OpcUa_BinaryDecoder_ReadDataValueArray;
    (*a_ppDecoder)->ReadVariantArray         = OpcUa_BinaryDecoder_ReadVariantArray;
    (*a_ppDecoder)->ReadEncodeableArray      = OpcUa_BinaryDecoder_ReadEncodeableArray;
    (*a_ppDecoder)->ReadEnumeratedArray      = OpcUa_BinaryDecoder_ReadEnumeratedArray;
    (*a_ppDecoder)->ReadMessage              = OpcUa_BinaryDecoder_ReadMessage;

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    if(pHandle != OpcUa_Null)
    {
        OpcUa_Free(pHandle);
    }

    OpcUa_FinishErrorHandling;
}
