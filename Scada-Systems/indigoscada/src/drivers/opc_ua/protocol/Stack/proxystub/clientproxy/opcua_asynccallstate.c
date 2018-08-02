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

#ifdef OPCUA_HAVE_CLIENTAPI

#include <opcua_semaphore.h>
#include <opcua_mutex.h>
#include <opcua_utilities.h>

/* types */
#include <opcua_builtintypes.h>
#include <opcua_encodeableobject.h>
#include <opcua_types.h>

/* communication */
#include <opcua_connection.h>

/* client api */
#include <opcua_channel.h>

/* self */
#include <opcua_asynccallstate.h>

/*============================================================================
 * OpcUa_AsyncCallState_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_AsyncCallState_Create(
    OpcUa_Void*             a_hChannel,
    OpcUa_Void*             a_pRequestData,
    OpcUa_EncodeableType*   a_pRequestType,
    OpcUa_AsyncCallState**  a_ppAsyncState)
{
    OpcUa_AsyncCallState* pAsyncState = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_AsyncCallState, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_hChannel);
    OpcUa_ReturnErrorIfArgumentNull(a_ppAsyncState);

    *a_ppAsyncState = OpcUa_Null;

    pAsyncState = (OpcUa_AsyncCallState*)OpcUa_Alloc(sizeof(OpcUa_AsyncCallState));
    OpcUa_GotoErrorIfAllocFailed(pAsyncState);
    OpcUa_MemSet(pAsyncState, 0, sizeof(OpcUa_AsyncCallState));

    pAsyncState->Channel       = a_hChannel;
    pAsyncState->RequestData   = a_pRequestData;
    pAsyncState->RequestType   = a_pRequestType;
    pAsyncState->Status        = OpcUa_BadWaitingForResponse;
    pAsyncState->WaitMutex     = OpcUa_Null;
    pAsyncState->WaitCondition = OpcUa_Null;

    uStatus = OPCUA_P_MUTEX_CREATE(&(pAsyncState->WaitMutex));
    OpcUa_GotoErrorIfBad(uStatus);

    /* create binary semaphore */
    uStatus = OPCUA_P_SEMAPHORE_CREATE(&pAsyncState->WaitCondition, 0, 1);
    OpcUa_GotoErrorIfBad(uStatus);

    *a_ppAsyncState = pAsyncState;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_AsyncCallState_Delete(&pAsyncState);

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_AsyncCallState_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_AsyncCallState_Delete(OpcUa_AsyncCallState** a_ppAsyncState)
{
    if (a_ppAsyncState != OpcUa_Null && *a_ppAsyncState != OpcUa_Null)
    {
        OpcUa_AsyncCallState* pAsyncState = *a_ppAsyncState;

        OPCUA_P_MUTEX_DELETE(&(pAsyncState->WaitMutex));
        OPCUA_P_SEMAPHORE_DELETE(&(pAsyncState->WaitCondition));
        OpcUa_Free(pAsyncState);

        *a_ppAsyncState = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_Session_WaitForCompletion
 *===========================================================================*/
OpcUa_StatusCode OpcUa_AsyncCallState_WaitForCompletion(
    OpcUa_AsyncCallState* a_pAsyncState,
    OpcUa_UInt32          a_uTimeout)
{
    OpcUa_UInt32    uElapsedTime        = 0;
    OpcUa_Boolean   bTimeoutOccurred    = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_AsyncCallState, "WaitForCompletion");

    OpcUa_ReturnErrorIfArgumentNull(a_pAsyncState);

    /* lock request mutex */
    OPCUA_P_MUTEX_LOCK(a_pAsyncState->WaitMutex);

    /* block until call completes */
    while (a_pAsyncState->Status == OpcUa_BadWaitingForResponse)
    {
        OpcUa_UInt32 uActualTimeout = OPCUA_INFINITE;
        OpcUa_UInt32 uStartCount = OpcUa_GetTickCount();
        OpcUa_UInt32 uEndCount = uStartCount;

        /* calculate the time left in the requested timeout period */
        if (a_uTimeout > 0)
        {
            if (a_uTimeout < uElapsedTime)
            {
                bTimeoutOccurred = OpcUa_True;
                a_pAsyncState->Status = OpcUa_BadOperationAbandoned;
                break;
            }

            uActualTimeout = a_uTimeout - uElapsedTime;
        }

        /* block until timeout or response arrives */
        if (uActualTimeout > 0 || (uActualTimeout == 0 && a_uTimeout == 0))
        {
            OPCUA_P_MUTEX_UNLOCK(a_pAsyncState->WaitMutex);
            uStatus = OPCUA_P_SEMAPHORE_TIMEDWAIT(a_pAsyncState->WaitCondition, uActualTimeout);
            if(OpcUa_IsEqual(OpcUa_GoodNonCriticalTimeout))
            {
                bTimeoutOccurred = OpcUa_True;
            }
            else if(OpcUa_IsBad(uStatus))
            {
                break;
            }
            OPCUA_P_MUTEX_LOCK(a_pAsyncState->WaitMutex);
        }

        /* abandon request if timeout expired */
        if (bTimeoutOccurred)
        {
            a_pAsyncState->Status = OpcUa_BadOperationAbandoned;
            break;
        }

        /* calculate the elapsed time */
        uEndCount = OpcUa_GetTickCount();

        if (uEndCount < uStartCount)
        {
            uElapsedTime += (uEndCount + (OpcUa_UInt32_Max - uStartCount));
        }
        else
        {
            uElapsedTime += (uEndCount - uStartCount);
        }
    }

    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_AsyncCallState_WaitForCompletion: OPCUA_P_SEMAPHORE_TIMEDWAIT returned error 0x%08X!\n", uStatus);
    }
    else
    {
        /* release request mutex */
        OPCUA_P_MUTEX_UNLOCK(a_pAsyncState->WaitMutex);

        uStatus = a_pAsyncState->Status;

        /* do not free async state object if timeout occurred (callback will happen eventually). */
        if(OpcUa_False != bTimeoutOccurred)
        {
            uStatus = OpcUa_BadTimeout;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_AsyncCallState_SignalCompletion
 *===========================================================================*/
OpcUa_StatusCode OpcUa_AsyncCallState_SignalCompletion(
    OpcUa_AsyncCallState* a_pAsyncState,
    OpcUa_StatusCode      a_uOperationStatus)
{
OpcUa_InitializeStatus(OpcUa_Module_AsyncCallState, "SignalCompletion");

    OpcUa_ReturnErrorIfArgumentNull(a_pAsyncState);

    /* lock request mutex */
    OPCUA_P_MUTEX_LOCK(a_pAsyncState->WaitMutex);

    /* set the return value for the waiter on this status */
    a_pAsyncState->Status = a_uOperationStatus;

    /* signal that the request has completed */
    uStatus = OPCUA_P_SEMAPHORE_POST(a_pAsyncState->WaitCondition, 1);
    OpcUa_GotoErrorIfBad(uStatus);

    /* release the wait mutex */
    OPCUA_P_MUTEX_UNLOCK(a_pAsyncState->WaitMutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    /* release the wait mutex */
    OPCUA_P_MUTEX_UNLOCK(a_pAsyncState->WaitMutex);

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_CLIENTAPI */
