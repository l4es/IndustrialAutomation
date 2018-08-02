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

#ifndef _OpcUa_SecureStream_H_
#define _OpcUa_SecureStream_H_ 1

struct _OpcUa_Connection;

#include <opcua_buffer.h>
#include <opcua_stream.h>
#include <opcua_securechannel_types.h>

OPCUA_BEGIN_EXTERN_C

/** @brief The start position of the message size in the transport stream */
#define OPCUA_TCP_PROTOCOL_BEGINOFMESSAGESIZE 4

/** @brief The size of the headers Type and Size fields marks the beginning of the encrypted data. */
#define OPCUA_TCP_PROTOCOL_MESSAGEHEADER_SIZE (sizeof(OpcUa_UInt32)*2)

/** @brief Types for security protocol messages. */
typedef enum _OpcUa_SecureStream_Type
{
    eOpcUa_SecureStream_Types_Invalid,
    eOpcUa_SecureStream_Types_StandardMessage,
    eOpcUa_SecureStream_Types_OpenSecureChannel,
    eOpcUa_SecureStream_Types_CloseSecureChannel,
    eOpcUa_SecureStream_Types_AbortMessage
}
OpcUa_SecureStream_Type;

/** @brief Defines for security protocol message types.
           MessageType(3) + IsFinal(1) + MessageSize(4)
*/
#define OPCUA_SECURESTREAM_MESSAGETYPE_OPN "OPNF0000"
#define OPCUA_SECURESTREAM_MESSAGETYPE_MSG "MSGF0000"
#define OPCUA_SECURESTREAM_MESSAGETYPE_CLO "CLOF0000"

/** @brief Lentgh of the message types. */
#define OPCUA_SECURESTREAM_MESSAGETYPE_LEN 8

/** @brief Stores state information for a secure stream. */
typedef struct _OpcUa_SecureStream
{
    /** @brief A checksum used to verify that the object is valid. */
    OpcUa_UInt32                SanityCheck;
    /** @brief How many times the buffer of the stream has been flushed to the underlying layer. */
    OpcUa_UInt32                uNoOfFlushes;
    /** @brief The non-secure stream that the stream is layered on top of. */
    OpcUa_Stream*               InnerStrm;
    /** @brief Identifies whether the stream is closed or not */
    OpcUa_Boolean               IsClosed;
    /** @brief Identifies whether the stream is locked or not */
    OpcUa_Boolean               IsLocked;
    /** @brief An array of buffers that store incoming multipart data in the stream. */
    OpcUa_Buffer*               Buffers;
    /** @brief The current count of buffers used in the stream. Must not exceed nMaxBuffers! Also index for writing.
               nBuffers is 1-based, index is 0 based. Last buffer is adressed by nBuffers-1 ( = nCurrentReadBuffer) */
    OpcUa_UInt32                nBuffers;
    /** @brief The maximum number of buffers in the stream (aka chunks per message). */
    OpcUa_UInt32                nMaxBuffers;
    /** @brief The index of the currently used buffer for reading. */
    OpcUa_UInt32                nCurrentReadBuffer;
    /** @brief The absolute position spanning all included buffers. Returned in GetPosition. */
    OpcUa_UInt32                nAbsolutePosition;

    /** @brief The Request Id the stream belongs to. */
    OpcUa_UInt32                RequestId;

    /** @brief The SecureChannelId that belongs to the stream */
    OpcUa_UInt32                SecureChannelId;
    /** @brief The secure channel for which the stream was created. */
    OpcUa_SecureChannel*        pSecureChannel;
    /** @brief Crypto provider for crypto operations if no secure channel exists. */
    OpcUa_CryptoProvider*       pAsymmetricCryptoProvider;

    /** @brief The type of the included secure channel message. */
    OpcUa_SecureStream_Type     eMessageType;
    /** @brief DER encoded bytes of the sender's X.509v3 certificate. */
    OpcUa_ByteString*           pSenderCertificate;
    /** @brief DER encoded bytes of the receiver's X.509v3 certificate. */
    OpcUa_ByteString*           pReceiverCertificate;
    /** @brief DER (PEM) encoded bytes of the receiver's or sender's private key. */
    OpcUa_Key*                  pPrivateKey;
    /** @brief DER encoded bytes of the sender's public key. */
    OpcUa_Key*                  pSenderPublicKey;
    /** @brief DER encoded bytes of the receiver's public key. */
    OpcUa_Key*                  pReceiverPublicKey;
    /** @brief DER encoded bytes of the receiver's certificate thumbprint. */
    OpcUa_ByteString*           pReceiverCertificateThumbprint;

    /** @brief The security mode to use for this stream. */
    OpcUa_MessageSecurityMode   eMessageSecurityMode;
    /** @brief This marks the beginning of the reserved end of write space. */
    OpcUa_UInt32                uFlushTrigger;
    /** @brief This marks the beginning of the reserved end of write space. */
    OpcUa_UInt32                uBeginOfRequestBody;
    /*! The size of an input block when using encryption. */
    OpcUa_UInt32                uPlainTextBlockSize;
    /*! The size of an output block when using encryption. */
    OpcUa_UInt32                uCipherTextBlockSize;
    /*! The size of the signature. */
    OpcUa_UInt32                uSignatureSize;
}
OpcUa_SecureStream;

/**
  @brief Creates a new stream to read a message from the connection.

  @param pTransportIstrm [in] The stream that provides access to the non-secure connection.
                              If this pointer is NULL then an error will be returned.

  @param pInputType     [out] The secure header type (SP, SO, SC, SM).
                              If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_CheckInputHeaderType(  OpcUa_InputStream*          pTransportIstrm,
                                                                        OpcUa_SecureMessageType*    pInputType          );

/**
  @brief Decodes the symmetric security header of the incoming stream.

  @param pTransportIstrm    [in]   The stream that provides access to the non-secure connection.
                                   If this pointer is NULL then an error will be returned.

  @param pSecureChannelId   [out]  The securechannel ID of the incoming stream.
                                   If this pointer is NULL then an error will be returned.
  @param pTokenId           [out]  The token ID of the incoming stream.
                                   If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_DecodeSymmetricSecurityHeader( OpcUa_InputStream*  pTransportIstrm,
                                                                                OpcUa_UInt32*       pSecureChannelId,
                                                                                OpcUa_UInt32*       pTokenId);

/**
  @brief Decodes the asymmetric security header of the incoming stream.

  @param pTransportIstrm                [in]  The stream that provides access to the non-secure connection.
                                              If this pointer is NULL then an error will be returned.

  @param pSecureChannelId               [out]  The securechannel ID of the incoming stream.
                                               If this pointer is NULL then an error will be returned.
  @param pSecurityPolicyUri             [out]  The security policy URI that was used to secure the incoming stream.
                                               If this pointer is NULL then an error will be returned.
  @param pSenderCertificate             [out]  The DER encoded bytes of the sender's X.509v3 certificate.
                                               If this pointer is NULL then an error will be returned.
  @param pReceiverCertificateThumbprint [out]  The DER encoded bytes of the receiver's certificate thumbprint.
                                               If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_DecodeAsymmetricSecurityHeader(OpcUa_InputStream*  pTransportIstrm,
                                                                                OpcUa_UInt32*       pSecureChannelId,
                                                                                OpcUa_String*       pSecurityPolicyUri,
                                                                                OpcUa_ByteString*   pSenderCertificate,
                                                                                OpcUa_ByteString*   pReceiverCertificateThumbprint);

/**
  @brief Decodes the sequence header of the incoming stream.

  @param pTransportIstrm    [in]  The stream that provides access to the non-secure connection.
                                  If this pointer is NULL then an error will be returned.

  @param pSequenceNumber    [out]  The sequence number provided by the sender.
                                   If this pointer is NULL then an error will be returned.
  @param pRequestId         [out]  The request ID provided by the sender.
                                   If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_DecodeSequenceHeader(  OpcUa_InputStream*  pTransportIstrm,
                                                                        OpcUa_UInt32*       pSequenceNumber,
                                                                        OpcUa_UInt32*       pRequestId      );

/**
  @brief Creates a new stream to read a standard message (MSG) from the connection.

  @param pCryptoProvider        [in]  The cryptoprovider used for cryptographic operations on the stream.
                                      If this pointer is NULL then an error will be returned.
  @param eMessageSecurityMode   [in]  The currently used message security mode of the secure channel.
  @param uMaxChunks             [in]  Maximum number of chunks allowed to be buffered by this stream.
  @param ppSecureIstrm          [out] The new input stream. If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_CreateInput(   OpcUa_CryptoProvider*       pCryptoProvider,
                                                                OpcUa_MessageSecurityMode   eMessageSecurityMode,
                                                                OpcUa_UInt32                uMaxChunks,
                                                                OpcUa_InputStream**         ppSecureIstrm);

/**
  @brief Appends a new message chunk from the transport stream to an existing secure stream.

  @param pTransportIstrm        [in] The stream that provides access to the non-secure connection and contains the new message chunk.
  @param pSecureIStream         [bi] The updated secure stream which contains the appended chunk.
  @param pSigningKey            [in] Key for verifying the streams content.
  @param pEncryptionKey         [in] Key for decrypting the streams content.
  @param pInitializationVector  [in] IV for decrypting the streams content.
  @param pCryptoProvider        [in] CryptoProvider providing the functions for the crypto operations.
  @param pSecureChannel         [in] The securechannel object of this connection.

*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_AppendInput(   OpcUa_InputStream*      pTransportIstrm,
                                                                OpcUa_InputStream*      pSecureIStream,
                                                                OpcUa_Key*              pSigningKey,
                                                                OpcUa_Key*              pEncryptionKey,
                                                                OpcUa_Key*              pInitializationVector,
                                                                OpcUa_CryptoProvider*   pCryptoProvider,
                                                                OpcUa_SecureChannel*    pSecureChannel);

/**
  @brief Creates a new stream to read an OpenSecureChannel message (OPN) from the connection.

  @param pCryptoProvider                [in]  The cryptoprovider used for cryptographic operations on the stream.
  @param eMessageSecurityMode           [in]  The currently used message security mode of the secure channel.
  @param pReceiverCertificate           [in]  The DER encoded bytes of the receiver's X.509v3 certificate.
  @param pReceiverPrivateKey            [in]  The DER (PEM) encoded bytes of the receiver's or sender's private key.
  @param pSenderCertificate             [in]  The DER encoded bytes of the sender's X.509v3 certificate.
  @param pReceiverCertificateThumbprint [in]  The DER encoded bytes of the receiver's certificate thumbprint.
  @param uMaxChunks                     [in]  Maximum number of chunks allowed to be buffered by this stream.
  @param ppSecureIstrm                  [out] The new input stream.
                                              If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_CreateOpenSecureChannelInput(  OpcUa_CryptoProvider*       pCryptoProvider,
                                                                                OpcUa_MessageSecurityMode   eMessageSecurityMode,
                                                                                OpcUa_ByteString*           pReceiverCertificate,
                                                                                OpcUa_Key*                  pReceiverPrivateKey,
                                                                                OpcUa_ByteString*           pSenderCertificate,
                                                                                OpcUa_ByteString*           pReceiverCertificateThumbprint,
                                                                                OpcUa_UInt32                uMaxChunks,
                                                                                OpcUa_InputStream**         ppSecureIstrm);

/**
    @brief Creates a new stream to write a standard message (MSG) to the connection.

    @param pInnerOstrm    [in]  The stream that provides access to the non-secure connection.
    @param eMessageType   [in]  Type of the message. Determines the stream header.
    @param uRequestId     [in]  The request ID of the request.
    @param pSecureChannel [in]  The securechannel ID of the securechannel.
    @param ppOstrm        [out] The new output stream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_CreateOutput(  OpcUa_OutputStream*         pInnerOstrm,
                                                                OpcUa_SecureStream_Type     eMessageType,
                                                                OpcUa_UInt32                uRequestId,
                                                                OpcUa_SecureChannel*        pSecureChannel,
                                                                OpcUa_OutputStream**        ppOstrm);

/**
  @brief Creates a new stream to write an OpenSecureChannel message (OPN) to the connection.

  @param pInnerOstrm                    [in]  The stream that provides access to the non-secure connection.
                                              If this pointer is NULL then an error will be returned.
  @param pSecureChannel                 [in]  The securechannel object of this connection.
  @param uRequestId                     [in]  The request ID of the request.
  @param pSecurityPolicyUri             [in]  The security policy URI that is used to secure the outgoing stream.
                                              If this pointer is NULL then an error will be returned.
  @param eMessageSecurityMode           [in]  The currently used message security mode of the secure channel.
                                              If this parameter is OpcUa_MessageSecurityMode_Invalid then an error will be returned.
  @param pCryptoProvider                [in]  The cryptoprovider used for cryptographic operations on the stream.
                                              If this pointer is NULL then an error will be returned.
  @param pSenderCertificate             [in]  The DER encoded bytes of the sender's X.509v3 certificate.
  @param pSenderPrivateKey              [in]  The DER (PEM) encoded bytes of the receiver's or sender's private key.
  @param pReceiverCertificate           [in]  The DER encoded bytes of the receiver's X.509v3 certificate.
  @param pReceiverCertificateThumbprint [in]  The DER encoded bytes of the receiver's X.509v3 certificate thumbprint.

  @param ppOstrm                        [out] The new output stream.
                                              If this pointer is NULL then an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_CreateOpenSecureChannelOutput( OpcUa_OutputStream*         pInnerOstrm,
                                                                                OpcUa_SecureChannel*        pSecureChannel,
                                                                                OpcUa_UInt32                uRequestId,
                                                                                OpcUa_String*               pSecurityPolicyUri,
                                                                                OpcUa_MessageSecurityMode   eMessageSecurityMode,
                                                                                OpcUa_CryptoProvider*       pCryptoProvider,
                                                                                OpcUa_ByteString*           pSenderCertificate,
                                                                                OpcUa_Key*                  pSenderPrivateKey,
                                                                                OpcUa_ByteString*           pReceiverCertificate,
                                                                                OpcUa_ByteString*           pReceiverCertificateThumbprint,
                                                                                OpcUa_OutputStream**        ppOstrm);

/**
  @brief Encrypts a given outputstream.

  @param pOstrm                 [bi]  The outputstream (plaintext) and after the function has been processed the encrypted outputstream.
                                      If this pointer is NULL then an error will be returned.
  @param pCryptoProvider        [in]  The cryptoprovider used for cryptographic operations on the stream.
  @param pCryptoKey             [in]  Asymmetric public key or symmetric key.
                                      If this pointer is NULL then an error will be returned.
  @param bUseSymmetricAlgorithm [in]  Indicates whether to use symmetric or asymmetric
                                      cryptographic algorithms to encrypt the output stream.
  @param pInitialVector         [in]  The initial vector that should be used in case of symmetric encryption.
                                      If bUseSymmetricAlgorithm is OpcUa_True then this pointer must not be NULL or an error will be returned.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_EncryptOutput( OpcUa_OutputStream*     pOstrm,
                                                                OpcUa_CryptoProvider*   pCryptoProvider,
                                                                OpcUa_Key*              pCryptoKey,
                                                                OpcUa_Boolean           bUseSymmetricAlgorithm,
                                                                OpcUa_Key*              pInitialVector);

/**
  @brief Calculates the total outputlength that is needed for encrypting a specific amount of data.

  @param uBufferLength          [in]  The length of the target buffer.
  @param pCryptoProvider        [in]  The cryptoprovider used for cryptographic operations on the stream.
                                      If this pointer is NULL then an error will be returned.
  @param pCryptoKey             [in]  Asymmetric public key or symmetric key.
                                      If this pointer is NULL then an error will be returned.
  @param bUseSymmetricAlgorithm [in]  Indicates whether to use symmetric or asymmetric
                                      cryptographic algorithms to encrypt the outputstream.
  @param pInitialVector         [in]  The initial vector that should be used in case of symmetric
                                      encryption.
  @param pOutputLength          [out] The resulting total outputlength that is needed for the encrypted data.
                                      If bUseSymmetricAlgorithm is OpcUa_True then this pointer must not be NULL or an error will be returned.
*/
OpcUa_StatusCode OpcUa_SecureStream_CalculateEncryptionOutputLength(    OpcUa_UInt32            uBufferLength,
                                                                        OpcUa_CryptoProvider*   pCryptoProvider,
                                                                        OpcUa_Key*              pCryptoKey,
                                                                        OpcUa_Boolean           bUseSymmetricAlgorithm,
                                                                        OpcUa_Key*              pInitialVector,
                                                                        OpcUa_UInt32*           pOutputLength);

/**
  @brief signs a given outputstream.

  @param pOstrm                 [bi]  The outputstream and after the function has been processed the signed outputstream.
                                      If this pointer is NULL then an error will be returned.
  @param pCryptoProvider        [in]  The cryptoprovider used for cryptographic operations on the stream.
  @param pCryptoKey             [in]  Asymmetric private key or symmetric key.
                                      If this pointer is NULL then an error will be returned.
  @param bUseSymmetricAlgorithm [in]  Indicates whether to use symmetric or asymmetric
                                      cryptographic algorithms to encrypt the outputstream.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_SecureStream_SignOutput(    OpcUa_OutputStream*     pOstrm,
                                                                OpcUa_CryptoProvider*   pCryptoProvider,
                                                                OpcUa_Key*              pCryptoKey,
                                                                OpcUa_Boolean           bUseSymmetricAlgorithm);

/**
  @brief Calculates the total outputlength that is needed when signing a specific amount of data.

  @param uBufferLength          [in]  The length of the target buffer.
  @param pCryptoProvider        [in]  The cryptoprovider used for cryptographic operations on the stream.
                                      If this pointer is NULL then an error will be returned.
  @param pCryptoKey             [in]  Asymmetric public key or symmetric key.
                                      If this pointer is NULL then an error will be returned.
  @param bUseSymmetricAlgorithm [in]  Indicates whether to use symmetric or asymmetric
                                      cryptographic algorithms to encrypt the outputstream.
  @param pOutputLength          [out] The resulting total outputlength that is needed for the signing data.
                                      If this pointer is NULL then an error will be returned.
*/
OpcUa_StatusCode OpcUa_SecureStream_CalculateSignatureOutputLength( OpcUa_UInt32            uBufferLength,
                                                                    OpcUa_CryptoProvider*   pCryptoProvider,
                                                                    OpcUa_Key*              pCryptoKey,
                                                                    OpcUa_Boolean           bUseSymmetricAlgorithm,
                                                                    OpcUa_UInt32*           pOutputLength);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_SecureStream_H_ */
