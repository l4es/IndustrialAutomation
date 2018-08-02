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

/* own */
#include <opcua_pki.h>

/*============================================================================
 * OpcUa_PKIProvider_ValidateCertificate
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_PKIProvider_ValidateCertificate(
    struct _OpcUa_PKIProvider*  a_pPKI,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Int*                  a_pValidationCode) /* Validation return codes from OpenSSL */
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_PkiProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPKI);
    OpcUa_ReturnErrorIfNull(a_pPKI->ValidateCertificate, OpcUa_BadNotSupported);

    return a_pPKI->ValidateCertificate(a_pPKI, a_pCertificate, a_pCertificateStore, a_pValidationCode);
}

/*============================================================================
 * OpcUa_PKIProvider_OpenCertificateStore
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_PKIProvider_OpenCertificateStore(
    struct _OpcUa_PKIProvider*  a_pPKI,
    OpcUa_Void**                a_ppCertificateStore)        /* type depends on store implementation */
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_PkiProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPKI);
    OpcUa_ReturnErrorIfNull(a_pPKI->OpenCertificateStore, OpcUa_BadNotSupported);

    return a_pPKI->OpenCertificateStore(a_pPKI, a_ppCertificateStore);
}

/*============================================================================
 * OpcUa_PKIProvider_SaveCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_PKIProvider_LoadCertificate(
    struct _OpcUa_PKIProvider*  a_pPKI,
    OpcUa_Void*                 a_pLoadHandle,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_ByteString*           a_pCertificate)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_PkiProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPKI);
    OpcUa_ReturnErrorIfNull(a_pPKI->SaveCertificate, OpcUa_BadNotSupported);

    return a_pPKI->LoadCertificate(a_pPKI, a_pLoadHandle, a_pCertificateStore, a_pCertificate);
}

/*============================================================================
 * OpcUa_PKIProvider_SaveCertificate
 *===========================================================================*/
OpcUa_StatusCode OpcUa_PKIProvider_SaveCertificate(
    struct _OpcUa_PKIProvider*  a_pPKI,
    OpcUa_ByteString*           a_pCertificate,
    OpcUa_Void*                 a_pCertificateStore,
    OpcUa_Void*                 a_pSaveHandle)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_PkiProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPKI);
    OpcUa_ReturnErrorIfNull(a_pPKI->SaveCertificate, OpcUa_BadNotSupported);

    return a_pPKI->SaveCertificate(a_pPKI, a_pCertificate, a_pCertificateStore, a_pSaveHandle);
}
/*============================================================================
 * OpcUa_PKIProvider_CloseCertificateStore
 *===========================================================================*/
OpcUa_StatusCode OpcUa_PKIProvider_CloseCertificateStore(
    struct _OpcUa_PKIProvider*   a_pPKI,
    OpcUa_Void**                 a_ppCertificateStore) /* type depends on store implementation */
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_PkiProvider);
    OpcUa_ReturnErrorIfArgumentNull(a_pPKI);
    OpcUa_ReturnErrorIfNull(a_pPKI->CloseCertificateStore, OpcUa_BadNotSupported);

    return a_pPKI->CloseCertificateStore(a_pPKI, a_ppCertificateStore);
}
