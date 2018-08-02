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

#include <memory.h>
#include "opcua_platformdefs.h"
#include "opcua_statuscodes.h"
#include "opcua_errorhandling.h"
#include "opcua_trace.h"

/*============================================================================
 * OpcUa_Boolean_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Boolean_P_NativeToWire(OpcUa_Boolean_Wire* wire, OpcUa_Boolean* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    *wire = *native;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Boolean_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Boolean_P_WireToNative(OpcUa_Boolean_Wire* native, OpcUa_Boolean_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    *native = *wire;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_SByte_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SByte_P_NativeToWire(OpcUa_SByte_Wire* wire, OpcUa_SByte* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    *wire = *native;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_SByte_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_SByte_P_WireToNative(OpcUa_SByte* native, OpcUa_SByte_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    *native = *wire;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Byte_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Byte_P_NativeToWire(OpcUa_Byte_Wire* wire, OpcUa_Byte* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    *wire = *native;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Byte_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Byte_P_WireToNative(OpcUa_Byte* native, OpcUa_Byte_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    *native = *wire;

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Int16_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int16_P_NativeToWire(OpcUa_Int16_Wire* wire, OpcUa_Int16* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_Int16));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Int16_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int16_P_WireToNative(OpcUa_Int16* native, OpcUa_Int16_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_Int16));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_UInt16_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt16_P_NativeToWire(OpcUa_UInt16_Wire* wire, OpcUa_UInt16* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_UInt16));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_UInt16_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt16_P_WireToNative(OpcUa_UInt16* native, OpcUa_UInt16_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_UInt16));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Int32_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int32_P_NativeToWire(OpcUa_Int32_Wire* wire, OpcUa_Int32* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_Int32));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Int32_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int32_P_WireToNative(OpcUa_Int32* native, OpcUa_Int32_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_Int32));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_UInt32_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt32_P_NativeToWire(OpcUa_UInt32_Wire* wire, OpcUa_UInt32* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_UInt32));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_UInt32_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt32_P_WireToNative(OpcUa_UInt32* native, OpcUa_UInt32_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_UInt32));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Int64_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int64_P_NativeToWire(OpcUa_Int64_Wire* wire, OpcUa_Int64* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_Int64));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Int64_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Int64_P_WireToNative(OpcUa_Int64* native, OpcUa_Int64_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_Int64));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_UInt64_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt64_P_NativeToWire(OpcUa_UInt64_Wire* wire, OpcUa_UInt64* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_UInt64));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_UInt64_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_UInt64_P_WireToNative(OpcUa_UInt64* native, OpcUa_UInt64_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_UInt64));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Float_P_NativeToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Float_P_NativeToWire(OpcUa_Float_Wire* wire, OpcUa_Float* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_Float));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Float_P_WireToNative
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Float_P_WireToNative(OpcUa_Float* native, OpcUa_Float_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_Float));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_Double_P_NativeToWire
 *===========================================================================*/

OpcUa_StatusCode OpcUa_Double_P_NativeToWire(OpcUa_Double_Wire* wire, OpcUa_Double* native)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(wire, native, sizeof(OpcUa_Double));

    return OpcUa_Good;
}

/*============================================================================
 * OpcUa_String_P_WireToWire
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Double_P_WireToNative(OpcUa_Double* native, OpcUa_Double_Wire* wire)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_BinarySerializer);
    OpcUa_ReturnErrorIfArgumentNull(wire);
    OpcUa_ReturnErrorIfArgumentNull(native);

    OpcUa_SwapBytes(native, wire, sizeof(OpcUa_Double));

    return OpcUa_Good;
}
