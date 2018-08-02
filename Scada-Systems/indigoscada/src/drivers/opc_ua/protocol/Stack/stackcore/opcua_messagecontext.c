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
#include <opcua_messagecontext.h>

/*============================================================================
 * OpcUa_MessageContext_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_MessageContext_Initialize(OpcUa_MessageContext* a_pContext)
{
    if (a_pContext != OpcUa_Null)
    {
        OpcUa_MemSet(a_pContext, 0, sizeof(OpcUa_MessageContext));

        a_pContext->MaxArrayLength      = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxArrayLength;
        a_pContext->MaxStringLength     = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength;
        a_pContext->MaxByteStringLength = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxByteStringLength;
        a_pContext->MaxMessageLength    = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize;
        a_pContext->MaxRecursionDepth   = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxRecursionDepth;
    }
}

/*============================================================================
 * OpcUa_MessageContext_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_MessageContext_Clear(OpcUa_MessageContext* a_pContext)
{
    if (a_pContext != OpcUa_Null)
    {
        OpcUa_MemSet(a_pContext, 0, sizeof(OpcUa_MessageContext));
    }
}
