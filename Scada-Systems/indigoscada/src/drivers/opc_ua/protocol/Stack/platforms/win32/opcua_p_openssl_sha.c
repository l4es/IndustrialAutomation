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
#include <openssl/sha.h>

/* own headers */
#include <opcua_p_openssl.h>

/*============================================================================
 * OpcUa_P_OpenSSL_SHA1_Generate
 *===========================================================================*/
/* SHA-1: 160 Bit output */
OpcUa_StatusCode OpcUa_P_OpenSSL_SHA1_Generate(
    OpcUa_CryptoProvider*         a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Byte*                   a_pMessageDigest)
{
OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "SHA1_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageDigest);

    if(SHA1(a_pData, a_dataLen, a_pMessageDigest) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_SHA2_224_Generate
 *===========================================================================*/
/* SHA-2: 224 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_SHA2_224_Generate(
    OpcUa_CryptoProvider*         a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Byte*                   a_pMessageDigest)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "SHA2_224_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageDigest);

    if(SHA224(a_pData, a_dataLen, a_pMessageDigest) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_SHA2_256_Generate
 *===========================================================================*/
/* SHA-2: 256 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_SHA2_256_Generate(
    OpcUa_CryptoProvider*         a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Byte*                   a_pMessageDigest)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "SHA2_256_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageDigest);

    if(SHA256(a_pData, a_dataLen, a_pMessageDigest) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_SHA2_384_Generate
 *===========================================================================*/
/* SHA-2: 384 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_SHA2_384_Generate(
    OpcUa_CryptoProvider*         a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Byte*                   a_pMessageDigest)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "SHA2_384_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageDigest);

    if(SHA384(a_pData, a_dataLen, a_pMessageDigest) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * OpcUa_P_OpenSSL_SHA2_512_Generate
 *===========================================================================*/
/* SHA-2: 512 Bits output */
OpcUa_StatusCode OpcUa_P_OpenSSL_SHA2_512_Generate(
    OpcUa_CryptoProvider*         a_pProvider,
    OpcUa_Byte*                   a_pData,
    OpcUa_UInt32                  a_dataLen,
    OpcUa_Byte*                   a_pMessageDigest)
{
    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "SHA2_512_Generate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pData);
    OpcUa_ReturnErrorIfArgumentNull(a_pMessageDigest);

    if(SHA512(a_pData, a_dataLen, a_pMessageDigest) == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

OpcUa_ReturnStatusCode;

OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_REQUIRE_OPENSSL */
