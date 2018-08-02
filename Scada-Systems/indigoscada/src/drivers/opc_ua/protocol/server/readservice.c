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
#include <opcua_string.h>
#include <opcua_memory.h>
#include <opcua_core.h>

#include "addressspace.h"
#include "browseservice.h"
#include "mytrace.h"
#include "readservice.h"
#include "general_header.h"




/*============================================================================
 * method which implements the Read service.
 *===========================================================================*/
OpcUa_StatusCode my_Read(
							OpcUa_Endpoint             a_hEndpoint,
							OpcUa_Handle               a_hContext,
							const OpcUa_RequestHeader* a_pRequestHeader,
							OpcUa_Double               a_nMaxAge,
							OpcUa_TimestampsToReturn   a_eTimestampsToReturn,
							OpcUa_Int32                a_nNoOfNodesToRead,
							const OpcUa_ReadValueId*   a_pNodesToRead,
							OpcUa_ResponseHeader*      a_pResponseHeader,
							OpcUa_Int32*               a_pNoOfResults,
							OpcUa_DataValue**          a_pResults,
							OpcUa_Int32*               a_pNoOfDiagnosticInfos,
							OpcUa_DiagnosticInfo**     a_pDiagnosticInfos)
{
	OpcUa_Int i,n;
	OpcUa_Void* p_Node;
	extern OpcUa_UInt32		securechannelId;
	extern OpcUa_UInt32		session_flag;
	extern OpcUa_Double		msec_counter;
	extern OpcUa_String*	p_user_name;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "OpcUa_ServerApi_Read");

    /* validate arguments. */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestHeader);
    OpcUa_ReferenceParameter(a_nMaxAge);
    OpcUa_ReferenceParameter(a_eTimestampsToReturn);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfNodesToRead, a_pNodesToRead);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfResults, a_pResults);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfDiagnosticInfos, a_pDiagnosticInfos);

	*a_pNoOfDiagnosticInfos=0;
	*a_pDiagnosticInfos=OpcUa_Null;

	RESET_SESSION_COUNTER

 #ifndef NO_DEBUGING_
	MY_TRACE("\n\n\nRREADSERVICE==============================================\n");
	if(p_user_name!=OpcUa_Null)
		MY_TRACE("\nUser:%s\n",OpcUa_String_GetRawString(p_user_name)); 
#endif /*_DEBUGING_*/
  

	if(OpcUa_IsBad(session_flag))
	{
		//teile client mit , dass Session geschlossen ist
#ifndef NO_DEBUGING_
		MY_TRACE("\nSession nicht aktiv\n"); 
#endif /*_DEBUGING_*/
		uStatus=OpcUa_BadSessionNotActivated;
		OpcUa_GotoError;
	}

	
	uStatus=check_authentication_token(a_pRequestHeader);
	if(OpcUa_IsBad(uStatus))
	{
#ifndef NO_DEBUGING_
		MY_TRACE("\nAuthentication Token invalid.\n"); 
#endif /*_DEBUGING_*/
		OpcUa_GotoError;
	}

	*a_pResults=OpcUa_Alloc(a_nNoOfNodesToRead*sizeof(OpcUa_DataValue));
	OpcUa_GotoErrorIfAllocFailed((*a_pResults))
	

	for(n=0;n<a_nNoOfNodesToRead;n++)
	{
		OpcUa_DataValue_Initialize((*a_pResults)+n);
		((*a_pResults)+n)->StatusCode=OpcUa_BadAttributeIdInvalid;
		p_Node=search_for_node((a_pNodesToRead+n)->NodeId);
		if(p_Node!= OpcUa_Null)   //pruefe ob Knoten existiert
		{
			if(((a_pNodesToRead+n)->AttributeId)<=7 && ((a_pNodesToRead+n)->AttributeId)>=1)
			{
				if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_NodeId)
				{
					((*a_pResults)+n)->Value.Value.NodeId=OpcUa_Memory_Alloc(sizeof(OpcUa_NodeId));
					if(((*a_pResults)+n)->Value.Value.NodeId !=OpcUa_Null)
					{
						OpcUa_NodeId_Initialize(((*a_pResults)+n)->Value.Value.NodeId);
						*(((*a_pResults)+n)->Value.Value.NodeId)=((_ObjectKnoten_*)p_Node)->BaseAttribute.NodeId; 
						fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_NodeId, OpcUa_VariantArrayType_Scalar,0);
						((*a_pResults)+n)->StatusCode=OpcUa_Good;
					}
					else
					{
						((*a_pResults)+n)->StatusCode=OpcUa_BadOutOfMemory;
					}
				}
				if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_NodeClass)
				{
					((*a_pResults)+n)->Value.Value.Int32=((_ObjectKnoten_*)p_Node)->BaseAttribute.NodeClass;
					fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Int32, OpcUa_VariantArrayType_Scalar,0);
					((*a_pResults)+n)->StatusCode=OpcUa_Good;
				}
				if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_BrowseName)
				{
					((*a_pResults)+n)->Value.Value.QualifiedName=OpcUa_Memory_Alloc(sizeof(OpcUa_QualifiedName));
					if(((*a_pResults)+n)->Value.Value.QualifiedName!=OpcUa_Null)
					{
						OpcUa_QualifiedName_Initialize(((*a_pResults)+n)->Value.Value.QualifiedName);
						OpcUa_String_AttachCopy(&((*a_pResults)+n)->Value.Value.QualifiedName->Name,((_ObjectKnoten_*)p_Node)->BaseAttribute.BrowseName);
						((*a_pResults)+n)->Value.Value.QualifiedName->NamespaceIndex=((_ObjectKnoten_*)p_Node)->BaseAttribute.NodeId.NamespaceIndex;     
						fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_QualifiedName, OpcUa_VariantArrayType_Scalar,0);
						((*a_pResults)+n)->StatusCode=OpcUa_Good;
					}
					else
					{
						((*a_pResults)+n)->StatusCode=OpcUa_BadOutOfMemory;
					}
				}
				if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_DisplayName)
				{
					((*a_pResults)+n)->Value.Value.LocalizedText=OpcUa_Memory_Alloc(sizeof(OpcUa_LocalizedText));
					if(((*a_pResults)+n)->Value.Value.LocalizedText!=OpcUa_Null)
					{
						OpcUa_LocalizedText_Initialize(((*a_pResults)+n)->Value.Value.LocalizedText);
						OpcUa_String_AttachCopy(&((*a_pResults)+n)->Value.Value.LocalizedText->Text,((_ObjectKnoten_*)p_Node)->BaseAttribute.DisplayName);
						OpcUa_String_AttachCopy(&((*a_pResults)+n)->Value.Value.LocalizedText->Locale,"en");
						fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_LocalizedText, OpcUa_VariantArrayType_Scalar,0);
						((*a_pResults)+n)->StatusCode=OpcUa_Good;
					}
					else
					{
						((*a_pResults)+n)->StatusCode=OpcUa_BadOutOfMemory;
					}

				}
				if(((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_Description || (a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_WriteMask) || (a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_UserWriteMask)
				{
						((*a_pResults)+n)->StatusCode=OpcUa_BadNotReadable;
				}
			}
			else
			{
				switch((((_ObjectKnoten_*)p_Node)->BaseAttribute.NodeClass))
				{
				case OpcUa_NodeClass_Variable:
					{
						if((a_pNodesToRead+n)->AttributeId<=20 && (a_pNodesToRead+n)->AttributeId>=13)
						{
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_Value)
							{
								 ((*a_pResults)+n)->StatusCode=fill_Variant_for_value_attribute((_VariableKnoten_*)p_Node, OpcUa_Null,((*a_pResults)+n));
								if(a_eTimestampsToReturn!=OpcUa_TimestampsToReturn_Neither)
								{
									uStatus=assigne_Timestamp(((*a_pResults)+n),a_eTimestampsToReturn);
									if(OpcUa_IsBad(uStatus))
									{
										((*a_pResults)+n)->StatusCode=OpcUa_BadInternalError;
										uStatus=OpcUa_Good;
									}
								}
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_DataType)
							{
								((*a_pResults)+n)->Value.Value.NodeId=OpcUa_Memory_Alloc(sizeof(OpcUa_NodeId));
								if(((*a_pResults)+n)->Value.Value.NodeId!=OpcUa_Null)
								{
									OpcUa_NodeId_Initialize(((*a_pResults)+n)->Value.Value.NodeId);
									*(((*a_pResults+n)->Value.Value.NodeId))=((_VariableKnoten_*)p_Node)->DataType;
									fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_NodeId, OpcUa_VariantArrayType_Scalar,0);
									((*a_pResults)+n)->StatusCode=OpcUa_Good;
								}
								else
								{
									((*a_pResults)+n)->StatusCode=OpcUa_BadOutOfMemory;
								}
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_ValueRank)
							{
								((*a_pResults)+n)->Value.Value.Int32=((_VariableKnoten_*)p_Node)->ValueRank;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Int32, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_ArrayDimensions)
							{
								
								((*a_pResults)+n)->Value.Value.UInt32=(((_VariableKnoten_*)p_Node)->ArrayDimensions);
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_UInt32, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;

							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_AccessLevel)
							{
								((*a_pResults)+n)->Value.Value.Byte=((_VariableKnoten_*)p_Node)->AccessLevel;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Byte, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_UserAccessLevel)
							{
								((*a_pResults)+n)->Value.Value.Byte=((_VariableKnoten_*)p_Node)->UserAccessLevel;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Byte, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_MinimumSamplingInterval)
							{
								((*a_pResults)+n)->Value.Value.Double=OpcUa_MinimumSamplingIntervals_Indeterminate;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Double, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_Historizing)
							{
								((*a_pResults)+n)->Value.Value.Boolean=((_VariableKnoten_*)p_Node)->Historizing;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
						}
						break;
					}
				case OpcUa_NodeClass_VariableType:
					{
						if(((a_pNodesToRead+n)->AttributeId<=16 && (a_pNodesToRead+n)->AttributeId>=13) ||((a_pNodesToRead+n)->AttributeId==8))
						{
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_Value)
							{
								 ((*a_pResults)+n)->StatusCode=fill_Variant_for_value_attribute((_VariableKnoten_*)p_Node, OpcUa_Null,((*a_pResults)+n));
								if(a_eTimestampsToReturn!=OpcUa_TimestampsToReturn_Neither)
								{
									uStatus=assigne_Timestamp(((*a_pResults)+n),a_eTimestampsToReturn);
									if(OpcUa_IsBad(uStatus))
									{
										((*a_pResults)+n)->StatusCode=OpcUa_BadInternalError;
										uStatus=OpcUa_Good;
									}
								}
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_DataType)
							{
								((*a_pResults)+n)->Value.Value.NodeId=OpcUa_Memory_Alloc(sizeof(OpcUa_NodeId));
								if(((*a_pResults)+n)->Value.Value.NodeId!=OpcUa_Null)
								{
									OpcUa_NodeId_Initialize(((*a_pResults)+n)->Value.Value.NodeId);
									*((*a_pResults)+n)->Value.Value.NodeId=((_VariableTypeKnoten_*)p_Node)->DataType;
									fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_NodeId, OpcUa_VariantArrayType_Scalar,0);
									((*a_pResults)+n)->StatusCode=OpcUa_Good;
								}
								else
								{
									((*a_pResults)+n)->StatusCode=OpcUa_BadOutOfMemory;
								}
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_ArrayDimensions)
							{
								((*a_pResults)+n)->Value.Value.UInt32=((_VariableTypeKnoten_*)p_Node)->ArrayDimensions; 
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_UInt32, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_IsAbstract)
							{
								((*a_pResults)+n)->Value.Value.Boolean=((_VariableTypeKnoten_*)p_Node)->IsAbstract;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							
						}
						break;
					}
				case OpcUa_NodeClass_Object:
					{
						if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_EventNotifier)
						{
							((*a_pResults)+n)->Value.Value.Byte=((_ObjectKnoten_*)p_Node)->EventNotifier;
							fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Byte, OpcUa_VariantArrayType_Scalar,0);
							((*a_pResults)+n)->StatusCode=OpcUa_Good;
						}
						break;
					}
					
				case OpcUa_NodeClass_ObjectType:
					{
						if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_IsAbstract)
						{
							((*a_pResults)+n)->Value.Value.Boolean=((_ObjectTypeKnoten_*)p_Node)->IsAbstract;
							fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
							((*a_pResults)+n)->StatusCode=OpcUa_Good;
						}
						break;
					}
					
				case OpcUa_NodeClass_DataType:
					{

						if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_IsAbstract)
						{
							((*a_pResults)+n)->Value.Value.Boolean=((_DataTypeKnoten_*)p_Node)->IsAbstract;
							fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
							((*a_pResults)+n)->StatusCode=OpcUa_Good;
						}
						break;
					}
				
				case OpcUa_NodeClass_ReferenceType:
					{
						if((a_pNodesToRead+n)->AttributeId<=10 && (a_pNodesToRead+n)->AttributeId>=8)
						{
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_IsAbstract)
							{
								((*a_pResults)+n)->Value.Value.Boolean=((_ReferenceTypeKnoten_*)p_Node)->IsAbstract;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_Symmetric)
							{
								((*a_pResults)+n)->Value.Value.Boolean=((_ReferenceTypeKnoten_*)p_Node)->Symmetric;
								fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
								((*a_pResults)+n)->StatusCode=OpcUa_Good;
							}
							if((a_pNodesToRead+n)->AttributeId==OpcUa_Attributes_InverseName)
							{
								((*a_pResults)+n)->Value.Value.LocalizedText=OpcUa_Memory_Alloc(sizeof(OpcUa_LocalizedText));
								if(((*a_pResults)+n)->Value.Value.NodeId!=OpcUa_Null)
								{
									OpcUa_LocalizedText_Initialize(((*a_pResults)+n)->Value.Value.LocalizedText);
									OpcUa_String_AttachCopy(&((*a_pResults)+n)->Value.Value.LocalizedText->Text, ((_ReferenceTypeKnoten_*)p_Node)->InverseName_text);
									OpcUa_String_AttachCopy(&((*a_pResults)+n)->Value.Value.LocalizedText->Locale, ((_ReferenceTypeKnoten_*)p_Node)->InverseName_locale);
									fill_datatype_arraytype_in_my_Variant(((*a_pResults)+n),OpcUaId_LocalizedText, OpcUa_VariantArrayType_Scalar,0);
									((*a_pResults)+n)->StatusCode=OpcUa_Good;
								}
								else
								{
									((*a_pResults)+n)->StatusCode=OpcUa_BadOutOfMemory;
								}
							}
						}
						break;
					}
				default:
					break;
					
				
				}
			}
			
		
		}
		else
		{
			((*a_pResults)+n)->StatusCode=OpcUa_BadNodeIdUnknown;
		}
		
	}
	
	

	*a_pNoOfResults=a_nNoOfNodesToRead;

#ifndef NO_DEBUGING_
	MY_TRACE("\nanzahl der nodes :%d\n",a_nNoOfNodesToRead);
	for(i=0;i<a_nNoOfNodesToRead;i++)
	{
		MY_TRACE("\n|%d|, |%d| attributeId:%u\n",(a_pNodesToRead+i)->NodeId.NamespaceIndex,(a_pNodesToRead+i)->NodeId.Identifier.Numeric,(a_pNodesToRead+i)->AttributeId);
		
	}
#endif /*_DEBUGING_*/


	
	uStatus = response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICE===ENDE============================================\n\n\n"); 
#endif /*_DEBUGING_*/

	RESET_SESSION_COUNTER

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    
	uStatus = response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICEENDE (IM SERVICE SIND FEHLER AUFGETRETTEN)===========\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER
    OpcUa_FinishErrorHandling;
}



OpcUa_StatusCode  fill_Variant_for_value_attribute(_VariableKnoten_*  p_Node, OpcUa_String* p_Index, OpcUa_DataValue* p_Results)
{
	OpcUa_Int					i;
	extern my_Variant			all_ValueAttribute_of_VariableTypeNodes_VariableNodes[];
	OpcUa_InitializeStatus(OpcUa_Module_Server, "fill_Variant_for_value_attribute");

	OpcUa_ReturnErrorIfArgumentNull(p_Node);
	/*OpcUa_ReturnErrorIfArgumentNull(p_Index);*/
	OpcUa_ReturnErrorIfArgumentNull(p_Results);
	if(p_Node->ValueIndex == (-1))
	{
		OpcUa_GotoErrorWithStatus(OpcUa_BadNotReadable)
	}
	
	if(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].ArrayType==OpcUa_VariantArrayType_Scalar)
	{
		switch(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Datatype)
		{
		case OpcUaId_Double:
			{
				p_Results->Value.Value.Double=all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Double;
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_Double, OpcUa_VariantArrayType_Scalar,0);
				break;
			}
		case OpcUaId_DateTime:
			{
				p_Results->Value.Value.DateTime=all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.DateTime;
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_DateTime, OpcUa_VariantArrayType_Scalar,0);
				break;
			}
		case OpcUaId_String:
			{
				uStatus= OpcUa_String_AttachCopy(&(p_Results->Value.Value.String),all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.String);
				OpcUa_GotoErrorIfBad(uStatus)
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_String, OpcUa_VariantArrayType_Scalar,0);
				break;
			}
		case OpcUaId_UInt32:
			{
				p_Results->Value.Value.UInt32=all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.UInt32;
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_UInt32, OpcUa_VariantArrayType_Scalar,0);
				break;
			}
		case OpcUaId_Boolean:
			{
				p_Results->Value.Value.Boolean=all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Boolean;
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_Boolean, OpcUa_VariantArrayType_Scalar,0);
				break;
			}
		}
	
	}

	if(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].ArrayType==OpcUa_VariantArrayType_Array)
	{
		switch(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Datatype)
		{
		case OpcUaId_Double:
			{
				p_Results->Value.Value.Array.Value.DoubleArray=OpcUa_Memory_Alloc((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_Double));
				OpcUa_GotoErrorIfAllocFailed((p_Results->Value.Value.Array.Value.DoubleArray))
				
				OpcUa_MemCpy((p_Results->Value.Value.Array.Value.DoubleArray),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_Double),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Value.DoubleArray),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_Double));
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_Double, OpcUa_VariantArrayType_Array,all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length);
				break;
			}
		case OpcUaId_String:
			{
				p_Results->Value.Value.Array.Value.StringArray=OpcUa_Memory_Alloc((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_String));
				OpcUa_GotoErrorIfAllocFailed((p_Results->Value.Value.Array.Value.DoubleArray))

				for(i=0;i<(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length);i++)
				{
					uStatus= OpcUa_String_AttachCopy((p_Results->Value.Value.Array.Value.StringArray)+i,*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Value.StringArray+i));
					if(OpcUa_IsBad(uStatus))
						OpcUa_GotoError
				}
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_String, OpcUa_VariantArrayType_Array,all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length);
				break;
			}
		case OpcUaId_UInt32:
			{
				p_Results->Value.Value.Array.Value.UInt32Array=OpcUa_Memory_Alloc((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_UInt32));
				OpcUa_GotoErrorIfAllocFailed((p_Results->Value.Value.Array.Value.UInt32Array))

				OpcUa_MemCpy((p_Results->Value.Value.Array.Value.UInt32Array),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_UInt32),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Value.UInt32Array),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_UInt32));
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_UInt32, OpcUa_VariantArrayType_Array,all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length);
				break;
			}
		case OpcUaId_Boolean:
			{
				p_Results->Value.Value.Array.Value.BooleanArray=OpcUa_Memory_Alloc((all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_Boolean));
				OpcUa_GotoErrorIfAllocFailed((p_Results->Value.Value.Array.Value.BooleanArray))

				OpcUa_MemCpy((p_Results->Value.Value.Array.Value.BooleanArray),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_Boolean),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Value.BooleanArray),(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length)*sizeof(OpcUa_Boolean));
				fill_datatype_arraytype_in_my_Variant(p_Results,OpcUaId_UInt32, OpcUa_VariantArrayType_Array,all_ValueAttribute_of_VariableTypeNodes_VariableNodes[p_Node->ValueIndex].Value.Array.Length);
				break;
			}
		}
	}
	
	OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;
	
	p_Results->StatusCode=uStatus;
	OpcUa_FinishErrorHandling;
}


OpcUa_StatusCode fill_datatype_arraytype_in_my_Variant(OpcUa_DataValue* p_Results,OpcUa_Byte   Datatype, OpcUa_Byte   ArrayType,OpcUa_Int ArrayLegth)
{
	OpcUa_ReturnErrorIfArgumentNull(p_Results);
	p_Results->Value.ArrayType=ArrayType;
	p_Results->Value.Datatype=Datatype;
	if(ArrayLegth>0)
		p_Results->Value.Value.Array.Length=ArrayLegth;
		
	return OpcUa_Good;
}

OpcUa_StatusCode assigne_Timestamp(OpcUa_DataValue* p_Results,OpcUa_TimestampsToReturn a_eTimestampsToReturn)
{
	OpcUa_StatusCode			uStatus     = OpcUa_Bad;
	OpcUa_ReturnErrorIfArgumentNull(p_Results);

	if(a_eTimestampsToReturn == OpcUa_TimestampsToReturn_Source)
	{
		p_Results->SourceTimestamp=OpcUa_DateTime_UtcNow();
		uStatus     = OpcUa_Good;
	}
	if(a_eTimestampsToReturn == OpcUa_TimestampsToReturn_Server)
	{
		p_Results->ServerTimestamp=OpcUa_DateTime_UtcNow();
		uStatus     = OpcUa_Good;
	}
	if(a_eTimestampsToReturn == OpcUa_TimestampsToReturn_Both)
	{
		p_Results->ServerTimestamp=OpcUa_DateTime_UtcNow();
		p_Results->SourceTimestamp=OpcUa_DateTime_UtcNow();
		uStatus     = OpcUa_Good;
	}
	return uStatus;
}
