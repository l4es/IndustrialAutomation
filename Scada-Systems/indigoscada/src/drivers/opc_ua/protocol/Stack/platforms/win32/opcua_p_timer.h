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
/******************************************************************************************************/

#ifndef _OpcUa_P_Timer_H_
#define _OpcUa_P_Timer_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * Defines
 *===========================================================================*/
/** @brief The number of timers available to the system. */
#ifndef OPCUA_P_TIMER_NO_OF_TIMERS
#define OPCUA_P_TIMER_NO_OF_TIMERS    200
#endif

/*============================================================================
 * The Timer Type
 *===========================================================================*/
typedef struct _OpcUa_P_InternalTimer
{
    /** @brief  */
    OpcUa_Boolean           bUsed;
    /** @brief  */
    OpcUa_UInt32            msecInterval;
    /** @brief  */
    OpcUa_P_Timer_Callback* TimerCallback;
    /** @brief  */
    OpcUa_P_Timer_Callback* KillCallback;
    /** @brief  */
    OpcUa_Void*             CallbackData;
    /** @brief  */
    OpcUa_UInt32            uLastFired;
    /** @brief  */
    OpcUa_UInt32            uDueTime;

} OpcUa_P_InternalTimer;

/*============================================================================
 * Create A Timer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Timer_Create(    OpcUa_Timer*            phTimer,
                                                        OpcUa_UInt32            msecInterval,
                                                        OpcUa_P_Timer_Callback* fpTimerCallback,
                                                        OpcUa_P_Timer_Callback* fpKillCallback,
                                                        OpcUa_Void*             pvCallbackData);

/*============================================================================
 * Delete A Timer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Timer_Delete(    OpcUa_Timer*            phTimer);

/*============================================================================
 * Initialize the Timer System
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_InitializeTimers(void);

/*============================================================================
 * Cleanup the Timer System
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Timer_CleanupTimers(void);

#if !OPCUA_MULTITHREADED
/*============================================================================
 * Select wrapper used in singlethreaded configuration.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_Socket_TimeredSelect(          OpcUa_RawSocket         MaxFds,
                                                        OpcUa_P_Socket_Array*   pFdSetRead,
                                                        OpcUa_P_Socket_Array*   pFdSetWrite,
                                                        OpcUa_P_Socket_Array*   pFdSetException,
                                                        OpcUa_UInt32            uTimeout);
#endif /* OPCUA_MULTITHREADED */

OPCUA_END_EXTERN_C
#endif /* _OpcUa_P_Timer_H_ */

