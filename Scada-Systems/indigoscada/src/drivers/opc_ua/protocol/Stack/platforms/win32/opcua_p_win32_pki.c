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

#ifndef _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
#endif /* _CRT_SECURE_NO_DEPRECATE */

/* System Headers */
#include <windows.h>
#include <Wincrypt.h>

/* UA platform definitions */
#include <opcua_p_internal.h>
#include <opcua_p_memory.h>

#if OPCUA_SUPPORT_PKI_WIN32

/* Todo: Check whether needed (maybe for conversion) */
#include <openssl/pem.h>
#include <openssl/x509_vfy.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>

/* own headers */
#include <opcua_p_win32_pki.h>

OpcUa_Void OpcUa_P_ByteString_Clear(OpcUa_ByteString* pValue);

/*============================================================================
 * OpcUa_P_Win32_OpcUaPkiFlags2Win32PkiFlags
 *===========================================================================*/
OpcUa_UInt32 OpcUa_P_Win32_OpcUaPkiFlags2Win32PkiFlags(OpcUa_UInt32 opcUaFlags)
{
    OpcUa_UInt32 win32Flags = 0;

    if((opcUaFlags & OPCUA_P_PKI_WIN32_STORE_USER) == OPCUA_P_PKI_WIN32_STORE_USER)
    {
        win32Flags = CERT_SYSTEM_STORE_CURRENT_USER;
    }

    if((opcUaFlags & OPCUA_P_PKI_WIN32_STORE_MACHINE) == OPCUA_P_PKI_WIN32_STORE_MACHINE)
    {
        win32Flags = win32Flags | CERT_SYSTEM_STORE_LOCAL_MACHINE;
    }

    if((opcUaFlags & OPCUA_P_PKI_WIN32_STORE_SERVICES) == OPCUA_P_PKI_WIN32_STORE_SERVICES)
    {
        win32Flags = win32Flags | CERT_SYSTEM_STORE_SERVICES;
    }

    return win32Flags;
}

/*============================================================================
 * OpcUa_P_Win32_MultiByteToWideChar
 *===========================================================================*/
static OpcUa_Char* OpcUa_P_Win32_MultiByteToWideChar(OpcUa_StringA a_sSrc)
{
    int iLength = 0;
    int iResult = 0;
    OpcUa_UInt32 uSize = 0;
    OpcUa_CharA* pData = OpcUa_Null;
    OpcUa_Char* pUnicode = OpcUa_Null;

    if (a_sSrc == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    uSize = (OpcUa_UInt32)strlen(a_sSrc);
    pData = a_sSrc;

    iLength = MultiByteToWideChar(
        CP_UTF8,
        0,
        pData,
        uSize,
        0,
        0);

    if (iLength == 0)
    {
        return OpcUa_Null;
    }

    pUnicode = (OpcUa_Char*)OpcUa_P_Memory_Alloc((iLength+1)*sizeof(OpcUa_Char));
    OpcUa_MemSet(pUnicode, 0, (iLength+1)*sizeof(OpcUa_Char));

    iResult = MultiByteToWideChar(
        CP_UTF8,
        0,
        pData,
        uSize,
        pUnicode,
        iLength);

    if (iResult == 0)
    {
        OpcUa_P_Memory_Free(pUnicode);
        return OpcUa_Null;
    }

    return pUnicode;
}

/*============================================================================
 * OpcUa_P_Win32_CertificateStore_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_OpenCertificateStore(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void**                a_ppCertificateStore)
{
    OpcUa_Char*                                 pStoreName              = OpcUa_Null;
    OpcUa_P_OpenSSL_CertificateStore_Config*    pCertificateStoreCfg    = OpcUa_Null;
    OpcUa_UInt32                                win32PKIFlags           = 0;

OpcUa_InitializeStatus(OpcUa_Module_P_Win32, "PKI_OpenCertificateStore");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);

    *a_ppCertificateStore = OpcUa_Null;

    pCertificateStoreCfg = (OpcUa_P_OpenSSL_CertificateStore_Config*)a_pProvider->Handle;

    if(pCertificateStoreCfg)
    {
        /* translate common OPC UA PKI flags to specific win32 PKI flags */
        win32PKIFlags = OpcUa_P_Win32_OpcUaPkiFlags2Win32PkiFlags(pCertificateStoreCfg->Flags);
    }

    pStoreName = OpcUa_P_Win32_MultiByteToWideChar(pCertificateStoreCfg->CertificateTrustListLocation);

    /*** OPEN CERTIFICATE STORE ***/
    if(!(*a_ppCertificateStore = CertOpenStore(
                                            CERT_STORE_PROV_SYSTEM,
                                            0,
                                            (HCRYPTPROV)OpcUa_Null,
                                            win32PKIFlags | CERT_STORE_OPEN_EXISTING_FLAG,
                                            pStoreName)))
    {
       uStatus = OpcUa_Bad;
    }

    if (pStoreName != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(pStoreName);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_Win32_CertificateStore_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_CloseCertificateStore(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void**                a_ppCertificateStore)
{

OpcUa_InitializeStatus(OpcUa_Module_P_Win32, "PKI_CloseCertificateStore");

    OpcUa_ReferenceParameter(a_pProvider);

    if(a_ppCertificateStore != OpcUa_Null)
    {
        if(*a_ppCertificateStore)
        {
            /*** CLOSE CERTIFICATE STORE ***/
            if (!CertCloseStore(*a_ppCertificateStore, CERT_CLOSE_STORE_FORCE_FLAG))
            {
                uStatus = OpcUa_Bad;
            }
        }
        *a_ppCertificateStore = OpcUa_Null;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_Win32_PKI_ValidateCertificate
 * note: for this check to succeed, the root certificate must be installed
 *       in the store "ROOT" / "Vertrauenswürdige Stammzertifizierungsstellen"
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_ValidateCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Int*                  a_pValidationCode /* Validation return codes from Win32 */
    )
{
    OpcUa_P_OpenSSL_CertificateStore_Config*    pCertificateStoreCfg;
    PCCERT_CHAIN_CONTEXT     pChainContext  = OpcUa_Null;
    CERT_ENHKEY_USAGE        EnhkeyUsage;
    CERT_USAGE_MATCH         CertUsage;
    CERT_CHAIN_PARA          ChainPara;
    DWORD                    dwFlags        = CERT_CHAIN_CACHE_END_CERT;
    PCCERT_CONTEXT           pTargetCert    = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_P_Win32, "PKI_ValidateCertificate");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificateStore);

    pCertificateStoreCfg = (OpcUa_P_OpenSSL_CertificateStore_Config*)a_pProvider->Handle;

    EnhkeyUsage.cUsageIdentifier        = 0;
    EnhkeyUsage.rgpszUsageIdentifier    = OpcUa_Null;

    CertUsage.dwType                    = USAGE_MATCH_TYPE_AND;
    CertUsage.Usage                     = EnhkeyUsage;

    ChainPara.cbSize                    = sizeof(CERT_CHAIN_PARA);
    ChainPara.RequestedUsage            = CertUsage;

    switch (pCertificateStoreCfg->Flags & OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ALL)
    {
        case OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ONLY_LEAF:
            dwFlags |= OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ONLY_LEAF|CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY;
            break;
        case OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ALL_EXCEPT_SELF_SIGNED:
            dwFlags |= CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT|CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY;
            break;
        case OPCUA_P_PKI_WIN32_CHECK_REVOCATION_ALL:
            dwFlags |= CERT_CHAIN_REVOCATION_CHECK_CHAIN|CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY;
            break;
    }

    /*** CONVERT CERTIFICATE ***/
    pTargetCert = CertCreateCertificateContext(X509_ASN_ENCODING, a_pCertificate->Data, a_pCertificate->Length);
    OpcUa_ReturnErrorIfArgumentNull(pTargetCert);

    /*** BUILD CERTIFICATE CHAIN ***/
    if(!CertGetCertificateChain(
        OpcUa_Null,             /* use the default chain engine */
        pTargetCert,            /* pointer to the end certificate */
        OpcUa_Null,             /* use the default time */
        a_pCertificateStore,    /* Use the store opened */
        &ChainPara,             /* use AND logic and enhanced key usage */
                                /*  as indicated in the ChainPara */
                                /*  data structure */
        dwFlags,                /* ==>REVOCATION CONFIGURATION */
        OpcUa_Null,             /* currently reserved */
        &pChainContext))        /* return a pointer to the chain created */
    {
        /* The certificate chain could not be created! */
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);   /* Todo: new statuscode should be defined: CertificateValidationFailed */
    }

    if(a_pValidationCode)
    {
        *a_pValidationCode = pChainContext->TrustStatus.dwErrorStatus;
    }

    /*** CHECK ERROR STATUS OF THE CERTIFICATE CHAIN ***/
    switch(pChainContext->TrustStatus.dwErrorStatus)
    {
    case CERT_TRUST_NO_ERROR :
         /* Todo Trace: No error found for this certificate or chain. */
         break;
    case CERT_TRUST_IS_NOT_TIME_VALID:
         /* Todo Trace: This certificate or one of the certificates in the certificate chain is not time-valid. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateTimeInvalid); /* or OpcUa_BadCertificateIssuerTimeInvalid */
         break;
    case CERT_TRUST_IS_NOT_TIME_NESTED:
         /* Todo Trace: Certificates in the chain are not properly time-nested. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateTimeInvalid);
         break;
    case CERT_TRUST_IS_REVOKED:
         /* Todo Trace: Trust for this certificate or one of the certificates in the certificate chain has been revoked. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateRevoked);
         break;
    case CERT_TRUST_IS_NOT_SIGNATURE_VALID:
         /* Todo Trace: The certificate or one of the certificates in the certificate chain does not have a valid signature. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateInvalid); /*Todo: we need a statuscode like: CertificateSignatureInvalid */
         break;
    case CERT_TRUST_IS_NOT_VALID_FOR_USAGE:
         /* Todo Trace: The certificate or certificate chain is not valid in its proposed usage. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateUseNotAllowed);
         break;
    case CERT_TRUST_IS_UNTRUSTED_ROOT:
         /* Todo Trace: The certificate or certificate chain is based on an untrusted root. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateUntrusted);
         break;
    case CERT_TRUST_REVOCATION_STATUS_UNKNOWN:
         /* Todo Trace: The revocation status of the certificate or one of the certificates in the certificate chain is unknown. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateRevocationUnknown);
         break;
    case CERT_TRUST_IS_CYCLIC:
         /* Todo Trace: One of the certificates in the chain was issued by a certification authority that the original certificate had certified. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateUntrusted); /*Todo: we need a statuscode like: CyclicCertificateChain */
         break;
    case CERT_TRUST_IS_PARTIAL_CHAIN:
         /* Todo Trace: The certificate chain is not complete. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateUntrusted); /*Todo: we need a statuscode like: PartialCertificateChain */
         break;
    default:
         /* Todo Trace: Unknown error code. */
         OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateUntrusted);
         break;
    } /* End switch */

    /*** CHECK INFO STATUS OF THE CERTIFICATE CHAIN ***/
    switch(pChainContext->TrustStatus.dwInfoStatus)
    {
    case 0:
         break;
    case CERT_TRUST_HAS_EXACT_MATCH_ISSUER :
         /* Todo Trace: An exact match issuer certificate has been found for this certificate. */
         break;
    case CERT_TRUST_HAS_KEY_MATCH_ISSUER:
        /* Todo Trace: A key match issuer certificate has been found for this certificate. */
         break;
    case CERT_TRUST_HAS_NAME_MATCH_ISSUER:
        /* Todo Trace: A name match issuer certificate has been found for this certificate. */
         break;
    case CERT_TRUST_IS_SELF_SIGNED:
         /* Todo Trace: This certificate is self-signed. */
         break;
    case CERT_TRUST_IS_COMPLEX_CHAIN:
         /* Todo Trace: The certificate chain created is a complex chain. */
         break;
    } /* end switch */

    /* Check if any certificate of the chain is in the store if trusted certificates */
    if (pCertificateStoreCfg->Flags & OPCUA_P_PKI_WIN32_REQUIRE_CHAIN_CERTIFICATE_IN_TRUST_LIST)
    {
        DWORD           chaindepth       = pChainContext->cChain == 1 ? pChainContext->rgpChain[0]->cElement : 0;
        PCCERT_CONTEXT  pSearchCert      = OpcUa_Null;

        while (pSearchCert = CertEnumCertificatesInStore(a_pCertificateStore, pSearchCert))
        {
            DWORD nElement;
            for (nElement = 0; nElement < chaindepth; nElement++)
            {
                PCCERT_CONTEXT pElement = pChainContext->rgpChain[0]->rgpElement[nElement]->pCertContext;
                if ( pElement->cbCertEncoded == pSearchCert->cbCertEncoded )
                {
                    if ( OpcUa_MemCmp(pElement->pbCertEncoded, pSearchCert->pbCertEncoded, pTargetCert->cbCertEncoded) == 0 )
                    {
                        goto break_loop;
                    }
                }
            }
        }

break_loop:
        if (pSearchCert)
        {
            CertFreeCertificateContext(pSearchCert);
        }
        else
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadCertificateUntrusted);
        }
    }

    /*** FREE RESOURCES ***/

    if (pTargetCert)
    {
        CertFreeCertificateContext(pTargetCert);
        pTargetCert = OpcUa_Null;
    }

    if(pChainContext)
    {
        CertFreeCertificateChain(pChainContext);
        pChainContext = OpcUa_Null;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if (pTargetCert)
    {
        CertFreeCertificateContext(pTargetCert);
        pTargetCert = OpcUa_Null;
    }

    if(pChainContext)
    {
        CertFreeCertificateChain(pChainContext);
        pChainContext = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_Win32_PKI_SaveCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_SaveCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Void*                 a_pSaveHandle)      /* Index or number within store/destination filepath */
{

    /*OpcUa_P_Win32_CertificateStore_Config*    pCertificateStoreCfg    = OpcUa_Null; */

    HCERTSTORE      hSystemStore = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_P_Win32, "PKI_SaveCertificate");

    OpcUa_ReferenceParameter(a_pProvider);
    OpcUa_ReferenceParameter(a_pSaveHandle);

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificateStore);
    /*OpcUa_ReturnErrorIfArgumentNull(a_pSaveHandle); */

    /*pCertificateStoreCfg = (OpcUa_P_Win32_CertificateStore_Config*)a_pProvider->Handle; */

    hSystemStore = (HCERTSTORE)a_pCertificateStore;

    /*** ADD CERTIFICATE TO SPECIFIED CERTIFICATE STORE ***/
    /* Destination is defined by OpenCertificateStore function parameters */
    if(CertAddEncodedCertificateToStore(
                                    hSystemStore,
                                    X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                    a_pCertificate->Data,
                                    a_pCertificate->Length,
                                    CERT_STORE_ADD_REPLACE_EXISTING, /*has to be configurable => SaveHandle or StoreConfig?? */
                                    NULL))
    {
        /*Trace: certificate added to store */
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
 * OpcUa_P_Win32_CertificateStore_Certificate_Load
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Win32_PKI_LoadCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void*                 a_pLoadHandle,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_ByteString*           a_pCertificate)
{
    PCCERT_CONTEXT  pTargetCert  = OpcUa_Null;
    HCERTSTORE      hSystemStore = OpcUa_Null;
    OpcUa_Char*     pSubjectName = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_P_Win32, "PKI_LoadCertificate");

    OpcUa_ReferenceParameter(a_pProvider);

    OpcUa_ReturnErrorIfArgumentNull(a_pCertificateStore);
    OpcUa_ReturnErrorIfArgumentNull(a_pLoadHandle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);

    hSystemStore = (HCERTSTORE)a_pCertificateStore;
    pSubjectName = OpcUa_P_Win32_MultiByteToWideChar(a_pLoadHandle);

    /*** FIND CERTIFICATE IN SYSTEM STORE ***/
    if(pTargetCert = CertFindCertificateInStore(
                                              hSystemStore,                             /* Store handle. */
                                              X509_ASN_ENCODING,                        /* Encoding type. */
                                              0,                                        /* Not used. */
                                              CERT_FIND_SUBJECT_STR,                    /* Find type. Find a string in the certificate's subject. */
                                              pSubjectName,                             /* The string to be searched for. */
                                              pTargetCert))                             /* Previous context. */
    {
        /* certificate found */
        a_pCertificate->Length = pTargetCert->cbCertEncoded;
        a_pCertificate->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pCertificate->Length*sizeof(OpcUa_Byte));

        uStatus = OpcUa_P_Memory_MemCpy(a_pCertificate->Data, a_pCertificate->Length, pTargetCert->pbCertEncoded, a_pCertificate->Length);
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        /* specified certificate could not be found in certificate store */
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotFound);
    }


    /*** FREE RESOURCES ***/
    if (pTargetCert)
    {
        CertFreeCertificateContext(pTargetCert);
        pTargetCert = OpcUa_Null;
    }

    if (pSubjectName != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(pSubjectName);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /*** FREE RESOURCES ***/
    if (pTargetCert)
    {
        CertFreeCertificateContext(pTargetCert);
        pTargetCert = OpcUa_Null;
    }

    if(a_pCertificate)
    {
        OpcUa_P_ByteString_Clear(a_pCertificate);
    }

    if (pSubjectName != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(pSubjectName);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_Win32_LoadPrivateKeyFromKeyStore
 * note: for this to work the Certificate + Private Key must be in PKCS#12 format
 *       and imported into the Store "MY" / "Eigene Zertifikate"
 * important: please check "Schlüssel als exportierbar markieren"
 * openssl pkcs12 -export -in cert.pem -inkey private_key.pem -out cert.p12
 * Internet -> Inhalte -> Zertifikate... -> Eigene Zertifikate -> Importieren...
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Win32_LoadPrivateKeyFromKeyStore(
    OpcUa_StringA           a_privateKeyFile,
    OpcUa_P_FileFormat      a_fileFormat,       /* Not used */
    OpcUa_StringA           a_password,         /* Not used */
    OpcUa_UInt              a_keyType,          /* Not used */
    OpcUa_Key*              a_pPrivateKey)
{
    HCERTSTORE        hSystemStore     = OpcUa_Null;
    HCERTSTORE        hMemoryStore     = OpcUa_Null;

    PCCERT_CONTEXT    pTempCertContext = OpcUa_Null;
    PCCERT_CONTEXT    pTargetCert      = OpcUa_Null;
    OpcUa_Char*       pSubjectName     = OpcUa_Null;

    CRYPT_DATA_BLOB   pfx              = {0, OpcUa_Null};

    BIO*              pBio             = OpcUa_Null;
    PKCS12*           pPKCS12          = OpcUa_Null;
    EVP_PKEY*         pEvpKey          = OpcUa_Null;
    int               i                = 0;
    RSA*              pRsaPrivateKey   = OpcUa_Null;
    unsigned char*    pData            = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_P_Win32, "PKI_LoadPrivateKey");

    OpcUa_ReturnErrorIfArgumentNull(a_privateKeyFile);
    OpcUa_ReturnErrorIfArgumentNull(a_pPrivateKey);

    OpcUa_ReferenceParameter(a_fileFormat);
    OpcUa_ReferenceParameter(a_password);
    OpcUa_ReferenceParameter(a_keyType);

    /*** OPEN SYSTEM STORE ***/
    /* This has to move to OpenCertificateStore, */
    /* but a parameter for PKIProvider has to be */
    /* provided in the function declaration first! */
    if(!(hSystemStore = CertOpenStore(
                                  CERT_STORE_PROV_SYSTEM,
                                  0,
                                  (HCRYPTPROV)OpcUa_Null,
                                  CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_OPEN_EXISTING_FLAG,
                                  L"MY")))
    {
        /* specified certificate store could not be opened! */
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    pSubjectName = OpcUa_P_Win32_MultiByteToWideChar(a_privateKeyFile);

    /*** FIND CERTIFICATE OF DESIRED PRIVATE KEY IN SYSTEM STORE ***/
    if(!(pTargetCert = CertFindCertificateInStore(
                                  hSystemStore,                             /* Store handle. */
                                  PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,  /* Encoding type. */
                                  0,                                        /* Not used. */
                                  CERT_FIND_SUBJECT_STR,                    /* Find type. Find a string in the certificate's subject. */
                                  pSubjectName,                             /* The string to be searched for. */
                                  pTargetCert)))                            /* Previous context. */
    {
        /* specified certificate could not be found in certificate store */
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotFound);
    }


    /*** OPEN MEMORY STORE ***/
    if(!(hMemoryStore = CertOpenStore(
                                    CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)OpcUa_Null,
                                    0,
                                    OpcUa_Null)))
    {
        /* specified certificate store could not be opened! */
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }


    /*** ADD LINK TO CERTIFICATE CONTEXT IN MEMORY STORE ***/
    if(!(CertAddCertificateLinkToStore(
                                    hMemoryStore,
                                    pTargetCert,
                                    CERT_STORE_ADD_REPLACE_EXISTING,
                                    &pTempCertContext)))
    {
        /* specified certificate could not be added to memory store! */
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }


    /*** EXPORT CERTIFICATE AND PRIVATE KEY FROM STORE (PFX) ***/
    ZeroMemory(&pfx, sizeof(pfx));
    if(PFXExportCertStoreEx(hMemoryStore, &pfx, OpcUa_Null, OpcUa_Null, EXPORT_PRIVATE_KEYS))
    {
        pfx.pbData = (BYTE *)CryptMemAlloc(sizeof(BYTE)*pfx.cbData);
        OpcUa_GotoErrorIfAllocFailed(pfx.pbData);
        if(!PFXExportCertStoreEx(hMemoryStore, &pfx, OpcUa_Null, OpcUa_Null, EXPORT_PRIVATE_KEYS))
        {
            /* specified certificate could not be added to memory store! */
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }
    else
    {
        /* specified certificate could not be added to memory store! */
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    /*** CONVERT TO OPENSSL STRUCTURE FOR VERIFICATION ***/
    pBio = BIO_new(BIO_s_mem());
    OpcUa_GotoErrorIfAllocFailed(pBio);
    BIO_write(pBio, pfx.pbData, pfx.cbData);
    pPKCS12 = d2i_PKCS12_bio(pBio, OpcUa_Null);
    i = PKCS12_parse(pPKCS12,OpcUa_Null,&pEvpKey,OpcUa_Null,OpcUa_Null);

    if((pEvpKey) && (i>0))
    {
        /* convert to intermediary openssl struct */
        pRsaPrivateKey = EVP_PKEY_get1_RSA(pEvpKey);
        EVP_PKEY_free(pEvpKey);
        pEvpKey = OpcUa_Null;
        OpcUa_GotoErrorIfNull(pRsaPrivateKey, OpcUa_Bad);

        /* get required length */
        a_pPrivateKey->Key.Length = i2d_RSAPrivateKey(pRsaPrivateKey, OpcUa_Null);
        OpcUa_GotoErrorIfTrue((a_pPrivateKey->Key.Length <= 0), OpcUa_Bad);

        /* allocate target buffer */
        a_pPrivateKey->Key.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pPrivateKey->Key.Length);
        OpcUa_GotoErrorIfAllocFailed(a_pPrivateKey->Key.Data);

        /* do real conversion */
        pData = a_pPrivateKey->Key.Data;
        a_pPrivateKey->Key.Length = i2d_RSAPrivateKey(pRsaPrivateKey, &pData);
        OpcUa_GotoErrorIfTrue((a_pPrivateKey->Key.Length <= 0), OpcUa_Bad);
        RSA_free(pRsaPrivateKey);
        pRsaPrivateKey = OpcUa_Null;
    }
    else
    {
        /* No private key information could be found */
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotFound);
    }

    /*** FREE RESOURCES ***/
    if(pBio)
    {
        BIO_free(pBio);
        pBio = OpcUa_Null;
    }

    if(pPKCS12)
    {
        PKCS12_free(pPKCS12);
        pPKCS12 = OpcUa_Null;
    }

    if(pTempCertContext)
    {
        CertFreeCertificateContext(pTempCertContext);
        pTempCertContext = OpcUa_Null;
    }

    /*** Free CerificateContextHandles ***/
    if(pTargetCert)
    {
        CertFreeCertificateContext(pTargetCert);
        pTargetCert = OpcUa_Null;
    }

    if(pfx.pbData)
    {
        CryptMemFree(pfx.pbData);
        pfx.pbData = OpcUa_Null;
    }

    /*** CLOSE MEMORY STORE ***/
    if(hMemoryStore)
    {
        if(!CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_CHECK_FLAG))
        {
            /* memory store could not be freed */
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
        hMemoryStore = OpcUa_Null;
    }

    /*** CLOSE SYSTEM STORE ***/
    /* This has to move to CloseCertificateStore, */
    /* but a parameter for PKIProvider has to be */
    /* provided in the function declaration first! */
    if(hSystemStore)
    {
        if(!CertCloseStore(hSystemStore, CERT_CLOSE_STORE_FORCE_FLAG))
        {
            /* system store could not be freed */
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }

    if (pSubjectName != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(pSubjectName);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /*** FREE RESOURCES ***/
    if(pEvpKey)
    {
        EVP_PKEY_free(pEvpKey);
    }

    if(pRsaPrivateKey)
    {
        RSA_free(pRsaPrivateKey);
    }

    if(a_pPrivateKey)
    {
        OpcUa_P_ByteString_Clear(&a_pPrivateKey->Key);
    }

    if(pBio)
    {
        BIO_free(pBio);
    }

    if(pPKCS12)
    {
        PKCS12_free(pPKCS12);
    }

    if(pTempCertContext)
    {
        CertFreeCertificateContext(pTempCertContext);
    }

    /*** Free CerificateContextHandles ***/
    if(pTargetCert)
    {
        CertFreeCertificateContext(pTargetCert);
    }

    if(pfx.pbData)
    {
        CryptMemFree(pfx.pbData);
    }

    /*** CLOSE MEMORY STORE ***/
    if(hMemoryStore)
    {
        if(!CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG))
        {
            /* memory store could not be freed */
            /* trace?? */
        }
    }

    /*** CLOSE SYSTEM STORE ***/
    /* This has to move to CloseCertificateStore, */
    /* but a parameter for PKIProvider has to be */
    /* provided in the function declaration first! */
    if(hSystemStore)
    {
        if(!CertCloseStore(hSystemStore, CERT_CLOSE_STORE_CHECK_FLAG))
        {
            /* system store could not be freed */
            /* trace?? */
        }
    }

    if(pSubjectName != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(pSubjectName);
    }

OpcUa_FinishErrorHandling;
}
#endif /* OPCUA_SUPPORT_PKI_WIN32 */
