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
#include <opcua_datetime.h>

#define OpcUa_P_DateTime_UtcNow                 OpcUa_ProxyStub_g_PlatformLayerCalltable->UtcNow
#define OpcUa_P_DateTime_GetTimeOfDay           OpcUa_ProxyStub_g_PlatformLayerCalltable->GetTimeOfDay
#define OpcUa_P_DateTime_GetDateTimeFromString  OpcUa_ProxyStub_g_PlatformLayerCalltable->GetDateTimeFromString
#define OpcUa_P_DateTime_GetStringFromDateTime  OpcUa_ProxyStub_g_PlatformLayerCalltable->GetStringFromDateTime

/*============================================================================*/
OpcUa_StatusCode OpcUa_DateTime_GetTimeOfDay(OpcUa_TimeVal* a_pValue)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_DateTime);
    OpcUa_ReturnErrorIfArgumentNull(a_pValue);

    OpcUa_P_DateTime_GetTimeOfDay(a_pValue);

    return OpcUa_Good;
}

/*============================================================================*/
OpcUa_StatusCode OpcUa_DateTime_GetDateTimeFromString(  OpcUa_StringA   a_pchDateTimeString,
                                                        OpcUa_DateTime* a_pDateTime)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_DateTime);
    OpcUa_ReturnErrorIfArgumentNull(a_pchDateTimeString);
    OpcUa_ReturnErrorIfArgumentNull(a_pDateTime);

    return OpcUa_P_DateTime_GetDateTimeFromString(a_pchDateTimeString, a_pDateTime);
}

/*============================================================================*/
OpcUa_StatusCode OpcUa_DateTime_GetStringFromDateTime(  OpcUa_DateTime  a_dateTime,
                                                        OpcUa_StringA   a_pBuffer,
                                                        OpcUa_UInt32    a_uLength)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_DateTime);
    OpcUa_ReturnErrorIfArgumentNull(a_pBuffer);

    if(a_uLength < 25)
    {
        return OpcUa_BadInvalidArgument;
    }

    return OpcUa_P_DateTime_GetStringFromDateTime(a_dateTime, a_pBuffer, a_uLength);
}
