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
 
#ifndef addressspace_init_
#define addressspace_init_

#include "addressspace.h"

#define ARRAY_SIZE_ArrayAdresse(x)					sizeof(x)/sizeof(x[0]),x

#define ARRAY_SIZE_(x)								sizeof(x)/sizeof(x[0])

#define KEINE_REF_									0,OpcUa_Null

#define std_index									0

#define	MY_NAMESPACE_INDEX							1

#define	MY_NAMESPACE_URI							"http://nanonamespace"

#define STD_NAMESPACE_URI							"http://opcfoundation.org/UA/"

#define NodeId(Identifier,NamespaceIndex)			OpcUa_IdentifierType_Numeric,NamespaceIndex,Identifier





/*alle_Referenzen***************************************************************/
	_ReferenceNode_  ROOT[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_ObjectsFolder,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(86,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_ViewsFolder,std_index)					,STD_NAMESPACE_URI}
	};

	_ReferenceNode_  Objects[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server,std_index)						,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(1,MY_NAMESPACE_INDEX)							,MY_NAMESPACE_URI}

	};
	
	_ReferenceNode_  Types[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_ReferenceTypesFolder,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_DataTypesFolder,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_ObjectTypesFolder,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_VariableTypesFolder,std_index)	,STD_NAMESPACE_URI}

	};
	

	_ReferenceNode_  Views[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)	,STD_NAMESPACE_URI}
		
	};

	_ReferenceNode_  References[]=
	{	
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HierarchicalReferences,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_NonHierarchicalReferences,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)	,OpcUa_True,	NodeId(OpcUaId_ReferenceTypesFolder,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_  HierarchicalReferences[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasChild,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_Organizes,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasEventSource,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_  NonHierarchicalReferences[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasModellingRule,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasTypeDefinition,std_index)	,STD_NAMESPACE_URI}
	};
	_ReferenceNode_  HasChild[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_Aggregates,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasSubtype,std_index)	,STD_NAMESPACE_URI}
	};
	_ReferenceNode_  HasEventSource[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasNotifier,std_index)	,STD_NAMESPACE_URI}
	};
	_ReferenceNode_  Aggregates	[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasProperty,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasComponent,std_index)	,STD_NAMESPACE_URI}
	};
	_ReferenceNode_  HasComponent[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)	,OpcUa_False,	NodeId(OpcUaId_HasOrderedComponent,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_  ReferenceTypes[]=
	{	
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_References,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_True		,NodeId(OpcUaId_TypesFolder,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ Server_State_Variable[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BaseDataVariableType,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ ServerState[]=
	{
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerState_EnumStrings,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ DataTypes[]=
	{
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_BaseDataType,std_index)				,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)				,STD_NAMESPACE_URI},
	};
	
	_ReferenceNode_ Server[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ServerType,std_index)				,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerArray,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_NamespaceArray,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServiceLevel,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_Auditing,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerDiagnostics,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_VendorServerInfo,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerRedundancy,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_True		,NodeId(OpcUaId_ObjectsFolder,std_index)			,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ ObjectTypes[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_True		,NodeId(OpcUaId_TypesFolder,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_BaseObjectType,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ BaseObjectType[]=
	{
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_True		,NodeId(OpcUaId_ObjectTypesFolder,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerType,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ModellingRuleType,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerType_ServerCapabilities,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerType_ServerDiagnostics,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerType_VendorServerInfo,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerType_ServerRedundancy,std_index)	,STD_NAMESPACE_URI}
	};

	/*_ReferenceNode_ ServerType[]=
	{
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerArray,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_NamespaceArray,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServiceLevel,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerType_Auditing,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerDiagnostics,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_VendorServerInfo,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerRedundancy,std_index)	,STD_NAMESPACE_URI}
	};*/

	_ReferenceNode_ ServerArray[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_PropertyType,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_True	,NodeId(OpcUaId_Server,std_index)						,STD_NAMESPACE_URI},
	};

	_ReferenceNode_ ModellingRuleType[]=
	{
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ModellingRule_Mandatory,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ModellingRule_Optional,std_index)	,STD_NAMESPACE_URI}
		
	};

	_ReferenceNode_ Mandatory[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ModellingRuleType,std_index)		,STD_NAMESPACE_URI}
	};
	
	_ReferenceNode_ ServerStatus[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ServerStatusType,std_index)						,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_StartTime,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_CurrentTime,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_State,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_SecondsTillShutdown,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_ShutdownReason,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server,std_index)								,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ VariableTypes[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_Organizes,std_index)			,OpcUa_False	,NodeId(OpcUaId_BaseVariableType,std_index)	,STD_NAMESPACE_URI}
		
	};

	_ReferenceNode_ BaseVariableType[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_BaseDataVariableType,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_PropertyType,std_index)			,STD_NAMESPACE_URI}
	};
	

	_ReferenceNode_ ServerCapabilities[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ServerType_ServerCapabilities,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_ServerProfileArray,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_LocaleIdArray,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_MinSupportedSampleRate,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_MaxBrowseContinuationPoints,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_MaxQueryContinuationPoints,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_MaxHistoryContinuationPoints,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_SoftwareCertificates,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_ModellingRules,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities_AggregateFunctions,std_index)		,STD_NAMESPACE_URI}
	};

	
	_ReferenceNode_ ServerProfileArray[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_PropertyType,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server_ServerCapabilities,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ ModellingRules[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_FolderType,std_index)					,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ ServiceLevel[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_PropertyType,std_index)					,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerCapabilities,std_index)	,STD_NAMESPACE_URI}
	};
	
	_ReferenceNode_ State[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BaseDataVariableType,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server_ServerStatus,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ BuildInfo[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BuildInfoType,std_index)								,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo_BuildDate,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo_BuildNumber,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo_ManufacturerName,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo_ProductName,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo_ProductUri,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerStatus_BuildInfo_SoftwareVersion,std_index),STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server_ServerStatus,std_index)						,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ BuildDate[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BaseDataVariableType,std_index)				,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_BuildInfoType,std_index)					,STD_NAMESPACE_URI}
	};

	
	_ReferenceNode_ ServerDiagnostics[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ServerType_ServerDiagnostics,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerDiagnostics_EnabledFlag,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server,std_index)								,STD_NAMESPACE_URI}
	};


	_ReferenceNode_ VendorServerInfo[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ServerType_VendorServerInfo,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server,std_index)							,STD_NAMESPACE_URI}
	};
	

	_ReferenceNode_ ServerRedundancy[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_ServerType_ServerRedundancy,std_index)				,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server,std_index)									,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_False	,NodeId(OpcUaId_Server_ServerRedundancy_RedundancySupport,std_index),STD_NAMESPACE_URI}

	};

		
	_ReferenceNode_ BaseDataType[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_Enumeration,std_index)	,STD_NAMESPACE_URI}
	};

	
	_ReferenceNode_ Enumeration[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerState,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_True		,NodeId(OpcUaId_Enumeration,std_index)			,STD_NAMESPACE_URI}
	};

	
	_ReferenceNode_ EnumStrings[]=
	{
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_True		,NodeId(OpcUaId_ServerState,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ NANO_SERVER_OBJECT[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BaseObjectType,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(2,MY_NAMESPACE_INDEX)					,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ DATA_VALUE[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BaseVariableType,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ EnabledFlag[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_PropertyType,std_index)				,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server_ServerDiagnostics,std_index)	,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ BaseDataVariableType[]=
	{
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_BuildInfoType,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasSubtype,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ RedundancySupport[]=
	{
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_PropertyType,std_index)				,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasProperty,std_index)			,OpcUa_True		,NodeId(OpcUaId_Server_ServerRedundancy,std_index)	,STD_NAMESPACE_URI}
	};
	
	_ReferenceNode_ State_Property[]=
	{
		{NodeId(OpcUaId_HasModellingRule,std_index)		,OpcUa_False	,NodeId(OpcUaId_ModellingRule_Mandatory,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BaseDataVariableType,std_index)		,STD_NAMESPACE_URI}
	};

	_ReferenceNode_ ServerStatusType[]=
	{
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType_State,std_index)			,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType_StartTime,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType_ShutdownReason,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType_SecondsTillShutdown,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType_CurrentTime,std_index)		,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasComponent,std_index)			,OpcUa_False	,NodeId(OpcUaId_ServerStatusType_BuildInfo,std_index)		,STD_NAMESPACE_URI}
 	};

	
	_ReferenceNode_ BuildInfo_Type[]=
	{
		{NodeId(OpcUaId_HasModellingRule,std_index)		,OpcUa_False	,NodeId(OpcUaId_ModellingRule_Mandatory,std_index)	,STD_NAMESPACE_URI},
		{NodeId(OpcUaId_HasTypeDefinition,std_index)	,OpcUa_False	,NodeId(OpcUaId_BuildInfoType,std_index)			,STD_NAMESPACE_URI}
	};
/***********************************************************************************/


/*alle_ObjectKnoten*************************************************************/
	_ObjectKnoten_ alle_ObjectKnoten[]=
	{ 
		{NodeId(OpcUaId_RootFolder,std_index)			,OpcUa_NodeClass_Object		,OpcUa_BrowseName_RootFolder			,"Root"				,ARRAY_SIZE_ArrayAdresse(ROOT)			,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_ObjectsFolder,std_index)		,OpcUa_NodeClass_Object		,OpcUa_BrowseName_ObjectsFolder			,"Objects"			,ARRAY_SIZE_ArrayAdresse(Objects)		,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_TypesFolder,std_index)			,OpcUa_NodeClass_Object		,OpcUa_BrowseName_TypesFolder			,"Types"			,ARRAY_SIZE_ArrayAdresse(Types)			,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_ReferenceTypesFolder,std_index)	,OpcUa_NodeClass_Object		,OpcUa_BrowseName_ReferenceTypesFolder	,"ReferenceTypes"	,ARRAY_SIZE_ArrayAdresse(ReferenceTypes),OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_DataTypesFolder,std_index)		,OpcUa_NodeClass_Object		,OpcUa_BrowseName_DataTypesFolder		,"DataTypes"		,ARRAY_SIZE_ArrayAdresse(DataTypes)		,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server,std_index)				,OpcUa_NodeClass_Object		,OpcUa_BrowseName_Server				,"Server"			,ARRAY_SIZE_ArrayAdresse(Server)		,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_ObjectTypesFolder,std_index)	,OpcUa_NodeClass_Object		,OpcUa_BrowseName_ObjectTypesFolder		,"ObjectTypes"		,ARRAY_SIZE_ArrayAdresse(ObjectTypes)	,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_ModellingRule_Mandatory,std_index),OpcUa_NodeClass_Object	,OpcUa_BrowseName_ModellingRule_Mandatory,"Mandatory"		,ARRAY_SIZE_ArrayAdresse(Mandatory)		,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_ModellingRule_Optional,std_index),OpcUa_NodeClass_Object	,OpcUa_BrowseName_ModellingRule_Optional,"Optional"			,ARRAY_SIZE_ArrayAdresse(Mandatory)/*Ok*/,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_VariableTypesFolder,std_index)	,OpcUa_NodeClass_Object		,OpcUa_BrowseName_VariableTypesFolder	,"VariableTypes"	,ARRAY_SIZE_ArrayAdresse(VariableTypes)	,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_ServerCapabilities,std_index),OpcUa_NodeClass_Object	,OpcUa_BrowseName_ServerCapabilities	,"ServerCapabilities",ARRAY_SIZE_ArrayAdresse(ServerCapabilities),OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_ServerCapabilities_ModellingRules,std_index),OpcUa_NodeClass_Object	,OpcUa_BrowseName_ModellingRules,"ModellingRules",ARRAY_SIZE_ArrayAdresse(ModellingRules),OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_ServerCapabilities_AggregateFunctions,std_index)	,OpcUa_NodeClass_Object	,OpcUa_BrowseName_AggregateFunctions,"AggregateFunctions",ARRAY_SIZE_ArrayAdresse(ModellingRules)/*ok*/,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_ServerCapabilities,std_index),OpcUa_NodeClass_Object	,OpcUa_BrowseName_ServerCapabilities	,"ServerCapabilities",ARRAY_SIZE_ArrayAdresse(ServerCapabilities),OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_ServerDiagnostics,std_index)	,OpcUa_NodeClass_Object	,OpcUa_BrowseName_ServerDiagnostics		,"ServerDiagnostics",ARRAY_SIZE_ArrayAdresse(ServerDiagnostics)	,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_VendorServerInfo,std_index)	,OpcUa_NodeClass_Object	,OpcUa_BrowseName_VendorServerInfo		,"VendorServerInfo",ARRAY_SIZE_ArrayAdresse(VendorServerInfo)	,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_Server_ServerRedundancy,std_index)	,OpcUa_NodeClass_Object	,OpcUa_BrowseName_ServerRedundancy		,"ServerRedundancy",ARRAY_SIZE_ArrayAdresse(ServerRedundancy)	,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_ViewsFolder,std_index)			,OpcUa_NodeClass_Object		,OpcUa_BrowseName_ViewsFolder			,"Views"			,ARRAY_SIZE_ArrayAdresse(Views)				,OpcUa_EventNotifiers_None},
		{NodeId(OpcUaId_VariableTypesFolder,std_index)	,OpcUa_NodeClass_Object		,OpcUa_BrowseName_VariableTypesFolder	,"VariableTypes"	,ARRAY_SIZE_ArrayAdresse(VariableTypes)		,OpcUa_EventNotifiers_None},
		{NodeId(1,MY_NAMESPACE_INDEX)					,OpcUa_NodeClass_Object		,"NANO_SERVER_OBJECT"					,"NANO_SERVER_OBJECT"	,ARRAY_SIZE_ArrayAdresse(NANO_SERVER_OBJECT),OpcUa_EventNotifiers_None}
		
	};

/*******************************************************************************/



/*alle_ObjectTypeKnoten*************************************************************/

	_ObjectTypeKnoten_  alle_ObjectTypeKnoten[]=
	{
		{NodeId(OpcUaId_BaseObjectType,std_index)				,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_BaseObjectType,			"BaseObjectType"			,ARRAY_SIZE_ArrayAdresse(BaseObjectType)	,OpcUa_True},	
		{NodeId(OpcUaId_FolderType,std_index)					,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_FolderType,				"FolderType"				,KEINE_REF_									,OpcUa_False},
		{NodeId(OpcUaId_ServerType,std_index)					,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_ServerType,				"ServerType"				,KEINE_REF_									,OpcUa_False},
		{NodeId(OpcUaId_ModellingRuleType,std_index)			,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_ModellingRuleType,		"ModellingRuleType"			,ARRAY_SIZE_ArrayAdresse(ModellingRuleType)	,OpcUa_False},
		{NodeId(OpcUaId_ServerType_ServerCapabilities,std_index),OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_ServerCapabilitiesType,	"ServerCapabilitiesType"	,KEINE_REF_									,OpcUa_False},
		{NodeId(OpcUaId_ServerType_ServerDiagnostics,std_index)	,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_ServerDiagnosticsType,	"ServerDiagnosticsType"		,KEINE_REF_									,OpcUa_False},
		{NodeId(OpcUaId_ServerType_VendorServerInfo,std_index)	,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_VendorServerInfoType,		"VendorServerInfoType"		,KEINE_REF_									,OpcUa_False},
		{NodeId(OpcUaId_ServerType_ServerRedundancy,std_index)	,OpcUa_NodeClass_ObjectType		,OpcUa_BrowseName_ServerRedundancyType,		"ServerRedundancyType"		,KEINE_REF_									,OpcUa_False}
		

	};
/***********************************************************************************/


/*alle_DataTypeKnoten_*************************************************************/
	_DataTypeKnoten_ alle_DataTypeKnoten[]=
	{
		{NodeId(OpcUaId_BaseDataType,std_index)			,OpcUa_NodeClass_DataType		,OpcUa_BrowseName_BaseDataType			,"BaseDataType"				,ARRAY_SIZE_ArrayAdresse(BaseDataType)			,OpcUa_True},
		{NodeId(OpcUaId_Enumeration,std_index)			,OpcUa_NodeClass_DataType		,OpcUa_BrowseName_Enumeration			,"Enumeration"				,ARRAY_SIZE_ArrayAdresse(Enumeration)			,OpcUa_True},
		{NodeId(OpcUaId_ServerState,std_index)			,OpcUa_NodeClass_DataType		,OpcUa_BrowseName_ServerState			,"ServerState"				,ARRAY_SIZE_ArrayAdresse(ServerState)			,OpcUa_True}


	};
/***********************************************************************************/


/*alle_ReferencesTypeKnoten*************************************************************/
	_ReferenceTypeKnoten_  alle_ReferencesTypeKnoten[]=
	{
		{NodeId(OpcUaId_References,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_References				,"References"				,ARRAY_SIZE_ArrayAdresse(References)				,OpcUa_True		,OpcUa_True	," "," "},
		{NodeId(OpcUaId_HierarchicalReferences,std_index)	,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HierarchicalReferences	,"HierarchicalReferences"	,ARRAY_SIZE_ArrayAdresse(HierarchicalReferences)	,OpcUa_True		,OpcUa_False," "," "},
		{NodeId(OpcUaId_NonHierarchicalReferences,std_index),OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_NonHierarchicalReferences	,"NonHierarchicalReferences",ARRAY_SIZE_ArrayAdresse(NonHierarchicalReferences)	,OpcUa_True		,OpcUa_True	," "," "},
		{NodeId(OpcUaId_HasChild,std_index)					,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasChild					,"HasChild"					,ARRAY_SIZE_ArrayAdresse(HasChild)					,OpcUa_True		,OpcUa_False," "," "},
		{NodeId(OpcUaId_Organizes,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_Organizes					,"Organizes"				,KEINE_REF_											,OpcUa_False	,OpcUa_False,"OrganizedBy"	,"en"},
		{NodeId(OpcUaId_HasEventSource,std_index)			,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasEventSource			,"HasEventSource"			,ARRAY_SIZE_ArrayAdresse(HasEventSource)			,OpcUa_False	,OpcUa_False," "," "},
		{NodeId(OpcUaId_HasNotifier	,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasNotifier				,"HasNotifier"				,KEINE_REF_											,OpcUa_False	,OpcUa_False," "," "},
		{NodeId(OpcUaId_Aggregates,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_Aggregates				,"Aggregates"				,ARRAY_SIZE_ArrayAdresse(Aggregates)				,OpcUa_True		,OpcUa_False," "," "},
		{NodeId(OpcUaId_HasSubtype,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasSubtype				,"HasSubtype"				,KEINE_REF_											,OpcUa_False	,OpcUa_False,"SubtypeOf"	,"en"},
		{NodeId(OpcUaId_HasProperty	,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasProperty				,"HasProperty"				,KEINE_REF_											,OpcUa_False	,OpcUa_False," "," "},
		{NodeId(OpcUaId_HasComponent,std_index)				,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasComponent				,"HasComponent"				,ARRAY_SIZE_ArrayAdresse(HasComponent)				,OpcUa_False	,OpcUa_False," "," "},
		{NodeId(OpcUaId_HasOrderedComponent	,std_index)		,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasOrderedComponent		,"HasOrderedComponent"		,KEINE_REF_											,OpcUa_False	,OpcUa_False," "," "},
		{NodeId(OpcUaId_HasTypeDefinition,std_index)		,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasTypeDefinition			,"HasTypeDefinition"		,KEINE_REF_											,OpcUa_False	,OpcUa_False," "," "},
		{NodeId(OpcUaId_HasModellingRule,std_index)			,OpcUa_NodeClass_ReferenceType	,OpcUa_BrowseName_HasModellingRule			,"HasModellingRule"			,KEINE_REF_											,OpcUa_False	,OpcUa_False," "," "}
	};
/***************************************************************************************/

/*all_VariableNodes*********************************************************************/
	_VariableKnoten_ all_VariableNodes[]=
	{
		{NodeId(OpcUaId_Server_ServerStatus,std_index)								,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ServerStatus,		"ServerStatus"							,ARRAY_SIZE_ArrayAdresse(ServerStatus)				,-1		,NodeId(OpcUaId_ServerStatusDataType,std_index)		,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_State,std_index)							,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_State,			"State"									,ARRAY_SIZE_ArrayAdresse(State)						,0		,NodeId(OpcUaId_ServerState,std_index)			,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead	 ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_ServerState_EnumStrings,std_index)								,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_EnumStrings		,"EnumStrings"							,ARRAY_SIZE_ArrayAdresse(EnumStrings)				,1		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_OneDimension	,1	,8		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerArray,std_index)									,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ServerArray		,"ServerArray"							,ARRAY_SIZE_ArrayAdresse(ServerArray)				,2		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_OneDimension	,1	,1		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_NamespaceArray,std_index)								,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_NamespaceArray	,"NamespaceArray"						,ARRAY_SIZE_ArrayAdresse(ServerArray)/*ok*/			,3		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_OneDimension	,1	,2		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_ServerProfileArray,std_index)			,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ServerProfileArray,"ServerProfileArray"					,ARRAY_SIZE_ArrayAdresse(ServerProfileArray)		,4		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_OneDimension	,1	,1		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_LocaleIdArray,std_index)				,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_LocaleIdArray		,"LocaleIdArray"						,ARRAY_SIZE_ArrayAdresse(ServerProfileArray)/*ok*/  ,5		,NodeId(OpcUaId_LocaleId,std_index)				,OpcUa_ValueRanks_OneDimension	,1	,1		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_MinSupportedSampleRate,std_index)		,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_MinSupportedSampleRate,"MinSupportedSampleRate"			,ARRAY_SIZE_ArrayAdresse(ServerProfileArray)/*ok*/  ,6		,NodeId(OpcUaId_Duration,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_MaxBrowseContinuationPoints,std_index),OpcUa_NodeClass_Variable	,OpcUa_BrowseName_MaxBrowseContinuationPoints,"MaxBrowseContinuationPoints",ARRAY_SIZE_ArrayAdresse(ServerProfileArray)/*ok*/   ,7		,NodeId(OpcUaId_UInt32,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead  ,OpcUa_AccessLevels_CurrentRead	,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_MaxQueryContinuationPoints,std_index),OpcUa_NodeClass_Variable	,OpcUa_BrowseName_MaxQueryContinuationPoints,"MaxQueryContinuationPoints"	,ARRAY_SIZE_ArrayAdresse(ServerProfileArray)/*ok*/  ,-1		,NodeId(OpcUaId_Int16,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_MaxHistoryContinuationPoints,std_index),OpcUa_NodeClass_Variable	,OpcUa_BrowseName_MaxHistoryContinuationPoints,"MaxHistoryContinuationPoints",ARRAY_SIZE_ArrayAdresse(ServerProfileArray)/*ok*/ ,-1		,NodeId(OpcUaId_Int16,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerCapabilities_SoftwareCertificates,std_index)		,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_SoftwareCertificates,"SoftwareCertificates"				,ARRAY_SIZE_ArrayAdresse(ServerProfileArray)/*ok*/ ,-1		,NodeId(OpcUaId_SignedSoftwareCertificate,std_index)	,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServiceLevel,std_index)									,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ServiceLevel		,"ServiceLevel"							,ARRAY_SIZE_ArrayAdresse(ServerArray)/*ok*/			,-1		,NodeId(OpcUaId_Byte,std_index)					,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_StartTime,std_index)						,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_StartTime			,"StartTime"							,ARRAY_SIZE_ArrayAdresse(State)/*ok*/				,-1		,NodeId(OpcUaId_Byte,std_index)					,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_ShutdownReason,std_index)					,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ShutdownReason	,"ShutdownReason"						,ARRAY_SIZE_ArrayAdresse(State)/*ok*/				,-1		,NodeId(OpcUaId_Byte,std_index)					,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_SecondsTillShutdown,std_index)				,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_SecondsTillShutdown,"SecondsTillShutdown"					,ARRAY_SIZE_ArrayAdresse(State)/*ok*/				,-1		,NodeId(OpcUaId_Byte,std_index)					,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_CurrentTime,std_index)						,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_CurrentTime		,"CurrentTime"							,ARRAY_SIZE_ArrayAdresse(State)/*ok*/				,11		,NodeId(OpcUaId_UtcTime,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead	 ,OpcUa_AccessLevels_CurrentRead		,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo,std_index)						,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_BuildInfo			,"BuildInfo"							,ARRAY_SIZE_ArrayAdresse(BuildInfo)					,-1		,NodeId(OpcUaId_Byte,std_index)					,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo_BuildDate,std_index)				,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_BuildDate			,"BuildDate"							,ARRAY_SIZE_ArrayAdresse(BuildDate)					,-1		,NodeId(OpcUaId_UtcTime,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo_BuildNumber,std_index)			,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_BuildNumber		,"BuildNumber"							,ARRAY_SIZE_ArrayAdresse(BuildDate)/*ok*/			,-1		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo_ManufacturerName,std_index)		,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ManufacturerName	,"ManufacturerName"						,ARRAY_SIZE_ArrayAdresse(BuildDate)/*ok*/			,-1		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo_ProductName,std_index)			,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ProductName		,"ProductName"							,ARRAY_SIZE_ArrayAdresse(BuildDate)/*ok*/			,-1		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo_ProductUri,std_index)				,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ProductUri		,"ProductUri"							,ARRAY_SIZE_ArrayAdresse(BuildDate)/*ok*/			,-1		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerStatus_BuildInfo_SoftwareVersion,std_index)		,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_SoftwareVersion	,"SoftwareVersion"						,ARRAY_SIZE_ArrayAdresse(BuildDate)/*ok*/			,-1		,NodeId(OpcUaId_String,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		 ,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_Server_Auditing,std_index)										,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_Auditing			,"Auditing"								,ARRAY_SIZE_ArrayAdresse(ServerArray)/*ok*/			,9		,NodeId(OpcUaId_Boolean,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead	,OpcUa_AccessLevels_CurrentRead		,OpcUa_False},
		{NodeId(2,MY_NAMESPACE_INDEX)													,OpcUa_NodeClass_Variable	,"DATA_VALUE"						,"DATA_VALUE"							,ARRAY_SIZE_ArrayAdresse(DATA_VALUE)				,8		,NodeId(OpcUaId_Double,std_index)				,OpcUa_ValueRanks_OneDimension	,1	,3		,OpcUa_AccessLevels_CurrentRead	,OpcUa_AccessLevels_CurrentRead		,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerDiagnostics_EnabledFlag,std_index)					,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_EnabledFlag		,"EnabledFlag"							,ARRAY_SIZE_ArrayAdresse(EnabledFlag)				,9		,NodeId(OpcUaId_Boolean,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead	,OpcUa_AccessLevels_CurrentRead		,OpcUa_False},
		{NodeId(OpcUaId_Server_ServerRedundancy_RedundancySupport,std_index)			,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_RedundancySupport	,"RedundancySupport"					,ARRAY_SIZE_ArrayAdresse(RedundancySupport)			,10		,NodeId(OpcUaId_RedundancySupport,std_index)	,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_CurrentRead	,OpcUa_AccessLevels_CurrentRead		,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType_State,std_index)								,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_State				,"State"								,ARRAY_SIZE_ArrayAdresse(State_Property)			,-1		,NodeId(OpcUaId_ServerState,std_index)			,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType_StartTime,std_index)							,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_StartTime			,"StartTime"							,ARRAY_SIZE_ArrayAdresse(State_Property)/*ok*/		,-1		,NodeId(OpcUaId_UtcTime,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType_ShutdownReason,std_index)						,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_ShutdownReason	,"ShutdownReason"						,ARRAY_SIZE_ArrayAdresse(State_Property)/*ok*/		,-1		,NodeId(OpcUaId_LocalizedText,std_index)		,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType_SecondsTillShutdown,std_index)					,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_SecondsTillShutdown,"SecondsTillShutdown"					,ARRAY_SIZE_ArrayAdresse(State_Property)/*ok*/		,-1		,NodeId(OpcUaId_UInt32,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType_CurrentTime,std_index)							,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_CurrentTime		,"CurrentTime"							,ARRAY_SIZE_ArrayAdresse(State_Property)/*ok*/		,-1		,NodeId(OpcUaId_UtcTime,std_index)				,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		,OpcUa_AccessLevels_None			,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType_BuildInfo,std_index)							,OpcUa_NodeClass_Variable	,OpcUa_BrowseName_BuildInfo			,"BuildInfo"							,ARRAY_SIZE_ArrayAdresse(BuildInfo_Type)			,-1		,NodeId(OpcUaId_BuildInfo,std_index)			,OpcUa_ValueRanks_Scalar		,0	,0		,OpcUa_AccessLevels_None		,OpcUa_AccessLevels_None			,OpcUa_False}
		 
	};
/***************************************************************************************/


/*all_VariableTypeNodes*****************************************************************/
	_VariableTypeKnoten_ all_VariableTypeNodes[]=
	{
		{NodeId(OpcUaId_BaseVariableType,std_index)		,OpcUa_NodeClass_VariableType	,OpcUa_BrowseName_BaseVariableType,	"BaseVariableType"		,ARRAY_SIZE_ArrayAdresse(BaseVariableType)		,-1		,NodeId(OpcUaId_BaseDataType,std_index)	,OpcUa_ValueRanks_Any				,0	 ,0		,OpcUa_True},
		{NodeId(OpcUaId_PropertyType,std_index)			,OpcUa_NodeClass_VariableType	,OpcUa_BrowseName_PropertyType	,	"PropertyType"			,KEINE_REF_										,-1		,NodeId(OpcUaId_BaseDataType,std_index)	,OpcUa_ValueRanks_Scalar			,0	 ,0		,OpcUa_False},
		{NodeId(OpcUaId_BaseDataVariableType,std_index)	,OpcUa_NodeClass_VariableType	,OpcUa_BrowseName_BaseDataVariableType,"BaseDataVariableType",ARRAY_SIZE_ArrayAdresse(BaseDataVariableType)	,-1		,NodeId(OpcUaId_BaseDataType,std_index)	,OpcUa_ValueRanks_Scalar			,0	 ,0		,OpcUa_False},
		{NodeId(OpcUaId_BuildInfoType,std_index)		,OpcUa_NodeClass_VariableType	,OpcUa_BrowseName_BuildInfoType		,"BuildInfoType"		,KEINE_REF_										,-1		,NodeId(OpcUaId_BuildInfo,std_index)	,OpcUa_ValueRanks_Scalar			,0	 ,0		,OpcUa_False},
		{NodeId(OpcUaId_ServerStatusType,std_index)		,OpcUa_NodeClass_VariableType	,OpcUa_BrowseName_ServerStatusType	,"ServerStatusType"		,ARRAY_SIZE_ArrayAdresse(ServerStatusType)		,-1		,NodeId(OpcUaId_BuildInfo,std_index)	,OpcUa_ValueRanks_Scalar			,0	 ,0		,OpcUa_False}
		
	};
/***************************************************************************************/


#endif  /*addressspace_init_*/
