/* ========================================================================
 * Copyright (c) 2005-2016 The OPC Foundation, Inc. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/
 
#ifndef _my_addressspace
#define _my_addressspace

#define ARRAYSIZE_OF_VALUEATTRIBUTE					12

typedef struct{
	OpcUa_NodeId			ReferenceTypeId;
	OpcUa_Boolean			IsInverse;
	OpcUa_NodeId			Target_NodeId;							/*OpcUa_ExpandedNodeId	TargetId;*/
    OpcUa_StringA			Target_NamespaceUri;					/*OpcUa_ExpandedNodeId	TargetId;*/								
}_ReferenceNode_;


typedef struct{
	OpcUa_NodeId	    NodeId;
	OpcUa_NodeClass     NodeClass;									
	OpcUa_StringA		BrowseName;									/*OpcUa_QualifiedName  BrowseName;*/
	OpcUa_StringA		DisplayName;								/*OpcUa_LocalizedText  DisplayName;*/
	OpcUa_Int32         NoOfReferences;								
  /*OpcUa_UInt32        WriteMask;*/
  /*OpcUa_UInt32        UserWriteMask;*/
	_ReferenceNode_* References;									/* OpcUa_ReferenceNode* References;*/
}_BaseAttribute_;


typedef struct{
	_BaseAttribute_		 BaseAttribute;
	OpcUa_Boolean        IsAbstract;
}_ObjectTypeKnoten_;

typedef struct{
	_BaseAttribute_		 BaseAttribute;
	OpcUa_Byte           EventNotifier;
}_ObjectKnoten_;

typedef struct{
	_BaseAttribute_		BaseAttribute;
	OpcUa_Boolean       IsAbstract;
    OpcUa_Boolean       Symmetric;
    OpcUa_StringA		InverseName_text;							 /*OpcUa_LocalizedText  InverseName;*/
	OpcUa_StringA		InverseName_locale;							 /*OpcUa_LocalizedText  InverseName;*/
}_ReferenceTypeKnoten_;


typedef struct{
	_BaseAttribute_		 BaseAttribute;
	OpcUa_Boolean        IsAbstract;
}_DataTypeKnoten_;


typedef union 
{
    OpcUa_UInt32*            UInt32Array;
    OpcUa_Double*            DoubleArray;
    OpcUa_StringA*           StringArray;
	OpcUa_Boolean*           BooleanArray;
}
my_VariantArrayUnion;

typedef struct
{
    /* The total number of elements in all dimensions. */
    OpcUa_Int32  Length;

    /* The data stored in the array. */
    my_VariantArrayUnion Value;
}
my_VariantArrayValue;

/* A union that contains one of the built-in types,which are used in nano server addressspace. */
typedef union 
{
    OpcUa_UInt32             UInt32;
    OpcUa_Double             Double;
    OpcUa_StringA            String;
	OpcUa_Boolean            Boolean;
	OpcUa_DateTime			 DateTime;
    my_VariantArrayValue	 Array;
}
my_VariantUnion;


typedef struct 
{
    /* Indicates the datatype stored in the Variant. This is always one of the OpcUa_BuiltInType values. */
    /* This is the datatype of a single element if the Variant contains an array. */
    OpcUa_Byte          Datatype;

    /* A flag indicating that an array with one or more dimensions is stored in the Variant. */
    OpcUa_Byte          ArrayType;

    /* The value stored in the Variant. */
    my_VariantUnion		Value;
}
my_Variant;

typedef struct{
	_BaseAttribute_		 BaseAttribute;
	OpcUa_Int			 ValueIndex;
    OpcUa_NodeId		 DataType;									
    OpcUa_Int32          ValueRank;
    OpcUa_Int32          NoOfArrayDimensions;
    OpcUa_UInt32         ArrayDimensions;
    OpcUa_Boolean        IsAbstract;
}_VariableTypeKnoten_;

typedef struct{
	_BaseAttribute_		 BaseAttribute;
	OpcUa_Int			 ValueIndex;
    OpcUa_NodeId		 DataType;									
    OpcUa_Int32          ValueRank;
    OpcUa_Int32          NoOfArrayDimensions;
    OpcUa_UInt32         ArrayDimensions;
    OpcUa_Byte           AccessLevel;
    OpcUa_Byte           UserAccessLevel;
   // OpcUa_Double         MinimumSamplingInterval;
    OpcUa_Boolean        Historizing;
}_VariableKnoten_;




#endif/*_my_addressspace*/