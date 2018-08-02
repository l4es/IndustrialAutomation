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

#ifndef _OpcUa_P_Win32_PKI_H_
#define _OpcUa_P_Win32_PKI_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
  @brief Creates a certificate store handle using the CERT_STORE_PROV_SYSTEM certificate store provider of the Windows Crypto API and opens
         the store.

  @param pProvider                  [in]  The PKIProvider handle.

                                          The provider contains a handle to the PKI configuration (OpcUa_P_OpenSSL_CertificateStore_Config).

                                          Depending on the PKIFlags set for the PKIProvider it uses the certificate store of the
                                          current user (CERT_SYSTEM_STORE_CURRENT_USER), the machine-wide store (CERT_SYSTEM_STORE_LOCAL_MACHINE)
                                          or the store associated to the running services (CERT_SYSTEM_STORE_SERVICES).

                                          In addition the CertificateTrustListLocation of OpcUa_P_OpenSSL_CertificateStore_Config is used to
                                          specify the folder (i.e. MMC Certificate Store Name) within the store that is opened which can be
                                          have the following values:

                                            - "My" (i.e. Personal folder; Windows 2000 or later),
                                            - "CA" (i.e. Trusted Root Certification Authorities folder, Windows 2000 or later),
                                            - "Trust" (i.e. Enterprise Trust folder, Windows 2000 or later),
                                            - "UserDS" (i.e. ActiveDirectory User Object folder, Windows 2000 or later),
                                            - "TrustedPublisher" (i.e. Trusted Publishers folder, Windows XP or later),
                                            - "Disallowed" (i.e. Un-trusted Certificates; Windows XP or later),
                                            - "AuthRoot" (i.e. Third-Party Root Certification Authorities; Windows XP or later) or
                                            - "TrustedPeople" (i.e. Trusted People; Windows XP or later).

                                          Note, that this does not affect validation of certificates since for that purpose all folders are
                                          taken into account. This is only used for loading certificates and their associated private keys as
                                          well as for specifying the location to which certificates are stored.

                                          Both pProvider and a_pProvider->Handle must not be OpcUa_Null!

  @param ppCertificateStore         [out] The handle to the certificate store.
*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_OpenCertificateStore(
    OpcUa_PKIProvider*          pProvider,
    OpcUa_Void**                ppCertificateStore);

/**
  @brief Closes the specified certificate store and frees the certificate store object.

  @param pProvider             [in] The PKIProvider handle.

                                    pProvider can be OpcUa_Null!

  @param ppCertificateStore    [in] The certificate store object.

                                    Both ppCertificateStore and *ppCertificateStore can be OpcUa_Null!
*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_CloseCertificateStore(
    OpcUa_PKIProvider*       pProvider,
    OpcUa_Void**             ppCertificateStore);

/**
  @brief Validates a X509 certificate provided in DER encoded string of bytes.

         Thereby chain validation mechanism is used why tries to build a certificate chain based on the provided
         certificate up to the root CA. For each certificate in the chain it is checked whether it is
            - well-formed,
            - has a valid signature,
            - has correct validity period and
            - is not revoked (meaning that it does not appear in the list of untrusted certificates in the certificate store).

  @param pProvider                [in]  The PKIProvider handle.

                                        pProvider can be OpcUa_Null!

  @param pCertificate             [in]  The certificate that should be validated as DER encoded string of bytes.

                                        pCertificate must not be OpcUa_Null!

  @param pCertificateStore        [in]  The certificate store that validates the passed in certificate.

                                        pCertificateStore must not be OpcUa_Null!

  @param pValidationCode          [out] The validation code, that gives information about the validation result.
                                        Therevy the trust status of the certificate chain is used.
*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_ValidateCertificate(
    OpcUa_PKIProvider*          pProvider,
    OpcUa_ByteString*           pCertificate,
    OpcUa_Void*                 pCertificateStore,
    OpcUa_Int*                  pValidationCode);

/**
  @brief Saves a given DER encoded certificate to specified certificate store.

         The location to which the certificate is stored is specified in pCertificateStore handle obtained
         by the OpcUa_P_Win32_PKI_OpenCertificateStore function.

         TODO: Currently matching certificates will be overwritten. However, there's the possibility to configure
               that behaviour. This could be an additional parameter in the function. This has to be discussed!

  @param pProvider                [in]  The PKIProvider handle.

                                        PKIProvider and a_pProvider->Handle must not be OpcUa_Null!

  @param pCertificateStore        [in]  The handle to the certificate store that should store the passed in certificate.

                                        a_pCertificateStore must not be OpcUa_Null;

  @param pCertificate             [in]  The certificate that should be stored in the certificate store.

                                        a_pCertificate and a_pCertificate->Data must not be OpcUa_Null!
                                        a_pCertificate->must be the size of the byte stored in a_pCertificate->Data!

  @param pSaveHandle              [in]  Currently not used!

                                        pSaveHandle can be OpcUa_Null;
*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_SaveCertificate(
    OpcUa_PKIProvider*          pProvider,
    OpcUa_ByteString*           pCertificate,
    OpcUa_Void*                 pCertificateStore,
    OpcUa_Void*                 pSaveHandle);

/**
  @brief Loads a certain certificate from a given certificate store. The exported certificate will be provided as a
         DER encoded string of bytes.

         The CertificateTrustListLocation of OpcUa_P_OpenSSL_CertificateStore_Config used for creating the CertificateStore handle
         defines the folder in the certificate store that contains the certificates that can be loaded. Typically the "My" folder
         contains end certificates loaded by applications.

         TODO: Add functionality opening a certificate store containing the certificates of all folders. This simplifies
               loading a certificate since the user does not have to know the exact folder in which the desired certificate
               is located.

  @param pProvider                [in]  The PKIProvider handle.

                                        pProvider can be OpcUa_Null! (Currently not used!)

  @param pLoadHandle              [in]  The handle containing the subject name of the certificate to be loaded.

                                        pLoadHandle must not be OpcUa_Null!

  @param pCertificateStore        [in]  The certificate store that contains the desired certificate.

                                        pCertificateStore must not be OpcUa_Null!

  @param pCertificate             [out] The desired certificate in DER encoded string of bytes.
*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_LoadCertificate(
    OpcUa_PKIProvider*          pProvider,
    OpcUa_Void*                 pLoadHandle,
    OpcUa_Void*                 pCertificateStore,
    OpcUa_ByteString*           pCertificate);

/**
  @brief Loads the private key of a certain certificate from the certificat store. The exported key will be provided as a
         DER encoded string of bytes.

        TODO: Currently only private keys from "My" folder of the system store can be loaded since this function does not have
              a parameter for a certificate store handle. Therefore a store with a certain configuration has to be opened within
              the function. This should be discussed and changed!

        TODO: Currently the private keys are exported to a unsecured string of DER encoded bytes and are loaded into the memory.
              There's the risk that other malicious applications could copy it. However, this requires the malicious appliction
              to exactly know where the key, what algorithm was used to create the key and how long it is. But if a malicious application
              manages to access the memory then a fundamental security problem is the whole system exists anyway.
              Nevertheless a mitigation strategy addressing that risk should be provided. One approach that reduces the risk is to not to
              load the key at the initialization phase of the application but only for a small duration when it is used i.e. for opening
              and renewing secure channels. Instead of providing the bytestring of the private key only a handle is provided. This has to
              be discussed.

  @param privateKeyFile           [in]  The subject name of the associated certificate from which the private key should be
                                        loaded.

                                        privateKeyFile must not be OpcUa_Null!

  @param fileFormat               [in]  Currently not used! Always DER encoding used!

                                        fileFormat can be OpcUa_Null!

  @param password                 [in]  Currently not used!

                                        password can be OpcUa_Null!

  @param pPrivateKey              [out] The desired private key in DER encoded string of bytes.
*/
OpcUa_StatusCode OpcUa_P_Win32_LoadPrivateKeyFromKeyStore(
    OpcUa_StringA           privateKeyFile,
    OpcUa_P_FileFormat      fileFormat,
    OpcUa_StringA           password,
    OpcUa_UInt              keyType,
    OpcUa_Key*              pPrivateKey);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Crypto_Win32_H_ */
