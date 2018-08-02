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
#include <opcua_datetime.h>
#include <opcua_guid.h>
#include <opcua_mutex.h>

/* stackcore */
#include <opcua_securechannel.h>
#include <opcua_cryptofactory.h>
#include <opcua_stream.h>

/* security */
#include <opcua_tcpsecurechannel.h>

/*============================================================================
 * OpcUa_TcpSecureChannel_GetSecuritySet
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_GetSecuritySet( OpcUa_SecureChannel*    a_pSecureChannel,
                                                        OpcUa_UInt32            a_uTokenId,
                                                        OpcUa_SecurityKeyset**  a_ppReceivingKeyset,
                                                        OpcUa_SecurityKeyset**  a_ppSendingKeyset,
                                                        OpcUa_CryptoProvider**  a_ppCryptoProvider)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "GetSecuritySet");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "GetSecurityKeyset: Keysets for token id %u requested.\n", a_uTokenId);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    if(a_pSecureChannel->CurrentChannelSecurityToken.TokenId == a_uTokenId)
    {
        /* immediately activate the new keyset */
        a_pSecureChannel->bCurrentTokenActive = OpcUa_True;

        if(a_ppReceivingKeyset != OpcUa_Null)
        {
            *a_ppReceivingKeyset = a_pSecureChannel->pCurrentReceivingKeyset;
        }

        if(a_ppSendingKeyset != OpcUa_Null)
        {
            *a_ppSendingKeyset = a_pSecureChannel->pCurrentSendingKeyset;
        }

        if(a_ppCryptoProvider != OpcUa_Null)
        {
            *a_ppCryptoProvider = a_pSecureChannel->pCurrentCryptoProvider;
        }
    }
    else if(a_pSecureChannel->PreviousChannelSecurityToken.TokenId == a_uTokenId)
    {
        if(a_ppReceivingKeyset != OpcUa_Null)
        {
            *a_ppReceivingKeyset = a_pSecureChannel->pPreviousReceivingKeyset;
        }

        if(a_ppSendingKeyset != OpcUa_Null)
        {
            *a_ppSendingKeyset = a_pSecureChannel->pPreviousSendingKeyset;
        }

        if(a_ppCryptoProvider != OpcUa_Null)
        {
            *a_ppCryptoProvider = a_pSecureChannel->pPreviousCryptoProvider;
        }
    }
    else
    {
        uStatus = OpcUa_BadSecureChannelTokenUnknown;
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "GetSecurityKeyset: Token id %u at secure channel %u invalid!\n", a_uTokenId, a_pSecureChannel->SecureChannelId);
        OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_GetCurrentSecuritySet
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_GetCurrentSecuritySet(  OpcUa_SecureChannel*    a_pSecureChannel,
                                                                OpcUa_UInt32*           a_puTokenId,
                                                                OpcUa_SecurityKeyset**  a_ppReceivingKeyset,
                                                                OpcUa_SecurityKeyset**  a_ppSendingKeyset,
                                                                OpcUa_CryptoProvider**  a_ppCryptoProvider)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "GetCurrentSecuritySet");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    if(a_pSecureChannel->bCurrentTokenActive != OpcUa_False)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "GetCurrentSecuritySet: Current Keysets requested. Returned token id is %u\n", a_pSecureChannel->CurrentChannelSecurityToken.TokenId);

        if(a_puTokenId != OpcUa_Null)
        {
            *a_puTokenId = a_pSecureChannel->CurrentChannelSecurityToken.TokenId;
        }

        if(a_ppReceivingKeyset != OpcUa_Null)
        {
            *a_ppReceivingKeyset = a_pSecureChannel->pCurrentReceivingKeyset;
        }

        if(a_ppSendingKeyset != OpcUa_Null)
        {
            *a_ppSendingKeyset = a_pSecureChannel->pCurrentSendingKeyset;
        }

        if(a_ppCryptoProvider != OpcUa_Null)
        {
            *a_ppCryptoProvider = a_pSecureChannel->pCurrentCryptoProvider;
        }
    }
    else
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "GetCurrentSecuritySet: Current Keysets requested. Inactive. Returned token id is %u\n", a_pSecureChannel->PreviousChannelSecurityToken.TokenId);

        if(a_puTokenId != OpcUa_Null)
        {
            *a_puTokenId = a_pSecureChannel->PreviousChannelSecurityToken.TokenId;
        }

        if(a_ppReceivingKeyset != OpcUa_Null)
        {
            *a_ppReceivingKeyset = a_pSecureChannel->pPreviousReceivingKeyset;
        }

        if(a_ppSendingKeyset != OpcUa_Null)
        {
            *a_ppSendingKeyset = a_pSecureChannel->pPreviousSendingKeyset;
        }

        if(a_ppCryptoProvider != OpcUa_Null)
        {
            *a_ppCryptoProvider = a_pSecureChannel->pPreviousCryptoProvider;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_ReleaseSecuritySet
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_ReleaseSecuritySet( OpcUa_SecureChannel*    a_pSecureChannel,
                                                            OpcUa_UInt32            a_uTokenId)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "ReleaseSecuritySet");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OpcUa_ReferenceParameter(a_uTokenId);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "ReleaseSecurityKeyset: Keyset for token %u released.\n", a_uTokenId);

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_ReviseLifetime
 *===========================================================================*/
static OpcUa_Void OpcUa_TcpSecureChannel_ReviseLifetime(OpcUa_UInt32  a_uRequestedLifetime,
                                                        OpcUa_UInt32* a_puRevisedLifetime)
{
    if(a_uRequestedLifetime < OPCUA_SECURITYTOKEN_LIFETIME_MIN)
    {
        *a_puRevisedLifetime = OPCUA_SECURITYTOKEN_LIFETIME_MIN;
    }
    else if(a_uRequestedLifetime > OPCUA_SECURITYTOKEN_LIFETIME_MAX)
    {
        *a_puRevisedLifetime = OPCUA_SECURITYTOKEN_LIFETIME_MAX;
    }
    else
    {
        *a_puRevisedLifetime = a_uRequestedLifetime;
    }
}

/*============================================================================
 * OpcUa_TcpSecureChannel_GenerateSecurityToken
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_GenerateSecurityToken(  OpcUa_SecureChannel*         a_pSecureChannel,
                                                                OpcUa_UInt32                 a_tokenLifeTime,
                                                                OpcUa_ChannelSecurityToken** a_ppSecurityToken)
{
    OpcUa_ChannelSecurityToken* pSecurityToken      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "GenerateSecurityToken");

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel->Handle);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecurityToken);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    /* initialize outparameters */
    *a_ppSecurityToken  = OpcUa_Null;

    /*** create token ***/
    pSecurityToken = (OpcUa_ChannelSecurityToken*)OpcUa_Alloc(sizeof(OpcUa_ChannelSecurityToken));
    OpcUa_GotoErrorIfAllocFailed(pSecurityToken);

    OpcUa_ChannelSecurityToken_Initialize(pSecurityToken);

    pSecurityToken->ChannelId       = a_pSecureChannel->SecureChannelId;
    pSecurityToken->TokenId         = a_pSecureChannel->NextTokenId;
    pSecurityToken->CreatedAt       = OPCUA_P_DATETIME_UTCNOW();

    OpcUa_TcpSecureChannel_ReviseLifetime(a_tokenLifeTime, &pSecurityToken->RevisedLifetime);

    /*** increment next token id ***/
    a_pSecureChannel->NextTokenId++;

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpSecureChannel_GenerateSecurityToken: TOKEN ID is %u-%u\n", pSecurityToken->ChannelId, pSecurityToken->TokenId);
    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

    /* assign outparameter */
    *a_ppSecurityToken = pSecurityToken;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

    if(pSecurityToken != OpcUa_Null)
    {
        OpcUa_Free(pSecurityToken);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_RenewSecurityToken
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_RenewSecurityToken( OpcUa_SecureChannel*         a_pSecureChannel,
                                                            OpcUa_ChannelSecurityToken*  a_pSecurityToken,
                                                            OpcUa_UInt32                 a_tokenLifeTime,
                                                            OpcUa_ChannelSecurityToken** a_ppSecurityToken)
{
    OpcUa_ChannelSecurityToken* pSecurityToken      = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "RenewSecurityToken");

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecurityToken);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecurityToken);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    /* initialize outparameters */
    *a_ppSecurityToken = OpcUa_Null;

    /*** create token ***/
    pSecurityToken = (OpcUa_ChannelSecurityToken*)OpcUa_Alloc(sizeof(OpcUa_ChannelSecurityToken));
    OpcUa_GotoErrorIfAllocFailed(pSecurityToken);

    OpcUa_ChannelSecurityToken_Initialize(pSecurityToken);

    pSecurityToken->TokenId         = a_pSecureChannel->NextTokenId;
    pSecurityToken->ChannelId       = a_pSecurityToken->ChannelId;
    pSecurityToken->CreatedAt       = OPCUA_P_DATETIME_UTCNOW();

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpSecureChannel_RenewSecurityToken: TOKEN ID is %u-%u\n", pSecurityToken->ChannelId, pSecurityToken->TokenId);

    OpcUa_TcpSecureChannel_ReviseLifetime(a_tokenLifeTime, &pSecurityToken->RevisedLifetime);

    /* increment renew counter */
    a_pSecureChannel->NextTokenId++;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

    /* assign outparameter */
    *a_ppSecurityToken  = pSecurityToken;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

    if(pSecurityToken != OpcUa_Null)
    {
        OpcUa_Free(pSecurityToken);
    }

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_GetSequenceNumber
 *===========================================================================*/
OpcUa_UInt32 OpcUa_TcpSecureChannel_GetSequenceNumber(OpcUa_SecureChannel* a_pSecureChannel)
{
    OpcUa_UInt32 uSequenceNumber = 0;

    if(a_pSecureChannel == OpcUa_Null)
    {
        return 0;
    }

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    uSequenceNumber = ++a_pSecureChannel->uLastSequenceNumberSent;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

    return uSequenceNumber;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_CheckSequenceNumber
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_CheckSequenceNumber(OpcUa_SecureChannel* a_pSecureChannel,
                                                            OpcUa_UInt32         a_uSequenceNumber)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "CheckSequenceNumber");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    /* The SequenceNumber shall also monotonically increase for all Messages
       and shall not wrap around until it is greater than 4 294 966 271
       (UInt32.MaxValue - 1 024). The first number after the wrap around
       shall be less than 1 024. */
    if(!((a_uSequenceNumber == a_pSecureChannel->uLastSequenceNumberRcvd + 1) ||
         ((a_uSequenceNumber < 1024) &&
          (a_pSecureChannel->uLastSequenceNumberRcvd > 4294966271u))))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSequenceNumberInvalid);
    }

    a_pSecureChannel->uLastSequenceNumberRcvd = a_uSequenceNumber;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_LockWriteMutex
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_LockWriteMutex(OpcUa_SecureChannel* a_pSecureChannel)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "LockWriteMutex");

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OPCUA_SECURECHANNEL_LOCK_WRITE(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_UnlockWriteMutex
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_UnlockWriteMutex(OpcUa_SecureChannel* a_pSecureChannel)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "UnlockWriteMutex");

    /* check parameters */
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OPCUA_SECURECHANNEL_UNLOCK_WRITE(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureChannel_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Open(   OpcUa_SecureChannel*            a_pSecureChannel,
                                                OpcUa_Handle                    a_hTransportConnection,
                                                OpcUa_ChannelSecurityToken      a_channelSecurityToken,
                                                OpcUa_MessageSecurityMode       a_messageSecurityMode,
                                                OpcUa_ByteString*               a_pbsClientCertificate,
                                                OpcUa_ByteString*               a_pbsServerCertificate,
                                                OpcUa_SecurityKeyset*           a_pReceivingKeyset,
                                                OpcUa_SecurityKeyset*           a_pSendingKeyset,
                                                OpcUa_CryptoProvider*           a_pCryptoProvider)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "Open");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pCryptoProvider);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    if(a_pSecureChannel->SecureChannelId != a_channelSecurityToken.ChannelId)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    a_pSecureChannel->bCurrentTokenActive                           = OpcUa_True;

    /*** TCP SECURECHANNEL ***/
    a_pSecureChannel->CurrentChannelSecurityToken.ChannelId         = a_channelSecurityToken.ChannelId;
    a_pSecureChannel->CurrentChannelSecurityToken.TokenId           = a_channelSecurityToken.TokenId;
    a_pSecureChannel->CurrentChannelSecurityToken.CreatedAt         = a_channelSecurityToken.CreatedAt;
    a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime   = a_channelSecurityToken.RevisedLifetime;

    /*** SECURECHANNEL ***/
    a_pSecureChannel->State                                         = OpcUa_SecureChannelState_Opened;
    a_pSecureChannel->TransportConnection                           = a_hTransportConnection;
    a_pSecureChannel->MessageSecurityMode                           = a_messageSecurityMode;
    a_pSecureChannel->uExpirationCounter                            = (OpcUa_UInt32)(a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime/OPCUA_SECURELISTENER_WATCHDOG_INTERVAL);
    a_pSecureChannel->uOverlapCounter                               = (OpcUa_UInt32)((a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime>>2)/OPCUA_SECURELISTENER_WATCHDOG_INTERVAL);

    /* copy client certificate */
    if(a_pbsClientCertificate != OpcUa_Null && a_pbsClientCertificate->Length > 0)
    {
        a_pSecureChannel->ClientCertificate.Data    = (OpcUa_Byte *)OpcUa_Alloc(a_pbsClientCertificate->Length);
        OpcUa_GotoErrorIfAllocFailed(a_pSecureChannel->ClientCertificate.Data);
        a_pSecureChannel->ClientCertificate.Length  = a_pbsClientCertificate->Length;
        OpcUa_MemCpy(   a_pSecureChannel->ClientCertificate.Data,
                        a_pSecureChannel->ClientCertificate.Length,
                        a_pbsClientCertificate->Data,
                        a_pbsClientCertificate->Length);
    }
    else
    {
        OpcUa_ByteString_Initialize(&a_pSecureChannel->ClientCertificate);
    }

    if(a_pbsServerCertificate != OpcUa_Null && a_pbsServerCertificate->Length > 0)
    {
        a_pSecureChannel->ServerCertificate.Data    = (OpcUa_Byte*)OpcUa_Alloc(a_pbsServerCertificate->Length);
        OpcUa_GotoErrorIfAllocFailed(a_pSecureChannel->ServerCertificate.Data);
        a_pSecureChannel->ServerCertificate.Length  = a_pbsServerCertificate->Length;

        OpcUa_MemCpy(   a_pSecureChannel->ServerCertificate.Data,
                        a_pSecureChannel->ServerCertificate.Length,
                        a_pbsServerCertificate->Data,
                        a_pbsServerCertificate->Length);
    }
    else
    {
        OpcUa_ByteString_Initialize(&a_pSecureChannel->ServerCertificate);
    }

    a_pSecureChannel->pCurrentReceivingKeyset   = a_pReceivingKeyset;
    a_pSecureChannel->pCurrentSendingKeyset     = a_pSendingKeyset;
    a_pSecureChannel->pCurrentCryptoProvider    = a_pCryptoProvider;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_SecurityKeyset_Clear(a_pReceivingKeyset);
    OpcUa_SecurityKeyset_Clear(a_pSendingKeyset);
    OpcUa_Free(a_pReceivingKeyset);
    OpcUa_Free(a_pSendingKeyset);

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_SecureChannel_Renew
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Renew(  OpcUa_SecureChannel*            a_pSecureChannel,
                                                OpcUa_Handle                    a_hTransportConnection,
                                                OpcUa_ChannelSecurityToken      a_ChannelSecurityToken,
                                                OpcUa_MessageSecurityMode       a_eMessageSecurityMode,
                                                OpcUa_ByteString*               a_pbsClientCertificate,
                                                OpcUa_ByteString*               a_pbsServerCertificate,
                                                OpcUa_SecurityKeyset*           a_pNewReceivingKeyset,
                                                OpcUa_SecurityKeyset*           a_pNewSendingKeyset,
                                                OpcUa_CryptoProvider*           a_pNewCryptoProvider)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "Renew");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_hTransportConnection);
    OpcUa_ReturnErrorIfArgumentNull(a_pNewCryptoProvider);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_TcpSecureChannel_Renew: New token id for channel %u is %u\n", a_pSecureChannel->SecureChannelId, a_ChannelSecurityToken.TokenId);

    if(a_pSecureChannel->TransportConnection != a_hTransportConnection)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    if(a_pSecureChannel->SecureChannelId != a_ChannelSecurityToken.ChannelId)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    /*** TCP SECURECHANNEL ***/
    /* Previous objects will be overwritten by current objects */
    a_pSecureChannel->bCurrentTokenActive                           = OpcUa_False;

    /* set channelSecurityToken members */
    a_pSecureChannel->PreviousChannelSecurityToken.ChannelId        = a_pSecureChannel->CurrentChannelSecurityToken.ChannelId;
    a_pSecureChannel->PreviousChannelSecurityToken.TokenId          = a_pSecureChannel->CurrentChannelSecurityToken.TokenId;
    a_pSecureChannel->PreviousChannelSecurityToken.CreatedAt        = a_pSecureChannel->CurrentChannelSecurityToken.CreatedAt;
    a_pSecureChannel->PreviousChannelSecurityToken.RevisedLifetime  = a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime;

    /* set current channelSecurityToken */
    a_pSecureChannel->CurrentChannelSecurityToken.ChannelId         = a_ChannelSecurityToken.ChannelId;
    a_pSecureChannel->CurrentChannelSecurityToken.TokenId           = a_ChannelSecurityToken.TokenId;
    a_pSecureChannel->CurrentChannelSecurityToken.CreatedAt         = a_ChannelSecurityToken.CreatedAt;
    a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime   = a_ChannelSecurityToken.RevisedLifetime;

    /*** SECURECHANNEL ***/
    a_pSecureChannel->State                                         = OpcUa_SecureChannelState_Opened;
    a_pSecureChannel->MessageSecurityMode                           = a_eMessageSecurityMode;
    a_pSecureChannel->uExpirationCounter                            = (OpcUa_UInt32)(a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime/OPCUA_SECURELISTENER_WATCHDOG_INTERVAL);
    a_pSecureChannel->uOverlapCounter                               = (OpcUa_UInt32)((a_pSecureChannel->CurrentChannelSecurityToken.RevisedLifetime>>2)/OPCUA_SECURELISTENER_WATCHDOG_INTERVAL);

    /* free old certificate and add new one */
    OpcUa_ByteString_Clear(&a_pSecureChannel->ClientCertificate);

    /* copy client certificate!!! */
    if(a_pbsClientCertificate != OpcUa_Null && a_pbsClientCertificate->Length > 0)
    {
        a_pSecureChannel->ClientCertificate.Data    = (OpcUa_Byte*)OpcUa_Alloc(a_pbsClientCertificate->Length);
        OpcUa_GotoErrorIfAllocFailed(a_pSecureChannel->ClientCertificate.Data);
        a_pSecureChannel->ClientCertificate.Length  = a_pbsClientCertificate->Length;
        OpcUa_MemCpy(   a_pSecureChannel->ClientCertificate.Data,
                        a_pSecureChannel->ClientCertificate.Length,
                        a_pbsClientCertificate->Data,
                        a_pbsClientCertificate->Length);
    }

    /* free old certificate and add new one */
    OpcUa_ByteString_Clear(&a_pSecureChannel->ServerCertificate);

    if(a_pbsServerCertificate != OpcUa_Null && a_pbsServerCertificate->Length > 0)
    {
        a_pSecureChannel->ServerCertificate.Data    = (OpcUa_Byte*)OpcUa_Alloc(a_pbsServerCertificate->Length);
        OpcUa_GotoErrorIfAllocFailed(a_pSecureChannel->ServerCertificate.Data);
        a_pSecureChannel->ServerCertificate.Length  = a_pbsServerCertificate->Length;
        OpcUa_MemCpy(   a_pSecureChannel->ServerCertificate.Data,
                        a_pSecureChannel->ServerCertificate.Length,
                        a_pbsServerCertificate->Data,
                        a_pbsServerCertificate->Length);
    }

    /* delete previous keysets */
    OpcUa_SecurityKeyset_Clear(a_pSecureChannel->pPreviousReceivingKeyset);
    OpcUa_SecurityKeyset_Clear(a_pSecureChannel->pPreviousSendingKeyset);
    OpcUa_Free(a_pSecureChannel->pPreviousReceivingKeyset);
    OpcUa_Free(a_pSecureChannel->pPreviousSendingKeyset);
    a_pSecureChannel->pPreviousReceivingKeyset  = OpcUa_Null;
    a_pSecureChannel->pPreviousSendingKeyset    = OpcUa_Null;

    /* assign current to previous */
    a_pSecureChannel->pPreviousReceivingKeyset                      = a_pSecureChannel->pCurrentReceivingKeyset;
    a_pSecureChannel->pPreviousSendingKeyset                        = a_pSecureChannel->pCurrentSendingKeyset;

    /* delete previous cryptoprovider */
    if(     a_pSecureChannel->pPreviousCryptoProvider != OpcUa_Null
        &&  a_pSecureChannel->pPreviousCryptoProvider != a_pSecureChannel->pCurrentCryptoProvider)
    {
        OPCUA_P_CRYPTOFACTORY_DELETECRYPTOPROVIDER(a_pSecureChannel->pPreviousCryptoProvider);
        OpcUa_Free(a_pSecureChannel->pPreviousCryptoProvider);
        a_pSecureChannel->pPreviousCryptoProvider = OpcUa_Null;
    }
    else
    {
        a_pSecureChannel->pPreviousCryptoProvider = OpcUa_Null;
    }

    /* make current cryptoprovider previous */
    a_pSecureChannel->pPreviousCryptoProvider                       = a_pSecureChannel->pCurrentCryptoProvider;
    a_pSecureChannel->pCurrentCryptoProvider                        = a_pNewCryptoProvider;
    a_pSecureChannel->pCurrentReceivingKeyset                       = a_pNewReceivingKeyset;
    a_pSecureChannel->pCurrentSendingKeyset                         = a_pNewSendingKeyset;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_SecurityKeyset_Clear(a_pNewReceivingKeyset);
    OpcUa_SecurityKeyset_Clear(a_pNewSendingKeyset);
    OpcUa_Free(a_pNewReceivingKeyset);
    OpcUa_Free(a_pNewSendingKeyset);

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Close(OpcUa_SecureChannel*   a_pSecureChannel)
{
    OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "Close");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);

    OPCUA_SECURECHANNEL_LOCK(a_pSecureChannel);

    a_pSecureChannel->State = OpcUa_SecureChannelState_Closed;

    OPCUA_SECURECHANNEL_UNLOCK(a_pSecureChannel);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Create(OpcUa_SecureChannel** a_ppSecureChannel)
{
    OpcUa_TcpSecureChannel* pTcpSecureChannel = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "Create");

    *a_ppSecureChannel = OpcUa_Null;

    pTcpSecureChannel = (OpcUa_TcpSecureChannel*)OpcUa_Alloc(sizeof(OpcUa_TcpSecureChannel));
    OpcUa_ReturnErrorIfAllocFailed(pTcpSecureChannel);
    OpcUa_MemSet(pTcpSecureChannel, 0, sizeof(OpcUa_TcpSecureChannel));

    *a_ppSecureChannel = (OpcUa_SecureChannel*)OpcUa_Alloc(sizeof(OpcUa_SecureChannel));
    OpcUa_GotoErrorIfAllocFailed(*a_ppSecureChannel);
    OpcUa_MemSet(*a_ppSecureChannel, 0, sizeof(OpcUa_SecureChannel));

    (*a_ppSecureChannel)->SecureChannelId           = OPCUA_SECURECHANNEL_ID_INVALID;
    (*a_ppSecureChannel)->NextTokenId               = 1;
    (*a_ppSecureChannel)->uLastSequenceNumberRcvd   = 0xFFFFFFFFU;
    (*a_ppSecureChannel)->uLastSequenceNumberSent   = OPCUA_SECURECHANNEL_STARTING_SEQUENCE_NUMBER;
    (*a_ppSecureChannel)->Handle                    = pTcpSecureChannel;
    (*a_ppSecureChannel)->Open                      = OpcUa_TcpSecureChannel_Open;
    (*a_ppSecureChannel)->Renew                     = OpcUa_TcpSecureChannel_Renew;
    (*a_ppSecureChannel)->Close                     = OpcUa_TcpSecureChannel_Close;
    (*a_ppSecureChannel)->GenerateSecurityToken     = OpcUa_TcpSecureChannel_GenerateSecurityToken;
    (*a_ppSecureChannel)->RenewSecurityToken        = OpcUa_TcpSecureChannel_RenewSecurityToken;
    (*a_ppSecureChannel)->GetSecuritySet            = OpcUa_TcpSecureChannel_GetSecuritySet;
    (*a_ppSecureChannel)->GetCurrentSecuritySet     = OpcUa_TcpSecureChannel_GetCurrentSecuritySet;
    (*a_ppSecureChannel)->ReleaseSecuritySet        = OpcUa_TcpSecureChannel_ReleaseSecuritySet;
    (*a_ppSecureChannel)->GetSequenceNumber         = OpcUa_TcpSecureChannel_GetSequenceNumber;
    (*a_ppSecureChannel)->CheckSequenceNumber       = OpcUa_TcpSecureChannel_CheckSequenceNumber;
    (*a_ppSecureChannel)->LockWriteMutex            = OpcUa_TcpSecureChannel_LockWriteMutex;
    (*a_ppSecureChannel)->UnlockWriteMutex          = OpcUa_TcpSecureChannel_UnlockWriteMutex;
    (*a_ppSecureChannel)->IsOpen                    = OpcUa_SecureChannel_IsOpen;
    (*a_ppSecureChannel)->DiscoveryOnly             = OpcUa_False;
    (*a_ppSecureChannel)->MessageSecurityMode       = OpcUa_MessageSecurityMode_None;

    uStatus = OPCUA_P_MUTEX_CREATE(&((*a_ppSecureChannel)->hSyncAccess));
    OpcUa_GotoErrorIfBad(uStatus);
    uStatus = OPCUA_P_MUTEX_CREATE(&((*a_ppSecureChannel)->hWriteMutex));
    OpcUa_GotoErrorIfBad(uStatus);
    OpcUa_String_Initialize(&((*a_ppSecureChannel)->SecurityPolicyUri));
    OpcUa_String_Initialize(&((*a_ppSecureChannel)->sPeerInfo));

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if (*a_ppSecureChannel != OpcUa_Null)
    {
        if((*a_ppSecureChannel)->hSyncAccess != OpcUa_Null)
        {
            OPCUA_P_MUTEX_DELETE(&((*a_ppSecureChannel)->hSyncAccess));
        }

        if((*a_ppSecureChannel)->hWriteMutex != OpcUa_Null)
        {
            OPCUA_P_MUTEX_DELETE(&((*a_ppSecureChannel)->hWriteMutex));
        }

        OpcUa_Free(*a_ppSecureChannel);
        *a_ppSecureChannel = OpcUa_Null;
    }

    OpcUa_Free(pTcpSecureChannel);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_Clear
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Clear(OpcUa_SecureChannel* a_pSecureChannel)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "Clear");

    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pSecureChannel->Handle);

    OpcUa_String_Clear(&a_pSecureChannel->SecurityPolicyUri);
    OpcUa_String_Clear(&a_pSecureChannel->sPeerInfo);

    OpcUa_Free(a_pSecureChannel->Handle);
    a_pSecureChannel->Handle = OpcUa_Null;

    OpcUa_ByteString_Clear(&a_pSecureChannel->ClientCertificate);

    OpcUa_ByteString_Clear(&a_pSecureChannel->ServerCertificate);

    if(a_pSecureChannel->pCurrentReceivingKeyset != OpcUa_Null)
    {
        OpcUa_SecurityKeyset_Clear(a_pSecureChannel->pCurrentReceivingKeyset);
        OpcUa_Free(a_pSecureChannel->pCurrentReceivingKeyset);
        a_pSecureChannel->pCurrentReceivingKeyset = OpcUa_Null;
    }

    if(a_pSecureChannel->pCurrentSendingKeyset != OpcUa_Null)
    {
        OpcUa_SecurityKeyset_Clear(a_pSecureChannel->pCurrentSendingKeyset);
        OpcUa_Free(a_pSecureChannel->pCurrentSendingKeyset);
        a_pSecureChannel->pCurrentSendingKeyset = OpcUa_Null;
    }

    if(a_pSecureChannel->pPreviousReceivingKeyset != OpcUa_Null)
    {
        OpcUa_SecurityKeyset_Clear(a_pSecureChannel->pPreviousReceivingKeyset);
        OpcUa_Free(a_pSecureChannel->pPreviousReceivingKeyset);
        a_pSecureChannel->pPreviousReceivingKeyset = OpcUa_Null;
    }

    if(a_pSecureChannel->pPreviousSendingKeyset != OpcUa_Null)
    {
        OpcUa_SecurityKeyset_Clear(a_pSecureChannel->pPreviousSendingKeyset);
        OpcUa_Free(a_pSecureChannel->pPreviousSendingKeyset);
        a_pSecureChannel->pPreviousSendingKeyset = OpcUa_Null;
    }

    /* delete both crypto providers */
    if(a_pSecureChannel->pCurrentCryptoProvider != OpcUa_Null)
    {
        if(a_pSecureChannel->pPreviousCryptoProvider == a_pSecureChannel->pCurrentCryptoProvider)
        {
            /* prevent double deletion */
            a_pSecureChannel->pPreviousCryptoProvider = OpcUa_Null;
        }

        OPCUA_P_CRYPTOFACTORY_DELETECRYPTOPROVIDER(a_pSecureChannel->pCurrentCryptoProvider);
        OpcUa_Free(a_pSecureChannel->pCurrentCryptoProvider);
        a_pSecureChannel->pCurrentCryptoProvider = OpcUa_Null;
    }

    if(a_pSecureChannel->pPreviousCryptoProvider != OpcUa_Null)
    {
        OPCUA_P_CRYPTOFACTORY_DELETECRYPTOPROVIDER(a_pSecureChannel->pPreviousCryptoProvider);
        OpcUa_Free(a_pSecureChannel->pPreviousCryptoProvider);
        a_pSecureChannel->pPreviousCryptoProvider = OpcUa_Null;
    }

    if(a_pSecureChannel->pPendingSecureIStream != OpcUa_Null)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)(a_pSecureChannel->pPendingSecureIStream));
        OpcUa_Stream_Delete((OpcUa_Stream**)&(a_pSecureChannel->pPendingSecureIStream));
    }

    while(a_pSecureChannel->pPendingSendBuffers != OpcUa_Null)
    {
        OpcUa_BufferList* pCurrentBuffer = a_pSecureChannel->pPendingSendBuffers;
        a_pSecureChannel->pPendingSendBuffers = pCurrentBuffer->pNext;
        OpcUa_Buffer_Clear(&pCurrentBuffer->Buffer);
        OpcUa_Free(pCurrentBuffer);
    }

    if(a_pSecureChannel->hSyncAccess != OpcUa_Null)
    {
        OPCUA_P_MUTEX_DELETE(&(a_pSecureChannel->hSyncAccess));
        a_pSecureChannel->hSyncAccess = OpcUa_Null;
    }

    if(a_pSecureChannel->hWriteMutex != OpcUa_Null)
    {
        OPCUA_P_MUTEX_DELETE(&(a_pSecureChannel->hWriteMutex));
        a_pSecureChannel->hWriteMutex = OpcUa_Null;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_TcpSecureChannel_Delete
 *===========================================================================*/
OpcUa_StatusCode OpcUa_TcpSecureChannel_Delete(OpcUa_SecureChannel**   a_ppSecureChannel)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureChannel, "Delete");

    if (a_ppSecureChannel != OpcUa_Null && *a_ppSecureChannel != OpcUa_Null)
    {
        OpcUa_TcpSecureChannel_Clear(*a_ppSecureChannel);
        OpcUa_Free(*a_ppSecureChannel);
        *a_ppSecureChannel = OpcUa_Null;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}
