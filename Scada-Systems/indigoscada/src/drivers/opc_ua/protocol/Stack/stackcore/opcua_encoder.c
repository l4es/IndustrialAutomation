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
#include <opcua_encoder.h>

/*============================================================================
 * OpcUa_Encoder_Close
 *===========================================================================*/
OpcUa_Void OpcUa_Encoder_Close(
    struct _OpcUa_Encoder*  a_pEncoder,
    OpcUa_Handle*           a_phEncodeContext)
{
    if (a_pEncoder != OpcUa_Null && a_pEncoder->Delete != OpcUa_Null)
    {
        a_pEncoder->Close(a_pEncoder, a_phEncodeContext);
    }
}

/*============================================================================
 * OpcUa_Encoder_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Encoder_Delete(
    struct _OpcUa_Encoder** a_ppEncoder)
{
    if (a_ppEncoder != OpcUa_Null && *a_ppEncoder != OpcUa_Null && (*a_ppEncoder)->Delete != OpcUa_Null)
    {
        (*a_ppEncoder)->Delete(a_ppEncoder);
    }
}
