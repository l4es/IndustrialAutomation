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

#ifdef OPCUA_HAVE_SERVERAPI

/* core extended */
#include <opcua_datetime.h>
#include <opcua_guid.h>
#include <opcua_list.h>
#include <opcua_timer.h>
#include <opcua_mutex.h>

/* stackcore */
#include <opcua_securechannel.h>

/* security */
#include <opcua_tcpsecurechannel.h>
#include <opcua_soapsecurechannel.h>
#include <opcua_securelistener_channelmanager.h>


/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager                                              */
/*==============================================================================*/
/**
* @brief Being part of a specific SecureListener, it manages the secure channel and connections.
*/
struct _OpcUa_SecureListener_ChannelManager
{
    /* @brief A list with current secure connections of type OpcUa_TcpSecureChannel. */
    OpcUa_List*                                                 SecureChannels;
    /* @brief Timer which periodically checks the secure channels for expired lifetimes. */
    OpcUa_Timer                                                 hLifeTimeWatchDog;
    /* @brief Called if a channel gets removed due timeout. */
    OpcUa_SecureListener_ChannelManager_ChannelRemovedCallback* pfCallback;
    /* @brief Cookie for the channel removed callback. */
    OpcUa_Void*                                                 pvCallbackData;
};

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_TimerCallback                            */
/*==============================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SecureListener_ChannelManager_TimerCallback(   OpcUa_Void*     a_pvCallbackData,
                                                                                    OpcUa_Timer     a_hTimer,
                                                                                    OpcUa_UInt32    a_msecElapsed)
{
    OpcUa_SecureListener_ChannelManager*    pChannelManager     = (OpcUa_SecureListener_ChannelManager*)a_pvCallbackData;
    OpcUa_SecureChannel*                    pTmpSecureChannel   = OpcUa_Null;
    OpcUa_UInt32                            nToDelete           = 0;
    OpcUa_ListElement*                      pTmpListFirst       = OpcUa_Null;
    OpcUa_ListElement*                      pTmpListLast        = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_TimerCallback");

    OpcUa_ReferenceParameter(a_hTimer);
    OpcUa_ReferenceParameter(a_msecElapsed);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelManager_TimerCallback: Checking Channels for lifetime expiration!\n");

    if(     OpcUa_Null != pChannelManager
        &&  OpcUa_Null != pChannelManager->SecureChannels)
    {
        /* remove all channels and delete list */
        OpcUa_List_Enter(pChannelManager->SecureChannels);

        OpcUa_List_ResetCurrent(pChannelManager->SecureChannels);
        pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(pChannelManager->SecureChannels);

        /* check all channels for timeout */
        while(pTmpSecureChannel != OpcUa_Null)
        {
            /* Each SecureChannel exists until it is explicitly closed or
               until the last token has expired and the overlap period has elapsed. */

            OPCUA_SECURECHANNEL_LOCK(pTmpSecureChannel);
            if(pTmpSecureChannel->State == OpcUa_SecureChannelState_Closed && pTmpSecureChannel->uRefCount == 0)
            {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelManager_TimerCallback: removing SecureChannel %u after it was closed!\n", pTmpSecureChannel->SecureChannelId);
                OpcUa_List_DeleteCurrentElement(pChannelManager->SecureChannels);
                OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                OpcUa_TcpSecureChannel_Delete(&pTmpSecureChannel);
                pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(pChannelManager->SecureChannels);
            }
            else if(pTmpSecureChannel->State == OpcUa_SecureChannelState_Opened)
            {
                if(pTmpSecureChannel->uExpirationCounter != 0)
                {
                    pTmpSecureChannel->uExpirationCounter--;
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetNextElement(pChannelManager->SecureChannels);
                }
                else if(pTmpSecureChannel->uOverlapCounter != 0)
                {
                    pTmpSecureChannel->uOverlapCounter--;
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetNextElement(pChannelManager->SecureChannels);
                }
                else if(pTmpSecureChannel->uRefCount == 0)
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureListener_ChannelManager_TimerCallback: removing SecureChannel %u after lifetime expired!\n", pTmpSecureChannel->SecureChannelId);

                    /* remove from channel manager and put into temp list for later notification */
                    OpcUa_List_EnQueueCurrentElement(pChannelManager->SecureChannels, &pTmpListFirst, &pTmpListLast);
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    nToDelete++;

                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(pChannelManager->SecureChannels);
                }
                else
                {
                    pTmpSecureChannel->State = OpcUa_SecureChannelState_Closed;
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetNextElement(pChannelManager->SecureChannels);
                }
            }
            else
            {
                /* inactive securechannel only use overlap counter */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureListener_ChannelManager_TimerCallback: inactive SecureChannel!\n");
                if(pTmpSecureChannel->uOverlapCounter != 0)
                {
                    pTmpSecureChannel->uOverlapCounter--;
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetNextElement(pChannelManager->SecureChannels);
                }
                else if(pTmpSecureChannel->uRefCount == 0)
                {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_SecureListener_ChannelManager_TimerCallback: removing inactive SecureChannel!\n");

                    /* remove from channel manager and put into temp list for later notification */
                    OpcUa_List_EnQueueCurrentElement(pChannelManager->SecureChannels, &pTmpListFirst, &pTmpListLast);
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    nToDelete++;

                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(pChannelManager->SecureChannels);
                }
                else
                {
                    OPCUA_SECURECHANNEL_UNLOCK(pTmpSecureChannel);
                    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetNextElement(pChannelManager->SecureChannels);
                }
            }
        }

        OpcUa_List_Leave(pChannelManager->SecureChannels);
    }

    /* notify application about all deleted securechannels and free their resources */
    if(nToDelete != 0)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelManager_TimerCallback: deleting %u SecureChannel!\n", nToDelete);
        while(pTmpListFirst != OpcUa_Null)
        {
            OpcUa_ListElement* pTmpListElement = pTmpListFirst;
            pTmpSecureChannel = (OpcUa_SecureChannel*)pTmpListElement->data;

            pChannelManager->pfCallback(pTmpSecureChannel,
                                        pChannelManager->pvCallbackData);

            pTmpListFirst = pTmpListFirst->nextElement;
            OpcUa_ListElement_Delete(&pTmpListElement);
            OpcUa_TcpSecureChannel_Delete(&pTmpSecureChannel);
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_TimerKillCallback                        */
/*==============================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SecureListener_ChannelManager_TimerKillCallback(   OpcUa_Void*     a_pvCallbackData,
                                                                                        OpcUa_Timer     a_hTimer,
                                                                                        OpcUa_UInt32    a_msecElapsed)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_TimerKillCallback");

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelManager_TimerKillCallback: Lifetime expiration timer stopped!\n");

    OpcUa_ReferenceParameter(a_pvCallbackData);
    OpcUa_ReferenceParameter(a_hTimer);
    OpcUa_ReferenceParameter(a_msecElapsed);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_Create                                   */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_Create(
    OpcUa_SecureListener_ChannelManager_ChannelRemovedCallback* a_pfChannelTimeoutCallback,
    OpcUa_Void*                                                 a_pvChannelTimeoutCallbackData,
    OpcUa_SecureListener_ChannelManager**                       a_ppChannelManager)
{
    OpcUa_SecureListener_ChannelManager* pSecureChannelMngr = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_Create");

    OpcUa_ReturnErrorIfArgumentNull(a_ppChannelManager);

    *a_ppChannelManager = OpcUa_Null;

    pSecureChannelMngr = (OpcUa_SecureListener_ChannelManager*)OpcUa_Alloc(sizeof(OpcUa_SecureListener_ChannelManager));
    OpcUa_ReturnErrorIfAllocFailed(pSecureChannelMngr);

    uStatus = OpcUa_SecureListener_ChannelManager_Initialize(
        a_pfChannelTimeoutCallback,
        a_pvChannelTimeoutCallbackData,
        pSecureChannelMngr);

    OpcUa_GotoErrorIfBad(uStatus);

    *a_ppChannelManager = pSecureChannelMngr;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Free(pSecureChannelMngr);
    pSecureChannelMngr = OpcUa_Null;

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_Initialize                               */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_Initialize(
    OpcUa_SecureListener_ChannelManager_ChannelRemovedCallback* a_pfChannelTimeoutCallback,
    OpcUa_Void*                                                 a_pvChannelTimeoutCallbackData,
    OpcUa_SecureListener_ChannelManager*                        a_pChannelManager)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_Initialize");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannelManager);

    OpcUa_MemSet(a_pChannelManager, 0, sizeof(OpcUa_SecureListener_ChannelManager));

    uStatus = OpcUa_List_Create(&(a_pChannelManager->SecureChannels));
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OpcUa_Timer_Create(   &(a_pChannelManager->hLifeTimeWatchDog),
                                    OPCUA_SECURELISTENER_WATCHDOG_INTERVAL,
                                    OpcUa_SecureListener_ChannelManager_TimerCallback,
                                    OpcUa_SecureListener_ChannelManager_TimerKillCallback,
                                    (OpcUa_Void*)a_pChannelManager);
    OpcUa_GotoErrorIfBad(uStatus);

    a_pChannelManager->pfCallback       = a_pfChannelTimeoutCallback;
    a_pChannelManager->pvCallbackData   = a_pvChannelTimeoutCallbackData;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_SecureListener_ChannelManager_Clear(a_pChannelManager);

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_Clear                                    */
/*==============================================================================*/
OpcUa_Void OpcUa_SecureListener_ChannelManager_Clear(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager)
{
    OpcUa_SecureChannel* pTmpSecureChannel = OpcUa_Null;

    if(OpcUa_Null != a_pChannelManager->hLifeTimeWatchDog)
    {
        OpcUa_Timer_Delete(&(a_pChannelManager->hLifeTimeWatchDog));
    }

    if(OpcUa_Null != a_pChannelManager->SecureChannels)
    {
        /* remove all channels and delete list */
        OpcUa_List_Enter(a_pChannelManager->SecureChannels);

        OpcUa_List_ResetCurrent(a_pChannelManager->SecureChannels);
        pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(a_pChannelManager->SecureChannels);

        while(pTmpSecureChannel != OpcUa_Null)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelManager_Clear: SecureChannel removed!\n");
            OpcUa_List_DeleteCurrentElement(a_pChannelManager->SecureChannels);
            OpcUa_TcpSecureChannel_Delete(&pTmpSecureChannel);
            pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(a_pChannelManager->SecureChannels);
        }

        OpcUa_List_Leave(a_pChannelManager->SecureChannels);
        OpcUa_List_Delete(&(a_pChannelManager->SecureChannels));
    }
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_Delete                                   */
/*==============================================================================*/
OpcUa_Void OpcUa_SecureListener_ChannelManager_Delete(
    OpcUa_SecureListener_ChannelManager** a_ppChannelManager)
{
    if(a_ppChannelManager != OpcUa_Null)
    {
        OpcUa_SecureListener_ChannelManager_Clear(*a_ppChannelManager);

        OpcUa_Free(*a_ppChannelManager);
        *a_ppChannelManager = OpcUa_Null;
        return;
    }

    return;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_IsValidChannelID                         */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_IsValidChannelID(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_UInt32                         a_uSecureChannelID)
{
    OpcUa_SecureChannel*      pTmpSecureChannel     = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_IsValidChannelID");

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);
    OpcUa_List_ResetCurrent(a_pChannelManager->SecureChannels);

    if(a_uSecureChannelID == OPCUA_SECURECHANNEL_ID_INVALID)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureListener - ChannelManager_IsValidChannelID: Invalid SecureChannelID found!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(a_pChannelManager->SecureChannels);

    while(pTmpSecureChannel)
    {
        if(pTmpSecureChannel->SecureChannelId == a_uSecureChannelID)
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureListener - ChannelManager_IsValidChannelID: Duplicate SecureChannelID found!\n");
            OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
        }
        pTmpSecureChannel = (OpcUa_SecureChannel *)OpcUa_List_GetNextElement(a_pChannelManager->SecureChannels);
    }

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_AddChannel                               */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_AddChannel(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_SecureChannel*                 a_pChannel)
{
    OpcUa_UInt32    nChannelCount = 0;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_AddChannel");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_pChannelManager);
    OpcUa_ReturnErrorIfArgumentNull(a_pChannelManager->SecureChannels);

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);

    OpcUa_List_GetNumberOfElements(a_pChannelManager->SecureChannels, &nChannelCount);

#if OPCUA_SECURELISTENER_MAXCONNECTIONS != 0
    if(nChannelCount >= OPCUA_SECURELISTENER_MAXCONNECTIONS)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadMaxConnectionsReached);
    }
#endif

    a_pChannel->uRefCount = 0;
    a_pChannel->ReleaseMethod = OpcUa_SecureListener_ChannelManager_ReleaseChannel;
    a_pChannel->ReleaseParam  = a_pChannelManager;
    uStatus = OpcUa_List_AddElement(a_pChannelManager->SecureChannels, a_pChannel);
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureListener - ChannelManager_AddChannel: SecureChannel added! %u in list\n", nChannelCount);

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_ReleaseChannel                           */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_ReleaseChannel(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_SecureChannel**                a_ppSecureChannel)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "ChannelManager_ReleaseChannel");

    OpcUa_ReturnErrorIfArgumentNull(a_pChannelManager);
    OpcUa_ReturnErrorIfArgumentNull(a_pChannelManager->SecureChannels);
    OpcUa_ReturnErrorIfArgumentNull(a_ppSecureChannel);

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);

    if(*a_ppSecureChannel == OpcUa_Null)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
    }
    else if((*a_ppSecureChannel)->uRefCount == 0)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureListener - ChannelManager_ReleaseChannel: invalid ref count!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }
    else
    {
        (*a_ppSecureChannel)->uRefCount--;
        *a_ppSecureChannel = OpcUa_Null;
    }

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}


/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_SetSecureChannelID                       */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_SetSecureChannelID(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_SecureChannel*                 a_pSecureChannel,
    OpcUa_UInt32                         a_uSecureChannelID)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "SetSecureChannelID");

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);

    a_pSecureChannel->SecureChannelId = a_uSecureChannelID;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_SetTransportConnection                   */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_SetTransportConnection(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_SecureChannel*                 a_pSecureChannel,
    OpcUa_Handle                         a_hTransportConnection)
{
OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "SetTransportConnection");

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);

    a_pSecureChannel->TransportConnection = a_hTransportConnection;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID              */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_UInt32                         a_uSecureChannelID,
    OpcUa_SecureChannel**                a_ppSecureChannel)
{
    OpcUa_SecureChannel*      pTmpSecureChannel     = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "GetChannelBySecureChannelID");

    *a_ppSecureChannel = OpcUa_Null;

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);

    uStatus = OpcUa_List_ResetCurrent(a_pChannelManager->SecureChannels);
    OpcUa_GotoErrorIfBad(uStatus);

    if(a_uSecureChannelID == OPCUA_SECURECHANNEL_ID_INVALID)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "SecureListener - OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID: Invalid SecureChannelID found!\n");
        OpcUa_GotoErrorWithStatus(OpcUa_BadSecureChannelIdInvalid);
    }

    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(a_pChannelManager->SecureChannels);

    while(pTmpSecureChannel != OpcUa_Null)
    {
        if(pTmpSecureChannel->SecureChannelId == a_uSecureChannelID)
        {
            *a_ppSecureChannel = pTmpSecureChannel;
            pTmpSecureChannel->uRefCount++;
            OpcUa_List_Leave(a_pChannelManager->SecureChannels);
            OpcUa_ReturnStatusCode;
        }

        pTmpSecureChannel = (OpcUa_SecureChannel *)OpcUa_List_GetNextElement(a_pChannelManager->SecureChannels);
    }

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "SecureListener - OpcUa_SecureListener_ChannelManager_GetChannelBySecureChannelID: Searched SecureChannel NOT found!\n");
    uStatus = OpcUa_BadSecureChannelIdInvalid;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}

/*==============================================================================*/
/* OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection          */
/*==============================================================================*/
OpcUa_StatusCode OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection(
    OpcUa_SecureListener_ChannelManager* a_pChannelManager,
    OpcUa_Handle                         a_hTransportConnection,
    OpcUa_SecureChannel**                a_ppSecureChannel)
{
    OpcUa_SecureChannel*      pTmpSecureChannel     = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_SecureListener, "GetChannelByTransportConnection");

    *a_ppSecureChannel = OpcUa_Null;

    OpcUa_List_Enter(a_pChannelManager->SecureChannels);

    uStatus = OpcUa_List_ResetCurrent(a_pChannelManager->SecureChannels);
    OpcUa_GotoErrorIfBad(uStatus);

    pTmpSecureChannel = (OpcUa_SecureChannel*)OpcUa_List_GetCurrentElement(a_pChannelManager->SecureChannels);

    while(pTmpSecureChannel != OpcUa_Null)
    {
        if(     pTmpSecureChannel->TransportConnection != OpcUa_Null
            &&  pTmpSecureChannel->TransportConnection == a_hTransportConnection) /* pointer valid and not reused till after this call */
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_SecureListener_ChannelManager_GetChannelByTransportConnection: Searched securechannel found!\n");
            *a_ppSecureChannel = pTmpSecureChannel;
            pTmpSecureChannel->uRefCount++;
            OpcUa_List_Leave(a_pChannelManager->SecureChannels);

            OpcUa_ReturnStatusCode;
        }

        pTmpSecureChannel = (OpcUa_SecureChannel *)OpcUa_List_GetNextElement(a_pChannelManager->SecureChannels);
    }

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

    uStatus = OpcUa_BadNotFound;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_List_Leave(a_pChannelManager->SecureChannels);

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_SERVERAPI */
