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
/* Modify the content of this file according to the socket implementation on your system.             */
/******************************************************************************************************/

#ifdef _MSC_VER
/* Disables warning for non secure functions in visual studio 2005. Debug only! */
#pragma warning(disable:4996) /* safe_functions */
#endif /* _MSC_VER */

/* System Headers */
#include <windows.h>
#include <stdio.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

/* additional UA dependencies */
#include <opcua_p_mutex.h>
#include <opcua_p_thread.h>

#if OPCUA_P_TRACE_ENABLE_TIME
  #include <opcua_p_datetime.h>
#endif /* OPCUA_P_TRACE_ENABLE_TIME */

/* own headers */
#include <opcua_p_trace.h>

OPCUA_EXPORT OpcUa_P_TraceHook g_OpcUa_P_TraceHook;

#ifndef _WIN32_WCE
  /* comment this out to disable debug console output. */
  #define OPCUA_P_ENABLE_VS_CONSOLE 1
#endif /* _WIN32_WCE */

#ifdef _WIN32_WCE
int OpcUa_Rename(const char* oldname, const char* newname);
int OpcUa_Unlink(const char* filename);
#else
#define OpcUa_Rename  rename
// _MSC_VER 1300 = VS2003
// _MSC_VER 1400 = VS2005
#if _MSC_VER < 1400
#define OpcUa_Unlink  unlink
#else
#define OpcUa_Unlink  _unlink
#endif /* _MSC_VER < 1400 */
#endif

#if OPCUA_P_TRACE_TO_FILE
    FILE*           OpcUa_P_Trace_g_hOutFile                = NULL;
    unsigned int    OpcUa_P_Trace_g_hOutFileNoOfEntries     = 0;
    unsigned int    OpcUa_P_Trace_g_hOutFileNoOfEntriesMax  = OPCUA_P_TRACE_G_MAX_FILE_ENTRIES;
#endif /* OPCUA_P_TRACE_TO_FILE */

/*============================================================================
 * Trace Initialize
 *===========================================================================*/
/**
* Initialize all resources needed for tracing.
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_Trace_Initialize(OpcUa_Void)
{
#if OPCUA_P_TRACE_TO_FILE
    OpcUa_P_Trace_g_hOutFile = fopen(OPCUA_P_TRACE_G_OUTFILE, "w");
#endif /* OPCUA_P_TRACE_TO_FILE */

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
#if OPCUA_P_TRACE_TO_FILE
    fflush(OpcUa_P_Trace_g_hOutFile);
    fclose(OpcUa_P_Trace_g_hOutFile);
    OpcUa_P_Trace_g_hOutFile = NULL;
#endif /* OPCUA_P_TRACE_TO_FILE */

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
#ifdef OPCUA_P_ENABLE_VS_CONSOLE
        /* visual studio debug console output */
        char buffer[20];
#endif /* OPCUA_P_ENABLE_VS_CONSOLE */

#if OPCUA_P_TRACE_ENABLE_TIME
        char dtbuffer[25];
#endif

#if OPCUA_P_TRACE_ENABLE_TIME
        OpcUa_P_DateTime_GetStringFromDateTime(OpcUa_P_DateTime_UtcNow(), dtbuffer, 25);
#endif /* OPCUA_P_TRACE_ENABLE_TIME */

#ifdef OPCUA_P_ENABLE_VS_CONSOLE
        /* visual studio debug console output */
        _snprintf(buffer, 20, "|%d| ", OpcUa_P_Thread_GetCurrentThreadId());
#if OPCUA_P_TRACE_ENABLE_TIME
        OutputDebugStringA(dtbuffer);
#endif /* OPCUA_P_TRACE_ENABLE_TIME */
        OutputDebugStringA(buffer);
        OutputDebugStringA(a_sMessage);
#endif /* OPCUA_P_ENABLE_VS_CONSOLE */

#ifndef OPCUA_P_TRACE_ENABLE_TIME
        printf("|%d| %s", OpcUa_P_Thread_GetCurrentThreadId(), a_sMessage);
#else
        printf("|%d| %s %s", OpcUa_P_Thread_GetCurrentThreadId(), &dtbuffer[11], a_sMessage);
#endif /* OPCUA_P_TRACE_ENABLE_TIME */

#if OPCUA_P_TRACE_TO_FILE
        if(OpcUa_P_Trace_g_hOutFile != NULL)
        {
            fprintf(OpcUa_P_Trace_g_hOutFile, "|%d| %s %s", OpcUa_P_Thread_GetCurrentThreadId(), &dtbuffer[11], a_sMessage);
#if OPCUA_P_TRACE_FFLUSH_IMMEDIATELY
            fflush(OpcUa_P_Trace_g_hOutFile);
#endif
            OpcUa_P_Trace_g_hOutFileNoOfEntries++;
        }
        if(OpcUa_P_Trace_g_hOutFileNoOfEntries >= OpcUa_P_Trace_g_hOutFileNoOfEntriesMax)
        {
            /* delete backup store and rename current file and create new one */
            fflush(OpcUa_P_Trace_g_hOutFile);
            fclose(OpcUa_P_Trace_g_hOutFile);
            OpcUa_P_Trace_g_hOutFile = NULL;
            OpcUa_Unlink(OPCUA_P_TRACE_G_OUTFILE_BACKUP);
            OpcUa_Rename(OPCUA_P_TRACE_G_OUTFILE, OPCUA_P_TRACE_G_OUTFILE_BACKUP);
            OpcUa_P_Trace_g_hOutFile = fopen(OPCUA_P_TRACE_G_OUTFILE, "w");
            OpcUa_P_Trace_g_hOutFileNoOfEntries = 0;
        }
#endif /* OPCUA_P_TRACE_TO_FILE */
    }
}

#ifdef _WIN32_WCE
int OpcUa_Unlink(const char* filename)
{
    WCHAR lpFileName[MAX_PATH];
    DWORD   i;
    BOOL    result;

    for ( i=0; i<MAX_PATH; i++ )
    {
        lpFileName[i] = (char)filename[i];
        if ( filename[i] == 0 )
        {
            break;
        }
    }
    result = DeleteFile(lpFileName);
    if ( result == 0 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
int OpcUa_Rename(const char* oldname, const char* newname)
{
#ifdef  UNICODE
    WCHAR lpExistingFileName[MAX_PATH];
    WCHAR lpNewFileName[MAX_PATH];
    DWORD   i;
    BOOL    result;
    for ( i=0; i<MAX_PATH; i++ )
    {
        lpExistingFileName[i] = (char)oldname[i];
        if ( oldname[i] == 0 )
        {
            break;
        }
    }
    for ( i=0; i<MAX_PATH; i++ )
    {
        lpNewFileName[i] = (char)newname[i];
        if ( newname[i] == 0 )
        {
            break;
        }
    }
#else   /* UNICODE */
    LPCTSTR lpExistingFileName = oldname;
    LPCTSTR lpNewFileName = newname;
    BOOL    result;
#endif /* UNICODE */
    result = MoveFile(lpExistingFileName, lpNewFileName);
    if ( result == 0 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
#endif /* _WIN32_WCE */

