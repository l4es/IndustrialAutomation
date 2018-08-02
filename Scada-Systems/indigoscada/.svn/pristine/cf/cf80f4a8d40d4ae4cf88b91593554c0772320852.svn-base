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

#ifndef _OpcUa_P_Crypto_H_
#define _OpcUa_P_Crypto_H_ 1

OPCUA_BEGIN_EXTERN_C

/* MESSAGE DIGEST */
#define OPCUA_P_SHA_160 160
#define OPCUA_P_SHA_224 224
#define OPCUA_P_SHA_256 256
#define OPCUA_P_SHA_384 384
#define OPCUA_P_SHA_512 512

/* SecurityPolicies */
#define OpcUa_SecurityPolicy_None                   "http://opcfoundation.org/UA/SecurityPolicy#None"
#define OpcUa_SecurityPolicy_Basic128Rsa15          "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"
#define OpcUa_SecurityPolicy_Basic256               "http://opcfoundation.org/UA/SecurityPolicy#Basic256"
#define OpcUa_SecurityPolicy_Basic256Sha256         "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256"
#define OpcUa_SecurityPolicy_Aes128Sha256RsaOaep    "http://opcfoundation.org/UA/SecurityPolicy#Aes128_Sha256_RsaOaep"
#define OpcUa_SecurityPolicy_Aes256Sha256RsaPss     "http://opcfoundation.org/UA/SecurityPolicy#Aes256_Sha256_RsaPss"

/* Signature Algorithm Uris */
#define OpcUa_AlgorithmUri_Signature_RsaSha1        "http://www.w3.org/2000/09/xmldsig#rsa-sha1"
#define OpcUa_AlgorithmUri_Signature_RsaSha256      "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256"
#define OpcUa_AlgorithmUri_Signature_HmacSha1       "http://www.w3.org/2000/09/xmldsig#hmac-sha1"
#define OpcUa_AlgorithmUri_Signature_HmacSha256     "http://www.w3.org/2001/04/xmldsig-more#hmac-sha256"
#define OpcUa_AlgorithmUri_Signature_RsaPssSha256   "http://opcfoundation.org/UA/security/rsa-pss-sha2-256"

/* Encryption Algorithms Uris */
#define OpcUa_AlgorithmUri_Ecryption_Aes128Cbc      "http://www.w3.org/2001/04/xmlenc#aes128-cbc"
#define OpcUa_AlgorithmUri_Ecryption_Aes256Cbc      "http://www.w3.org/2001/04/xmlenc#aes256-cbc"
#define OpcUa_AlgorithmUri_Encryption_RsaOaep       "http://www.w3.org/2001/04/xmlenc#rsa-oaep"
#define OpcUa_AlgorithmUri_Encryption_Rsa15         "http://www.w3.org/2001/04/xmlenc#rsa-1_5"
#define OpcUa_AlgorithmUri_Encryption_RsaOaepSha256 "http://opcfoundation.org/UA/security/rsa-oaep-sha2-256"

/* Encryption Algs */
#define OpcUa_P_NoEncryption_Name                   ""
#define OpcUa_P_NoEncryption_Id                     0

#define OpcUa_P_AES_128_CBC_Name                    "AES-128-CBC"
#define OpcUa_P_AES_128_CBC_Id                      1

#define OpcUa_P_AES_256_CBC_Name                    "AES-256-CBC"
#define OpcUa_P_AES_256_CBC_Id                      2

#define OpcUa_P_RSA_PKCS1_V15_Name                  "RSA-PKCS-#1-V1.5"
#define OpcUa_P_RSA_PKCS1_V15_Id                    3

#define OpcUa_P_RSA_OAEP_Name                       "RSA-OAEP"
#define OpcUa_P_RSA_OAEP_Id                         4

#define OpcUa_P_RSA_OAEP_SHA256_Name                "RSA-OAEP-SHA256"
#define OpcUa_P_RSA_OAEP_SHA256_Id                  5

/* Signature Algs */
#define OpcUa_P_NoSignature_Name                    ""
#define OpcUa_P_NoSignature_Id                      0

#define OpcUa_P_RSA_PKCS1_V15_SHA1_Name             "RSA-PKCS-#1-V1.5-SHA1"
#define OpcUa_P_RSA_PKCS1_V15_SHA1_Id               6

#define OpcUa_P_RSA_PKCS1_V15_SHA256_Name           "RSA-PKCS-#1-V1.5-SHA256"
#define OpcUa_P_RSA_PKCS1_V15_SHA256_Id             7

#define OpcUa_P_HMAC_SHA1_Name                      "HMAC-SHA1"
#define OpcUa_P_HMAC_SHA1_Id                        8

#define OpcUa_P_HMAC_SHA256_Name                    "HMAC-SHA256"
#define OpcUa_P_HMAC_SHA256_Id                      9

#define OpcUa_P_RSA_PSS_SHA256_Name                 "RSA-PSS-SHA256"
#define OpcUa_P_RSA_PSS_SHA256_Id                   10

/* Key Derivation Algs */
#define OpcUa_P_PSHA1_Name                          "P-SHA1"
#define OpcUa_P_PSHA1_Id                            12

#define OpcUa_P_PSHA256_Name                        "P-SHA256"
#define OpcUa_P_PSHA256_Id                          13

OPCUA_END_EXTERN_C

#endif
