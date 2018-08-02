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
#include <opcua_builtintypes.h>
#include <opcua_statuscodes.h>
#include <opcua_encodeableobject.h>

/*============================================================================
 * OpcUa_VariantUnion_Clear
 *===========================================================================*/
static OpcUa_Void OpcUa_VariantUnion_Clear(OpcUa_UInt16 uDatatype, OpcUa_VariantUnion* a_pValue);

/*============================================================================
 * OpcUa_VariantArrayValue_Clear
 *===========================================================================*/
static OpcUa_Void OpcUa_VariantArrayValue_Clear(OpcUa_UInt16 uDatatype, OpcUa_Int32 iLength, OpcUa_VariantArrayUnion* pValue);

/*============================================================================
 * OpcUa_VariantMatrixValue_GetElementCount
 *===========================================================================*/
OpcUa_Int32 OpcUa_VariantMatrix_GetElementCount(OpcUa_VariantMatrixValue* a_pValue)
{
    OpcUa_Int32 ii = 0;
    OpcUa_Int32 iLength = 1;

    if (a_pValue == OpcUa_Null)
    {
        return 0;
    }

    if (a_pValue->NoOfDimensions == 0 || a_pValue->Dimensions == OpcUa_Null)
    {
        return 0;
    }

    for (ii = 0; ii < a_pValue->NoOfDimensions; ii++)
    {
        iLength *= a_pValue->Dimensions[ii];
    }

    return iLength;
}

/*============================================================================
 * OpcUa_ByteString_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_ByteString_Initialize(OpcUa_ByteString* a_pValue)
{
    if(a_pValue == OpcUa_Null)
    {
        return;
    }

    a_pValue->Data = OpcUa_Null;
    a_pValue->Length = -1;
}

/*============================================================================
 * OpcUa_ByteString_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_ByteString_Clear(OpcUa_ByteString* a_pValue)
{
    if(a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_Free(a_pValue->Data);
    a_pValue->Data = OpcUa_Null;
    a_pValue->Length = -1;
}

/*============================================================================
 * OpcUa_NodeId_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_NodeId_Initialize(OpcUa_NodeId* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    a_pValue->IdentifierType = OpcUa_IdentifierType_Numeric;
    a_pValue->Identifier.Numeric = 0;
    a_pValue->NamespaceIndex = 0;
}

/*============================================================================
 * OpcUa_NodeId_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_NodeId_Clear(OpcUa_NodeId* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    switch (a_pValue->IdentifierType)
    {
        default:
        case OpcUa_IdentifierType_Numeric:
        {
            break;
        }

        case OpcUa_IdentifierType_String:
        {
            OpcUa_String_Clear(&(a_pValue->Identifier.String));
            break;
        }

        case OpcUa_IdentifierType_Guid:
        {
            OpcUa_Free(a_pValue->Identifier.Guid);
            break;
        }

        case OpcUa_IdentifierType_Opaque:
        {
            OpcUa_ByteString_Clear(&a_pValue->Identifier.ByteString);
            break;
        }
    }

    a_pValue->IdentifierType = OpcUa_IdentifierType_Numeric;
    a_pValue->Identifier.Numeric = 0;
    a_pValue->NamespaceIndex = 0;
}

/*============================================================================
 * OpcUa_NodeId_IsNull
 *===========================================================================*/
OpcUa_Boolean OpcUa_NodeId_IsNull(OpcUa_NodeId* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return OpcUa_True;
    }

    if (a_pValue->NamespaceIndex != 0)
    {
        return OpcUa_False;
    }

    switch (a_pValue->IdentifierType)
    {
        case OpcUa_IdentifierType_Numeric:
        {
            if (a_pValue->Identifier.Numeric != 0)
            {
                return OpcUa_False;
            }

            break;
        }

        case OpcUa_IdentifierType_String:
        {
            if (!OpcUa_String_IsNull(&(a_pValue->Identifier.String)) && OpcUa_StrLen(&(a_pValue->Identifier.String)) > 0)
            {
                return OpcUa_False;
            }

            break;
        }

        case OpcUa_IdentifierType_Guid:
        {
            return OpcUa_Guid_IsNull(a_pValue->Identifier.Guid);
        }

        case OpcUa_IdentifierType_Opaque:
        {
            if (a_pValue->Identifier.ByteString.Length > 0)
            {
                return OpcUa_False;
            }

            break;
        }
    }

    return OpcUa_True;
}

/*============================================================================
 * OpcUa_ExpandedNodeId_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_ExpandedNodeId_Initialize(OpcUa_ExpandedNodeId* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_ExpandedNodeId));
    OpcUa_NodeId_Initialize(&a_pValue->NodeId);
}

/*============================================================================
 * OpcUa_ExpandedNodeId_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_ExpandedNodeId_Clear(OpcUa_ExpandedNodeId* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_NodeId_Clear(&a_pValue->NodeId);
    OpcUa_String_Clear(&(a_pValue->NamespaceUri));
    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_ExpandedNodeId));
}

/*============================================================================
 * OpcUa_ExpandedNodeId_IsNull
 *===========================================================================*/
OpcUa_Boolean OpcUa_ExpandedNodeId_IsNull(OpcUa_ExpandedNodeId* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return OpcUa_True;
    }

    if((!OpcUa_String_IsNull(&(a_pValue->NamespaceUri))) && (OpcUa_StrLen(&(a_pValue->NamespaceUri)) > 0))
    {
        return OpcUa_False;
    }

    return OpcUa_NodeId_IsNull(&a_pValue->NodeId);
}

/*============================================================================
 * OpcUa_DiagnosticInfo_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_DiagnosticInfo_Initialize(OpcUa_DiagnosticInfo* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_DiagnosticInfo));

    a_pValue->SymbolicId    = -1;
    a_pValue->NamespaceUri  = -1;
    a_pValue->Locale        = -1;
    a_pValue->LocalizedText = -1;
}

/*============================================================================
 * OpcUa_DiagnosticInfo_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_DiagnosticInfo_Clear(OpcUa_DiagnosticInfo* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_String_Clear(&(a_pValue->AdditionalInfo));

    if(a_pValue->InnerDiagnosticInfo != OpcUa_Null)
    {
        OpcUa_DiagnosticInfo_Clear(a_pValue->InnerDiagnosticInfo);
        OpcUa_Free(a_pValue->InnerDiagnosticInfo);
    }

    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_DiagnosticInfo));
}

/*============================================================================
 * OpcUa_LocalizedText_Initialize
 *===========================================================================*/
void OpcUa_LocalizedText_Initialize(OpcUa_LocalizedText* a_pValue)
{
    if (a_pValue != OpcUa_Null)
    {
        OpcUa_Field_Initialize(String, Locale);
        OpcUa_Field_Initialize(String, Text);
    }
}

/*============================================================================
 * OpcUa_LocalizedText_Clear
 *===========================================================================*/
void OpcUa_LocalizedText_Clear(OpcUa_LocalizedText* a_pValue)
{
    if (a_pValue != OpcUa_Null)
    {
        OpcUa_Field_Clear(String, Locale);
        OpcUa_Field_Clear(String, Text);
    }
}

/*============================================================================
 * OpcUa_QualifiedName_Initialize
 *===========================================================================*/
void OpcUa_QualifiedName_Initialize(OpcUa_QualifiedName* a_pValue)
{
    if (a_pValue != OpcUa_Null)
    {
        OpcUa_Field_Initialize(Int32, NamespaceIndex);
        OpcUa_Field_Initialize(String, Name);
    }
}

/*============================================================================
 * OpcUa_QualifiedName_Clear
 *===========================================================================*/
void OpcUa_QualifiedName_Clear(OpcUa_QualifiedName* a_pValue)
{
    if (a_pValue != OpcUa_Null)
    {
        OpcUa_Field_Clear(Int32, NamespaceIndex);
        OpcUa_Field_Clear(String, Name);
    }
}

/*============================================================================
 * OpcUa_DataValue_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_DataValue_Initialize(OpcUa_DataValue* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_DataValue));
}

/*============================================================================
 * OpcUa_DataValue_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_DataValue_Clear(OpcUa_DataValue* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_Variant_Clear(&a_pValue->Value);
    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_DataValue));
}

/*============================================================================
 * OpcUa_ExtensionObject_Create
 *===========================================================================*/
OpcUa_Void OpcUa_ExtensionObject_Create(OpcUa_ExtensionObject** a_ppValue)
{
    if (a_ppValue != OpcUa_Null)
    {
        *a_ppValue = (OpcUa_ExtensionObject*)OpcUa_Alloc(sizeof(OpcUa_ExtensionObject));
        if(*a_ppValue != OpcUa_Null)
        {
            OpcUa_ExtensionObject_Initialize(*a_ppValue);
        }
    }
}

/*============================================================================
 * OpcUa_ExtensionObject_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_ExtensionObject_Initialize(OpcUa_ExtensionObject* a_pValue)
{
    if (a_pValue != OpcUa_Null)
    {
        OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_ExtensionObject));
    }
}

/*============================================================================
 * OpcUa_ExtensionObject_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_ExtensionObject_Clear(OpcUa_ExtensionObject* a_pValue)
{
    if (a_pValue != OpcUa_Null)
    {
        OpcUa_ExpandedNodeId_Clear(&a_pValue->TypeId);

        switch (a_pValue->Encoding)
        {
            case OpcUa_ExtensionObjectEncoding_Binary:
            {
                OpcUa_ByteString_Clear(&a_pValue->Body.Binary);
                break;
            }

            case OpcUa_ExtensionObjectEncoding_Xml:
            {
                OpcUa_XmlElement_Clear(&a_pValue->Body.Xml);
                break;
            }

            case OpcUa_ExtensionObjectEncoding_EncodeableObject:
            {
                OpcUa_EncodeableObject_Delete(a_pValue->Body.EncodeableObject.Type, &a_pValue->Body.EncodeableObject.Object);
                break;
            }
            default:
            {
                break;
            }
        }

        OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_ExtensionObject));
    }
}

/*============================================================================
 * OpcUa_ExtensionObject_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_ExtensionObject_Delete(OpcUa_ExtensionObject** a_ppValue)
{
    if (a_ppValue != OpcUa_Null && *a_ppValue != OpcUa_Null)
    {
        OpcUa_ExtensionObject_Clear(*a_ppValue);
        OpcUa_Free(*a_ppValue);
        *a_ppValue = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_Variant_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_Variant_Initialize(OpcUa_Variant* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_Variant));
}

/*============================================================================
 * OpcUa_Variant_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_Variant_Clear(OpcUa_Variant* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    if (a_pValue->ArrayType == OpcUa_VariantArrayType_Scalar)
    {
        OpcUa_VariantUnion_Clear(a_pValue->Datatype, &a_pValue->Value);
    }
    else if (a_pValue->ArrayType == OpcUa_VariantArrayType_Array)
    {
        OpcUa_VariantArrayValue_Clear(
            a_pValue->Datatype,
            a_pValue->Value.Array.Length,
            &a_pValue->Value.Array.Value);
    }
    else if (a_pValue->ArrayType == OpcUa_VariantArrayType_Matrix)
    {
        OpcUa_VariantArrayValue_Clear(
            a_pValue->Datatype,
            OpcUa_VariantMatrix_GetElementCount(&a_pValue->Value.Matrix),
            &a_pValue->Value.Matrix.Value);

        if (a_pValue->Value.Matrix.Dimensions != OpcUa_Null)
        {
            OpcUa_Free(a_pValue->Value.Matrix.Dimensions);
        }
    }
    else
    {
        OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_Variant));
        return;
    }

    a_pValue->ArrayType = 0;
    a_pValue->Datatype = 0;
}

/*============================================================================
 * OpcUa_VariantArrayValue_Clear
 *===========================================================================*/
static OpcUa_Void OpcUa_VariantArrayValue_Clear(
    OpcUa_UInt16             a_uDatatype,
    OpcUa_Int32              a_iLength,
    OpcUa_VariantArrayUnion* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    /* clear each element in the array */
    switch (a_uDatatype)
    {
        case OpcUaType_Null:
        case OpcUaType_Boolean:
        case OpcUaType_SByte:
        case OpcUaType_Byte:
        case OpcUaType_Int16:
        case OpcUaType_UInt16:
        case OpcUaType_Int32:
        case OpcUaType_UInt32:
        case OpcUaType_Int64:
        case OpcUaType_UInt64:
        case OpcUaType_Float:
        case OpcUaType_Double:
        case OpcUaType_DateTime:
        case OpcUaType_Guid:
        case OpcUaType_StatusCode:
        default:
        {
            break;
        }

        case OpcUaType_String:
        {
            OpcUa_ClearArray(a_pValue->StringArray, a_iLength, String);
            break;
        }

        case OpcUaType_ByteString:
        {
            OpcUa_ClearArray(a_pValue->ByteStringArray, a_iLength, ByteString);
            break;
        }

        case OpcUaType_XmlElement:
        {
            OpcUa_ClearArray(a_pValue->XmlElementArray, a_iLength, XmlElement);
            break;
        }

        case OpcUaType_NodeId:
        {
            OpcUa_ClearArray(a_pValue->NodeIdArray, a_iLength, NodeId);
            break;
        }

        case OpcUaType_ExpandedNodeId:
        {
            OpcUa_ClearArray(a_pValue->ExpandedNodeIdArray, a_iLength, ExpandedNodeId);
            break;
        }

        case OpcUaType_QualifiedName:
        {
            OpcUa_ClearArray(a_pValue->QualifiedNameArray, a_iLength, QualifiedName);
            break;
        }

        case OpcUaType_LocalizedText:
        {
            OpcUa_ClearArray(a_pValue->LocalizedTextArray, a_iLength, LocalizedText);
            break;
        }

        case OpcUaType_ExtensionObject:
        {
            OpcUa_ClearArray(a_pValue->ExtensionObjectArray, a_iLength, ExtensionObject);
            break;
        }

        case OpcUaType_DataValue:
        {
            OpcUa_DataValue_ClearArray(a_pValue->DataValueArray, a_iLength);
            break;
        }

        case OpcUaType_Variant:
        {
            OpcUa_Variant_ClearArray(a_pValue->VariantArray, a_iLength);
            break;
        }
    }

    /* free the memory allocated for the array. */
    OpcUa_Free(a_pValue->Array);
}

/*============================================================================
 * OpcUa_VariantUnion_Clear
 *===========================================================================*/
static OpcUa_Void OpcUa_VariantUnion_Clear(OpcUa_UInt16 datatype, OpcUa_VariantUnion* a_pValue)
{
    if (a_pValue == OpcUa_Null)
    {
        return;
    }

    switch (datatype)
    {
        case OpcUaType_Null:
        case OpcUaType_Boolean:
        case OpcUaType_SByte:
        case OpcUaType_Byte:
        case OpcUaType_Int16:
        case OpcUaType_UInt16:
        case OpcUaType_Int32:
        case OpcUaType_UInt32:
        case OpcUaType_Int64:
        case OpcUaType_UInt64:
        case OpcUaType_Float:
        case OpcUaType_Double:
        case OpcUaType_DateTime:
        case OpcUaType_StatusCode:
        default:
        {
            break;
        }

        case OpcUaType_Guid:
        {
            OpcUa_Free(a_pValue->Guid);
            break;
        }

        case OpcUaType_String:
        {
            OpcUa_String_Clear(&(a_pValue->String));
            break;
        }

        case OpcUaType_ByteString:
        {
            OpcUa_ByteString_Clear(&a_pValue->ByteString);
            break;
        }

        case OpcUaType_XmlElement:
        {
            OpcUa_XmlElement_Clear(&a_pValue->XmlElement);
            break;
        }

        case OpcUaType_NodeId:
        {
            OpcUa_NodeId_Clear(a_pValue->NodeId);
            OpcUa_Free(a_pValue->NodeId);
            break;
        }

        case OpcUaType_ExpandedNodeId:
        {
            OpcUa_ExpandedNodeId_Clear(a_pValue->ExpandedNodeId);
            OpcUa_Free(a_pValue->ExpandedNodeId);
            break;
        }

        case OpcUaType_QualifiedName:
        {
            OpcUa_QualifiedName_Clear(a_pValue->QualifiedName);
            OpcUa_Free(a_pValue->QualifiedName);
            break;
        }

        case OpcUaType_LocalizedText:
        {
            OpcUa_LocalizedText_Clear(a_pValue->LocalizedText);
            OpcUa_Free(a_pValue->LocalizedText);
            break;
        }

        case OpcUaType_DataValue:
        {
            OpcUa_DataValue_Clear(a_pValue->DataValue);
            OpcUa_Free(a_pValue->DataValue);
            break;
        }

        case OpcUaType_ExtensionObject:
        {
            OpcUa_ExtensionObject_Delete(&a_pValue->ExtensionObject);
            break;
        }
    }

    /* OpcUa_MemSet(a_pValue, 0, sizeof(OpcUa_VariantUnion)); */
}
