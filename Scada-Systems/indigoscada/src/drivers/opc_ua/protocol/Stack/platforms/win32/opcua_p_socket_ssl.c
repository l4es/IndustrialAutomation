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

/* System Headers */
#include <stdlib.h>
#include <memory.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

/* platform layer includes */
#include <opcua_p_mutex.h>
#include <opcua_p_pkifactory.h>

/* own headers */
#include <opcua_p_socket.h>
#include <opcua_p_memory.h>
#include <opcua_p_socket_internal.h>
#include <opcua_p_socket_interface.h>
#include <opcua_p_socket_ssl.h>

#if OPCUA_P_SOCKETMANAGER_SUPPORT_SSL

#include <openssl/err.h>
#include <openssl/ssl.h>

/*============================================================================
 * The Ssl Socket Type
 *===========================================================================*/

/**
* Internal representation for a logical socket (client and server). Includes
* beside the system socket additional information for handling.
*/
struct _OpcUa_InternalSslSocket
{
    OpcUa_SocketServiceTable*        pSocketServiceTable;/* socket service table */
#if OPCUA_USE_SYNCHRONISATION
    OpcUa_Mutex                      pMutex;             /* critical section; synchronize object access */
#endif /* OPCUA_USE_SYNCHRONISATION */
    OpcUa_Socket_CertificateCallback pfnCertificateValidation;
    OpcUa_Socket_EventCallback       pfnEventCallback;   /* function to call on event */
    OpcUa_Void*                      pvUserData;         /* data for callback */
    OpcUa_ByteString*                pServerCertificate;
    OpcUa_Key*                       pServerPrivateKey;
    OpcUa_Void*                      pPKIConfig;
    SSL_CTX*                         pSslContext;
    SSL*                             pSslConnection;
    BIO*                             pRawBio;
    OpcUa_Socket                     pRawSocket;         /* underlying system socket */
    OpcUa_UInt                       bListenSocket:1;    /* is the socket opened in listen mode */
    OpcUa_UInt                       bWantShutdown:1;    /* is the socket already closing down  */
    OpcUa_UInt                       bWriteBlocked:1;    /* is an unfinished ssl write pending  */
    OpcUa_UInt                       bReadBlocked:1;     /* does the application refuse to read */
    OpcUa_UInt                       bSslProgress:1;     /* did the ssl protocol make progress  */
    OpcUa_UInt                       bSslError:1;        /* a fatal ssl protocol error occurred */
#ifndef OPENSSL_NO_DH
    DH*                              pDHparams;          /* optional DH param on listen socket  */
#endif /* OPENSSL_NO_DH */
};

typedef struct _OpcUa_InternalSslSocket OpcUa_InternalSslSocket;

/*============================================================================
 * Process the SSL Protocol
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SslSocket_DoStateMachine( OpcUa_InternalSslSocket* pInternalSocket,
                                                        OpcUa_Boolean            bCanCallBack,
                                                        OpcUa_Byte*              pWriteData,
                                                        OpcUa_UInt32*            pWriteSize)
{
    OpcUa_UInt32        nBytesRead;
    OpcUa_Int32         result;
    int                 ssl_result;
    int                 ssl_error;
    char*               buf;
    int                 wpending0;
    int                 wpending1;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslSocket_DoStateMachine");

    do
    {
        uStatus = OpcUa_Good;

        if(bCanCallBack)
        {
            result = BIO_nwrite0(pInternalSocket->pRawBio, &buf);
            if(result > 0)
            {
                uStatus = OpcUa_P_Socket_Read(pInternalSocket->pRawSocket,
                                              (OpcUa_Byte*)buf, result, &nBytesRead);
                if(OpcUa_IsGood(uStatus) && nBytesRead > 0)
                {
                    BIO_nwrite(pInternalSocket->pRawBio, NULL, nBytesRead);
                    uStatus = OpcUa_GoodCallAgain;
                }
                else
                {
                    if(OpcUa_IsNotEqual(OpcUa_BadWouldBlock))
                    {
                        result = BIO_shutdown_wr(pInternalSocket->pRawBio);
                    }
                    uStatus = OpcUa_Good;
                }
            }
        }

        if(pInternalSocket->bWantShutdown)
        {
            ssl_result = SSL_shutdown(pInternalSocket->pSslConnection);
            ssl_error = SSL_get_error(pInternalSocket->pSslConnection, ssl_result);
            if(ssl_result > 0 && BIO_nread0(pInternalSocket->pRawBio, NULL) > 0)
            {
                ssl_result = 0;
            }
            if(ssl_result != 0 && ssl_error != SSL_ERROR_WANT_READ
               && ssl_error != SSL_ERROR_WANT_WRITE)
            {
                OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
                OpcUa_GotoErrorWithStatus(OpcUa_BadDisconnect);
            }
        }
        else if(pWriteSize != OpcUa_Null && *pWriteSize > 0)
        {
            ssl_result = SSL_write(pInternalSocket->pSslConnection, pWriteData, *pWriteSize);
            ssl_error = SSL_get_error(pInternalSocket->pSslConnection, ssl_result);
            if(ssl_result > 0)
            {
                pInternalSocket->bSslProgress = OpcUa_True;
                pWriteData  += ssl_result;
                *pWriteSize -= ssl_result;
            }
            else if(ssl_error == SSL_ERROR_SSL || ssl_error == SSL_ERROR_SYSCALL)
            {
                pInternalSocket->bSslError = OpcUa_True;
                OpcUa_SslSocket_DoStateMachine(pInternalSocket, OpcUa_False,
                                               OpcUa_Null, OpcUa_Null);
                OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
                OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
            }
        }
        else if(bCanCallBack)
        {
            wpending0 = BIO_wpending(pInternalSocket->pRawBio);
            pInternalSocket->bSslProgress = OpcUa_False;
            if(pInternalSocket->bWriteBlocked)
            {
                pInternalSocket->bWriteBlocked = OpcUa_False;

#if OPCUA_USE_SYNCHRONISATION
                OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

                pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_WRITE_EVENT,
                                                  pInternalSocket->pvUserData, 0, OpcUa_True);

#if OPCUA_USE_SYNCHRONISATION
                OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
                if(pInternalSocket->bSslError)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
                }
            }
            if(!pInternalSocket->bReadBlocked)
            {
                pInternalSocket->bReadBlocked = OpcUa_True;

#if OPCUA_USE_SYNCHRONISATION
                OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

                pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_READ_EVENT,
                                                  pInternalSocket->pvUserData, 0, OpcUa_True);

#if OPCUA_USE_SYNCHRONISATION
                OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
                if(pInternalSocket->bSslError)
                {
                    OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
                }
            }
            wpending1 = BIO_wpending(pInternalSocket->pRawBio);
            if(wpending1 > 0 && (wpending0 > wpending1 || pInternalSocket->bSslProgress))
            {
                uStatus = OpcUa_GoodCallAgain;
            }
        }

        result = BIO_nread0(pInternalSocket->pRawBio, &buf);
        if(result > 0)
        {
            result = OpcUa_P_Socket_Write(pInternalSocket->pRawSocket,
                                          (OpcUa_Byte*)buf, result, OpcUa_False);
            if(result > 0)
            {
                BIO_nread(pInternalSocket->pRawBio, NULL, result);
                uStatus = OpcUa_GoodCallAgain;
            }
            else if(result < 0)
            {
                pInternalSocket->bSslError = OpcUa_True;
                OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
                OpcUa_GotoErrorWithStatus(OpcUa_BadDisconnect);
            }
        }
    }
    while(uStatus == OpcUa_GoodCallAgain);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Read SSL Socket.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_SslRead( OpcUa_Socket    a_pSocket,
                                                       OpcUa_Byte*     a_pBuffer,
                                                       OpcUa_UInt32    a_nBufferSize,
                                                       OpcUa_UInt32*   a_pBytesRead)
{
    OpcUa_InternalSslSocket*   pInternalSocket     = (OpcUa_InternalSslSocket*)a_pSocket;
    int                        ssl_result;
    int                        ssl_error;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslRead");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(a_pBuffer);
    OpcUa_GotoErrorIfArgumentNull(a_pBytesRead);

    *a_pBytesRead = 0;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(pInternalSocket->bListenSocket || pInternalSocket->bWantShutdown || pInternalSocket->bSslError)
    {
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    pInternalSocket->bReadBlocked = OpcUa_False;
    ssl_result = SSL_read(pInternalSocket->pSslConnection,
                          a_pBuffer, a_nBufferSize);
    ssl_error = SSL_get_error(pInternalSocket->pSslConnection, ssl_result);

    if(ssl_result > 0)
    {
        *a_pBytesRead = (OpcUa_UInt32)ssl_result;
        pInternalSocket->bSslProgress = OpcUa_True;
    }
    else
    {
        switch(ssl_error)
        {
            case SSL_ERROR_WANT_WRITE:
            case SSL_ERROR_WANT_READ:
                uStatus = OpcUa_BadWouldBlock;
                break;
            case SSL_ERROR_ZERO_RETURN:
                uStatus = OpcUa_BadDisconnect;
                break;
            case SSL_ERROR_SSL:
            case SSL_ERROR_SYSCALL:
                pInternalSocket->bSslError = OpcUa_True;
                OpcUa_SslSocket_DoStateMachine(pInternalSocket, OpcUa_False,
                                               OpcUa_Null, OpcUa_Null);
                OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
                uStatus = OpcUa_BadInternalError;
                break;
            default:
                uStatus = OpcUa_BadCommunicationError;
                break;
        }
    }

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Write SSL Socket.
 *===========================================================================*/
/* returns number of bytes written to the socket */
static OpcUa_Int32 OpcUa_P_SocketService_SslWrite( OpcUa_Socket    a_pSocket,
                                                   OpcUa_Byte*     a_pBuffer,
                                                   OpcUa_UInt32    a_uBufferSize,
                                                   OpcUa_Boolean   a_bBlock)
{
    OpcUa_InternalSslSocket*    pInternalSocket     = (OpcUa_InternalSslSocket*)a_pSocket;
    OpcUa_Int32                 result;
    OpcUa_UInt32                RemainingBufferSize = a_uBufferSize;

    OpcUa_ReturnErrorIfNull(a_pSocket, OPCUA_SOCKET_ERROR);
    OpcUa_ReturnErrorIfNull(a_pBuffer, OPCUA_SOCKET_ERROR);

    if(a_bBlock != OpcUa_False)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_P_Socket_Write: Blocking write not supported.\n");
        return OPCUA_SOCKET_ERROR;
    }

    if(a_uBufferSize == 0)
    {
        return OPCUA_SOCKET_ERROR;
    }

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(pInternalSocket->bListenSocket || pInternalSocket->bWantShutdown || pInternalSocket->bSslError)
    {
#if OPCUA_USE_SYNCHRONISATION
        OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        return OPCUA_SOCKET_ERROR;
    }

    OpcUa_SslSocket_DoStateMachine(pInternalSocket, OpcUa_False,
                                   a_pBuffer, &RemainingBufferSize);

    /* update size before returning */
    result = a_uBufferSize - RemainingBufferSize;
    pInternalSocket->bWriteBlocked = RemainingBufferSize > 0;

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    return result;
}

/*============================================================================
 * Close SSL Socket.
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_SslClose(OpcUa_Socket a_pSocket)
{
    OpcUa_InternalSslSocket* pInternalSocket = (OpcUa_InternalSslSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslClose");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(pInternalSocket->bListenSocket)
    {
        OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
    }
    else if(!pInternalSocket->bSslError)
    {
        /* Initiate SSL Shutdown */
        pInternalSocket->bWantShutdown = OpcUa_True;
        OpcUa_SslSocket_DoStateMachine(pInternalSocket, OpcUa_False,
                                       OpcUa_Null, OpcUa_Null);
    }

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get IP Address and Port Number of the Peer
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_SslGetPeerInfo(  OpcUa_Socket  a_pSocket,
                                                               OpcUa_CharA*  a_achPeerInfoBuffer,
                                                               OpcUa_UInt32  a_uiPeerInfoBufferSize)
{
    OpcUa_InternalSslSocket* pInternalSocket = (OpcUa_InternalSslSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslGetPeerInfo");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);
    OpcUa_GotoErrorIfArgumentNull(a_achPeerInfoBuffer);

    uStatus = OpcUa_P_Socket_GetPeerInfo(pInternalSocket->pRawSocket, a_achPeerInfoBuffer, a_uiPeerInfoBufferSize);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get last socket error
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_SslGetLastError(OpcUa_Socket a_pSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslGetLastError");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Set socket user data
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_P_SocketService_SslSetUserData(OpcUa_Socket a_pSocket,
                                                             OpcUa_Void*  a_pvUserData)
{
    OpcUa_InternalSocket*   pInternalSocket = (OpcUa_InternalSocket*)a_pSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslSetUserData");

    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    pInternalSocket->pvUserData = a_pvUserData;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Initialize Socket Service Table
 *===========================================================================*/
static OpcUa_SocketServiceTable OpcUa_SslSocketServiceTable =
{
  OpcUa_P_SocketService_SslRead,
  OpcUa_P_SocketService_SslWrite,
  OpcUa_P_SocketService_SslClose,
  OpcUa_P_SocketService_SslGetPeerInfo,
  OpcUa_P_SocketService_SslGetLastError,
  OpcUa_P_SocketService_SslSetUserData
};

/*============================================================================
 * Close and Delete the Ssl Socket
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SslSocket_InternalClose( OpcUa_InternalSslSocket* pInternalSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslSocket_InternalClose");

    pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_CLOSE_EVENT,
                                      pInternalSocket->pvUserData, 0, OpcUa_True);

    if(!pInternalSocket->bListenSocket)
    {
        BIO_free(pInternalSocket->pRawBio);
        SSL_free(pInternalSocket->pSslConnection);
        SSL_CTX_free(pInternalSocket->pSslContext);
    }
#ifndef OPENSSL_NO_DH
    else if(pInternalSocket->pDHparams != OpcUa_Null)
    {
        DH_free(pInternalSocket->pDHparams);
    }
#endif /* OPENSSL_NO_DH */

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Delete(&pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_P_Memory_Free(pInternalSocket);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Verify SSL Client Certificate
 *===========================================================================*/
static int OpcUa_SslSocket_VerifyCertificate( X509_STORE_CTX *ctx, void *arg)
{
    OpcUa_InternalSslSocket* pInternalSocket   = (OpcUa_InternalSslSocket*)arg;
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
    STACK_OF(X509)*          pChain            = X509_STORE_CTX_get0_untrusted(ctx);
#else
    STACK_OF(X509)*          pChain            = ctx->untrusted;
#endif
    int                      n;
    unsigned char*           p;
    OpcUa_StatusCode         uStatus;
    OpcUa_ByteString         ClientCert;
    OpcUa_PKIProvider        PKIProvider;
    OpcUa_Handle             hCertificateStore = OpcUa_Null;
    OpcUa_Int                validationCode    = X509_V_ERR_APPLICATION_VERIFICATION;

    ClientCert.Length = 0;
    for(n=0; n<sk_X509_num(pChain); n++)
    {
        ClientCert.Length += i2d_X509(sk_X509_value(pChain, n), OpcUa_Null);
    }

    ClientCert.Data = (OpcUa_Byte*)OpcUa_P_Memory_Alloc(ClientCert.Length);
    if(ClientCert.Data == OpcUa_Null)
    {
        X509_STORE_CTX_set_error(ctx, X509_V_ERR_OUT_OF_MEM);
        return -1;
    }

    p = ClientCert.Data;
    for(n=0; n<sk_X509_num(pChain); n++)
    {
        i2d_X509(sk_X509_value(pChain, n), &p);
    }

    OpcUa_MemSet(&PKIProvider, 0, sizeof(PKIProvider));
    uStatus = OpcUa_P_PKIFactory_CreatePKIProvider(pInternalSocket->pPKIConfig, &PKIProvider);
    if(OpcUa_IsGood(uStatus))
    {
        uStatus = PKIProvider.OpenCertificateStore(&PKIProvider, &hCertificateStore);
        if(OpcUa_IsGood(uStatus))
        {
            uStatus = PKIProvider.ValidateCertificate(&PKIProvider, &ClientCert, hCertificateStore,
                                                      &validationCode);
            PKIProvider.CloseCertificateStore(&PKIProvider, &hCertificateStore);
        }
        OpcUa_P_PKIFactory_DeletePKIProvider(&PKIProvider);
    }

    if(OpcUa_IsBad(uStatus))
    {
        if(validationCode == X509_V_OK)
        {
            validationCode = X509_V_ERR_APPLICATION_VERIFICATION;
        }
        if(pInternalSocket->pfnCertificateValidation != OpcUa_Null)
        {
            uStatus = pInternalSocket->pfnCertificateValidation(pInternalSocket, pInternalSocket->pvUserData,
                                                                &ClientCert, uStatus);
            if(OpcUa_IsEqual(OpcUa_BadContinue))
            {
                validationCode = X509_V_OK;
            }
        }
    }
    else
    {
        validationCode = X509_V_OK;
        if(pInternalSocket->pfnCertificateValidation != OpcUa_Null)
        {
            uStatus = pInternalSocket->pfnCertificateValidation(pInternalSocket, pInternalSocket->pvUserData,
                                                                &ClientCert, uStatus);
            if(OpcUa_IsBad(uStatus) && OpcUa_IsNotEqual(OpcUa_BadContinue))
            {
                validationCode = X509_V_ERR_APPLICATION_VERIFICATION;
            }
        }
    }

    ERR_clear_error();
    OpcUa_P_Memory_Free(ClientCert.Data);
    X509_STORE_CTX_set_error(ctx, validationCode);
    return validationCode == X509_V_OK ? 1 : 0;
}

/*============================================================================
 * Verify SSL Client Certificate
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SslSocket_InitializeSslContext( OpcUa_InternalSslSocket* pInternalSocket)
{
    EVP_PKEY*            pKey;
    X509*                pCert;
    const unsigned char* p;
    int                  result;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "InitializeSslContext");

    p = pInternalSocket->pServerPrivateKey->Key.Data;
    pKey = d2i_PrivateKey(EVP_PKEY_RSA, OpcUa_Null, &p,
                          pInternalSocket->pServerPrivateKey->Key.Length);
    if(pKey == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }
    result = SSL_CTX_use_PrivateKey(pInternalSocket->pSslContext, pKey);
    EVP_PKEY_free(pKey);
    if(result <= 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }

    p = pInternalSocket->pServerCertificate->Data;
    pCert = d2i_X509(OpcUa_Null, &p, pInternalSocket->pServerCertificate->Length);
    if(pCert == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }
    result = SSL_CTX_use_certificate(pInternalSocket->pSslContext, pCert);
    X509_free(pCert);
    if(result <= 0)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }

    while(p < pInternalSocket->pServerCertificate->Data + pInternalSocket->pServerCertificate->Length)
    {
        pCert = d2i_X509(OpcUa_Null, &p, pInternalSocket->pServerCertificate->Data
                         + pInternalSocket->pServerCertificate->Length - p);
        if(pCert == OpcUa_Null)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
        }
        result = SSL_CTX_add_extra_chain_cert(pInternalSocket->pSslContext, pCert);
        if(result <= 0)
        {
            X509_free(pCert);
            OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
        }
    }

    SSL_CTX_set_cert_verify_callback( pInternalSocket->pSslContext,
                                      OpcUa_SslSocket_VerifyCertificate,
                                      pInternalSocket);
    SSL_CTX_set_verify( pInternalSocket->pSslContext,
                        OPCUA_P_SOCKETMANAGER_SSL_VERIFY_OPTION,
                        OpcUa_Null);
    SSL_CTX_set_options( pInternalSocket->pSslContext,
                         OPCUA_P_SOCKETMANAGER_SSL_PROTOCOL_OPTION);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Accept a SSL server socket
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SslSocket_InternalAccept( OpcUa_InternalSslSocket* a_pInternalSocket,
                                                        OpcUa_Socket*            a_pRawSocket)
{
    OpcUa_InternalSslSocket* pInternalSocket = OpcUa_Null;
    BIO*                     pSslBio         = OpcUa_Null;
    int                      result;
#ifndef OPENSSL_NO_ECDH
    EC_KEY*                  ecdh;
#endif

OpcUa_InitializeStatus(OpcUa_Module_Socket, "InternalAccept");

    OpcUa_GotoErrorIfArgumentNull(a_pInternalSocket);
    OpcUa_GotoErrorIfArgumentNull(a_pRawSocket);
    OpcUa_GotoErrorIfTrue(!a_pInternalSocket->bListenSocket, OpcUa_BadInvalidArgument);

    pInternalSocket = (OpcUa_InternalSslSocket*)OpcUa_P_Memory_Alloc(sizeof(OpcUa_InternalSslSocket));
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket);

    OpcUa_MemSet(pInternalSocket, 0, sizeof(OpcUa_InternalSslSocket));

#if OPCUA_USE_SYNCHRONISATION
    uStatus = OpcUa_P_Mutex_Create(&pInternalSocket->pMutex);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_USE_SYNCHRONISATION */

    pInternalSocket->pSocketServiceTable      = &OpcUa_SslSocketServiceTable;
    pInternalSocket->pfnCertificateValidation = a_pInternalSocket->pfnCertificateValidation;
    pInternalSocket->pfnEventCallback         = a_pInternalSocket->pfnEventCallback;
    pInternalSocket->pvUserData               = a_pInternalSocket->pvUserData;
    pInternalSocket->pServerCertificate       = a_pInternalSocket->pServerCertificate;
    pInternalSocket->pServerPrivateKey        = a_pInternalSocket->pServerPrivateKey;
    pInternalSocket->pPKIConfig               = a_pInternalSocket->pPKIConfig;
    pInternalSocket->pRawSocket               = a_pRawSocket;
    pInternalSocket->bListenSocket            = OpcUa_False;
    pInternalSocket->bWantShutdown            = OpcUa_False;
    pInternalSocket->bWriteBlocked            = OpcUa_False;
    pInternalSocket->bReadBlocked             = OpcUa_False;
    pInternalSocket->bSslProgress             = OpcUa_False;
    pInternalSocket->bSslError                = OpcUa_False;

    pInternalSocket->pSslContext              = SSL_CTX_new(SSLv23_server_method());
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket->pSslContext);

    uStatus = OpcUa_SslSocket_InitializeSslContext(pInternalSocket);
    OpcUa_GotoErrorIfBad(uStatus);

#ifndef OPENSSL_NO_ECDH
    /* Enable Perfect Forward Secrecy, using EECDH. */
    ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if(ecdh != OpcUa_Null)
    {
        SSL_CTX_set_tmp_ecdh(pInternalSocket->pSslContext, ecdh);
        EC_KEY_free(ecdh);
    }
#endif /* OPENSSL_NO_ECDH */
#ifndef OPENSSL_NO_DH
    if(a_pInternalSocket->pDHparams != OpcUa_Null)
    {
        SSL_CTX_set_tmp_dh(pInternalSocket->pSslContext,
                           a_pInternalSocket->pDHparams);
        SSL_CTX_set_options(pInternalSocket->pSslContext,
                            SSL_OP_SINGLE_DH_USE);
    }
#endif /* OPENSSL_NO_DH */

    pInternalSocket->pSslConnection           = SSL_new(pInternalSocket->pSslContext);
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket->pSslConnection);

    pInternalSocket->pRawBio                  = BIO_new(BIO_s_bio());
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket->pRawBio);

    pSslBio                                   = BIO_new(BIO_s_bio());
    OpcUa_GotoErrorIfAllocFailed(pSslBio);

    result = BIO_make_bio_pair(pSslBio, pInternalSocket->pRawBio);
    OpcUa_GotoErrorIfTrue(result <= 0, OpcUa_BadInternalError);

    SSL_set_bio(pInternalSocket->pSslConnection, pSslBio, pSslBio);
    pSslBio = OpcUa_Null;
    SSL_set_accept_state(pInternalSocket->pSslConnection);

    uStatus = OpcUa_P_Socket_SetUserData(pInternalSocket->pRawSocket, pInternalSocket);
    OpcUa_GotoErrorIfBad(uStatus);

    pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_ACCEPT_EVENT,
                                      pInternalSocket->pvUserData, 0, OpcUa_True);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_P_Socket_SetUserData(a_pRawSocket, OpcUa_Null);
    OpcUa_P_Socket_Close(a_pRawSocket);

    if(pSslBio != OpcUa_Null)
    {
        BIO_free(pSslBio);
    }

    if(pInternalSocket != OpcUa_Null)
    {
        if(pInternalSocket->pRawBio != OpcUa_Null)
        {
            BIO_free(pInternalSocket->pRawBio);
        }

        if(pInternalSocket->pSslConnection != OpcUa_Null)
        {
            SSL_free(pInternalSocket->pSslConnection);
        }

        if(pInternalSocket->pSslContext != OpcUa_Null)
        {
            SSL_CTX_free(pInternalSocket->pSslContext);
        }

#if OPCUA_USE_SYNCHRONISATION
        if(pInternalSocket->pMutex != OpcUa_Null)
        {
            OpcUa_P_Mutex_Delete(&pInternalSocket->pMutex);
        }
#endif /* OPCUA_USE_SYNCHRONISATION */

        OpcUa_P_Memory_Free(pInternalSocket);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Read data from the Raw Socket
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SslSocket_InternalRead( OpcUa_InternalSslSocket* pInternalSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslSocket_InternalRead");

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_SslSocket_DoStateMachine(pInternalSocket, OpcUa_True,
                                   OpcUa_Null, OpcUa_Null);

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Write data to the Raw Socket
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_SslSocket_InternalWrite( OpcUa_InternalSslSocket* pInternalSocket)
{
OpcUa_InitializeStatus(OpcUa_Module_Socket, "SslSocket_InternalWrite");

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Lock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_SslSocket_DoStateMachine(pInternalSocket, OpcUa_True,
                                   OpcUa_Null, OpcUa_Null);

#if OPCUA_USE_SYNCHRONISATION
    OpcUa_P_Mutex_Unlock(pInternalSocket->pMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Handle Events from the Raw Socket
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_RawSocket_EventCallback( OpcUa_Socket   a_hSocket,
                                                       OpcUa_UInt32   a_uintSocketEvent,
                                                       OpcUa_Void*    a_pUserData,
                                                       OpcUa_UInt16   a_usPortNumber,
                                                       OpcUa_Boolean  a_bIsSSL)
{
    OpcUa_InternalSslSocket* pInternalSocket = (OpcUa_InternalSslSocket*)a_pUserData;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "RawSocket_EventCallback");

    OpcUa_GotoErrorIfArgumentNull(a_hSocket);
    OpcUa_GotoErrorIfArgumentNull(a_pUserData);
    OpcUa_ReferenceParameter(a_usPortNumber);
    OpcUa_ReferenceParameter(a_bIsSSL);

    switch(a_uintSocketEvent)
    {
    case OPCUA_SOCKET_READ_EVENT:
        {
            OpcUa_SslSocket_InternalRead(pInternalSocket);
            break;
        }
    case OPCUA_SOCKET_WRITE_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_RawSocket_EventCallback OPCUA_SOCKET_WRITE_EVENT\n");
            OpcUa_SslSocket_InternalWrite(pInternalSocket);
            break;
        }
    case OPCUA_SOCKET_CONNECT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_RawSocket_EventCallback: OPCUA_SOCKET_CONNECT_EVENT\n");
            pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_CONNECT_EVENT,
                                              pInternalSocket->pvUserData, 0, OpcUa_True);
            break;
        }
    case OPCUA_SOCKET_CLOSE_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_RawSocket_EventCallback: OPCUA_SOCKET_CLOSE_EVENT\n");
            OpcUa_SslSocket_InternalClose(pInternalSocket);
            break;
        }
    case OPCUA_SOCKET_TIMEOUT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_RawSocket_EventCallback: OPCUA_SOCKET_TIMEOUT_EVENT\n");
            pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_TIMEOUT_EVENT,
                                              pInternalSocket->pvUserData, 0, OpcUa_True);
            OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
            break;
        }
    case OPCUA_SOCKET_EXCEPT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_RawSocket_EventCallback: OPCUA_SOCKET_EXCEPT_EVENT\n");
            pInternalSocket->pfnEventCallback(pInternalSocket, OPCUA_SOCKET_EXCEPT_EVENT,
                                              pInternalSocket->pvUserData, 0, OpcUa_True);
            OpcUa_P_Socket_Close(pInternalSocket->pRawSocket);
            break;
        }
    case OPCUA_SOCKET_ACCEPT_EVENT:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_RawSocket_EventCallback: OPCUA_SOCKET_ACCEPT_EVENT\n");
            OpcUa_SslSocket_InternalAccept(pInternalSocket, a_hSocket);
            break;
        }
    default:
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_RawSocket_EventCallback: Unknown event!\n");
            break;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Create a SSL server socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateSslServer(  OpcUa_SocketManager              a_pSocketManager,
                                                                       OpcUa_StringA                    a_sAddress,
                                                                       OpcUa_Boolean                    a_bListenOnAllInterfaces,
                                                                       OpcUa_ByteString*                a_pServerCertificate,
                                                                       OpcUa_Key*                       a_pServerPrivateKey,
                                                                       OpcUa_Void*                      a_pPKIConfig,
                                                                       OpcUa_Socket_EventCallback       a_pfnSocketCallBack,
                                                                       OpcUa_Socket_CertificateCallback a_pfnCertificateCallBack,
                                                                       OpcUa_Void*                      a_pCallbackData,
                                                                       OpcUa_Socket*                    a_pSocket)
{
    OpcUa_InternalSslSocket* pInternalSocket = OpcUa_Null;
#ifndef OPENSSL_NO_DH
    OpcUa_StringA            pFileName;
    BIO*                     bio;
#endif /* OPENSSL_NO_DH */

OpcUa_InitializeStatus(OpcUa_Module_Socket, "CreateSslServer");

    OpcUa_GotoErrorIfArgumentNull(a_pServerCertificate);
    OpcUa_GotoErrorIfArgumentNull(a_pServerPrivateKey);
    OpcUa_GotoErrorIfArgumentNull(a_pfnSocketCallBack);
    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    if(a_pServerPrivateKey->Type != OpcUa_Crypto_KeyType_Rsa_Private
       || a_pServerPrivateKey->Key.Data == OpcUa_Null
       || a_pServerCertificate->Data == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    *a_pSocket = OpcUa_Null;
    pInternalSocket = (OpcUa_InternalSslSocket*)OpcUa_P_Memory_Alloc(sizeof(OpcUa_InternalSslSocket));
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket);

    OpcUa_MemSet(pInternalSocket, 0, sizeof(OpcUa_InternalSslSocket));

#if OPCUA_USE_SYNCHRONISATION
    uStatus = OpcUa_P_Mutex_Create(&pInternalSocket->pMutex);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_USE_SYNCHRONISATION */

    pInternalSocket->pSocketServiceTable      = &OpcUa_SslSocketServiceTable;
    pInternalSocket->pfnCertificateValidation = a_pfnCertificateCallBack;
    pInternalSocket->pfnEventCallback         = a_pfnSocketCallBack;
    pInternalSocket->pvUserData               = a_pCallbackData;
    pInternalSocket->pServerCertificate       = a_pServerCertificate;
    pInternalSocket->pServerPrivateKey        = a_pServerPrivateKey;
    pInternalSocket->pPKIConfig               = a_pPKIConfig;
    pInternalSocket->bListenSocket            = OpcUa_True;

#ifndef OPENSSL_NO_DH
    pInternalSocket->pDHparams                = OpcUa_Null;
    pFileName = OpcUa_P_PKIFactory_GetDHParamFileName(a_pPKIConfig);
    if(pFileName != OpcUa_Null)
    {
        bio = BIO_new_file(pFileName, "r");
        if(bio != OpcUa_Null)
        {
            pInternalSocket->pDHparams = PEM_read_bio_DHparams(bio, NULL, NULL, "");
            BIO_free(bio);
        }
    }
#endif /* OPENSSL_NO_DH */

    *a_pSocket = pInternalSocket;

    uStatus = OpcUa_P_SocketManager_CreateServer( a_pSocketManager,
                                                  a_sAddress,
                                                  a_bListenOnAllInterfaces,
                                                  OpcUa_RawSocket_EventCallback,
                                                  pInternalSocket,
                                                  &pInternalSocket->pRawSocket);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(a_pSocket != OpcUa_Null)
    {
        *a_pSocket = OpcUa_Null;
    }

    if(pInternalSocket != OpcUa_Null)
    {
#if OPCUA_USE_SYNCHRONISATION
        if(pInternalSocket->pMutex != OpcUa_Null)
        {
            OpcUa_P_Mutex_Delete(&pInternalSocket->pMutex);
        }
#endif /* OPCUA_USE_SYNCHRONISATION */

#ifndef OPENSSL_NO_DH
        if(pInternalSocket->pDHparams != OpcUa_Null)
        {
            DH_free(pInternalSocket->pDHparams);
        }
#endif /* OPENSSL_NO_DH */

        OpcUa_P_Memory_Free(pInternalSocket);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Create a SSL client socket
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_SocketManager_CreateSslClient(  OpcUa_SocketManager              a_pSocketManager,
                                                                       OpcUa_StringA                    a_sRemoteAddress,
                                                                       OpcUa_UInt16                     a_uLocalPort,
                                                                       OpcUa_ByteString*                a_pClientCertificate,
                                                                       OpcUa_Key*                       a_pClientPrivateKey,
                                                                       OpcUa_Void*                      a_pPKIConfig,
                                                                       OpcUa_Socket_EventCallback       a_pfnSocketCallBack,
                                                                       OpcUa_Socket_CertificateCallback a_pfnCertificateCallBack,
                                                                       OpcUa_Void*                      a_pCallbackData,
                                                                       OpcUa_Socket*                    a_pSocket)
{
    OpcUa_InternalSslSocket* pInternalSocket = OpcUa_Null;
    BIO*                     pSslBio         = OpcUa_Null;
    int                      result;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "CreateSslClient");

    OpcUa_GotoErrorIfArgumentNull(a_pClientCertificate);
    OpcUa_GotoErrorIfArgumentNull(a_pClientPrivateKey);
    OpcUa_GotoErrorIfArgumentNull(a_pfnSocketCallBack);
    OpcUa_GotoErrorIfArgumentNull(a_pSocket);

    if(a_pClientPrivateKey->Type != OpcUa_Crypto_KeyType_Rsa_Private
       || a_pClientPrivateKey->Key.Data == OpcUa_Null
       || a_pClientCertificate->Data == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }

    *a_pSocket = OpcUa_Null;
    pInternalSocket = (OpcUa_InternalSslSocket*)OpcUa_P_Memory_Alloc(sizeof(OpcUa_InternalSslSocket));
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket);

    OpcUa_MemSet(pInternalSocket, 0, sizeof(OpcUa_InternalSslSocket));

#if OPCUA_USE_SYNCHRONISATION
    uStatus = OpcUa_P_Mutex_Create(&pInternalSocket->pMutex);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_USE_SYNCHRONISATION */

    pInternalSocket->pSocketServiceTable      = &OpcUa_SslSocketServiceTable;
    pInternalSocket->pfnCertificateValidation = a_pfnCertificateCallBack;
    pInternalSocket->pfnEventCallback         = a_pfnSocketCallBack;
    pInternalSocket->pvUserData               = a_pCallbackData;
    pInternalSocket->pServerCertificate       = a_pClientCertificate;
    pInternalSocket->pServerPrivateKey        = a_pClientPrivateKey;
    pInternalSocket->pPKIConfig               = a_pPKIConfig;
    pInternalSocket->bListenSocket            = OpcUa_False;
    pInternalSocket->bWantShutdown            = OpcUa_False;
    pInternalSocket->bWriteBlocked            = OpcUa_False;
    pInternalSocket->bReadBlocked             = OpcUa_False;
    pInternalSocket->bSslProgress             = OpcUa_False;
    pInternalSocket->bSslError                = OpcUa_False;

    pInternalSocket->pSslContext              = SSL_CTX_new(SSLv23_client_method());
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket->pSslContext);

    uStatus = OpcUa_SslSocket_InitializeSslContext(pInternalSocket);
    OpcUa_GotoErrorIfBad(uStatus);

    pInternalSocket->pSslConnection           = SSL_new(pInternalSocket->pSslContext);
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket->pSslConnection);

    pInternalSocket->pRawBio                  = BIO_new(BIO_s_bio());
    OpcUa_GotoErrorIfAllocFailed(pInternalSocket->pRawBio);

    pSslBio                                   = BIO_new(BIO_s_bio());
    OpcUa_GotoErrorIfAllocFailed(pSslBio);

    result = BIO_make_bio_pair(pSslBio, pInternalSocket->pRawBio);
    OpcUa_GotoErrorIfTrue(result <= 0, OpcUa_BadInternalError);

    SSL_set_bio(pInternalSocket->pSslConnection, pSslBio, pSslBio);
    pSslBio = OpcUa_Null;
    SSL_set_connect_state(pInternalSocket->pSslConnection);

    *a_pSocket = pInternalSocket;

    uStatus = OpcUa_P_SocketManager_CreateClient( a_pSocketManager,
                                                  a_sRemoteAddress,
                                                  a_uLocalPort,
                                                  OpcUa_RawSocket_EventCallback,
                                                  pInternalSocket,
                                                  &pInternalSocket->pRawSocket);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(a_pSocket != OpcUa_Null)
    {
        *a_pSocket = OpcUa_Null;
    }

    if(pSslBio != OpcUa_Null)
    {
        BIO_free(pSslBio);
    }

    if(pInternalSocket != OpcUa_Null)
    {
        if(pInternalSocket->pRawBio != OpcUa_Null)
        {
            BIO_free(pInternalSocket->pRawBio);
        }

        if(pInternalSocket->pSslConnection != OpcUa_Null)
        {
            SSL_free(pInternalSocket->pSslConnection);
        }

        if(pInternalSocket->pSslContext != OpcUa_Null)
        {
            SSL_CTX_free(pInternalSocket->pSslContext);
        }

#if OPCUA_USE_SYNCHRONISATION
        if(pInternalSocket->pMutex != OpcUa_Null)
        {
            OpcUa_P_Mutex_Delete(&pInternalSocket->pMutex);
        }
#endif /* OPCUA_USE_SYNCHRONISATION */

        OpcUa_P_Memory_Free(pInternalSocket);
    }

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_P_SOCKETMANAGER_SUPPORT_SSL */
