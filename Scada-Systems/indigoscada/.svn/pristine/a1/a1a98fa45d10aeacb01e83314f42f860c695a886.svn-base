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

#if OPCUA_USE_SYNCHRONISATION
# define OPCUA_P_SEMAPHORE_CREATE    OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreCreate
# define OPCUA_P_SEMAPHORE_DELETE    OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreDelete
# define OPCUA_P_SEMAPHORE_WAIT      OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreWait
# define OPCUA_P_SEMAPHORE_TIMEDWAIT OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreTimedWait
# define OPCUA_P_SEMAPHORE_POST      OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphorePost
#else /* OPCUA_USE_SYNCHRONISATION */
# define OPCUA_P_SEMAPHORE_CREATE(a, b, c) OpcUa_Good
# define OPCUA_P_SEMAPHORE_DELETE(a)
# define OPCUA_P_SEMAPHORE_WAIT(a) OpcUa_Good
# define OPCUA_P_SEMAPHORE_TIMEDWAIT(a, b) OpcUa_Good
# define OPCUA_P_SEMAPHORE_POST(a, b) OpcUa_Good
#endif /* OPCUA_USE_SYNCHRONISATION */

