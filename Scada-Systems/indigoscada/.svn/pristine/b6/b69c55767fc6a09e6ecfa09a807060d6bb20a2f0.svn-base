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

/* stackcore */
#include <opcua_securechannel.h>

/* own */
#include <opcua_crypto.h>

/*============================================================================
 * OpcUa_Crypto_GenerateAsymmetricKeypair
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Crypto_GenerateAsymmetricKeypair(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_UInt                      a_type,
    OpcUa_UInt32                    a_bits,
    OpcUa_Key*                      a_pPublicKey,
    OpcUa_Key*                      a_pPrivateKey)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->GenerateAsymmetricKeypair, OpcUa_BadNotSupported);

    return a_pProvider->GenerateAsymmetricKeypair(a_pProvider, a_type, a_bits, a_pPublicKey, a_pPrivateKey);
}

/*============================================================================
 * OpcUa_Crypto_GetAsymmetricKeyLength
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Crypto_GetAsymmetricKeyLength(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Key                       a_publicKey,
    OpcUa_UInt32*                   a_pBits)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->GetAsymmetricKeyLength, OpcUa_BadNotSupported);

    return a_pProvider->GetAsymmetricKeyLength(a_pProvider, a_publicKey, a_pBits);
}

/*============================================================================
 * OpcUa_Crypto_GenerateKey
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Crypto_GenerateKey(
    struct _OpcUa_CryptoProvider*    a_pProvider,
    OpcUa_Int32                      a_keyLen,
    OpcUa_Key*                       a_pKey)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->GenerateKey, OpcUa_BadNotSupported);

    return a_pProvider->GenerateKey(a_pProvider, a_keyLen, a_pKey);
}

/*============================================================================
 * OpcUa_Crypto_DeriveKey
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Crypto_DeriveKey(
    struct _OpcUa_CryptoProvider* a_pProvider,
    OpcUa_ByteString              a_secret, /* clientnonce | servernonce, servernonce | clientnonce */
    OpcUa_ByteString              a_seed,
    OpcUa_Int32                   a_keyLen, /* output len */
    OpcUa_Key*                    a_pKey)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->DeriveKey, OpcUa_BadNotSupported);

    return a_pProvider->DeriveKey(a_pProvider, a_secret, a_seed, a_keyLen, a_pKey);
}
/*============================================================================
 * OpcUa_Crypto_DeriveChannelKeysets
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Crypto_DeriveChannelKeysets(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_ByteString                a_clientNonce,
    OpcUa_ByteString                a_serverNonce,
    OpcUa_Int32                     a_keySize,
    OpcUa_SecurityKeyset*           a_pClientKeyset,
    OpcUa_SecurityKeyset*           a_pServerKeyset)
{
    return a_pProvider->DeriveChannelKeysets(
                                            a_pProvider,
                                            a_clientNonce,
                                            a_serverNonce,
                                            a_keySize,
                                            a_pClientKeyset,
                                            a_pServerKeyset);
}

/*============================================================================
 * OpcUa_Crypto_GetPublicKeyFromCert
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Crypto_GetPublicKeyFromCert(
    struct _OpcUa_CryptoProvider*       a_pProvider,
    OpcUa_ByteString*                   a_pCertificate,
    OpcUa_StringA                       a_password,             /* this could be optional */
    OpcUa_Key*                          a_pPublicKey)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->GetPublicKeyFromCert, OpcUa_BadNotSupported);

    return a_pProvider->GetPublicKeyFromCert(a_pProvider, a_pCertificate, a_password, a_pPublicKey);
}

/*============================================================================
 * OpcUa_Crypto_GetSignatureFromCert
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Crypto_GetSignatureFromCert(
    struct _OpcUa_CryptoProvider*       a_pProvider,
    OpcUa_ByteString*                   a_pCertificate,
    OpcUa_Signature*                    a_pSignature)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->GetSignatureFromCert, OpcUa_BadNotSupported);

    return a_pProvider->GetSignatureFromCert(a_pProvider, a_pCertificate, a_pSignature);
}

/*============================================================================
 * OpcUa_Crypto_GetCertificateThumbprint
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Crypto_GetCertificateThumbprint(
    struct _OpcUa_CryptoProvider*       a_pProvider,
    OpcUa_ByteString*                   a_pCertificate,
    OpcUa_ByteString*                   a_pCertificateThumbprint)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->GetCertificateThumbprint, OpcUa_BadNotSupported);

    return a_pProvider->GetCertificateThumbprint(a_pProvider, a_pCertificate, a_pCertificateThumbprint);
}

/*============================================================================
 * OpcUa_Crypto_CreateCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Crypto_CreateCertificate(
    struct _OpcUa_CryptoProvider*       a_pProvider,
    OpcUa_Int32                         a_serialNumber,
    OpcUa_UInt32                        a_validToInSec,
    OpcUa_Crypto_NameEntry*             a_pNameEntries,      /* will be used for issuer and subject thus it's selfigned cert */
    OpcUa_UInt                          a_nameEntriesCount,  /* will be used for issuer and subject thus it's selfigned cert */
    OpcUa_Key                           a_pSubjectPublicKey, /* EVP_PKEY* - type defines also public key algorithm */
    OpcUa_Crypto_Extension*             a_pExtensions,
    OpcUa_UInt                          a_extensionsCount,
    OpcUa_UInt                          a_signatureHashAlgorithm, /* EVP_sha1(),... */
    OpcUa_Key                           a_pIssuerPrivateKey, /* EVP_PKEY* - type defines also signature algorithm */
    OpcUa_ByteString*                   a_pCertificate)      /* DER encoded byte string */
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->CreateCertificate, OpcUa_BadNotSupported);

    return a_pProvider->CreateCertificate(a_pProvider,
                                          a_serialNumber,
                                          a_validToInSec,
                                          a_pNameEntries,
                                          a_nameEntriesCount,
                                          a_pSubjectPublicKey,
                                          a_pExtensions,
                                          a_extensionsCount,
                                          a_signatureHashAlgorithm,
                                          a_pIssuerPrivateKey,
                                          a_pCertificate);
}


/*============================================================================
 * OpcUa_Crypto_AsymmetricEncrypt
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Crypto_AsymmetricEncrypt(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*                     a_pPlainText,
    OpcUa_UInt32                    a_plainTextLen,
    OpcUa_Key*                      a_publicKey,
    OpcUa_Byte*                     a_pCipherText,
    OpcUa_UInt32*                   a_pCipherTextLen)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->AsymmetricEncrypt, OpcUa_BadNotSupported);

    return a_pProvider->AsymmetricEncrypt(
        a_pProvider,
        a_pPlainText,
        a_plainTextLen,
        a_publicKey,
        a_pCipherText,
        a_pCipherTextLen);
}

/*============================================================================
 * OpcUa_Crypto_AsymmetricDecrypt
 *============================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Crypto_AsymmetricDecrypt(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*                     a_pCipherText,
    OpcUa_UInt32                    a_cipherTextLen,
    OpcUa_Key*                      a_privateKey,
    OpcUa_Byte*                     a_pPlainText,
    OpcUa_UInt32*                   a_pPlainTextLen)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->AsymmetricDecrypt, OpcUa_BadNotSupported);

    return a_pProvider->AsymmetricDecrypt(
        a_pProvider,
        a_pCipherText,
        a_cipherTextLen,
        a_privateKey,
        a_pPlainText,
        a_pPlainTextLen);
}

/*============================================================================
 * OpcUa_Crypto_AsymmetricSign
 *============================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Crypto_AsymmetricSign(
    struct _OpcUa_CryptoProvider*    a_pProvider,
    OpcUa_ByteString                 a_data,
    OpcUa_Key*                       a_privateKey,
    OpcUa_ByteString*                a_pSignature)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->AsymmetricSign, OpcUa_BadNotSupported);

    return a_pProvider->AsymmetricSign(
                                        a_pProvider,
                                        a_data,
                                        a_privateKey,
                                        a_pSignature);
}

/*============================================================================
 * OpcUa_Crypto_AsymmetricVerify
 *============================================================================*/
OpcUa_StatusCode OpcUa_Crypto_AsymmetricVerify(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_ByteString                a_data,
    OpcUa_Key*                      a_publicKey,
    OpcUa_ByteString*               a_pSignature)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->AsymmetricVerify, OpcUa_BadNotSupported);

    return a_pProvider->AsymmetricVerify(
                                            a_pProvider,
                                            a_data,
                                            a_publicKey,
                                            a_pSignature);
}

/*============================================================================
 * OpcUa_Crypto_SymmetricEncrypt
 *============================================================================*/
OpcUa_StatusCode OpcUa_Crypto_SymmetricEncrypt(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*                     a_pPlainText,
    OpcUa_UInt32                    a_plainTextLen,
    OpcUa_Key*                      a_key,
    OpcUa_Byte*                     a_pInitalVector,
    OpcUa_Byte*                     a_pCipherText,
    OpcUa_UInt32*                   a_pCipherTextLen)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->SymmetricEncrypt, OpcUa_BadNotSupported);

    return a_pProvider->SymmetricEncrypt(a_pProvider,
                                        a_pPlainText,
                                        a_plainTextLen,
                                        a_key,
                                        a_pInitalVector,
                                        a_pCipherText,
                                        a_pCipherTextLen);
}

/*============================================================================
 * OpcUa_Crypto_SymmetricDecrypt
 *============================================================================*/
OpcUa_StatusCode OpcUa_Crypto_SymmetricDecrypt(
    struct _OpcUa_CryptoProvider*   a_pProvider,
    OpcUa_Byte*                     a_pCipherText,
    OpcUa_UInt32                    a_cipherTextLen,
    OpcUa_Key*                      a_key,
    OpcUa_Byte*                     a_pInitalVector,
    OpcUa_Byte*                     a_pPlainText,
    OpcUa_UInt32*                   a_pPlainTextLen)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->SymmetricDecrypt, OpcUa_BadNotSupported);

    return a_pProvider->SymmetricDecrypt(a_pProvider,
                                         a_pCipherText,
                                         a_cipherTextLen,
                                         a_key,
                                         a_pInitalVector,
                                         a_pPlainText,
                                         a_pPlainTextLen);
}

/*============================================================================
 * OpcUa_Crypto_SymmetricSign
 *============================================================================*/
OpcUa_StatusCode OpcUa_Crypto_SymmetricSign(
    struct _OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Key*                    a_key,
    OpcUa_ByteString*             a_pSignature)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->SymmetricSign, OpcUa_BadNotSupported);

    return a_pProvider->SymmetricSign(a_pProvider, a_pData, a_dataLen, a_key, a_pSignature);
}

/*============================================================================
 * OpcUa_Crypto_SymmetricVerify
 *============================================================================*/
OpcUa_StatusCode OpcUa_Crypto_SymmetricVerify(
    struct _OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Key*                    a_key,
    OpcUa_ByteString*             a_pSignature)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Crypto);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfNull(a_pProvider->SymmetricVerify, OpcUa_BadNotSupported);

    return a_pProvider->SymmetricVerify(
        a_pProvider,
        a_pData,
        a_dataLen,
        a_key,
        a_pSignature);
}

/*============================================================================
 * OpcUa_Key_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_Key_Initialize(OpcUa_Key* a_pKey)
{
    OpcUa_ByteString_Initialize(&a_pKey->Key);
    a_pKey->Type = 0;
    a_pKey->fpClearHandle = OpcUa_Null;
}

/*============================================================================
 * OpcUa_Key_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_Key_Clear(OpcUa_Key* a_pKey)
{
    if(a_pKey != OpcUa_Null)
    {
        if(OPCUA_CRYPTO_KEY_ISNOHANDLE(a_pKey))
        {
            if(a_pKey->Key.Data != OpcUa_Null && a_pKey->Key.Length > 0)
            {
                OpcUa_DestroySecretData(a_pKey->Key.Data, a_pKey->Key.Length);
            }
            OpcUa_ByteString_Clear(&a_pKey->Key);
        }
        else
        {
            if(a_pKey->fpClearHandle != OpcUa_Null)
            {
                a_pKey->fpClearHandle(a_pKey);
            }
            else
            {
                a_pKey->Key.Data    = OpcUa_Null;
            }

            a_pKey->Key.Length  = -1;
        }
        a_pKey->Type = OpcUa_Crypto_KeyType_Invalid;
    }
}
/*============================================================================
 * OpcUa_Signature_Initialize
 *===========================================================================*/
OPCUA_EXPORT OpcUa_Void OpcUa_Signature_Initialize(OpcUa_Signature* a_pSignature)
{
    OpcUa_ByteString_Initialize(&a_pSignature->Signature);
    a_pSignature->Algorithm = 0;
}

/*============================================================================
 * OpcUa_Signature_Clear
 *===========================================================================*/
OPCUA_EXPORT OpcUa_Void OpcUa_Signature_Clear(OpcUa_Signature* a_pSignature)
{
    if(a_pSignature != OpcUa_Null)
    {
        OpcUa_ByteString_Clear(&a_pSignature->Signature);
        a_pSignature->Algorithm = 0;
    }
}

/*============================================================================
 * OpcUa_SecurityKeyset_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_SecurityKeyset_Initialize(OpcUa_SecurityKeyset* pSecurityKeyset)
{
    OpcUa_Key_Initialize(&pSecurityKeyset->EncryptionKey);
    OpcUa_Key_Initialize(&pSecurityKeyset->InitializationVector);
    OpcUa_Key_Initialize(&pSecurityKeyset->SigningKey);
}

/*============================================================================
 * OpcUa_SecurityKeyset_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_SecurityKeyset_Clear(OpcUa_SecurityKeyset* a_pSecurityKeyset)
{
    if(a_pSecurityKeyset != OpcUa_Null)
    {
        OpcUa_Key_Clear(&a_pSecurityKeyset->SigningKey);
        OpcUa_Key_Clear(&a_pSecurityKeyset->EncryptionKey);
        OpcUa_Key_Clear(&a_pSecurityKeyset->InitializationVector);
    }
}
