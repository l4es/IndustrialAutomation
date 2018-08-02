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

#ifndef _OpcUa_Thread_H_
#define _OpcUa_Thread_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * Type Definition
 *===========================================================================*/

/**
 * @brief Thread main entry function definition.
 */
typedef OpcUa_Void (OpcUa_PfnThreadMain)(OpcUa_Void* pArgument);

/**
 * @brief Describes a thread handle.
 */

/*============================================================================
 * Type Management
 *===========================================================================*/

/**
 * @brief Create a thread.
 *
 * @param ppThread [in/out] Pointer to the thread handle. Contains the created thread or OpcUa_Null.
 *
 * @return An error code for the operation.
 */
OPCUA_EXPORT
OpcUa_StatusCode    OpcUa_Thread_Create(        OpcUa_Thread*        pThread,
                                                OpcUa_PfnThreadMain* pThreadMain,
                                                OpcUa_Void*          pThreadArgument);

/**
 * @brief Delete a thread.
 *
 * @param ppThread [in] Pointer to the thread handle.
 *
 * @return
 */
OPCUA_EXPORT
OpcUa_Void          OpcUa_Thread_Delete(        OpcUa_Thread* pThread);


/*============================================================================
 * Type Operations
 *===========================================================================*/

/**
 * @brief Start a Thread.
 *
 * @param Thread [in] The thread handle.
 *
 * @return An error code for the operation.
 */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_Thread_Start(            OpcUa_Thread   Thread);

/**
 * @brief Wait For Thread Shutdown.
 *
 * @param Thread        [in] The thread handle.
 * @param msecTimeout   [in] The maximum time to wait for shutdown.
 *
 * @return An error code for the operation.
 */
OPCUA_EXPORT
OpcUa_StatusCode OpcUa_Thread_WaitForShutdown(  OpcUa_Thread   Thread,
                                                OpcUa_UInt32    msecTimeout);


/**
 * @brief Lets the thread sleep for a certain amount of time.
 *
 * @param msecTimeout [in] The time in milliseconds to suspend the calling thread.
 */
OPCUA_EXPORT
OpcUa_Void OpcUa_Thread_Sleep(                  OpcUa_UInt32    msecTimeout);

/**
 * @brief Get the ID of the current thread.
 *
 * @return The thread ID.
 */
OPCUA_EXPORT
OpcUa_UInt32 OpcUa_Thread_GetCurrentThreadId();

/**
 * @brief Check if the main function of the given thread object is running.
 *        State may have already changed when function returns!
 *
 * @return OpcUa_True if running, OpcUa_False else.
 */
OPCUA_EXPORT
OpcUa_Boolean OpcUa_Thread_IsRunning(           OpcUa_Thread    hThread);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Thread_H_ */
