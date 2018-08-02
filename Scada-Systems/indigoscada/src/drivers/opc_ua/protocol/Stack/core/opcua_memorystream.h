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

#ifndef _OpcUa_MemoryStream_H_
#define _OpcUa_MemoryStream_H_ 1
#ifdef OPCUA_HAVE_MEMORYSTREAM
#include <opcua_stream.h>

OPCUA_BEGIN_EXTERN_C

/**
  @brief Allocates a new readable memory stream.

  The caller must ensure the buffer is valid memory until Close is called.

  @param buffer     [in]  The buffer which is the source for the stream.
  @param bufferSize [in]  The length of the buffer.
  @param istrm      [out] The input stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_MemoryStream_CreateReadable(
    OpcUa_Byte*         buffer,
    OpcUa_UInt32        bufferSize,
    OpcUa_InputStream** istrm);

/**
  @brief Allocates a new writeable memory stream.

  @param blockSize  [in]  The size of the block to allocate when new memory is required.
  @param maxSize    [in]  The maximum buffer size (0 means no limit).
  @param ostrm      [out] The output stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_MemoryStream_CreateWriteable(
    OpcUa_UInt32         blockSize,
    OpcUa_UInt32         maxSize,
    OpcUa_OutputStream** ostrm);

/**
  @brief Returns the internal buffer for a writeable stream.

  This function cannot be called until the stream is closed.

  The memory returned by this function is owned by the stream and will be
  de-allocated when OpcUa_MemoryStream_Delete is called.

  @param ostrm      [in]  The output stream.
  @param buffer     [out] The buffer which contains the data written to the stream.
  @param bufferSize [out] The amount of valid data in the buffer.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_MemoryStream_GetBuffer(
    OpcUa_OutputStream* ostrm,
    OpcUa_Byte**        buffer,
    OpcUa_UInt32*       bufferSize);

/**
  @brief Declare OpcUa_##Type##_CopyTo function for whatever type you want.

  Declare the generic OpcUa_##Type##_CopyTo function.
  The first param is the input, the second param is the output.

  @param Type       [in] Type name w/o "OpcUa_"
*/
#define OPCUA_DECLARE_GENERIC_COPY(Type) \
OpcUa_StatusCode OpcUa_##Type##_CopyTo(OpcUa_##Type *, OpcUa_##Type *);

/**
  @brief Implement OpcUa_##Type##_CopyTo function for scalar type.

  @param Type       [in] Type name w/o "OpcUa_"
  @param SizeHint   [in] Allocation size hint
*/
#define OPCUA_IMPLEMENT_SCALAR_COPY(Type, SizeHint) \
OpcUa_StatusCode OpcUa_##Type##_CopyTo(OpcUa_##Type *pInput, OpcUa_##Type *pOutput) \
{ \
   extern OpcUa_EncodeableTypeTable OpcUa_ProxyStub_g_EncodeableTypes; \
   extern OpcUa_StringTable OpcUa_ProxyStub_g_NamespaceUris; \
   OpcUa_StatusCode uStatus; \
   OpcUa_MessageContext cContext; \
   OpcUa_Encoder* pEncoder; \
   OpcUa_Handle hEncoderContext; \
   OpcUa_Decoder* pDecoder; \
   OpcUa_Handle hDecoderContext; \
   OpcUa_OutputStream* pOutputStream; \
   OpcUa_InputStream* pInputStream; \
   OpcUa_Byte* pBuffer; \
   OpcUa_UInt32 uBufferSize; \
   OpcUa_MessageContext_Initialize(&cContext); \
   cContext.KnownTypes = &OpcUa_ProxyStub_g_EncodeableTypes; \
   cContext.NamespaceUris = &OpcUa_ProxyStub_g_NamespaceUris; \
   cContext.AlwaysCheckLengths = OpcUa_False; \
   uStatus = OpcUa_MemoryStream_CreateWriteable(SizeHint, 0, &pOutputStream); \
   if (OpcUa_IsBad(uStatus)) goto err0; \
   uStatus = OpcUa_BinaryEncoder_Create(&pEncoder); \
   if (OpcUa_IsBad(uStatus)) goto err1; \
   uStatus = pEncoder->Open(pEncoder, pOutputStream, &cContext, &hEncoderContext); \
   if (OpcUa_IsBad(uStatus)) goto err2; \
   uStatus = pEncoder->Write##Type((OpcUa_Encoder*)hEncoderContext, OpcUa_Null, pInput, OpcUa_Null); \
   if (OpcUa_IsBad(uStatus)) goto err3; \
   pOutputStream->Close((OpcUa_Stream*)pOutputStream); \
   uStatus = OpcUa_MemoryStream_GetBuffer(pOutputStream, &pBuffer, &uBufferSize); \
   if (OpcUa_IsBad(uStatus)) goto err3; \
   uStatus = OpcUa_MemoryStream_CreateReadable(pBuffer, uBufferSize, &pInputStream); \
   if (OpcUa_IsBad(uStatus)) goto err3; \
   uStatus = OpcUa_BinaryDecoder_Create(&pDecoder); \
   if (OpcUa_IsBad(uStatus)) goto err4; \
   uStatus = pDecoder->Open(pDecoder, pInputStream, &cContext, &hDecoderContext); \
   if (OpcUa_IsBad(uStatus)) goto err5; \
   uStatus = pDecoder->Read##Type((OpcUa_Decoder*)hDecoderContext, OpcUa_Null, pOutput); \
   OpcUa_Decoder_Close(pDecoder, &hDecoderContext); \
err5: \
   OpcUa_Decoder_Delete(&pDecoder); \
err4: \
   pInputStream->Close((OpcUa_Stream*)pInputStream); \
   pInputStream->Delete((OpcUa_Stream**)&pInputStream); \
err3: \
   OpcUa_Encoder_Close(pEncoder, &hEncoderContext); \
err2: \
   OpcUa_Encoder_Delete(&pEncoder); \
err1: \
   pOutputStream->Delete((OpcUa_Stream**)&pOutputStream); \
err0: \
   OpcUa_MessageContext_Clear(&cContext); \
   return uStatus; \
}

/**
  @brief Implement OpcUa_##Type##_CopyTo function for encodeable type.

  @param Type       [in] Type name w/o "OpcUa_"
  @param SizeHint   [in] Allocation size hint
*/
#define OPCUA_IMPLEMENT_ENCODEABLE_COPY(Type, SizeHint) \
OpcUa_StatusCode OpcUa_##Type##_CopyTo(OpcUa_##Type *pInput, OpcUa_##Type *pOutput) \
{ \
   extern OpcUa_EncodeableTypeTable OpcUa_ProxyStub_g_EncodeableTypes; \
   extern OpcUa_StringTable OpcUa_ProxyStub_g_NamespaceUris; \
   OpcUa_StatusCode uStatus; \
   OpcUa_MessageContext cContext; \
   OpcUa_Encoder* pEncoder; \
   OpcUa_Handle hEncoderContext; \
   OpcUa_Decoder* pDecoder; \
   OpcUa_Handle hDecoderContext; \
   OpcUa_OutputStream* pOutputStream; \
   OpcUa_InputStream* pInputStream; \
   OpcUa_Byte* pBuffer; \
   OpcUa_UInt32 uBufferSize; \
   OpcUa_MessageContext_Initialize(&cContext); \
   cContext.KnownTypes = &OpcUa_ProxyStub_g_EncodeableTypes; \
   cContext.NamespaceUris = &OpcUa_ProxyStub_g_NamespaceUris; \
   cContext.AlwaysCheckLengths = OpcUa_False; \
   uStatus = OpcUa_MemoryStream_CreateWriteable(SizeHint, 0, &pOutputStream); \
   if (OpcUa_IsBad(uStatus)) goto err0; \
   uStatus = OpcUa_BinaryEncoder_Create(&pEncoder); \
   if (OpcUa_IsBad(uStatus)) goto err1; \
   uStatus = pEncoder->Open(pEncoder, pOutputStream, &cContext, &hEncoderContext); \
   if (OpcUa_IsBad(uStatus)) goto err2; \
   uStatus = OpcUa_##Type##_Encode(pInput, (OpcUa_Encoder*)hEncoderContext); \
   if (OpcUa_IsBad(uStatus)) goto err3; \
   pOutputStream->Close((OpcUa_Stream*)pOutputStream); \
   uStatus = OpcUa_MemoryStream_GetBuffer(pOutputStream, &pBuffer, &uBufferSize); \
   if (OpcUa_IsBad(uStatus)) goto err3; \
   uStatus = OpcUa_MemoryStream_CreateReadable(pBuffer, uBufferSize, &pInputStream); \
   if (OpcUa_IsBad(uStatus)) goto err3; \
   uStatus = OpcUa_BinaryDecoder_Create(&pDecoder); \
   if (OpcUa_IsBad(uStatus)) goto err4; \
   uStatus = pDecoder->Open(pDecoder, pInputStream, &cContext, &hDecoderContext); \
   if (OpcUa_IsBad(uStatus)) goto err5; \
   uStatus = OpcUa_##Type##_Decode(pOutput, (OpcUa_Decoder*)hDecoderContext); \
   OpcUa_Decoder_Close(pDecoder, &hDecoderContext); \
err5: \
   OpcUa_Decoder_Delete(&pDecoder); \
err4: \
   pInputStream->Close((OpcUa_Stream*)pInputStream); \
   pInputStream->Delete((OpcUa_Stream**)&pInputStream); \
err3: \
   OpcUa_Encoder_Close(pEncoder, &hEncoderContext); \
err2: \
   OpcUa_Encoder_Delete(&pEncoder); \
err1: \
   pOutputStream->Delete((OpcUa_Stream**)&pOutputStream); \
err0: \
   OpcUa_MessageContext_Clear(&cContext); \
   return uStatus; \
}

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_MEMORYSTREAM */
#endif /* _OpcUa_MemoryStream_H_ */
