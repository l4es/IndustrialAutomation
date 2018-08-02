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

#ifndef _OpcUa_ThreadPool_H_
#define _OpcUa_ThreadPool_H_ 1

#ifdef OPCUA_HAVE_THREADPOOL

/**
 * @brief Threadpool Handle.
 */
typedef OpcUa_Void* OpcUa_ThreadPool;

OPCUA_BEGIN_EXTERN_C

/**
 * @brief Create a thread pool with uMinThreads static threads and uMaxThreads - uMinThreads dynamic threads.
 */
OpcUa_StatusCode    OPCUA_DLLCALL OpcUa_ThreadPool_Create(      OpcUa_ThreadPool*       phThreadPool,
                                                                OpcUa_UInt32            uMinThreads,
                                                                OpcUa_UInt32            uMaxThreads,
                                                                OpcUa_UInt32            uMaxJobs,
                                                                OpcUa_Boolean           bBlockIfFull,
                                                                OpcUa_UInt32            uTimeout);

/**
 * @brief Destroy a thread pool.
 */
OpcUa_Void          OPCUA_DLLCALL OpcUa_ThreadPool_Delete(      OpcUa_ThreadPool*       phThreadPool);

/**
 * @brief Assing a job to a thread pool. The job may be queued for later execution.
 */
OpcUa_StatusCode    OPCUA_DLLCALL OpcUa_ThreadPool_AddJob(      OpcUa_ThreadPool        hThreadPool,
                                                                OpcUa_PfnThreadMain*    pFunction,
                                                                OpcUa_Void*             pArgument);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_THREADPOOL */

#endif /* _OpcUa_ThreadPool_H_ */

