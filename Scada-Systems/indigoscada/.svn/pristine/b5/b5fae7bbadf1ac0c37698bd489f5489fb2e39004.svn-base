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

#if OPCUA_MUTEX_ERROR_CHECKING
    #define OPCUA_P_MUTEX_CREATE(xMutex)  OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexCreate( xMutex, __FILE__, __LINE__)
    #define OPCUA_P_MUTEX_DELETE(xMutex)  OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexDelete( xMutex, __FILE__, __LINE__)
    #define OPCUA_P_MUTEX_LOCK(xMutex)    OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexLock(   xMutex, __FILE__, __LINE__)
    #define OPCUA_P_MUTEX_UNLOCK(xMutex)  OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexUnlock( xMutex, __FILE__, __LINE__)
#else /* OPCUA_MUTEX_ERROR_CHECKING */
#if OPCUA_USE_SYNCHRONISATION
    #define OPCUA_P_MUTEX_CREATE OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexCreate
    #define OPCUA_P_MUTEX_DELETE OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexDelete
    #define OPCUA_P_MUTEX_LOCK   OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexLock
    #define OPCUA_P_MUTEX_UNLOCK OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexUnlock
#else /* OPCUA_USE_SYNCHRONISATION */
    #define OPCUA_P_MUTEX_CREATE(xMutex) OpcUa_Good
    #define OPCUA_P_MUTEX_DELETE(xMutex)
    #define OPCUA_P_MUTEX_LOCK(xMutex)
    #define OPCUA_P_MUTEX_UNLOCK(xMutex)
#endif /* OPCUA_USE_SYNCHRONISATION */
#endif /* OPCUA_MUTEX_ERROR_CHECKING */

