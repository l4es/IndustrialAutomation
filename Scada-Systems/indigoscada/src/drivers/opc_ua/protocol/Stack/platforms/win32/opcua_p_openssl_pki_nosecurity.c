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

/* own headers */
#include <opcua_p_openssl_pki.h>

/*============================================================================
 * OpcUa_P_OpenSSL_CertificateStore_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_OpenCertificateStore(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void**                a_ppCertificateStore)
{
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_ppCertificateStore);

    return OpcUa_BadNotSupported;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_NoSecurity_CloseCertificateStore
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_CloseCertificateStore(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void**                a_ppCertificateStore)
{
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_ppCertificateStore);

    return OpcUa_BadNotSupported;
}


/*============================================================================
 * OpcUa_P_OpenSSL_PKI_ValidateCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_ValidateCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Int*                  a_pValidationCode)
{
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_pCertificateStore);
    OpcUa_ReferenceParameter(a_pCertificate);
    OpcUa_ReferenceParameter(a_pValidationCode);

    return OpcUa_BadNotSupported;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_NoSecurity_SaveCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_SaveCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Void*                 a_pSaveHandle)
{
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_pCertificateStore);
    OpcUa_ReferenceParameter(a_pCertificate);
    OpcUa_ReferenceParameter(a_pSaveHandle);

    return OpcUa_BadNotSupported;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_NoSecurity_LoadCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_LoadCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void*                 a_pLoadHandle,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_ByteString*           a_pCertificate)
{
    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_pCertificateStore);
    OpcUa_ReferenceParameter(a_pCertificate);
    OpcUa_ReferenceParameter(a_pLoadHandle);

    return OpcUa_BadNotSupported;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_NoSecurity_LoadCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_LoadPrivateKeyFromFile(
    OpcUa_StringA               a_privateKeyFile,
    OpcUa_P_FileFormat          a_fileFormat,
    OpcUa_StringA               a_password,
    OpcUa_UInt                  a_keyType,
    OpcUa_Key*                  a_pPrivateKey)
{
    OpcUa_ReferenceParameter(a_privateKeyFile);
    OpcUa_ReferenceParameter(a_fileFormat);
    OpcUa_ReferenceParameter(a_password);
    OpcUa_ReferenceParameter(a_keyType);
    OpcUa_ReferenceParameter(a_pPrivateKey);

    return OpcUa_BadNotSupported;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_NoSecurity_ExtractCertificateData
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_NoSecurity_ExtractCertificateData(
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_ByteString*           a_pIssuer,
    OpcUa_ByteString*           a_pSubject,
    OpcUa_ByteString*           a_pSubjectUri,
    OpcUa_ByteString*           a_pSubjectIP,
    OpcUa_ByteString*           a_pSubjectDNS,
    OpcUa_ByteString*           a_pCertThumbprint,
    OpcUa_UInt32*               a_pSubjectHash,
    OpcUa_UInt32*               a_pCertRawLength)
{
    OpcUa_ReferenceParameter(a_pCertificate);
    OpcUa_ReferenceParameter(a_pIssuer);
    OpcUa_ReferenceParameter(a_pSubject);
    OpcUa_ReferenceParameter(a_pSubjectUri);
    OpcUa_ReferenceParameter(a_pSubjectIP);
    OpcUa_ReferenceParameter(a_pSubjectDNS);
    OpcUa_ReferenceParameter(a_pCertThumbprint);
    OpcUa_ReferenceParameter(a_pSubjectHash);
    OpcUa_ReferenceParameter(a_pCertRawLength);

    return OpcUa_BadNotSupported;
}
