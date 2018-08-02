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

#include <opcua.h>
#include <opcua_list.h>
#include <opcua_utilities.h>

#include <opcua_timer.h>

/*============================================================================
 * Delete A Timer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Timer_Delete(OpcUa_Timer* a_phTimer)
{
    return OPCUA_P_TIMER_DELETE(a_phTimer);
}


/*============================================================================
 * Create A Timer
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Timer_Create(  OpcUa_Timer*            a_phTimer,
                                                    OpcUa_UInt32            a_msecInterval,
                                                    OpcUa_Timer_Callback*   a_fpTimerCallback,
                                                    OpcUa_Timer_Callback*   a_fpKillCallback,
                                                    OpcUa_Void*             a_pvCallbackData)
{
    return OPCUA_P_TIMER_CREATE(    a_phTimer,
                                    a_msecInterval,
                                    a_fpTimerCallback,
                                    a_fpKillCallback,
                                    a_pvCallbackData);
}

