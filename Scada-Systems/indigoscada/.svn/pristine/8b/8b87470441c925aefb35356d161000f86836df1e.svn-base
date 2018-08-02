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

#ifndef _OpcUa_Trace_H_
#define _OpcUa_Trace_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * Trace Levels
 *===========================================================================*/
/* custom trace levels - add your trace levels here ... */
#define OPCUA_TRACE_LEVEL_YOURTRACELEVEL 0x00000040
/* ... */

/* predefined trace levels */
#define OPCUA_TRACE_LEVEL_ERROR         0x00000020 /* in-system errors, which require bugfixing        */
#define OPCUA_TRACE_LEVEL_WARNING       0x00000010 /* in-system warnings and extern errors             */
#define OPCUA_TRACE_LEVEL_SYSTEM        0x00000008 /* rare system messages (start, stop, connect)      */
#define OPCUA_TRACE_LEVEL_INFO          0x00000004 /* more detailed information about system events    */
#define OPCUA_TRACE_LEVEL_DEBUG         0x00000002 /* information needed for debug reasons             */
#define OPCUA_TRACE_LEVEL_CONTENT       0x00000001 /* all message content                              */

/* trace level packages */
#define OPCUA_TRACE_OUTPUT_LEVEL_ERROR   (OPCUA_TRACE_LEVEL_ERROR)
#define OPCUA_TRACE_OUTPUT_LEVEL_WARNING (OPCUA_TRACE_LEVEL_ERROR | OPCUA_TRACE_LEVEL_WARNING)
#define OPCUA_TRACE_OUTPUT_LEVEL_SYSTEM  (OPCUA_TRACE_LEVEL_ERROR | OPCUA_TRACE_LEVEL_WARNING | OPCUA_TRACE_LEVEL_SYSTEM)
#define OPCUA_TRACE_OUTPUT_LEVEL_INFO    (OPCUA_TRACE_LEVEL_ERROR | OPCUA_TRACE_LEVEL_WARNING | OPCUA_TRACE_LEVEL_SYSTEM | OPCUA_TRACE_LEVEL_INFO)
#define OPCUA_TRACE_OUTPUT_LEVEL_DEBUG   (OPCUA_TRACE_LEVEL_ERROR | OPCUA_TRACE_LEVEL_WARNING | OPCUA_TRACE_LEVEL_SYSTEM | OPCUA_TRACE_LEVEL_INFO | OPCUA_TRACE_LEVEL_DEBUG)
#define OPCUA_TRACE_OUTPUT_LEVEL_CONTENT (OPCUA_TRACE_LEVEL_ERROR | OPCUA_TRACE_LEVEL_WARNING | OPCUA_TRACE_LEVEL_SYSTEM | OPCUA_TRACE_LEVEL_INFO | OPCUA_TRACE_LEVEL_DEBUG | OPCUA_TRACE_LEVEL_CONTENT)
#define OPCUA_TRACE_OUTPUT_LEVEL_ALL     (0xFFFFFFFF)
#define OPCUA_TRACE_OUTPUT_LEVEL_NONE    (0x00000000)
/*============================================================================
 * Trace Initialize
 *===========================================================================*/
/**
* Initialize all resources needed for tracing.
*/
OPCUA_EXPORT OpcUa_StatusCode OPCUA_DLLCALL OpcUa_Trace_Initialize();

/*============================================================================
 * Trace Initialize
 *===========================================================================*/
/**
* Clear all resources needed for tracing.
*/
OPCUA_EXPORT OpcUa_Void OPCUA_DLLCALL OpcUa_Trace_Clear();

/*============================================================================
 * Change Trace Level
 *===========================================================================*/
/**
 * Activate or deactivate trace output during runtime.
 */
OPCUA_EXPORT OpcUa_Void OPCUA_DLLCALL OpcUa_Trace_ChangeTraceLevel(OpcUa_UInt32 a_uNewTraceLevel);

/*============================================================================
 * Activate/Deactivate Trace
 *===========================================================================*/
/**
 * Activate or deactivate trace output during runtime.
 */
OPCUA_EXPORT OpcUa_Void OPCUA_DLLCALL OpcUa_Trace_Toggle(OpcUa_Boolean a_bActive);

/*============================================================================
 * Tracefunction
 *===========================================================================*/
/**
* @brief Writes the given string and the parameters to the trace device, if the given
* trace level is activated in the header file.
*
* @see OpcUa_P_Trace
*
* @return The number of bytes written to the trace device.
*/
#if OPCUA_TRACE_ENABLE
 #if OPCUA_TRACE_FILE_LINE_INFO
  #define OpcUa_Trace(xLevel, ...) OpcUa_Trace_Imp(xLevel, __FILE__, __LINE__, __VA_ARGS__)
 #else /* OPCUA_TRACE_FILE_LINE_INFO */
  #define OpcUa_Trace OpcUa_Trace_Imp
 #endif /* OPCUA_TRACE_FILE_LINE_INFO */
#else /* OPCUA_TRACE_ENABLE */
#ifdef _MSC_VER
 #if _MSC_VER >= 1400
  #define OpcUa_Trace(x,y,...) ((void)0)
 #else
  #define OpcUa_Trace OpcUa_Trace_Nop
 #endif
#elif defined(__GNUC__)
#define OpcUa_Trace(x,...)
#else
#define OpcUa_Trace OpcUa_Trace_Nop
#endif
#endif /* OPCUA_TRACE_ENABLE */

OPCUA_EXPORT OpcUa_Boolean OPCUA_DLLCALL OpcUa_Trace_Imp(
    OpcUa_UInt32 uTraceLevel,
#if OPCUA_TRACE_FILE_LINE_INFO
    OpcUa_CharA* sFile,
    OpcUa_UInt32 sLine,
#endif /* OPCUA_TRACE_FILE_LINE_INFO */
    OpcUa_CharA* sFormat,
    ...);

OPCUA_EXPORT OpcUa_Boolean OPCUA_DLLCALL OpcUa_Trace_Nop(
    OpcUa_UInt32 uTraceLevel,
#if OPCUA_TRACE_FILE_LINE_INFO
    OpcUa_CharA* sFile,
    OpcUa_UInt32 sLine,
#endif /* OPCUA_TRACE_FILE_LINE_INFO */
    OpcUa_CharA* sFormat,
    ...);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Trace_H_ */
