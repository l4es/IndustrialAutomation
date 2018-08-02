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
#include <opcua_p_cryptofactory.h>
#include <opcua_p_mutex.h>
#include <opcua_p_thread.h>

#if OPCUA_REQUIRE_OPENSSL

/* System Headers */
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#if OPCUA_P_SOCKETMANAGER_SUPPORT_SSL
#include <openssl/ssl.h>
#endif /* OPCUA_P_SOCKETMANAGER_SUPPORT_SSL */

/* own headers */
#include <opcua_p_openssl.h>

/*============================================================================
 * OpcUa_P_ByteString_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_P_ByteString_Clear(OpcUa_ByteString* a_pValue)
{
    if(a_pValue == OpcUa_Null)
    {
        return;
    }

    a_pValue->Length = -1;

    if(a_pValue->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pValue->Data);
        a_pValue->Data = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_Key_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_P_Key_Clear(OpcUa_Key* a_pKey)
{
    OpcUa_P_ByteString_Clear(&a_pKey->Key);
    a_pKey->Type = 0;
}

/*============================================================================
 * simple locking for OpenSSL
 *===========================================================================*/
#if OPCUA_USE_SYNCHRONISATION
static OpcUa_Mutex OpenSSL_Mutex;
static void OpcUa_P_OpenSSL_Lock(int mode, int type, const char *file, int line)
{
    OpcUa_ReferenceParameter(type);
    OpcUa_ReferenceParameter(file);
    OpcUa_ReferenceParameter(line);
    if(mode & CRYPTO_LOCK)
    {
        OpcUa_P_Mutex_Lock(OpenSSL_Mutex);
    }
    if(mode & CRYPTO_UNLOCK)
    {
        OpcUa_P_Mutex_Unlock(OpenSSL_Mutex);
    }
}
#endif /* OPCUA_USE_SYNCHRONISATION */

/*============================================================================
 * OpcUa_P_OpenSSL_Initialize
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_Initialize()
{
#if OPCUA_USE_SYNCHRONISATION
    OpcUa_StatusCode uStatus = OpcUa_P_Mutex_Create(&OpenSSL_Mutex);
    OpcUa_ReturnErrorIfBad(uStatus);
    CRYPTO_set_id_callback(OpcUa_P_Thread_GetCurrentThreadId);
    CRYPTO_set_locking_callback(OpcUa_P_OpenSSL_Lock);
#endif /* OPCUA_USE_SYNCHRONISATION */
    OpenSSL_add_all_algorithms();
#if OPCUA_P_SOCKETMANAGER_SUPPORT_SSL
    SSL_library_init();
    SSL_load_error_strings();
#endif /* OPCUA_P_SOCKETMANAGER_SUPPORT_SSL */
    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_P_OpenSSL_Cleanup
 *===========================================================================*/
void OpcUa_P_OpenSSL_Cleanup()
{
#if OPCUA_P_SOCKETMANAGER_SUPPORT_SSL
#if OPENSSL_VERSION_NUMBER >= 0x1000200fL && !defined(OPENSSL_NO_COMP)
    SSL_COMP_free_compression_methods();
#endif
#endif /* OPCUA_P_SOCKETMANAGER_SUPPORT_SSL */
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_remove_state(0);
    ERR_free_strings();
#if OPCUA_USE_SYNCHRONISATION
    CRYPTO_set_id_callback(OpcUa_Null);
    CRYPTO_set_locking_callback(OpcUa_Null);
    OpcUa_P_Mutex_Delete(&OpenSSL_Mutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/*============================================================================
 * OpcUa_P_OpenSSL_Thread_Cleanup()
 *===========================================================================*/
void OPCUA_DLLCALL OpcUa_P_OpenSSL_Thread_Cleanup()
{
    ERR_remove_state(0);
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
    OPENSSL_thread_stop();
#endif
}

/*============================================================================
 * OpcUa_P_OpenSSL_DestroySecretData()
 *===========================================================================*/
void OPCUA_DLLCALL OpcUa_P_OpenSSL_DestroySecretData(OpcUa_Void*  data,
                                                     OpcUa_UInt32 bytes)
{
    OPENSSL_cleanse(data, bytes);
}

/*============================================================================
 * OpcUa_P_OpenSSL_AES_128_CBC_Encrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_AES_128_CBC_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen, /* message length = outputlength */
    OpcUa_Key*              a_key,
    OpcUa_Byte*             a_pInitalVector,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "AES_128_CBC_Encrypt");

    if(a_key->Key.Length == 16) /* check 128 bit key (16*8) */
    {
        uStatus = OpcUa_P_OpenSSL_AES_CBC_Encrypt(
                                                a_pProvider,
                                                a_pPlainText,
                                                a_plainTextLen,
                                                a_key,
                                                a_pInitalVector,
                                                a_pCipherText,
                                                a_pCipherTextLen);
    }
    else
    {
        uStatus = OpcUa_Bad;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_AES_128_CBC_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_AES_128_CBC_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen, /* cipher length */
    OpcUa_Key*              a_key,
    OpcUa_Byte*             a_pInitalVector,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "AES_128_CBC_Decrypt");

    if(a_key->Key.Length == 16) /* check 128 bit key (16*8) */
    {
        uStatus = OpcUa_P_OpenSSL_AES_CBC_Decrypt(
                                                a_pProvider,
                                                a_pCipherText,
                                                a_cipherTextLen,
                                                a_key,
                                                a_pInitalVector,
                                                a_pPlainText,
                                                a_pCipherTextLen);
    }
    else
    {
        uStatus = OpcUa_Bad;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_AES_128_CBC_Encrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_AES_256_CBC_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen, /* message length = outputlength */
    OpcUa_Key*              a_key,
    OpcUa_Byte*             a_pInitalVector,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "AES_256_CBC_Encrypt");

    if(a_key->Key.Length == 32) /* check 256 bit key (32*8) */
    {
        uStatus = OpcUa_P_OpenSSL_AES_CBC_Encrypt(
                                                a_pProvider,
                                                a_pPlainText,
                                                a_plainTextLen,
                                                a_key,
                                                a_pInitalVector,
                                                a_pCipherText,
                                                a_pCipherTextLen);
    }
    else
    {
        uStatus = OpcUa_Bad;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_AES_256_CBC_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_AES_256_CBC_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen, /* cipher length */
    OpcUa_Key*              a_key,
    OpcUa_Byte*             a_pInitalVector,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "AES_256_CBC_Decrypt");

    if(a_key->Key.Length == 32) /* check 256 bit key (32*8) */
    {
        uStatus = OpcUa_P_OpenSSL_AES_CBC_Decrypt(
                                                a_pProvider,
                                                a_pCipherText,
                                                a_cipherTextLen,
                                                a_key,
                                                a_pInitalVector,
                                                a_pPlainText,
                                                a_pCipherTextLen);
    }
    else
    {
        uStatus = OpcUa_Bad;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PKCS1_V15_Encrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PKCS1_V15_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen,
    OpcUa_Key*              a_publicKey,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PKCS1_V15_Encrypt");

    uStatus = OpcUa_P_OpenSSL_RSA_Public_Encrypt(   a_pProvider,
                                                    a_pPlainText,
                                                    a_plainTextLen,
                                                    a_publicKey,
                                                    RSA_PKCS1_PADDING,
                                                    a_pCipherText,
                                                    a_pCipherTextLen);
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PKCS1_V15_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PKCS1_V15_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen,
    OpcUa_Key*              a_privateKey,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pPlainTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PKCS1_V15_Decrypt");

    uStatus = OpcUa_P_OpenSSL_RSA_Private_Decrypt(
                                                a_pProvider,
                                                a_pCipherText,
                                                a_cipherTextLen,
                                                a_privateKey,
                                                RSA_PKCS1_PADDING,
                                                a_pPlainText,
                                                a_pPlainTextLen);
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_OAEP_Encrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_OAEP_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen,
    OpcUa_Key*              a_publicKey,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_OAEP_Encrypt");

    uStatus = OpcUa_P_OpenSSL_RSA_Public_Encrypt(
                                                a_pProvider,
                                                a_pPlainText,
                                                a_plainTextLen,
                                                a_publicKey,
                                                RSA_PKCS1_OAEP_PADDING,
                                                a_pCipherText,
                                                a_pCipherTextLen);
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_OAEP_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_OAEP_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen,
    OpcUa_Key*              a_privateKey,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pPlainTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_OAEP_Decrypt");

    uStatus = OpcUa_P_OpenSSL_RSA_Private_Decrypt(
                                                a_pProvider,
                                                a_pCipherText,
                                                a_cipherTextLen,
                                                a_privateKey,
                                                RSA_PKCS1_OAEP_PADDING,
                                                a_pPlainText,
                                                a_pPlainTextLen);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_OAEP_SHA256_Encrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_OAEP_SHA256_Encrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32            a_plainTextLen,
    OpcUa_Key*              a_publicKey,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32*           a_pCipherTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_OAEP_SHA256_Encrypt");

    uStatus = OpcUa_P_OpenSSL_RSA_SHA256_Public_Encrypt(
                                                a_pProvider,
                                                a_pPlainText,
                                                a_plainTextLen,
                                                a_publicKey,
                                                a_pCipherText,
                                                a_pCipherTextLen);
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_OAEP_Decrypt
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_OAEP_SHA256_Decrypt(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*             a_pCipherText,
    OpcUa_UInt32            a_cipherTextLen,
    OpcUa_Key*              a_privateKey,
    OpcUa_Byte*             a_pPlainText,
    OpcUa_UInt32*           a_pPlainTextLen)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_OAEP_SHA256_Decrypt");

    uStatus = OpcUa_P_OpenSSL_RSA_SHA256_Private_Decrypt(
                                                a_pProvider,
                                                a_pCipherText,
                                                a_cipherTextLen,
                                                a_privateKey,
                                                a_pPlainText,
                                                a_pPlainTextLen);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA1_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA1_Sign(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA1_Sign");

    uStatus = OpcUa_P_OpenSSL_HMAC_SHA1_Generate(
                                                a_pProvider,
                                                a_pData,
                                                a_dataLen,
                                                a_key,
                                                a_pSignature);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA1_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA1_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString mac = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA1_Verify");

    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);
    OpcUa_ReturnErrorIfArgumentNull(a_key);
    OpcUa_ReturnErrorIfArgumentNull(a_key->Key.Data);

    if(a_key->Key.Length < 1)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(a_pSignature->Length != 20)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if((OpcUa_Int32)a_dataLen < 1)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_HMAC_SHA1_Generate(
                                                a_pProvider,
                                                a_pData,
                                                a_dataLen,
                                                a_key,
                                                &mac);
    OpcUa_GotoErrorIfBad(uStatus);

    if(mac.Length > 0)
    {
        mac.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(mac.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(mac.Data);
    }
    else
    {
        uStatus = OpcUa_Bad;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_HMAC_SHA1_Generate(
                                                a_pProvider,
                                                a_pData,
                                                a_dataLen,
                                                a_key,
                                                &mac);
    OpcUa_GotoErrorIfBad(uStatus);

    if((OpcUa_MemCmp(mac.Data, a_pSignature->Data, mac.Length))==0)
    {
        uStatus = OpcUa_Good;
    }
    else
    {
        uStatus = OpcUa_BadSignatureInvalid;
    }

    if(mac.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(mac.Data);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(mac.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(mac.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA1_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA256_Sign(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA256_Sign");

    uStatus = OpcUa_P_OpenSSL_HMAC_SHA2_256_Generate(
                                                    a_pProvider,
                                                    a_pData,
                                                    a_dataLen,
                                                    a_key,
                                                    a_pSignature);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA256_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA256_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString mac = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA256_Verify");

    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);

    if(a_pSignature->Length != 32)
    {
        uStatus = OpcUa_BadInvalidArgument;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_HMAC_SHA2_256_Generate(
                                                    a_pProvider,
                                                    a_pData,
                                                    a_dataLen,
                                                    a_key,
                                                    &mac);
    OpcUa_GotoErrorIfBad(uStatus);

    if(mac.Length > 0)
    {
        mac.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(mac.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(mac.Data);
    }
    else
    {
        uStatus = OpcUa_Bad;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_HMAC_SHA2_256_Generate(
                                                    a_pProvider,
                                                    a_pData,
                                                    a_dataLen,
                                                    a_key,
                                                    &mac);
    OpcUa_GotoErrorIfBad(uStatus);

    if((OpcUa_MemCmp(mac.Data, a_pSignature->Data, mac.Length))==0)
    {
        uStatus = OpcUa_Good;
    }
    else
    {
        uStatus = OpcUa_BadSignatureInvalid;
    }

    if(mac.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(mac.Data);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(mac.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(mac.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA1_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA1_Sign(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_ByteString        a_data,
    OpcUa_Key*              a_privateKey,
    OpcUa_ByteString*       a_pSignature)
{
    OpcUa_ByteString messageDigest = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PKCS1_V15_SHA1_Sign");

    messageDigest.Length = 20; /* 160 bit */

    if(a_data.Data != OpcUa_Null)
    {
        messageDigest.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(messageDigest.Length);
        OpcUa_GotoErrorIfAllocFailed(messageDigest.Data);

        uStatus = OpcUa_P_OpenSSL_SHA1_Generate(    a_pProvider,
                                                    a_data.Data,
                                                    a_data.Length,
                                                    messageDigest.Data);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_RSA_Private_Sign( a_pProvider,
                                                messageDigest,
                                                a_privateKey,
                                                NID_sha1,
                                                a_pSignature);

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA1_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA1_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_publicKey,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString messageDigest = OPCUA_BYTESTRING_STATICINITIALIZER;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PKCS1_V15_SHA1_Verify");

    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);

    messageDigest.Length = 20; /* 160 bit */
    messageDigest.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(messageDigest.Length*sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(messageDigest.Data);

    uStatus = OpcUa_P_OpenSSL_SHA1_Generate(a_pProvider, a_data.Data, a_data.Length, messageDigest.Data);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_P_OpenSSL_RSA_Public_Verify(
                                                a_pProvider,
                                                messageDigest,
                                                a_publicKey,
                                                NID_sha1,
                                                a_pSignature);

    OpcUa_P_Memory_Free(messageDigest.Data);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA256_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA256_Sign(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_privateKey,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString messageDigest = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PKCS1_V15_SHA256_Sign");

    messageDigest.Length = 32; /* 256 bit */

    if(a_data.Data != OpcUa_Null)
    {
        messageDigest.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(messageDigest.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(messageDigest.Data);

        uStatus = OpcUa_P_OpenSSL_SHA2_256_Generate(a_pProvider, a_data.Data, a_data.Length, messageDigest.Data);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_RSA_Private_Sign(
                                                a_pProvider,
                                                messageDigest,
                                                a_privateKey,
                                                NID_sha256,
                                                a_pSignature);

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA256_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PKCS1_V15_SHA256_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_publicKey,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString messageDigest = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PKCS1_V15_SHA256_Verify");

    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);

    messageDigest.Length = 32; /* 256 bit */
    messageDigest.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(messageDigest.Length*sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(messageDigest.Data);

    uStatus = OpcUa_P_OpenSSL_SHA2_256_Generate(a_pProvider, a_data.Data, a_data.Length, messageDigest.Data);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_P_OpenSSL_RSA_Public_Verify(
                                                a_pProvider,
                                                messageDigest,
                                                a_publicKey,
                                                NID_sha256,
                                                a_pSignature);

    OpcUa_P_Memory_Free(messageDigest.Data);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PSS_SHA256_Sign
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PSS_SHA256_Sign(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_privateKey,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString messageDigest = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PSS_SHA256_Sign");

    messageDigest.Length = 32; /* 256 bit */

    if(a_data.Data != OpcUa_Null)
    {
        messageDigest.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(messageDigest.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(messageDigest.Data);

        uStatus = OpcUa_P_OpenSSL_SHA2_256_Generate(a_pProvider, a_data.Data, a_data.Length, messageDigest.Data);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    uStatus = OpcUa_P_OpenSSL_RSA_PSS_Private_Sign(
                                                a_pProvider,
                                                messageDigest,
                                                a_privateKey,
                                                a_pSignature);

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_RSA_PSS_SHA256_Verify
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_RSA_PSS_SHA256_Verify(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString      a_data,
    OpcUa_Key*            a_publicKey,
    OpcUa_ByteString*     a_pSignature)
{
    OpcUa_ByteString messageDigest = OPCUA_BYTESTRING_STATICINITIALIZER;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "RSA_PSS_SHA256_Verify");

    OpcUa_ReturnErrorIfArgumentNull(a_pSignature);

    messageDigest.Length = 32; /* 256 bit */
    messageDigest.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(messageDigest.Length*sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(messageDigest.Data);

    uStatus = OpcUa_P_OpenSSL_SHA2_256_Generate(a_pProvider, a_data.Data, a_data.Length, messageDigest.Data);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_P_OpenSSL_RSA_PSS_Public_Verify(
                                                a_pProvider,
                                                messageDigest,
                                                a_publicKey,
                                                a_pSignature);

    OpcUa_P_Memory_Free(messageDigest.Data);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(messageDigest.Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(messageDigest.Data);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_DeriveChannelKeyset
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_DeriveChannelKeyset(
    OpcUa_CryptoProvider*   a_pCryptoProvider,
    OpcUa_ByteString        a_remoteNonce,
    OpcUa_ByteString        a_localNonce,
    OpcUa_SecurityKeyset*   a_pKeyset)
{
    OpcUa_Key MasterKey;
    OpcUa_UInt32 uKeyDataSize = 0;
    OpcUa_Boolean bCalculateSizes = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "DeriveChannelKeyset");

    if(a_pKeyset->SigningKey.Key.Data == OpcUa_Null)
    {
        a_pKeyset->SigningKey.Key.Length = a_pCryptoProvider->DerivedSignatureKeyLength;
        bCalculateSizes = OpcUa_True;
    }

    uKeyDataSize += a_pKeyset->SigningKey.Key.Length;

    if(a_pKeyset->EncryptionKey.Key.Data == OpcUa_Null)
    {
        a_pKeyset->EncryptionKey.Key.Length = a_pCryptoProvider->DerivedEncryptionKeyLength;
        bCalculateSizes = OpcUa_True;
    }

    uKeyDataSize += a_pKeyset->EncryptionKey.Key.Length;

    if(a_pKeyset->InitializationVector.Key.Data == OpcUa_Null)
    {
        a_pKeyset->InitializationVector.Key.Length = 16;
        bCalculateSizes = OpcUa_True;
    }

    uKeyDataSize += a_pKeyset->InitializationVector.Key.Length;

    if(bCalculateSizes)
    {
        OpcUa_ReturnStatusCode;
    }

    /************************************************************************************/

    /* preinitialize */
    MasterKey.Type = 0;
    /*MasterKey.fpClearHandle = OpcUa_Null;*/
    MasterKey.Key.Length = -1;
    MasterKey.Key.Data = OpcUa_Null;

    /* check required nonce length */
    if(a_remoteNonce.Length < a_pCryptoProvider->SymmetricKeyLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNonceInvalid);
    }

    if(a_localNonce.Length < a_pCryptoProvider->SymmetricKeyLength)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNonceInvalid);
    }

    /* create the client master key */
    uStatus = a_pCryptoProvider->DeriveKey( a_pCryptoProvider,
                                            a_remoteNonce,
                                            a_localNonce,
                                            uKeyDataSize,
                                            &MasterKey);
    OpcUa_GotoErrorIfBad(uStatus);

    if(MasterKey.Key.Length <= 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadUnexpectedError);
    }

    /* MasterKey */
    MasterKey.Key.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(MasterKey.Key.Length*sizeof(OpcUa_Byte));
    OpcUa_GotoErrorIfAllocFailed(MasterKey.Key.Data);
    uStatus = a_pCryptoProvider->DeriveKey( a_pCryptoProvider,
                                            a_remoteNonce,
                                            a_localNonce,
                                            uKeyDataSize,
                                            &MasterKey);
    OpcUa_GotoErrorIfBad(uStatus);

    /* SigningKey */
    uStatus = OpcUa_P_Memory_MemCpy(a_pKeyset->SigningKey.Key.Data,
                                    a_pKeyset->SigningKey.Key.Length,
                                    MasterKey.Key.Data,
                                    a_pKeyset->SigningKey.Key.Length);
    OpcUa_GotoErrorIfBad(uStatus);

    /* EncryptingKey */
    uStatus = OpcUa_P_Memory_MemCpy(a_pKeyset->EncryptionKey.Key.Data,
                                    a_pKeyset->EncryptionKey.Key.Length,
                                    MasterKey.Key.Data + a_pKeyset->SigningKey.Key.Length,
                                    a_pKeyset->EncryptionKey.Key.Length);

    OpcUa_GotoErrorIfBad(uStatus);

    /* InitializationVector */
    OpcUa_P_Memory_MemCpy(  a_pKeyset->InitializationVector.Key.Data,
                            a_pKeyset->InitializationVector.Key.Length,
                            MasterKey.Key.Data + a_pKeyset->SigningKey.Key.Length + a_pKeyset->EncryptionKey.Key.Length,
                            a_pKeyset->InitializationVector.Key.Length);

    OpcUa_GotoErrorIfBad(uStatus);

    /* cleanup */
    OpcUa_P_OpenSSL_DestroySecretData(MasterKey.Key.Data, MasterKey.Key.Length);
    OpcUa_P_Key_Clear(&MasterKey);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_P_Key_Clear(&MasterKey);
    OpcUa_P_Key_Clear(&a_pKeyset->SigningKey);
    OpcUa_P_Key_Clear(&a_pKeyset->EncryptionKey);
    OpcUa_P_Key_Clear(&a_pKeyset->InitializationVector);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_DeriveChannelKeysets
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_DeriveChannelKeysets(
    OpcUa_CryptoProvider*   a_pCryptoProvider,
    OpcUa_ByteString        a_clientNonce,
    OpcUa_ByteString        a_serverNonce,
    OpcUa_Int32             a_keySize,
    OpcUa_SecurityKeyset*   a_pClientKeyset,
    OpcUa_SecurityKeyset*   a_pServerKeyset)
{
OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "DeriveChannelKeysets");

    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_clientNonce.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_serverNonce.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientKeyset);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerKeyset);

    OpcUa_ReferenceParameter(a_keySize);

    uStatus = OpcUa_P_OpenSSL_DeriveChannelKeyset(
        a_pCryptoProvider,
        a_serverNonce,
        a_clientNonce,
        a_pClientKeyset);

    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_P_OpenSSL_DeriveChannelKeyset(
        a_pCryptoProvider,
        a_clientNonce,
        a_serverNonce,
        a_pServerKeyset);

    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_GenerateAsymmetricKeyPair
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_GenerateAsymmetricKeyPair(
    OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_UInt              a_type,
    OpcUa_UInt32            a_bits,
    OpcUa_Key*              a_pPublicKey,
    OpcUa_Key*              a_pPrivateKey)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "GenerateAsymmetricKeyPair");

    if(a_type == OpcUa_Crypto_Rsa_Id)
    {
        uStatus = OpcUa_P_OpenSSL_RSA_GenerateKeys(a_pProvider, a_bits, a_pPublicKey, a_pPrivateKey);
    }
    else
    {
        uStatus = OpcUa_BadInvalidArgument;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_REQUIRE_OPENSSL */
