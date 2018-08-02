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
#include <openssl/hmac.h>

/* own headers */
#include <opcua_p_openssl.h>

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA1_Generate
 *===========================================================================*/
/* HMAC-SHA-1: 160 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA1_Generate(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pMac)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA1_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_key);
    OpcUa_ReturnErrorIfArgumentNull(a_key->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pMac);

    if(a_pMac->Data == OpcUa_Null)
    {
        a_pMac->Length = 20;
        OpcUa_ReturnStatusCode;
    }

    if(HMAC(EVP_sha1(),a_key->Key.Data,a_key->Key.Length,a_pData,a_dataLen,a_pMac->Data,(unsigned int*)&(a_pMac->Length)) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA2_224_Generate
 *===========================================================================*/
/* HMAC-SHA-2: 224 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA2_224_Generate(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pMac)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA224_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_key);
    OpcUa_ReturnErrorIfArgumentNull(a_key->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pMac);

    if(a_pMac->Data == OpcUa_Null)
    {
        a_pMac->Length = 28;
        OpcUa_ReturnStatusCode;
    }

    if(HMAC(EVP_sha224(),a_key->Key.Data,a_key->Key.Length,a_pData,a_dataLen,a_pMac->Data,(unsigned int*)&(a_pMac->Length)) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA2_256_Generate
 *===========================================================================*/
/* HMAC-SHA-2: 256 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA2_256_Generate(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pMac)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA256_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_key);
    OpcUa_ReturnErrorIfArgumentNull(a_key->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pMac);

    if(a_pMac->Data == OpcUa_Null)
    {
        a_pMac->Length = 32;
        OpcUa_ReturnStatusCode;
    }

    if(HMAC(EVP_sha256(),a_key->Key.Data,a_key->Key.Length,a_pData,a_dataLen,a_pMac->Data,(unsigned int*)&(a_pMac->Length)) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA2_384_Generate
 *===========================================================================*/
/* HMAC-SHA-2: 384 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA2_384_Generate(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pMac)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA384_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_key);
    OpcUa_ReturnErrorIfArgumentNull(a_key->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pMac);

    if(a_pMac->Data == OpcUa_Null)
    {
        a_pMac->Length = 48;
        OpcUa_ReturnStatusCode;
    }

    if(HMAC(EVP_sha384(),a_key->Key.Data,a_key->Key.Length,a_pData,a_dataLen,a_pMac->Data,(unsigned int*)&(a_pMac->Length)) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_HMAC_SHA2_512_Generate
 *===========================================================================*/
/* HMAC-SHA-2: 512 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_HMAC_SHA2_512_Generate(
    OpcUa_CryptoProvider* a_pProvider,
    OpcUa_Byte*           a_pData,
    OpcUa_UInt32          a_dataLen,
    OpcUa_Key*            a_key,
    OpcUa_ByteString*     a_pMac)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "HMAC_SHA512_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_key);
    OpcUa_ReturnErrorIfArgumentNull(a_key->Key.Data);
    OpcUa_ReturnErrorIfArgumentNull(a_pMac);

    if(a_pMac->Data == OpcUa_Null)
    {
        a_pMac->Length = 64;
        OpcUa_ReturnStatusCode;
    }

    if(HMAC(EVP_sha512(),a_key->Key.Data,a_key->Key.Length,a_pData,a_dataLen,a_pMac->Data,(unsigned int*)&(a_pMac->Length)) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_REQUIRE_OPENSSL */
