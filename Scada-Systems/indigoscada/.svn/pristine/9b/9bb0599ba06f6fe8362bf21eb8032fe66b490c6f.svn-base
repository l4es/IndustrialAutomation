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
 
/* serverstub (basic includes for implementing a server based on the stack) */
#include <opcua_serverstub.h>

/* extensions from the stack (get additional functionality) */
#include <opcua_p_types.h>
#include <opcua_string.h>
#include <opcua_memory.h>
#include <opcua_core.h>

#include "addressspace.h"
#include "general_header.h"

OpcUa_StatusCode initialize_value_attribute_of_variablenodes_variabletypenodes(OpcUa_Void)
{
	extern OpcUa_Int			Continuation_Point_Identifier;
	extern OpcUa_Int			Cont_Point_Counter;
	extern OpcUa_UInt32			session_flag;
	extern OpcUa_String*		p_user_name;
	extern OpcUa_Double			session_timeout;
	extern my_Variant			all_ValueAttribute_of_VariableTypeNodes_VariableNodes[];
	OpcUa_InitializeStatus(OpcUa_Module_Server, "initialize_value_attribute_of_variablenodes_variabletypenodes");

/*initializing session relevant variables*/
	Continuation_Point_Identifier=0;
	Cont_Point_Counter=0;
	session_flag=SESSION_NOT_ACTIVATED;
	p_user_name=OpcUa_Null;
	session_timeout=REVISED_SESSIONTIMEOUT;
/********************************/


//Variable: State	
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[0].ArrayType						=OpcUa_VariantArrayType_Scalar;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[0].Datatype						=OpcUaId_UInt32;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[0].Value.UInt32					=0;
//--------------------------
	
//Variable: EnumStrings
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].ArrayType						=OpcUa_VariantArrayType_Array;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Datatype						=OpcUaId_String;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Length				=8;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray	=OpcUa_Memory_Alloc(8*sizeof(OpcUa_StringA));
	OpcUa_ReturnErrorIfAllocFailed((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray))
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+0)="RUNNING_0";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+1)="FAILED_1";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+2)="NO_CONFIGURATION_2";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+3)="SUSPENDED_3";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+4)="SHUPDOWN_4";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+5)="TEST_5";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+6)="COMMUNICATIONFAULT_6";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[1].Value.Array.Value.StringArray+7)="UNKNOWN_7";
//--------------------------

//Variable: ServerArray
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[2].ArrayType						=OpcUa_VariantArrayType_Array;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[2].Datatype						=OpcUaId_String;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[2].Value.Array.Length				=1;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[2].Value.Array.Value.StringArray	=OpcUa_Memory_Alloc(sizeof(OpcUa_StringA));
	OpcUa_ReturnErrorIfAllocFailed((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[2].Value.Array.Value.StringArray))
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[2].Value.Array.Value.StringArray+0)="Nano_Server";
//--------------------------

//Variable: NamespaceArray
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].ArrayType						=OpcUa_VariantArrayType_Array;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].Datatype						=OpcUaId_String;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].Value.Array.Length				=2;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].Value.Array.Value.StringArray	=OpcUa_Memory_Alloc(2*sizeof(OpcUa_StringA));
	OpcUa_ReturnErrorIfAllocFailed((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].Value.Array.Value.StringArray))
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].Value.Array.Value.StringArray+0)="http://opcfoundation.org/UA/";
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[3].Value.Array.Value.StringArray+1)="http://nanonamespace";
//--------------------------

//Variable: ServerProfileArray
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[4].ArrayType						=OpcUa_VariantArrayType_Array;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[4].Datatype						=OpcUaId_String;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[4].Value.Array.Length				=1;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[4].Value.Array.Value.StringArray	=OpcUa_Memory_Alloc(sizeof(OpcUa_StringA));
	OpcUa_ReturnErrorIfAllocFailed((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[4].Value.Array.Value.StringArray))
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[4].Value.Array.Value.StringArray+0)="http://opcfoundation.org/UA-Profile/Server/NanoEmbeddedDevice";
//--------------------------

//Variable: LocaleIdArray
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[5].ArrayType						=OpcUa_VariantArrayType_Array;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[5].Datatype						=OpcUaId_String;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[5].Value.Array.Length				=1;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[5].Value.Array.Value.StringArray	=OpcUa_Memory_Alloc(sizeof(OpcUa_StringA));
	OpcUa_ReturnErrorIfAllocFailed((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[5].Value.Array.Value.StringArray))
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[5].Value.Array.Value.StringArray+0)="en";
//--------------------------

//Variable: MaxBrowseContinuationPoints
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[6].ArrayType						=OpcUa_VariantArrayType_Scalar;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[6].Datatype						=OpcUaId_Double;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[6].Value.UInt32					=0;
//--------------------------

//Variable: MaxBrowseContinuationPoints
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[7].ArrayType						=OpcUa_VariantArrayType_Scalar;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[7].Datatype						=OpcUaId_UInt32 ;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[7].Value.UInt32					=1;
//--------------------------

//Variable: DATA_VALUE
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].ArrayType						=OpcUa_VariantArrayType_Array;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray	=OpcUa_Memory_Alloc(3*sizeof(OpcUa_Double));
	OpcUa_ReturnErrorIfAllocFailed((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray))
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Datatype						=OpcUaId_Double;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Length				=3;
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray+0)=3.14;
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray+1)=0.534124312;
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray+2)=1234.5346435;
//--------------------------

//Variable: EnabledFlag and Auditing
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[9].ArrayType						=OpcUa_VariantArrayType_Scalar;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[9].Datatype						=OpcUaId_Boolean;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[9].Value.Boolean					=OpcUa_False;
//---------------------------

//Variable: RedundancySupport 
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[10].ArrayType						=OpcUa_VariantArrayType_Scalar;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[10].Datatype						=OpcUaId_UInt32;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[10].Value.UInt32					=0;
//-------------------------

//Variable: CurrentTime
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[11].ArrayType						=OpcUa_VariantArrayType_Scalar;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[11].Datatype						=OpcUaId_DateTime;
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[11].Value.DateTime				=OpcUa_DateTime_UtcNow();
//-------------------------
	
	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;

	OpcUa_FinishErrorHandling;
}
