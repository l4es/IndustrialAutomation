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

/* System Headers */
 #include <stdio.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

/* additional UA dependencies */
#include <opcua_p_mutex.h>
#include <opcua_p_thread.h>

/* own headers */
#include <opcua_p_trace.h>
#include <opcua_p_datetime.h>

OPCUA_EXPORT OpcUa_P_TraceHook g_OpcUa_P_TraceHook;

/*============================================================================
 * Trace Initialize
 *===========================================================================*/
/**
* Initialize all resources needed for tracing.
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Trace_Initialize(OpcUa_Void)
{
    return OpcUa_Good;
}

/*============================================================================
 * Trace Clear
 *===========================================================================*/
/**
* Clear all resources needed for tracing.
*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_Trace_Clear(OpcUa_Void)
{
    return;
}

/*============================================================================
 * Tracefunction
 *===========================================================================*/
/**
 * Writes the given string to the trace device, if the given trace level is
 * activated in the header file.
 */

OpcUa_Void OPCUA_DLLCALL OpcUa_P_Trace(
#if OPCUA_TRACE_FILE_LINE_INFO
                                        OpcUa_UInt32 level,
                                        OpcUa_CharA* sFile,
                                        OpcUa_UInt32 line,
#endif
                                        OpcUa_CharA* a_sMessage)
{
#if OPCUA_TRACE_FILE_LINE_INFO
    OpcUa_ReferenceParameter(level);
    OpcUa_ReferenceParameter(sFile);
    OpcUa_ReferenceParameter(line);
#endif

    /* send to tracehook if registered */
    if(g_OpcUa_P_TraceHook != OpcUa_Null)
    {
        g_OpcUa_P_TraceHook(a_sMessage);
    }
    else /* send to console */
    {
        char dtbuffer[25];
        OpcUa_DateTime timestamp;

        timestamp = OpcUa_P_DateTime_UtcNow();
        OpcUa_P_DateTime_GetStringFromDateTime(timestamp, dtbuffer, 25);

        printf("|%ld| %s %s", OpcUa_P_Thread_GetCurrentThreadId(), &dtbuffer[11], a_sMessage);
    }
}

