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

#if OPCUA_MUTEX_ERROR_CHECKING
    #define OpcUa_P_Mutex_Create(xMutex)  OpcUa_P_Mutex_CreateImp( xMutex, __FILE__, __LINE__)
    #define OpcUa_P_Mutex_Delete(xMutex)  OpcUa_P_Mutex_DeleteImp( xMutex, __FILE__, __LINE__)
    #define OpcUa_P_Mutex_Lock(xMutex)    OpcUa_P_Mutex_LockImp(   xMutex, __FILE__, __LINE__)
    #define OpcUa_P_Mutex_Unlock(xMutex)  OpcUa_P_Mutex_UnlockImp( xMutex, __FILE__, __LINE__)
#else /* OPCUA_MUTEX_ERROR_CHECKING */
    #define OpcUa_P_Mutex_Create(xMutex)  OpcUa_P_Mutex_CreateImp( xMutex)
    #define OpcUa_P_Mutex_Delete(xMutex)  OpcUa_P_Mutex_DeleteImp( xMutex)
    #define OpcUa_P_Mutex_Lock(xMutex)    OpcUa_P_Mutex_LockImp(   xMutex)
    #define OpcUa_P_Mutex_Unlock(xMutex)  OpcUa_P_Mutex_UnlockImp( xMutex)
#endif /* OPCUA_MUTEX_ERROR_CHECKING */

#if OPCUA_MUTEX_ERROR_CHECKING
    OpcUa_StatusCode    OPCUA_DLLCALL OpcUa_P_Mutex_CreateImp(   OpcUa_Mutex*    phMutex, char* file, int line);
    OpcUa_Void          OPCUA_DLLCALL OpcUa_P_Mutex_DeleteImp(   OpcUa_Mutex*    phMutex, char* file, int line);
    OpcUa_Void          OPCUA_DLLCALL OpcUa_P_Mutex_LockImp(     OpcUa_Mutex     hMutex,  char* file, int line);
    OpcUa_Void          OPCUA_DLLCALL OpcUa_P_Mutex_UnlockImp(   OpcUa_Mutex     hMutex,  char* file, int line);
#else
    OpcUa_StatusCode    OPCUA_DLLCALL OpcUa_P_Mutex_CreateImp(   OpcUa_Mutex*    phMutex);
    OpcUa_Void          OPCUA_DLLCALL OpcUa_P_Mutex_DeleteImp(   OpcUa_Mutex*    phMutex);
    OpcUa_Void          OPCUA_DLLCALL OpcUa_P_Mutex_LockImp(     OpcUa_Mutex     hMutex);
    OpcUa_Void          OPCUA_DLLCALL OpcUa_P_Mutex_UnlockImp(   OpcUa_Mutex     hMutex);
#endif

