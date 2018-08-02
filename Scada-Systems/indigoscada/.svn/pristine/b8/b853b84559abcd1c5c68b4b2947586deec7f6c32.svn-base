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
 
#ifndef _browseservice_
#define _browseservice_

#define RESET_SESSION_COUNTER	msec_counter=0;

#define Is_my_node(startNodeId,meinNode) (startNodeId.NamespaceIndex==meinNode.BaseAttribute.NodeId.NamespaceIndex) && (startNodeId.Identifier.Numeric==meinNode.BaseAttribute.NodeId.Identifier.Numeric) && (startNodeId.IdentifierType==meinNode.BaseAttribute.NodeId.IdentifierType)



typedef struct
{
	OpcUa_Int					Cont_Point_Identiefer;
	OpcUa_Int					Aktuelle_Ref;
	OpcUa_BrowseDescription		NodeToBrowse;

}_my_continuationpoint_;


 
 


//@brief Browse Prototype
OpcUa_StatusCode my_Browse(
							OpcUa_Endpoint                 a_hEndpoint,
							OpcUa_Handle                   a_hContext,
							const OpcUa_RequestHeader*     a_pRequestHeader,
							const OpcUa_ViewDescription*   a_pView,
							OpcUa_UInt32                   a_nRequestedMaxReferencesPerNode,
							OpcUa_Int32                    a_nNoOfNodesToBrowse,
							 OpcUa_BrowseDescription* a_pNodesToBrowse,
							OpcUa_ResponseHeader*          a_pResponseHeader,
							OpcUa_Int32*                   a_pNoOfResults,
							OpcUa_BrowseResult**           a_pResults,
							OpcUa_Int32*                   a_pNoOfDiagnosticInfos,
							OpcUa_DiagnosticInfo**         a_pDiagnosticInfos);


 // @brief BrowseNext Prototype.

OpcUa_StatusCode my_BrowseNext(
							OpcUa_Endpoint             a_hEndpoint,
							OpcUa_Handle               a_hContext,
							const OpcUa_RequestHeader* a_pRequestHeader,
							OpcUa_Boolean              a_bReleaseContinuationPoints,
							OpcUa_Int32                a_nNoOfContinuationPoints,
							const OpcUa_ByteString*    a_pContinuationPoints,
							OpcUa_ResponseHeader*      a_pResponseHeader,
							OpcUa_Int32*               a_pNoOfResults,
							OpcUa_BrowseResult**       a_pResults,
							OpcUa_Int32*               a_pNoOfDiagnosticInfos,
							OpcUa_DiagnosticInfo**     a_pDiagnosticInfos);


OpcUa_StatusCode		browse						(OpcUa_BrowseDescription* ,OpcUa_BrowseResult*,OpcUa_Int );  //  besseren namen ausdenken!!!

OpcUa_Void*				search_for_node				(OpcUa_NodeId );

OpcUa_Boolean			ist_unterknoten				( OpcUa_NodeId , OpcUa_NodeId  ,OpcUa_Boolean   );

OpcUa_Boolean			check_Mask					(OpcUa_UInt32 ,OpcUa_UInt32 );

OpcUa_Boolean			check_dir					(OpcUa_BrowseDirection ,_ReferenceNode_* );

OpcUa_Boolean			need_continuationpoint		(OpcUa_BrowseDescription* ,OpcUa_Int);

OpcUa_StatusCode		check_authentication_token	(const OpcUa_RequestHeader* );

OpcUa_StatusCode		response_header_ausfuellen	(OpcUa_ResponseHeader*  ,const OpcUa_RequestHeader*, OpcUa_StatusCode);



#endif /*_browseservice_*/