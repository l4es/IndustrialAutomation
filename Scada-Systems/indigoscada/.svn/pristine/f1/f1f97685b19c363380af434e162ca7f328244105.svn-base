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
#include <opcua_string.h>

#include <opcua_guid.h>

OpcUa_Guid OpcUa_Guid_Null = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

#define OpcUa_P_Guid_Create OpcUa_ProxyStub_g_PlatformLayerCalltable->GuidCreate

/*============================================================================
 * CreateGuid
 *===========================================================================*/
OpcUa_Guid* OpcUa_Guid_Create(OpcUa_Guid* a_pGuid)
{
    if(a_pGuid == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    a_pGuid = OpcUa_P_Guid_Create(a_pGuid);

    return a_pGuid;
}

/*============================================================================
 * OpcUa_Guid_ToString
 *===========================================================================*/
OpcUa_CharA* OpcUa_Guid_ToStringA(  OpcUa_Guid*     a_pGuid,
                                    OpcUa_CharA*    a_pchGuid)
{
    if((a_pGuid == OpcUa_Null) || (a_pchGuid == OpcUa_Null))
    {
        return OpcUa_Null;
    }

    OpcUa_SPrintfA( a_pchGuid,
#if OPCUA_USE_SAFE_FUNCTIONS
                    OPCUA_GUID_LEXICAL_LENGTH + 1,
#endif /* OPCUA_USE_SAFE_FUNCTIONS */
                    "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                    a_pGuid->Data1,
                    a_pGuid->Data2,
                    a_pGuid->Data3,
                    a_pGuid->Data4[0], a_pGuid->Data4[1],
                    a_pGuid->Data4[2], a_pGuid->Data4[3], a_pGuid->Data4[4],
                    a_pGuid->Data4[5], a_pGuid->Data4[6], a_pGuid->Data4[7]);

    return a_pchGuid;
}

/*============================================================================
 * GetStringFromGuid
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Guid_ToString(   OpcUa_Guid*     a_pGuid,
                                        OpcUa_String**  a_ppString)
{
    OpcUa_CharA pRawString[OPCUA_GUID_LEXICAL_LENGTH + 1];

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Guid);

    OpcUa_ReturnErrorIfArgumentNull(a_ppString);
    OpcUa_ReturnErrorIfArgumentNull(a_pGuid);

    *a_ppString = OpcUa_Null;

    OpcUa_SPrintfA( pRawString,
#if OPCUA_USE_SAFE_FUNCTIONS
                    OPCUA_GUID_LEXICAL_LENGTH + 1,
#endif /* OPCUA_USE_SAFE_FUNCTIONS */
                    "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                    a_pGuid->Data1,
                    a_pGuid->Data2,
                    a_pGuid->Data3,
                    a_pGuid->Data4[0], a_pGuid->Data4[1],
                    a_pGuid->Data4[2], a_pGuid->Data4[3], a_pGuid->Data4[4],
                    a_pGuid->Data4[5], a_pGuid->Data4[6], a_pGuid->Data4[7]);

    return OpcUa_String_CreateNewString(pRawString,
                                        OPCUA_GUID_LEXICAL_LENGTH,
                                        0,
                                        OpcUa_True,
                                        OpcUa_True,
                                        a_ppString);
}

/*============================================================================
 * GetGuidFromString
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Guid_FromString( OpcUa_CharA*    a_pText,
                                        OpcUa_Guid*     a_pGuid)
{
    OpcUa_Int32     nScanned    = 0;
    OpcUa_UInt16    Byte0       = 0;
    OpcUa_UInt16    Byte1       = 0;
    OpcUa_UInt16    Byte2       = 0;
    OpcUa_UInt16    Byte3       = 0;
    OpcUa_UInt16    Byte4       = 0;
    OpcUa_UInt16    Byte5       = 0;
    OpcUa_UInt16    Byte6       = 0;
    OpcUa_UInt16    Byte7       = 0;

    if(     (a_pText == OpcUa_Null)
        ||  (a_pGuid == OpcUa_Null))
    {
        return OpcUa_BadInvalidArgument;
    }

    nScanned = OpcUa_SScanfA(   a_pText,
                                "{%08x-%04hx-%04hx-%02hx%02hx-%02hx%02hx%02hx%02hx%02hx%02hx}",
                                &a_pGuid->Data1,
                                &a_pGuid->Data2,
                                &a_pGuid->Data3,
                                &Byte0, &Byte1,
                                &Byte2, &Byte3,
                                &Byte4, &Byte5,
                                &Byte6, &Byte7);

    a_pGuid->Data4[0] = (OpcUa_UCharA)Byte0;
    a_pGuid->Data4[1] = (OpcUa_UCharA)Byte1;
    a_pGuid->Data4[2] = (OpcUa_UCharA)Byte2;
    a_pGuid->Data4[3] = (OpcUa_UCharA)Byte3;
    a_pGuid->Data4[4] = (OpcUa_UCharA)Byte4;
    a_pGuid->Data4[5] = (OpcUa_UCharA)Byte5;
    a_pGuid->Data4[6] = (OpcUa_UCharA)Byte6;
    a_pGuid->Data4[7] = (OpcUa_UCharA)Byte7;

    if(nScanned != 11)
    {
        return OpcUa_BadInvalidArgument;
    }

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Guid_IsEqual
 *===========================================================================*/
OpcUa_Boolean OpcUa_Guid_IsEqual(   OpcUa_Guid* a_pGuid1,
                                    OpcUa_Guid* a_pGuid2)
{
    if(a_pGuid1 == a_pGuid2)
    {
        return OpcUa_True;
    }

    if(a_pGuid1 == OpcUa_Null || a_pGuid2 == OpcUa_Null)
    {
        return OpcUa_False;
    }

    if(OpcUa_MemCmp(a_pGuid1, a_pGuid2, sizeof(OpcUa_Guid)) == 0)
    {
        return OpcUa_True;
    }

    return OpcUa_False;
}

/*============================================================================
 * IsNullGuid
 *===========================================================================*/
OpcUa_Boolean OpcUa_Guid_IsNull(OpcUa_Guid* a_pGuid)
{
    if(a_pGuid == OpcUa_Null)
    {
        return OpcUa_True;
    }

    if(OpcUa_MemCmp(a_pGuid, &OpcUa_Guid_Null, sizeof(OpcUa_Guid)) == 0)
    {
        return OpcUa_True;
    }

    return OpcUa_False;
}

/*============================================================================
 * OpcUa_Guid_Copy
 *===========================================================================*/
OpcUa_Void OpcUa_Guid_Copy( OpcUa_Guid* a_pDestination,
                            OpcUa_Guid* a_pSource)
{
    if(     a_pDestination  == a_pSource
        ||  a_pDestination  == OpcUa_Null
        ||  a_pSource       == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemCpy(   a_pDestination,
                    sizeof(OpcUa_Guid),
                    a_pSource,
                    sizeof(OpcUa_Guid));
}

