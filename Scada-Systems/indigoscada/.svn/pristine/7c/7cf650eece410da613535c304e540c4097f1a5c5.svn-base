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

#include <opcua_builtintypes.h>

#ifndef _OpcUa_BinaryEncoderInternal_H_
#define _OpcUa_BinaryEncoderInternal_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * OpcUa_NodeEncoding
 *===========================================================================*/
/** @brief Defines the possible node id encoding values. */
typedef enum _OpcUa_NodeEncoding
{
    OpcUa_NodeEncoding_TwoByte = 0x00,
    OpcUa_NodeEncoding_FourByte = 0x01,
    OpcUa_NodeEncoding_Numeric = 0x02,
    OpcUa_NodeEncoding_String = 0x03,
    OpcUa_NodeEncoding_Guid = 0x04,
    OpcUa_NodeEncoding_ByteString = 0x05,
    OpcUa_NodeEncoding_UriMask = 0x80,
    OpcUa_NodeEncoding_ServerIndexMask = 0x40,
    OpcUa_NodeEncoding_TypeMask = 0x3F
}
OpcUa_NodeEncoding;

/*============================================================================
 * OpcUa_DiagnosticInfo_EncodingByte
 *===========================================================================*/
typedef enum _OpcUa_DiagnosticInfo_EncodingByte
{
    OpcUa_DiagnosticInfo_EncodingByte_SymbolicId = 0x01,
    OpcUa_DiagnosticInfo_EncodingByte_NamespaceUri = 0x02,
    OpcUa_DiagnosticInfo_EncodingByte_LocalizedText = 0x04,
    OpcUa_DiagnosticInfo_EncodingByte_Locale = 0x08,
    OpcUa_DiagnosticInfo_EncodingByte_AdditionalInfo = 0x10,
    OpcUa_DiagnosticInfo_EncodingByte_InnerStatusCode = 0x20,
    OpcUa_DiagnosticInfo_EncodingByte_InnerDiagnosticInfo = 0x40
}
OpcUa_DiagnosticInfo_EncodingByte;

/*============================================================================
 * OpcUa_LocalizedText_EncodingByte
 *===========================================================================*/
typedef enum _OpcUa_LocalizedText_EncodingByte
{
    OpcUa_LocalizedText_EncodingByte_Locale = 0x01,
    OpcUa_LocalizedText_EncodingByte_Text = 0x02
}
OpcUa_LocalizedText_EncodingByte;

/*============================================================================
 * OpcUa_DataValue_EncodingByte
 *===========================================================================*/
typedef enum _OpcUa_DataValue_EncodingByte
{
    OpcUa_DataValue_EncodingByte_Value = 0x01,
    OpcUa_DataValue_EncodingByte_StatusCode = 0x02,
    OpcUa_DataValue_EncodingByte_SourceTimestamp = 0x04,
    OpcUa_DataValue_EncodingByte_ServerTimestamp = 0x08,
    OpcUa_DataValue_EncodingByte_SourcePicoseconds = 0x10,
    OpcUa_DataValue_EncodingByte_ServerPicoseconds = 0x20
}
OpcUa_DataValue_EncodingByte;

/*============================================================================
 * Variant Encoding Constants
 *===========================================================================*/
#define OpcUa_Variant_ArrayMask 0x80
#define OpcUa_Variant_ArrayDimensionsMask 0x40
#define OpcUa_Variant_TypeMask 0x3F

OPCUA_END_EXTERN_C

#endif /* _OpcUa_BinaryEncoderInternal_H_ */
