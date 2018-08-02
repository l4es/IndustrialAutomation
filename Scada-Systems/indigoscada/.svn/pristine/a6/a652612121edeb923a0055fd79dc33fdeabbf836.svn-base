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

/*============================================================================
* Copy uintCount Characters from a OpcUa_StringA to another OpcUa_StringA
*===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_String_strncpy(OpcUa_StringA strDestination, OpcUa_UInt32 uiDestSize, OpcUa_StringA strSource, OpcUa_UInt32 uiLength);

/*============================================================================
* Append uintCount Characters from a OpcUa_String to another OpcUa_String.
*===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_String_strncat(OpcUa_StringA strDestination, OpcUa_UInt32 uiDestSize, OpcUa_StringA strSource, OpcUa_UInt32 uiLength);

/*============================================================================
* Get the Length from a OpcUa_String
*===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_String_strlen(OpcUa_StringA   a_pCString);

/*============================================================================
* Compare two OpcUa_Strings Case Sensitive
*===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_String_strncmp(OpcUa_StringA string1, OpcUa_StringA string2, OpcUa_UInt32 uiLength);

/*============================================================================
* Compare two OpcUa_Strings NOT Case Sensitive
*===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_String_strnicmp(OpcUa_StringA string1, OpcUa_StringA string2, OpcUa_UInt32 uiLength);

/*============================================================================
* Print a list of values into a string.
*===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_String_vsnprintf(OpcUa_StringA sDest, OpcUa_UInt32 uCount, const OpcUa_StringA sFormat, varg_list argptr);

/*============================================================================
* Print a list of values into a string with a max length.
*===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_String_snprintf(OpcUa_StringA sTarget, OpcUa_UInt32  nCount, OpcUa_StringA sFormat, ...);

