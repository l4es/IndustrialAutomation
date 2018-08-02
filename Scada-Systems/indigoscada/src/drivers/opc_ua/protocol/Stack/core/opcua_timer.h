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

#ifndef _OpcUa_Timer_H_
#define _OpcUa_Timer_H_ 1

OPCUA_BEGIN_EXTERN_C

#define OPCUA_P_TIMER_CREATE  OpcUa_ProxyStub_g_PlatformLayerCalltable->TimerCreate
#define OPCUA_P_TIMER_DELETE  OpcUa_ProxyStub_g_PlatformLayerCalltable->TimerDelete
#define OPCUA_P_CLEANUPTIMERS OpcUa_ProxyStub_g_PlatformLayerCalltable->TimersCleanup

typedef OpcUa_StatusCode (OPCUA_DLLCALL OpcUa_Timer_Callback)(  OpcUa_Void*             pvCallbackData,
                                                                OpcUa_Timer             hTimer,
                                                                OpcUa_UInt32            msecElapsed);

OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Timer_Create( OpcUa_Timer*            hTimer,
                                                                OpcUa_UInt32            msecInterval,
                                                                OpcUa_Timer_Callback*   fpTimerCallback,
                                                                OpcUa_Timer_Callback*   fpKillCallback,
                                                                OpcUa_Void*             pvCallbackData);

OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Timer_Delete( OpcUa_Timer*            phTimer);

OPCUA_END_EXTERN_C

#endif /*_OpcUa_Timer_H_ */
