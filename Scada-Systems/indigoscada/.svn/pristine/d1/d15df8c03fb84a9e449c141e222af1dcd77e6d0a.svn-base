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

#include <opcua.h>
#include <opcua_stream.h>
#include <opcua_securechannel_types.h>
#include <opcua_securechannel.h>
#include <opcua_securestream.h>

/*============================================================================
 * OpcUa_SecureChannel_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_Open(
    OpcUa_SecureChannel*            a_pSecureChannel,
    OpcUa_Handle                    a_hTransportConnection,
    OpcUa_ChannelSecurityToken      a_channelSecurityToken,
    OpcUa_MessageSecurityMode       a_messageSecurityMode,
    OpcUa_ByteString*               a_clientCertificate,
    OpcUa_ByteString*               a_serverCertificate,
    OpcUa_SecurityKeyset*           a_pReceivingKeyset,
    OpcUa_SecurityKeyset*           a_pSendingKeyset,
    OpcUa_CryptoProvider*           a_pCryptoProvider)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_SecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    return a_pSecureChannel->Open(  a_pSecureChannel,
                                    a_hTransportConnection,
                                    a_channelSecurityToken,
                                    a_messageSecurityMode,
                                    a_clientCertificate,
                                    a_serverCertificate,
                                    a_pReceivingKeyset,
                                    a_pSendingKeyset,
                                    a_pCryptoProvider);
}

/*============================================================================
 * OpcUa_SecureChannel_Renew
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_Renew(
    OpcUa_SecureChannel*            a_pSecureChannel,
    OpcUa_Handle                    a_hTransportConnection,
    OpcUa_ChannelSecurityToken      a_channelSecurityToken,
    OpcUa_MessageSecurityMode       a_messageSecurityMode,
    OpcUa_ByteString*               a_clientCertificate,
    OpcUa_ByteString*               a_serverCertificate,
    OpcUa_SecurityKeyset*           a_pReceivingKeyset,
    OpcUa_SecurityKeyset*           a_pSendingKeyset,
    OpcUa_CryptoProvider*           a_pCryptoProvider)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_SecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    return a_pSecureChannel->Renew( a_pSecureChannel,
                                    a_hTransportConnection,
                                    a_channelSecurityToken,
                                    a_messageSecurityMode,
                                    a_clientCertificate,
                                    a_serverCertificate,
                                    a_pReceivingKeyset,
                                    a_pSendingKeyset,
                                    a_pCryptoProvider);
}

/*============================================================================
 * OpcUa_SecureChannel_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_Close( OpcUa_SecureChannel*   a_pSecureChannel)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_SecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    return a_pSecureChannel->Close(a_pSecureChannel);
}

/*============================================================================
 * OpcUa_SecureChannel_GenerateSecurityToken
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_GenerateSecurityToken(
    OpcUa_SecureChannel*            a_pSecureChannel,
    OpcUa_UInt32                     a_tokenLifeTime,
    OpcUa_ChannelSecurityToken**    a_ppSecurityToken)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_SecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    return a_pSecureChannel->GenerateSecurityToken( a_pSecureChannel,
                                                    a_tokenLifeTime,
                                                    a_ppSecurityToken);
}

/*============================================================================
 * OpcUa_SecureChannel_RenewSecurityToken
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_RenewSecurityToken(
    OpcUa_SecureChannel*            a_pSecureChannel,
    OpcUa_ChannelSecurityToken*     a_pSecurityToken,
    OpcUa_UInt32                     a_tokenLifeTime,
    OpcUa_ChannelSecurityToken**    a_ppSecurityToken)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_SecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecurityToken);

    return a_pSecureChannel->RenewSecurityToken(a_pSecureChannel,
                                                a_pSecurityToken,
                                                a_tokenLifeTime,
                                                a_ppSecurityToken);
}

/*============================================================================
 * OpcUa_SecureChannel_GetPendingInputStream
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_GetPendingInputStream(
    OpcUa_SecureChannel* a_pSecureChannel,
    OpcUa_InputStream**  a_ppSecureIStream)
{
    OpcUa_ReferenceParameter(a_pSecureChannel);
    OpcUa_ReferenceParameter(a_ppSecureIStream);

    *a_ppSecureIStream = a_pSecureChannel->pPendingSecureIStream;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_SecureChannel_SetPendingInputStream
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SecureChannel_SetPendingInputStream(
    OpcUa_SecureChannel*    a_pSecureChannel,
    OpcUa_InputStream*      a_pSecureIStream)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "SetPendingInputStream");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    a_pSecureChannel->pPendingSecureIStream = a_pSecureIStream;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureChannel_DeriveKeys
 *===========================================================================*/
/* static helper function */
OpcUa_StatusCode OpcUa_SecureChannel_DeriveKeys(    OpcUa_MessageSecurityMode   uSecurityMode,
                                                    OpcUa_CryptoProvider*       pCryptoProvider,
                                                    OpcUa_ByteString*           pClientNonce,
                                                    OpcUa_ByteString*           pServerNonce,
                                                    OpcUa_SecurityKeyset**      ppClientKeyset,
                                                    OpcUa_SecurityKeyset**      ppServerKeyset)
{
    OpcUa_SecurityKeyset*               pClientKeyset               = OpcUa_Null;
    OpcUa_SecurityKeyset*               pServerKeyset               = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "DeriveKeys");

    /* allocate memory for the session key structures */
    pClientKeyset = (OpcUa_SecurityKeyset*)OpcUa_Alloc(sizeof(OpcUa_SecurityKeyset));
    OpcUa_GotoErrorIfAllocFailed(pClientKeyset);

    OpcUa_SecurityKeyset_Initialize(pClientKeyset);

    pServerKeyset = (OpcUa_SecurityKeyset*)OpcUa_Alloc(sizeof(OpcUa_SecurityKeyset));
    OpcUa_GotoErrorIfAllocFailed(pServerKeyset);

    OpcUa_SecurityKeyset_Initialize(pServerKeyset);

    if((uSecurityMode == OpcUa_MessageSecurityMode_SignAndEncrypt) || (uSecurityMode == OpcUa_MessageSecurityMode_Sign))
    {
        /* get the lengths for the keys */
        uStatus = pCryptoProvider->DeriveChannelKeysets(    pCryptoProvider,
                                                            *pClientNonce,  /* client nonce */
                                                            *pServerNonce,  /* server nonce */
                                                            -1,             /* key size default */
                                                            pClientKeyset,  /* outparam */
                                                            pServerKeyset); /* outparam */
        OpcUa_GotoErrorIfBad(uStatus);

        /* allocate memory for keys in the keyset */
        /* client keys */
        pClientKeyset->SigningKey.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pClientKeyset->SigningKey.Key.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pClientKeyset->SigningKey.Key.Data);

        pClientKeyset->EncryptionKey.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pClientKeyset->EncryptionKey.Key.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pClientKeyset->EncryptionKey.Key.Data);

        pClientKeyset->InitializationVector.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pClientKeyset->InitializationVector.Key.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pClientKeyset->InitializationVector.Key.Data);

        /* server keys */
        pServerKeyset->SigningKey.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pServerKeyset->SigningKey.Key.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pServerKeyset->SigningKey.Key.Data);

        pServerKeyset->EncryptionKey.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pServerKeyset->EncryptionKey.Key.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pServerKeyset->EncryptionKey.Key.Data);

        pServerKeyset->InitializationVector.Key.Data = (OpcUa_Byte*)OpcUa_Alloc(pServerKeyset->InitializationVector.Key.Length*sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pServerKeyset->InitializationVector.Key.Data);

        /* derive keys with settings from cryptoprovider */
        uStatus = pCryptoProvider->DeriveChannelKeysets(    pCryptoProvider,
                                                            *pClientNonce,  /* client nonce */
                                                            *pServerNonce,  /* server nonce */
                                                            -1,             /* key size default */
                                                            pClientKeyset,  /* outparam */
                                                            pServerKeyset); /* outparam */
        OpcUa_GotoErrorIfBad(uStatus);
    }
    else
    {
        /*** OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_NONE ***/

        /* generate fake client and server keyset */
        pClientKeyset->SigningKey.Type                  = OpcUa_Crypto_KeyType_Symmetric;
        pClientKeyset->SigningKey.Key.Length            = 1;
        pClientKeyset->SigningKey.Key.Data              = (OpcUa_Byte*)OpcUa_Alloc(sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pClientKeyset->SigningKey.Key.Data);

        pClientKeyset->EncryptionKey.Type               = OpcUa_Crypto_KeyType_Symmetric;
        pClientKeyset->EncryptionKey.Key.Length         = 1;
        pClientKeyset->EncryptionKey.Key.Data           = (OpcUa_Byte*)OpcUa_Alloc(sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pClientKeyset->EncryptionKey.Key.Data);

        pClientKeyset->InitializationVector.Type        = OpcUa_Crypto_KeyType_Random;
        pClientKeyset->InitializationVector.Key.Length  = 1;
        pClientKeyset->InitializationVector.Key.Data    = (OpcUa_Byte*)OpcUa_Alloc(sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pClientKeyset->InitializationVector.Key.Data);

        pServerKeyset->SigningKey.Type                  = OpcUa_Crypto_KeyType_Symmetric;
        pServerKeyset->SigningKey.Key.Length            = 1;
        pServerKeyset->SigningKey.Key.Data              = (OpcUa_Byte*)OpcUa_Alloc(sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pServerKeyset->SigningKey.Key.Data);

        pServerKeyset->EncryptionKey.Type               = OpcUa_Crypto_KeyType_Symmetric;
        pServerKeyset->EncryptionKey.Key.Length         = 1;
        pServerKeyset->EncryptionKey.Key.Data           = (OpcUa_Byte*)OpcUa_Alloc(sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pServerKeyset->EncryptionKey.Key.Data);

        pServerKeyset->InitializationVector.Type        = OpcUa_Crypto_KeyType_Random;
        pServerKeyset->InitializationVector.Key.Length  = 1;
        pServerKeyset->InitializationVector.Key.Data    = (OpcUa_Byte*)OpcUa_Alloc(sizeof(OpcUa_Byte));
        OpcUa_GotoErrorIfAllocFailed(pServerKeyset->InitializationVector.Key.Data);
   }

    *ppClientKeyset = pClientKeyset;
    *ppServerKeyset = pServerKeyset;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_SecurityKeyset_Clear(pServerKeyset);
    OpcUa_Free(pServerKeyset);
    OpcUa_SecurityKeyset_Clear(pClientKeyset);
    OpcUa_Free(pClientKeyset);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureChannel_IsOpen
 *===========================================================================*/
OpcUa_Boolean OpcUa_SecureChannel_IsOpen(OpcUa_SecureChannel* a_pSecureChannel)
{
    if(a_pSecureChannel != OpcUa_Null)
    {
        return (a_pSecureChannel->State == OpcUa_SecureChannelState_Opened)?OpcUa_True:OpcUa_False;
    }
    else
    {
        return OpcUa_False;
    }
}
