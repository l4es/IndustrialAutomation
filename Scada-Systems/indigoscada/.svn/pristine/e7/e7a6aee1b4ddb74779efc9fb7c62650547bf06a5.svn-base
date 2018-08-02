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

#include <opcua_mutex.h>
#include <opcua_thread.h>

#include <opcua_trace.h>

#define OPCUA_P_TRACE               OpcUa_ProxyStub_g_PlatformLayerCalltable->Trace
#define OPCUA_P_TRACE_INITIALIZE    OpcUa_ProxyStub_g_PlatformLayerCalltable->TraceInitialize
#define OPCUA_P_TRACE_CLEAR         OpcUa_ProxyStub_g_PlatformLayerCalltable->TraceClear

#define OPCUA_P_STRINGA_VSNPRINTF   OpcUa_ProxyStub_g_PlatformLayerCalltable->StrVsnPrintf

/*============================================================================
 * Trace Lock
 *===========================================================================*/
/**
* Global Trace Buffer.
*/
OpcUa_CharA OpcUa_Trace_g_aTraceBuffer[OPCUA_TRACE_MAXLENGTH];

/*============================================================================
 * Trace Lock
 *===========================================================================*/
/**
* Global Mutex to synchronize access to the trace device.
*/
#if OPCUA_USE_SYNCHRONISATION
OpcUa_Mutex OpcUa_Trace_s_pLock = OpcUa_Null;
#endif /* OPCUA_USE_SYNCHRONISATION */


/*============================================================================
 * Trace Initialize
 *===========================================================================*/
/**
* Initialize all resources needed for tracing.
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Trace_Initialize(OpcUa_Void)
{
    OpcUa_StatusCode    uStatus = OpcUa_Good;

#if OPCUA_USE_SYNCHRONISATION
    uStatus = OPCUA_P_MUTEX_CREATE(&OpcUa_Trace_s_pLock);
    OpcUa_ReturnErrorIfBad(uStatus);
#endif /* OPCUA_USE_SYNCHRONISATION */

    uStatus = OPCUA_P_TRACE_INITIALIZE();

    return uStatus;
}

/*============================================================================
 * Trace Clear
 *===========================================================================*/
/**
* Clear all resources needed for tracing.
*/
OpcUa_Void OPCUA_DLLCALL OpcUa_Trace_Clear(OpcUa_Void)
{
#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_DELETE(&OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */
    OPCUA_P_TRACE_CLEAR();
}

/*============================================================================
 * Activate/Deactivate Trace
 *===========================================================================*/
/**
 * Activate or deactivate trace output during runtime.
 * @param a_bActive Description
 */
OpcUa_Void OPCUA_DLLCALL OpcUa_Trace_Toggle(OpcUa_Boolean a_bActive)
{
#if OPCUA_USE_SYNCHRONISATION
    if(OpcUa_Trace_s_pLock == OpcUa_Null)
    {
        return;
    }
    OPCUA_P_MUTEX_LOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* check if app wants trace output */
    OpcUa_ProxyStub_g_Configuration.bProxyStub_Trace_Enabled = a_bActive;

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */

    return;
}

/*============================================================================
 * Change Trace Level
 *===========================================================================*/
/**
 * Activate or deactivate trace output during runtime.
 * @param a_uNewTraceLevel Description
 */
OpcUa_Void OPCUA_DLLCALL OpcUa_Trace_ChangeTraceLevel(OpcUa_UInt32 a_uNewTraceLevel)
{
#if OPCUA_USE_SYNCHRONISATION
    if(OpcUa_Trace_s_pLock == OpcUa_Null)
    {
        return;
    }
    OPCUA_P_MUTEX_LOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_ProxyStub_g_Configuration.uProxyStub_Trace_Level = a_uNewTraceLevel;

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */

    return;
}

OpcUa_Boolean OPCUA_DLLCALL OpcUa_Trace_Nop(OpcUa_UInt32     a_uTraceLevel,
#if OPCUA_TRACE_FILE_LINE_INFO
                                            OpcUa_CharA*     a_sFile,
                                            OpcUa_UInt32     a_sLine,
#endif /* OPCUA_TRACE_FILE_LINE_INFO */
                                            OpcUa_CharA*     a_sFormat,
                                            ...)
{
    OpcUa_ReferenceParameter(a_uTraceLevel);
    OpcUa_ReferenceParameter(a_sFormat);
#if OPCUA_TRACE_FILE_LINE_INFO
    OpcUa_ReferenceParameter(a_sFile);
    OpcUa_ReferenceParameter(a_sLine);
#endif /* OPCUA_TRACE_FILE_LINE_INFO */

    return OpcUa_False;
}

/*============================================================================
 * Tracefunction
 *===========================================================================*/
/**
* Writes the given string and the parameters to the trace device, if the given
* trace level is activated.
*/
OpcUa_Boolean OPCUA_DLLCALL OpcUa_Trace_Imp(OpcUa_UInt32    a_uTraceLevel,
#if OPCUA_TRACE_FILE_LINE_INFO
                                            OpcUa_CharA*    a_sFile,
                                            OpcUa_UInt32    a_sLine,
#endif /* OPCUA_TRACE_FILE_LINE_INFO */
                                            OpcUa_CharA*    a_sFormat,
                                            ...)
{
#if OPCUA_TRACE_ENABLE
    OpcUa_Boolean bTraced = OpcUa_False;

#if OPCUA_USE_SYNCHRONISATION
    if(OpcUa_Trace_s_pLock == OpcUa_Null)
    {
        return OpcUa_False;
    }

    OPCUA_P_MUTEX_LOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */

    /* check if app wants trace output */
    if(OpcUa_ProxyStub_g_Configuration.bProxyStub_Trace_Enabled == OpcUa_False)
    {
#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */
        return OpcUa_False;
    }

    if(a_uTraceLevel & OpcUa_ProxyStub_g_Configuration.uProxyStub_Trace_Level)
    {
        varg_list argumentList;
        VA_START(argumentList, a_sFormat);

        OPCUA_P_STRINGA_VSNPRINTF(OpcUa_Trace_g_aTraceBuffer,
                                  OPCUA_TRACE_MAXLENGTH,
                                  a_sFormat,
                                  argumentList);

        /* send trace buffer to platform trace device */
#if OPCUA_TRACE_FILE_LINE_INFO
        OPCUA_P_TRACE(a_uTraceLevel, a_sFile, a_sLine, OpcUa_Trace_g_aTraceBuffer);
#else
        OPCUA_P_TRACE(OpcUa_Trace_g_aTraceBuffer);
#endif
        bTraced = OpcUa_True;
        VA_END(argumentList);
    }

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(OpcUa_Trace_s_pLock);
#endif /* OPCUA_USE_SYNCHRONISATION */

    return bTraced;

#else /* OPCUA_TRACE_ENABLE == NO */

    OpcUa_ReferenceParameter(a_uTraceLevel);
    OpcUa_ReferenceParameter(a_sFormat);
#if OPCUA_TRACE_FILE_LINE_INFO
    OpcUa_ReferenceParameter(a_sFile);
    OpcUa_ReferenceParameter(a_sLine);
#endif /* OPCUA_TRACE_FILE_LINE_INFO */

    return OpcUa_False;

#endif /* if OPCUA_TRACE_ENABLE == YES */
}

