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

#ifndef _OpcUa_H_
#define _OpcUa_H_

/******************************************************************/
/* STACK INTERNAL FILE - NOT MEANT TO BE INCLUDED BY APPLICATIONS */
/******************************************************************/

/* Collection of includes needed everywhere or at least very often.   */
/* All other includes MUST be included in the source file!            */
#include <opcua_platformdefs.h> /* includes typemapping of primitives */
#include <opcua_proxystub.h>
#include <opcua_statuscodes.h>
#include <opcua_stackstatuscodes.h>
#include <opcua_errorhandling.h>

#include <opcua_string.h>
#include <opcua_memory.h>
#include <opcua_trace.h>

/* platform interface and referenced stack files */
#include <opcua_types.h>       /* needed for some security related files in p_interface */
#include <opcua_crypto.h>      /* needed for some security related files in p_interface */
#include <opcua_pki.h>
#include <opcua_p_interface.h> /* standalone file */

/* Do not include headers in headers if not absolutely necessary.   */
/* This creates a cascade of includes up to the libraries API level.*/


OPCUA_BEGIN_EXTERN_C

/* import  */
extern OpcUa_Port_CallTable*            OpcUa_ProxyStub_g_PlatformLayerCalltable;
extern OpcUa_ProxyStubConfiguration     OpcUa_ProxyStub_g_Configuration;

/*============================================================================
 * OpcUa_ProxyStub_RegisterChannel
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_RegisterChannel();

/*============================================================================
 * OpcUa_ProxyStub_RegisterEndpoint
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_RegisterEndpoint();

/*============================================================================
 * OpcUa_ProxyStub_DeRegisterChannel
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_DeRegisterChannel();

/*============================================================================
 * OpcUa_ProxyStub_DeRegisterEndpoint
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_DeRegisterEndpoint();

OPCUA_END_EXTERN_C
#endif /* _OpcUa_H_ */
