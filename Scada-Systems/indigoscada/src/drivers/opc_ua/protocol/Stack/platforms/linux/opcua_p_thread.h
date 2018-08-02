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

/*============================================================================
 * Create a platform thread
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_P_Thread_Create(      OpcUa_RawThread* pThread);

/*============================================================================
 * Delete Raw Thread
 *===========================================================================*/
OpcUa_Void          OpcUa_P_Thread_Delete(      OpcUa_RawThread* pRawThread);

/*============================================================================
 * Start Thread
 *===========================================================================*/
OpcUa_StatusCode    OpcUa_P_Thread_Start(   OpcUa_RawThread             pThread,
                                            OpcUa_PfnInternalThreadMain pfnStartFunction,
                                            OpcUa_Void*                 pArguments);

/*============================================================================
 * Send the thread to sleep.
 *===========================================================================*/
OpcUa_Void          OpcUa_P_Thread_Sleep(   OpcUa_UInt32                msecTimeout);

/*============================================================================
 * Get Current Thread Id
 *===========================================================================*/
/* the return type "unsigned long" is necessary to hold a "pthread_t" value */
unsigned long       OpcUa_P_Thread_GetCurrentThreadId(  OpcUa_Void);

