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
#include <opcua_decoder.h>

/*============================================================================
 * OpcUa_Decoder_Close
 *===========================================================================*/
OpcUa_Void OpcUa_Decoder_Close(
    struct _OpcUa_Decoder* a_pDecoder,
    OpcUa_Handle* a_phDecodeContext)
{
    if (a_pDecoder != OpcUa_Null && a_pDecoder->Delete != OpcUa_Null)
    {
        a_pDecoder->Close(a_pDecoder, a_phDecodeContext);
    }
}

/*============================================================================
 * OpcUa_Decoder_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Decoder_Delete(
    struct _OpcUa_Decoder** a_ppDecoder)
{
    if (a_ppDecoder != OpcUa_Null && *a_ppDecoder != OpcUa_Null && (*a_ppDecoder)->Delete != OpcUa_Null)
    {
        (*a_ppDecoder)->Delete(a_ppDecoder);
    }
}
