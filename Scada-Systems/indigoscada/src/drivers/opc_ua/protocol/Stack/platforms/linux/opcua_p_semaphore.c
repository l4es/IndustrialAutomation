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

#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>


/* UA platform definitions */
#include <opcua_p_internal.h>

/* additional UA dependencies */
#include <opcua_p_datetime.h>
#include <opcua_p_memory.h>
#include <opcua_utilities.h>

/* own headers */
#include <opcua_semaphore.h>
#include <opcua_p_semaphore.h>

static int report_error(int code, const char *where, const char *what)
{
    if (code != 0)
    {
        char szMsg[256];
        /* strerror_r is the thread safe version of strerror */
        strerror_r(code, szMsg, 256);

        fprintf(stderr, "%s: %s failure: %s\n",
                where,
                what,
                szMsg);
    }
    return code;
}

/** Creates a new semaphore.
 * @param a_Semaphore Pointer to semaphore handle. This returns the newly created semaphore.
 * @param a_uInitalValue The initial value of the semaphore.
 * @param a_uMaxRange The maximum value of the semaphore. This has no effect on linux, because the system has no maximim limit. But the parameter is checked to make sense 0 &lt;= InitValue &lt; MaxRange.
 * @return OpcUa_Good if the semaphore could be created, OpcUa_BadInvalidArgument MaxRange is not plausible, or OpcUa_BadInternalError in case of a system call error.
 */
OpcUa_StatusCode    OpcUa_P_Semaphore_Create(OpcUa_Semaphore* a_Semaphore,
                                             OpcUa_UInt32   a_uInitalValue,
                                             OpcUa_UInt32   a_uMaxRange)
{
    sem_t* pInternalSemaphore = OpcUa_Null;

    if (a_uMaxRange == 0) return OpcUa_BadInvalidArgument;
    if (a_uMaxRange < a_uInitalValue) return OpcUa_BadInvalidArgument;

    *a_Semaphore = OpcUa_Null;

    pInternalSemaphore = (sem_t *)malloc(sizeof(sem_t));

    if(pInternalSemaphore == OpcUa_Null)
    {
        return OpcUa_BadOutOfMemory;
    }

    if(sem_init(pInternalSemaphore, 0, (unsigned int)a_uInitalValue) != 0) /* 0 = process local semaphore */
    {
        free(pInternalSemaphore);
        return OpcUa_BadInternalError;
    }

    *a_Semaphore = (OpcUa_Semaphore)pInternalSemaphore;

    return OpcUa_Good;
}

/** Deletes the semaphore. */
OpcUa_Void OpcUa_P_Semaphore_Delete(OpcUa_Semaphore* pRawSemaphore)
{
    sem_t*  pInternalSemaphore  = OpcUa_Null;

    if (pRawSemaphore == OpcUa_Null || *pRawSemaphore == OpcUa_Null) return;

    pInternalSemaphore = (sem_t*) *pRawSemaphore;

    report_error(sem_destroy(pInternalSemaphore), "OpcUa_P_Semaphore_Delete", "sem_destroy");
    free(pInternalSemaphore);
    *pRawSemaphore = OpcUa_Null;
}

/** Aquires a resource.
 * This function blocks until a resource could be aquired.
 * Use OpcUa_P_Semaphore_TimedWait if you don't wont to block forever.
 * This function handles interruptions due to signals and automatically
 * restarts the wait operation.
 * @param RawSemaphore Handle to semaphore.
 * @return OpcUa_Good if the resource was successfully aquired,
 * OpcUa_BadInternalError in case of a system call error.
 */
OpcUa_StatusCode OpcUa_P_Semaphore_Wait(OpcUa_Semaphore RawSemaphore)
{
    sem_t* pInternalSemaphore = (sem_t*)RawSemaphore;
    int err;

    do
    {
        err = 0;
        if (sem_wait(pInternalSemaphore) == -1) err = errno;
    }
    while (err == EINTR);

    if(err != 0)
    {
        report_error(errno, "OpcUa_P_Semaphore_Wait", "sem_wait failed");
        return OpcUa_BadInternalError;
    }

    return OpcUa_Good;
}

/** Aquires a resource.
 * This function behaves like OpcUa_P_Semaphore_Wait, but does not block forever.
 * In case of a timeout the function returns OpcUa_GoodNonCriticalTimeout.
 * @param RawSemaphore Handle to semaphore.
 * @param msecTimeout Maximum time to wait to aquire the resource.
 * @return OpcUa_Good if the resource was successfully aquired, OpcUa_GoodNonCriticalTimeout in case of a timeout,
 * OpcUa_BadInternalError in case of a system call error.
 */
OpcUa_StatusCode OpcUa_P_Semaphore_TimedWait(OpcUa_Semaphore RawSemaphore, OpcUa_UInt32 msecTimeout)
{
    sem_t*          pInternalSemaphore = (sem_t*)RawSemaphore;
    struct timespec Timeout;
    int err;

    if (msecTimeout == OpcUa_Infinite)
    {
        do
        {
            err = 0;
            if (sem_wait(pInternalSemaphore) == -1) err = errno;
        }
        while (err == EINTR);

        if (err != 0)
        {
            report_error(errno, "OpcUa_P_Semaphore_TimedWait", "sem_wait failed");
            return OpcUa_BadInternalError;
        }
    }
    else
    {
        int sec  = msecTimeout / 1000;
        int nsec = (msecTimeout % 1000) * 1000000;

        if (clock_gettime(CLOCK_REALTIME, &Timeout) == -1)
        {
            report_error(errno, "OpcUa_P_Semaphore_TimedWait", "clock_gettime failed");
            return OpcUa_BadInternalError;
        }

        Timeout.tv_sec  += sec;
        Timeout.tv_nsec += nsec;
        if (Timeout.tv_nsec >= 1000000000)
        {
            Timeout.tv_nsec -= 1000000000;
            Timeout.tv_sec++;
        }

        do
        {
            err = 0;
            if (sem_timedwait(pInternalSemaphore, &Timeout) == -1) err = errno;
        }
        while (err == EINTR);

        if(err != 0)
        {
            if(errno == ETIMEDOUT)
            {
                return OpcUa_GoodNonCriticalTimeout;
            }
            else
            {
                report_error(errno, "OpcUa_P_Semaphore_TimedWait", "sem_timedwait failed");
                return OpcUa_BadInternalError;
            }
        }
    }

    return OpcUa_Good;
}

/** Gives back a number of aquired resources.
 * This means it unblocks other blocking OpcUa_P_Semaphore_Wait or OpcUa_P_Semaphore_TimedWait calls.
 * @param RawSemaphore Handle to semaphore.
 * @param uReleaseCount Gives back uReleaseCount resources.
 * @return OpcUa_Good on success or OpcUa_BadInternalError in case of a system call error.
 */
OpcUa_StatusCode OpcUa_P_Semaphore_Post(OpcUa_Semaphore RawSemaphore,
                                        OpcUa_UInt32    uReleaseCount)
{
    sem_t* pInternalSemaphore = (sem_t*)RawSemaphore;

    if (uReleaseCount == 0) return OpcUa_BadInvalidArgument;

    while(uReleaseCount > 0)
    {
        if (sem_post(pInternalSemaphore) != 0)
        {
            return OpcUa_BadInternalError;
        }
        --uReleaseCount;
    }
    return OpcUa_Good;
}
