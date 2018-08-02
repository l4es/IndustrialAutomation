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

/* UA platform definitions */
#include <opcua_p_internal.h>
#include <opcua_p_memory.h>

#if OPCUA_REQUIRE_OPENSSL

/* System Headers */
#include <openssl/rsa.h>
#include <openssl/evp.h>

/* own headers */
#include <opcua_p_openssl.h>
#include <opcua_p_pki.h>

#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
#define get_pkey_rsa(evp) EVP_PKEY_get0_RSA(evp)
#else
#define get_pkey_rsa(evp) ((evp)->pkey.rsa)
#endif

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_GenerateKeys
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_GenerateKeys(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_UInt32            a_bits,
    OpcUa_Key*              a_pPublicKey,
    OpcUa_Key*              a_pPrivateKey)
{
    RSA*            pRsa        = OpcUa_Null;
    unsigned char*  pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_GenerateKeys");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPublicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pPrivateKey);

    a_pPublicKey->Key.Data      = OpcUa_Null;
    a_pPrivateKey->Key.Data     = OpcUa_Null;

    if((a_bits < a_pProvider->MinimumAsymmetricKeyLength*8) || (a_bits > a_pProvider->MaximumAsymmetricKeyLength*8))
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    pRsa = RSA_generate_key(a_bits, RSA_F4, NULL, OpcUa_Null);
    OpcUa_GotoErrorIfNull(pRsa, OpcUa_Bad);

    /* get required length */
    a_pPublicKey->Key.Length = i2d_RSAPublicKey(pRsa, NULL);
    OpcUa_GotoErrorIfTrue((a_pPublicKey->Key.Length <= 0), OpcUa_Bad);

    /* allocate target buffer */
    a_pPublicKey->Key.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pPublicKey->Key.Length);
    OpcUa_GotoErrorIfAllocFailed(a_pPublicKey->Key.Data);

    pData = a_pPublicKey->Key.Data;
    a_pPublicKey->Key.Length = i2d_RSAPublicKey(pRsa, &pData);

    /* get required length */
    a_pPrivateKey->Key.Length = i2d_RSAPrivateKey(pRsa, NULL);
    OpcUa_GotoErrorIfTrue((a_pPrivateKey->Key.Length <= 0), OpcUa_Bad);

    /* allocate target buffer */
    a_pPrivateKey->Key.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pPrivateKey->Key.Length);
    OpcUa_GotoErrorIfAllocFailed(a_pPrivateKey->Key.Data);

    pData = a_pPrivateKey->Key.Data;
    a_pPrivateKey->Key.Length = i2d_RSAPrivateKey(pRsa, &pData);

    /* clean up */
    RSA_free(pRsa);

    a_pPublicKey->Type = OpcUa_Crypto_KeyType_Rsa_Public;
    a_pPrivateKey->Type = OpcUa_Crypto_KeyType_Rsa_Private;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(a_pPublicKey->Key.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pPublicKey->Key.Data);
        a_pPublicKey->Key.Data = OpcUa_Null;
    }

    if(pRsa != OpcUa_Null)
    {
        RSA_free(pRsa);
    }

OpcUa_FinishErrorHandling;
}

/*===========================================================================*
OpcUa_P_OpenSSL_RSA_Public_GetKeyLength
*===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_Public_GetKeyLength(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Key               a_publicKey,
    OpcUa_UInt32*           a_pKeyLen)
{
    EVP_PKEY*       pPublicKey      = OpcUa_Null;
    const unsigned char *pData;

    OpcUa_UInt32    uKeySize            = 0;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_Public_GetKeyLength");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey.Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pKeyLen);

    *a_pKeyLen = 0;

    if(a_publicKey.Type != OpcUa_Crypto_KeyType_Rsa_Public)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_publicKey.Key.Data;
    pPublicKey = d2i_PublicKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_publicKey.Key.Length);

    if(pPublicKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    uKeySize = RSA_size(get_pkey_rsa(pPublicKey));

    if((uKeySize < a_pProvider->MinimumAsymmetricKeyLength) || (uKeySize > a_pProvider->MaximumAsymmetricKeyLength))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecurityConfig);
    }

    *a_pKeyLen = uKeySize*8;

    EVP_PKEY_free(pPublicKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pPublicKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPublicKey);
    }

    *a_pKeyLen = (OpcUa_UInt32)-1;

OpcUa_FinishErrorHandling;
}

/*** RSA ASYMMETRIC ENCRYPTION ***/

/*===========================================================================*
OpcUa_P_OpenSSL_RSA_Public_Encrypt
*===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_Public_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen,
    OpcUa_Key*              a_publicKey,
    OpcUa_Int16             a_padding,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    EVP_PKEY*       pPublicKey      = OpcUa_Null;

    OpcUa_UInt32    uKeySize            = 0;
    OpcUa_UInt32    uEncryptedDataSize  = 0;
    OpcUa_UInt32    uPlainTextPosition  = 0;
    OpcUa_UInt32    uCipherTextPosition = 0;
    OpcUa_UInt32    uBytesToEncrypt     = 0;
    OpcUa_Int32     iEncryptedBytes     = 0;
    const unsigned char *pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_Public_Encrypt");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_publicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pCipherTextLen);

    *a_pCipherTextLen = 0;

    if((OpcUa_Int32)a_plainTextLen < 1)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(a_publicKey->Type != OpcUa_Crypto_KeyType_Rsa_Public)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_publicKey->Key.Data;
    pPublicKey = d2i_PublicKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_publicKey->Key.Length);

    if(pPublicKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    uKeySize = RSA_size(get_pkey_rsa(pPublicKey));

    /* check padding type */
    switch(a_padding)
    {
    case RSA_PKCS1_PADDING:
        {
            if(uKeySize <= 11)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            uEncryptedDataSize = uKeySize - 11;
            break;
        }
    case RSA_PKCS1_OAEP_PADDING:
        {
            if(uKeySize <= 42)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            uEncryptedDataSize = uKeySize - 42;
            break;
        }
    case RSA_NO_PADDING:
        {
            if(uKeySize == 0)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            uEncryptedDataSize = uKeySize;
            break;
        }
    default:
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
        }
    }

    uPlainTextPosition  = a_plainTextLen;
    uCipherTextPosition = ((uPlainTextPosition - 1) / uEncryptedDataSize + 1) * uKeySize;
    uBytesToEncrypt     = (uPlainTextPosition - 1) % uEncryptedDataSize + 1;
    *a_pCipherTextLen   = uCipherTextPosition;

    if((a_pCipherText != OpcUa_Null) && (a_pPlainText != OpcUa_Null))
    {

        /* encrypt in reverse order so that a_pCipherText may alias a_pPlainText */
        while(uPlainTextPosition > 0)
        {
            uCipherTextPosition -= uKeySize;
            uPlainTextPosition  -= uBytesToEncrypt;

            iEncryptedBytes = RSA_public_encrypt(   uBytesToEncrypt,                    /* how much to encrypt  */
                                                    a_pPlainText + uPlainTextPosition,  /* what to encrypt      */
                                                    a_pCipherText + uCipherTextPosition,/* where to encrypt     */
                                                    get_pkey_rsa(pPublicKey),           /* public key           */
                                                    a_padding);                         /* padding mode         */
            if(iEncryptedBytes < 0)
            {
                uStatus = OpcUa_Bad;
                OpcUa_GotoError;
            }

            uBytesToEncrypt = uEncryptedDataSize;
        }

    }

    EVP_PKEY_free(pPublicKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pPublicKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPublicKey);
    }

    *a_pCipherTextLen = (OpcUa_UInt32)-1;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_Private_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_Private_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen,
    OpcUa_Key*              a_privateKey,
    OpcUa_Int16             a_padding,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pPlainTextLen)
{
    EVP_PKEY*       pPrivateKey     = OpcUa_Null;

    OpcUa_UInt32    keySize         = 0;
    OpcUa_Int32     decryptedBytes  = 0;
    OpcUa_UInt32    iCipherText     = 0;
    OpcUa_UInt32    decDataSize     = 0;

    const unsigned char *pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_Private_Decrypt");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pCipherText);
    OpcUa_ReturnErrorIfArgumentNull(a_privateKey);
    OpcUa_ReturnErrorIfArgumentNull(a_privateKey->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pPlainTextLen);

    *a_pPlainTextLen = 0;

    if((OpcUa_Int32)a_cipherTextLen < 1)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(a_privateKey->Type != OpcUa_Crypto_KeyType_Rsa_Private)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_privateKey->Key.Data;
    pPrivateKey = d2i_PrivateKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_privateKey->Key.Length);

    if(pPrivateKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    keySize = RSA_size(get_pkey_rsa(pPrivateKey));

    if(keySize == 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    if((a_cipherTextLen%keySize) != 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    /* check padding type */
    switch(a_padding)
    {
    case RSA_PKCS1_PADDING:
        {
            if(keySize <= 11)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            decDataSize = keySize - 11;
            break;
        }
    case RSA_PKCS1_OAEP_PADDING:
        {
            if(keySize <= 42)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
            }
            decDataSize = keySize - 42;
            break;
        }
    case RSA_NO_PADDING:
        {
            decDataSize = keySize;
            break;
        }
    default:
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
        }
    }

    while(iCipherText < a_cipherTextLen)
    {
        if(a_pPlainText != OpcUa_Null)
        {
            decryptedBytes = RSA_private_decrypt(   keySize,                            /* how much to decrypt  */
                                                    a_pCipherText + iCipherText,        /* what to decrypt      */
                                                    a_pPlainText + (*a_pPlainTextLen),  /* where to decrypt     */
                                                    get_pkey_rsa(pPrivateKey),          /* private key          */
                                                    a_padding);                         /* padding mode         */

            /* if decryption fails return the same result as if signature check fails */
            /* also fail if zero bytes are decoded */
            if(decryptedBytes <= 0)
            {
                /* continue decrypting the message in constant time */
                uStatus = OpcUa_BadSignatureInvalid;
                decryptedBytes = decDataSize;
                /* do not leak timing information by skipping the memcpy */
                memmove(a_pPlainText + (*a_pPlainTextLen), a_pCipherText + iCipherText, decryptedBytes);
            }
            /* only the last part may be smaller */
            else if(iCipherText + keySize < a_cipherTextLen && (OpcUa_UInt32)decryptedBytes != decDataSize)
            {
                uStatus = OpcUa_BadSignatureInvalid;
            }
        }
        else
        {
            decryptedBytes = decDataSize;
        }

        *a_pPlainTextLen = *a_pPlainTextLen + decryptedBytes;
        iCipherText = iCipherText + keySize;
    }

    EVP_PKEY_free(pPrivateKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pPrivateKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPrivateKey);
    }

    *a_pPlainTextLen = (OpcUa_UInt32)-1;

OpcUa_FinishErrorHandling;
}

/*** RSA ASYMMETRIC SIGNATURE ***/

/*===========================================================================*
OpcUa_P_OpenSSL_RSA_Private_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_Private_Sign(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_privateKey,
    OpcUa_Int16           a_padding,          /* The signature algorithm used (e.g. NID_sha1). */
    OpcUa_ByteString*     a_pSignature)       /* output length >= key length */
{
    EVP_PKEY*               pSSLPrivateKey  = OpcUa_Null;
    const unsigned char*    pData           = OpcUa_Null;
    int                     iErr            = 0;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_Private_Sign");

    /* unused parameters */
    OpcUa_ReferenceParameter(a_pProvider);

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_data.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_privateKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignature->Data);
    pData = a_privateKey->Key.Data;
    OpcUa_ReturnErrorIfArgumentNull(pData);
    OpcUa_ReturnErrorIfTrue((a_privateKey->Type != OpcUa_Crypto_KeyType_Rsa_Private), OpcUa_BadInvalidArgument);

    /* convert private key and check key length against buffer length */
    pSSLPrivateKey = d2i_PrivateKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_privateKey->Key.Length);
    OpcUa_GotoErrorIfTrue((pSSLPrivateKey == OpcUa_Null), OpcUa_BadUnexpectedError);
    OpcUa_GotoErrorIfTrue((a_pSignature->Length < RSA_size(get_pkey_rsa(pSSLPrivateKey))), OpcUa_BadInvalidArgument);

    /* sign data */
    iErr = RSA_sign(a_padding, a_data.Data, a_data.Length, a_pSignature->Data, (unsigned int*)&a_pSignature->Length, get_pkey_rsa(pSSLPrivateKey));
    OpcUa_GotoErrorIfTrue((iErr != 1), OpcUa_BadUnexpectedError);

    /* free internal key representation */
    EVP_PKEY_free(pSSLPrivateKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pSSLPrivateKey != OpcUa_Null)
    {
        EVP_PKEY_free(pSSLPrivateKey);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_Public_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_Public_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_publicKey,
    OpcUa_Int16           a_padding,          /* The signature algorithm used (e.g. NID_sha1). */
    OpcUa_ByteString*     a_pSignature)
{
    EVP_PKEY*            pPublicKey      = OpcUa_Null;
    OpcUa_Int32          keySize         = 0;
    const unsigned char *pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_Public_Verify");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_data.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);

    if(a_publicKey->Type != OpcUa_Crypto_KeyType_Rsa_Public)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_publicKey->Key.Data;
    pPublicKey = d2i_PublicKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_publicKey->Key.Length);

    if(pPublicKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    keySize = RSA_size(get_pkey_rsa(pPublicKey));

    if(keySize == 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    if(RSA_verify(a_padding, a_data.Data, a_data.Length, a_pSignature->Data, a_pSignature->Length, get_pkey_rsa(pPublicKey)) != 1)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSignatureInvalid);
    }

    EVP_PKEY_free(pPublicKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pPublicKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPublicKey);
    }

OpcUa_FinishErrorHandling;
}

/*** EVP-BASED RSA OAEP SHA256 ASYMMETRIC ENCRYPTION (requires OpenSSL 1.0.2) ***/

/*===========================================================================*
OpcUa_P_OpenSSL_RSA_SHA256_Public_Encrypt
*===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_SHA256_Public_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen,
    OpcUa_Key*              a_publicKey,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
#if OPENSSL_VERSION_NUMBER < 0x1000200fL
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_pCipherText);
    OpcUa_ReferenceParameter(a_pCipherTextLen);
    OpcUa_ReferenceParameter(a_publicKey);
    OpcUa_ReferenceParameter(a_pPlainText);
    OpcUa_ReferenceParameter(a_plainTextLen);
    return OpcUa_BadNotSupported;
#else
    EVP_PKEY_CTX*   pCtx            = OpcUa_Null;
    EVP_PKEY*       pPublicKey      = OpcUa_Null;

    OpcUa_UInt32    uKeySize            = 0;
    OpcUa_UInt32    uEncryptedDataSize  = 0;
    OpcUa_UInt32    uPlainTextPosition  = 0;
    OpcUa_UInt32    uCipherTextPosition = 0;
    OpcUa_UInt32    uBytesToEncrypt     = 0;
    size_t          iEncryptedBytes     = 0;
    int             ret                 = 0;
    const unsigned char *pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_SHA256_Public_Encrypt");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_publicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pCipherTextLen);

    *a_pCipherTextLen = 0;

    if((OpcUa_Int32)a_plainTextLen < 1)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(a_publicKey->Type != OpcUa_Crypto_KeyType_Rsa_Public)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_publicKey->Key.Data;
    pPublicKey = d2i_PublicKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_publicKey->Key.Length);

    if(pPublicKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    uKeySize = RSA_size(get_pkey_rsa(pPublicKey));

    if(uKeySize <= 66)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }
    uEncryptedDataSize = uKeySize - 66;

    pCtx = EVP_PKEY_CTX_new(pPublicKey, NULL);
    OpcUa_GotoErrorIfTrue((pCtx == OpcUa_Null), OpcUa_Bad);
    ret = EVP_PKEY_encrypt_init(pCtx);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_OAEP_PADDING);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_oaep_md(pCtx, EVP_sha256());
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);

    uPlainTextPosition  = a_plainTextLen;
    uCipherTextPosition = ((uPlainTextPosition - 1) / uEncryptedDataSize + 1) * uKeySize;
    uBytesToEncrypt     = (uPlainTextPosition - 1) % uEncryptedDataSize + 1;
    *a_pCipherTextLen   = uCipherTextPosition;

    if((a_pCipherText != OpcUa_Null) && (a_pPlainText != OpcUa_Null))
    {

        /* encrypt in reverse order so that a_pCipherText may alias a_pPlainText */
        while(uPlainTextPosition > 0)
        {
            uCipherTextPosition -= uKeySize;
            uPlainTextPosition  -= uBytesToEncrypt;

            iEncryptedBytes = uKeySize;
            ret = EVP_PKEY_encrypt(pCtx,
                                   a_pCipherText + uCipherTextPosition,/* where to encrypt     */
                                   &iEncryptedBytes,
                                   a_pPlainText + uPlainTextPosition,  /* what to encrypt      */
                                   uBytesToEncrypt);                   /* how much to encrypt  */

            if(ret < 0)
            {
                uStatus = OpcUa_Bad;
                OpcUa_GotoError;
            }

            uBytesToEncrypt = uEncryptedDataSize;
        }

    }

    EVP_PKEY_CTX_free(pCtx);
    EVP_PKEY_free(pPublicKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pCtx != OpcUa_Null)
    {
        EVP_PKEY_CTX_free(pCtx);
    }

    if(pPublicKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPublicKey);
    }

    *a_pCipherTextLen = (OpcUa_UInt32)-1;

OpcUa_FinishErrorHandling;
#endif
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_SHA256_Private_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_SHA256_Private_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen,
    OpcUa_Key*              a_privateKey,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pPlainTextLen)
{
#if OPENSSL_VERSION_NUMBER < 0x1000200fL
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_pCipherText);
    OpcUa_ReferenceParameter(a_cipherTextLen);
    OpcUa_ReferenceParameter(a_privateKey);
    OpcUa_ReferenceParameter(a_pPlainText);
    OpcUa_ReferenceParameter(a_pPlainTextLen);
    return OpcUa_BadNotSupported;
#else
    EVP_PKEY_CTX*   pCtx            = OpcUa_Null;
    EVP_PKEY*       pPrivateKey     = OpcUa_Null;

    OpcUa_UInt32    keySize         = 0;
    size_t          decryptedBytes  = 0;
    OpcUa_UInt32    iCipherText     = 0;
    OpcUa_UInt32    decDataSize     = 0;
    int             ret             = 0;

    const unsigned char *pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_SHA256_Private_Decrypt");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pCipherText);
    OpcUa_ReturnErrorIfArgumentNull(a_privateKey);
    OpcUa_ReturnErrorIfArgumentNull(a_privateKey->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pPlainTextLen);

    *a_pPlainTextLen = 0;

    if((OpcUa_Int32)a_cipherTextLen < 1)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(a_privateKey->Type != OpcUa_Crypto_KeyType_Rsa_Private)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_privateKey->Key.Data;
    pPrivateKey = d2i_PrivateKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_privateKey->Key.Length);

    if(pPrivateKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    keySize = RSA_size(get_pkey_rsa(pPrivateKey));

    if(keySize == 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    if((a_cipherTextLen%keySize) != 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    /* check padding type */
    if(keySize <= 66)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }
    decDataSize = keySize - 66;

    pCtx = EVP_PKEY_CTX_new(pPrivateKey, NULL);
    OpcUa_GotoErrorIfTrue((pCtx == OpcUa_Null), OpcUa_Bad);
    ret = EVP_PKEY_decrypt_init(pCtx);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_OAEP_PADDING);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_oaep_md(pCtx, EVP_sha256());
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);

    while(iCipherText < a_cipherTextLen)
    {
        if(a_pPlainText != OpcUa_Null)
        {
            decryptedBytes = keySize; /* actually that's a lie */
            ret = EVP_PKEY_decrypt(pCtx,
                                   a_pPlainText + (*a_pPlainTextLen),  /* where to decrypt     */
                                   &decryptedBytes,
                                   a_pCipherText + iCipherText,        /* what to decrypt      */
                                   keySize);

            /* if decryption fails return the same result as if signature check fails */
            /* also fail if zero bytes are decoded */
            if(ret <= 0 || decryptedBytes == 0)
            {
                /* continue decrypting the message in constant time */
                uStatus = OpcUa_BadSignatureInvalid;
                decryptedBytes = decDataSize;
                /* do not leak timing information by skipping the memcpy */
                memmove(a_pPlainText + (*a_pPlainTextLen), a_pCipherText + iCipherText, decryptedBytes);
            }
            /* only the last part may be smaller */
            else if(iCipherText + keySize < a_cipherTextLen && decryptedBytes != decDataSize)
            {
                uStatus = OpcUa_BadSignatureInvalid;
            }
        }
        else
        {
            decryptedBytes = decDataSize;
        }

        *a_pPlainTextLen = *a_pPlainTextLen + decryptedBytes;
        iCipherText = iCipherText + keySize;
    }

    EVP_PKEY_CTX_free(pCtx);
    EVP_PKEY_free(pPrivateKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pCtx != OpcUa_Null)
    {
        EVP_PKEY_CTX_free(pCtx);
    }

    if(pPrivateKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPrivateKey);
    }

    *a_pPlainTextLen = (OpcUa_UInt32)-1;

OpcUa_FinishErrorHandling;
#endif
}

/*** EVP-BASED RSA SSA PSS ASYMMETRIC SIGNATURE (requires OpenSSL 1.0.0) ***/

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PSS_Private_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PSS_Private_Sign(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_privateKey,
    OpcUa_ByteString*     a_pSignature)
{
#if OPENSSL_VERSION_NUMBER < 0x1000000fL
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_data);
    OpcUa_ReferenceParameter(a_privateKey);
    OpcUa_ReferenceParameter(a_pSignature);
    return OpcUa_BadNotSupported;
#else
    EVP_PKEY_CTX*           pCtx            = OpcUa_Null;
    EVP_PKEY*               pSSLPrivateKey  = OpcUa_Null;
    const unsigned char*    pData           = OpcUa_Null;
    int                     ret             = 0;
    size_t                  siglen          = 0;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PSS_Private_Sign");

    /* unused parameters */
    OpcUa_ReferenceParameter(a_pProvider);

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_data.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_privateKey);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignature->Data);
    pData = a_privateKey->Key.Data;
    OpcUa_ReturnErrorIfArgumentNull(pData);
    OpcUa_ReturnErrorIfTrue((a_privateKey->Type != OpcUa_Crypto_KeyType_Rsa_Private), OpcUa_BadInvalidArgument);

    /* convert private key and check key length against buffer length */
    pSSLPrivateKey = d2i_PrivateKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_privateKey->Key.Length);
    OpcUa_GotoErrorIfTrue((pSSLPrivateKey == OpcUa_Null), OpcUa_BadUnexpectedError);
    OpcUa_GotoErrorIfTrue((a_pSignature->Length < RSA_size(get_pkey_rsa(pSSLPrivateKey))), OpcUa_BadInvalidArgument);

    pCtx = EVP_PKEY_CTX_new(pSSLPrivateKey, NULL);
    OpcUa_GotoErrorIfTrue((pCtx == OpcUa_Null), OpcUa_Bad);
    ret = EVP_PKEY_sign_init(pCtx);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_PSS_PADDING);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_signature_md(pCtx, EVP_sha256());
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_pss_saltlen(pCtx, -1);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    siglen = a_pSignature->Length;
    ret = EVP_PKEY_sign(pCtx, a_pSignature->Data, &siglen, a_data.Data, a_data.Length);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_BadUnexpectedError);
    a_pSignature->Length = (OpcUa_Int32)siglen;

    EVP_PKEY_CTX_free(pCtx);
    EVP_PKEY_free(pSSLPrivateKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pCtx != OpcUa_Null)
    {
        EVP_PKEY_CTX_free(pCtx);
    }

    if(pSSLPrivateKey != OpcUa_Null)
    {
        EVP_PKEY_free(pSSLPrivateKey);
    }

OpcUa_FinishErrorHandling;
#endif
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PSS_Public_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PSS_Public_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_publicKey,
    OpcUa_ByteString*     a_pSignature)
{
#if OPENSSL_VERSION_NUMBER < 0x1000000fL
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_data);
    OpcUa_ReferenceParameter(a_publicKey);
    OpcUa_ReferenceParameter(a_pSignature);
    return OpcUa_BadNotSupported;
#else
    EVP_PKEY_CTX*        pCtx            = OpcUa_Null;
    EVP_PKEY*            pPublicKey      = OpcUa_Null;
    const unsigned char *pData           = OpcUa_Null;
    int                  ret             = 0;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PSS_Public_Verify");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_data.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey);
    OpcUa_ReturnErrorIfArgumentNull(a_publicKey->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);

    if(a_publicKey->Type != OpcUa_Crypto_KeyType_Rsa_Public)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pData = a_publicKey->Key.Data;
    pPublicKey = d2i_PublicKey(EVP_PKEY_RSA, OpcUa_Null, &pData, a_publicKey->Key.Length);

    if(pPublicKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    pCtx = EVP_PKEY_CTX_new(pPublicKey, NULL);
    OpcUa_GotoErrorIfTrue((pCtx == OpcUa_Null), OpcUa_Bad);
    ret = EVP_PKEY_verify_init(pCtx);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_PSS_PADDING);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_signature_md(pCtx, EVP_sha256());
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_CTX_set_rsa_pss_saltlen(pCtx, -1);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_Bad);
    ret = EVP_PKEY_verify(pCtx, a_pSignature->Data, a_pSignature->Length, a_data.Data, a_data.Length);
    OpcUa_GotoErrorIfTrue((ret <= 0), OpcUa_BadSignatureInvalid);

    EVP_PKEY_CTX_free(pCtx);
    EVP_PKEY_free(pPublicKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pCtx != OpcUa_Null)
    {
        EVP_PKEY_CTX_free(pCtx);
    }

    if(pPublicKey != OpcUa_Null)
    {
        EVP_PKEY_free(pPublicKey);
    }

OpcUa_FinishErrorHandling;
#endif
}

#endif /* OPCUA_REQUIRE_OPENSSL */
