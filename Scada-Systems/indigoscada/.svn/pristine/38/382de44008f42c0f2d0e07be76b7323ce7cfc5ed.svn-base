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
#include <opcua_enumeratedtype.h>

/*============================================================================
 * OpcUa_EnumeratedType_FindName
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EnumeratedType_FindName(
    OpcUa_EnumeratedType* a_pType,
    OpcUa_Int32           a_nValue,
    OpcUa_StringA*        a_pName)
{
    OpcUa_UInt32 ii = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "EnumeratedType_FindName");

    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReturnErrorIfArgumentNull(a_pName);

    *a_pName = OpcUa_Null;

    for (ii = 0; a_pType->Values[ii].Name != OpcUa_Null; ii++)
    {
        if (a_pType->Values[ii].Value == a_nValue)
        {
            *a_pName = a_pType->Values[ii].Name;
            break;
        }
    }

    OpcUa_GotoErrorIfTrue(a_pType->Values[ii].Name == OpcUa_Null, OpcUa_BadInvalidArgument);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EnumeratedType_FindValue
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_EnumeratedType_FindValue(
    OpcUa_EnumeratedType* a_pType,
    OpcUa_StringA         a_sName,
    OpcUa_Int32*          a_pValue)
{
    OpcUa_UInt32 ii = 0;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "EnumeratedType_FindValue");

    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReturnErrorIfArgumentNull(a_sName);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);

    *a_pValue = 0;

    for (ii = 0; a_pType->Values[ii].Name != OpcUa_Null; ii++)
    {
        if (OpcUa_StrCmpA(a_pType->Values[ii].Name, a_sName) == 0)
        {
            *a_pValue = a_pType->Values[ii].Value;
            break;
        }
    }

    OpcUa_GotoErrorIfTrue(a_pType->Values[ii].Name == OpcUa_Null, OpcUa_BadInvalidArgument);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}
