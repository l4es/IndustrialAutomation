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

/* base */
#include <opcua.h>

#ifdef OPCUA_HAVE_THREADPOOL

/* core */
#include <opcua_mutex.h>
#include <opcua_semaphore.h>
#include <opcua_thread.h>
#include <opcua_list.h>

/* self */
#include <opcua_threadpool.h>

#define OPCUA_THREADPOOL_SILENT 1

#ifdef OPCUA_THREADPOOL_SILENT
  #define OPCUA_THREADPOOL_SILENCER(xTrace)
#else /* OPCUA_THREADPOOL_SILENT */
  #define OPCUA_THREADPOOL_SILENCER(xTrace) xTrace
#endif /* OPCUA_THREADPOOL_SILENT */

/** @todo: get rid of dynamic thread list; use default job list instead. */

/*============================================================================
 * Types
 *===========================================================================*/

struct _OpcUa_ThreadPoolInternal
{
    /** @brief Synchronize access to the threadpool. */
    OpcUa_Mutex     hMutex;
    /** @brief Array of static threads created on pool creation. */
    OpcUa_Thread*   aStaticThreads;
    /** @brief Number of threads created on pool creation. Is equal to MinThreads. */
    OpcUa_UInt32    uNoOfStaticThreads;
    /** @brief Max number of jobs being processed or waiting in queue. */
    OpcUa_UInt32    uNoOfJobsMax;
    /** @brief Number of jobs being processed or waiting in queue. Used for comparing against max value. */
    OpcUa_UInt32    uNoOfJobs;
    /** @brief Used as event to unlock a worker thread. */
    OpcUa_Semaphore hJobAdded;
    /** @brief Number of free threads in the pool. (static) */
    OpcUa_UInt32    uFreeThreads;
    /** @brief Maximum number of threads created. Is equal to MaxThreads. (static + dynamic) */
    OpcUa_UInt32    uMaxNoOfThreads;
    /** @brief Current number of threads created. (static + dynamic) */
    OpcUa_UInt32    uNoOfThreads;
#if OPCUA_THREADPOOL_EXPANSION
    /** @brief */
    OpcUa_List*     DynamicThreadList;
#endif /* OPCUA_THREADPOOL_EXPANSION */
    /** @brief Id of the last issued job. */
    OpcUa_UInt32    uJobId;
    /** @brief List of jobs waiting to be executed. */
    OpcUa_List*     JobList;
    /** @brief Checked by all static worker threads to find out wether to reloop or shutdown. */
    OpcUa_Boolean   bStop;
    /** @brief AddJob blocks a slot in the is free. */
    OpcUa_Boolean   bBlockIfFull;
    /** @brief Signals an available slot. */
    OpcUa_Semaphore hQueueOpenSemaphore;
    /** @brief Max time interval AddJob blocks. */
    OpcUa_UInt32    uTimeOut;
};

/** @brief The management structure of a thread pool. */
typedef struct _OpcUa_ThreadPoolInternal OpcUa_ThreadPoolInternal;

struct _OpcUa_ThreadPool_Job
{
    /** @brief Handle of the pool this job belongs to. */
    OpcUa_ThreadPoolInternal*   pThreadPool;
    /** @brief The function that needs to be executed by the worker. */
    OpcUa_PfnThreadMain*        pFunction;
    /** @brief User supplied arguments for function. */
    OpcUa_Void*                 pArgument;
    /** @brief Pool local id for this job. */
    OpcUa_UInt32                uJobId;
#if OPCUA_THREADPOOL_EXPANSION
    /** @brief This job is done. */
    OpcUa_Boolean               bFinished;
    /** @brief Handle of a dynamic thread, that executed this job. */
    OpcUa_Thread                hThread;
#endif /* OPCUA_THREADPOOL_EXPANSION */
};

/** @brief A particular job to be executed by worker from the thread pool. */
typedef struct _OpcUa_ThreadPool_Job OpcUa_ThreadPool_Job;

/*============================================================================
 * Functions
 *===========================================================================*/

/*****************************************************************************/
/** @brief */
static OpcUa_Void OpcUa_ThreadPool_ThreadMain(OpcUa_Void* a_pArguments)
{
    OpcUa_ThreadPoolInternal*   pThreadPoolInternal = (OpcUa_ThreadPoolInternal*)a_pArguments;
    OpcUa_ThreadPool_Job*       pThreadPoolJob      = OpcUa_Null;
    OpcUa_Boolean               bRestlessWorker     = OpcUa_False;
    OpcUa_StatusCode            uStatus             = OpcUa_Good;

    if(pThreadPoolInternal == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_ThreadPool_ThreadMain: Threadpoolworker started with invalid poolhandle!\n");
        return;
    }

    OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_ThreadMain: Worker starting up.\n");)

    /* Fetch job. */
    OpcUa_List_Enter(pThreadPoolInternal->JobList);
    pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_RemoveFirstElement(pThreadPoolInternal->JobList);
    OpcUa_List_Leave(pThreadPoolInternal->JobList);

    /* loop as long no stop signal is available and a job has to be done. */
    while(pThreadPoolInternal->bStop == OpcUa_False)
    {
        if(pThreadPoolJob == OpcUa_Null)
        {
            /* update pool if this wasnt an empty reloop */
            if(OpcUa_IsNotEqual(OpcUa_GoodNonCriticalTimeout))
            {
                /* Update pool. */
                OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
                pThreadPoolInternal->uFreeThreads++;
                OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
            }

            bRestlessWorker = OpcUa_False;

            uStatus = OPCUA_P_SEMAPHORE_TIMEDWAIT(  pThreadPoolInternal->hJobAdded,
                                                    OPCUA_THREADPOOL_RELOOPTIME);
        }

        switch(uStatus)
        {
        case OpcUa_GoodNonCriticalTimeout:
            {
                /* reloop while expression */
                break;
            }
        case OpcUa_Good:
            {
                if(pThreadPoolJob == OpcUa_Null)
                {
                    /* Try to fetch job. */
                    OpcUa_List_Enter(pThreadPoolInternal->JobList);
                    pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_RemoveFirstElement(pThreadPoolInternal->JobList);
                    OpcUa_List_Leave(pThreadPoolInternal->JobList);
                }

                if(pThreadPoolJob != OpcUa_Null && pThreadPoolJob->pFunction != OpcUa_Null)
                {
                    /* Update pool only if we woke up by an event and are not looping. */
                    if(bRestlessWorker == OpcUa_False)
                    {
                        OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
                        pThreadPoolInternal->uFreeThreads--;
                        OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
                    }

                    /* Execute and delete job. */
                    OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_ThreadMain: Executing job with ID %u!\n", pThreadPoolJob->uJobId);)

                    /***************************************************/
                    pThreadPoolJob->pFunction(pThreadPoolJob->pArgument);
                    /***************************************************/

                    OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
                    if(pThreadPoolInternal->uNoOfJobs >= pThreadPoolInternal->uNoOfJobsMax)
                    {
                        if(pThreadPoolInternal->uNoOfJobs > pThreadPoolInternal->uNoOfJobsMax)
                        {
                            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "OpcUa_ThreadPool_ThreadMain: number of jobs larger than max: %u to %u!\n", pThreadPoolInternal->uNoOfJobs, pThreadPoolInternal->uNoOfJobsMax);
                        }

                        pThreadPoolInternal->uNoOfJobs--;
                        OPCUA_P_SEMAPHORE_POST(pThreadPoolInternal->hQueueOpenSemaphore, 1);
                    }
                    else
                    {
                        pThreadPoolInternal->uNoOfJobs--;
                    }
                    OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);

                    OpcUa_Free(pThreadPoolJob);
                }

                /* Fetch job. */
                OpcUa_List_Enter(pThreadPoolInternal->JobList);
                pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_RemoveFirstElement(pThreadPoolInternal->JobList);
                OpcUa_List_Leave(pThreadPoolInternal->JobList);

                if(pThreadPoolJob != OpcUa_Null)
                {
                    bRestlessWorker = OpcUa_True;
                }

                break;
            }
        default:
            {
                /* Bad result from semaphore wait. */
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_ThreadPool_ThreadMain: SemaphoreWait reported error 0x%X! I quit...\n", uStatus);

                /* Update pool. */
                OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
                pThreadPoolInternal->uFreeThreads--;
                OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
                return;
            }
        } /* switch wait result */

        uStatus = OpcUa_Good;
    } /* while */

    OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_ThreadMain: Stopping!\n");)

    /* Update pool. */
    OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
    pThreadPoolInternal->uFreeThreads--;
    OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);

    return;
}

/*****************************************************************************/
/** @brief */

#if OPCUA_THREADPOOL_EXPANSION
static OpcUa_Void OpcUa_ThreadPool_DynamicThreadMain(OpcUa_Void* a_pArguments)
{
    OpcUa_ThreadPool_Job*       pThreadPoolJob      = (OpcUa_ThreadPool_Job*)a_pArguments;

    if(pThreadPoolJob == OpcUa_Null)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_ThreadPool_DynamicThreadMain: Thread started with invalid jobhandle!\n");
        return;
    }

    OPCUA_P_MUTEX_LOCK(pThreadPoolJob->pThreadPool->hMutex);

    if(pThreadPoolJob->pFunction != OpcUa_Null)
    {
        /* Execute and delete job. */
        OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_DynamicThreadMain: Executing job with ID %u!\n", pThreadPoolJob->uJobId);)
        OPCUA_P_MUTEX_UNLOCK(pThreadPoolJob->pThreadPool->hMutex);

        /***************************************************/
        pThreadPoolJob->pFunction(pThreadPoolJob->pArgument);
        /***************************************************/

        OPCUA_P_MUTEX_LOCK(pThreadPoolJob->pThreadPool->hMutex);
        pThreadPoolJob->pThreadPool->uNoOfJobs--;
        OPCUA_P_SEMAPHORE_POST(pThreadPoolJob->pThreadPool->hQueueOpenSemaphore, 1);
    }

    pThreadPoolJob->pThreadPool->uNoOfThreads--;
    OPCUA_P_MUTEX_UNLOCK(pThreadPoolJob->pThreadPool->hMutex);

    OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_DynamicThreadMain: Job %u Stopping!\n", pThreadPoolJob->uJobId);)

    pThreadPoolJob->bFinished = OpcUa_True;

    return;
}
#endif /* OPCUA_THREADPOOL_EXPANSION */

/*****************************************************************************/
/** @brief */
OpcUa_Void OPCUA_DLLCALL OpcUa_ThreadPool_Clear(OpcUa_ThreadPool a_hThreadPool)
{
    OpcUa_ThreadPoolInternal*   pThreadPoolInternal = (OpcUa_ThreadPoolInternal*)a_hThreadPool;
    OpcUa_UInt32                i                   = 0;

#if OPCUA_THREADPOOL_EXPANSION
    OpcUa_UInt32                nDynJobs            = 0;
#endif

    if(a_hThreadPool == OpcUa_Null)
    {
        return;
    }

    if(pThreadPoolInternal->hMutex != OpcUa_Null)
    {
        OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
    }

    pThreadPoolInternal->bStop = OpcUa_True;

    if(OpcUa_Null != pThreadPoolInternal->hJobAdded)
    {
        OPCUA_P_SEMAPHORE_POST(pThreadPoolInternal->hJobAdded, pThreadPoolInternal->uNoOfStaticThreads);
    }

    if(OpcUa_Null != pThreadPoolInternal->hMutex)
    {
        OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
    }

    for(i = 0; i < pThreadPoolInternal->uNoOfStaticThreads; i++)
    {
        if(pThreadPoolInternal->aStaticThreads[i] != OpcUa_Null)
        {
            OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Verifying stop of thread %u in pool %p.\n", i, pThreadPoolInternal);)
            OpcUa_Thread_WaitForShutdown(   pThreadPoolInternal->aStaticThreads[i],
                                            OPCUA_INFINITE);
            OpcUa_Thread_Delete(&(pThreadPoolInternal->aStaticThreads[i]));
            OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Thread %u in pool %p has been deleted.\n", i, pThreadPoolInternal);)
        }
        else
        {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Thread %u in pool %p is empty.\n", i, pThreadPoolInternal);
        }
    }

#if OPCUA_THREADPOOL_EXPANSION

    if(OpcUa_Null != pThreadPoolInternal->hMutex)
    {
        OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
    }

    if(OpcUa_Null != pThreadPoolInternal->DynamicThreadList)
    {
        OpcUa_List_GetNumberOfElements(pThreadPoolInternal->DynamicThreadList, &nDynJobs);
    }

    if(nDynJobs != 0)
    {
        OpcUa_ThreadPool_Job* pThreadPoolJob = OpcUa_Null;

        OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Cleaning up dynamic thread jobs...\n");)

        pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_RemoveFirstElement(pThreadPoolInternal->DynamicThreadList);

        while(pThreadPoolJob != OpcUa_Null)
        {
            OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);

            OpcUa_Thread_WaitForShutdown(   pThreadPoolJob->hThread,
                                            OPCUA_INFINITE);

            OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);

            OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Deleting dynamic thread job id %u!\n", pThreadPoolJob->uJobId);)

            OpcUa_Thread_Delete(&(pThreadPoolJob->hThread));
            OpcUa_Free(pThreadPoolJob);

            pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_RemoveFirstElement(pThreadPoolInternal->DynamicThreadList);
        }

        OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Dynamic threads cleaned up!\n");)
    }

    if(OpcUa_Null != pThreadPoolInternal->hMutex)
    {
        OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
    }

    if(pThreadPoolInternal->uNoOfThreads != pThreadPoolInternal->uNoOfStaticThreads)
    {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_ThreadPool_Clear: Inconsistent thread count!\n");
    }

    if(OpcUa_Null != pThreadPoolInternal->DynamicThreadList)
    {
        OpcUa_List_Delete(&(pThreadPoolInternal->DynamicThreadList));
    }
#endif /* OPCUA_THREADPOOL_EXPANSION */

    if(OpcUa_Null != pThreadPoolInternal->aStaticThreads)
    {
        OpcUa_Free(pThreadPoolInternal->aStaticThreads);
    }

    if(OpcUa_Null != pThreadPoolInternal->hMutex)
    {
        OPCUA_P_MUTEX_DELETE(&pThreadPoolInternal->hMutex);
    }

    if(OpcUa_Null != pThreadPoolInternal->hQueueOpenSemaphore)
    {
        OPCUA_P_SEMAPHORE_DELETE(&pThreadPoolInternal->hQueueOpenSemaphore);
    }

    if(OpcUa_Null != pThreadPoolInternal->hJobAdded)
    {
        OPCUA_P_SEMAPHORE_DELETE(&pThreadPoolInternal->hJobAdded);
    }

    if(OpcUa_Null != pThreadPoolInternal->JobList)
    {
        OpcUa_List_Delete(&pThreadPoolInternal->JobList);
    }

    return;
}

/*****************************************************************************/
/** @brief */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ThreadPool_Initialize( OpcUa_ThreadPool  a_hThreadPool,
                                                            OpcUa_UInt32      a_uMinThreads,
                                                            OpcUa_UInt32      a_uMaxThreads,
                                                            OpcUa_UInt32      a_uMaxJobs,
                                                            OpcUa_Boolean     a_bBlockIfFull,
                                                            OpcUa_UInt32      a_uTimeout)
{
    OpcUa_ThreadPoolInternal*   pThreadPoolInternal = OpcUa_Null;
    OpcUa_UInt32                i                   = 0;

OpcUa_InitializeStatus(OpcUa_Module_ThreadPool, "Initialize");

    if(a_hThreadPool == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    pThreadPoolInternal = (OpcUa_ThreadPoolInternal*)a_hThreadPool;
    OpcUa_MemSet(pThreadPoolInternal, 0, sizeof(OpcUa_ThreadPoolInternal));

    if((a_uMaxThreads < a_uMinThreads) || (a_uMinThreads == 0))
    {
        return OpcUa_BadInvalidArgument;
    }

#if !OPCUA_THREADPOOL_EXPANSION
    OpcUa_ReturnErrorIfTrue(a_uMinThreads != a_uMaxThreads, OpcUa_BadInvalidArgument);
#endif /* OPCUA_THREADPOOL_EXPANSION */

    pThreadPoolInternal = (OpcUa_ThreadPoolInternal*)a_hThreadPool;

    pThreadPoolInternal->uFreeThreads       = 0;
    pThreadPoolInternal->uNoOfStaticThreads = a_uMinThreads;
    pThreadPoolInternal->uMaxNoOfThreads    = a_uMaxThreads;
    pThreadPoolInternal->uNoOfThreads       = a_uMinThreads;
    pThreadPoolInternal->uJobId             = 0;
    pThreadPoolInternal->bStop              = OpcUa_False;
    pThreadPoolInternal->uNoOfJobs          = 0;

    pThreadPoolInternal->uNoOfJobsMax       = a_uMaxJobs;
    pThreadPoolInternal->bBlockIfFull       = a_bBlockIfFull;
    pThreadPoolInternal->uTimeOut           = a_uTimeout;

    pThreadPoolInternal->aStaticThreads = (OpcUa_Thread*)OpcUa_Alloc((a_uMinThreads * sizeof(OpcUa_Thread)));
    OpcUa_GotoErrorIfAllocFailed(pThreadPoolInternal->aStaticThreads);
    OpcUa_MemSet(pThreadPoolInternal->aStaticThreads, 0, (a_uMinThreads * sizeof(OpcUa_Thread)));

#if OPCUA_THREADPOOL_EXPANSION
    uStatus = OpcUa_List_Create(        &pThreadPoolInternal->DynamicThreadList);
    OpcUa_GotoErrorIfBad(uStatus);
#endif /* OPCUA_THREADPOOL_EXPANSION */

    uStatus = OpcUa_List_Create(        &pThreadPoolInternal->JobList);
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_SEMAPHORE_CREATE( &pThreadPoolInternal->hJobAdded,
                                        0,              /* initial value */
                                        a_uMinThreads); /* max value     */
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_SEMAPHORE_CREATE( &pThreadPoolInternal->hQueueOpenSemaphore,
                                        0,  /* initial value */
                                        1); /* max value */
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_MUTEX_CREATE(     &pThreadPoolInternal->hMutex);
    OpcUa_GotoErrorIfBad(uStatus);

    OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);

    for(i = 0; i < a_uMinThreads; i++)
    {
        /* start all threads */
        OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Initialize: Starting Thread %u in Pool %p.\n", i, pThreadPoolInternal);)
        uStatus = OpcUa_Thread_Create(  &(pThreadPoolInternal->aStaticThreads[i]),
                                        OpcUa_ThreadPool_ThreadMain,
                                        pThreadPoolInternal);
        if(OpcUa_IsBad(uStatus))
        {
            OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
            OpcUa_GotoError;
        }

        uStatus = OpcUa_Thread_Start(   pThreadPoolInternal->aStaticThreads[i]);
        if(OpcUa_IsBad(uStatus))
        {
            OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
            OpcUa_GotoError;
        }
    }

    OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Initialize: All Threads in Pool %p started.\n", pThreadPoolInternal);)

    OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_ThreadPool_Clear((OpcUa_ThreadPool)pThreadPoolInternal);

OpcUa_FinishErrorHandling;
}

/*****************************************************************************/
/** @brief */
OpcUa_Void OPCUA_DLLCALL OpcUa_ThreadPool_Delete(OpcUa_ThreadPool* a_phThreadPool)
{
    if(a_phThreadPool == OpcUa_Null || *a_phThreadPool == OpcUa_Null)
    {
        return;
    }

    OpcUa_ThreadPool_Clear(*a_phThreadPool);

    OpcUa_Free(*a_phThreadPool);
    *a_phThreadPool = OpcUa_Null;

    return;
}

/*****************************************************************************/
/** @brief */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ThreadPool_Create( OpcUa_ThreadPool* a_phThreadPool,
                                                        OpcUa_UInt32      a_uMinThreads,
                                                        OpcUa_UInt32      a_uMaxThreads,
                                                        OpcUa_UInt32      a_uMaxJobs,
                                                        OpcUa_Boolean     a_bBlockIfFull,
                                                        OpcUa_UInt32      a_uTimeout)
{
    OpcUa_ThreadPoolInternal* pThreadPoolInternal = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_ThreadPool, "Create");

    OpcUa_ReturnErrorIfArgumentNull(a_phThreadPool);

#if !OPCUA_THREADPOOL_EXPANSION
    OpcUa_ReturnErrorIfTrue(a_uMinThreads != a_uMaxThreads, OpcUa_BadInvalidArgument);
#endif /* OPCUA_THREADPOOL_EXPANSION */

    *a_phThreadPool = OpcUa_Null;

    pThreadPoolInternal = (OpcUa_ThreadPoolInternal*)OpcUa_Alloc(sizeof(OpcUa_ThreadPoolInternal));
    OpcUa_ReturnErrorIfAllocFailed(pThreadPoolInternal);

    uStatus = OpcUa_ThreadPool_Initialize(  (OpcUa_ThreadPool)pThreadPoolInternal,
                                            a_uMinThreads,
                                            a_uMaxThreads,
                                            a_uMaxJobs,
                                            a_bBlockIfFull,
                                            a_uTimeout);
    OpcUa_GotoErrorIfBad(uStatus);

    *a_phThreadPool = pThreadPoolInternal;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_Free(pThreadPoolInternal);

OpcUa_FinishErrorHandling;
}

/*****************************************************************************/
/** @brief */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ThreadPool_AddJob( OpcUa_ThreadPool        a_hThreadPool,
                                                        OpcUa_PfnThreadMain*    a_pFunction,
                                                        OpcUa_Void*             a_pArgument)
{
    OpcUa_ThreadPoolInternal*   pThreadPoolInternal = OpcUa_Null;
    OpcUa_ThreadPool_Job*       pThreadPoolJob      = OpcUa_Null;
#if OPCUA_THREADPOOL_EXPANSION
    OpcUa_UInt32                uThreadCount        = 0;
#endif /* OPCUA_THREADPOOL_EXPANSION */

OpcUa_InitializeStatus(OpcUa_Module_ThreadPool, "AddJob");

    OpcUa_ReturnErrorIfArgumentNull(a_hThreadPool);
    OpcUa_ReturnErrorIfArgumentNull(a_pFunction);

    pThreadPoolInternal = (OpcUa_ThreadPoolInternal*)a_hThreadPool;

#if OPCUA_THREADPOOL_EXPANSION

    /* try to clean up list of dynamic threads */
    OpcUa_List_Enter(pThreadPoolInternal->DynamicThreadList);

    OpcUa_List_ResetCurrent(pThreadPoolInternal->DynamicThreadList);
    pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_GetCurrentElement(pThreadPoolInternal->DynamicThreadList);

    while(pThreadPoolJob != OpcUa_Null)
    {
        if(pThreadPoolJob->bFinished != OpcUa_False)
        {
            OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_Clear: Deleting dynamic thread for job id %u!\n", pThreadPoolJob->uJobId);)
            OpcUa_List_DeleteCurrentElement(pThreadPoolInternal->DynamicThreadList);

            OpcUa_Thread_WaitForShutdown(   pThreadPoolJob->hThread,
                                            OPCUA_INFINITE);

            OpcUa_Thread_Delete(pThreadPoolJob->hThread);
            OpcUa_Free(pThreadPoolJob);

            pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_GetCurrentElement(pThreadPoolInternal->DynamicThreadList);
        }
        else
        {
            pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_List_GetNextElement(pThreadPoolInternal->DynamicThreadList);
        }
    }

    OpcUa_List_Leave(pThreadPoolInternal->DynamicThreadList);

#endif /* OPCUA_THREADPOOL_EXPANSION */

    OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);

    /* check if job maximum is reached */
    if(pThreadPoolInternal->uNoOfJobs < pThreadPoolInternal->uNoOfJobsMax)
    {
        pThreadPoolInternal->uNoOfJobs++;
    }
    else
    {
        /* either return with error or block */
        if(pThreadPoolInternal->bBlockIfFull != OpcUa_False)
        {
            /* retest job count after semaphore triggered */
            while(pThreadPoolInternal->uNoOfJobs >= pThreadPoolInternal->uNoOfJobsMax)
            {
                OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
                uStatus = OPCUA_P_SEMAPHORE_TIMEDWAIT(  pThreadPoolInternal->hQueueOpenSemaphore,
                                                        pThreadPoolInternal->uTimeOut);
                if(OpcUa_IsEqual(OpcUa_GoodNonCriticalTimeout))
                {
                    uStatus = OpcUa_BadTimeout;
                }
                OpcUa_ReturnErrorIfBad(uStatus);

                OPCUA_P_MUTEX_LOCK(pThreadPoolInternal->hMutex);
            }

            pThreadPoolInternal->uNoOfJobs++;
        }
        else /* don't block */
        {
            OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
            uStatus = OpcUa_BadWouldBlock;
            OpcUa_ReturnStatusCode;
        }
    }

    /* Create and initialize job */
    pThreadPoolJob = (OpcUa_ThreadPool_Job*)OpcUa_Alloc(sizeof(OpcUa_ThreadPool_Job));
    OpcUa_GotoErrorIfAllocFailed(pThreadPoolJob);
    OpcUa_MemSet(pThreadPoolJob, 0, sizeof(OpcUa_ThreadPool_Job));

    pThreadPoolJob->pThreadPool = pThreadPoolInternal;
    pThreadPoolJob->uJobId      = pThreadPoolInternal->uJobId++;
    pThreadPoolJob->pFunction   = a_pFunction;
    pThreadPoolJob->pArgument   = a_pArgument;

#if OPCUA_THREADPOOL_EXPANSION

    /* try to create new dynamic thread for this job, if no static thread is available */
    if(     pThreadPoolInternal->uFreeThreads == 0
        &&  pThreadPoolInternal->uNoOfThreads < pThreadPoolInternal->uMaxNoOfThreads)
    {
        uStatus = OpcUa_Thread_Create(  &pThreadPoolJob->hThread,
                                        OpcUa_ThreadPool_DynamicThreadMain,
                                        pThreadPoolJob);
        OpcUa_GotoErrorIfBad(uStatus);

        uThreadCount = pThreadPoolInternal->uNoOfThreads++;

        uStatus = OpcUa_List_AddElementToEnd(pThreadPoolInternal->DynamicThreadList, (OpcUa_Void*)pThreadPoolJob);
        OpcUa_GotoErrorIfBad(uStatus);

        pThreadPoolJob->bFinished = OpcUa_False;

        uStatus = OpcUa_Thread_Start(pThreadPoolJob->hThread);
        OpcUa_GotoErrorIfBad(uStatus);

        /* leave directly. */
        OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);
        OpcUa_ReturnStatusCode;
    } /* else (as third alternative) queue the job anyway. */

#endif /* OPCUA_THREADPOOL_EXPANSION */

    OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);

    OPCUA_THREADPOOL_SILENCER(OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "OpcUa_ThreadPool_AddJob: Adding new element.\n");)

    /* Put job into list. */
    OpcUa_List_Enter(pThreadPoolInternal->JobList);
    uStatus = OpcUa_List_AddElementToEnd(   pThreadPoolInternal->JobList,
                                            pThreadPoolJob);
    OpcUa_List_Leave(pThreadPoolInternal->JobList);
    OpcUa_GotoErrorIfBad(uStatus);

    /* Let the system wake one thread to fetch the job from the queue. */
    OPCUA_P_SEMAPHORE_POST(pThreadPoolInternal->hJobAdded, 1);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if OPCUA_THREADPOOL_EXPANSION

    if(pThreadPoolJob != OpcUa_Null)
    {
        OpcUa_Thread_Delete(pThreadPoolJob->hThread);
    }

    OpcUa_List_DeleteElement(pThreadPoolInternal->DynamicThreadList, (OpcUa_Void*)pThreadPoolJob);
    pThreadPoolInternal->uNoOfThreads = uThreadCount;

#endif /* OPCUA_THREADPOOL_EXPANSION */

    if(pThreadPoolJob != OpcUa_Null)
    {
        OpcUa_Free(pThreadPoolJob);
    }

    OPCUA_P_MUTEX_UNLOCK(pThreadPoolInternal->hMutex);

OpcUa_FinishErrorHandling;
}

#endif /* OPCUA_HAVE_THREADPOOL */

