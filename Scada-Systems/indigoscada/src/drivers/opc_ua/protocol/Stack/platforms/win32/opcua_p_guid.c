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
/* Modify the content of this file according to the guid implementation on your system or emulate     */
/* the uuid generation process in this file.                                                          */
/******************************************************************************************************/

/* System Headers */
#include <windows.h>
#include <time.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

/* own headers */
#include <opcua_guid.h>
#include <opcua_p_guid.h>
#include <opcua_p_datetime.h>

/*============================================================================
 * CreateGuid
 *===========================================================================*/
/**
* CreateGuid generates a global unique identifier. It calls the
* Win32 API function for doing this.
*/

#define _GUID_CREATE_NOT_AVAILABLE

OpcUa_Guid* OPCUA_DLLCALL OpcUa_P_Guid_Create(OpcUa_Guid* Guid)
{
#ifndef _GUID_CREATE_NOT_AVAILABLE
    if(UuidCreate((UUID*)Guid) != RPC_S_OK)
    {
        /* Error */
        Guid = OpcUa_Null;
        return OpcUa_Null;
    }

    /* Good */
    return Guid;
#else
    unsigned int *data = (unsigned int*)Guid;
    int chunks = 16 / sizeof(unsigned int);
    static const int intbits = sizeof(int)*8;
    static int randbits = 0;
    if (!randbits)
    {
        OpcUa_DateTime now;
        int max = RAND_MAX;
        do { ++randbits; } while ((max=max>>1));
        now = OpcUa_P_DateTime_UtcNow();
        srand(now.dwLowDateTime^now.dwHighDateTime);
        rand(); /* Skip first */
    }

    while (chunks--)
    {
        unsigned int randNumber = 0;
        int filled;
        for (filled = 0; filled < intbits; filled += randbits)
            randNumber |= rand()<<filled;
         *(data+chunks) = randNumber;
    }

    Guid->Data4[0] = (Guid->Data4[0] & 0x3F) | 0x80; /* UV_DCE */
    Guid->Data3 = (Guid->Data3 & 0x0FFF) | 0x4000;   /* UV_Random */

    return Guid;
#endif
}
