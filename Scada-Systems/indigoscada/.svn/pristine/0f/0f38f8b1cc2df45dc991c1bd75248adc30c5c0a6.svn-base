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
 
#ifndef _GENERAL_HEADER_
#define _GENERAL_HEADER_


#define SESSION_NOT_ACTIVATED	0x80270000
#define	SESSION_ACTIVATED		0x00000000

#define RESET_SESSION_COUNTER	msec_counter=0;

#define REVISED_SESSIONTIMEOUT  30000    


/*********************************************************************************************/
/***********************                 Prototypes of services       ************************/
/*********************************************************************************************/

OpcUa_StatusCode		check_authentication_token										(const OpcUa_RequestHeader* );
OpcUa_StatusCode		check_securechannelId											( OpcUa_Endpoint ,  OpcUa_Handle );
OpcUa_StatusCode		check_useridentitytoken											(const OpcUa_ExtensionObject* );
OpcUa_StatusCode		speichere_username												(const OpcUa_ExtensionObject* );
OpcUa_Void				username_free													(OpcUa_Void);
OpcUa_StatusCode		vergleiche_username												(const OpcUa_ExtensionObject*);
OpcUa_StatusCode		check_password													(const OpcUa_ExtensionObject* );
OpcUa_StatusCode		response_header_ausfuellen										(OpcUa_ResponseHeader*  ,const OpcUa_RequestHeader*, OpcUa_StatusCode);
OpcUa_StatusCode		my_GetDateTimeDiffInSeconds32								( OpcUa_DateTime  ,OpcUa_DateTime  , OpcUa_UInt32*  );
OpcUa_StatusCode		getEndpoints													(OpcUa_Int32*  ,OpcUa_EndpointDescription** );
OpcUa_StatusCode		initialize_value_attribute_of_variablenodes_variabletypenodes	(OpcUa_Void);
OpcUa_StatusCode		fill_server_variable											(OpcUa_ApplicationDescription* );

 

/** @brief Synchronous service(GetEndpointsService) handler. */
OpcUa_StatusCode myserverGetEndpointsService(
												OpcUa_Endpoint               a_hEndpoint,
												OpcUa_Handle                 a_hContext,
												OpcUa_RequestHeader*         a_pRequestHeader,
												OpcUa_String*                a_pEndpointUrl,
												OpcUa_Int32                  a_nNoOfLocaleIds,
												OpcUa_String*                a_pLocaleIds,
												OpcUa_Int32                  a_nNoOfProfileUris,
												OpcUa_String*                a_pProfileUris,
												OpcUa_ResponseHeader*        a_pResponseHeader,
												OpcUa_Int32*                 a_pNoOfEndpoints,
												OpcUa_EndpointDescription**  a_ppEndpoints);

//@brief CreateSession Prototype
 OpcUa_StatusCode myserver_CreateSession(
											OpcUa_Endpoint                      a_hEndpoint,
											OpcUa_Handle                        a_hContext,
											const OpcUa_RequestHeader*          a_pRequestHeader,
											const OpcUa_ApplicationDescription* a_pClientDescription,
											const OpcUa_String*                 a_pServerUri,
											const OpcUa_String*                 a_pEndpointUrl,
											const OpcUa_String*                 a_pSessionName,
											const OpcUa_ByteString*             a_pClientNonce,
											const OpcUa_ByteString*             a_pClientCertificate,
											OpcUa_Double                        a_nRequestedSessionTimeout,
											OpcUa_UInt32                        a_nMaxResponseMessageSize,
											OpcUa_ResponseHeader*               a_pResponseHeader,
											OpcUa_NodeId*                       a_pSessionId,
											OpcUa_NodeId*                       a_pAuthenticationToken,
											OpcUa_Double*                       a_pRevisedSessionTimeout,
											OpcUa_ByteString*                   a_pServerNonce,
											OpcUa_ByteString*                   a_pServerCertificate,
											OpcUa_Int32*                        a_pNoOfServerEndpoints,
											OpcUa_EndpointDescription**         a_pServerEndpoints,
											OpcUa_Int32*                        a_pNoOfServerSoftwareCertificates,
											OpcUa_SignedSoftwareCertificate**   a_pServerSoftwareCertificates,
											OpcUa_SignatureData*                a_pServerSignature,
											OpcUa_UInt32*                       a_pMaxRequestMessageSize);

 OpcUa_StatusCode my_ActivateSession(
											OpcUa_Endpoint                         a_hEndpoint,
											OpcUa_Handle                           a_hContext,
											const OpcUa_RequestHeader*             a_pRequestHeader,
											const OpcUa_SignatureData*             a_pClientSignature,
											OpcUa_Int32                            a_nNoOfClientSoftwareCertificates,
											const OpcUa_SignedSoftwareCertificate* a_pClientSoftwareCertificates,
											OpcUa_Int32                            a_nNoOfLocaleIds,
											const OpcUa_String*                    a_pLocaleIds,
											const OpcUa_ExtensionObject*           a_pUserIdentityToken,
											const OpcUa_SignatureData*             a_pUserTokenSignature,
											OpcUa_ResponseHeader*                  a_pResponseHeader,
											OpcUa_ByteString*                      a_pServerNonce,
											OpcUa_Int32*                           a_pNoOfResults,
											OpcUa_StatusCode**                     a_pResults,
											OpcUa_Int32*                           a_pNoOfDiagnosticInfos,
											OpcUa_DiagnosticInfo**                 a_pDiagnosticInfos);


 
OpcUa_StatusCode my_CloseSession(
											OpcUa_Endpoint             a_hEndpoint,
											OpcUa_Handle               a_hContext,
											const OpcUa_RequestHeader* a_pRequestHeader,
											OpcUa_Boolean              a_bDeleteSubscriptions,
											OpcUa_ResponseHeader*      a_pResponseHeader);

//@brief FindServers Prototype 
OpcUa_StatusCode my_FindServers(
											OpcUa_Endpoint                 a_hEndpoint,
											OpcUa_Handle                   a_hContext,
											const OpcUa_RequestHeader*     a_pRequestHeader,
											const OpcUa_String*            a_pEndpointUrl,
											OpcUa_Int32                    a_nNoOfLocaleIds,
											const OpcUa_String*            a_pLocaleIds,
											OpcUa_Int32                    a_nNoOfServerUris,
											const OpcUa_String*            a_pServerUris,
											OpcUa_ResponseHeader*          a_pResponseHeader,
											OpcUa_Int32*                   a_pNoOfServers,
											OpcUa_ApplicationDescription** a_pServers);

#endif /*_GENERAL_HEADER_*/
