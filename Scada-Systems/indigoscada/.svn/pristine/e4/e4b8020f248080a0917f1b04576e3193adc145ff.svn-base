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

/* core */
#include <opcua_mutex.h>
#include <opcua_semaphore.h>

/* self */
#include <opcua_thread.h>


/* mappings to platform layer calltable. */
#define OpcUa_P_Thread_Create               OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadCreate
#define OpcUa_P_Thread_Delete               OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadDelete
#define OpcUa_P_Thread_Start                OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadStart
#define OpcUa_P_Thread_Sleep                OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadSleep
#define OpcUa_P_Thread_GetCurrentThreadId   OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadGetCurrentId


typedef struct _OpcUa_ThreadInternal OpcUa_ThreadInternal;
struct _OpcUa_ThreadInternal
{
    /** @brief The handle of the platform thread. */
    OpcUa_RawThread      RawThread;
    /** @brief Mutex to synchronize access to the thread object. */
    OpcUa_Mutex          Mutex;
    /** @brief Enables to wait for the termination of the thread. */
    OpcUa_Semaphore      ShutdownEvent;
    /** @brief Tells, if the thread is currently running. */
    OpcUa_Boolean        IsRunning;
    /** @brief A stop flag. */
    /*OpcUa_Boolean        MustStop;*/
    /** @brief The entry function of the thread. */
    OpcUa_PfnThreadMain* ThreadMain;
    /** @brief The data which will be the argument to the thread function. */
    OpcUa_Void*          ThreadData;
};

/*============================================================================
 * The internal main entry function.
 *===========================================================================*/
OpcUa_Void InternalThreadMain(OpcUa_Void* a_Thread)
{
    OpcUa_ThreadInternal* Thread = (OpcUa_ThreadInternal*)a_Thread;

    if(Thread == OpcUa_Null)
    {
        return;
    }

    /* call the user function */
    Thread->ThreadMain(Thread->ThreadData);

    OPCUA_P_MUTEX_LOCK(Thread->Mutex);
    Thread->IsRunning = OpcUa_False;
    OPCUA_P_SEMAPHORE_POST(Thread->ShutdownEvent, 1);
    OPCUA_P_MUTEX_UNLOCK(Thread->Mutex);
}

/*============================================================================
 * Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Thread_Create(   OpcUa_Thread*           a_pThread,
                                        OpcUa_PfnThreadMain*    a_pThreadMain,
                                        OpcUa_Void*             a_pThreadArgument)
{
    OpcUa_StatusCode        uStatus = OpcUa_Good;
    OpcUa_ThreadInternal*   pThread = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Thread);

    OpcUa_ReturnErrorIfArgumentNull(a_pThread);
    OpcUa_ReturnErrorIfArgumentNull(a_pThreadMain);

    pThread = (OpcUa_ThreadInternal*)OpcUa_Alloc(sizeof(OpcUa_ThreadInternal));

    OpcUa_ReturnErrorIfAllocFailed(pThread);

    OpcUa_MemSet(pThread, 0, sizeof(OpcUa_ThreadInternal));

    pThread->IsRunning      = OpcUa_False;
    pThread->ThreadMain     = a_pThreadMain;
    pThread->ThreadData     = a_pThreadArgument;
    pThread->Mutex          = OpcUa_Null;
    pThread->ShutdownEvent  = OpcUa_Null;

    uStatus = OpcUa_P_Thread_Create(&(pThread->RawThread));
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_SEMAPHORE_CREATE( &(pThread->ShutdownEvent),
                                        1,  /* the initial value is 1 (signalled, 1 free resource) */
                                        1); /* the maximum value is 1 */
    OpcUa_GotoErrorIfBad(uStatus);

    uStatus = OPCUA_P_MUTEX_CREATE(&(pThread->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    *a_pThread = pThread;

    return OpcUa_Good;

Error:

    return uStatus;
}


/*============================================================================
 * Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Thread_Delete(OpcUa_Thread* a_pThread)
{
    OpcUa_ThreadInternal* pThread = OpcUa_Null;

    if(a_pThread == OpcUa_Null || *a_pThread == OpcUa_Null)
    {
        return;
    }

    pThread = *((OpcUa_ThreadInternal**)a_pThread);

    if(pThread->IsRunning != OpcUa_False)
    {
        return;
    }

    if(pThread->ShutdownEvent)
    {
        OPCUA_P_SEMAPHORE_DELETE(&(pThread->ShutdownEvent));
    }

    if(pThread->RawThread)
    {
        OpcUa_P_Thread_Delete(&(pThread->RawThread));
    }

    if(pThread->Mutex)
    {
        OPCUA_P_MUTEX_DELETE(&(pThread->Mutex));
    }

    pThread->ThreadData = OpcUa_Null;
    pThread->ThreadMain = OpcUa_Null;


    OpcUa_Free(*a_pThread);

    *a_pThread = OpcUa_Null;

    return;
}


/*============================================================================
 * Start a created thread.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Thread_Start(OpcUa_Thread a_Thread)
{
    OpcUa_StatusCode        uStatus     = OpcUa_Good;
    OpcUa_ThreadInternal*   pThread     = OpcUa_Null;
    OpcUa_Int32             intThreadId = 0;

    OpcUa_ReturnErrorIfArgumentNull(a_Thread);

    pThread = (OpcUa_ThreadInternal*)a_Thread;

    OPCUA_P_MUTEX_LOCK(pThread->Mutex);
    if(pThread->IsRunning != OpcUa_False)
    {
        OPCUA_P_MUTEX_UNLOCK(pThread->Mutex);
        return OpcUa_Good;
    }

    /* set semaphore to waitable */
    uStatus = OPCUA_P_SEMAPHORE_WAIT((pThread->ShutdownEvent));
    OpcUa_GotoErrorIfBad(uStatus);

    pThread->IsRunning = OpcUa_True;

    intThreadId = OpcUa_P_Thread_Start( pThread->RawThread,
                                        InternalThreadMain,
                                        (OpcUa_Void*)pThread);

    if(intThreadId != 0)
    {
        pThread->IsRunning = OpcUa_False;

        OPCUA_P_MUTEX_UNLOCK(pThread->Mutex);
        uStatus = OpcUa_BadInternalError;
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "OpcUa_Thread_Start: Error during thread creation!\n");
        goto Error;
    }
    OPCUA_P_MUTEX_UNLOCK(pThread->Mutex);

    return OpcUa_Good;

Error:

    return uStatus;
}

/*============================================================================
 * Wait for a thread to shutdown.
 *===========================================================================*/
/* conversion is done internally, the parameter must be filled with a value representing milliseconds */
OpcUa_StatusCode OpcUa_Thread_WaitForShutdown(  OpcUa_Thread a_Thread,
                                                OpcUa_UInt32 a_msecTimeout)
{
    OpcUa_StatusCode        uStatus = OpcUa_Good;
    OpcUa_ThreadInternal*   Thread  = OpcUa_Null;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Thread);

    Thread = (OpcUa_ThreadInternal*)a_Thread;

    OpcUa_ReturnErrorIfArgumentNull(Thread);

    OPCUA_P_MUTEX_LOCK(Thread->Mutex);
    if(Thread->IsRunning == OpcUa_False)
    {
        /* printf("wait for shutdown: thread is not running\n");*/
        OPCUA_P_MUTEX_UNLOCK(Thread->Mutex);
        return OpcUa_Good;
    }
    OPCUA_P_MUTEX_UNLOCK(Thread->Mutex);

    uStatus = OPCUA_P_SEMAPHORE_TIMEDWAIT(Thread->ShutdownEvent, a_msecTimeout);
    if(OpcUa_IsBad(uStatus))
    {
        return uStatus;
    }

    OPCUA_P_MUTEX_LOCK(Thread->Mutex);
    if(Thread->IsRunning == OpcUa_False)
    {
        /* Release the semaphore again to enable other threads waiting on it to get unlocked. */
        uStatus = OPCUA_P_SEMAPHORE_POST(   Thread->ShutdownEvent,
                                            1);
        /*printf("wait for shutdown: thread stopped\n");*/
        OPCUA_P_MUTEX_UNLOCK(Thread->Mutex);
        return OpcUa_Good;
    }

    OPCUA_P_MUTEX_UNLOCK(Thread->Mutex);

    /* printf("wait for shutdown: thread is still running\n");*/

    return OpcUa_GoodNonCriticalTimeout;
}

/*============================================================================
 * Let the thread sleep for a certian amount of time
 *===========================================================================*/
OpcUa_Void OpcUa_Thread_Sleep(OpcUa_UInt32 a_msecTimeout)
{
    OpcUa_P_Thread_Sleep(a_msecTimeout);
}

/*============================================================================
 * Retrieve the id of the active thread.
 *===========================================================================*/
OpcUa_UInt32 OpcUa_Thread_GetCurrentThreadId(OpcUa_Void)
{
    return (OpcUa_UInt32)OpcUa_P_Thread_GetCurrentThreadId();
}

/*============================================================================
 * Check if the main function of the given thread object is running.
 *===========================================================================*/
OpcUa_Boolean OpcUa_Thread_IsRunning(OpcUa_Thread a_hThread)
{
    OpcUa_ThreadInternal* pThread = (OpcUa_ThreadInternal*)a_hThread;
    OpcUa_Boolean bTemp = OpcUa_False;

    if(OpcUa_Null == pThread)
    {
        return OpcUa_False;
    }

    OPCUA_P_MUTEX_LOCK(pThread->Mutex);

    bTemp = pThread->IsRunning;

    OPCUA_P_MUTEX_UNLOCK(pThread->Mutex);

    return bTemp;
}
