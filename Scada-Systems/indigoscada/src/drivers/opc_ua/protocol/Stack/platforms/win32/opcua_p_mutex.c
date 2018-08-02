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
/* Modify the content of this file according to the mutex implementation on your system.              */
/* Hint: Emulate with binary semaphore                                                                */
/* This is the pthreads implementation + win32 native (see OPCUA_USE_POSIX)                           */
/* For Information about pthread see http://sourceware.org/pthreads-win32/.                           */
/******************************************************************************************************/

/* System Headers */

#define OPCUA_MUTEX_USE_SPINCOUNT 0

#if OPCUA_MUTEX_USE_SPINCOUNT
#define _WIN32_WINNT  0x0403
#endif

#include <windows.h>
#include <stdio.h>

/* UA platform definitions */
#include <opcua_p_internal.h>
#include <opcua_p_memory.h>

/* own headers */
#include <opcua_mutex.h>
#include <opcua_p_mutex.h>

#if OPCUA_MUTEX_ERROR_CHECKING
#include <opcua_p_thread.h>
#endif

/* #define OPCUA_USE_POSIX */


#if OPCUA_MUTEX_ERROR_CHECKING
#ifdef OPCUA_USE_POSIX /* errorchecking is not implemented with the posix mutex. */
#else OPCUA_USE_POSIX /* OPCUA_USE_POSIX */

    /* not maintainded code */

    #include <opcua_thread.h> /* for thread id */

    OpcUa_Int32 g_nMutexId = 0;

    struct _OpcUa_P_InternalMutex
    {
        OpcUa_Void*     pSystemMutex;
        OpcUa_Int32     nMutexId;
        OpcUa_UInt32    uThreadId;
        OpcUa_Int32     nLockCount;
    };
    typedef struct _OpcUa_P_InternalMutex OpcUa_P_InternalMutex;
#endif /* OPCUA_USE_POSIX */
#endif /* OPCUA_MUTEX_ERROR_CHECKING */

#ifdef OPCUA_USE_POSIX
#include <pthread.h>

/*============================================================================
 * Initialize the mutex.
 *===========================================================================*/

/* add errorchecking */

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Mutex_Initialize(OpcUa_Mutex hMutex)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    int                 result      = 0;
    pthread_mutex_t*    pPosixMutex = (pthread_mutex_t*)(hMutex);
    pthread_mutexattr_t att;

    /* initialize and set mutex attribute object */
    result = pthread_mutexattr_init(&att);
    if(result != 0)
    {
        return OpcUa_Bad;
    }

    result = pthread_mutexattr_settype(&att, PTHREAD_MUTEX_RECURSIVE);
    if(result != 0)
    {
        pthread_mutexattr_destroy(&att);
        return OpcUa_Bad;
    }

    /* initialize mutex with the attribute object */
    result = pthread_mutex_init(pPosixMutex, &att); /* code */
    if(result != 0)
    {
        uStatus = OpcUa_Bad;
    }

    /* delete the temporary attribute object */
    result = pthread_mutexattr_destroy(&att); /* code */
    if(result != 0)
    {
        uStatus = OpcUa_Bad;
    }

    return uStatus;
}

/*============================================================================
 * Allocate the mutex.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Mutex_Create(OpcUa_Mutex* phMutex)
{
    OpcUa_StatusCode uStatus        = OpcUa_Good;
    pthread_mutex_t* pPosixMutex    = OpcUa_Null;

    if(phMutex == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    pPosixMutex = OpcUa_P_Memory_Alloc(sizeof(pthread_mutex_t));
    OpcUa_ReturnErrorIfAllocFailed(pPosixMutex);

    uStatus = OpcUa_P_Mutex_Initialize((OpcUa_Mutex)pPosixMutex);
    if(OpcUa_IsBad(uStatus))
    {
        OpcUa_P_Mutex_Delete((OpcUa_Mutex*)&pPosixMutex);
    }
    else
    {
        *phMutex = (OpcUa_Mutex)pPosixMutex;
    }

    return uStatus;
}

/*============================================================================
 * Clear the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_Clear(OpcUa_Mutex hMutex)
{
    pthread_mutex_t* pPosixMutex = OpcUa_Null;
    pPosixMutex = (pthread_mutex_t*)hMutex;

    if(pPosixMutex == OpcUa_Null)
    {
        return;
    }

    pthread_mutex_destroy(pPosixMutex);
}

/*============================================================================
 * Clear and free the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_Delete(OpcUa_Mutex* phMutex)
{
    if( phMutex == OpcUa_Null || *phMutex == OpcUa_Null)
    {
        return;
    }
    else
    {
        OpcUa_P_Mutex_Clear(*phMutex);

        OpcUa_P_Memory_Free(*phMutex);

        *phMutex = OpcUa_Null;
    }
}

/*============================================================================
 * Lock the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_Lock(OpcUa_Mutex hMutex)
{
    OpcUa_StatusCode    uStatus      = OpcUa_Good;
    int                 apiResult   = 0;
    pthread_mutex_t*    pPosixMutex = (pthread_mutex_t*)(hMutex);

    if(hMutex == OpcUa_Null)
    {
        printf("Error: OpcUa_P_Mutex_Lock!\n");
        getchar();
        return;
    }

    apiResult = pthread_mutex_lock(pPosixMutex);

    if(apiResult != 0)
    {
        /* debug; makes no sense, i know */
#ifdef _DEBUG
        if(apiResult == EINVAL)
        {
            uStatus = OpcUa_BadInternalError;
        }
        else if(apiResult == EDEADLK)
        {
            uStatus = OpcUa_BadInternalError;
        }
        else
#endif /* _DEBUG */
        {
            uStatus = OpcUa_BadInternalError;
        }
    }
}

/*============================================================================
 * Unlock the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_Unlock(OpcUa_Mutex hMutex)
{
    OpcUa_StatusCode    uStatus      = OpcUa_Good;
    int                 apiResult   = 0;
    pthread_mutex_t*    pPosixMutex = (pthread_mutex_t*)(hMutex);

    if(hMutex == OpcUa_Null)
    {
        printf("Error: OpcUa_P_Mutex_Unlock!\n");
        getchar();
        return;
    }

    apiResult = pthread_mutex_unlock(pPosixMutex);

    if(apiResult != 0)
    {
        /* EPERM = 1 (unlocking unowned mutex) */
        uStatus = OpcUa_BadInternalError;
    }
}
#else /* OPCUA_USE_POSIX */

#if OPCUA_MUTEX_ERROR_CHECKING
/*============================================================================
 * Allocate the mutex.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Mutex_CreateImp(OpcUa_Mutex* phMutex, char* file, int line)
{
    OpcUa_StatusCode uStatus = OpcUa_Good;
    CRITICAL_SECTION *pCS;
    OpcUa_P_InternalMutex* pInternalMutex = OpcUa_Null;

    if(phMutex == OpcUa_Null)
    {
        printf("Invalid Argument Error in: OpcUa_P_Mutex_Create!\n");
        return OpcUa_BadInvalidArgument;
    }

    pCS = OpcUa_P_Memory_Alloc(sizeof(CRITICAL_SECTION));
    if(pCS == NULL)
    {
        printf("Out of memory Error in: OpcUa_P_Mutex_Create!\n");
        return OpcUa_BadOutOfMemory;
    }

    pInternalMutex = (OpcUa_P_InternalMutex*)OpcUa_P_Memory_Alloc(sizeof(OpcUa_P_InternalMutex));
    if(pInternalMutex == NULL)
    {
        printf("Out of memory Error in: OpcUa_P_Mutex_Create!\n");
        return OpcUa_BadOutOfMemory;
    }

    g_nMutexId++;

    pInternalMutex->pSystemMutex = pCS;
    pInternalMutex->nLockCount = 0;
    pInternalMutex->uThreadId  = 0;
    pInternalMutex->nMutexId   = g_nMutexId;

    InitializeCriticalSection(pCS);

    *phMutex = (OpcUa_Mutex)pInternalMutex;

    /* printf("Created  Mutex%d, File: %s, Line: %d\n", pInternalMutex->nMutexId, file, line); */

    return uStatus;
}

/*============================================================================
 * Clear and free the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_DeleteImp(OpcUa_Mutex* phMutex, char* file, int line)
{
    OpcUa_Int32 nMutexId;
    CRITICAL_SECTION *pCS = NULL;
    OpcUa_P_InternalMutex* pInternalMutex = OpcUa_Null;

    if(phMutex == OpcUa_Null || *phMutex == OpcUa_Null)
    {
        printf("Invalid Argument Error in: OpcUa_P_Mutex_Delete!\n");
        return;
    }

    pInternalMutex = (OpcUa_P_InternalMutex*)*phMutex;

    pCS = (CRITICAL_SECTION*)pInternalMutex->pSystemMutex;

    if(pInternalMutex->nLockCount != 0)
    {
        printf("Error in OpcUa_P_Mutex_Delete. LockCount != 0.\n");
    }

    nMutexId = pInternalMutex->nMutexId;
    OpcUa_P_Memory_Free(pInternalMutex);

    DeleteCriticalSection(pCS);

    OpcUa_P_Memory_Free(pCS);

    *phMutex = OpcUa_Null;

    /* printf("Deleted Mutex%d, File: %s, Line: %d\n", nMutexId, file, line); */

    return;
}

/*============================================================================
 * Lock the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_LockImp(OpcUa_Mutex hMutex, char* file, int line)
{
    OpcUa_P_InternalMutex* pInternalMutex = OpcUa_Null;

    if(hMutex == OpcUa_Null)
    {
        /* Debug */
        printf("InvalidArgument Error in OpcUa_P_Mutex_Lock: File: %s, Line: %d\n", file, line);
        return;
    }

    pInternalMutex = (OpcUa_P_InternalMutex*)hMutex;

    EnterCriticalSection((CRITICAL_SECTION*)pInternalMutex->pSystemMutex);

    pInternalMutex->nLockCount++;

    pInternalMutex->uThreadId = OpcUa_P_Thread_GetCurrentThreadId();

    /* printf("Locked   Mutex%d ThreadID: %d, LockCount: %d, File: %s, Line: %d\n", pInternalMutex->nMutexId, pInternalMutex->uThreadId, pInternalMutex->nLockCount, file, line); */

    hMutex = (OpcUa_Mutex)pInternalMutex;

    return;
}

/*============================================================================
 * Unlock the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_UnlockImp(OpcUa_Mutex hMutex, char* file, int line)
{
    OpcUa_P_InternalMutex* pInternalMutex = OpcUa_Null;

    if(hMutex == OpcUa_Null)
    {
        /* debug */
        printf("InvalidArgument Error in OpcUa_P_Mutex_Unlock: File: %s, Line: %d\n", file, line);
        return;
    }

    pInternalMutex = (OpcUa_P_InternalMutex*)hMutex;

    if(pInternalMutex->nLockCount == 0)
    {
        printf("(ERROR) Unlocking Mutex%d ThreadID: %d, Count: %d, File: %s, Line: %d\n", pInternalMutex->nMutexId, pInternalMutex->uThreadId, pInternalMutex->nLockCount, file, line);
        return;
    }

    if(pInternalMutex->uThreadId != OpcUa_P_Thread_GetCurrentThreadId())
    {
        printf("(ERROR) Unlocking Mutex%d ThreadID: %d, Count: %d, File: %s, Line: %d\n", pInternalMutex->nMutexId, pInternalMutex->uThreadId, pInternalMutex->nLockCount, file, line);
        return;
    }

    pInternalMutex->nLockCount--;

    /* printf("Unlocked Mutex%d ThreadID: %d, New LockCount: %d, File: %s, Line: %d\n", pInternalMutex->nMutexId, pInternalMutex->uThreadId, pInternalMutex->nLockCount, file, line); */

    hMutex = (OpcUa_Mutex)pInternalMutex;

    LeaveCriticalSection((CRITICAL_SECTION*)pInternalMutex->pSystemMutex);

    return;
}
#else /* OPCUA_MUTEX_ERROR_CHECKING */
/*============================================================================
 * Allocate the mutex.
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Mutex_CreateImp(OpcUa_Mutex* a_phMutex)
{
    OpcUa_StatusCode uStatus = OpcUa_Good;
    LPCRITICAL_SECTION lpCriticalSection = NULL;

#if OPCUA_MUTEX_USE_SPINCOUNT
    BOOL bRet;
    LONG hr;
#endif

    if(a_phMutex == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    if(*a_phMutex != OpcUa_Null)
    {
        *a_phMutex = OpcUa_Null;
    }

    lpCriticalSection = OpcUa_P_Memory_Alloc(sizeof(CRITICAL_SECTION));

    if(lpCriticalSection == NULL)
    {
        return OpcUa_BadOutOfMemory;
    }

#if OPCUA_MUTEX_USE_SPINCOUNT
    bRet = InitializeCriticalSectionAndSpinCount(   lpCriticalSection,
                                                    0x00000400) ;
    if (bRet == 0)
    {
        hr = GetLastError();
        OpcUa_P_Memory_Free(lpCriticalSection);
        return OpcUa_Bad;
    }

#else
    InitializeCriticalSection(lpCriticalSection);
#endif

    *a_phMutex = (OpcUa_Mutex)lpCriticalSection;

    return uStatus;
}

/*============================================================================
 * Clear and free the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_DeleteImp(OpcUa_Mutex* a_phMutex)
{
    LPCRITICAL_SECTION lpCriticalSection = NULL;
    if(a_phMutex == OpcUa_Null || *a_phMutex == OpcUa_Null)
    {
        return;
    }

    lpCriticalSection = (LPCRITICAL_SECTION)*a_phMutex;

    DeleteCriticalSection(lpCriticalSection);

    OpcUa_P_Memory_Free(lpCriticalSection);

    *a_phMutex = OpcUa_Null;

    return;
}

/*============================================================================
 * Lock the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_LockImp(OpcUa_Mutex hMutex)
{
    if(hMutex == OpcUa_Null)
    {
        /* Debug */
        /* printf("Error: OpcUa_P_Mutex_Lock!\n"); */
        return;
    }

    EnterCriticalSection((CRITICAL_SECTION*)hMutex);

    return;
}

/*============================================================================
 * Unlock the mutex.
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Mutex_UnlockImp(OpcUa_Mutex hMutex)
{
    if(hMutex == OpcUa_Null)
    {
        /* debug */
        /* printf("Error: OpcUa_P_Mutex_Unlock!\n"); */
        return;
    }

    LeaveCriticalSection((CRITICAL_SECTION*)hMutex);

    return;
}
#endif /* OPCUA_MUTEX_ERROR_CHECKING */
#endif /* OPCUA_USE_POSIX */
