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


/*********************************************************************************************/
/*****************        OPC UA Client based on the Ansi C Stack         *****************/
/*********************************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif
#include "main.h"

#define OpcUa_String_CopyTo(xSource, xDestination) OpcUa_String_StrnCpy((xDestination), (xSource), OPCUA_STRING_LENDONTCARE)
#define OpcUa_NodeId_CopyTo(xSource, xDestination) memcpy((xDestination), (xSource), sizeof(OpcUa_NodeId));

/*********************************************************************************************/
/***********************                  Globals                     ************************/
/*********************************************************************************************/
OpcUa_Handle                                Client_g_pPortLayerHandle = OpcUa_Null;
OpcUa_StatusCode                            Client_g_uStatus = OpcUa_Good;
OpcUa_PKIProvider                           Client_g_PkiProvider = { OpcUa_Null, OpcUa_Null, OpcUa_Null, OpcUa_Null, OpcUa_Null, OpcUa_Null, OpcUa_Null, OpcUa_Null };
OpcUa_String                                Client_g_pSecurityPolicy = OPCUA_STRING_STATICINITIALIZER;
OpcUa_ByteString                            Client_g_ClientCertificate = OPCUA_BYTESTRING_STATICINITIALIZER;
OpcUa_ByteString                            Client_g_ServerCertificate = OPCUA_BYTESTRING_STATICINITIALIZER;
OpcUa_ByteString                            Client_g_ClientPrivateKey = OPCUA_BYTESTRING_STATICINITIALIZER;
OpcUa_P_OpenSSL_CertificateStore_Config     Client_g_PkiConfig = { OpcUa_NO_PKI, OpcUa_Null, OpcUa_Null, OpcUa_Null, 0, OpcUa_Null };
OpcUa_Endpoint_SecurityPolicyConfiguration* Client_g_pSecurityPolicyConfigurations = OpcUa_Null;
OpcUa_ProxyStubConfiguration                Client_g_pProxyStubConfiguration;


/*********************************************************************************************/
/***********************                  Functions                   ************************/
/*********************************************************************************************/

/*===========================================================================================*/
/** @brief Wait for x to be pressed.                                                         */
/*===========================================================================================*/
OpcUa_Boolean Client_CheckForKeypress()
{
	if (_kbhit())
	{
		if (_getch() == 'x')
		{
			return 1;
		}
	}
	
	return 0;
}

/*===========================================================================================*/
/** @brief Sets a servers and clients certificate and private key.                           */
/*===========================================================================================*/
static OpcUa_StatusCode Client_InitializePKI()
{
	OpcUa_Handle hCertificateStore = OpcUa_Null;

	OpcUa_InitializeStatus(OpcUa_Module_Server, "InitializePKI");
	
#if UACLIENT_USE_SECURE_COMMUNICATION

	Client_g_PkiConfig.PkiType = OpcUa_OpenSSL_PKI;
	Client_g_PkiConfig.CertificateTrustListLocation = UACLIENT_CERTIFICATE_TRUST_LIST_LOCATION;
	Client_g_PkiConfig.CertificateRevocationListLocation = UACLIENT_CERTIFICATE_REVOCATION_LIST_LOCATION;
	Client_g_PkiConfig.CertificateUntrustedListLocation = UACLIENT_CERTIFICATE_REVOCATION_LIST_LOCATION;

	uStatus = OpcUa_PKIProvider_Create(&Client_g_PkiConfig, &Client_g_PkiProvider);
	OpcUa_ReturnErrorIfBad(uStatus);
	
	/* Open Certificate Store */
	uStatus = Client_g_PkiProvider.OpenCertificateStore(
		&Client_g_PkiProvider,
		&hCertificateStore);
	
	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_InitializePKI: Failed to open certificate store! (0x%08X)\n", uStatus);
		OpcUa_GotoError;
	}

	/*** Get client certificate ***/
	uStatus = Client_g_PkiProvider.LoadCertificate(
		&Client_g_PkiProvider,
		UACLIENT_CERTIFICATE_LOCATION,
		hCertificateStore,
		&Client_g_ClientCertificate);
	
	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_InitializePKI: Failed to load client certificate \"%s\"! (0x%08X)\n", UACLIENT_CERTIFICATE_LOCATION, uStatus);
		OpcUa_GotoError;
	}

	{
		OpcUa_Int iValidationCode = 0;

		uStatus = Client_g_PkiProvider.ValidateCertificate(
			&Client_g_PkiProvider,
			&Client_g_ClientCertificate,
			hCertificateStore,
			&iValidationCode);

		if (OpcUa_IsBad(uStatus))
		{
			OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_InitializePKI: Client certificate invalid!\n");
			return uStatus;
		}
	}

	/*** Get private key ***/
	uStatus = Client_g_PkiProvider.LoadPrivateKeyFromFile(
		UACLIENT_PRIVATE_KEY_LOCATION,
		OpcUa_Crypto_Encoding_PKCS12,
		OpcUa_Null,
		&Client_g_ClientPrivateKey);

	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_InitializePKI: Failed to load client private key \"%s\"! (0x%08X)\n", UACLIENT_PRIVATE_KEY_LOCATION, uStatus);
		OpcUa_GotoError;
	}

	/* Close Certificate Store */
	Client_g_PkiProvider.CloseCertificateStore(&Client_g_PkiProvider, &hCertificateStore);

#else

	Client_g_PkiConfig.PkiType = OpcUa_NO_PKI;

	/* allocate and initialize policy configurations */
	Client_g_pSecurityPolicyConfigurations = (OpcUa_Endpoint_SecurityPolicyConfiguration*) OpcUa_Alloc(sizeof(OpcUa_Endpoint_SecurityPolicyConfiguration));
	OpcUa_ReturnErrorIfAllocFailed(Client_g_pSecurityPolicyConfigurations);

	OpcUa_String_Initialize(&Client_g_pSecurityPolicyConfigurations->sSecurityPolicy);
	Client_g_pSecurityPolicyConfigurations->pbsClientCertificate = OpcUa_Null;

	OpcUa_String_AttachReadOnly(&Client_g_pSecurityPolicyConfigurations->sSecurityPolicy, OpcUa_SecurityPolicy_None);
	
	Client_g_pSecurityPolicyConfigurations->uMessageSecurityModes = OPCUA_ENDPOINT_MESSAGESECURITYMODE_NONE;

#endif /* UACLIENT_USE_SECURE_COMMUNICATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

	OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_InitializePKI: Could not initialize client PKI.\n");

	if (hCertificateStore != OpcUa_Null)
	{
		Client_g_PkiProvider.CloseCertificateStore(&Client_g_PkiProvider,
			&hCertificateStore);
	}

	OpcUa_FinishErrorHandling;
}


/*===========================================================================================*/
/** @brief Initializes the demo application.                                                 */
/*===========================================================================================*/
OpcUa_StatusCode Client_Initialize(OpcUa_Void)
{
	OpcUa_StatusCode uStatus = OpcUa_Good;

	Client_g_pProxyStubConfiguration.bProxyStub_Trace_Enabled = OpcUa_True;   //to deactivate Tracer set this variable Opc Ua False.
	Client_g_pProxyStubConfiguration.uProxyStub_Trace_Level = UACLIENT_TRACE_LEVEL;
	Client_g_pProxyStubConfiguration.iSerializer_MaxAlloc = -1;
	Client_g_pProxyStubConfiguration.iSerializer_MaxStringLength = -1;
	Client_g_pProxyStubConfiguration.iSerializer_MaxByteStringLength = -1;
	Client_g_pProxyStubConfiguration.iSerializer_MaxArrayLength = -1;
	Client_g_pProxyStubConfiguration.iSerializer_MaxMessageSize = -1;
	Client_g_pProxyStubConfiguration.iSerializer_MaxRecursionDepth = OPCUA_ENCODER_MAXRECURSIONDEPTH;
	Client_g_pProxyStubConfiguration.bSecureListener_ThreadPool_Enabled = OpcUa_False;
	Client_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MinThreads = -1;
	Client_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxThreads = -1;
	Client_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxJobs = -1;
	Client_g_pProxyStubConfiguration.bSecureListener_ThreadPool_BlockOnAdd = OpcUa_True;
	Client_g_pProxyStubConfiguration.uSecureListener_ThreadPool_Timeout = OPCUA_INFINITE;
	Client_g_pProxyStubConfiguration.iTcpListener_DefaultChunkSize = -1;
	Client_g_pProxyStubConfiguration.iTcpConnection_DefaultChunkSize = -1;
	Client_g_pProxyStubConfiguration.iTcpTransport_MaxMessageLength = -1;
	Client_g_pProxyStubConfiguration.iTcpTransport_MaxChunkCount = -1;
	Client_g_pProxyStubConfiguration.bTcpListener_ClientThreadsEnabled = OpcUa_False;
	Client_g_pProxyStubConfiguration.bTcpStream_ExpectWriteToBlock = OpcUa_True;

	uStatus = OpcUa_P_Initialize(&Client_g_pPortLayerHandle);
	OpcUa_ReturnErrorIfBad(uStatus);

	uStatus = OpcUa_ProxyStub_Initialize(Client_g_pPortLayerHandle, &Client_g_pProxyStubConfiguration);
	OpcUa_ReturnErrorIfBad(uStatus);

	uStatus = OpcUa_String_AttachReadOnly(&Client_g_pSecurityPolicy, UACLIENT_SECURITY_POLICY);
	OpcUa_ReturnErrorIfBad(uStatus);
	
	uStatus = Client_InitializePKI();
	OpcUa_ReturnErrorIfBad(uStatus);

	return uStatus;
}

/*===========================================================================================*/
/** @brief Cleans up all security ressources from the demo application.                      */
/*===========================================================================================*/
OpcUa_Void Client_SecurityClear(OpcUa_Void)
{
	OpcUa_String_Clear(&Client_g_pSecurityPolicy);

	OpcUa_ByteString_Clear(&Client_g_ClientPrivateKey);
	OpcUa_ByteString_Clear(&Client_g_ClientCertificate);
	OpcUa_ByteString_Clear(&Client_g_ServerCertificate);

	OpcUa_PKIProvider_Delete(&Client_g_PkiProvider);
}

/*===========================================================================================*/
/** @brief Cleans up all ressources from the demo application.                               */
/*===========================================================================================*/
OpcUa_Void Client_Cleanup(OpcUa_Void)
{
	/* clear pki and security policies */
	Client_SecurityClear();

	OpcUa_ProxyStub_Clear();

	OpcUa_P_Clean(&Client_g_pPortLayerHandle);
}

/*===========================================================================================*/
/** @brief Receives events from the channel.                                                 */
/*===========================================================================================*/
OpcUa_StatusCode Client_ChannelCallback(
	OpcUa_Channel                   a_hChannel,
	OpcUa_Void*                     a_pvCallbackData,
	OpcUa_Channel_Event             a_eEvent,
	OpcUa_StatusCode                a_uStatus)
{
	OpcUa_InitializeStatus(OpcUa_Module_Client, "ChannelCallback");

	OpcUa_ReferenceParameter(a_hChannel);
	OpcUa_ReferenceParameter(a_pvCallbackData);
	OpcUa_ReferenceParameter(a_uStatus);
	
	switch (a_eEvent)
	{
		case eOpcUa_Channel_Event_Connected:
		{
			OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_ChannelCallback: Channel has been established.\n");
			break;
		}

		case eOpcUa_Channel_Event_Disconnected:
		{
			OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_ChannelCallback: Channel has been lost.\n");
			break;
		}

		default:
		{
			OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_ChannelCallback: Unknown event %d.\n", a_eEvent);
			break;
		}
	}

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

typedef struct _Session
{
	OpcUa_Channel Channel;
	OpcUa_String EndpointUrl;
	OpcUa_String ApplicationUri;
	OpcUa_NodeId SessionId;
	OpcUa_NodeId AuthenticationToken;
	OpcUa_Double RevisedSessionTimeout;
	OpcUa_ByteString ServerNonce;
	OpcUa_ByteString ServerCertificate;
	OpcUa_UserTokenPolicy IdentityTokenPolicy;
	OpcUa_ExtensionObject IdentityToken;
	OpcUa_UInt32 SubscriptionId;
	OpcUa_Int32 SequenceNumbersCount;
	OpcUa_UInt32* SequenceNumbers;
	OpcUa_Int32 MonitoredItemsCount;
	OpcUa_UInt32* MonitoredItems;
	OpcUa_DataValue* MonitoredValues;
}
Session;

void Session_Initialize(Session* pSession)
{
	OpcUa_MemSet(pSession, 0, sizeof(Session));
}

void Session_Clear(Session* pSession)
{
	OpcUa_String_Clear(&pSession->EndpointUrl);
	OpcUa_String_Clear(&pSession->ApplicationUri);
	OpcUa_NodeId_Clear(&pSession->SessionId);
	OpcUa_NodeId_Clear(&pSession->AuthenticationToken);
	OpcUa_ByteString_Clear(&pSession->ServerNonce);
	OpcUa_ByteString_Clear(&pSession->ServerCertificate);
	OpcUa_UserTokenPolicy_Clear(&pSession->IdentityTokenPolicy);
	OpcUa_ExtensionObject_Clear(&pSession->IdentityToken);
	OpcUa_ClearArray(pSession->SequenceNumbers, pSession->SequenceNumbersCount, UInt32);
	OpcUa_Free(pSession->MonitoredItems);
	pSession->MonitoredItems = 0;
	OpcUa_ClearArray(pSession->MonitoredValues, pSession->MonitoredItemsCount, DataValue);

	OpcUa_MemSet(pSession, 0, sizeof(Session));
}

/*===========================================================================================*/
/** @brief Connect to the server.                                                            */
/*===========================================================================================*/
OpcUa_StatusCode Client_Connect(Session* a_pSession, OpcUa_Boolean a_bUseSecurity)
{
	OpcUa_String szSecurityPolicy;
	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_Connect");
	OpcUa_ReturnErrorIfArgumentNull(a_pSession);
	OpcUa_String_Initialize(&szSecurityPolicy);
	
	if (a_bUseSecurity)
	{
		uStatus = OpcUa_String_AttachReadOnly(&szSecurityPolicy, UACLIENT_SECURITY_POLICY);
		OpcUa_ReturnErrorIfBad(uStatus);
	
		uStatus = OpcUa_Channel_Connect(
			a_pSession->Channel,
			UACLIENT_SERVER_URL,
			Client_ChannelCallback,
			OpcUa_Null,
			&Client_g_ClientCertificate,
			&Client_g_ClientPrivateKey,
			&Client_g_ServerCertificate,
			&Client_g_PkiConfig,
			&szSecurityPolicy,
			OPCUA_SECURITYTOKEN_LIFETIME_MAX,
			UACLIENT_SECURITY_MODE,
			UACLIENT_TIMEOUT);
	}
	else
	{
		uStatus = OpcUa_String_AttachReadOnly(&szSecurityPolicy, UACLIENT_SECURITY_POLICY_NONE);
		OpcUa_ReturnErrorIfBad(uStatus);

		uStatus = OpcUa_Channel_Connect(
			a_pSession->Channel,
			UACLIENT_SERVER_URL,
			Client_ChannelCallback,
			OpcUa_Null,
			&Client_g_ClientCertificate,
			&Client_g_ClientPrivateKey,
			&Client_g_ServerCertificate,
			&Client_g_PkiConfig,
			&szSecurityPolicy,
			OPCUA_SECURITYTOKEN_LIFETIME_MAX,
			OpcUa_MessageSecurityMode_None,
			UACLIENT_TIMEOUT);
	}

	/* check for common errors */
	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_Connect: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	OpcUa_String_Clear(&szSecurityPolicy);

	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;

	OpcUa_String_Clear(&szSecurityPolicy);
	OpcUa_Channel_Disconnect(a_pSession->Channel);

	OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Create an ExtensionObject from an EncodeableType                                  */
/*===========================================================================================*/

void* OpcUa_ExtensionObject_CreateFromType(OpcUa_ExtensionObject* a_pExtension, OpcUa_EncodeableType* a_pType)
{
	OpcUa_StatusCode uStatus = OpcUa_EncodeableObject_Create(a_pType, &a_pExtension->Body.EncodeableObject.Object);

	if (OpcUa_IsBad(uStatus))
	{
		return OpcUa_Null;
	}

	if ((a_pExtension == OpcUa_Null) || (a_pType == OpcUa_Null))
	{
		return OpcUa_Null;
	}

	a_pExtension->TypeId.NodeId.IdentifierType = OpcUa_IdentifierType_Numeric;
	a_pExtension->TypeId.NodeId.Identifier.Numeric = a_pType->BinaryEncodingTypeId;
	a_pExtension->Encoding = OpcUa_ExtensionObjectEncoding_EncodeableObject;
	a_pExtension->Body.EncodeableObject.Type = a_pType;

	return a_pExtension->Body.EncodeableObject.Object;
}

/*===========================================================================================*/
/** @brief Select the anonymous user token policy											 */
/*===========================================================================================*/
OpcUa_StatusCode Client_SelectAnonymousUserTokenPolicy(Session* a_pSession, OpcUa_EndpointDescription* a_pEndpoint)
{
	OpcUa_UserTokenPolicy* pAnonynmous = NULL;
		// need to choose a user identity token.
	OpcUa_UserTokenPolicy* pToken = NULL;
	int ii;
	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_SelectAnonymousUserTokenPolicy");
	OpcUa_ReturnErrorIfArgumentNull(a_pSession);
	OpcUa_ReturnErrorIfArgumentNull(a_pEndpoint);
	
	for (ii = 0; ii < a_pEndpoint->NoOfUserIdentityTokens; ii++)
	{
		pToken = &a_pEndpoint->UserIdentityTokens[ii];

		if (pToken->TokenType == OpcUa_UserTokenType_Anonymous)
		{
			// We choose anonymous
			pAnonynmous = &a_pEndpoint->UserIdentityTokens[ii];
			break;
		}

		pToken = NULL;
	}

	if (pToken != NULL)
	{
		OpcUa_AnonymousIdentityToken* pBody = (OpcUa_AnonymousIdentityToken*)OpcUa_ExtensionObject_CreateFromType(&a_pSession->IdentityToken, &OpcUa_AnonymousIdentityToken_EncodeableType);
		OpcUa_GotoErrorIfNull(pBody, OpcUa_BadOutOfMemory);

		uStatus = OpcUa_String_CopyTo(&pAnonynmous->PolicyId, &pBody->PolicyId);
		OpcUa_GotoErrorIfBad(uStatus);

		pToken = pAnonynmous;

		a_pSession->IdentityTokenPolicy.TokenType = pToken->TokenType;
		uStatus = OpcUa_String_CopyTo(&pToken->PolicyId, &a_pSession->IdentityTokenPolicy.PolicyId);
		OpcUa_GotoErrorIfBad(uStatus);

		uStatus = OpcUa_String_CopyTo(&pToken->SecurityPolicyUri, &a_pSession->IdentityTokenPolicy.SecurityPolicyUri);
		OpcUa_GotoErrorIfBad(uStatus);

		uStatus = OpcUa_String_CopyTo(&pToken->IssuedTokenType, &a_pSession->IdentityTokenPolicy.IssuedTokenType);
		OpcUa_GotoErrorIfBad(uStatus);

		uStatus = OpcUa_String_CopyTo(&pToken->IssuerEndpointUrl, &a_pSession->IdentityTokenPolicy.IssuerEndpointUrl);
		OpcUa_GotoErrorIfBad(uStatus);
	}
	else
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_SelectAnonymousUserTokenPolicy: No anonymous user token policy supported by server endpoint.\n");
	}

	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;
	OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Get the endpoints.                                                                */
/*===========================================================================================*/
OpcUa_StatusCode Client_GetEndpoints(Session* a_pSession)
{
	int ii;
	OpcUa_RequestHeader requestHeader;
	OpcUa_ResponseHeader responseHeader;
	OpcUa_String endpointUrl;
	OpcUa_Int32 endpointCount = 0;
	OpcUa_EndpointDescription* pEndpoints = NULL;

	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_GetEndpoints");
	OpcUa_ReturnErrorIfArgumentNull(a_pSession);

	OpcUa_RequestHeader_Initialize(&requestHeader);
	OpcUa_ResponseHeader_Initialize(&responseHeader);

	requestHeader.TimeoutHint = 60000;
	requestHeader.Timestamp = OpcUa_DateTime_UtcNow();

	uStatus = OpcUa_String_AttachToString(UACLIENT_SERVER_URL, OPCUA_STRINGLENZEROTERMINATED, 0, OpcUa_True, OpcUa_True, &endpointUrl);
	OpcUa_GotoErrorIfBad(uStatus);

	uStatus = OpcUa_ClientApi_GetEndpoints(
		a_pSession->Channel,
		&requestHeader,
		&endpointUrl,
		0,
		OpcUa_Null,
		0,
		OpcUa_Null,
		&responseHeader,
		&endpointCount,
		&pEndpoints);

	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_GetEndpoints: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	for (ii = 0; ii < endpointCount; ii++)
	{
		if (pEndpoints[ii].SecurityMode == UACLIENT_SECURITY_MODE && OpcUa_String_StrnCmp(&pEndpoints[ii].SecurityPolicyUri, &Client_g_pSecurityPolicy, OPCUA_STRING_LENDONTCARE, OpcUa_False) == 0)
		{
			OpcUa_String_CopyTo(&pEndpoints[ii].EndpointUrl, &a_pSession->EndpointUrl);
			OpcUa_String_CopyTo(&pEndpoints[ii].Server.ApplicationUri, &a_pSession->ApplicationUri);
			//OpcUa_ByteString_CopyTo(&pEndpoints[ii].ServerCertificate, &Client_g_ServerCertificate);
			
			uStatus = Client_SelectAnonymousUserTokenPolicy(a_pSession, &pEndpoints[ii]);
			OpcUa_GotoErrorIfBad(uStatus);
			break;
		}
	}

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_GetEndpoints: Selecting Endpoint %s.\n", OpcUa_String_GetRawString(&a_pSession->EndpointUrl));

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_String_Clear(&endpointUrl);
	OpcUa_ClearArray(pEndpoints, endpointCount, EndpointDescription);

	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_String_Clear(&endpointUrl);
	OpcUa_ClearArray(pEndpoints, endpointCount, EndpointDescription);

OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Create a new session.                                                             */
/*===========================================================================================*/
OpcUa_StatusCode Client_CreateSession(Session* a_pSession)
{
	OpcUa_RequestHeader requestHeader;
	OpcUa_ResponseHeader responseHeader;
	OpcUa_ApplicationDescription clientDescription;
	OpcUa_ByteString clientNonce;
	OpcUa_ByteString clientCertificate;
	OpcUa_Double requestedSessionTimeout = 0;
	OpcUa_UInt32 maxResponseMessageSize = 0;
	OpcUa_Int32 noOfServerEndpoints = 0;
	OpcUa_EndpointDescription* pServerEndpoints = NULL;
	OpcUa_Int32 noOfServerSoftwareCertificates = 0;
	OpcUa_SignedSoftwareCertificate* pServerSoftwareCertificates = NULL;
	OpcUa_SignatureData serverSignature;
	OpcUa_UInt32 maxRequestMessageSize = 0;
	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_CreateSession");
	OpcUa_ReturnErrorIfArgumentNull(a_pSession);
	
	OpcUa_RequestHeader_Initialize(&requestHeader);
	OpcUa_ResponseHeader_Initialize(&responseHeader);
	OpcUa_ApplicationDescription_Initialize(&clientDescription);
	OpcUa_ByteString_Initialize(&clientNonce);
	OpcUa_ByteString_Initialize(&clientCertificate);
	OpcUa_SignatureData_Initialize(&serverSignature);

	requestHeader.TimeoutHint = 60000;
	requestHeader.Timestamp = OpcUa_DateTime_UtcNow();

	clientDescription.ApplicationType = OpcUa_ApplicationType_Client;

	OpcUa_String_AttachReadOnly(&clientDescription.ApplicationName.Text, UACLIENTNAME);
	OpcUa_String_AttachReadOnly(&clientDescription.ApplicationUri, UACLIENTURI);
	OpcUa_String_AttachReadOnly(&clientDescription.ProductUri, UACLIENT_PRODUCTURI);

	requestedSessionTimeout = 60000; 
	maxResponseMessageSize = 1024*1024;

	uStatus = OpcUa_ClientApi_CreateSession(
		a_pSession->Channel,
		&requestHeader,
		&clientDescription,
		&a_pSession->ApplicationUri,
		&a_pSession->EndpointUrl,
		&clientDescription.ApplicationName.Text,
		&clientNonce,
		&clientCertificate,
		requestedSessionTimeout,
		maxResponseMessageSize,
		&responseHeader,
		&a_pSession->SessionId,
		&a_pSession->AuthenticationToken,
		&a_pSession->RevisedSessionTimeout,
		&a_pSession->ServerNonce,
		&a_pSession->ServerCertificate,
		&noOfServerEndpoints,
		&pServerEndpoints,
		&noOfServerSoftwareCertificates,
		&pServerSoftwareCertificates,
		&serverSignature,
		&maxRequestMessageSize);

	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_CreateSession: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	if (OpcUa_IsBad(responseHeader.ServiceResult))
	{
		uStatus = responseHeader.ServiceResult;
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_CreateSession: ERROR 0x%8X.\n", responseHeader.ServiceResult);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_CreateSession: Selecting Endpoint %s.\n", OpcUa_String_GetRawString(&a_pSession->EndpointUrl));

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_ApplicationDescription_Clear(&clientDescription);
	OpcUa_ByteString_Clear(&clientNonce);
	OpcUa_ByteString_Clear(&clientCertificate);
	OpcUa_SignatureData_Clear(&serverSignature);
	OpcUa_ClearArray(pServerEndpoints, noOfServerEndpoints, EndpointDescription);
	OpcUa_ClearArray(pServerSoftwareCertificates, noOfServerSoftwareCertificates, SignedSoftwareCertificate);

	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_ApplicationDescription_Clear(&clientDescription);
	OpcUa_ByteString_Clear(&clientNonce);
	OpcUa_ByteString_Clear(&clientCertificate);
	OpcUa_SignatureData_Clear(&serverSignature);
	OpcUa_ClearArray(pServerEndpoints, noOfServerEndpoints, EndpointDescription);
	OpcUa_ClearArray(pServerSoftwareCertificates, noOfServerSoftwareCertificates, SignedSoftwareCertificate);

OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Activate a new session.                                                           */
/*===========================================================================================*/
OpcUa_StatusCode Client_ActivateSession(Session* a_pSession)
{
	OpcUa_RequestHeader requestHeader;
	OpcUa_ResponseHeader responseHeader;
	OpcUa_SignatureData clientSignature;
	OpcUa_SignatureData identityTokenSignature;
	OpcUa_Int32 nResultCount = 0;
	OpcUa_StatusCode* pResults = NULL;
	OpcUa_Int32 nDiagnosticInfoCount = 0;
	OpcUa_DiagnosticInfo* pDiagnosticInfos = NULL;
	OpcUa_ByteString revisedNonce;

	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_ActivateSession");

	OpcUa_ReturnErrorIfArgumentNull(a_pSession);

	OpcUa_RequestHeader_Initialize(&requestHeader);
	OpcUa_ResponseHeader_Initialize(&responseHeader);
	OpcUa_SignatureData_Initialize(&clientSignature);
	OpcUa_SignatureData_Initialize(&identityTokenSignature);
	OpcUa_ByteString_Initialize(&revisedNonce);

	uStatus = OpcUa_NodeId_CopyTo(&a_pSession->AuthenticationToken, &requestHeader.AuthenticationToken);
	//OpcUa_GotoErrorIfBad(uStatus);

	requestHeader.TimeoutHint = 60000;
	requestHeader.Timestamp = OpcUa_DateTime_UtcNow();

	uStatus = OpcUa_ClientApi_ActivateSession(
		a_pSession->Channel,
		&requestHeader,
		&clientSignature,
		0,
		NULL,
		0,
		NULL,
		&a_pSession->IdentityToken,
		&identityTokenSignature,
		&responseHeader,
		&revisedNonce,
		&nResultCount,
		&pResults,
		&nDiagnosticInfoCount,
		&pDiagnosticInfos);

	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_ActivateSession: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	if (OpcUa_IsBad(responseHeader.ServiceResult))
	{
		uStatus = responseHeader.ServiceResult;
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_ActivateSession: ERROR 0x%8X.\n", responseHeader.ServiceResult);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	OpcUa_ByteString_Clear(&a_pSession->ServerNonce);
	a_pSession->ServerNonce = revisedNonce;
	OpcUa_ByteString_Initialize(&revisedNonce);

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_CreateSession: Activating Session Endpoint %s.\n", OpcUa_String_GetRawString(&a_pSession->EndpointUrl));

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_SignatureData_Clear(&clientSignature);
	OpcUa_SignatureData_Clear(&identityTokenSignature);
	OpcUa_ClearArray(pResults, nResultCount, StatusCode);
	OpcUa_ClearArray(pDiagnosticInfos, nDiagnosticInfoCount, DiagnosticInfo);

	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_SignatureData_Clear(&clientSignature);
	OpcUa_SignatureData_Clear(&identityTokenSignature);
	OpcUa_ClearArray(pResults, nResultCount, StatusCode);
	OpcUa_ClearArray(pDiagnosticInfos, nDiagnosticInfoCount, DiagnosticInfo);
	OpcUa_ByteString_Clear(&revisedNonce);

	OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Browse nodes.                                                                    */
/*===========================================================================================*/
OpcUa_StatusCode Client_Browse(Session* a_pSession, OpcUa_Int a_iNodeID)
{
	OpcUa_RequestHeader requestHeader;
	OpcUa_ResponseHeader responseHeader;
	OpcUa_Int32 nResultCount = 0;
	OpcUa_BrowseResult* pBrowseResults = OpcUa_Null;
	OpcUa_Int32 nDiagnosticInfoCount = 0;
	OpcUa_DiagnosticInfo* pDiagnosticInfos = NULL;
	OpcUa_ViewDescription view;
	OpcUa_BrowseDescription nodesToBrowse;

	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_Browse");

	OpcUa_ReturnErrorIfArgumentNull(a_pSession);
		
	OpcUa_RequestHeader_Initialize(&requestHeader);
	OpcUa_ResponseHeader_Initialize(&responseHeader);

	uStatus = OpcUa_NodeId_CopyTo(&a_pSession->AuthenticationToken, &requestHeader.AuthenticationToken);
	//OpcUa_GotoErrorIfBad(uStatus);

	requestHeader.TimeoutHint = 60000;
	requestHeader.Timestamp = OpcUa_DateTime_UtcNow();
	
	OpcUa_ViewDescription_Initialize(&view);

	OpcUa_BrowseDescription_Initialize(&nodesToBrowse);
	nodesToBrowse.NodeId.Identifier.Numeric = a_iNodeID;
	nodesToBrowse.NodeId.IdentifierType = OpcUa_IdentifierType_Numeric;
	nodesToBrowse.NodeId.NamespaceIndex = 0;
	nodesToBrowse.BrowseDirection = OpcUa_BrowseDirection_Forward;
	nodesToBrowse.IncludeSubtypes = OpcUa_True;
	nodesToBrowse.NodeClassMask = 0xFF;
	nodesToBrowse.ResultMask = OpcUa_BrowseResultMask_All;
	
	uStatus = OpcUa_ClientApi_Browse(
		a_pSession->Channel,
		&requestHeader,
		&view,
		10,
		1,
		&nodesToBrowse,
		&responseHeader,
		&nResultCount,
		&pBrowseResults,
		&nDiagnosticInfoCount,
		&pDiagnosticInfos);
	
	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_Browse: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	if (OpcUa_IsBad(responseHeader.ServiceResult))
	{
		uStatus = responseHeader.ServiceResult;
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_Browse: ERROR 0x%8X.\n", responseHeader.ServiceResult);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_Browse: SUCCESS\n");

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_ClearArray(pBrowseResults, nResultCount, BrowseResult);
	OpcUa_ClearArray(pDiagnosticInfos, nDiagnosticInfoCount, DiagnosticInfo);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);
	OpcUa_ClearArray(pBrowseResults, nResultCount, BrowseResult);
	OpcUa_ClearArray(pDiagnosticInfos, nDiagnosticInfoCount, DiagnosticInfo);

OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Read node.                                                                    */
/*===========================================================================================*/
OpcUa_StatusCode Client_ReadNode(Session* a_pSession, OpcUa_Int a_iNodeID, OpcUa_DataValue** pValueRead)
{
	OpcUa_RequestHeader requestHeader;
	OpcUa_ResponseHeader responseHeader;
	OpcUa_ReadValueId nodesToRead;
	OpcUa_Int32 nNoOfResults;
	OpcUa_DataValue* pResults = OpcUa_Null;
	OpcUa_Int32 nDiagnosticInfoCount = 0;
	OpcUa_DiagnosticInfo* pDiagnosticInfos = NULL;

	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_ReadNode");

	OpcUa_ReturnErrorIfArgumentNull(a_pSession);
	OpcUa_ReturnErrorIfArgumentNull(pValueRead);
		
	OpcUa_RequestHeader_Initialize(&requestHeader);
	OpcUa_ResponseHeader_Initialize(&responseHeader);

	uStatus = OpcUa_NodeId_CopyTo(&a_pSession->AuthenticationToken, &requestHeader.AuthenticationToken);
	//OpcUa_GotoErrorIfBad(uStatus);

	requestHeader.TimeoutHint = 60000;
	requestHeader.Timestamp = OpcUa_DateTime_UtcNow();

	OpcUa_ReadValueId_Initialize(&nodesToRead);
	nodesToRead.NodeId.Identifier.Numeric = a_iNodeID;
	nodesToRead.NodeId.IdentifierType = OpcUa_IdentifierType_Numeric;
	nodesToRead.NodeId.NamespaceIndex = 0;
	nodesToRead.AttributeId = OpcUa_Attributes_Value;
		
	uStatus = OpcUa_ClientApi_Read(
		a_pSession->Channel,
		&requestHeader,
		36000, // 10 seconds
		OpcUa_TimestampsToReturn_Both,
		1,
		&nodesToRead,
		&responseHeader,
		&nNoOfResults,
		&pResults,
		&nDiagnosticInfoCount,
		&pDiagnosticInfos);

	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_ReadNode: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	if (OpcUa_IsBad(responseHeader.ServiceResult))
	{
		uStatus = responseHeader.ServiceResult;
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_ReadNode: ERROR 0x%8X.\n", responseHeader.ServiceResult);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_ReadNode: SUCCESS\n");

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);

	if ((nNoOfResults > 0) && (pResults[0].StatusCode == OpcUa_Good))
	{
		*pValueRead = pResults;
	}

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);

OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Close session.                                                                    */
/*===========================================================================================*/
OpcUa_StatusCode Client_CloseSession(Session* a_pSession)
{
	OpcUa_RequestHeader requestHeader;
	OpcUa_ResponseHeader responseHeader;

	OpcUa_InitializeStatus(OpcUa_Module_Client, "Client_CloseSession");

	OpcUa_ReturnErrorIfArgumentNull(a_pSession);
		
	OpcUa_RequestHeader_Initialize(&requestHeader);
	OpcUa_ResponseHeader_Initialize(&responseHeader);

	uStatus = OpcUa_NodeId_CopyTo(&a_pSession->AuthenticationToken, &requestHeader.AuthenticationToken);
	//OpcUa_GotoErrorIfBad(uStatus);

	requestHeader.TimeoutHint = 60000;
	requestHeader.Timestamp = OpcUa_DateTime_UtcNow();

	uStatus = OpcUa_ClientApi_CloseSession(
		a_pSession->Channel,
		&requestHeader,
		OpcUa_True,
		&responseHeader);

	if (OpcUa_IsBad(uStatus))
	{
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_CloseSession: ERROR 0x%8X.\n", uStatus);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	if (OpcUa_IsBad(responseHeader.ServiceResult))
	{
		uStatus = responseHeader.ServiceResult;
		OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Client_CloseSession: ERROR 0x%8X.\n", responseHeader.ServiceResult);
		OpcUa_GotoErrorIfBad(uStatus);
	}

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Client_CloseSession: SUCCESS\n");

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

	OpcUa_RequestHeader_Clear(&requestHeader);
	OpcUa_ResponseHeader_Clear(&responseHeader);

OpcUa_FinishErrorHandling;
}

typedef struct EVENT_INSTANCE_TAG
{
	char*					messageHandle;
	int						messageTrackingId;
} EVENT_INSTANCE;

static int messageCounter = 0;


/*===========================================================================================*/
/** @brief Main entry function.                                                              */
/*===========================================================================================*/
int Main_Client()
{
	OpcUa_StatusCode uStatus = OpcUa_Good;
	Session session;

	messageCounter = 0;

	Session_Initialize(&session);

	uStatus = Client_Initialize();
	OpcUa_GotoErrorIfBad(uStatus);

	/* need to fetch the metadata from the server using an insecure channel */
	uStatus = OpcUa_Channel_Create(&session.Channel, OpcUa_Channel_SerializerType_Binary);
	OpcUa_GotoErrorIfBad(uStatus);

	uStatus = Client_Connect(&session, OpcUa_False);
	OpcUa_GotoErrorIfBad(uStatus);

	uStatus = Client_GetEndpoints(&session);
	OpcUa_GotoErrorIfBad(uStatus);

	OpcUa_Channel_Disconnect(session.Channel);
	OpcUa_Channel_Delete(&session.Channel);

	/* now need to connect to server using a secure channel with any access token */
	uStatus = OpcUa_Channel_Create(&session.Channel, OpcUa_Channel_SerializerType_Binary);
	OpcUa_GotoErrorIfBad(uStatus);

	uStatus = Client_Connect(&session, OpcUa_True);
	OpcUa_GotoErrorIfBad(uStatus);

	/* create a normal UA session and activate it */
	uStatus = Client_CreateSession(&session);
	OpcUa_GotoErrorIfBad(uStatus);
	
	uStatus = Client_ActivateSession(&session);
	OpcUa_GotoErrorIfBad(uStatus);

	uStatus = Client_Browse(&session, 0); //apa+++
	OpcUa_GotoErrorIfBad(uStatus);//apa+++

	OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "**** Client Session active, pless 'x' to shutdown! ****\n");

	/* read the current server time and publish it every second until the 'x' key is pressed */
	while (!Client_CheckForKeypress())
	{
		OpcUa_DataValue* value = OpcUa_Null;
		const OpcUa_Int serverTimeNodeID = 2258;
		
		uStatus = Client_ReadNode(&session, serverTimeNodeID, &value);
		OpcUa_GotoErrorIfBad(uStatus);

		if ((value != OpcUa_Null) && (value->Value.Datatype == OpcUaType_DateTime))
		{
			char valueBuffer[50];
			char sourceTimeStampBuffer[50];
			char serverTimeStampBuffer[50];
			char msgText[1024];
			
			OpcUa_DateTime_GetStringFromDateTime(value->Value.Value.DateTime, valueBuffer, 50);
			OpcUa_Trace(OPCUA_TRACE_LEVEL_SYSTEM, "Current server time: %s\n", valueBuffer);
			printf("%s\n", valueBuffer);
			
			OpcUa_DateTime_GetStringFromDateTime(value->SourceTimestamp, sourceTimeStampBuffer, 50);

			
			OpcUa_DateTime_GetStringFromDateTime(value->ServerTimestamp, serverTimeStampBuffer, 50);
		}
		
		Sleep(1000);
	}

	/* close the session, */
	uStatus = Client_CloseSession(&session);
	OpcUa_GotoErrorIfBad(uStatus);

	/* disconnect and delete the channel */
	OpcUa_Channel_Disconnect(session.Channel);
	OpcUa_Channel_Delete(&session.Channel);
	
	Session_Clear(&session);
	Client_Cleanup();

	return 0;

Error:

	OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "**** Error ****\n");

	if (session.Channel != OpcUa_Null)
	{
		OpcUa_Channel_Delete(&session.Channel);
	}

	Session_Clear(&session);
	Client_Cleanup();

	return uStatus;
}

/*********************************************************************************************/
/***********************                End Of File                   ************************/
/*********************************************************************************************/