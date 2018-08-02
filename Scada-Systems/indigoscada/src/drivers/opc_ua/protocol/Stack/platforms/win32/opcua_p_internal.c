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
/* P-Layer internal helper functions.                                                                 */
/******************************************************************************************************/

/* System Headers */
#include <windows.h>
#include <stdlib.h>

/* UA platform definitions */
#include <opcua_p_internal.h>


/*============================================================================
 * Calculate DateTime Difference
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiff( OpcUa_DateTime  a_Value1,
                                                        OpcUa_DateTime  a_Value2,
                                                        OpcUa_DateTime* a_pResult)
{
    UINT64 ullValue1 = 0;
    UINT64 ullValue2 = 0;
    UINT64 ullResult = 0;

    OpcUa_ReturnErrorIfArgumentNull(a_pResult);

    a_pResult->dwLowDateTime = (OpcUa_UInt32)0;
    a_pResult->dwHighDateTime = (OpcUa_UInt32)0;

    ullValue1 = a_Value1.dwHighDateTime;
    ullValue1 = (ullValue1 << 32) + a_Value1.dwLowDateTime;

    ullValue2 = a_Value2.dwHighDateTime;
    ullValue2 = (ullValue2 << 32) + a_Value2.dwLowDateTime;

    if(ullValue1 > ullValue2)
    {
        return OpcUa_BadInvalidArgument;
    }

    ullResult = ullValue2 - ullValue1;

    a_pResult->dwLowDateTime = (OpcUa_UInt32)(ullResult & 0x00000000FFFFFFFF);
    a_pResult->dwHighDateTime = (OpcUa_UInt32)((ullResult & 0xFFFFFFFF00000000i64) >> 32);

    return OpcUa_Good;
}

/*============================================================================
 * Calculate DateTime Difference In Seconds (Rounded)
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiffInSeconds32(  OpcUa_DateTime  a_Value1,
                                                                    OpcUa_DateTime  a_Value2,
                                                                    OpcUa_UInt32*   a_puResult)
{
    UINT64 ullValue1 = 0;
    UINT64 ullValue2 = 0;
    UINT64 ullResult = 0;

    OpcUa_ReturnErrorIfArgumentNull(a_puResult);

    *a_puResult = (OpcUa_UInt32)0;

    ullValue1 = a_Value1.dwHighDateTime;
    ullValue1 = (ullValue1 << 32) + a_Value1.dwLowDateTime;

    ullValue2 = a_Value2.dwHighDateTime;
    ullValue2 = (ullValue2 << 32) + a_Value2.dwLowDateTime;

    if(ullValue1 > ullValue2)
    {
        return OpcUa_BadInvalidArgument;
    }

    ullResult = (UINT64)((ullValue2 - ullValue1 + 5000000) / 10000000);

    if(ullResult > (UINT64)OpcUa_UInt32_Max)
    {
        return OpcUa_BadOutOfRange;
    }

    *a_puResult = (OpcUa_UInt32)(ullResult & 0x00000000FFFFFFFF);

    return OpcUa_Good;
}
