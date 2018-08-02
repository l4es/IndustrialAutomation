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

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <memory.h>


#include <opcua_p_internal.h>
#include <opcua_p_thread.h>
#include <opcua_p_openssl.h>


/*============================================================================
 * Port Layer Thread Main
 *===========================================================================*/

typedef struct _OpcUa_P_ThreadArg
{
    pthread_t                    hThread;
    OpcUa_PfnInternalThreadMain* pfnInternalThreadMain;
    OpcUa_Void*                  ThreadArgs;
} OpcUa_P_ThreadArg;

pthread_attr_t *opcua_p_thread_attr = NULL;

/**
* This is the function, the new thread starts with. The only thing to do here,
* is calling the InternalThreadMain from OpcUa_Thread.c and your internal stuff.
*/

void* pthread_start(void* args)
{
    OpcUa_P_ThreadArg*  p_P_ThreadArgs;
    OpcUa_Void*         pArgument;

    if(args == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    p_P_ThreadArgs = (OpcUa_P_ThreadArg*)args;

    pArgument       = p_P_ThreadArgs->ThreadArgs;

    /* run stack thread! */
    p_P_ThreadArgs->pfnInternalThreadMain(pArgument);

#if OPCUA_REQUIRE_OPENSSL
    OpcUa_P_OpenSSL_Thread_Cleanup();
#endif

    pthread_exit(NULL);

    return OpcUa_Null;
}

/*============================================================================
 * Initialize Raw Thread.
 *===========================================================================*/
OpcUa_Void OpcUa_P_Thread_Initialize(OpcUa_RawThread RawThread)
{
    memset((OpcUa_Void*)RawThread, 0, sizeof(OpcUa_P_ThreadArg));
    return;
}

/*============================================================================
 * Create a platform thread
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Thread_Create(OpcUa_RawThread* pRawThread)
{
    OpcUa_StatusCode uStatus = OpcUa_Good;
    *pRawThread = (OpcUa_RawThread)malloc(sizeof(OpcUa_P_ThreadArg));
    OpcUa_ReturnErrorIfAllocFailed(*pRawThread);

    OpcUa_P_Thread_Initialize(*pRawThread);

    return uStatus;
}

/*============================================================================
 * Clear Raw Thread
 *===========================================================================*/
OpcUa_Void OpcUa_P_Thread_Clear(OpcUa_RawThread RawThread)
{
    memset((OpcUa_Void*)RawThread, 0, sizeof(pthread_t));
    return;
}

/*============================================================================
 * Delete Raw Thread
 *===========================================================================*/
OpcUa_Void OpcUa_P_Thread_Delete(OpcUa_RawThread* pRawThread)
{
    pthread_join(*(pthread_t*)*pRawThread, NULL);
    free(*pRawThread);
    *pRawThread = OpcUa_Null;
    return;
}

/*============================================================================
 * Create Thread
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Thread_Start(  OpcUa_RawThread             pThread,
                                        OpcUa_PfnInternalThreadMain pfnStartFunction,
                                        OpcUa_Void*                 pArguments)
{
    OpcUa_Int32         apiResult;
    OpcUa_P_ThreadArg*  pThreadArguments;
    pthread_t*          phThread;

    if(pThread == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    pThreadArguments = (OpcUa_P_ThreadArg*)pThread;

    phThread = &pThreadArguments->hThread;
    pThreadArguments->pfnInternalThreadMain = pfnStartFunction;
    pThreadArguments->ThreadArgs            = pArguments;

    apiResult = pthread_create(phThread, opcua_p_thread_attr, pthread_start, pThreadArguments);

    switch(apiResult)
    {
    case EAGAIN:
        {
            return OpcUa_BadResourceUnavailable;
        }
    case 0:
        {
            return OpcUa_Good;
        }
    default:
        {
            return OpcUa_BadInternalError;
        }
    }
}

/*============================================================================
 * Send the thread to sleep.
 *===========================================================================*/
OpcUa_Void OpcUa_P_Thread_Sleep(OpcUa_UInt32 msecTimeout)
{
    usleep(msecTimeout * 1000);
}

/*============================================================================
 * Get Current Thread Id
 *===========================================================================*/
unsigned long OpcUa_P_Thread_GetCurrentThreadId(OpcUa_Void)
{
    return (unsigned long)pthread_self();
}
