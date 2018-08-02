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

#ifndef _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
#endif /* _CRT_SECURE_NO_DEPRECATE */

/* System Headers */
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509_vfy.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>



/* own headers */
#include <opcua_p_openssl_pki.h>

/* WORKAROUND */
#include <windows.h>
#include <string.h>
#include <stdio.h>

/*============================================================================
 * OpcUa_P_OpenSSL_BuildFullPath
 *===========================================================================*/
static
#ifdef _WIN32_WCE
OpcUa_StatusCode OpcUa_P_OpenSSL_BuildFullPath( /*  in */ char*         a_pPath,
                                                /*  in */ wchar_t*      a_wpFileName,
                                                /*  in */ unsigned int  a_uiFullPathBufferLength,
                                                /* out */ char*         a_pFullPath)
{
    char         a_pFileName[MAX_PATH];
    unsigned int uiPathLength;
    unsigned int uiFileLength;
    DWORD        i;

    for ( i=0; i<MAX_PATH; i++ )
    {
        a_pFileName[i] = (char)a_wpFileName[i];
        if ( a_wpFileName[i] == 0 )
        {
            break;
        }
    }
#else /* _WIN32_WCE */
OpcUa_StatusCode OpcUa_P_OpenSSL_BuildFullPath( /*  in */ char*         a_pPath,
                                                /*  in */ char*         a_pFileName,
                                                /*  in */ unsigned int  a_uiFullPathBufferLength,
                                                /* out */ char*         a_pFullPath)
{
    unsigned int uiPathLength;
    unsigned int uiFileLength;
#endif /* _WIN32_WCE */

    OpcUa_ReturnErrorIfArgumentNull(a_pPath);
    OpcUa_ReturnErrorIfArgumentNull(a_pFileName);
    OpcUa_ReturnErrorIfArgumentNull(a_pFullPath);

    uiPathLength = (unsigned int)strlen(a_pPath);
    uiFileLength = (unsigned int)strlen(a_pFileName);

    if((uiPathLength + uiFileLength + 2) > a_uiFullPathBufferLength)
    {
        return OpcUa_BadInvalidArgument;
    }

#if OPCUA_USE_SAFE_FUNCTIONS
    strncpy_s(a_pFullPath, a_uiFullPathBufferLength, a_pPath, uiPathLength + 1);
    strncat_s(a_pFullPath, a_uiFullPathBufferLength, "\\", 1);
    strncat_s(a_pFullPath, a_uiFullPathBufferLength, a_pFileName, uiFileLength);
#else /* OPCUA_USE_SAFE_FUNCTIONS */
    strncpy(a_pFullPath, a_pPath, uiPathLength + 1);
    strncat(a_pFullPath, "\\", 1);
    strncat(a_pFullPath, a_pFileName, uiFileLength);
#endif /* OPCUA_USE_SAFE_FUNCTIONS */

    return OpcUa_Good;
}

/*============================================================================
 * FindFirstFile -- CE Variant
 *===========================================================================*/
#ifdef _WIN32_WCE
static
HANDLE FindFirstFileCE( /*  in */ char*              DirSpec,
                        /* out */ LPWIN32_FIND_DATAW lpFindFileData)
{
    wchar_t     DirSpecW[MAX_PATH];
    DWORD       i;

    for ( i=0; i<MAX_PATH; i++ )
    {
        DirSpecW[i] = DirSpec[i];
        if ( DirSpec[i] == 0 )
        {
            break;
        }
    }

    return FindFirstFileW(DirSpecW, lpFindFileData);
}
#undef FindFirstFile
#define FindFirstFile FindFirstFileCE
#else /* Windows: use Ansi */
#undef TEXT
#define TEXT(x) x
#undef FindFirstFile
#define FindFirstFile FindFirstFileA
#undef FindNextFile
#define FindNextFile FindNextFileA
#undef WIN32_FIND_DATA
#define WIN32_FIND_DATA WIN32_FIND_DATAA
#endif

/*============================================================================
 * verify_callback
 *===========================================================================*/
static OpcUa_Int OpcUa_P_OpenSSL_CertificateStore_Verify_Callback(int a_ok, X509_STORE_CTX* a_pStore)
{
    OpcUa_P_OpenSSL_CertificateStore_Config*    pCertificateStoreCfg;

    pCertificateStoreCfg = X509_STORE_CTX_get_app_data(a_pStore);
    if(a_ok == 0)
    {
        /* certificate not ok */
        char    buf[256];
        X509*   err_cert;
        int     err;
        int     depth;

        err_cert = X509_STORE_CTX_get_current_cert(a_pStore);
        err      = X509_STORE_CTX_get_error(a_pStore);
        depth    = X509_STORE_CTX_get_error_depth(a_pStore);

        X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "\nverify error:\n\tnum=%d:%s\n\tdepth=%d\n\t%s\n", err, X509_verify_cert_error_string(err), depth, buf);

        X509_NAME_oneline(X509_get_issuer_name(err_cert), buf, 256);
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "\tissuer=%s\n", buf);

        switch (err)
        {
            case X509_V_ERR_UNABLE_TO_GET_CRL:
                if (pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_SUPPRESS_CRL_NOT_FOUND_ERROR)
                    a_ok = 1;
                break;

            case X509_V_ERR_CRL_NOT_YET_VALID:
            case X509_V_ERR_CRL_HAS_EXPIRED:
                if (pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_SUPPRESS_CRL_VALIDITY_PERIOD_CHECK)
                    a_ok = 1;
                break;

            case X509_V_ERR_CERT_NOT_YET_VALID:
            case X509_V_ERR_CERT_HAS_EXPIRED:
                if (pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_SUPPRESS_CERT_VALIDITY_PERIOD_CHECK)
                    a_ok = 1;
                break;
        }
    }

    return a_ok;
}

/*============================================================================
 * OpcUa_P_OpenSSL_CertificateStore_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_OpenCertificateStore(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void**                a_ppCertificateStore)           /* type depends on store implementation */
{
    OpcUa_P_OpenSSL_CertificateStore_Config*    pCertificateStoreCfg;
    X509_STORE*         pStore;
    X509_LOOKUP*        pLookup;
    HANDLE              hFind                   = INVALID_HANDLE_VALUE;
    char                DirSpec[MAX_PATH];
    char                CertFile[MAX_PATH];
    DWORD               dwError;
    WIN32_FIND_DATA     FindFileData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_OpenCertificateStore");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_ppCertificateStore);

    *a_ppCertificateStore = OpcUa_Null;

    pCertificateStoreCfg = (OpcUa_P_OpenSSL_CertificateStore_Config*)a_pProvider->Handle;

    if(!(*a_ppCertificateStore = pStore = X509_STORE_new()))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    X509_STORE_set_verify_cb_func(pStore, OpcUa_P_OpenSSL_CertificateStore_Verify_Callback);

    if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_USE_DEFAULT_CERT_CRL_LOOKUP_METHOD)
    {
        if(X509_STORE_set_default_paths(pStore) != 1)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "error at X509_STORE_set_default_paths!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }

    if(!(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_DONT_ADD_TRUST_LIST_TO_ROOT_CERTIFICATES))
    {
        if(pCertificateStoreCfg->CertificateTrustListLocation == OpcUa_Null || pCertificateStoreCfg->CertificateTrustListLocation[0] == '\0')
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }

        /* how to search for certificate & CRLs */
        if(!(pLookup = X509_STORE_add_lookup(pStore, X509_LOOKUP_file())))
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }

        uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateTrustListLocation, TEXT("*.der"), MAX_PATH, DirSpec);
        OpcUa_GotoErrorIfBad(uStatus);

        hFind = FindFirstFile(DirSpec, &FindFileData);
        if(hFind != INVALID_HANDLE_VALUE)
        {
            do {
                uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateTrustListLocation, FindFileData.cFileName, MAX_PATH, CertFile);
                OpcUa_GotoErrorIfBad(uStatus);

                /* add CACertificate lookup */
                if(X509_LOOKUP_load_file(pLookup, CertFile, X509_FILETYPE_ASN1) != 1) /* DER encoded */
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "error at X509_LOOKUP_load_file: skipping %s\n", CertFile);
                }
            }
            while(FindNextFile(hFind, &FindFileData) != 0);

            dwError = GetLastError();
            if(dwError != ERROR_NO_MORE_FILES)
            {
                uStatus = OpcUa_Bad;
                OpcUa_GotoErrorIfBad(uStatus);
            }

            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }

    if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_ADD_UNTRUSTED_LIST_TO_ROOT_CERTIFICATES)
    {
        if(pCertificateStoreCfg->CertificateUntrustedListLocation == OpcUa_Null || pCertificateStoreCfg->CertificateUntrustedListLocation[0] == '\0')
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }

        if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_UNTRUSTED_LIST_IS_INDEX)
        {
            /* how to search for certificate */
            if(!(pLookup = X509_STORE_add_lookup(pStore, X509_LOOKUP_hash_dir())))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }

            /* add hash lookup */
            if(X509_LOOKUP_add_dir(pLookup, pCertificateStoreCfg->CertificateUntrustedListLocation, X509_FILETYPE_ASN1) != 1) /* DER encoded */
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "error at X509_LOOKUP_add_dir!\n");
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }
        }
        else
        {
            /* how to search for certificate & CRLs */
            if(!(pLookup = X509_STORE_add_lookup(pStore, X509_LOOKUP_file())))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }

            uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateUntrustedListLocation, TEXT("*.der"), MAX_PATH, DirSpec);
            OpcUa_GotoErrorIfBad(uStatus);

            hFind = FindFirstFile(DirSpec, &FindFileData);
            if(hFind != INVALID_HANDLE_VALUE)
            {
                do {
                    uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateUntrustedListLocation, FindFileData.cFileName, MAX_PATH, CertFile);
                    OpcUa_GotoErrorIfBad(uStatus);

                    /* add CACertificate lookup */
                    if(X509_LOOKUP_load_file(pLookup, CertFile, X509_FILETYPE_ASN1) != 1) /* DER encoded */
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "error at X509_LOOKUP_load_file: skipping %s\n", CertFile);
                    }
                }
                while(FindNextFile(hFind, &FindFileData) != 0);

                dwError = GetLastError();
                if(dwError != ERROR_NO_MORE_FILES)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_Bad);
                }

                FindClose(hFind);
                hFind = INVALID_HANDLE_VALUE;
            }
        }
    }

    if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL)
    {
        if(pCertificateStoreCfg->CertificateRevocationListLocation == OpcUa_Null || pCertificateStoreCfg->CertificateRevocationListLocation[0] == '\0')
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }

        if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_REVOCATION_LIST_IS_INDEX)
        {
            /* how to search for certificate & CRLs */
            if(!(pLookup = X509_STORE_add_lookup(pStore, X509_LOOKUP_hash_dir())))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }

            /* add CTL lookup */
            if(X509_LOOKUP_add_dir(pLookup, pCertificateStoreCfg->CertificateRevocationListLocation, X509_FILETYPE_PEM) != 1) /* PEM encoded */
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "error at X509_LOOKUP_add_dir!\n");
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }
        }
        else if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_REVOCATION_LIST_IS_CONCATENATED_PEM_FILE)
        {
            /* how to search for certificate & CRLs */
            if(!(pLookup = X509_STORE_add_lookup(pStore, X509_LOOKUP_file())))
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }

            /* add CRL lookup */
            if(X509_load_crl_file(pLookup, pCertificateStoreCfg->CertificateRevocationListLocation, X509_FILETYPE_PEM) != 1) /* PEM encoded */
            {
                /* printf("error at X509_load_crl_file!\n"); */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "error at X509_load_crl_file!\n");
            }
        }
        else
        {
            /* how to search for certificate & CRLs */
            if(!(pLookup = X509_STORE_add_lookup(pStore, X509_LOOKUP_file())))
            {
                uStatus = OpcUa_Bad;
                OpcUa_GotoErrorIfBad(uStatus);
            }

            uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateRevocationListLocation, TEXT("*.crl"), MAX_PATH, DirSpec);
            OpcUa_GotoErrorIfBad(uStatus);

            hFind = FindFirstFile(DirSpec, &FindFileData);
            if(hFind != INVALID_HANDLE_VALUE)
            {
                do {
                    uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateRevocationListLocation, FindFileData.cFileName, MAX_PATH, CertFile);
                    OpcUa_GotoErrorIfBad(uStatus);

                    /* add CACertificate lookup */
                    if(X509_load_crl_file(pLookup, CertFile, X509_FILETYPE_PEM) != 1) /* PEM encoded */
                    {
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "error at X509_load_crl_file: skipping %s\n", CertFile);
                    }
                }
                while(FindNextFile(hFind, &FindFileData) != 0);

                dwError = GetLastError();
                if(dwError != ERROR_NO_MORE_FILES)
                {
                    uStatus = OpcUa_Bad;
                    OpcUa_GotoErrorIfBad(uStatus);
                }

                FindClose(hFind);
                hFind = INVALID_HANDLE_VALUE;
            }
        }

        if((pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL) == OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL)
        {
            /* set the flags of the store so that CRLs are consulted */
            if(X509_STORE_set_flags(pStore, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL) != 1)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }
        }
        else if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ONLY_LEAF)
        {
            /* set the flags of the store so that CRLs are consulted */
            if(X509_STORE_set_flags(pStore, X509_V_FLAG_CRL_CHECK) != 1)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }
        }
    }

    if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_CHECK_SELF_SIGNED_SIGNATURE)
    {
        /* set the flags of the store so that CRLs are consulted */
        if(X509_STORE_set_flags(pStore, X509_V_FLAG_CHECK_SS_SIGNATURE) != 1)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }

    if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_ALLOW_PROXY_CERTIFICATES)
    {
        /* set the flags of the store so that CRLs are consulted */
        if(X509_STORE_set_flags(pStore, X509_V_FLAG_ALLOW_PROXY_CERTS) != 1)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
    }

    if(*a_ppCertificateStore != OpcUa_Null)
    {
        X509_STORE_free((X509_STORE*)*a_ppCertificateStore);
        *a_ppCertificateStore = OpcUa_Null;
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_CertificateStore_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_CloseCertificateStore(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void**                a_ppCertificateStore) /* type depends on store implementation */
{
OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_CloseCertificateStore");

    OpcUa_ReferenceParameter(a_pProvider);

    if(*a_ppCertificateStore != OpcUa_Null)
    {
        X509_STORE_free((X509_STORE*)*a_ppCertificateStore);
        *a_ppCertificateStore = OpcUa_Null;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_ValidateCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_ValidateCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Int*                  a_pValidationCode /* Validation return codes from OpenSSL */
    )
{
    OpcUa_P_OpenSSL_CertificateStore_Config*    pCertificateStoreCfg;

    const unsigned char* p;

    X509*               pX509Certificate        = OpcUa_Null;
    STACK_OF(X509)*     pX509Chain              = OpcUa_Null;
    X509_STORE_CTX*     verify_ctx              = OpcUa_Null;    /* holds data used during verification process */

    HANDLE              hFind                   = INVALID_HANDLE_VALUE;
    char                DirSpec[MAX_PATH];
    char                CertFile[MAX_PATH];
    WIN32_FIND_DATA     FindFileData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_ValidateCertificate");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificateStore);
    OpcUa_ReturnErrorIfArgumentNull(a_pValidationCode);

    pCertificateStoreCfg = (OpcUa_P_OpenSSL_CertificateStore_Config*)a_pProvider->Handle;

    /* convert DER encoded bytestring certificate to openssl X509 certificate */
    p = a_pCertificate->Data;
    if(!(pX509Certificate = d2i_X509((X509**)OpcUa_Null, &p, a_pCertificate->Length)))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    while(p < a_pCertificate->Data + a_pCertificate->Length)
    {
        X509* pX509AddCertificate;
        if(!(pX509AddCertificate = d2i_X509((X509**)OpcUa_Null, &p, a_pCertificate->Data + a_pCertificate->Length - p)))
        {
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
        if(pX509Chain == NULL)
        {
            pX509Chain = sk_X509_new_null();
            OpcUa_GotoErrorIfAllocFailed(pX509Chain);
        }
        if(!sk_X509_push(pX509Chain, pX509AddCertificate))
        {
            X509_free(pX509AddCertificate);
            OpcUa_GotoErrorWithStatus(OpcUa_Bad);
        }
    }

    /* create verification context and initialize it */
    if(!(verify_ctx = X509_STORE_CTX_new()))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
    if(X509_STORE_CTX_init(verify_ctx, (X509_STORE*)a_pCertificateStore, pX509Certificate, pX509Chain) != 1)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }
#else
    X509_STORE_CTX_init(verify_ctx, (X509_STORE*)a_pCertificateStore, pX509Certificate, pX509Chain);
#endif

    if(X509_STORE_CTX_set_app_data(verify_ctx, pCertificateStoreCfg) != 1)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    if((pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL) == OPCUA_P_PKI_OPENSSL_CHECK_REVOCATION_ALL_EXCEPT_SELF_SIGNED
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
        && !X509_STORE_CTX_get_check_issued(verify_ctx) (verify_ctx, pX509Certificate, pX509Certificate))
#else
        && !verify_ctx->check_issued(verify_ctx, pX509Certificate, pX509Certificate))
#endif
    {
        /* set the flags of the store so that CRLs are consulted */
        X509_STORE_CTX_set_flags(verify_ctx, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL);
    }

    /* verify the certificate */
    *a_pValidationCode = X509_V_OK;
    if(X509_verify_cert(verify_ctx) <= 0)
    {
        *a_pValidationCode = X509_STORE_CTX_get_error(verify_ctx);
        switch(X509_STORE_CTX_get_error(verify_ctx))
        {
        case X509_V_ERR_CERT_HAS_EXPIRED:
        case X509_V_ERR_CERT_NOT_YET_VALID:
        case X509_V_ERR_CRL_NOT_YET_VALID:
        case X509_V_ERR_CRL_HAS_EXPIRED:
        case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
        case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
        case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD:
        case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD:
            {
                uStatus = OpcUa_BadCertificateTimeInvalid;
                break;
            }
        case X509_V_ERR_CERT_REVOKED:
            {
                uStatus = OpcUa_BadCertificateRevoked;
                break;
            }
        case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            {
                uStatus = OpcUa_BadCertificateUntrusted;
                break;
            }
        case X509_V_ERR_CERT_SIGNATURE_FAILURE:
            {
                uStatus = OpcUa_BadSecurityChecksFailed;
                break;
            }
        default:
            {
                uStatus = OpcUa_BadCertificateInvalid;
            }
        }
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(pCertificateStoreCfg->Flags & OPCUA_P_PKI_OPENSSL_REQUIRE_CHAIN_CERTIFICATE_IN_TRUST_LIST)
    {
        BIO*             pCertificateFile;
        X509*            pTrustCert;
        STACK_OF(X509)*  chain;
        int              trusted, n;

        chain = X509_STORE_CTX_get_chain(verify_ctx);
        trusted = 0;
        if(pCertificateStoreCfg->CertificateTrustListLocation == NULL || pCertificateStoreCfg->CertificateTrustListLocation[0] == '\0')
        {
            uStatus = OpcUa_Bad;
            OpcUa_GotoErrorIfBad(uStatus);
        }

        uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateTrustListLocation, TEXT("*.der"), MAX_PATH, DirSpec);
        OpcUa_GotoErrorIfBad(uStatus);

        hFind = FindFirstFile(DirSpec, &FindFileData);
        if(hFind != INVALID_HANDLE_VALUE)
        {
            do {
                uStatus = OpcUa_P_OpenSSL_BuildFullPath(pCertificateStoreCfg->CertificateTrustListLocation, FindFileData.cFileName, MAX_PATH, CertFile);
                OpcUa_GotoErrorIfBad(uStatus);

                /* read DER certificates */
                pCertificateFile = BIO_new_file(CertFile, "rb");
                if(pCertificateFile == OpcUa_Null)
                {
                    continue; /* ignore access errors */
                }

                pTrustCert = d2i_X509_bio(pCertificateFile, (X509**)OpcUa_Null);
                BIO_free(pCertificateFile);
                if(pTrustCert == OpcUa_Null)
                {
                    continue; /* ignore parse errors */
                }

                for(n = 0; n < sk_X509_num(chain); n++)
                {
                    if (X509_cmp(sk_X509_value(chain, n), pTrustCert) == 0)
                       break;
                }

                X509_free(pTrustCert);
                if(n < sk_X509_num(chain))
                {
                    trusted = 1;
                    break;
                }
            }
            while(FindNextFile(hFind, &FindFileData) != 0);

            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }

        if(!trusted)
        {
            uStatus = OpcUa_BadCertificateUntrusted;
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }

    X509_STORE_CTX_free(verify_ctx);
    X509_free(pX509Certificate);
    if(pX509Chain != OpcUa_Null)
    {
        sk_X509_pop_free(pX509Chain, X509_free);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
    }

    if(verify_ctx != OpcUa_Null)
    {
        X509_STORE_CTX_free(verify_ctx);
    }

    if(pX509Certificate != OpcUa_Null)
    {
        X509_free(pX509Certificate);
    }

    if(pX509Chain != OpcUa_Null)
    {
        sk_X509_pop_free(pX509Chain, X509_free);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_SaveCertificate
 *===========================================================================*/
/*
    ToDo:   Create Access to OpenSSL certificate store
            => Only API to In-Memory-Store is available for version 0.9.8x
            => Wait until Directory- and/or File-Store is available
*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_SaveCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Void*                 a_pSaveHandle)      /* Index or number within store/destination filepath */
{
    X509*                                       pX509Certificate        = OpcUa_Null;
    BIO*                                        pCertificateFile        = OpcUa_Null;

    const unsigned char*                        p;

    OpcUa_UInt32                                i;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_SaveCertificate");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificateStore);
    OpcUa_ReturnErrorIfArgumentNull(a_pSaveHandle);

    /* save DER certificate */
    pCertificateFile = BIO_new_file((const char*)a_pSaveHandle, "wb");
    OpcUa_GotoErrorIfArgumentNull(pCertificateFile);

    /* convert openssl X509 certificate to DER encoded bytestring certificate */
    p = a_pCertificate->Data;
    while(p < a_pCertificate->Data + a_pCertificate->Length)
    {
        if(!(pX509Certificate = d2i_X509((X509**)OpcUa_Null, &p, a_pCertificate->Data + a_pCertificate->Length - p)))
        {
            BIO_free (pCertificateFile);
            uStatus = OpcUa_Bad;
            OpcUa_GotoErrorIfBad(uStatus);
        }

        i = i2d_X509_bio(pCertificateFile, pX509Certificate);

        if(i < 1)
        {
            BIO_free (pCertificateFile);
            uStatus =  OpcUa_Bad;
            OpcUa_GotoErrorIfBad(uStatus);
        }

        X509_free(pX509Certificate);
        pX509Certificate = OpcUa_Null;
    }

    if(BIO_free (pCertificateFile) == 0)
    {
        uStatus =  OpcUa_Bad;
        OpcUa_GotoErrorIfBad(uStatus);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pX509Certificate != OpcUa_Null)
    {
        X509_free(pX509Certificate);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_CertificateStore_Certificate_Load
 *===========================================================================*/
/*
    ToDo:   Create Access to OpenSSL certificate store
            => Only API to In-Memory-Store is available for version 0.9.8x
            => Wait until Directory- and/or File-Store is available
*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_LoadCertificate(
    OpcUa_PKIProvider*          a_pProvider,
    OpcUa_Void*                 a_pLoadHandle,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_ByteString*           a_pCertificate)
{
    OpcUa_Byte*     buf                 = OpcUa_Null;
    OpcUa_Byte*     p                   = OpcUa_Null;
    BIO*            pCertificateFile    = OpcUa_Null;
    X509*           pTmpCert            = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_LoadCertificate");

    OpcUa_ReturnErrorIfArgumentNull(a_pProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pProvider->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_pLoadHandle);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificateStore);
    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);

    /* read DER certificates */
    pCertificateFile = BIO_new_file((const char*)a_pLoadHandle, "rb");
    OpcUa_GotoErrorIfArgumentNull(pCertificateFile);

    if(!(pTmpCert = d2i_X509_bio(pCertificateFile, (X509**)OpcUa_Null)))
    {
        uStatus = OpcUa_Bad;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    a_pCertificate->Length = i2d_X509(pTmpCert, NULL);
    buf = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pCertificate->Length);
    OpcUa_GotoErrorIfAllocFailed(buf);
    p = buf;
    for (;;)
    {
        i2d_X509(pTmpCert, &p);
        X509_free(pTmpCert);
        if(!(pTmpCert = d2i_X509_bio(pCertificateFile, (X509**)OpcUa_Null)))
        {
            break;
        }
        p = OpcUa_P_Memory_ReAlloc(buf, a_pCertificate->Length + i2d_X509(pTmpCert, NULL));
        OpcUa_GotoErrorIfAllocFailed(p);
        buf = p;
        p = buf + a_pCertificate->Length;
        a_pCertificate->Length += i2d_X509(pTmpCert, NULL);
    }

    if(BIO_free (pCertificateFile) == 0)
    {
        pCertificateFile = OpcUa_Null;
        uStatus =  OpcUa_Bad;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    a_pCertificate->Data = buf;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pCertificateFile != OpcUa_Null)
    {
        BIO_free(pCertificateFile);
    }

    if(pTmpCert != OpcUa_Null)
    {
        X509_free(pTmpCert);
    }

    if(buf != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(buf);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_P_OpenSSL_PKI_LoadPrivateKeyFromFile
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_LoadPrivateKeyFromFile(
    OpcUa_StringA           a_privateKeyFile,
    OpcUa_P_FileFormat      a_fileFormat,
    OpcUa_StringA           a_password,         /* optional: just needed encrypted PEM */
    OpcUa_UInt              a_keyType,
    OpcUa_Key*              a_pPrivateKey)
{
    BIO*            pPrivateKeyFile     = OpcUa_Null;
    RSA*            pRsaPrivateKey      = OpcUa_Null;
    EVP_PKEY*       pEvpKey             = OpcUa_Null;
    unsigned char*  pData;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_LoadPrivateKeyFromFile");

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_privateKeyFile);
    OpcUa_ReturnErrorIfArgumentNull(a_pPrivateKey);

    if(a_fileFormat == OpcUa_Crypto_Encoding_Invalid)
    {
        return OpcUa_BadInvalidArgument;
    }

    /* open file */
    pPrivateKeyFile = BIO_new_file((const char*)a_privateKeyFile, "rb");
    OpcUa_ReturnErrorIfArgumentNull(pPrivateKeyFile);

    /* read and convert file */
    switch(a_fileFormat)
    {
    case OpcUa_Crypto_Encoding_PEM:
        {
            /* read from file */
            pEvpKey = PEM_read_bio_PrivateKey(  pPrivateKeyFile,    /* file                 */
                                                NULL,               /* key struct           */
                                                0,                  /* password callback    */
                                                a_password);        /* default passphrase or arbitrary handle */
            break;
        }
    case OpcUa_Crypto_Encoding_PKCS12:
        {
            int i;

            /* read from file. */
            PKCS12* pPkcs12 = d2i_PKCS12_bio(pPrivateKeyFile, NULL);

            if(pPkcs12 == NULL)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }

            /* parse the certificate. */
            if(a_keyType == OpcUa_Crypto_KeyType_Asymmetric)
            {
                X509 *pCert;
                STACK_OF(X509) *pCA = sk_X509_new_null();
                OpcUa_GotoErrorIfNull(pCA, OpcUa_Bad);
                i = PKCS12_parse(pPkcs12, a_password, &pEvpKey, &pCert, &pCA);
                PKCS12_free(pPkcs12);

                if(i <= 0 || pCert == NULL)
                {
                    sk_X509_pop_free(pCA, X509_free);
                    OpcUa_GotoErrorWithStatus(OpcUa_Bad);
                }

                a_pPrivateKey->Key.Length = i2d_X509(pCert, NULL);
                for(i = 0; i < sk_X509_num(pCA); i++)
                {
                    a_pPrivateKey->Key.Length += i2d_X509(sk_X509_value(pCA, i), NULL);
                }

                a_pPrivateKey->Key.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pPrivateKey->Key.Length);
                if(a_pPrivateKey->Key.Data == OpcUa_Null)
                {
                    sk_X509_pop_free(pCA, X509_free);
                    X509_free(pCert);
                    OpcUa_GotoErrorWithStatus(OpcUa_Bad);
                }

                pData = a_pPrivateKey->Key.Data;
                i2d_X509(pCert, &pData);
                for(i = 0; i < sk_X509_num(pCA); i++)
                {
                    i2d_X509(sk_X509_value(pCA, i), &pData);
                }

                a_pPrivateKey->Type = OpcUa_Crypto_KeyType_Asymmetric;

                sk_X509_pop_free(pCA, X509_free);
                X509_free(pCert);
                EVP_PKEY_free(pEvpKey);
                BIO_free(pPrivateKeyFile);
                OpcUa_ReturnStatusCode;
            }

            i = PKCS12_parse(pPkcs12, a_password, &pEvpKey, NULL, NULL);
            PKCS12_free(pPkcs12);

            if(i <= 0)
            {
                OpcUa_GotoErrorWithStatus(OpcUa_Bad);
            }
            break;
        }
    case OpcUa_Crypto_Encoding_DER:
        {
            switch(a_keyType)
            {
            case OpcUa_Crypto_KeyType_Rsa_Private:
                pRsaPrivateKey = d2i_RSAPrivateKey_bio(pPrivateKeyFile, OpcUa_Null);
                break;

            default:
                uStatus = OpcUa_BadNotSupported;
                OpcUa_GotoError;
            }
            break;
        }
    default:
        {
            uStatus = OpcUa_BadNotSupported;
            OpcUa_GotoError;
        }
    }

    if(pEvpKey != NULL)
    {
        /* convert to intermediary openssl struct */
        switch(a_keyType)
        {
        case OpcUa_Crypto_KeyType_Any:
        case OpcUa_Crypto_KeyType_Rsa_Private:
            pRsaPrivateKey = EVP_PKEY_get1_RSA(pEvpKey);
            break;

        default:
            uStatus = OpcUa_BadNotSupported;
            OpcUa_GotoError;
        }

        EVP_PKEY_free(pEvpKey);
        pEvpKey = NULL;
    }

    if(pRsaPrivateKey != NULL)
    {
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

        a_pPrivateKey->Type = OpcUa_Crypto_KeyType_Rsa_Private;

        RSA_free(pRsaPrivateKey);
    }
    else
    {
        OpcUa_GotoErrorWithStatus(OpcUa_Bad);
    }

    BIO_free(pPrivateKeyFile);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(pEvpKey)
    {
        EVP_PKEY_free(pEvpKey);
    }

    if(a_pPrivateKey != OpcUa_Null)
    {
        if(a_pPrivateKey->Key.Data != OpcUa_Null)
        {
            OpcUa_P_Memory_Free(a_pPrivateKey->Key.Data);
            a_pPrivateKey->Key.Data = OpcUa_Null;
            a_pPrivateKey->Key.Length = -1;
        }
    }

    if(pPrivateKeyFile != NULL)
    {
        BIO_free(pPrivateKeyFile);
    }

    if(pRsaPrivateKey != NULL)
    {
        RSA_free(pRsaPrivateKey);
    }

OpcUa_FinishErrorHandling;
}

/**
  @brief Extracts data from a certificate.

  @param pCertificate          [in] The certificate to examine.
  @param pIssuer               [out, optional] The issuer name of the certificate.
  @param pSubject              [out, optional] The subject name of the certificate.
  @param pSubjectUri           [out, optional] The subject's URI of the certificate.
  @param pSubjectIP            [out, optional] The subject's IP of the certificate.
  @param pSubjectDNS           [out, optional] The subject's DNS name of the certificate.
  @param pCertThumbprint       [out, optional] The thumbprint of the certificate.
  @param pSubjectHash          [out, optional] The hash code of the certificate.
  @param pCertRawLength        [out, optional] The length of the DER encoded data.
                               can be smaller than the total length of pCertificate in case of chain certificate or garbage follow.
*/
OpcUa_StatusCode OpcUa_P_OpenSSL_PKI_ExtractCertificateData(
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
    X509*                       pX509Cert = OpcUa_Null;
    char*                       pName = OpcUa_Null;
    GENERAL_NAMES*              pNames = OpcUa_Null;
    const unsigned char*        p;

OpcUa_InitializeStatus(OpcUa_Module_P_OpenSSL, "PKI_ExtractCertificateData");

    OpcUa_ReturnErrorIfArgumentNull(a_pCertificate);

    if(a_pIssuer != OpcUa_Null)
    {
        a_pIssuer->Data = OpcUa_Null;
        a_pIssuer->Length = -1;
    }

    if(a_pSubject != OpcUa_Null)
    {
        a_pSubject->Data = OpcUa_Null;
        a_pSubject->Length = -1;
    }

    if(a_pSubjectUri != OpcUa_Null)
    {
        a_pSubjectUri->Data = OpcUa_Null;
        a_pSubjectUri->Length = -1;
    }

    if(a_pSubjectIP != OpcUa_Null)
    {
        a_pSubjectIP->Data = OpcUa_Null;
        a_pSubjectIP->Length = -1;
    }

    if(a_pSubjectDNS != OpcUa_Null)
    {
        a_pSubjectDNS->Data = OpcUa_Null;
        a_pSubjectDNS->Length = -1;
    }

    if(a_pCertThumbprint != OpcUa_Null)
    {
        a_pCertThumbprint->Data = OpcUa_Null;
        a_pCertThumbprint->Length = -1;
    }

    if(a_pSubjectHash != OpcUa_Null)
    {
        *a_pSubjectHash = 0;
    }

    if(a_pCertRawLength != OpcUa_Null)
    {
        *a_pCertRawLength = 0;
    }

    /* convert openssl X509 certificate to DER encoded bytestring certificate */
    p = a_pCertificate->Data;
    if(!(pX509Cert = d2i_X509((X509**)OpcUa_Null, &p, a_pCertificate->Length)))
    {
        uStatus = OpcUa_Bad;
        OpcUa_GotoErrorIfBad(uStatus);
    }

    if(a_pIssuer != OpcUa_Null)
    {
        pName = X509_NAME_oneline(X509_get_issuer_name(pX509Cert), NULL, 0);
        OpcUa_GotoErrorIfAllocFailed(pName);
        a_pIssuer->Length = (OpcUa_Int32)strlen(pName)+1;
        a_pIssuer->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pIssuer->Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(a_pIssuer->Data);
        uStatus = OpcUa_P_Memory_MemCpy(a_pIssuer->Data, a_pIssuer->Length, pName, a_pIssuer->Length);
        OpcUa_GotoErrorIfBad(uStatus);
        OPENSSL_free(pName);
        pName = OpcUa_Null;
    }

    if(a_pSubject != OpcUa_Null)
    {
        pName = X509_NAME_oneline(X509_get_subject_name(pX509Cert), NULL, 0);
        OpcUa_GotoErrorIfAllocFailed(pName);
        a_pSubject->Length = (OpcUa_Int32)strlen(pName)+1;
        a_pSubject->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pSubject->Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(a_pSubject->Data);
        uStatus = OpcUa_P_Memory_MemCpy(a_pSubject->Data, a_pSubject->Length, pName, a_pSubject->Length);
        OpcUa_GotoErrorIfBad(uStatus);
        OPENSSL_free(pName);
        pName = OpcUa_Null;
    }

    if(a_pSubjectUri != OpcUa_Null || a_pSubjectIP != OpcUa_Null || a_pSubjectDNS != OpcUa_Null)
    {
        pNames = X509_get_ext_d2i(pX509Cert, NID_subject_alt_name, OpcUa_Null, OpcUa_Null);
        if (pNames != OpcUa_Null)
        {
            int num;
            for (num = 0; num < sk_GENERAL_NAME_num(pNames); num++)
            {
                GENERAL_NAME *value = sk_GENERAL_NAME_value(pNames, num);
                switch (value->type)
                {
                case GEN_URI:
                    if (a_pSubjectUri != OpcUa_Null && a_pSubjectUri->Data == OpcUa_Null)
                    {
                        a_pSubjectUri->Length = value->d.ia5->length+1;
                        a_pSubjectUri->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pSubjectUri->Length*sizeof(OpcUa_Byte));
                        OpcUa_GotoErrorIfAllocFailed(a_pSubjectUri->Data);
                        uStatus = OpcUa_P_Memory_MemCpy(a_pSubjectUri->Data, a_pSubjectUri->Length, value->d.ia5->data, a_pSubjectUri->Length);
                        OpcUa_GotoErrorIfBad(uStatus);
                    }
                break;

                case GEN_IPADD:
                    if (a_pSubjectIP != OpcUa_Null && a_pSubjectIP->Data == OpcUa_Null)
                    {
                        a_pSubjectIP->Length = value->d.ip->length;
                        a_pSubjectIP->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pSubjectIP->Length*sizeof(OpcUa_Byte));
                        OpcUa_GotoErrorIfAllocFailed(a_pSubjectIP->Data);
                        uStatus = OpcUa_P_Memory_MemCpy(a_pSubjectIP->Data, a_pSubjectIP->Length, value->d.ip->data, a_pSubjectIP->Length);
                        OpcUa_GotoErrorIfBad(uStatus);
                    }
                break;

                case GEN_DNS:
                    if (a_pSubjectDNS != OpcUa_Null && a_pSubjectDNS->Data == OpcUa_Null)
                    {
                        a_pSubjectDNS->Length = value->d.ia5->length+1;
                        a_pSubjectDNS->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pSubjectDNS->Length*sizeof(OpcUa_Byte));
                        OpcUa_GotoErrorIfAllocFailed(a_pSubjectDNS->Data);
                        uStatus = OpcUa_P_Memory_MemCpy(a_pSubjectDNS->Data, a_pSubjectDNS->Length, value->d.ia5->data, a_pSubjectDNS->Length);
                        OpcUa_GotoErrorIfBad(uStatus);
                    }
                break;
                }
            }
            sk_GENERAL_NAME_pop_free(pNames, GENERAL_NAME_free);
            pNames = OpcUa_Null;
        }
    }

    if(a_pCertThumbprint != OpcUa_Null)
    {
        a_pCertThumbprint->Length = SHA_DIGEST_LENGTH;
        a_pCertThumbprint->Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(a_pCertThumbprint->Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(a_pCertThumbprint->Data);
        if(X509_digest(pX509Cert, EVP_sha1(), a_pCertThumbprint->Data, NULL) <= 0)
        {
            uStatus = OpcUa_Bad;
            OpcUa_GotoErrorIfBad(uStatus);
        }
    }

    if(a_pSubjectHash != OpcUa_Null)
    {
        *a_pSubjectHash = X509_NAME_hash(X509_get_subject_name(pX509Cert));
    }

    if(a_pCertRawLength != OpcUa_Null)
    {
        *a_pCertRawLength = (OpcUa_UInt32)(p - a_pCertificate->Data);
    }

    X509_free(pX509Cert);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(a_pIssuer != OpcUa_Null && a_pIssuer->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pIssuer->Data);
        a_pIssuer->Data = OpcUa_Null;
        a_pIssuer->Length = -1;
    }

    if(a_pSubject != OpcUa_Null && a_pSubject->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pSubject->Data);
        a_pSubject->Data = OpcUa_Null;
        a_pSubject->Length = -1;
    }

    if(a_pSubjectUri != OpcUa_Null && a_pSubjectUri->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pSubjectUri->Data);
        a_pSubjectUri->Data = OpcUa_Null;
        a_pSubjectUri->Length = -1;
    }

    if(a_pSubjectIP != OpcUa_Null && a_pSubjectIP->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pSubjectIP->Data);
        a_pSubjectIP->Data = OpcUa_Null;
        a_pSubjectIP->Length = -1;
    }

    if(a_pSubjectDNS != OpcUa_Null && a_pSubjectDNS->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pSubjectDNS->Data);
        a_pSubjectDNS->Data = OpcUa_Null;
        a_pSubjectDNS->Length = -1;
    }

    if(a_pCertThumbprint != OpcUa_Null && a_pCertThumbprint->Data != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(a_pCertThumbprint->Data);
        a_pCertThumbprint->Data = OpcUa_Null;
        a_pCertThumbprint->Length = -1;
    }

    if (pName != OpcUa_Null)
    {
        OPENSSL_free(pName);
    }

    if (pNames != OpcUa_Null)
    {
        sk_GENERAL_NAME_pop_free(pNames, GENERAL_NAME_free);
    }

    if(pX509Cert != OpcUa_Null)
    {
        X509_free(pX509Cert);
    }

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_REQUIRE_OPENSSL */
