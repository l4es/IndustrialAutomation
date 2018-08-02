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

#include <stdlib.h>
#include <time.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

#if OPCUA_REQUIRE_OPENSSL
#include <openssl/rand.h>
#endif

/* own headers */
#include <opcua_guid.h>
#include <opcua_p_guid.h>
/*============================================================================
 * CreateGuid
 *===========================================================================*/
/**
* CreateGuid generates a global unique identifier.
*/
OpcUa_Guid* OpcUa_P_Guid_Create(OpcUa_Guid* guid)
{
#if OPCUA_REQUIRE_OPENSSL
    if(RAND_bytes((unsigned char*)guid, sizeof(OpcUa_Guid)) <= 0)
    {
        return OpcUa_Null;
    }
#else
    unsigned int *data = (unsigned int*)guid;
    int chunks = 16 / sizeof(unsigned int);
    static const int intbits = sizeof(int)*8;
    static int randbits = 0;
    if(!randbits)
    {
        int max = RAND_MAX;
        do { ++randbits; } while ((max=max>>1));
        srand(time(NULL));
        rand(); /* Skip first */
    }

    while(chunks--)
    {
        unsigned int randNumber = 0;
        int filled;
        for (filled = 0; filled < intbits; filled += randbits)
            randNumber |= (unsigned int)rand()<<filled;
        memcpy(data+chunks, &randNumber, sizeof(randNumber));
    }
#endif /* OPCUA_REQUIRE_OPENSSL */

    guid->Data4[0] = (guid->Data4[0] & 0x3F) | 0x80; /* UV_DCE */
    guid->Data3 = (guid->Data3 & 0x0FFF) | 0x4000;   /* UV_Random */

    return guid;
}

