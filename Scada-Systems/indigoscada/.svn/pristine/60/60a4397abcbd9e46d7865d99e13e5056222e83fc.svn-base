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
#include <opcua_utilities.h>
#include <opcua_pkifactory.h>
#include <opcua_cryptofactory.h>
#include <opcua_core.h>

#if OPCUA_MUTEX_ERROR_CHECKING
#define OPCUA_MUTEX_ERROR_CHECKING_PARAMETERS ,__FILE__,__LINE__
#else
#define OPCUA_MUTEX_ERROR_CHECKING_PARAMETERS
#endif

/*********************************************************************************/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Mutex_Create(          OpcUa_Mutex* phNewMutex)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexCreate(phNewMutex OPCUA_MUTEX_ERROR_CHECKING_PARAMETERS);
}

OpcUa_Void OPCUA_DLLCALL OpcUa_Mutex_Delete(                OpcUa_Mutex* phMutex)
{
    OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexDelete(phMutex OPCUA_MUTEX_ERROR_CHECKING_PARAMETERS);
}

OpcUa_Void OPCUA_DLLCALL OpcUa_Mutex_Lock(                  OpcUa_Mutex hMutex)
{
    OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexLock(hMutex OPCUA_MUTEX_ERROR_CHECKING_PARAMETERS);
}

OpcUa_Void OPCUA_DLLCALL OpcUa_Mutex_Unlock(                OpcUa_Mutex hMutex)
{
    OpcUa_ProxyStub_g_PlatformLayerCalltable->MutexUnlock(hMutex OPCUA_MUTEX_ERROR_CHECKING_PARAMETERS);
}

/*********************************************************************************/
OpcUa_DateTime OPCUA_DLLCALL OpcUa_DateTime_UtcNow(         OpcUa_Void)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->UtcNow();
}

OpcUa_UInt32 OPCUA_DLLCALL OpcUa_Utility_GetTickCount(      OpcUa_Void)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->UtilGetTickCount();
}

/*********************************************************************************/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Semaphore_Create(      OpcUa_Semaphore*    phNewSemaphore,
                                                            OpcUa_UInt32        uInitalValue,
                                                            OpcUa_UInt32        uMaxRange)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreCreate(phNewSemaphore,
                                                                     uInitalValue,
                                                                     uMaxRange);
}

OpcUa_Void OPCUA_DLLCALL OpcUa_Semaphore_Delete(            OpcUa_Semaphore* phSemaphore)
{
    OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreDelete(phSemaphore);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Semaphore_Wait(        OpcUa_Semaphore hSemaphore)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreWait(hSemaphore);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Semaphore_TimedWait(   OpcUa_Semaphore     hSemaphore,
                                                            OpcUa_UInt32        msecTimeout)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphoreTimedWait(hSemaphore, msecTimeout);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Semaphore_Post(        OpcUa_Semaphore     hSemaphore,
                                                            OpcUa_UInt32        uReleaseCount)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SemaphorePost(hSemaphore, uReleaseCount);
}

/*********************************************************************************/

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SocketManager_Create(OpcUa_SocketManager*  ppSocketManager,
                                                          OpcUa_UInt32          nSockets,
                                                          OpcUa_UInt32          uintFlags)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketManagerCreate(ppSocketManager,
        nSockets,
        uintFlags);
}


OpcUa_Void       OPCUA_DLLCALL OpcUa_SocketManager_Delete(OpcUa_SocketManager* pSocketManager)
{
    OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketManagerDelete(pSocketManager);
}


OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SocketManager_CreateServer(OpcUa_SocketManager         pSocketManager,
                                                                OpcUa_StringA               LocalAdress,
                                                                OpcUa_Boolean               ListenOnAllInterfaces,
                                                                OpcUa_Socket_EventCallback  pfnSocketCallBack,
                                                                OpcUa_Void*                 pCookie,
                                                                OpcUa_Socket*               ppSocket)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketManagerCreateServer(
        pSocketManager,
        LocalAdress,
        ListenOnAllInterfaces,
        pfnSocketCallBack,
        pCookie,
        ppSocket);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SocketManager_CreateClient(OpcUa_SocketManager         pSocketManager,
                                                                OpcUa_StringA               RemoteAdress,
                                                                OpcUa_UInt16                LocalPort,
                                                                OpcUa_Socket_EventCallback  pfnSocketCallBack,
                                                                OpcUa_Void*                 pCookie,
                                                                OpcUa_Socket*               ppSocket)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketManagerCreateClient(
        pSocketManager,
        RemoteAdress,
        LocalPort,
        pfnSocketCallBack,
        pCookie,
        ppSocket);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Socket_Close(  OpcUa_Socket pSocket)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketClose(pSocket);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Socket_Read(   OpcUa_Socket    pSocket,
                                                    OpcUa_Byte*     pBuffer,
                                                    OpcUa_UInt32    BufferSize,
                                                    OpcUa_UInt32*   puintBytesRead)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketRead(    pSocket,
                                                                    pBuffer,
                                                                    BufferSize,
                                                                    puintBytesRead);
}

OpcUa_Int32      OPCUA_DLLCALL OpcUa_Socket_Write(  OpcUa_Socket    pSocket,
                                                    OpcUa_Byte*     pBuffer,
                                                    OpcUa_UInt32    BufferSize,
                                                    OpcUa_Boolean   bBlock)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketWrite(   pSocket,
                                                                    pBuffer,
                                                                    BufferSize,
                                                                    bBlock);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SocketManager_Loop(OpcUa_SocketManager pSocketManager,
                                                        OpcUa_UInt32        msecTimeout,
                                                        OpcUa_Boolean       bRunOnce)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketManagerServeLoop(
        pSocketManager,
        msecTimeout,
        bRunOnce);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_SocketManager_SignalEvent(OpcUa_SocketManager pSocketManager,
                                                               OpcUa_UInt32        uintEvent,
                                                               OpcUa_Boolean       bAllLists)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->SocketManagerSignalEvent(
        pSocketManager,
        uintEvent,
        bAllLists);
}

/*********************************************************************************/

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_PKIProvider_Create(    OpcUa_Void*         a_pCertificateStoreConfig,
                                                            OpcUa_PKIProvider*  a_pProvider)
{
    return OPCUA_P_PKIFACTORY_CREATEPKIPROVIDER(    a_pCertificateStoreConfig,
                                                    a_pProvider);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_PKIProvider_Delete(    OpcUa_PKIProvider*  a_pProvider)
{
    return OPCUA_P_PKIFACTORY_DELETEPKIPROVIDER(    a_pProvider);
}

/*********************************************************************************/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_CryptoProvider_Create( OpcUa_StringA           a_psSecurityProfileUri,
                                                            OpcUa_CryptoProvider*   a_pProvider)
{
    return OPCUA_P_CRYPTOFACTORY_CREATECRYPTOPROVIDER(  a_psSecurityProfileUri,
                                                        a_pProvider);
}

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_CryptoProvider_Delete(OpcUa_CryptoProvider*   a_pProvider)
{
    return OPCUA_P_CRYPTOFACTORY_DELETECRYPTOPROVIDER(  a_pProvider);
}
/*********************************************************************************/

OpcUa_Void OPCUA_DLLCALL OpcUa_ThreadCleanupOpenSSL()
{
    if(OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadCleanupOpenSSL != OpcUa_Null)
    {
        OpcUa_ProxyStub_g_PlatformLayerCalltable->ThreadCleanupOpenSSL();
    }
}

/*********************************************************************************/

OpcUa_StatusCode OPCUA_DLLCALL OpcUa_OpenSSLSeedPRNG(   OpcUa_Byte*  a_pEntropy,
                                                        OpcUa_UInt32 a_nBytes)
{
    OpcUa_StatusCode uStatus = OpcUa_BadNotImplemented;
    if(OpcUa_ProxyStub_g_PlatformLayerCalltable->OpenSSLSeedPRNG != OpcUa_Null)
    {
        uStatus = OpcUa_ProxyStub_g_PlatformLayerCalltable->OpenSSLSeedPRNG(a_pEntropy,
                                                                            a_nBytes);
    }
    return uStatus;
}

/*********************************************************************************/

OpcUa_Int32 OPCUA_DLLCALL OpcUa_StringA_vsnprintf(  OpcUa_StringA               a_sDest,
                                                    OpcUa_UInt32                a_uCount,
                                                    const OpcUa_StringA         a_sFormat,
                                                    varg_list                   a_argptr)
{
    return OpcUa_ProxyStub_g_PlatformLayerCalltable->StrVsnPrintf(  a_sDest,
                                                                    a_uCount,
                                                                    a_sFormat,
                                                                    a_argptr);
}

OpcUa_Int32 OPCUA_DLLCALL OpcUa_StringA_snprintf(   OpcUa_StringA               a_sDest,
                                                    OpcUa_UInt32                a_uCount,
                                                    const OpcUa_StringA         a_sFormat,
                                                    ...)
{
        OpcUa_Int32 ret = 0;
        varg_list argumentList;

        if(a_sDest == OpcUa_Null || a_uCount == 0 || a_sFormat == OpcUa_Null)
        {
            return -1;
        }

        VA_START(argumentList, a_sFormat);

        ret = OpcUa_ProxyStub_g_PlatformLayerCalltable->StrVsnPrintf(   a_sDest,
                                                                        a_uCount,
                                                                        a_sFormat,
                                                                        argumentList);
        VA_END(argumentList);

        return ret;
}

