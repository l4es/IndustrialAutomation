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

/******************************************************************************************************/
/* Platform Portability Layer                                                                         */
/* Modify the content of this file according to the semaphore implementation on your system.          */
/* This is the pthreads implementation.                                                               */
/* For Information about pthread see http://sourceware.org/pthreads-win32/.                           */
/* The waits are affected by thread cancelation!                                                      */
/******************************************************************************************************/

/* #define OPCUA_USE_POSIX 1 */

/* System Headers */
#include <windows.h>
#include <time.h>

#ifdef OPCUA_USE_POSIX
#include <pthread.h>
#include <semaphore.h>
#endif /* OPCUA_USE_POSIX */

/* UA platform definitions */
#include <opcua_p_internal.h>

/* additional UA dependencies */
#include <opcua_datetime.h>
#include <opcua_utilities.h>

/* own headers */
#include <opcua_semaphore.h>
#include <opcua_p_semaphore.h>


#ifdef OPCUA_USE_POSIX
/*============================================================================
 * Create a Semaphore
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_P_Semaphore_Create(      OpcUa_Semaphore* a_Semaphore,
                                                   OpcUa_UInt32     a_uInitalValue,
                                                   OpcUa_UInt32     a_uMaxRange)
{
    sem_t* pInternalSemaphore = OpcUa_Null;

    /* no way for max range in posix semaphore? */
    OpcUa_ReferenceParameter(a_uMaxRange);

    *a_Semaphore = OpcUa_Null;

    pInternalSemaphore = OpcUa_Alloc(sizeof(sem_t));

    if(pInternalSemaphore == OpcUa_Null)
    {
        return OpcUa_BadOutOfMemory;
    }

    if(sem_init(pInternalSemaphore, 0, (unsigned int)a_uInitalValue) != 0) /* 0 = process local semaphore */
    {
        OpcUa_Free(pInternalSemaphore);
        return OpcUa_BadInternalError;
    }

    *a_Semaphore = (OpcUa_Semaphore)pInternalSemaphore;

    return OpcUa_Good;
}

/*============================================================================
 * Delete the semaphore.
 *===========================================================================*/
OpcUa_Void OpcUa_P_Semaphore_Delete(OpcUa_Semaphore* pRawSemaphore)
{
    sem_t*              ppInternalSemaphore = (sem_t*)*pRawSemaphore;
    struct sem_t_*      pInternalSemaphore  = OpcUa_Null;

    if(     pRawSemaphore           == OpcUa_Null
        ||  *ppInternalSemaphore    == OpcUa_Null)
    {
        return;
    }

    /* This needs some explanation:                                  */
    /* sem_post has a dead zone in which the system semaphore is     */
    /* already released, but the posix object is still locked        */
    /* with a mutex. A Thread, waiting on a semaphore and deleting   */
    /* it immediately after getting it, will fail doing this, if the */
    /* thread, that called release is still in that dead zone.       */
    /* Additionally, sem_destroy resets the handle, so i have to     */
    /* store it outside and restore if it failed to give sem_destroy */
    /* further chances. Since when deleting a synchronization        */
    /* resource is always critical and it has to be assured that     */
    /* no other thread may be waiting on it, the following code is   */
    /* valid. */

    while(*pRawSemaphore != OpcUa_Null)
    {
        pInternalSemaphore = *ppInternalSemaphore;
        if(sem_destroy(ppInternalSemaphore) == 0)
        {
            OpcUa_Free(*pRawSemaphore);
            *pRawSemaphore = OpcUa_Null;
        }
        else
        {
            /* self-suspend to give release thread time to */
            /* leave the post completely                   */
            *ppInternalSemaphore = pInternalSemaphore;
            Sleep(0);
        }
    }
}

/*============================================================================
 * Wait
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Semaphore_Wait(OpcUa_Semaphore RawSemaphore)
{
    sem_t* pInternalSemaphore = (sem_t*)RawSemaphore;

    if(sem_wait(pInternalSemaphore) != 0)
    {
        return OpcUa_BadInternalError;
    }

    return OpcUa_Good;
}

/*============================================================================
 * Wait for a maximum of seconds.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Semaphore_TimedWait(OpcUa_Semaphore RawSemaphore, OpcUa_UInt32 msecTimeout)
{
    sem_t*          pInternalSemaphore = (sem_t*)RawSemaphore;
    struct timespec Timeout;
    OpcUa_TimeVal   now;

    /* convert the given timeout into a format pthreadlib understands */
    OpcUa_DateTime_GetTimeOfDay(&now);

    if (msecTimeout == INFINITE)
    {
        Timeout.tv_sec  = OpcUa_Int32_Max;
        Timeout.tv_nsec = OpcUa_Int32_Max;
    }
    else
    {
        Timeout.tv_sec  = now.uintSeconds + ((long)(msecTimeout/1000));
        Timeout.tv_nsec = (now.uintMicroSeconds * 1000) + ((msecTimeout % 1000) * 1000000);
    }


    if(sem_timedwait(pInternalSemaphore, &Timeout) != 0)
    {
        DWORD dwErrno = OpcUa_GetLastError();

        if(dwErrno == ETIMEDOUT)
        {
            return OpcUa_GoodTimeout;
        }
        else
        {
            return OpcUa_BadInternalError;
        }
    }

    return OpcUa_Good;
}

/*============================================================================
 * Post (free)
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Semaphore_Post(OpcUa_Semaphore RawSemaphore,
                                        OpcUa_UInt32    uReleaseCount)
{
    sem_t* pInternalSemaphore = (sem_t*)RawSemaphore;

    if(sem_post_multiple(pInternalSemaphore, (int)uReleaseCount) != 0)
    {
        return OpcUa_BadInternalError;
    }

    return OpcUa_Good;
}

#else /* OPCUA_USE_POSIX */

/*============================================================================
 * Create a Semaphore
 *===========================================================================*/
OpcUa_StatusCode    OPCUA_DLLCALL OpcUa_P_Semaphore_Create( OpcUa_Semaphore* a_Semaphore,
                                                            OpcUa_UInt32     a_uInitalValue,
                                                            OpcUa_UInt32     a_uMaxRange)
{
    HANDLE InternalSemaphore = NULL;

    *a_Semaphore = OpcUa_Null;

    if((a_uMaxRange == 0) || (a_uMaxRange < a_uInitalValue))
    {
        return OpcUa_BadInvalidArgument;
    }


    InternalSemaphore = CreateSemaphore(    NULL,
                                            a_uInitalValue,
                                            a_uMaxRange,
                                            (LPCTSTR)NULL);

    if(InternalSemaphore == NULL)
    {
        return OpcUa_BadInternalError;
    }

    *a_Semaphore = (OpcUa_Semaphore)InternalSemaphore;

    return OpcUa_Good;
}

/*============================================================================
 * Delete the semaphore.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Semaphore_Delete(OpcUa_Semaphore* pRawSemaphore)
{
    if(pRawSemaphore == OpcUa_Null || *pRawSemaphore == OpcUa_Null)
    {
        return;
    }

    if(CloseHandle((HANDLE)*pRawSemaphore) != 0)
    {
        /* CloseHandle succeeded if it returned non-zero */
        *pRawSemaphore = OpcUa_Null;
    }
    else
    {
        /* CloseHandle failed if it returned zero */
    }
}

/*============================================================================
 * Wait
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Semaphore_Wait(OpcUa_Semaphore RawSemaphore)
{
    HANDLE  InternalSemaphore = (HANDLE)RawSemaphore;
    DWORD   dwResult;

    dwResult = WaitForSingleObject(InternalSemaphore, INFINITE);

    if(dwResult == WAIT_TIMEOUT)
    {
        return OpcUa_BadInternalError;
    }
/* TODO: find return value */
/*
    else if(dwResult == WAIT_ABANDONED)
    {
    }
*/
    else if(dwResult == WAIT_OBJECT_0)
    {
        return OpcUa_Good;
    }
    else /*dwResult == WAIT_FAILED*/
    {
        return OpcUa_BadInternalError;
    }
}

/*============================================================================
 * Wait for a maximum of seconds.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Semaphore_TimedWait( OpcUa_Semaphore a_RawSemaphore,
                                                            OpcUa_UInt32    a_msecTimeout)
{
    HANDLE  InternalSemaphore = (HANDLE)a_RawSemaphore;
    DWORD   dwResult;

    dwResult = WaitForSingleObject(InternalSemaphore, a_msecTimeout);

    if(dwResult == WAIT_TIMEOUT)
    {
        return OpcUa_GoodNonCriticalTimeout;
    }
    else if(dwResult == WAIT_OBJECT_0)
    {
        return OpcUa_Good;
    }
    else /*dwResult == WAIT_FAILED*/
    {
        return OpcUa_BadInternalError;
    }
}

/*============================================================================
 * Post (free)
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Semaphore_Post(  OpcUa_Semaphore RawSemaphore,
                                                        OpcUa_UInt32    uReleaseCount)
{
    HANDLE InternalSemaphore = (HANDLE)RawSemaphore;

    if(uReleaseCount < 1)
    {
        return OpcUa_BadInvalidArgument;
    }

    if(ReleaseSemaphore(InternalSemaphore, uReleaseCount, NULL))
    {
        return OpcUa_Good;
    }
    else
    {
        DWORD dwLastError = GetLastError();

        switch(dwLastError)
        {
        case ERROR_TOO_MANY_POSTS:
            {
                return OpcUa_BadTooManyPosts;
            }
        default:
            {
                return OpcUa_BadInternalError;
            }
        }
    }
}
#endif /* OPCUA_USE_POSIX */

