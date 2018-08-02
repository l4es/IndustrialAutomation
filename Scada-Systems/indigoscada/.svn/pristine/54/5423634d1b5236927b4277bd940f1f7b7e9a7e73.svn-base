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

#ifndef _OpcUa_P_PKI_H_
#define _OpcUa_P_PKI_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
  @brief The supported PKIs.
*/
typedef enum _OpcUa_P_PKI_Types
{
    OpcUa_Invalid_PKI   = 0,
    OpcUa_NO_PKI        = 1,
    OpcUa_Override      = 2,
    OpcUa_OpenSSL_PKI   = 3,
    OpcUa_Win32_PKI     = 4
} OpcUa_P_PKI_Types;

/* WIN32 PKI specific flag */
#define OPCUA_P_PKI_WIN32_STORE_USER     1
#define OPCUA_P_PKI_WIN32_STORE_MACHINE  2
#define OPCUA_P_PKI_WIN32_STORE_SERVICES 4
#define OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ONLY_LEAF               0x0200
#define OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ALL_EXCEPT_SELF_SIGNED  0x0400
#define OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ALL                     0x0600
#define OPCUA_P_PKI_WIN32_REQUIRE_CHAIN_CERTIFICATE_IN_TRUST_LIST  0x1000

/* OPENSSL PKI specific flags */
#define OPCUA_P_PKI_OPENSSL_USE_DEFAULT_CERT_CRL_LOOKUP_METHOD       0x0001
#define OPCUA_P_PKI_OPENSSL_DONT_ADD_TRUST_LIST_TO_ROOT_CERTIFICATES 0x0002
#define OPCUA_P_PKI_OPENSSL_ADD_UNTRUSTED_LIST_TO_ROOT_CERTIFICATES  0x0004
#define OPCUA_P_PKI_OPENSSL_UNTRUSTED_LIST_IS_INDEX                  0x0008
#define OPCUA_P_PKI_OPENSSL_REVOCATION_LIST_IS_INDEX                 0x0010
#define OPCUA_P_PKI_OPENSSL_REVOCATION_LIST_IS_CONCATENATED_PEM_FILE 0x0020
#define OPCUA_P_PKI_OPENSSL_SUPPRESS_CERT_VALIDITY_PERIOD_CHECK      0x0040
#define OPCUA_P_PKI_OPENSSL_SUPPRESS_CRL_VALIDITY_PERIOD_CHECK       0x0080
#define OPCUA_P_PKI_OPENSSL_SUPPRESS_CRL_NOT_FOUND_ERROR             0x0100
#define OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ONLY_LEAF               0x0200
#define OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL_EXCEPT_SELF_SIGNED  0x0400
#define OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL                     0x0600
#define OPCUA_P_PKI_OPENSSL_CHECK_SELF_SIGNED_SIGNATURE              0x0800
#define OPCUA_P_PKI_OPENSSL_REQUIRE_CHAIN_CERTIFICATE_IN_TRUST_LIST  0x1000
#define OPCUA_P_PKI_OPENSSL_ALLOW_PROXY_CERTIFICATES                 0x2000
#define OPCUA_P_PKI_OPENSSL_OVERRIDE_IS_DHPARAM_FILE                 0x4000

/* Override PKI specific flags */
#define OPCUA_P_PKI_OVERRIDE_HANDLE_IS_DHPARAM_FILE                  0x4000

/**
  @brief The openssl pki config.
  */
struct _OpcUa_P_OpenSSL_CertificateStore_Config
{
    /*! @brief used PKI type. */
    OpcUa_P_PKI_Types   PkiType;

    /*! @brief The trusted certificate store location. */
    OpcUa_StringA       CertificateTrustListLocation;

    /*! @brief The certificate revocation list. */
    OpcUa_StringA       CertificateRevocationListLocation;

    /*! @brief The untrusted certificate store location. */
    OpcUa_StringA       CertificateUntrustedListLocation;

    /*! @brief PKI-specific flags. */
    OpcUa_UInt32        Flags;

    /*! @brief External PKIProvider IF to override default implementation. Checked when Configuration name is "Override" */
    OpcUa_Void*         Override;
};
typedef struct _OpcUa_P_OpenSSL_CertificateStore_Config OpcUa_P_OpenSSL_CertificateStore_Config;

/** note on: .PkiType == OpcUa_OpenSSL_PKI:
 *
 * recommended flags:
 * .Flags = OPCUA_P_PKI_OPENSSL_ADD_UNTRUSTED_LIST_TO_ROOT_CERTIFICATES|
 *          OPCUA_P_PKI_OPENSSL_REQUIRE_CHAIN_CERTIFICATE_IN_TRUST_LIST|
 *          OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL;
 *
 * .CertificateTrustListLocation = "<trusted-certs-directory>";
 * .CertificateUntrustedListLocation = "<untrusted-certs-directory>";
 * .CertificateRevocationListLocation = "<revoked-certs-directory>";
 *
 * compatibility:
 * .Flags = 0;
 * .CertificateTrustListLocation = "<certs-directory>";
 */


/**
  @brief The certificate und key format enumeration.
*/
typedef enum _OpcUa_P_FileFormat
{
    OpcUa_Crypto_Encoding_Invalid   = 0,
    OpcUa_Crypto_Encoding_DER       = 1,
    OpcUa_Crypto_Encoding_PEM       = 2,
    OpcUa_Crypto_Encoding_PKCS12    = 3
}
OpcUa_P_FileFormat;

OPCUA_END_EXTERN_C

#endif
