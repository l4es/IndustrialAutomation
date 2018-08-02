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
#include <opcua_mutex.h>
#include <opcua_string.h>

/* stackcore */
#include <opcua_stream.h>
#include <opcua_securechannel.h>
#include <opcua_binaryencoder.h>
#include <opcua_builtintypes.h>
#include <opcua_connection.h>
#include <opcua_tcpstream.h>

/* security */
#include <opcua_securechannel_types.h>
#include <opcua_tcpsecurechannel.h>
#include <opcua_securestream.h>

/* (de-)activate internal optimizations; use old (tested) behavior if complications arise */

/* Setting both options to OPCUA_CONFIG_NO implies in-place decryption! Make sure, the underlying crypto engine can handle this! */
/* optimization; set to OPCUA_CONFIG_YES for old behavior, which includes additional allocation, memcpy and free */
#define OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT OPCUA_CONFIG_NO
/* optimization; set to OPCUA_CONFIG_YES for old behavior, which includes additional allocation, memcpy and free */
#define OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT  OPCUA_CONFIG_NO

/* optimization; set to OPCUA_CONFIG_YES for old behavior, which includes additional allocation, memcpy and free */
#define OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT OPCUA_CONFIG_NO
/* optimization; set to OPCUA_CONFIG_YES for old behavior, which includes additional allocation, memcpy and free */
#define OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT  OPCUA_CONFIG_NO

/*============================================================================
 * OpcUa_SecureStream_SanityCheck
 *===========================================================================*/
#define OpcUa_SecureStream_SanityCheck 0x725BED4F

/*============================================================================
 * INTERNAL FUNCTIONS
 *===========================================================================*/

/**
 * @brief INTERNAL FUNCTION: Prepares the stream for sending it to the socket.
 *
 *         To be called by SecureStream_Flush. Prepares the buffered data of an
 *         outgoing secure stream for sending.
 *
 * @param pOstrm    [in/out]    The stream that provides access to the non-secure connection.
 *                              If this pointer is NULL then an error will be returned.
 * @return the OPC UA Status code.
*/
static OpcUa_StatusCode OpcUa_SecureStream_PrepareForSending(OpcUa_OutputStream*    pOstrm,
                                                             OpcUa_CryptoProvider*  pCryptoProvider,
                                                             OpcUa_Key*             pSigningKey,
                                                             OpcUa_Key*             pEncryptionKey,
                                                             OpcUa_Key*             pInitializationVector,
                                                             OpcUa_UInt32           uTokenId);

/**
 *  @brief INTERNAL FUNCTION: Encodes asymmetric security header into a stream.
 *
 *  @param pOstrm                           [bi]    The stream that provides access to the non-secure connection.
 *                                                  This pointer must not be NULL.
 *
 *  @param eStreamType                      [in]    The type of the passed in stream (OpenSecureChannel, StandardMessage or CloseSecureChannel).
 *  @param uSecureChannelId                 [in]    The secure channel ID that should be encoded in the stream.
 *  @param uRequestId                       [in]    The request ID that should be encoded in the stream.
 *  @param pSecurityPolicyUri               [in]    The security policy URI that should be encoded in the stream.
 *                                                  This pointer must not be NULL.
 *  @param pSenderCertificate               [in]    The DER encoded bytestring of the sender's X.509v3 certificate.
 *                                                  This pointer must not be NULL.
 *  @param pReceiverCertificateThumbprint   [in]    The DER encoded bytestring of the sender's X.509v3 certificate.
 *                                                  This pointer must not be NULL.
 * @return the OPC UA Status code.
 */
static OpcUa_StatusCode OpcUa_SecureStream_EncodeAsymmetricSecurityHeader(  OpcUa_OutputStream*         pOstrm,
                                                                            OpcUa_SecureStream_Type     eStreamType,
                                                                            OpcUa_MessageSecurityMode   eMessageSecurityMode,
                                                                            OpcUa_UInt32                uSecureChannelId,
                                                                            OpcUa_UInt32                uRequestId,
                                                                            OpcUa_String*               pSecurityPolicyUri,
                                                                            OpcUa_ByteString*           pSenderCertificate,
                                                                            OpcUa_ByteString*           pReceiverCertificateThumbprint);

/**
  @brief INTERNAL FUNCTION: Encodes symmetric security header into a stream.

  @param pOstrm             [in/out]  The stream that provides access to the non-secure connection.
                                      This pointer must not be NULL.

  @param eStreamType        [in]  The type of the passed in stream (OpenSecureChannel, StandardMessage or CloseSecureChannel).
  @param uSecureChannelId   [in]  The secure channel ID that should be encoded in the stream.
  @param uRequestId         [in]  The request ID that should be encoded in the stream.
  @param uTokenId           [in]  The currently used token ID for the secure stream.
  @return the OPC UA Status code.
*/
static OpcUa_StatusCode OpcUa_SecureStream_EncodeSymmetricSecurityHeader(   OpcUa_OutputStream*     pOstrm,
                                                                            OpcUa_SecureStream_Type eStreamType,
                                                                            OpcUa_UInt32            uSecureChannelId,
                                                                            OpcUa_UInt32            uRequestId,
                                                                            OpcUa_UInt32            uTokenId);

/**
  @brief INTERNAL FUNCTION: Writes the local buffer content into the underlying stream.
*/
static OpcUa_StatusCode OpcUa_SecureStream_Flush(   OpcUa_OutputStream* pOstrm,
                                                    OpcUa_Boolean       bLastCall);

/**
  @brief INTERNAL FUNCTION: Closes a given input stream. Is called by OpcUa_SecureStream_Close.
*/
static OpcUa_StatusCode OpcUa_SecureInputStream_Close(  OpcUa_InputStream* pIstrm);

/**
  @brief INTERNAL FUNCTION: Closes a given output stream. Is called by OpcUa_SecureStream_Close.
*/
static OpcUa_StatusCode OpcUa_SecureOutputStream_Close( OpcUa_OutputStream* pOstrm);

/**
  @brief INTERNAL FUNCTION: Calculates how many bytes of real data fit into a chunk (signature, encryption)
*/
static OpcUa_StatusCode OpcUa_SecureStream_CalculateFlushTrigger(   OpcUa_SecureStream* pSecureStream,
                                                                    OpcUa_UInt32        pBufferLength);

/**
  @brief INTERNAL FUNCTION: Decrypts a given buffer.
*/
static OpcUa_StatusCode OpcUa_SecureStream_DecryptInputBuffer(  OpcUa_Buffer*           pEncryptedBuffer,
                                                                OpcUa_CryptoProvider*   pCryptoProvider,
                                                                OpcUa_Key*              pCryptoKey,
                                                                OpcUa_Boolean           bUseSymmetricAlgorithm,
                                                                OpcUa_Key*              pInitialVector);

/**
  @brief INTERNAL FUNCTION: Verifies the signature of a given inputstream.

         If the validation fails, then the statuscode OpcUa_BadSignatureInvalid is returned.

  @param pStream                [in]  The stream containing the data to verify.
  @param pBuffer                [in]  The buffer containing the signed message (including the signature).
                                      If this pointer is NULL then an error will be returned.
  @param pCryptoKey             [in]  Asymmetric public key or symmetric key.
  @param bUseSymmetricAlgorithm [in]  Indicates whether to use symmetric or asymmetric
                                      cryptographic algorithms to decrypt the inputstream.
* @return the OPC UA Status code.
*/
static OpcUa_StatusCode OpcUa_SecureStream_VerifyInputBuffer(   OpcUa_SecureStream*     pStream,
                                                                OpcUa_Buffer*           pBuffer,
                                                                OpcUa_CryptoProvider*   pCryptoProvider,
                                                                OpcUa_Key*              pCryptoKey,
                                                                OpcUa_Boolean           bUseSymmetricAlgorithm);

/*============================================================================
 * OpcUa_SecureStream_GetAsymmetricEncryptionBlockSizes
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetAsymmetricEncryptionBlockSizes(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Key*            a_pPublicKey,
    OpcUa_UInt32*         a_pPlainTextBlockSize,
    OpcUa_UInt32*         a_pCipherTextBlockSize)
{
    OpcUa_UInt32 uSizeInBits = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetAsymmetricEncryptionBlockSizes");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPublicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pPlainTextBlockSize);
    OpcUa_ReturnErrorIfArgumentNull(a_pCipherTextBlockSize);

    *a_pPlainTextBlockSize = 0;
    *a_pCipherTextBlockSize = 0;

    switch (a_pProvider->AsymmetricEncryptionAlgorithmId)
    {
        case OpcUa_P_NoEncryption_Id:
        {
            *a_pPlainTextBlockSize = *a_pCipherTextBlockSize = 1;
            break;
        }
        case OpcUa_P_RSA_PKCS1_V15_Id:
        case OpcUa_P_RSA_OAEP_Id:
        case OpcUa_P_RSA_OAEP_SHA256_Id:
        {
            uStatus = OpcUa_Crypto_GetAsymmetricKeyLength(a_pProvider, *a_pPublicKey, &uSizeInBits);
            OpcUa_GotoErrorIfBad(uStatus);

            if (uSizeInBits/8 <= a_pProvider->AsymmetricEncryptionOverhead)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }

            *a_pPlainTextBlockSize = uSizeInBits/8 - a_pProvider->AsymmetricEncryptionOverhead;
            *a_pCipherTextBlockSize = uSizeInBits/8;
            break;
        }
        default:
        {
            uStatus = OpcUa_BadNotSupported;
            break;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_GetAsymmetricSignatureSize
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetAsymmetricSignatureSize(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Key*            a_pPublicKey,
    OpcUa_UInt32*         a_pSignatureSize)
{
    OpcUa_UInt32 uSizeInBits = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetAsymmetricSignatureSize");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPublicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignatureSize);

    *a_pSignatureSize = 0;

    switch (a_pProvider->AsymmetricSignatureAlgorithmId)
    {
        case OpcUa_P_NoSignature_Id:
        {
            *a_pSignatureSize = 0;
            break;
        }
        case OpcUa_P_RSA_PKCS1_V15_SHA1_Id:
        case OpcUa_P_RSA_PKCS1_V15_SHA256_Id:
        case OpcUa_P_RSA_PSS_SHA256_Id:
        {
            uStatus = OpcUa_Crypto_GetAsymmetricKeyLength(a_pProvider, *a_pPublicKey, &uSizeInBits);
            OpcUa_GotoErrorIfBad(uStatus);

            *a_pSignatureSize = uSizeInBits/8;
            break;
        }
        default:
        {
            uStatus = OpcUa_BadNotSupported;
            break;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureStream_GetSymmetricSignatureSize
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetSymmetricSignatureSize(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_UInt32*         a_pSignatureSize)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetSymmetricSignatureSize");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignatureSize);

    *a_pSignatureSize = 0;

    switch (a_pProvider->SymmetricSignatureAlgorithmId)
    {
        case OpcUa_P_NoSignature_Id:
        {
            *a_pSignatureSize = 0;
            break;
        }
        case OpcUa_P_HMAC_SHA1_Id:
        {
            *a_pSignatureSize = 20;
            break;
        }

        case OpcUa_P_HMAC_SHA256_Id:
        {
            *a_pSignatureSize = 32;
            break;
        }
        default:
        {
            uStatus = OpcUa_BadNotSupported;
            break;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_GetSymmetricEncryptionBlockSizes
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetSymmetricEncryptionBlockSizes(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_UInt32*         a_pPlainTextBlockSize,
    OpcUa_UInt32*         a_pCipherTextBlockSize)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetSymmetricEncryptionBlockSizes");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPlainTextBlockSize);
    OpcUa_ReturnErrorIfArgumentNull(a_pCipherTextBlockSize);

    *a_pPlainTextBlockSize = 0;
    *a_pCipherTextBlockSize = 0;

    switch (a_pProvider->SymmetricEncryptionAlgorithmId)
    {
        case OpcUa_P_NoEncryption_Id:
        {
            *a_pPlainTextBlockSize = *a_pCipherTextBlockSize = 1;
            break;
        }
        case OpcUa_P_AES_128_CBC_Id:
        case OpcUa_P_AES_256_CBC_Id:
        {
            *a_pPlainTextBlockSize = *a_pCipherTextBlockSize = 16;
            break;
        }
        default:
        {
            *a_pPlainTextBlockSize = *a_pCipherTextBlockSize = 1;
            break;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_GetPaddingSize
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetPaddingSize(
    OpcUa_SecureStream* a_pSecureStream,
    OpcUa_UInt32        a_uEncryptedDataSize,
    OpcUa_UInt32*       a_pPaddingSize)
{
    OpcUa_UInt32 uPlainTextSize = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetPaddingSize");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pPaddingSize);

    *a_pPaddingSize = 0;

    uPlainTextSize += a_uEncryptedDataSize;
    uPlainTextSize += a_pSecureStream->uSignatureSize;
    uPlainTextSize += a_pSecureStream->uPlainTextBlockSize > 256 ? 2 : 1;

    if (uPlainTextSize%a_pSecureStream->uPlainTextBlockSize != 0)
    {
        *a_pPaddingSize = a_pSecureStream->uPlainTextBlockSize - (uPlainTextSize%a_pSecureStream->uPlainTextBlockSize);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureStream_GetPublicKey
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetPublicKey(
    OpcUa_CryptoProvider* a_pCryptoProvider,
    OpcUa_ByteString*     a_pCertificate,
    OpcUa_Key**           a_ppPublicKey)
{
    OpcUa_Key* pPublicKey = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetPublicKey");

    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_ppPublicKey);

    *a_ppPublicKey = OpcUa_Null;

    pPublicKey = (OpcUa_Key*)OpcUa_Alloc(sizeof(OpcUa_Key));
    OpcUa_ReturnErrorIfAllocFailed(pPublicKey);
    OpcUa_Key_Initialize(pPublicKey);

    /* Get Length */
    uStatus = a_pCryptoProvider->GetPublicKeyFromCert(  a_pCryptoProvider,
                                                        a_pCertificate,
                                                        OpcUa_Null,
                                                        pPublicKey);
    OpcUa_GotoErrorIfBad(uStatus);

    if(OPCUA_CRYPTO_KEY_ISNOHANDLE(pPublicKey))
    {
        /* only alloc, if the key isn't a handle */
        pPublicKey->Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pPublicKey->Key.Length);
        OpcUa_GotoErrorIfAllocFailed(pPublicKey->Key.Data);
        uStatus = a_pCryptoProvider->GetPublicKeyFromCert(  a_pCryptoProvider,
                                                            a_pCertificate,
                                                            OpcUa_Null,
                                                            pPublicKey);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    *a_ppPublicKey = pPublicKey;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if (pPublicKey != OpcUa_Null)
    {
        if (pPublicKey->Key.Data != OpcUa_Null)
        {
            OpcUa_Free(pPublicKey->Key.Data);
        }

        OpcUa_Free(pPublicKey);
    }

OpcUa_FinishErrorHandling;
}


/*============================================================================
 * IMPLEMENTATION OF STREAM "INTERFACE"
 *===========================================================================*/

/**
  @brief Reads data from a given input stream. Implements Read of the OpcUa_InputStream "interface"

*/
static OpcUa_StatusCode OpcUa_SecureStream_Read(    OpcUa_InputStream*             pIstrm,
                                                    OpcUa_Byte*                    pTargetBuffer,
                                                    OpcUa_UInt32*                  pCount);

/**
  @brief @brief Writes data from a given output stream. Implements Write of the OpcUa_OutputStream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_Write(   OpcUa_OutputStream* pOstrm,
                                                    OpcUa_Byte*         pBuffer,
                                                    OpcUa_UInt32        uCount);

/**
  @brief Closes a given stream. Implements Close of the OpcUa_Stream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_Close(   OpcUa_Stream* pStrm);

/**
  @brief Deletes a given stream. Implements Delete of the OpcUa_Stream "interface".
*/
static OpcUa_Void OpcUa_SecureStream_Delete(    OpcUa_Stream** pStrm);

/**
  @brief Gets the position of given stream. Implements GetPosition of the OpcUa_Stream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_GetPosition( OpcUa_Stream* pStrm,
                                                        OpcUa_UInt32* pPosition);

/**
  @brief Sets the position of given stream. Implements SetPosition of the OpcUa_Stream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_SetPosition( OpcUa_Stream*   pStrm,
                                                        OpcUa_UInt32    uPosition);

/**
  @brief Attaches a buffer to a given stream. Implements AttachBuffer of the OpcUa_Stream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_AttachBuffer(    OpcUa_Stream*   pStrm,
                                                            OpcUa_Buffer*   pBuffer);

/**
  @brief Detaches a buffer from a given stream. Implements DetachBuffer of the OpcUa_Stream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_DetachBuffer(    OpcUa_Stream*   pStrm,
                                                            OpcUa_Buffer*   pBuffer);

/**
  @brief Gets the chunklength that is used by a given stream. Implements DetachBuffer of the OpcUa_Stream "interface".
*/
static OpcUa_StatusCode OpcUa_SecureStream_GetChunkLength(  OpcUa_Stream*   pStrm,
                                                            OpcUa_UInt32*   puLength);

/*============================================================================
 * OpcUa_SecureStream_PrepareForSending
 *===========================================================================*/
/* To be called by SecureStream_Flush. Prepares the buffered data of an outgoing secure stream for sending. */
/* SecureChannel must be locked during the sending process! */
static OpcUa_StatusCode OpcUa_SecureStream_PrepareForSending(OpcUa_OutputStream*    a_pOstrm,
                                                             OpcUa_CryptoProvider*  a_pCryptoProvider,
                                                             OpcUa_Key*             a_pSigningKey,
                                                             OpcUa_Key*             a_pEncryptionKey,
                                                             OpcUa_Key*             a_pInitializationVector,
                                                             OpcUa_UInt32           a_uTokenId)
{
    OpcUa_SecureStream*         pSecureStream           = OpcUa_Null;
    OpcUa_UInt32                uStartOfEncryption      = 0;
    OpcUa_UInt32                uMessageLength          = 0;
    OpcUa_UInt32                uSequenceNumber         = OPCUA_SECURECHANNEL_STARTING_SEQUENCE_NUMBER;
    OpcUa_Boolean               bUsingAsymmetricKeys    = OpcUa_False;
    OpcUa_MessageSecurityMode   eSecurityMode           = OpcUa_MessageSecurityMode_SignAndEncrypt;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "PrepareForSending");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);

    pSecureStream = (OpcUa_SecureStream*)a_pOstrm->Handle;
    OpcUa_ReturnErrorIfArgumentNull(pSecureStream);

    if(pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel)
    {
        bUsingAsymmetricKeys = OpcUa_True;
    }
    else
    {
        if(!(pSecureStream->pSecureChannel->IsOpen(pSecureStream->pSecureChannel)))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureStream_PrepareForSending: SecureStream belongs to closed secure channel!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelClosed);
        }
    }

    uSequenceNumber = pSecureStream->pSecureChannel->GetSequenceNumber(pSecureStream->pSecureChannel);

    eSecurityMode = pSecureStream->eMessageSecurityMode;

    /*** calculate the start position of encryption ***/
    uStartOfEncryption = pSecureStream->uBeginOfRequestBody - OPCUA_TCP_PROTOCOL_MESSAGEHEADER_SIZE;

    if(pSecureStream->eMessageType != eOpcUa_SecureStream_Types_OpenSecureChannel)
    {
        /* set the position of the stream to the message length field to update the token id and sequence number */
        uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uStartOfEncryption - sizeof(OpcUa_UInt32));
        OpcUa_GotoErrorIfBad(uStatus);

        /* Encode updated token id */
        uStatus = OpcUa_UInt32_BinaryEncode(a_uTokenId, a_pOstrm);
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        /* set the position of the stream to the message length field to update the sequence number */
        uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uStartOfEncryption);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* encode the sequence number in the stream */
    uStatus = OpcUa_UInt32_BinaryEncode(uSequenceNumber, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* add the signature */
    if(eSecurityMode != OpcUa_MessageSecurityMode_None)
    {
        OpcUa_UInt32 uActualSize = pSecureStream->Buffers[0].EndOfData + pSecureStream->uSignatureSize;

        if(eSecurityMode == OpcUa_MessageSecurityMode_SignAndEncrypt)
        {
            OpcUa_UInt32 ii;
            OpcUa_UInt32 uPaddingSize;
            OpcUa_UInt32 uEncryptedDataSize;
            OpcUa_Byte   bPadding;

            /** add padding ***/
            uStatus = OpcUa_SecureStream_GetPaddingSize(pSecureStream,
                                                        pSecureStream->Buffers[0].EndOfData - uStartOfEncryption,
                                                        &uPaddingSize);
            OpcUa_GotoErrorIfBad(uStatus);

            /* set the position to the end of data */
            uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], pSecureStream->Buffers[0].EndOfData);
            OpcUa_GotoErrorIfBad(uStatus);

            bPadding = (OpcUa_Byte)uPaddingSize;
            for (ii = 0; ii <= uPaddingSize; ii++)
            {
                uStatus = OpcUa_Buffer_Write(&pSecureStream->Buffers[0], &bPadding, 1);
                OpcUa_GotoErrorIfBad(uStatus);
            }

            /* add extra padding byte if present */
            if (pSecureStream->uPlainTextBlockSize>256)
            {
                bPadding = (OpcUa_Byte)(uPaddingSize/256);
                uStatus = OpcUa_Buffer_Write(&pSecureStream->Buffers[0], &bPadding, 1);
                OpcUa_GotoErrorIfBad(uStatus);
            }

            uActualSize = pSecureStream->Buffers[0].EndOfData + pSecureStream->uSignatureSize;
            uEncryptedDataSize = ((uActualSize - uStartOfEncryption)/pSecureStream->uPlainTextBlockSize)*pSecureStream->uCipherTextBlockSize;
            uActualSize = uEncryptedDataSize + uStartOfEncryption;
        }

        /* encode the length in the stream */
        uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], OPCUA_TCP_PROTOCOL_BEGINOFMESSAGESIZE);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_UInt32_BinaryEncode(uActualSize, a_pOstrm);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_SecureStream_SignOutput(a_pOstrm,
                                                a_pCryptoProvider,
                                                a_pSigningKey, /* private key for asymmetric, signing key for symmetric */
                                                (OpcUa_Boolean)(!bUsingAsymmetricKeys));
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /*** check message security mode ***/
    switch(eSecurityMode)
    {
    case OpcUa_MessageSecurityMode_None:
        {
            /*** update message size in the header ***/
            /* Header | Body */
            /* message length is the same as the buffer length since it will not be changed until it is sent */
            uMessageLength = pSecureStream->Buffers[0].EndOfData;

            /* set the position of the stream to the message length field to update the value */
            uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], OPCUA_TCP_PROTOCOL_BEGINOFMESSAGESIZE);
            OpcUa_GotoErrorIfBad(uStatus);

            /* encode the length in the stream */
            uStatus = OpcUa_UInt32_BinaryEncode(uMessageLength, a_pOstrm);
            break;
        }
    case OpcUa_MessageSecurityMode_Sign:
        {
            /* signing is done above! */
            break;
        }
    case OpcUa_MessageSecurityMode_SignAndEncrypt:
        {
            /* set the position to the beginning of encrypted data */
            uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uStartOfEncryption);
            OpcUa_GotoErrorIfBad(uStatus);

            /* encrypt secure stream */
            uStatus = OpcUa_SecureStream_EncryptOutput( a_pOstrm,
                                                        a_pCryptoProvider,
                                                        a_pEncryptionKey, /* (bUsingAsymmetricKeys) ? pSecureStream->pReceiverPublicKey : &pSecureStream->pKeyset->EncryptionKey */
                                                        (OpcUa_Boolean)(!bUsingAsymmetricKeys),
                                                        a_pInitializationVector);
            break;
        }
    default:
        {
            uStatus = OpcUa_Bad;
        }
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_Read
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_Read(    OpcUa_InputStream*             a_pIstrm,
                                                    OpcUa_Byte*                    a_pTargetBuffer,
                                                    OpcUa_UInt32*                  a_pCount)
{
    OpcUa_SecureStream* pSecureStream       = OpcUa_Null;
    OpcUa_UInt32        uBytesToRead        = 0;
    OpcUa_UInt32        uBytesLeftToRead    = 0;
    OpcUa_Boolean       bReadAgain          = OpcUa_True;
    OpcUa_Byte*         pTargetBuffer       = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Read");

    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pTargetBuffer);
    OpcUa_ReturnErrorIfArgumentNull(a_pCount);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureStream, a_pIstrm, Read);

    uBytesLeftToRead    = *a_pCount;
    pTargetBuffer       = a_pTargetBuffer;

    pSecureStream = (OpcUa_SecureStream*)a_pIstrm->Handle;

    /* verify stream state */
    if(pSecureStream->IsClosed)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    do
    {
        /* set available amount of data as maximum */
        uBytesToRead = pSecureStream->Buffers[pSecureStream->nCurrentReadBuffer].EndOfData - pSecureStream->Buffers[pSecureStream->nCurrentReadBuffer].Position;

        if(uBytesToRead >= uBytesLeftToRead)
        {
            /* ok - all of the bytes requested are in one buffer */
            uBytesToRead = uBytesLeftToRead;
        }
        else
        {
            /* not all can be delivered */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureStream_Read: end of buffer reached: %u requested, %u available in buffer; need to skip)!\n", uBytesLeftToRead, uBytesToRead);
        }

        /* no data left in buffer, trigger skipping */
        if(uBytesToRead > 0)
        {
            /* read requested data */
            uStatus = OpcUa_Buffer_Read( &pSecureStream->Buffers[pSecureStream->nCurrentReadBuffer],
                                         pTargetBuffer,
                                         &uBytesToRead);
            OpcUa_GotoErrorIfBad(uStatus);

            /* prepare for next read */
            uBytesLeftToRead -= uBytesToRead;
        }

        if((uBytesLeftToRead == 0) || (pSecureStream->nCurrentReadBuffer >= (pSecureStream->nBuffers - 1)))
        {
            bReadAgain = OpcUa_False;
        }
        else
        {
            /* skip to next buffer */
            pSecureStream->nCurrentReadBuffer++;
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureStream_Read: Buffer skip to index %u of %u!\n", pSecureStream->nCurrentReadBuffer, pSecureStream->nBuffers);

            pTargetBuffer += uBytesToRead;
        }
    } while(bReadAgain != OpcUa_False);

    if(uBytesLeftToRead > 0)
    {
        uStatus = OpcUa_BadEndOfStream;
    }

    pSecureStream->nAbsolutePosition += *a_pCount - uBytesLeftToRead;

    *a_pCount -= uBytesLeftToRead;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_EncodeAsymmetricSecurityHeader
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_EncodeAsymmetricSecurityHeader(  OpcUa_OutputStream*         a_pOstrm,
                                                                            OpcUa_SecureStream_Type     a_eStreamType,
                                                                            OpcUa_MessageSecurityMode   a_eMessageSecurityMode,
                                                                            OpcUa_UInt32                a_uSecureChannelId,
                                                                            OpcUa_UInt32                a_uRequestId,
                                                                            OpcUa_String*               a_pSecurityPolicyUri,
                                                                            OpcUa_ByteString*           a_pSenderCertificate,
                                                                            OpcUa_ByteString*           a_pReceiverCertificateThumbprint)
{
    OpcUa_ByteString    NullByteString  = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "EncodeAsymmetricSecurityHeader");

    switch(a_eStreamType)
    {
    case eOpcUa_SecureStream_Types_OpenSecureChannel:
        {
            uStatus = a_pOstrm->Write(  a_pOstrm,
                                        (OpcUa_Byte*)OPCUA_SECURESTREAM_MESSAGETYPE_OPN,
                                        OPCUA_SECURESTREAM_MESSAGETYPE_LEN);

            break;
        }
    default:
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
        }
    }
    OpcUa_GotoErrorIfBad(uStatus);

    /* SecureChannelId */
    uStatus = OpcUa_UInt32_BinaryEncode(a_uSecureChannelId, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* SecurityUri */
    uStatus = OpcUa_String_BinaryEncode(a_pSecurityPolicyUri, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    switch(a_eMessageSecurityMode)
    {
    case OpcUa_MessageSecurityMode_None:
        {
            /* SenderCert */
            uStatus = OpcUa_ByteString_BinaryEncode(&NullByteString, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);

            /* ReceiverCertThumbprint */
            uStatus = OpcUa_ByteString_BinaryEncode(&NullByteString, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);

            break;
        }
    case OpcUa_MessageSecurityMode_SignAndEncrypt:
        {
            /* SenderCert */
            uStatus = OpcUa_ByteString_BinaryEncode(a_pSenderCertificate, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);

            /* ReceiverCertThumbprint */
            uStatus = OpcUa_ByteString_BinaryEncode(a_pReceiverCertificateThumbprint, a_pOstrm);
            OpcUa_ReturnErrorIfBad(uStatus);

            break;
        }
    default:
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
        }
    }

    /* SequenceNumber */
    /* NOTE: Sequence number is set in the prepareForSending function */
    uStatus = OpcUa_UInt32_BinaryEncode(0, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* RequestId */
    uStatus = OpcUa_UInt32_BinaryEncode(a_uRequestId, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_EncodeSymmetricSecurityHeader
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_EncodeSymmetricSecurityHeader(   OpcUa_OutputStream*     a_pOstrm,
                                                                            OpcUa_SecureStream_Type a_eStreamType,
                                                                            OpcUa_UInt32            a_uSecureChannelId,
                                                                            OpcUa_UInt32            a_uRequestId,
                                                                            OpcUa_UInt32            a_uTokenId)
{
    OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "EncodeSymmetricSecurityHeader");

    switch(a_eStreamType)
    {
    case eOpcUa_SecureStream_Types_StandardMessage:
        {
            uStatus = a_pOstrm->Write(  a_pOstrm,
                                        (OpcUa_Byte*)OPCUA_SECURESTREAM_MESSAGETYPE_MSG,
                                        OPCUA_SECURESTREAM_MESSAGETYPE_LEN);
            break;
        }
    case eOpcUa_SecureStream_Types_CloseSecureChannel:
        {
            uStatus = a_pOstrm->Write(  a_pOstrm,
                                        (OpcUa_Byte*)OPCUA_SECURESTREAM_MESSAGETYPE_CLO,
                                        OPCUA_SECURESTREAM_MESSAGETYPE_LEN);
            break;
        }
    default:
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
        }
    }
    OpcUa_GotoErrorIfBad(uStatus);

    /* SecureChannelId */
    uStatus = OpcUa_UInt32_BinaryEncode(a_uSecureChannelId, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* TokenId */
    uStatus = OpcUa_UInt32_BinaryEncode(a_uTokenId, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* SequenceNumber */
    /* NOTE: Sequence number is set in the prepareForSending function */
    uStatus = OpcUa_UInt32_BinaryEncode(0, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* RequestId */
    uStatus = OpcUa_UInt32_BinaryEncode(a_uRequestId, a_pOstrm);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_Flush
 *===========================================================================*/
/* write the local buffer content into the underlying stream */
static OpcUa_StatusCode OpcUa_SecureStream_Flush(   OpcUa_OutputStream* a_pOstrm,
                                                    OpcUa_Boolean       a_bLastCall)
{
    OpcUa_SecureStream*     pSecureStream           = OpcUa_Null;
    OpcUa_CryptoProvider*   pCryptoProvider         = OpcUa_Null;
    OpcUa_Key*              pSigningKey             = OpcUa_Null;
    OpcUa_Key*              pEncryptionKey          = OpcUa_Null;
    OpcUa_Key*              pInitializationVector   = OpcUa_Null;
    OpcUa_UInt32            uTokenId                = 0;
    OpcUa_SecureChannel*    pSecureChannel          = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Flush");

    OpcUa_ReferenceParameter(a_bLastCall);

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureStream, a_pOstrm, Flush);

    pSecureStream = (OpcUa_SecureStream*)a_pOstrm->Handle;
    pSecureChannel = pSecureStream->pSecureChannel;

    /* verify stream state */
    if(pSecureStream->IsClosed)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(!OpcUa_Buffer_IsEmpty(&(pSecureStream->Buffers[0])))
    {
        /* set chunk flag if more data is to come. */
        if(a_bLastCall == OpcUa_False)
        {
            pSecureStream->Buffers[0].Data[3] = 'C';
        }

        if(pSecureStream->IsLocked == OpcUa_False)
        {
            uStatus = pSecureChannel->LockWriteMutex(pSecureChannel);
            OpcUa_GotoErrorIfBad(uStatus);
            pSecureStream->IsLocked = OpcUa_True;
        }

        if(pSecureChannel->TransportConnection == OpcUa_Null)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadNotConnected);
        }

        /* check, which security set should be used */
        if(pSecureStream->eMessageType != eOpcUa_SecureStream_Types_OpenSecureChannel)
        {
            /* got a secure channel, retrieve current security set */
            OpcUa_SecurityKeyset* pSendingKeyset = OpcUa_Null;

            uStatus = pSecureChannel->GetCurrentSecuritySet(pSecureChannel,
                                                            &uTokenId,
                                                            OpcUa_Null,
                                                            &pSendingKeyset,
                                                            &pCryptoProvider);
            OpcUa_GotoErrorIfBad(uStatus);

            pSigningKey             = &pSendingKeyset->SigningKey;
            pEncryptionKey          = &pSendingKeyset->EncryptionKey;
            pInitializationVector   = &pSendingKeyset->InitializationVector;
        }
        else
        {
            pSigningKey             = pSecureStream->pPrivateKey;
            pEncryptionKey          = pSecureStream->pReceiverPublicKey;
            pCryptoProvider         = pSecureStream->pAsymmetricCryptoProvider;

            /* not required for open secure channel */
            pInitializationVector   = OpcUa_Null;
            uTokenId                = 0;
        }

        /** sign and encrypt **/
        uStatus = OpcUa_SecureStream_PrepareForSending( a_pOstrm,
                                                        pCryptoProvider,
                                                        pSigningKey,
                                                        pEncryptionKey,
                                                        pInitializationVector,
                                                        uTokenId);

        if(pSecureStream->eMessageType != eOpcUa_SecureStream_Types_OpenSecureChannel)
        {
            /* release reference to security set - failsafe, no errorchecking required */
            pSecureChannel->ReleaseSecuritySet( pSecureChannel,
                                                uTokenId);
        }

        OpcUa_GotoErrorIfBad(uStatus);

        if(pSecureChannel->bAsyncWriteInProgress)
        {
            OpcUa_BufferList* pBufferEntry = OpcUa_Alloc(sizeof(OpcUa_BufferList));
            OpcUa_GotoErrorIfAllocFailed(pBufferEntry);
            pBufferEntry->Buffer = pSecureStream->Buffers[0];
            pBufferEntry->Buffer.Position = 0;
            pBufferEntry->pNext = OpcUa_Null;
            if(pSecureChannel->pPendingSendBuffers == OpcUa_Null)
            {
                pSecureChannel->pPendingSendBuffers = pBufferEntry;
            }
            else
            {
                OpcUa_BufferList* pLastBuffer = pSecureChannel->pPendingSendBuffers;
                while(pLastBuffer->pNext != OpcUa_Null)
                {
                    pLastBuffer = pLastBuffer->pNext;
                }
                pLastBuffer->pNext = pBufferEntry;
            }

            pSecureStream->Buffers[0].Data = OpcUa_Null;
            pSecureStream->Buffers[0].Size = 0;
        }
        else
        {

            /* try to attach own buffer to transport stream */
            uStatus = pSecureStream->InnerStrm->AttachBuffer(   pSecureStream->InnerStrm,
                                                                &pSecureStream->Buffers[0]);
            OpcUa_GotoErrorIfBad(uStatus);

            /* manually flush the underlying stream to retain control over all buffers */
            uStatus = ((OpcUa_OutputStream*)(pSecureStream->InnerStrm))->Flush( (OpcUa_OutputStream*)(pSecureStream->InnerStrm),
                                                                                a_bLastCall);
            if(OpcUa_IsEqual(OpcUa_BadWouldBlock))
            {
                OpcUa_BufferList* pBufferList = OpcUa_Alloc(sizeof(OpcUa_BufferList));
                OpcUa_GotoErrorIfAllocFailed(pBufferList);
                /* regain control over the buffer object */
                uStatus = pSecureStream->InnerStrm->DetachBuffer(   pSecureStream->InnerStrm,
                                                                    &pSecureStream->Buffers[0]);
                if(OpcUa_IsBad(uStatus))
                {
                    OpcUa_Free(pBufferList);
                    OpcUa_GotoError;
                }
                pBufferList->Buffer = pSecureStream->Buffers[0];
                pBufferList->pNext = OpcUa_Null;
                pSecureChannel->pPendingSendBuffers = pBufferList;
                pSecureChannel->bAsyncWriteInProgress = OpcUa_True;

                pSecureStream->Buffers[0].Data = OpcUa_Null;
                pSecureStream->Buffers[0].Size = 0;
            }
            else if(OpcUa_IsBad(uStatus))
            {
                OpcUa_StatusCode uStatusTemp = OpcUa_Good;

                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureStream_Flush: Could not flush transport stream! Status 0x%0X!\n", uStatus);
                /* regain control over the buffer object */
                uStatusTemp = pSecureStream->InnerStrm->DetachBuffer(   pSecureStream->InnerStrm,
                                                                        &pSecureStream->Buffers[0]);
                if(OpcUa_IsBad(uStatusTemp))
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureStream_Flush: Could not detach buffer back from transport stream! Status 0x%0X!\n", uStatusTemp);
                }
            }
            else
            {
                /* regain control over the buffer object */
                uStatus = pSecureStream->InnerStrm->DetachBuffer(   pSecureStream->InnerStrm,
                                                                    &pSecureStream->Buffers[0]);
            }
        }

        if(OpcUa_IsGood(uStatus))
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureStream_Flush: Flush number %u! %s\n", pSecureStream->uNoOfFlushes, a_bLastCall?"(Last)":"");

            /* increment flush counter */
            pSecureStream->uNoOfFlushes++;
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureStream_Flush: Flush %u failed with 0x%08X! %s\n", pSecureStream->uNoOfFlushes, uStatus, a_bLastCall?"(Last)":"");
            OpcUa_GotoError;
        }

        /* reset buffer if more data is to come. */
        if(a_bLastCall == OpcUa_False)
        {
            OpcUa_StatusCode uStatusPre = OpcUa_Good;

            OpcUa_Buffer_SetEmpty(&pSecureStream->Buffers[0]);

            /* more data is to come; reset stream */
            if(pSecureStream->eMessageType != eOpcUa_SecureStream_Types_OpenSecureChannel)
            {
                uStatusPre = OpcUa_SecureStream_EncodeSymmetricSecurityHeader(  a_pOstrm,
                                                                                pSecureStream->eMessageType,
                                                                                pSecureStream->SecureChannelId,
                                                                                pSecureStream->RequestId,
                                                                                uTokenId);
            }
            else
            {
                uStatusPre = OpcUa_SecureStream_EncodeAsymmetricSecurityHeader( a_pOstrm,
                                                                                pSecureStream->eMessageType,
                                                                                pSecureStream->eMessageSecurityMode,
                                                                                pSecureStream->SecureChannelId,
                                                                                pSecureStream->RequestId,
                                                                                OpcUa_Null, /* security policy */
                                                                                pSecureStream->pSenderCertificate,
                                                                                pSecureStream->pReceiverCertificateThumbprint);
            }
            if(OpcUa_IsBad(uStatusPre))
            {
                uStatus = uStatusPre;
            }

            OpcUa_GotoErrorIfBad(uStatus);
        }
        else
        {
            if(pSecureChannel->pPendingSendBuffers != OpcUa_Null)
            {
                pSecureChannel->uPendingMessageCount++;
            }

            uStatus = pSecureChannel->UnlockWriteMutex(pSecureChannel);
            OpcUa_GotoErrorIfBad(uStatus);
            pSecureStream->IsLocked = OpcUa_False;
        }
        /* else leave the stream as it is; it has to be deleted soon */
    } /* not empty */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureStream->IsLocked == OpcUa_True && a_bLastCall == OpcUa_True)
    {
        pSecureChannel->UnlockWriteMutex(pSecureChannel);
        pSecureStream->IsLocked = OpcUa_False;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_Write
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_Write(   OpcUa_OutputStream* a_pOstrm,
                                                    OpcUa_Byte*         a_pBuffer,
                                                    OpcUa_UInt32        a_uCount)
{
    OpcUa_SecureStream* pSecureStream   = OpcUa_Null;
    OpcUa_UInt32        uMaxCount       = 0;
    OpcUa_UInt32        uDataLeft       = 0;
    OpcUa_UInt32        uDataWritten    = 0;
    OpcUa_Byte*         pWriteStart     = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Write");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureStream, a_pOstrm, Write);

    pSecureStream = (OpcUa_SecureStream*)a_pOstrm->Handle;

    /* verify stream state */
    if(pSecureStream->IsClosed)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* do the writing */
    uMaxCount   = pSecureStream->uFlushTrigger - pSecureStream->Buffers[0].Position;
    uDataLeft   = a_uCount;
    pWriteStart = a_pBuffer;

    /* check if bufferlimit will be hit */
    if(uMaxCount < a_uCount)
    {
        /* we need at least 1 flush */
        while(uMaxCount < uDataLeft)
        {
            /* write as much as possible into the stream */
            uStatus = OpcUa_Buffer_Write(&pSecureStream->Buffers[0], pWriteStart, uMaxCount);

            /* ToDo: check statuscode */

            uDataWritten += uMaxCount;

            /* flush the current content */
            uStatus = OpcUa_SecureStream_Flush( a_pOstrm,
                                                OpcUa_False); /* more calls to come */

            /* check statuscode */
            OpcUa_GotoErrorIfBad(uStatus);

            /* stream is now reset */
            /* recalculate state */
            uDataLeft   = a_uCount - uDataWritten;  /* amount of data left */
            pWriteStart = &a_pBuffer[uDataWritten]; /* begin of data left */
            uMaxCount   = pSecureStream->uFlushTrigger - pSecureStream->Buffers[0].Position;
        }
    }
    else
    {
        uDataLeft = a_uCount;   /* amount of data to write */
        pWriteStart = a_pBuffer;/* begin of data to write */
    }

    /* write requested data into internal buffer */
    uStatus = OpcUa_Buffer_Write(&pSecureStream->Buffers[0], pWriteStart, uDataLeft);
    OpcUa_ReturnErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureInputStream_Close
 *===========================================================================*/

/* ToDo: change OpcUa_InputStream to OpcUa_SecureStream */
static OpcUa_StatusCode OpcUa_SecureInputStream_Close(OpcUa_InputStream* a_pIstrm)
{
    OpcUa_SecureStream* pStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    pStream = (OpcUa_SecureStream*)a_pIstrm->Handle;

    pStream->IsClosed = OpcUa_True;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureOutputStream_Close
 *===========================================================================*/

/* ToDo: change OpcUa_InputStream to OpcUa_SecureStream */
static OpcUa_StatusCode OpcUa_SecureOutputStream_Close(OpcUa_OutputStream* a_pOstrm)
{
    OpcUa_SecureStream* pSecureStream   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);

    pSecureStream   = (OpcUa_SecureStream*)a_pOstrm->Handle;

    uStatus = OpcUa_SecureStream_Flush( a_pOstrm,
                                        OpcUa_True); /* stream will be closed closed -> definitely last call */

    pSecureStream->IsClosed = OpcUa_True;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_Close
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_Close(OpcUa_Stream* a_pStrm)
{
    OpcUa_SecureStream* pStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pStrm);
    /* ToDo: check whether a_pStrm->Handle is null */
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureStream, a_pStrm, Close);

    pStream = (OpcUa_SecureStream*)a_pStrm->Handle;

    /* verify stream state */
    if (pStream->IsClosed)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* close input stream */
    if (a_pStrm->Type == OpcUa_StreamType_Input)
    {
        uStatus = OpcUa_SecureInputStream_Close((OpcUa_InputStream*)a_pStrm);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* close output stream */
    else if (a_pStrm->Type == OpcUa_StreamType_Output)
    {
        uStatus = OpcUa_SecureOutputStream_Close((OpcUa_OutputStream*)a_pStrm);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* ToDo: return error when none of these types above comes in */

    /* flag the stream as closed. */
    pStream->IsClosed = OpcUa_True;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_Delete
 *===========================================================================*/
static OpcUa_Void OpcUa_SecureStream_Delete(OpcUa_Stream** a_ppStrm)
{
    if(a_ppStrm != OpcUa_Null && *a_ppStrm != OpcUa_Null)
    {
        OpcUa_SecureStream* pStream = OpcUa_Null;
        OpcUa_UInt32 uIndex = 0;

        pStream = (OpcUa_SecureStream*)(*a_ppStrm)->Handle;

        if (pStream->IsLocked == OpcUa_True && pStream->pSecureChannel != OpcUa_Null)
        {
            pStream->pSecureChannel->UnlockWriteMutex(pStream->pSecureChannel);
            pStream->IsLocked = OpcUa_False;
        }

        if (pStream->pSecureChannel != OpcUa_Null && pStream->pSecureChannel->ReleaseMethod != OpcUa_Null)
        {
            pStream->pSecureChannel->ReleaseMethod(
                pStream->pSecureChannel->ReleaseParam,
                &pStream->pSecureChannel);
        }

        if(pStream->pSenderPublicKey)
        {
            OpcUa_Key_Clear(pStream->pSenderPublicKey);
            OpcUa_Free(pStream->pSenderPublicKey);
        }

        if(pStream->pReceiverPublicKey)
        {
            OpcUa_Key_Clear(pStream->pReceiverPublicKey);
            OpcUa_Free(pStream->pReceiverPublicKey);
        }

        /* clear and free all buffers */
        for(uIndex = 0; uIndex < pStream->nBuffers; uIndex++)
        {
            OpcUa_Buffer_Clear(&pStream->Buffers[uIndex]);
        }
        OpcUa_Free(pStream->Buffers);

        OpcUa_Free(pStream);
        pStream = OpcUa_Null;

        OpcUa_Free(*a_ppStrm);
        *a_ppStrm = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_SecureStream_Skip
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_Skip(    OpcUa_InputStream*  a_pIstrm,
                                                    OpcUa_UInt32        a_nCount)
{
    OpcUa_SecureStream* pSecureStream       = OpcUa_Null;
    OpcUa_UInt32        uBytesToSkip        = 0;
    OpcUa_UInt32        uBytesLeftToSkip    = 0;
    OpcUa_Boolean       bReadAgain          = OpcUa_True;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "Skip");

    OpcUa_ReturnErrorIfArgumentNull(a_pIstrm);

    OpcUa_ReturnErrorIfTrue((a_nCount == 0), OpcUa_Good); /* nothing to do */

    uBytesLeftToSkip = a_nCount;

    pSecureStream = (OpcUa_SecureStream*)a_pIstrm->Handle;

    /* verify stream state */
    if(pSecureStream->IsClosed)
    {
        uStatus = OpcUa_BadInvalidState;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    do
    {
        /* set available amount of data as maximum */
        uBytesToSkip = pSecureStream->Buffers[pSecureStream->nCurrentReadBuffer].EndOfData - pSecureStream->Buffers[pSecureStream->nCurrentReadBuffer].Position;

        if(uBytesToSkip >= uBytesLeftToSkip)
        {
            /* ok - all of the bytes required are in one buffer */
            uBytesToSkip = uBytesLeftToSkip;
        }
        else
        {
            /* not all can be delivered */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureStream_Skip: end of buffer reached: %u requested, %u available in buffer; need to skip)!\n", uBytesLeftToSkip, uBytesToSkip);
        }

        /* no data left in buffer, trigger skipping */
        if(uBytesToSkip > 0)
        {
            /* read requested data */

            /* TODO: intra buffer skip */
            uStatus = OpcUa_Buffer_Skip( &pSecureStream->Buffers[pSecureStream->nCurrentReadBuffer],
                                         uBytesToSkip);
            OpcUa_GotoErrorIfBad(uStatus);

            /* prepare for next read */
            uBytesLeftToSkip -= uBytesToSkip;
        }

        if((uBytesLeftToSkip == 0) || (pSecureStream->nCurrentReadBuffer >= (pSecureStream->nBuffers - 1)))
        {
            bReadAgain = OpcUa_False;
        }
        else
        {
            /* skip to next buffer */
            pSecureStream->nCurrentReadBuffer++;
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureStream_Read: Buffer skip to index %u of %u!\n", pSecureStream->nCurrentReadBuffer, pSecureStream->nBuffers);
        }

    } while(bReadAgain != OpcUa_False);

    if(uBytesLeftToSkip > 0)
    {
        uStatus = OpcUa_BadEndOfStream;
    }

    /* todo: check if enough bytes were in the involved buffers to finish the skip operation */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_GetPosition
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetPosition( OpcUa_Stream* a_pStrm,
                                                        OpcUa_UInt32* a_pPosition)
{
    OpcUa_SecureStream* pSecureStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetPosition");

    OpcUa_ReturnErrorIfArgumentNull(a_pStrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pStrm->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pPosition);

    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureStream, a_pStrm, GetPosition);

    pSecureStream = (OpcUa_SecureStream*)a_pStrm->Handle;
    *a_pPosition = pSecureStream->nAbsolutePosition;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_Stream_SetPosition
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_SetPosition( OpcUa_Stream*   a_pStrm,
                                                        OpcUa_UInt32    a_uPosition)
{
    OpcUa_SecureStream* pSecureStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "SetPosition");

    OpcUa_ReturnErrorIfArgumentNull(a_pStrm);
    OpcUa_ReturnErrorIfInvalidObject(OpcUa_SecureStream, a_pStrm, SetPosition);

    pSecureStream = (OpcUa_SecureStream*)a_pStrm->Handle;

    if(a_pStrm->Type == OpcUa_StreamType_Input)
    {
        /* check if position is set forward or backward */
        if(a_uPosition < pSecureStream->nAbsolutePosition)
        {
            if(pSecureStream->nCurrentReadBuffer != 0)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureStream_SetPosition: Owner tried to seek backward! Not supported!\n");
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            else
            {
                OpcUa_UInt32 uPosition = 0;

                OpcUa_Buffer_GetPosition(&pSecureStream->Buffers[0], &uPosition);

                uPosition = uPosition - (pSecureStream->nAbsolutePosition - a_uPosition);

                /* only allow set position to a backward position when still in the first buffer */
                OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uPosition);
                pSecureStream->nAbsolutePosition = a_uPosition;
            }
        }
        else
        {
            uStatus = OpcUa_SecureStream_Skip(  (OpcUa_InputStream*)a_pStrm,
                                                a_uPosition - pSecureStream->nAbsolutePosition);
        }
    }
    else
    {
        /* output stream */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_SecureStream_SetPosition: Owner tried to seek in output stream!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CheckInputHeaderType
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_CheckInputHeaderType(   OpcUa_InputStream*          a_pTransportIstrm,
                                                            OpcUa_SecureMessageType*    a_pInputType)
{
    OpcUa_UInt32        uLength         = 4;
    OpcUa_Byte          abyBuffer[5];

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CheckInputHeaderType");

    OpcUa_ReturnErrorIfArgumentNull(a_pTransportIstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pInputType);

    /* (low level) read the signature */
    uStatus = a_pTransportIstrm->Read(  a_pTransportIstrm,
                                        &abyBuffer[0],
                                        &uLength);
    OpcUa_GotoErrorIfBad(uStatus);

    /* (mid level) read the length field (again, tcp layer did this already) */
    uStatus = OpcUa_UInt32_BinaryDecode(&uLength, a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* ToDo: This has to be optimized (SHILKA?) / Int32 comparison instead */

    if(OpcUa_MemCmp(abyBuffer,"MSG",3) == 0)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureStream - CheckInputHeaderType - Common Service\n");
        *a_pInputType = OpcUa_SecureMessageType_SM;
    }
    else if(OpcUa_MemCmp(abyBuffer,"OPN",3) == 0)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureStream - CheckInputHeaderType - OpenSecureChannel Service\n");
        *a_pInputType = OpcUa_SecureMessageType_SO;
    }
    else if(OpcUa_MemCmp(abyBuffer,"CLO",3) == 0)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureStream - CheckInputHeaderType - CloseSecureChannel Service\n");
        *a_pInputType = OpcUa_SecureMessageType_SC;
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureStream - CheckInputHeaderType - Unknown Service\n");
        uStatus = OpcUa_Bad;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_DecodeSymmetricSecurityHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_DecodeSymmetricSecurityHeader(  OpcUa_InputStream*  a_pTransportIstrm,
                                                                    OpcUa_UInt32*       a_pSecureChannelId,
                                                                    OpcUa_UInt32*       a_pTokenId)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "DecodeSymmetricSecurityHeader");

    OpcUa_ReturnErrorIfArgumentNull(a_pTransportIstrm);

    /* read SecureChannelId */
    uStatus = OpcUa_UInt32_BinaryDecode(a_pSecureChannelId,  a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read TokenId from security header */
    uStatus = OpcUa_UInt32_BinaryDecode(a_pTokenId, a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_DecodeAsymmetricSecurityHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_DecodeAsymmetricSecurityHeader( OpcUa_InputStream*  a_pTransportIstrm,
                                                                    OpcUa_UInt32*       a_pSecureChannelId,
                                                                    OpcUa_String*       a_pSecurityPolicyUri,
                                                                    OpcUa_ByteString*   a_pSenderCertificate,
                                                                    OpcUa_ByteString*   a_pReceiverCertificateThumbprint)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "DecodeAsymmetricSecurityHeader");

    OpcUa_ReturnErrorIfArgumentNull(a_pTransportIstrm);

    /* read SecureChannelId */
    uStatus = OpcUa_UInt32_BinaryDecode(a_pSecureChannelId,  a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read RequestId */
    /* 255 => length of the SecurityPolicyUri => see Part 6 */
    uStatus = OpcUa_String_BinaryDecode(a_pSecurityPolicyUri, 255, a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read sender certificate */
    /* MaxSenderCertificateSize => see Part 6 */
    uStatus = OpcUa_ByteString_BinaryDecode(a_pSenderCertificate, 50000, a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);


    /* read receiver certificate thumbprint */
    /* 20 => SHA1 => see Part 6 */
    uStatus = OpcUa_ByteString_BinaryDecode(a_pReceiverCertificateThumbprint, 20, a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_DecodeSequenceHeader
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_DecodeSequenceHeader(   OpcUa_InputStream*  a_pTransportIstrm,
                                                            OpcUa_UInt32*       a_pSequenceNumber,
                                                            OpcUa_UInt32*       a_pRequestId)
{

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "DecodeSequenceHeader");

    OpcUa_ReturnErrorIfArgumentNull(a_pTransportIstrm);

    /* read SequenceNumber */
    uStatus = OpcUa_UInt32_BinaryDecode(a_pSequenceNumber,  a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read RequestId */
    uStatus = OpcUa_UInt32_BinaryDecode(a_pRequestId,  a_pTransportIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_AppendInput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_AppendInput(OpcUa_InputStream*      a_pTransportIstrm,
                                                OpcUa_InputStream*      a_pSecureIStream,
                                                OpcUa_Key*              a_pSigningKey,
                                                OpcUa_Key*              a_pEncryptionKey,
                                                OpcUa_Key*              a_pInitializationVector,
                                                OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                OpcUa_SecureChannel*    a_pSecureChannel)
{
    OpcUa_Buffer        readBuffer;
    OpcUa_UInt32        uChunkLength    = 0;
    OpcUa_UInt32        uSequenceNumber = 0;
    OpcUa_UInt32        uRequestId      = 0;
    OpcUa_SecureStream* pSecureStream   = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "AppendInput");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureIStream);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureIStream->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pTransportIstrm);

    pSecureStream = (OpcUa_SecureStream*)a_pSecureIStream->Handle;
    pSecureStream->InnerStrm = (OpcUa_Stream*)a_pTransportIstrm;

    /* try to detach the buffer */
    uStatus = a_pTransportIstrm->DetachBuffer(  (OpcUa_Stream*)a_pTransportIstrm,
                                                &readBuffer);

    if(OpcUa_IsBad(uStatus))
    {
        /* if that is not supported by the underlying stream, copy the content */
        if(OpcUa_IsEqual(OpcUa_BadNotSupported))
        {
            OpcUa_UInt32 uBufferPosition = 0;

            /* get the length of the streambuffer */
            uStatus = a_pTransportIstrm->GetChunkLength((OpcUa_Stream*)a_pTransportIstrm,
                                                        &uChunkLength);
            OpcUa_GotoErrorIfBad(uStatus);

            /* get the position in the streambuffer */
            uStatus = a_pTransportIstrm->GetPosition((OpcUa_Stream*)a_pTransportIstrm,
                                                     &uBufferPosition);
            OpcUa_GotoErrorIfBad(uStatus);

            /* create own buffer */
            uStatus = OpcUa_Buffer_Initialize(  &readBuffer,
                                                OpcUa_Null,
                                                0,
                                                uChunkLength,
                                                0,
                                                OpcUa_True);
            OpcUa_GotoErrorIfBad(uStatus);

            /* rewind the original buffer */
            uStatus = OpcUa_Stream_SetPosition((OpcUa_Stream*)a_pTransportIstrm,
                                               OpcUa_StreamPosition_Start);
            OpcUa_GotoErrorIfBad(uStatus);

            while (OpcUa_IsGood(uStatus))
            {
                OpcUa_Byte pBuffer[4096];
                /* see comment above */

                OpcUa_UInt32 uCount = sizeof(pBuffer);

                /* read block from stream */
                uStatus = OpcUa_Stream_Read(    a_pTransportIstrm,
                                                pBuffer,
                                                &uCount);

                if (OpcUa_IsBad(uStatus))
                {
                    if (uStatus == OpcUa_BadEndOfStream)
                    {
                        uStatus = OpcUa_Good;
                        break;
                    }
                    OpcUa_GotoErrorIfBad(uStatus);
                }

                uStatus = OpcUa_Buffer_Write(   &readBuffer,
                                                pBuffer,
                                                uCount);
                OpcUa_GotoErrorIfBad(uStatus);
            }

            /* update the buffer position in an own buffer */
            uStatus = OpcUa_Buffer_SetPosition(&readBuffer, uBufferPosition);
            OpcUa_GotoErrorIfBad(uStatus);

            /* restore the buffer position in the original buffer */
            uStatus = OpcUa_Stream_SetPosition((OpcUa_Stream*)a_pTransportIstrm,
                                               uBufferPosition);
            OpcUa_GotoErrorIfBad(uStatus);

        }
        else /* or leave with error */
        {
            OpcUa_GotoError;
        }
    }

    if(pSecureStream->nBuffers >= pSecureStream->nMaxBuffers)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureStream_AppendInput: %u max chunks per message exceeded! Close connection!\n", pSecureStream->nMaxBuffers);
        OpcUa_GotoErrorWithStatus(OpcUa_BadEncodingLimitsExceeded);
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureStream_AppendInput: Appending buffer %u!\n", pSecureStream->nBuffers);
    }

    switch(pSecureStream->eMessageSecurityMode)
    {
    case OpcUa_MessageSecurityMode_None:
        {
            break;
        }
    case OpcUa_MessageSecurityMode_Sign:
        {
            uStatus = OpcUa_SecureStream_VerifyInputBuffer( pSecureStream,
                                                            &readBuffer,
                                                            a_pCryptoProvider,
                                                            a_pSigningKey, /*(pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel) ? pSecureStream->pSenderPublicKey : &pSecureStream->pKeyset->SigningKey,*/
                                                            (OpcUa_Boolean)((pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel) ? OpcUa_False : OpcUa_True));
            OpcUa_GotoErrorIfBad(uStatus);

            /* remove signature from buffer */
            readBuffer.EndOfData -= pSecureStream->uSignatureSize;
            break;
        }
    case OpcUa_MessageSecurityMode_SignAndEncrypt:
        {
            if(pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel)
            {
                /* check whether certificate thumbprint of the server is the same thumbprint as provided in the header */
                OpcUa_ByteString receiverCertificateThumbprint = OPCUA_BYTESTRING_STATICINITIALIZER;

                uStatus = a_pCryptoProvider->GetCertificateThumbprint( a_pCryptoProvider,
                                                                       pSecureStream->pReceiverCertificate,
                                                                       &receiverCertificateThumbprint);
                OpcUa_GotoErrorIfBad(uStatus);

                if(receiverCertificateThumbprint.Length > 0)
                {
                    receiverCertificateThumbprint.Data = (OpcUa_Byte*)OpcUa_Alloc(receiverCertificateThumbprint.Length);
                    OpcUa_GotoErrorIfAllocFailed(receiverCertificateThumbprint.Data);

                    uStatus = a_pCryptoProvider->GetCertificateThumbprint( a_pCryptoProvider,
                                                                           pSecureStream->pReceiverCertificate,
                                                                           &receiverCertificateThumbprint);
                    if(OpcUa_IsBad(uStatus))
                    {
                        OpcUa_ByteString_Clear(&receiverCertificateThumbprint);
                        OpcUa_GotoError;
                    }
                }
                else
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_Bad);
                }

                if(pSecureStream->pReceiverCertificateThumbprint->Length != receiverCertificateThumbprint.Length
                   || OpcUa_MemCmp(pSecureStream->pReceiverCertificateThumbprint->Data, receiverCertificateThumbprint.Data, receiverCertificateThumbprint.Length) != 0)
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureStream_AppendInput: Certificate Thumbprints do not match!\n");
                    OpcUa_ByteString_Clear(&receiverCertificateThumbprint);
                    OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid);
                }

                /* no longer needed after this point */
                OpcUa_ByteString_Clear(&receiverCertificateThumbprint);
            }

            uStatus = OpcUa_SecureStream_DecryptInputBuffer(&readBuffer,
                                                            a_pCryptoProvider,
                                                            a_pEncryptionKey,
                                                            (pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel) ? OpcUa_False : OpcUa_True,
                                                            a_pInitializationVector);
            if(OpcUa_IsEqual(OpcUa_BadSignatureInvalid) && (pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel))
            {
                /* even if the decryption fails, ensure that there is no different timing */
                (OpcUa_Void) OpcUa_SecureStream_VerifyInputBuffer(pSecureStream,
                                                                  &readBuffer,
                                                                  a_pCryptoProvider,
                                                                  a_pSigningKey,
                                                                  OpcUa_False);
            }
            OpcUa_GotoErrorIfBad(uStatus);

            uStatus = OpcUa_SecureStream_VerifyInputBuffer( pSecureStream,
                                                            &readBuffer,
                                                            a_pCryptoProvider,
                                                            a_pSigningKey,
                                                            (OpcUa_Boolean)((pSecureStream->eMessageType == eOpcUa_SecureStream_Types_OpenSecureChannel) ? OpcUa_False : OpcUa_True));
            OpcUa_GotoErrorIfBad(uStatus);

            /* remove signature from buffer */
            readBuffer.EndOfData -= pSecureStream->uSignatureSize;

            /* remove extra padding from buffer if present */
            if (pSecureStream->uPlainTextBlockSize > 256)
            {
                readBuffer.EndOfData--;
                if(readBuffer.EndOfData < readBuffer.Position + 256*readBuffer.Data[readBuffer.EndOfData])
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
                }
                readBuffer.EndOfData -= 256*readBuffer.Data[readBuffer.EndOfData];
            }

            /* remove padding from buffer */
            readBuffer.EndOfData--;
            if(readBuffer.EndOfData < readBuffer.Position + readBuffer.Data[readBuffer.EndOfData])
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            readBuffer.EndOfData -= readBuffer.Data[readBuffer.EndOfData];
            break;
        }
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_SecureStream_AppendInput: Invalid message security mode requested!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
        }
    }

    /* ToDo: check against MAXCHUNKPERMESSAGE */

    /* increment and copy buffer to secure stream */
    pSecureStream->Buffers[pSecureStream->nBuffers++] = readBuffer;
    readBuffer.FreeBuffer = OpcUa_False;

    /* the following function must read from the last attached buffer */
    pSecureStream->nCurrentReadBuffer = pSecureStream->nBuffers - 1;

    /* decode the sequence header from the plaintext buffer */
    uStatus = OpcUa_SecureStream_DecodeSequenceHeader(  a_pSecureIStream,
                                                        &uSequenceNumber,
                                                        &uRequestId);
    OpcUa_GotoErrorIfBad(uStatus);

    /* set or check sequence number and request id */
    if(pSecureStream->nCurrentReadBuffer == 0)
    {
        /* this is the first chunk of this multipart stream - set the request id */
        pSecureStream->RequestId = uRequestId;
    }
    else
    {
        /* not the first chunk - check request id */
        if(pSecureStream->RequestId != uRequestId)
        {
            /* error - sender mixed chunk order or maybe this is a replay attack */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
                        "OpcUa_SecureStream_AppendInput: Streams RID %u does not match received one: %u\n",
                        pSecureStream->RequestId,
                        uRequestId);
            OpcUa_GotoErrorWithStatus(OpcUa_BadRequestHeaderInvalid);
        }
    }

    uStatus = a_pSecureChannel->CheckSequenceNumber(a_pSecureChannel,
                                                    uSequenceNumber);
    if(OpcUa_IsBad(uStatus))
    {
        /* error - sender mixed chunk order or maybe this is a replay attack */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
                    "OpcUa_SecureStream_AppendInput: Streams SID %u does not match expected one: %u\n",
                    uSequenceNumber);
        OpcUa_GotoError;
    }

    OpcUa_Trace(    OPCUA_TRACE_LEVEL_DEBUG,
                    "OpcUa_SecureStream_AppendInput: appended chunk with SN %u, RID %u\n",
                    uSequenceNumber,
                    uRequestId);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Buffer_Clear(&readBuffer);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_AttachBuffer
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_AttachBuffer(OpcUa_Stream*   a_pStrm,
                                                        OpcUa_Buffer*   a_pBuffer)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "AttachBuffer");

    OpcUa_ReferenceParameter(a_pStrm);
    OpcUa_ReferenceParameter(a_pBuffer);

    OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_DetachBuffer
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_DetachBuffer(OpcUa_Stream*   a_pStrm,
                                                        OpcUa_Buffer*   a_pBuffer)
{
    OpcUa_SecureStream* pSecureStream   = OpcUa_Null;
    OpcUa_UInt32        uBufferIndex    = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "DetachBuffer");

    OpcUa_ReferenceParameter(a_pBuffer);

    pSecureStream = (OpcUa_SecureStream*)a_pStrm->Handle;

    if(pSecureStream->nBuffers > 0)
    {
        *a_pBuffer = pSecureStream->Buffers[0];
        pSecureStream->Buffers[0].Data = OpcUa_Null;

        for(uBufferIndex = 0; uBufferIndex < pSecureStream->nBuffers; uBufferIndex++)
        {
            OpcUa_Buffer_Clear(&pSecureStream->Buffers[uBufferIndex]);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_GetChunkLength
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_GetChunkLength(OpcUa_Stream*   a_pStrm,
                                                          OpcUa_UInt32*   a_puLength)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "GetChunkLength");

    OpcUa_ReferenceParameter(a_pStrm);
    OpcUa_ReferenceParameter(a_puLength);

    OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_SecureStream_CalculateFlushTrigger
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_CalculateFlushTrigger(   OpcUa_SecureStream* a_pSecureStream,
                                                                    OpcUa_UInt32        a_pBufferLength)
{

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CalculateFlushTrigger");

    /* zero value means an unlimited buffer length */
    a_pBufferLength = (a_pBufferLength > 0)? a_pBufferLength: OpcUa_UInt32_Max;

    /*** check security mode ***/
    switch(a_pSecureStream->eMessageSecurityMode)
    {
    case OpcUa_MessageSecurityMode_None:
        {
            a_pSecureStream->uFlushTrigger = a_pBufferLength;
            break;
        }
    case OpcUa_MessageSecurityMode_Sign:
        {
            /*** save room for the signature ***/
            a_pSecureStream->uFlushTrigger = a_pBufferLength - a_pSecureStream->uSignatureSize;
            break;
        }
    case OpcUa_MessageSecurityMode_SignAndEncrypt:
        {
            OpcUa_UInt32 uMaxEncryptedBlocks = 0;
            OpcUa_UInt32 uNonEncryptedHeaderLength = a_pSecureStream->uBeginOfRequestBody;

            /* must remove the length of the sequence header */
            if (uNonEncryptedHeaderLength != 0)
            {
                if (uNonEncryptedHeaderLength >= a_pBufferLength || uNonEncryptedHeaderLength <= 8)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadUnexpectedError);
                }

                uNonEncryptedHeaderLength -= 8;
            }

            /*** figure out the maximum number of encryption blocks ***/
            uMaxEncryptedBlocks = ((a_pBufferLength - uNonEncryptedHeaderLength)/a_pSecureStream->uCipherTextBlockSize);

            if (uMaxEncryptedBlocks == 0)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadUnexpectedError);
            }

            a_pSecureStream->uFlushTrigger = uNonEncryptedHeaderLength + a_pSecureStream->uPlainTextBlockSize*uMaxEncryptedBlocks - a_pSecureStream->uSignatureSize
                                             - (a_pSecureStream->uPlainTextBlockSize > 256 ? 2 : 1);
            break;
        }
    case OpcUa_MessageSecurityMode_Invalid:
    default:
        {
            /* error */
            OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityModeRejected);
        }
    } /* switch security mode */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CreateInput
 *===========================================================================*/
/* This function assumes that the given stream contains a full secure message chunk.
   It is not assumed that the content of this message contains a full encoded
   request or response. If it is a partial message, the buffer gets detached
   and stored for delayed decryption. */

/* ToDo: add plaintext blocksize, ciphertext blocksize, signature size as parameters */
/* ToDo: Optimization of buffer allocation (pool/buffermanager) */
OpcUa_StatusCode OpcUa_SecureStream_CreateInput(OpcUa_CryptoProvider*       a_pCryptoProvider,
                                                OpcUa_MessageSecurityMode   a_eMessageSecurityMode,
                                                OpcUa_UInt32                a_uMaxChunks,
                                                OpcUa_InputStream**         a_ppIstrm)
{
    OpcUa_SecureStream* pSecureStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CreateInput");

    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_ppIstrm);

    /* initialize out parameter */
    *a_ppIstrm = OpcUa_Null;

    /* allocate handle */
    pSecureStream = (OpcUa_SecureStream*)OpcUa_Alloc(sizeof(OpcUa_SecureStream));
    OpcUa_GotoErrorIfAllocFailed(pSecureStream);
    OpcUa_MemSetD(pSecureStream, 0, sizeof(OpcUa_SecureStream));

    /* set Buffermaximum */
    pSecureStream->nMaxBuffers = a_uMaxChunks;

    pSecureStream->Buffers = (OpcUa_Buffer*)OpcUa_Alloc(sizeof(OpcUa_Buffer) * pSecureStream->nMaxBuffers);
    OpcUa_GotoErrorIfAllocFailed(pSecureStream->Buffers);
    OpcUa_MemSet(pSecureStream->Buffers, 0, sizeof(OpcUa_Buffer) * pSecureStream->nMaxBuffers);

    pSecureStream->SanityCheck      = OpcUa_SecureStream_SanityCheck;
    pSecureStream->IsClosed         = OpcUa_False;
    pSecureStream->IsLocked         = OpcUa_False;
    pSecureStream->InnerStrm        = OpcUa_Null;

    pSecureStream->RequestId            = 0;
    pSecureStream->nBuffers             = 0; /* this is buffers used! */
    pSecureStream->nCurrentReadBuffer   = 0;
    pSecureStream->eMessageType         = eOpcUa_SecureStream_Types_StandardMessage;
    pSecureStream->eMessageSecurityMode = a_eMessageSecurityMode;
    pSecureStream->uNoOfFlushes         = 0;
    pSecureStream->uPlainTextBlockSize  = 1;
    pSecureStream->uCipherTextBlockSize = 1;
    pSecureStream->uSignatureSize       = 0;

    pSecureStream->pSenderPublicKey     = OpcUa_Null;
    pSecureStream->pReceiverPublicKey   = OpcUa_Null;
    pSecureStream->pPrivateKey          = OpcUa_Null;
    pSecureStream->pSenderCertificate   = OpcUa_Null;
    pSecureStream->pReceiverCertificate = OpcUa_Null;
    pSecureStream->pReceiverCertificateThumbprint = OpcUa_Null;

    pSecureStream->nAbsolutePosition    = 0;
    pSecureStream->SecureChannelId      = 0;
    pSecureStream->pSecureChannel       = OpcUa_Null;

    /* get the encryption block sizes */
    uStatus = OpcUa_SecureStream_GetSymmetricEncryptionBlockSizes(
        a_pCryptoProvider,
        &pSecureStream->uPlainTextBlockSize,
        &pSecureStream->uCipherTextBlockSize);

    OpcUa_GotoErrorIfBad(uStatus);

    /* get the signature size */
    uStatus = OpcUa_SecureStream_GetSymmetricSignatureSize(a_pCryptoProvider, &pSecureStream->uSignatureSize);
    OpcUa_GotoErrorIfBad(uStatus);

    /* allocate stream */
    *a_ppIstrm = (OpcUa_InputStream*)OpcUa_Alloc(sizeof(OpcUa_InputStream));
    OpcUa_GotoErrorIfAllocFailed(*a_ppIstrm);
    OpcUa_MemSet(*a_ppIstrm, 0, sizeof(OpcUa_InputStream));

    (*a_ppIstrm)->Type        = OpcUa_StreamType_Input;
    (*a_ppIstrm)->Handle      = pSecureStream;
    (*a_ppIstrm)->GetPosition = OpcUa_SecureStream_GetPosition;
    (*a_ppIstrm)->SetPosition = OpcUa_SecureStream_SetPosition;
    (*a_ppIstrm)->Close       = OpcUa_SecureStream_Close;
    (*a_ppIstrm)->Delete      = OpcUa_SecureStream_Delete;
    (*a_ppIstrm)->Read        = OpcUa_SecureStream_Read;
    (*a_ppIstrm)->DetachBuffer= OpcUa_SecureStream_DetachBuffer;
    (*a_ppIstrm)->AttachBuffer= OpcUa_SecureStream_AttachBuffer;


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureStream != OpcUa_Null)
    {
        OpcUa_Free(pSecureStream->Buffers);
        OpcUa_Free(pSecureStream);
    }

    if(*a_ppIstrm != OpcUa_Null)
    {
        OpcUa_Free(*a_ppIstrm);
        *a_ppIstrm = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CreateOpenSecureChannelInput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_CreateOpenSecureChannelInput(   OpcUa_CryptoProvider*       a_pCryptoProvider,
                                                                    OpcUa_MessageSecurityMode   a_eMessageSecurityMode,
                                                                    OpcUa_ByteString*           a_pReceiverCertificate,
                                                                    OpcUa_Key*                  a_pReceiverPrivateKey,
                                                                    OpcUa_ByteString*           a_pSenderCertificate,
                                                                    OpcUa_ByteString*           a_pReceiverCertificateThumbprint,
                                                                    OpcUa_UInt32                a_uMaxChunks,
                                                                    OpcUa_InputStream**         a_ppSecureIstrm)
{
    OpcUa_SecureStream* pSecureStream = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CreateOpenSecureChannelInput");

    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureIstrm);

    /* initialize out parameter */
    *a_ppSecureIstrm = OpcUa_Null;

    /* allocate handle */
    pSecureStream = (OpcUa_SecureStream*)OpcUa_Alloc(sizeof(OpcUa_SecureStream));
    OpcUa_GotoErrorIfAllocFailed(pSecureStream);
    OpcUa_MemSetD(pSecureStream, 0, sizeof(OpcUa_SecureStream));

    pSecureStream->nMaxBuffers = a_uMaxChunks;

    pSecureStream->Buffers = (OpcUa_Buffer*)OpcUa_Alloc(sizeof(OpcUa_Buffer) * pSecureStream->nMaxBuffers);
    OpcUa_GotoErrorIfAllocFailed(pSecureStream->Buffers);
    OpcUa_MemSet(pSecureStream->Buffers, 0, sizeof(OpcUa_Buffer) * pSecureStream->nMaxBuffers);

    pSecureStream->SanityCheck                      = OpcUa_SecureStream_SanityCheck;
    pSecureStream->nBuffers                         = 0;
    pSecureStream->nCurrentReadBuffer               = 0;
    pSecureStream->IsClosed                         = OpcUa_False;
    pSecureStream->IsLocked                         = OpcUa_False;
    pSecureStream->InnerStrm                        = OpcUa_Null;
    pSecureStream->RequestId                        = 0;
    pSecureStream->eMessageType                     = eOpcUa_SecureStream_Types_OpenSecureChannel;
    pSecureStream->eMessageSecurityMode             = a_eMessageSecurityMode;
    pSecureStream->pSenderCertificate               = a_pSenderCertificate;
    pSecureStream->pPrivateKey                      = a_pReceiverPrivateKey;
    pSecureStream->pReceiverCertificate             = a_pReceiverCertificate;
    pSecureStream->pReceiverCertificateThumbprint   = a_pReceiverCertificateThumbprint;
    pSecureStream->uNoOfFlushes                     = 0;
    pSecureStream->uPlainTextBlockSize              = 1;
    pSecureStream->uCipherTextBlockSize             = 1;
    pSecureStream->uSignatureSize                   = 0;
    pSecureStream->nAbsolutePosition                = 0;
    pSecureStream->SecureChannelId                  = 0;
    pSecureStream->pSecureChannel                   = OpcUa_Null;

    if(a_eMessageSecurityMode != OpcUa_MessageSecurityMode_None)
    {
        /* Get receivers public key if possible. */
        if((a_pReceiverCertificate->Length > 0) && (a_pReceiverCertificate->Data != OpcUa_Null))
        {
            uStatus = OpcUa_SecureStream_GetPublicKey(a_pCryptoProvider, a_pReceiverCertificate, &pSecureStream->pReceiverPublicKey);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        /* Get senders public key if possible. */
        if((a_pSenderCertificate->Length > 0) && (a_pSenderCertificate->Data != OpcUa_Null))
        {
            uStatus = OpcUa_SecureStream_GetPublicKey(a_pCryptoProvider, a_pSenderCertificate, &pSecureStream->pSenderPublicKey);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        /* get the encryption block sizes */
        uStatus = OpcUa_SecureStream_GetAsymmetricEncryptionBlockSizes(
            a_pCryptoProvider,
            pSecureStream->pReceiverPublicKey,
            &pSecureStream->uPlainTextBlockSize,
            &pSecureStream->uCipherTextBlockSize);

        OpcUa_GotoErrorIfBad(uStatus);

        /* get the signature size */
        uStatus = OpcUa_SecureStream_GetAsymmetricSignatureSize(
            a_pCryptoProvider,
            pSecureStream->pSenderPublicKey,
            &pSecureStream->uSignatureSize);

        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        pSecureStream->pSenderPublicKey                 = OpcUa_Null;
        pSecureStream->pReceiverPublicKey               = OpcUa_Null;
    }

    /* allocate stream */
    *a_ppSecureIstrm = (OpcUa_InputStream*)OpcUa_Alloc(sizeof(OpcUa_InputStream));
    OpcUa_GotoErrorIfAllocFailed(*a_ppSecureIstrm);
    OpcUa_MemSet(*a_ppSecureIstrm, 0, sizeof(OpcUa_InputStream));

    (*a_ppSecureIstrm)->Type        = OpcUa_StreamType_Input;
    (*a_ppSecureIstrm)->Handle      = pSecureStream;
    (*a_ppSecureIstrm)->GetPosition = OpcUa_SecureStream_GetPosition;
    (*a_ppSecureIstrm)->SetPosition = OpcUa_SecureStream_SetPosition;
    (*a_ppSecureIstrm)->Close       = OpcUa_SecureStream_Close;
    (*a_ppSecureIstrm)->Delete      = OpcUa_SecureStream_Delete;
    (*a_ppSecureIstrm)->Read        = OpcUa_SecureStream_Read;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureStream != OpcUa_Null)
    {
        if(pSecureStream->pSenderPublicKey != OpcUa_Null)
        {
            OpcUa_Key_Clear(pSecureStream->pSenderPublicKey);
            OpcUa_Free(pSecureStream->pSenderPublicKey);
        }
        if(pSecureStream->pReceiverPublicKey != OpcUa_Null)
        {
            OpcUa_Key_Clear(pSecureStream->pReceiverPublicKey);
            OpcUa_Free(pSecureStream->pReceiverPublicKey);
        }
        OpcUa_Free(pSecureStream->Buffers);
        OpcUa_Free(pSecureStream);
    }

    if(*a_ppSecureIstrm != OpcUa_Null)
    {
        OpcUa_Free(*a_ppSecureIstrm);
        *a_ppSecureIstrm = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CreateOpenSecureChannelOutput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_CreateOpenSecureChannelOutput(  OpcUa_OutputStream*         a_pInnerOstrm,
                                                                    OpcUa_SecureChannel*        a_pSecureChannel,
                                                                    OpcUa_UInt32                a_uRequestId,
                                                                    OpcUa_String*               a_pSecurityPolicyUri,
                                                                    OpcUa_MessageSecurityMode   a_eMessageSecurityMode,
                                                                    OpcUa_CryptoProvider*       a_pCryptoProvider,
                                                                    OpcUa_ByteString*           a_pSenderCertificate,
                                                                    OpcUa_Key*                  a_pSenderPrivateKey,
                                                                    OpcUa_ByteString*           a_pReceiverCertificate,
                                                                    OpcUa_ByteString*           a_pReceiverCertificateThumbprint,
                                                                    OpcUa_OutputStream**        a_ppSecureOstrm)
{
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;
    OpcUa_OutputStream*     pSecureOstrm        = OpcUa_Null;

    OpcUa_UInt32            uChunkLength        = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CreateOpenSecureChannelOutput");

    OpcUa_ReturnErrorIfArgumentNull(a_pInnerOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecurityPolicyUri);
    OpcUa_ReturnErrorIfTrue((OpcUa_MessageSecurityMode_Invalid == a_eMessageSecurityMode), OpcUa_BadInvalidArgument);
    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureOstrm);

    /* initialize out parameter */
    *a_ppSecureOstrm = OpcUa_Null;

    /*** create SecureStream ***/
    pSecureStream = (OpcUa_SecureStream*)OpcUa_Alloc(sizeof(OpcUa_SecureStream));
    OpcUa_GotoErrorIfAllocFailed(pSecureStream);
    OpcUa_MemSetD(pSecureStream, 0, sizeof(OpcUa_SecureStream));

    pSecureStream->uBeginOfRequestBody = 0;

    /* internal buffer management */
    uStatus = a_pInnerOstrm->GetChunkLength((OpcUa_Stream*)a_pInnerOstrm, &uChunkLength);
    OpcUa_GotoErrorIfBad(uStatus);

    pSecureStream->nBuffers             = 1; /* only one buffer for outstreams! */
    pSecureStream->nCurrentReadBuffer   = 0;

    pSecureStream->Buffers = (OpcUa_Buffer*)OpcUa_Alloc(sizeof(OpcUa_Buffer)); /* only one buffer per outstream */
    OpcUa_GotoErrorIfAllocFailed(pSecureStream->Buffers);

    uStatus = OpcUa_Buffer_Initialize(  &pSecureStream->Buffers[0], /* the buffer */
                                        OpcUa_Null,                 /* initialize data */
                                        0,                          /* size of initialize data */
                                        uChunkLength,               /* block size */
                                        uChunkLength,               /* max size = initial size */
                                        OpcUa_True);                /* free memory on delete */
    OpcUa_GotoErrorIfBad(uStatus);

    /* general stream settings */
    pSecureStream->SanityCheck                      = OpcUa_SecureStream_SanityCheck;
    pSecureStream->IsClosed                         = OpcUa_False;
    pSecureStream->IsLocked                         = OpcUa_False;
    pSecureStream->InnerStrm                        = (OpcUa_Stream*)a_pInnerOstrm;

    /* encryption management information */
    pSecureStream->SecureChannelId                  = a_pSecureChannel->SecureChannelId;
    pSecureStream->pSecureChannel                   = a_pSecureChannel;

    pSecureStream->pAsymmetricCryptoProvider        = a_pCryptoProvider;

    pSecureStream->RequestId                        = a_uRequestId;
    pSecureStream->eMessageType                     = eOpcUa_SecureStream_Types_OpenSecureChannel;
    pSecureStream->eMessageSecurityMode             = a_eMessageSecurityMode;
    pSecureStream->uNoOfFlushes                     = 0;
    pSecureStream->pPrivateKey                      = a_pSenderPrivateKey;
    pSecureStream->pReceiverCertificate             = a_pReceiverCertificate;
    pSecureStream->uPlainTextBlockSize              = 1;
    pSecureStream->uCipherTextBlockSize             = 1;
    pSecureStream->uSignatureSize                   = 0;
    pSecureStream->pSenderCertificate               = OpcUa_Null;
    pSecureStream->pReceiverCertificateThumbprint   = OpcUa_Null;
    pSecureStream->nAbsolutePosition                = 0;

    if(a_eMessageSecurityMode != OpcUa_MessageSecurityMode_None)
    {
        /* Get receivers public key if possible. */
        if((a_pReceiverCertificate->Length > 0) && (a_pReceiverCertificate->Data != OpcUa_Null))
        {
            uStatus = OpcUa_SecureStream_GetPublicKey(  a_pCryptoProvider,
                                                        a_pReceiverCertificate,
                                                        &pSecureStream->pReceiverPublicKey);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        /* Get senders public key if possible. */
        if((a_pSenderCertificate->Length > 0) && (a_pSenderCertificate->Data != OpcUa_Null))
        {
            uStatus = OpcUa_SecureStream_GetPublicKey(  a_pCryptoProvider,
                                                        a_pSenderCertificate,
                                                        &pSecureStream->pSenderPublicKey);
            OpcUa_GotoErrorIfBad(uStatus);
        }

        /* get the encryption block sizes */
        uStatus = OpcUa_SecureStream_GetAsymmetricEncryptionBlockSizes( a_pCryptoProvider,
                                                                        pSecureStream->pReceiverPublicKey,
                                                                        &pSecureStream->uPlainTextBlockSize,
                                                                        &pSecureStream->uCipherTextBlockSize);

        OpcUa_GotoErrorIfBad(uStatus);

        /* get the signature size */
        uStatus = OpcUa_SecureStream_GetAsymmetricSignatureSize(a_pCryptoProvider,
                                                                pSecureStream->pSenderPublicKey,
                                                                &pSecureStream->uSignatureSize);

        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        pSecureStream->pSenderPublicKey                 = OpcUa_Null;
        pSecureStream->pReceiverPublicKey               = OpcUa_Null;
    }

    /* Calculate flush trigger */
    uStatus = OpcUa_SecureStream_CalculateFlushTrigger(pSecureStream, uChunkLength);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** create OutputStream ***/
    *a_ppSecureOstrm = (OpcUa_OutputStream*)OpcUa_Alloc(sizeof(OpcUa_OutputStream));
    OpcUa_GotoErrorIfAllocFailed(*a_ppSecureOstrm);
    OpcUa_MemSet(*a_ppSecureOstrm, 0, sizeof(OpcUa_OutputStream));

    (*a_ppSecureOstrm)->Type                  = OpcUa_StreamType_Output;
    (*a_ppSecureOstrm)->Handle                = pSecureStream;
    (*a_ppSecureOstrm)->GetPosition           = OpcUa_SecureStream_GetPosition;
    (*a_ppSecureOstrm)->SetPosition           = OpcUa_SecureStream_SetPosition;
    (*a_ppSecureOstrm)->Close                 = OpcUa_SecureStream_Close;
    (*a_ppSecureOstrm)->Delete                = OpcUa_SecureStream_Delete;
    (*a_ppSecureOstrm)->Write                 = OpcUa_SecureStream_Write;
    (*a_ppSecureOstrm)->Flush                 = OpcUa_SecureStream_Flush;
    (*a_ppSecureOstrm)->DetachBuffer          = OpcUa_SecureStream_DetachBuffer;
    (*a_ppSecureOstrm)->AttachBuffer          = OpcUa_SecureStream_AttachBuffer;
    (*a_ppSecureOstrm)->GetChunkLength        = OpcUa_SecureStream_GetChunkLength;

    pSecureStream = (OpcUa_SecureStream*)(*a_ppSecureOstrm)->Handle;
    pSecureOstrm  = *a_ppSecureOstrm; /* shortcut */

    /*** encode Request- and SecurityHeader ***/
    uStatus = OpcUa_SecureStream_EncodeAsymmetricSecurityHeader(    pSecureOstrm,
                                                                    pSecureStream->eMessageType,
                                                                    a_eMessageSecurityMode,
                                                                    pSecureStream->SecureChannelId,
                                                                    a_uRequestId,
                                                                    a_pSecurityPolicyUri,
                                                                    a_pSenderCertificate,
                                                                    a_pReceiverCertificateThumbprint);
    OpcUa_GotoErrorIfBad(uStatus);

    /* store postion of stream */
    uStatus = OpcUa_Buffer_GetPosition(&pSecureStream->Buffers[0], &pSecureStream->uBeginOfRequestBody);

    /* Flush trigger must be recalculated after encoding the header */
    uStatus = OpcUa_SecureStream_CalculateFlushTrigger(pSecureStream, uChunkLength);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureStream != OpcUa_Null)
    {
        if(pSecureStream->pSenderPublicKey != OpcUa_Null)
        {
            OpcUa_Key_Clear(pSecureStream->pSenderPublicKey);
            OpcUa_Free(pSecureStream->pSenderPublicKey);
        }
        if(pSecureStream->pReceiverPublicKey != OpcUa_Null)
        {
            OpcUa_Key_Clear(pSecureStream->pReceiverPublicKey);
            OpcUa_Free(pSecureStream->pReceiverPublicKey);
        }
        OpcUa_Free(pSecureStream->Buffers);
        OpcUa_Free(pSecureStream);
    }

    if(*a_ppSecureOstrm != OpcUa_Null)
    {
        OpcUa_Free(*a_ppSecureOstrm);
        *a_ppSecureOstrm = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CreateOutput
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_CreateOutput(   OpcUa_OutputStream*             a_pInnerOstrm,
                                                    OpcUa_SecureStream_Type         a_eMessageType,
                                                    OpcUa_UInt32                    a_uRequestId,
                                                    OpcUa_SecureChannel*            a_pSecureChannel,
                                                    OpcUa_OutputStream**            a_ppOstrm)
{
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;   /* the internal handle */
    OpcUa_OutputStream*     pOstrm              = OpcUa_Null;   /* the outer representation */
    OpcUa_UInt32            uChunkLength        = 0;            /* used to get the buffer size */

    OpcUa_CryptoProvider*   pCryptoProvider     = OpcUa_Null;   /* TODO: Get from secure channel */
    OpcUa_UInt32            uSecureChannelId    = 0;            /* TODO: Get from secure channel */
    OpcUa_UInt32            uTokenId            = 0;            /* TODO: Get from secure channel */

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CreateOutput");

    OpcUa_ReturnErrorIfArgumentNull(a_pInnerOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_ppOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    *a_ppOstrm = OpcUa_Null;

    uSecureChannelId = a_pSecureChannel->SecureChannelId;

    /*** create SecureStream ***/
    pSecureStream = (OpcUa_SecureStream*)OpcUa_Alloc(sizeof(OpcUa_SecureStream));
    OpcUa_GotoErrorIfAllocFailed(pSecureStream);
    OpcUa_MemSetD(pSecureStream, 0, sizeof(OpcUa_SecureStream));

    pSecureStream->uBeginOfRequestBody = 0;

    /* internal buffer management */
    uStatus = a_pInnerOstrm->GetChunkLength((OpcUa_Stream*)a_pInnerOstrm, &uChunkLength);
    OpcUa_GotoErrorIfBad(uStatus);

    pSecureStream->nBuffers             = 1; /* only one buffer for outstreams! */
    pSecureStream->nCurrentReadBuffer   = 0;

    pSecureStream->Buffers = (OpcUa_Buffer*)OpcUa_Alloc(sizeof(OpcUa_Buffer)); /* only one buffer per outstream */
    OpcUa_GotoErrorIfAllocFailed(pSecureStream->Buffers);

    uStatus = OpcUa_Buffer_Initialize(  &pSecureStream->Buffers[0], /* the buffer */
                                        OpcUa_Null,                 /* initialize data */
                                        0,                          /* size of initialize data */
                                        uChunkLength,               /* block size */
                                        uChunkLength,               /* max size = initial size */
                                        OpcUa_True);                /* free memory on delete */
    OpcUa_GotoErrorIfBad(uStatus);

    /* general stream settings */
    pSecureStream->SanityCheck          = OpcUa_SecureStream_SanityCheck;
    pSecureStream->IsClosed             = OpcUa_False;
    pSecureStream->IsLocked             = OpcUa_False;
    pSecureStream->InnerStrm            = (OpcUa_Stream*)a_pInnerOstrm;

    /* encryption management information */
    pSecureStream->RequestId            = a_uRequestId;
    pSecureStream->eMessageType         = eOpcUa_SecureStream_Types_StandardMessage;
    pSecureStream->pSenderCertificate   = OpcUa_Null;
    pSecureStream->pReceiverCertificate = OpcUa_Null;
    pSecureStream->pReceiverCertificateThumbprint = OpcUa_Null;
    pSecureStream->pPrivateKey          = OpcUa_Null;
    pSecureStream->uNoOfFlushes         = 0;
    pSecureStream->uPlainTextBlockSize  = 1;
    pSecureStream->uCipherTextBlockSize = 1;
    pSecureStream->uSignatureSize       = 0;
    pSecureStream->pSecureChannel       = a_pSecureChannel;
    pSecureStream->SecureChannelId      = uSecureChannelId;
    pSecureStream->pSenderPublicKey     = OpcUa_Null;
    pSecureStream->pReceiverPublicKey   = OpcUa_Null;
    pSecureStream->eMessageSecurityMode = a_pSecureChannel->MessageSecurityMode;

    pSecureStream->nAbsolutePosition    = 0;

    /* get security keyset (only CryptoProvider) for calculating flush triggers. */
    uStatus = a_pSecureChannel->GetCurrentSecuritySet(  a_pSecureChannel,
                                                        &uTokenId,
                                                        OpcUa_Null,
                                                        OpcUa_Null,
                                                        &pCryptoProvider);
    OpcUa_GotoErrorIfBad(uStatus);

    /* get the encryption block sizes */
    uStatus = OpcUa_SecureStream_GetSymmetricEncryptionBlockSizes(  pCryptoProvider,
                                                                    &pSecureStream->uPlainTextBlockSize,
                                                                    &pSecureStream->uCipherTextBlockSize);

    if(OpcUa_IsGood(uStatus))
    {
        /* get the signature size */
        uStatus = OpcUa_SecureStream_GetSymmetricSignatureSize( pCryptoProvider,
                                                                &pSecureStream->uSignatureSize);
    }

    /* release reference to security set */
    a_pSecureChannel->ReleaseSecuritySet(   a_pSecureChannel,
                                            uTokenId);

    OpcUa_GotoErrorIfBad(uStatus);

    /* Calculate flush trigger */
    uStatus = OpcUa_SecureStream_CalculateFlushTrigger( pSecureStream,
                                                        uChunkLength);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** create OutputStream ***/
    *a_ppOstrm = (OpcUa_OutputStream*)OpcUa_Alloc(sizeof(OpcUa_OutputStream));
    OpcUa_GotoErrorIfAllocFailed(*a_ppOstrm);
    OpcUa_MemSet(*a_ppOstrm, 0, sizeof(OpcUa_OutputStream));

    (*a_ppOstrm)->Type                  = OpcUa_StreamType_Output;
    (*a_ppOstrm)->Handle                = pSecureStream;
    (*a_ppOstrm)->GetPosition           = OpcUa_SecureStream_GetPosition;
    (*a_ppOstrm)->SetPosition           = OpcUa_SecureStream_SetPosition;
    (*a_ppOstrm)->Close                 = OpcUa_SecureStream_Close;
    (*a_ppOstrm)->Delete                = OpcUa_SecureStream_Delete;
    (*a_ppOstrm)->Write                 = OpcUa_SecureStream_Write;
    (*a_ppOstrm)->Flush                 = OpcUa_SecureStream_Flush;
    (*a_ppOstrm)->DetachBuffer          = OpcUa_SecureStream_DetachBuffer;
    (*a_ppOstrm)->AttachBuffer          = OpcUa_SecureStream_AttachBuffer;
    (*a_ppOstrm)->GetChunkLength        = OpcUa_SecureStream_GetChunkLength;

    pSecureStream = (OpcUa_SecureStream*)(*a_ppOstrm)->Handle;
    pOstrm  = *a_ppOstrm;

    /*** preencode Request- and SecurityHeader ***/
    uStatus = OpcUa_SecureStream_EncodeSymmetricSecurityHeader( pOstrm,
                                                                a_eMessageType,
                                                                uSecureChannelId,
                                                                a_uRequestId,
                                                                uTokenId);
    OpcUa_GotoErrorIfBad(uStatus);

    /* store postion */
    uStatus = OpcUa_Buffer_GetPosition(&pSecureStream->Buffers[0], &pSecureStream->uBeginOfRequestBody);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Flush trigger must be recalculated after encoding the header */
    uStatus = OpcUa_SecureStream_CalculateFlushTrigger(pSecureStream, uChunkLength);
    OpcUa_GotoErrorIfBad(uStatus);

#if 0
    /* shrink data buffer */
    pSecureStream->Buffers[0].Data = OpcUa_ReAlloc(pSecureStream->Buffers[0].Data,pSecureStream->uBeginOfRequestBody);
    pSecureStream->Buffers[0].Size = pSecureStream->uBeginOfRequestBody;
#endif

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSecureStream != OpcUa_Null)
    {
        OpcUa_Free(pSecureStream->Buffers);
        OpcUa_Free(pSecureStream);
    }

    if(*a_ppOstrm != OpcUa_Null)
    {
        OpcUa_Free(*a_ppOstrm);
        *a_ppOstrm = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_DecryptInputBuffer
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_DecryptInputBuffer(  OpcUa_Buffer*           a_pEncryptedBuffer,
                                                                OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                                OpcUa_Key*              a_pCryptoKey,
                                                                OpcUa_Boolean           a_bUseSymmetricAlgorithm,
                                                                OpcUa_Key*              a_pInitialVector)
{

    OpcUa_Byte*         pCipherText             = OpcUa_Null;
    OpcUa_UInt32        uCipherTextLen          = 0;

    OpcUa_Byte*         pPlainText              = OpcUa_Null;
    OpcUa_UInt32        uPlainTextLen           = 0;

    OpcUa_UInt32        uBeginOfEncryptedData   = 0;

#if !OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT
    OpcUa_UInt32        uiPlainTextSpace        = 0;
#endif

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "DecryptInputBuffer");

    /* set the position of the stream back to the beginning */
    uStatus = OpcUa_Buffer_GetPosition( a_pEncryptedBuffer,
                                        &uBeginOfEncryptedData);
    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT

    /* get encrypted data length */
    uCipherTextLen = a_pEncryptedBuffer->EndOfData - uBeginOfEncryptedData;

    pCipherText = (OpcUa_Byte*)OpcUa_Alloc(uCipherTextLen * sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(pCipherText);

    /* get encrypted data from buffer */
    uStatus = OpcUa_Buffer_Read(a_pEncryptedBuffer, pCipherText, &uCipherTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

#else /* OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT */

    uStatus = OpcUa_Buffer_GetData(a_pEncryptedBuffer, &pCipherText, &uCipherTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

    /* get encrypted data length */
    uCipherTextLen = a_pEncryptedBuffer->EndOfData - uBeginOfEncryptedData;

    pCipherText = pCipherText + uBeginOfEncryptedData;

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT */

    /* decrypt data*/
    if(a_bUseSymmetricAlgorithm == OpcUa_False)
    {
        /* get needed buffer length */
        uStatus = a_pCryptoProvider->AsymmetricDecrypt( a_pCryptoProvider,
                                                        pCipherText,
                                                        uCipherTextLen,
                                                        a_pCryptoKey,
                                                        OpcUa_Null,
                                                        &uPlainTextLen);
        OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT

        pPlainText = (OpcUa_Byte*)OpcUa_Alloc(uPlainTextLen * sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pPlainText);

#else /* OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT */

        uStatus = OpcUa_Buffer_GetData(a_pEncryptedBuffer, &pPlainText, &uiPlainTextSpace);
        OpcUa_GotoErrorIfBad(uStatus);

        pPlainText = pPlainText + uBeginOfEncryptedData;

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT */

        /* decrypt */
        uStatus = a_pCryptoProvider->AsymmetricDecrypt( a_pCryptoProvider,
                                                        pCipherText,
                                                        uCipherTextLen,
                                                        a_pCryptoKey,
                                                        pPlainText,
                                                        &uPlainTextLen);
    }
    else
    {
        /* get needed buffer length */
        uStatus = a_pCryptoProvider->SymmetricDecrypt(  a_pCryptoProvider,
                                                        pCipherText,
                                                        uCipherTextLen,
                                                        a_pCryptoKey,
                                                        a_pInitialVector->Key.Data,
                                                        OpcUa_Null,
                                                        &uPlainTextLen);
        OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT

        pPlainText = (OpcUa_Byte*)OpcUa_Alloc(uPlainTextLen * sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pPlainText);

#else /* OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT */

        uStatus = OpcUa_Buffer_GetData(a_pEncryptedBuffer, &pPlainText, &uiPlainTextSpace);
        OpcUa_GotoErrorIfBad(uStatus);

        pPlainText = pPlainText + uBeginOfEncryptedData;

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT */

        /* decrypt */
        uStatus = a_pCryptoProvider->SymmetricDecrypt(  a_pCryptoProvider,
                                                        pCipherText,
                                                        uCipherTextLen,
                                                        a_pCryptoKey,
                                                        a_pInitialVector->Key.Data,
                                                        pPlainText,
                                                        &uPlainTextLen);
    }

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "SecureStream->DecryptInputBuffer: Could not decrypt message!\n");
        OpcUa_GotoError;
    }

#if OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT

    /* set the stream position to the beginning of the sequence header */
    uStatus = OpcUa_Buffer_SetPosition(a_pEncryptedBuffer, uBeginOfEncryptedData);
    OpcUa_GotoErrorIfBad(uStatus);

    /* write the decrypted data back into the stream */
    uStatus = OpcUa_Buffer_Write(   a_pEncryptedBuffer,
                                    pPlainText,
                                    uPlainTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

    /* free unused decrypted data, since it is written in the ciphertextbuffer */
    if(pPlainText != OpcUa_Null)
    {
        OpcUa_Free(pPlainText);
        pPlainText = OpcUa_Null;
    }

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT */

    uStatus = OpcUa_Buffer_SetEndOfData(a_pEncryptedBuffer, uBeginOfEncryptedData + uPlainTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

    /* set the stream position to the beginning of the sequence header */
    uStatus = OpcUa_Buffer_SetPosition(a_pEncryptedBuffer, uBeginOfEncryptedData);
    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT

    if(pCipherText != OpcUa_Null)
    {
        OpcUa_Free(pCipherText);
    }

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT

    if(pPlainText != OpcUa_Null)
    {
        OpcUa_Free(pPlainText);
    }

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_PLAINTEXT */

#if OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT

    if(pCipherText != OpcUa_Null)
    {
        OpcUa_Free(pCipherText);
    }

#endif /* OPCUA_SECURESTREAM_DECRYPT_COPY_CIPHERTEXT */

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_EncryptOutput
 *===========================================================================*/
/* ToDo: use the same buffer for encrypting */
OpcUa_StatusCode OpcUa_SecureStream_EncryptOutput(  OpcUa_OutputStream*     a_pOstrm,
                                                    OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                    OpcUa_Key*              a_pEncryptionKey,
                                                    OpcUa_Boolean           a_bUseSymmetricAlgorithm,
                                                    OpcUa_Key*              a_pInitializationVector)
{
    OpcUa_SecureStream*     pSecureStream       = OpcUa_Null;

    OpcUa_Byte*             pCipherText         = OpcUa_Null;
    OpcUa_UInt32            uCipherTextLen      = 0;

    OpcUa_Byte*             pPlainText          = OpcUa_Null;
    OpcUa_UInt32            uPlainTextLen       = 0;

    OpcUa_UInt32            uBeginOfEncryption  = 0;

#if !OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT
    OpcUa_UInt32            uiCipherTextSpace   = 0;
#endif

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "EncryptOutput");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);

    if(a_bUseSymmetricAlgorithm != OpcUa_False)
    {
        OpcUa_ReturnErrorIfArgumentNull(a_pInitializationVector);
    }

    pSecureStream       = (OpcUa_SecureStream*)a_pOstrm->Handle;

    /*** get data from buffer ***/

    /* set the stream position to the beginning of the request body */
    uStatus = OpcUa_Buffer_GetPosition(&pSecureStream->Buffers[0], &uBeginOfEncryption);
    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT

    /* get message body length */
    uPlainTextLen = pSecureStream->Buffers[0].EndOfData - uBeginOfEncryption;
    pPlainText = (OpcUa_Byte*)OpcUa_Alloc(uPlainTextLen * sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(pPlainText);

    /* read the data from the stream */
    uStatus = OpcUa_Buffer_Read(&pSecureStream->Buffers[0], pPlainText, &uPlainTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

#else /* OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT */

    uStatus = OpcUa_Buffer_GetData(&pSecureStream->Buffers[0], &pPlainText, &uPlainTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

    /* get encrypted data length */
    uPlainTextLen = pSecureStream->Buffers[0].EndOfData - uBeginOfEncryption;

    pPlainText = pPlainText + uBeginOfEncryption;

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT */

    /*** encrypt data ***/
    if(a_bUseSymmetricAlgorithm == OpcUa_False)
    {
        uStatus = a_pCryptoProvider->AsymmetricEncrypt( a_pCryptoProvider,
                                                        pPlainText,
                                                        uPlainTextLen,
                                                        a_pEncryptionKey,
                                                        OpcUa_Null,
                                                        &uCipherTextLen);
        OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT

        pCipherText = (OpcUa_Byte*)OpcUa_Alloc(uCipherTextLen * sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pCipherText);

#else /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

        uStatus = OpcUa_Buffer_GetData(&pSecureStream->Buffers[0], &pCipherText, &uiCipherTextSpace);
        OpcUa_GotoErrorIfBad(uStatus);

        pCipherText = pCipherText + uBeginOfEncryption;

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

        uStatus = a_pCryptoProvider->AsymmetricEncrypt( a_pCryptoProvider,
                                                        pPlainText,
                                                        uPlainTextLen,
                                                        a_pEncryptionKey,
                                                        pCipherText,
                                                        &uCipherTextLen);
    }
    else
    {
        uStatus = a_pCryptoProvider->SymmetricEncrypt(  a_pCryptoProvider,
                                                        pPlainText,
                                                        uPlainTextLen,
                                                        a_pEncryptionKey,
                                                        a_pInitializationVector->Key.Data,
                                                        OpcUa_Null,
                                                        &uCipherTextLen);
        OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT

        pCipherText = (OpcUa_Byte*)OpcUa_Alloc(uCipherTextLen * sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pCipherText);

#else /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

        uStatus = OpcUa_Buffer_GetData(&pSecureStream->Buffers[0], &pCipherText, &uiCipherTextSpace);
        OpcUa_GotoErrorIfBad(uStatus);

        pCipherText = pCipherText + uBeginOfEncryption;

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

        uStatus = a_pCryptoProvider->SymmetricEncrypt(  a_pCryptoProvider,
                                                        pPlainText,
                                                        uPlainTextLen,
                                                        a_pEncryptionKey,
                                                        a_pInitializationVector->Key.Data,
                                                        pCipherText,
                                                        &uCipherTextLen);
    }

    OpcUa_GotoErrorIfBad(uStatus);

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT

    /* set the stream position to the beginning of the request body */
    uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uBeginOfEncryption);
    OpcUa_GotoErrorIfBad(uStatus);

    /* write secured data into transport stream */
    uStatus = OpcUa_Buffer_Write(&pSecureStream->Buffers[0], pCipherText, uCipherTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** clean up ***/
    /* free unused encrypted data, since it is written in the ciphertextbuffer */
    if(pCipherText != OpcUa_Null)
    {
        OpcUa_Free(pCipherText);
    }

#else /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

    /* set the stream position to the beginning of the request body */
    uStatus = OpcUa_Buffer_SetEndOfData(&pSecureStream->Buffers[0], uBeginOfEncryption + uCipherTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uBeginOfEncryption + uCipherTextLen);
    OpcUa_GotoErrorIfBad(uStatus);

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT

    if(pPlainText != OpcUa_Null)
    {
        OpcUa_Free(pPlainText);
    }

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT

    if(pCipherText != OpcUa_Null)
    {
        OpcUa_Free(pCipherText);
    }

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_CIPHERTEXT */

#if OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT

    if(pPlainText != OpcUa_Null)
    {
        OpcUa_Free(pPlainText);
    }

#endif /* OPCUA_SECURESTREAM_ENCRYPT_COPY_PLAINTEXT */

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CalculateEncryptionOutputLength
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_CalculateEncryptionOutputLength(    OpcUa_UInt32            a_uBufferLength,
                                                                        OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                                        OpcUa_Key*              a_pCryptoKey,
                                                                        OpcUa_Boolean           a_bUseSymmetricAlgorithm,
                                                                        OpcUa_Key*              a_pInitalVecor,
                                                                        OpcUa_UInt32*           a_pOutputLength)
{
    OpcUa_Byte*  pDecryptedData   = OpcUa_Null;
    OpcUa_UInt32 encryptedDataLen = 0;

    OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CalculateEncryptionOutputLength");

    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoKey);

    if(a_bUseSymmetricAlgorithm)
    {
        OpcUa_ReturnErrorIfArgumentNull(a_pInitalVecor);
    }

    OpcUa_ReturnErrorIfArgumentNull(a_pOutputLength);

    /* get message body length */
    pDecryptedData = (OpcUa_Byte*)OpcUa_Alloc(a_uBufferLength*sizeof(OpcUa_Byte));
    OpcUa_ReturnErrorIfAllocFailed(pDecryptedData);

    /*** encrypt data ***/
    if(a_bUseSymmetricAlgorithm == OpcUa_False)
    {
        uStatus = a_pCryptoProvider->AsymmetricEncrypt( a_pCryptoProvider,
                                                        pDecryptedData,
                                                        a_uBufferLength,
                                                        a_pCryptoKey,
                                                        OpcUa_Null,
                                                        &encryptedDataLen);
    }
    else
    {
        uStatus = a_pCryptoProvider->SymmetricEncrypt(  a_pCryptoProvider,
                                                        pDecryptedData,
                                                        a_uBufferLength,
                                                        a_pCryptoKey,
                                                        a_pInitalVecor->Key.Data,
                                                        OpcUa_Null,
                                                        &encryptedDataLen);
    }

    OpcUa_GotoErrorIfBad(uStatus);

    *a_pOutputLength = encryptedDataLen;

    /*** clean up ***/
   /* free unused encrypted data, since it is written in the ciphertextbuffer */
    if(pDecryptedData != OpcUa_Null)
    {
        OpcUa_Free(pDecryptedData);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pDecryptedData != OpcUa_Null)
    {
        OpcUa_Free(pDecryptedData);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_SignOutput
 *===========================================================================*/
/* ToDo: use the same buffer for signing */
OpcUa_StatusCode OpcUa_SecureStream_SignOutput( OpcUa_OutputStream*     a_pOstrm,
                                                OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                OpcUa_Key*              a_pEncryptionKey,
                                                OpcUa_Boolean           a_bUseSymmetricAlgorithm)
{
    OpcUa_SecureStream* pSecureStream       = OpcUa_Null;
    OpcUa_Byte*         pDataBytes          = OpcUa_Null;
    OpcUa_UInt32        uDataLen            = 0;
    OpcUa_ByteString    bsSignature         = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_ByteString    bsData               = OPCUA_BYTESTRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "SignOutput");

    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm);
    OpcUa_ReturnErrorIfArgumentNull(a_pOstrm->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pEncryptionKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);

    pSecureStream   = (OpcUa_SecureStream*)a_pOstrm->Handle;

    /* get data from buffer */
    uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], 0);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Buffer_GetData(&pSecureStream->Buffers[0], &pDataBytes, &uDataLen);
    OpcUa_GotoErrorIfBad(uStatus);

    bsData.Length = uDataLen;
    bsData.Data   = pDataBytes;

    pDataBytes = OpcUa_Null;

    bsSignature.Length = pSecureStream->uSignatureSize;
    bsSignature.Data   = (OpcUa_Byte*)OpcUa_Alloc(bsSignature.Length * sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(bsSignature.Data);

    /* sign data*/
    if(a_bUseSymmetricAlgorithm == OpcUa_False)
    {
        uStatus = a_pCryptoProvider->AsymmetricSign(a_pCryptoProvider,
                                                    bsData,
                                                    a_pEncryptionKey,
                                                    &bsSignature);
    }
    else
    {
        uStatus = a_pCryptoProvider->SymmetricSign( a_pCryptoProvider,
                                                    bsData.Data,
                                                    bsData.Length,
                                                    a_pEncryptionKey,
                                                    &bsSignature);
    }

    OpcUa_GotoErrorIfBad(uStatus);

    /* move to end of buffer */
    uStatus = OpcUa_Buffer_SetPosition(&pSecureStream->Buffers[0], uDataLen);
    OpcUa_GotoErrorIfBad(uStatus);

    /* add signature */
    uStatus = OpcUa_Buffer_Write(&pSecureStream->Buffers[0], bsSignature.Data, bsSignature.Length);
    OpcUa_GotoErrorIfBad(uStatus);

    /*** clean up ***/
    OpcUa_ByteString_Clear(&bsSignature);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_ByteString_Clear(&bsSignature);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_CalculateSignatureOutputLength
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureStream_CalculateSignatureOutputLength( OpcUa_UInt32            a_uBufferLength,
                                                                    OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                                    OpcUa_Key*              a_pCryptoKey,
                                                                    OpcUa_Boolean           a_bUseSymmetricAlgorithm,
                                                                    OpcUa_UInt32*           a_pOutputLength)
{
    OpcUa_ByteString bsSignature = OPCUA_BYTESTRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "CalculateSignatureOutputLength");

    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pOutputLength);

    /* sign data*/
    if(a_bUseSymmetricAlgorithm == OpcUa_False)
    {
        OpcUa_ByteString bsData = OPCUA_BYTESTRING_STATICINITIALIZER;

        bsData.Length = a_uBufferLength;
        uStatus = a_pCryptoProvider->AsymmetricSign(a_pCryptoProvider,
                                                    bsData,
                                                    a_pCryptoKey,
                                                    &bsSignature);

        OpcUa_ByteString_Clear(&bsData);
    }
    else
    {
        uStatus = a_pCryptoProvider->SymmetricSign( a_pCryptoProvider,
                                                    OpcUa_Null,
                                                    a_uBufferLength,
                                                    a_pCryptoKey,
                                                    &bsSignature);
    }
    OpcUa_GotoErrorIfBad(uStatus);

    /* buffer length +  signature length */
    *a_pOutputLength = a_uBufferLength + bsSignature.Length;

    /*** clean up ***/
    OpcUa_ByteString_Clear(&bsSignature);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_ByteString_Clear(&bsSignature);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureStream_VerifyInput
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SecureStream_VerifyInputBuffer(   OpcUa_SecureStream*     a_pStream,
                                                                OpcUa_Buffer*           a_pBuffer,
                                                                OpcUa_CryptoProvider*   a_pCryptoProvider,
                                                                OpcUa_Key*              a_pEncryptionKey,
                                                                OpcUa_Boolean           a_bUseSymmetricAlgorithm)
{
    OpcUa_ByteString    bsSignature = OPCUA_BYTESTRING_STATICINITIALIZER;
    OpcUa_ByteString    bsData      = OPCUA_BYTESTRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_SecureStream, "VerifyInputBuffer");

    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);
    OpcUa_ReturnErrorIfTrue(a_pBuffer->EndOfData < a_pBuffer->Position + a_pStream->uSignatureSize, OpcUa_BadInvalidArgument);

    /* contain data */
    bsData.Data         = a_pBuffer->Data;

    /* contain signature */
    bsSignature.Data    = &a_pBuffer->Data[a_pBuffer->EndOfData - a_pStream->uSignatureSize];
    bsSignature.Length  = a_pStream->uSignatureSize;
    bsData.Length       = (OpcUa_UInt32)(bsSignature.Data - bsData.Data);

    /* verify signature */
    if(a_bUseSymmetricAlgorithm == OpcUa_False)
    {
        uStatus = a_pCryptoProvider->AsymmetricVerify( a_pCryptoProvider,
                                                       bsData,
                                                       a_pEncryptionKey,
                                                       &bsSignature);
    }
    else
    {
        uStatus = a_pCryptoProvider->SymmetricVerify(   a_pCryptoProvider,
                                                        bsData.Data,
                                                        bsData.Length,
                                                        a_pEncryptionKey,
                                                        &bsSignature);
    }

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
